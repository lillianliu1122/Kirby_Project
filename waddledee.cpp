#include "waddledee.h"

WaddleDee::WaddleDee(int startX, int startY, int range)
    : Enemy(startX, startY, "WaddleDee", "None", true), walkRange(range) {
    vx = -1; // 預設向左移動
}

void WaddleDee::updateBehavior(int kirbyX, int kirbyY) {
    Q_UNUSED(kirbyX); Q_UNUSED(kirbyY);
    if (isDead) return;
    x += vx;
    y += vy;

    // 範圍內來回移動 (邊界反轉)
    if (x > startX + walkRange) {
        vx = -std::abs(vx);
    } else if (x < startX - walkRange) {
        vx = std::abs(vx);
    }

    // 2. 動畫計時 (每 8 幀切換一次圖片)
    frameTimer++;
    if (frameTimer >= 8) {
        frameTimer = 0;

        if (vx < 0) {
            // 如果向左，index 在 0, 1, 2 循環
            frameIndex = (frameIndex + 1) % 3;
        } else {
            // 如果向右，index 在 3, 4, 5, 6 循環
            if (frameIndex < 3 || frameIndex > 6) frameIndex = 3;
            else frameIndex = 3 + ((frameIndex - 3 + 1) % 4);
        }
    }
}

void WaddleDee::draw(QPainter &painter) {
    if (isDead) return;
    QString path = QString(":/Image/Waddle Dee/Waddle_Dee_%1.png").arg(frameIndex);
    QPixmap img(path);
    painter.drawPixmap(x, y, width, height, img);
}
