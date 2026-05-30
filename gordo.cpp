#include "gordo.h"

Gordo::Gordo(int startX, int startY, int range)
    : Enemy(startX, startY, "Gordo", "None", false), moveRange(range), moveDirection(1) {
    vy = 3;
}

void Gordo::updateBehavior(int kirbyX, int kirbyY) {
    Q_UNUSED(kirbyX);
    Q_UNUSED(kirbyY);
    y += vy * moveDirection; // 7.4 沿預先設定的固定軌跡上下移動 [cite: 13, 43]
    if (y < startY - moveRange || y > startY + moveRange) {
        moveDirection = -moveDirection;
    }
}

void Gordo::draw(QPainter &painter) {
    QPixmap img(":/Image/Gordo/Gordo(0).png");
    painter.drawPixmap(x, y, width, height, img);
}
