#include "slopeplatform.h"

SlopePlatform::SlopePlatform(float x1, float y1, float x2, float y2)
    : x1(x1), y1(y1), x2(x2), y2(y2)
{
}

float SlopePlatform::getYAtX(float x) const
{
    // 線性插值：算出 x 位置對應的斜面高度
    float t = (x - x1) / (x2 - x1);
    return y1 + t * (y2 - y1);
}

bool SlopePlatform::containsX(float x) const
{
    return x >= x1 && x <= x2;
}

void SlopePlatform::draw(QPainter &painter, float cameraX)
{
    // debug 用，畫出綠色斜線
    painter.setPen(QPen(Qt::green, 2));
    painter.drawLine((int)(x1 - cameraX), (int)y1,
                     (int)(x2 - cameraX), (int)y2);
}
