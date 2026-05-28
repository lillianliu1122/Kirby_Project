#include "portal.h"

QPixmap* Portal::imgDoor = nullptr;
QPixmap* Portal::imgGoal = nullptr;

Portal::Portal(float x, float y, PortalType type)
    : type(type), x(x), y(y)
{
}

void Portal::draw(QPainter &painter, float cameraX)
{
    if (!imgDoor) imgDoor = new QPixmap(":/Image/item/door.png");
    if (!imgGoal) imgGoal = new QPixmap(":/Image/item/goal_door.png");

    QPixmap *img = (type == PortalType::ToStage2) ? imgDoor : imgGoal;
    if (!img->isNull()) {
        painter.drawPixmap((int)(x - cameraX), (int)y, W, H, *img);
    }
}

QRectF Portal::getRect() const
{
    return QRectF(x, y, W, H);
}
