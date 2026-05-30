#include "gordo.h"

Gordo::Gordo(int startX, int startY, int range)
    : Enemy(startX, startY, "Gordo", "None", false), moveRange(range), moveDirection(1), startY(startY) {
    vy = 3;
    frameIndex = 0;
    animationTimer = 0;
    animationFrames[0].load(":/Image/Gordo/Gordo(0).png");
    animationFrames[1].load(":/Image/Gordo/Gordo(1).png");
}

void Gordo::updateBehavior(int kirbyX, int kirbyY) {
    Q_UNUSED(kirbyX);
    Q_UNUSED(kirbyY);
    y += vy * moveDirection; // 7.4 沿預先設定的固定軌跡上下移動 [cite: 13, 43]
    if (y < startY - moveRange || y > startY + moveRange) {
        moveDirection = -moveDirection;
    }

    // 動畫切換
    animationTimer++;
    if (animationTimer >= 10) { // 數字越大，切換速度越慢
        frameIndex = 1 - frameIndex; // 在 0 和 1 之間切換
        animationTimer = 0;
    }
}

void Gordo::draw(QPainter &painter) {
    painter.drawPixmap(x, y, width, height, animationFrames[frameIndex]);
}
