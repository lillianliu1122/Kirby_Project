#ifndef HOTHEAD_H
#define HOTHEAD_H

#include "enemy.h"
#include "platform.h"
#include "slopeplatform.h"
#include <QVector>

struct FireBall {
    bool active = false;
    bool hitKirby = false;
    float fx, fy, fvx;
    int width = 100, height = 100;
};

class HotHead : public Enemy {
public:
    enum State { PATROL = 0, FIRE_BALL = 1, FLAME_BREATH = 2 };

    HotHead(int startX, int startY, int range, const QVector<Platform>* platforms);
    void updateBehavior(int kirbyX, int kirbyY) override;
    void updateFireBall(int kirbyX, int kirbyY, const QRect& kirbyRect, const QVector<Platform>& platforms, const QVector<SlopePlatform>& slopes);
    void draw(QPainter &painter) override;

    FireBall fireBall;
    State state;
    QRect getFlameRect() const;
    bool isFlaming() const { return state == FLAME_BREATH; }

private:
    int moveRange, startX, actionTimer, fireBallCooldown;
    bool isFacingRight, isBreathingFire;
    const QVector<Platform>* currentPlatforms;

    void updatePatrol();
    void updateFlameBreath();
};

#endif
