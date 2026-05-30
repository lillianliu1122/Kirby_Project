#include "hothead.h"

HotHead::HotHead(int startX, int startY)
    : Enemy(startX, startY, "HotHead", "Fire", true),
      state(0), actionTimer(0), isFacingRight(false), isBreathingFire(false) {
    vx = -1;
}

void HotHead::updateBehavior(int kirbyX, int kirbyY) {
    if (isDead) return;
    actionTimer++;
    isFacingRight = (kirbyX > x);

    if (fireBall.active) {
        fireBall.fx += fireBall.fvx;
        if (std::abs(fireBall.fx - x) > 600) fireBall.active = false;
    }

    int distanceX = std::abs(kirbyX - x);
    int distanceY = std::abs(kirbyY - y);

    // 7.4 攻擊模式切換：太近就噴火，500px內水平前方就吐火球 [cite: 14, 44]
    if (distanceX < 120 && distanceY < 50) {
        state = 2; // Flame Breath 近距離噴火
    } else if (distanceX < 500 && distanceY < 50 && ((isFacingRight && kirbyX > x) || (!isFacingRight && kirbyX < x))) {
        state = 1; // Fire Ball 遠距離火球
    } else {
        state = 0; // 平時巡邏
    }

    if (state == 0) {
        isBreathingFire = false;
        x += vx;
        if (std::abs(x - startX) > 100) vx = -vx;
    }
    else if (state == 1) {
        isBreathingFire = false;
        if (actionTimer % 90 == 0 && !fireBall.active) {
            fireBall.active = true;
            fireBall.fx = isFacingRight ? (x + width) : (x - 20);
            fireBall.fy = y + height / 3;
            fireBall.fvx = isFacingRight ? 6 : -6;
        }
    }
    else if (state == 2) {
        isBreathingFire = true; // 7.4 停止移動並朝前方噴火 [cite: 14, 44]
        if (actionTimer % 120 == 0) state = 0; // 結束後恢復原本移動
    }
}

void HotHead::draw(QPainter &painter) {
    if (isDead) return;
    QPixmap img(":/Image/Hot Head/Hot_head_stop_L.png");
    painter.drawPixmap(x, y, width, height, img);

    if (state == 2) { // 畫近距離噴火範圍
        painter.setBrush(QBrush(QColor(255, 69, 0, 180)));
        if (isFacingRight) painter.drawRect(x + width, y, 80, height);
        else painter.drawRect(x - 80, y, 80, height);
    }
    if (fireBall.active) { // 畫飛行火球
        painter.setBrush(QBrush(QColor(255, 165, 0)));
        painter.drawEllipse(fireBall.fx, fireBall.fy, 24, 24);
    }
}
