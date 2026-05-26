#ifndef PLATFORM_H
#define PLATFORM_H

#include <QRectF>
#include <QPixmap>
#include <QPainter>

enum class PlatformType {
    Floor,   // 地板
    Brick,    // 磚頭（可被星星彈打破）
    Invisible  // 透明碰撞區塊（無圖片，單向）
};

class Platform
{
public:
    Platform(float x, float y, float w, float h, PlatformType type);
    Platform() : type(PlatformType::Floor), visible(true), x(0), y(0), w(0), h(0) {}

    void draw(QPainter &painter, float cameraX);
    QRectF getRect() const;

    PlatformType type;
    bool visible;  // 磚頭被打破後設為 false

private:
    float x, y, w, h;
    static QPixmap *imgFloor;
    static QPixmap *imgBrick;
};

#endif // PLATFORM_H
