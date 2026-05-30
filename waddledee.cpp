#include "waddledee.h"

WaddleDee::WaddleDee(int startX, int startY, int range)
    : Enemy(startX, startY, "WaddleDee", "None", true), walkRange(range) {
    vx = -1; // 初始往左走
}

void WaddleDee::updateBehavior(int kirbyX, int kirbyY) {
    Q_UNUSED(kirbyX); Q_UNUSED(kirbyY);
    if (isDead) return;
    x += vx;
    y += vy;

    // 1. 範圍內來回移動
    // 如果怪物離出生點的距離超過了 walkRange，就反轉速度
    if (x > startX + walkRange) {
        vx = -std::abs(vx);     // 強制向左
    } else if (x < startX - walkRange) {
        vx = std::abs(vx);      // 強制向右
    }

    // 2. 更新動畫計時器 (每 8 幀切換一次圖片)
    frameTimer++;
    if (frameTimer >= 8) {
        frameTimer = 0;
        // 根據方向決定動畫循環的範圍
        if (vx < 0) {
            // 如果向左，index 在 0, 1, 2 循環
            frameIndex = (frameIndex + 1) % 3;
        } else {
            // 如果向右，index 在 3, 4, 5, 6 循環
            // 計算方式：3 + (目前的 index - 3 + 1) % 4
            // 注意：如果剛從左轉右，且 frameIndex 還在 0-2 之間
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
