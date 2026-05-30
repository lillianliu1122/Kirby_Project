#ifndef SPARKY_H
#define SPARKY_H

#include "enemy.h"

class Sparky : public Enemy {
public:
    Sparky(int startX, int startY, int range);
    void updateBehavior(int kirbyX, int kirbyY) override;
    void draw(QPainter &painter) override;

    bool isDischarging; // 外部存取用，判斷是否在放電

private:
    int timer;
    int jumpTimer;
    int moveDirection = 1;
    int moveRange;
    bool isGrounded;
    bool lastFacingLeft = true;
};

#endif // SPARKY_H
