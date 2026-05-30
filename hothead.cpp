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
    isFacingRight = (kirbyX > x);

    int distanceX = std::abs(kirbyX - x);
    int distanceY = std::abs(kirbyY - y);

    if (distanceX < 120 && distanceY < 500) state = FLAME_BREATH;
    else if (distanceX < 500 && distanceY < 50 && ((isFacingRight && kirbyX > x) || (!isFacingRight && kirbyX < x))) state = FIRE_BALL;
    else state = PATROL;

    switch(state) {
        case PATROL: updatePatrol(); break;
        case FIRE_BALL: updateFireBall(kirbyX); break;
        case FLAME_BREATH: updateFlameBreath(); break;
    }
}

void HotHead::updateFireBall(int kirbyX) {
    Q_UNUSED(kirbyX);
    if (!currentPlatforms) return;
    vx = 0;
    isFacingRight = (kirbyX > x);

    if (!fireBall.active) {
        fireBallCooldown++;
        if (fireBallCooldown >= 90) {
            fireBall.active = true;
            fireBall.hitKirby = false;
            fireBall.fx = isFacingRight ? (x + width) : (x - 20);
            fireBall.fy = y;
            fireBall.fvx = isFacingRight ? 6 : -6;
            fireBallCooldown = 0;
        }
    } else {
        fireBall.fx += fireBall.fvx;
    }
}

void HotHead::updatePatrol() {
    x += vx;
    if (std::abs(x - startX) > moveRange) vx = -vx;
}

void HotHead::updateFlameBreath() {
    if (actionTimer % 120 == 0) state = PATROL;
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
    if (fireBall.active) {
            qDebug() << "火球狀態為 Active，目前位置: (" << fireBall.fx << ", " << fireBall.fy << ")";
            painter.drawPixmap(fireBall.fx, fireBall.fy, 100, 100, QPixmap(":/Image/Hot Head/Hot_head_fire(1).png"));
        }
}
