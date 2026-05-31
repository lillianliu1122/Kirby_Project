#ifndef STARBULLET_H
#define STARBULLET_H

#include <QPainter>
#include <QRectF>

class StarBullet
{
public:
    StarBullet(float x, float y, bool goingRight);
    StarBullet() : active(false), goingRight(true), x(0), y(0), vx(0) {}

    void update();
    void draw(QPainter &painter, float cameraX);
    QRectF getRect() const;

    bool active;       // false 表示已消失
    bool goingRight;

private:
    float x, y;
    float vx;
    static const int W = 60;
    static const int H = 60;
    static QPixmap *img;
};

#endif // STARBULLET_H
