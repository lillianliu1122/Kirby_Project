#ifndef WADDLEDEE_H
#define WADDLEDEE_H

#include "enemy.h"

class WaddleDee : public Enemy {
public:
    WaddleDee(int startX, int startY, int range);
    void updateBehavior(int kirbyX, int kirbyY) override;
    void draw(QPainter &painter) override;

private:
    int walkRange;
    int frameTimer; // 用於計時，控制切換速度
    int frameIndex; // 當前播放到第幾張圖 (0-6)
};

#endif
