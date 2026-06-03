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

    // 沿 Y 軸範圍移動，超出範圍則反轉方向
    y += vy * moveDirection;
    if (y < startY - moveRange || y > startY + moveRange) {
        moveDirection = -moveDirection;
    }

    // 動畫切換
    animationTimer++;
    if (animationTimer >= 10) {
        frameIndex = 1 - frameIndex; // 在 0 和 1 之間切換
        animationTimer = 0;
    }
}

void Gordo::draw(QPainter &painter) {
    painter.drawPixmap(x, y, width, height, animationFrames[frameIndex]);
}
