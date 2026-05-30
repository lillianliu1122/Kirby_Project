#ifndef HOTHEAD_H
#define HOTHEAD_H

#include "enemy.h"
#include "platform.h"
#include <QVector>

struct FireBall {
    bool active = false;
    bool hitKirby = false;
    float fx, fy, fvx;
    int width = 24, height = 24;
};

class HotHead : public Enemy {
public:
    enum State { PATROL = 0, FIRE_BALL = 1, FLAME_BREATH = 2 };

    HotHead(int startX, int startY, int range, const QVector<Platform>* platforms);
    void updateBehavior(int kirbyX, int kirbyY) override;
    void draw(QPainter &painter) override;

    FireBall fireBall;
    State state;

private:
    int moveRange, startX, actionTimer, fireBallCooldown;
    bool isFacingRight, isBreathingFire;
    const QVector<Platform>* currentPlatforms;

    void updatePatrol();
    void updateFireBall(int kirbyX);
    void updateFlameBreath();
};

#endif
