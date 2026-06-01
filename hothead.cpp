#include "hothead.h"
#include <QDebug>

HotHead::HotHead(int startX, int startY, int range, const QVector<Platform>* platforms)
    : Enemy(startX, startY, "HotHead", "Fire", true),
      state(PATROL), moveRange(range), startX(startX), actionTimer(0),
      fireBallCooldown(0), isFacingRight(false), isBreathingFire(false), currentPlatforms(platforms) {
    vx = -1;
}

void HotHead::updateBehavior(int kirbyX, int kirbyY) {
    if (isDead) return;
    actionTimer++;

    // ✅ 火球飛行中不更新 isFacingRight，避免干擾火球方向
    if (!fireBall.active) {
        isFacingRight = (kirbyX > x);
    }

    int distanceX = std::abs(kirbyX - x);
    int distanceY = std::abs(kirbyY - y);

    if (distanceX < 120 && distanceY < 50) {
        state = FLAME_BREATH;
        vx = 0;
    }
    else if (distanceX < 500 && distanceY < 50 && ((isFacingRight && kirbyX > x) || (!isFacingRight && kirbyX < x))) {
        state = FIRE_BALL;
    }
    else {
        state = PATROL;
        if (vx == 0) vx = (isFacingRight ? 1 : -1);
    }

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
    if (isDead) {
        fireBall.active = false;
        fireBall.hitKirby = false;
        return;
    }
    if (!fireBall.active) {
        isFacingRight = (kirbyX > x);

        int distanceX = std::abs(kirbyX - x);
        int distanceY = std::abs(kirbyY - y);

        bool inFront = (isFacingRight && kirbyX > x) || (!isFacingRight && kirbyX < x);
        if (inFront && distanceX <= 500 && distanceY < 50) {
            fireBallCooldown++;
            if (fireBallCooldown >= 30) {
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
    else {
        fireBall.fx += fireBall.fvx;
        if (fireBall.fx < 0 || fireBall.fx > 9000) {
            fireBall.active = false;
            return;
        }
        QRect fireBallRect(static_cast<int>(fireBall.fx) + 5,
                           static_cast<int>(fireBall.fy) + 15,
                           70, 20);


        for (const Platform& platform : platforms) {
            QRectF pr = platform.getRect();

            // ✅ 只檢查寬度 > 高度的平台（水平地板），忽略牆壁
            if (pr.width() <= pr.height()) continue;


            if (fireBallRect.intersects(pr.toRect())) {
                fireBall.active = false;
                return;
            }
        }
        // 斜坡碰撞
        for (const SlopePlatform& slope : slopes) {
            float ballFrontX = (fireBall.fvx > 0) ? (fireBall.fx + 75) : fireBall.fx + 5;

            // ✅ 只要前端 X 進入斜坡範圍就消失，不管 Y
            if (slope.containsX(ballFrontX)) {
                fireBall.active = false;
                return;
            }
        }

        if (fireBallRect.intersects(kirbyRect)) {
            fireBall.hitKirby = true;
            fireBall.active = false;
            return;
        }
    }
}

void HotHead::updatePatrol() {
    x += vx;
    if (std::abs(x - startX) > moveRange) vx = -vx;
}

QRect HotHead::getFlameRect() const {
    if (state != FLAME_BREATH) return QRect(); // 不在噴火狀態回空的
    // 火焰在身體旁邊，寬80高跟身體一樣
    int flameX = isFacingRight ? (x + width) : (x - 80);
    return QRect(flameX, y, 80, height);
}

void HotHead::updateFlameBreath() {
    // ✅ 噴火持續 120 幀後回到巡邏
    if (actionTimer % 120 == 0) {
        state = PATROL;
        vx = isFacingRight ? 1 : -1; // ✅ 恢復移動方向
    }
}

void HotHead::draw(QPainter &painter) {
    if (isDead) return;

    QString imgPath;

    // 判斷狀態決定圖片
    if (state == FIRE_BALL || state == FLAME_BREATH) {
        imgPath = isFacingRight ? ":/Image/Hot Head/Hot_head_attack_R.png"
                                : ":/Image/Hot Head/Hot_head_attack_L.png";
    } else {
        // 巡邏狀態：移動時用 run，靜止時用 stop
        imgPath = (vx > 0) ? ":/Image/Hot Head/Hot_head_run_R.png"
                               : ":/Image/Hot Head/Hot_head_run_L.png";
    }

    painter.drawPixmap(x, y, width, height, QPixmap(imgPath));

    // 繪製攻擊特效
    if (state == FLAME_BREATH) {
        QString breathImg = isFacingRight ? ":/Image/Hot Head/Hot_head_fire(2)_R.png"
                                          : ":/Image/Hot Head/Hot_head_fire(2)_L.png";
        painter.drawPixmap(isFacingRight ? x + width : x - 80, y, 80, height, QPixmap(breathImg));
    }

    if (fireBall.active) {
        painter.drawPixmap(fireBall.fx, fireBall.fy, 100, 100, QPixmap(":/Image/Hot Head/Hot_head_fire(1).png"));
    }
}
