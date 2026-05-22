#include "platform.h"
/*
QPixmap Platform::imgFloor;
QPixmap Platform::imgBrick;
bool Platform::imagesLoaded = false;*/
QPixmap* Platform::imgFloor = nullptr;
QPixmap* Platform::imgBrick = nullptr;

Platform::Platform(float x, float y, float w, float h, PlatformType type)
    : type(type), visible(true), x(x), y(y), w(w), h(h)
{
    /*if (!imagesLoaded) {
        imgFloor = QPixmap(":/Image/item/floor.png");
        imgBrick = QPixmap(":/Image/item/brick.png");
        imagesLoaded = true;
    }*/
}

void Platform::draw(QPainter &painter, float cameraX)
{
    if (!visible) return;

    //int drawX = (int)(x - cameraX);
    //QPixmap &img = (type == PlatformType::Brick) ? imgBrick : imgFloor;

    // 第一次畫的時候才載入圖片
    if (!imgFloor) imgFloor = new QPixmap(":/Image/item/floor.png");
    if (!imgBrick) imgBrick = new QPixmap(":/Image/item/brick.png");

    int drawX = (int)(x - cameraX);
    QPixmap &img = (type == PlatformType::Brick) ? *imgBrick : *imgFloor;

    // 用圖片鋪滿整個平台寬度
    for (int i = 0; i < (int)w; i += img.width()) {
        int tileW = qMin(img.width(), (int)w - i);
        painter.drawPixmap(drawX + i, (int)y, tileW, (int)h,
                           img, 0, 0, tileW, img.height());
    }
}

QRectF Platform::getRect() const
{
    return QRectF(x, y, w, h);
}
