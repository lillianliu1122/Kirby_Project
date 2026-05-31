#include "starbullet.h"

QPixmap* StarBullet::img = nullptr;

StarBullet::StarBullet(float x, float y, bool goingRight)
    : active(true), goingRight(goingRight),
      x(x), y(y), vx(goingRight ? 10.0f : -10.0f)
{
}

void StarBullet::update()
{
    if (!active) return;
    x += vx;
}

void StarBullet::draw(QPainter &painter, float cameraX)
{
    if (!active) return;
    if (!img) img = new QPixmap(":/Image/Kirby_normal/kirby_attack_star(2).png");

    int drawX = (int)(x - cameraX);
    painter.drawPixmap(drawX, (int)y, W, H, *img);
}

QRectF StarBullet::getRect() const
{
    return QRectF(x, y, W, H);
}
