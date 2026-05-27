#ifndef PORTAL_H
#define PORTAL_H

#include <QRectF>
#include <QPixmap>
#include <QPainter>

enum class PortalType {
    ToStage2,  // 任意門（Stage1 終點）
    Goal       // 終點門（Stage2 終點）
};

class Portal
{
public:
    Portal(float x, float y, PortalType type);
    Portal() : type(PortalType::ToStage2), x(0), y(0) {}

    void draw(QPainter &painter, float cameraX);
    QRectF getRect() const;

    PortalType type;

private:
    float x, y;
    static const int W = 200;
    static const int H = 268;
    static QPixmap *imgDoor;
    static QPixmap *imgGoal;
};

#endif // PORTAL_H
