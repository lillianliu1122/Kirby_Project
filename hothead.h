#ifndef HOTHEAD_H
#define HOTHEAD_H

#include "enemy.h"

struct FireBall {
    bool active = false;
    float fx, fy, fvx;
};

class HotHead : public Enemy {
public:
    HotHead(int startX, int startY);
    void updateBehavior(int kirbyX, int kirbyY) override;
    void draw(QPainter &painter) override;

private:
    int state; // 0: 巡邏, 1: 吐火球, 2: 噴火
    int actionTimer;
    bool isFacingRight;
    bool isBreathingFire;
    FireBall fireBall;
};

#endif // HOTHEAD_H
