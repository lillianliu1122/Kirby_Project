#ifndef GORDO_H
#define GORDO_H

#include "enemy.h"

class Gordo : public Enemy {
public:
    Gordo(int startX, int startY, int range);
    void updateBehavior(int kirbyX, int kirbyY) override;
    void draw(QPainter &painter) override;

private:
    int moveRange;
    int moveDirection;
    int startY;           // 記錄初始 Y 座標，以便在範圍內移動

    QPixmap animationFrames[2]; // 存放兩張圖片
    int frameIndex;             // 目前該顯示哪一張圖 (0 或 1)
    int animationTimer;         // 用於計時，控制切換速度
};

#endif // GORDO_H
