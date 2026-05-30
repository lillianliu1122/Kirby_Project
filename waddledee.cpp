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
        x = startX + walkRange; // 強制校正位置
        vx = -std::abs(vx);     // 強制向左
        frameIndex = 3; // 轉向右，重置為 3
        frameTimer = 0; // 同步重置計時器
    } else if (x < startX - walkRange) {
        x = startX - walkRange; // 強制校正位置
        vx = std::abs(vx);      // 強制向右
        frameIndex = 0; // 轉向左，重置為 0
        frameTimer = 0; // 同步重置計時器
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

    // 3. 物理移動 (包含重力)
    //if (!isGrounded) vy += 0.6f;
    x += vx;
    y += vy;

    // 2. 地板檢測 (確保不穿過地板)
    /*if (y >= 800) {
        y = 800; vy = 0; isGrounded = true;
    } else {
        isGrounded = false;
    }*/

}

void WaddleDee::draw(QPainter &painter) {
    if (isDead) return;
    QString path = QString(":/Image/Waddle Dee/Waddle_Dee_%1.png").arg(frameIndex);
    QPixmap img(path);

    painter.drawPixmap(x, y, width, height, img);
}
