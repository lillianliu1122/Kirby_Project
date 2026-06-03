#include "hothead.h"
#include <QDebug>

HotHead::HotHead(int startX, int startY, int range, const QVector<Platform>* platforms)
    : Enemy(startX, startY, "HotHead", "Fire", true),
      state(PATROL), moveRange(range), startX(startX), actionTimer(0),
      fireBallCooldown(0), isFacingRight(false), isBreathingFire(false), currentPlatforms(platforms) {
    vx = -1; // 預設向左移動
}

void HotHead::updateBehavior(int kirbyX, int kirbyY) {
    if (isDead) return;
    actionTimer++;

    // 火球飛行中維持當前朝向
    if (!fireBall.active) {
        isFacingRight = (kirbyX > x);
    }

    // 根據和 kirby 距離決定行為狀態
    int distanceX = std::abs(kirbyX - x);
    int distanceY = std::abs(kirbyY - y);

    // 近距離：噴火
    if (distanceX < 168 && distanceY < 50) {
        state = FLAME_BREATH;
        vx = 0;
    }
    // 中距離：發射火球
    else if (distanceX < 500 && distanceY < 50 && ((isFacingRight && kirbyX > x) || (!isFacingRight && kirbyX < x))) {
        state = FIRE_BALL;
    }
    // 遠距離：移動
    else {
        state = PATROL;
        if (vx == 0) vx = (isFacingRight ? 1 : -1);
    }

    // 根據當前狀態執行對應動作
    switch(state) {
        case PATROL:
            updatePatrol();
            break;
        case FIRE_BALL:
            break;
        case FLAME_BREATH:
            updateFlameBreath();
            break;
    }
}

void HotHead::updateFireBall(int kirbyX, int kirbyY, const QRect& kirbyRect, const QVector<Platform>& platforms, const QVector<SlopePlatform>& slopes) {
    // 死亡時清除火球
    if (isDead) {
        fireBall.active = false;
        fireBall.hitKirby = false;
        return;
    }

    // 噴火焰時不發射火球
    if (state == FLAME_BREATH) {
        fireBall.active = false;
        fireBallCooldown = 0;
        return;
    }

    // 火球不存在時，判斷是否發射
    if (!fireBall.active) {
        isFacingRight = (kirbyX > x);
        int distanceX = std::abs(kirbyX - x);
        int distanceY = std::abs(kirbyY - y);
        bool inFront = (isFacingRight && kirbyX > x) || (!isFacingRight && kirbyX < x);

        if (inFront && distanceX <= 500 && distanceY < 50) {
            fireBallCooldown++;
            if (fireBallCooldown >= 30) { // 累積 30 幀後發射
                fireBall.active = true;
                fireBall.fx = isFacingRight ? (x + width) : (x - 20);
                fireBall.fy = y + height / 2 -50;
                fireBall.fvx = isFacingRight ? 5 : -5;
                fireBallCooldown = 0;
            }
        } else {
            fireBallCooldown = 0;
        }
    }
    // 火球存在時，更新位置並判斷碰撞
    else {
        fireBall.fx += fireBall.fvx;

        // 超出地圖範圍消失
        if (fireBall.fx < 0 || fireBall.fx > 9000) {
            fireBall.active = false;
            return;
        }

        QRect fireBallRect(static_cast<int>(fireBall.fx) + 5,
                           static_cast<int>(fireBall.fy) + 15,
                           70, 20);

        // 碰到水平平台消失（忽略牆壁）
        for (const Platform& platform : platforms) {
            QRectF pr = platform.getRect();
            if (pr.width() <= pr.height()) continue; // 跳過垂直牆壁
            if (fireBallRect.intersects(pr.toRect())) {
                fireBall.active = false;
                return;
            }
        }

        // 碰到斜坡消失
        for (const SlopePlatform& slope : slopes) {
            float ballFrontX = (fireBall.fvx > 0) ? (fireBall.fx + 75) : fireBall.fx + 5;
            if (slope.containsX(ballFrontX)) {
                fireBall.active = false;
                return;
            }
        }

        // 碰到 kirby 造成傷害
        if (fireBallRect.intersects(kirbyRect)) {
            fireBall.hitKirby = true;
            fireBall.active = false;
            return;
        }
    }
}

// 移動
void HotHead::updatePatrol() {
    x += vx;
    if (std::abs(x - startX) > moveRange) vx = -vx; // 超出範圍就反向
}

// 取得火焰碰撞範圍
QRect HotHead::getFlameRect() const {
    if (state != FLAME_BREATH) return QRect(); // 非噴火狀態回傳空矩形
    int flameW = 168; // 火焰寬度
    int flameX = isFacingRight ? (x + width) : (x - flameW);
    return QRect(flameX, y, flameW, height);
}

// 噴火狀態更新
void HotHead::updateFlameBreath() {
    // 噴火持續 120 幀後回到移動
    if (actionTimer % 120 == 0) {
        state = PATROL;
        vx = isFacingRight ? 1 : -1;
    }
}

void HotHead::draw(QPainter &painter) {
    if (isDead) return;

    // 根據狀態選擇圖片
    QString imgPath;
    if (state == FIRE_BALL || state == FLAME_BREATH) {
        imgPath = isFacingRight ? ":/Image/Hot Head/Hot_head_attack_R.png"
                                : ":/Image/Hot Head/Hot_head_attack_L.png";
    } else {
        imgPath = (vx > 0) ? ":/Image/Hot Head/Hot_head_run_R.png"
                               : ":/Image/Hot Head/Hot_head_run_L.png";
    }
    painter.drawPixmap(x, y, width, height, QPixmap(imgPath));

    // 畫火焰
    if (state == FLAME_BREATH) {
        QString breathImg = isFacingRight ? ":/Image/Hot Head/Hot_head_fire(2)_R.png"
                                          : ":/Image/Hot Head/Hot_head_fire(2)_L.png";
        painter.drawPixmap(isFacingRight ? x + width : x - 168, y, 168, height, QPixmap(breathImg));
    }

    // 畫火球
    if (fireBall.active) {
        painter.drawPixmap(fireBall.fx, fireBall.fy, 100, 100, QPixmap(":/Image/Hot Head/Hot_head_fire(1).png"));
    }
}
