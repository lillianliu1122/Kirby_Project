#ifndef SLOPEPLATFORM_H
#define SLOPEPLATFORM_H

#include <QPainter>

class SlopePlatform
{
public:
    // (x1,y1) 是左端點，(x2,y2) 是右端點
    SlopePlatform(float x1, float y1, float x2, float y2);
    SlopePlatform() : x1(0), y1(0), x2(0), y2(0) {}

    // 取得在某個 x 位置的斜面高度
    float getYAtX(float x) const;

    // Kirby 的 x 是否在斜面範圍內
    bool containsX(float x) const;

    // debug 用，畫出斜面線
    void draw(QPainter &painter, float cameraX);

    float x1, y1, x2, y2;
};

#endif // SLOPEPLATFORM_H
