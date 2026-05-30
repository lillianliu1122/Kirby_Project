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
};

#endif // GORDO_H
