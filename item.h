#ifndef ITEM_H
#define ITEM_H

#include <QPainter>
#include <QRectF>
#include <QPixmap>

struct Tomato {
    float x, y;
    bool isActive = true;

    QRectF getRect() const { return QRectF(x, y, 100, 100); }

    void draw(QPainter &painter, float cameraX) {
        if (!isActive) return;
        QPixmap img(":/Image/item/Maxim Tomato.png");
        painter.drawPixmap((int)(x - cameraX), (int)y, 100, 100, img);
    }
};

struct OneUp {
    float x, y;
    bool isCollected = false;

    QRectF getRect() const { return QRectF(x, y, 100, 100); }

    void draw(QPainter &painter, float cameraX) {
        if (isCollected) return;
        QPixmap img(":/Image/item/1UP.png");
        painter.drawPixmap((int)(x - cameraX), (int)y, 100, 100, img);
    }
};

#endif
