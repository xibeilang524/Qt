#include "NinePatchPainter.h"
#include <QPixmap>
#include <QList>
#include <QRect>
#include <QPainter>

/*-----------------------------------------------------------------------------|
 |                           NinePatchPainterPrivate                           |
 |----------------------------------------------------------------------------*/
class NinePatchPainterPrivate {
public:
    NinePatchPainterPrivate(int left, int top, int right, int bottom, const QString &imagePath);

    void prepareNinePatchImages(const QString imagePath); // 根据给定的 left, top, right, bottom 把图片按九宫格切为 9 个子图
    QList<QRect> calculateNinePatchRects(const QRect &rect) const; // 根据给定的 left, top, right, bottom 把 rect 按九宫格切为 9 个 rect
    QPixmap scaleImage(const QPixmap &pixmap, const QSize &size) const; // 对图片进行缩放处理

    int left;   // 左边的宽
    int top;    // 上边的宽
    int right;  // 右边的宽
    int bottom; // 下边的宽

    QPixmap leftPixmap;        // 左边的图片
    QPixmap topLeftPixmap;     // 左上角的图片
    QPixmap topPixmap;         // 顶部的图片
    QPixmap topRightPixmap;    // 右上角的图片
    QPixmap rightPixmap;       // 右边的图片
    QPixmap bottomLeftPixmap;  // 左下角的图片
    QPixmap bottomPixmap;      // 底部的图片
    QPixmap bottomRightPixmap; // 右下角的图片
    QPixmap centerPixmap;      // 中间的图片
};

NinePatchPainterPrivate::NinePatchPainterPrivate(int left, int top, int right, int bottom, const QString &imagePath)
    : left(left), top(top), right(right), bottom(bottom) {
    prepareNinePatchImages(imagePath); // 准备好九宫格的 9 个子图，程序运行期间不会变，所以保存起来
}

void NinePatchPainterPrivate::prepareNinePatchImages(const QString imagePath) {
    // 加载背景图
    QPixmap pixmap(imagePath);

    // 把圆角背景图分割成 9 个部分
    QRect pixmapRect(0, 0, pixmap.width(), pixmap.height());
    QList<QRect> rects = calculateNinePatchRects(pixmapRect);

    leftPixmap        = pixmap.copy(rects.at(0));
    topLeftPixmap     = pixmap.copy(rects.at(1));
    topPixmap         = pixmap.copy(rects.at(2));
    topRightPixmap    = pixmap.copy(rects.at(3));
    rightPixmap       = pixmap.copy(rects.at(4));
    bottomRightPixmap = pixmap.copy(rects.at(5));
    bottomPixmap      = pixmap.copy(rects.at(6));
    bottomLeftPixmap  = pixmap.copy(rects.at(7));
    centerPixmap      = pixmap.copy(rects.at(8));
}

QList<QRect> NinePatchPainterPrivate::calculateNinePatchRects(const QRect &rect) const {
    // 根据给定的 4 个边的宽和高，把 Rect 从上到下，从左到右分割成 9 个部分
    // 顺时针计算
    int x = rect.x();
    int y = rect.y();
    int cw = rect.width() - left - right;
    int ch = rect.height() - top - bottom;

    QRect leftRect(x, y + top, left, ch);
    QRect topLeftRect(x, y, left, top);
    QRect topRect(x + left, y, cw, top);
    QRect topRightRect(x + left + cw, y, right, top);
    QRect rightRect(x + left + cw, y + top, right, ch);
    QRect bottomRightRect(x + left + cw, y + top + ch, right, bottom);
    QRect bottomRect(x + left, y + top + ch, cw, bottom);
    QRect bottomLeftRect(x, y + top + ch, left, bottom);
    QRect centerRect(x + left, y + top, cw, ch);

    return QList<QRect>() << leftRect << topLeftRect
                          << topRect << topRightRect << rightRect
                          << bottomRightRect << bottomRect << bottomLeftRect
                          << centerRect;
}

QPixmap NinePatchPainterPrivate::scaleImage(const QPixmap &pixmap, const QSize &size) const {
    // 缩放的时候忽略图片的高宽比，使用平滑缩放的效果
    return pixmap.scaled(size, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
}

/*-----------------------------------------------------------------------------|
 |                              NinePatchPainter                               |
 |----------------------------------------------------------------------------*/
NinePatchPainter::NinePatchPainter(int left, int top, int right, int bottom, const QString &imagePath)
    : d(new NinePatchPainterPrivate(left, top, right, bottom, imagePath)) {
}

NinePatchPainter::~NinePatchPainter() {
    delete d;
}

void NinePatchPainter::draw(QPainter *painter, const QRect &rect) const {
    // 把要绘制的 Rect 分割成 9 个部分，上，右，下，左 4 边的宽和背景图的一样
    QList<QRect> rects = d->calculateNinePatchRects(rect);

    QRect leftRect        = rects.at(0);
    QRect topLeftRect     = rects.at(1);
    QRect topRect         = rects.at(2);
    QRect topRightRect    = rects.at(3);
    QRect rightRect       = rects.at(4);
    QRect bottomRightRect = rects.at(5);
    QRect bottomRect      = rects.at(6);
    QRect bottomLeftRect  = rects.at(7);
    QRect centerRect      = rects.at(8);

    // 把背景图片上的 9 个部分使用缩放的方式绘制到 Rect 上对应的 9 个部分
    painter->drawTiledPixmap(leftRect, d->scaleImage(d->leftPixmap, leftRect.size()));
    painter->drawPixmap(topLeftRect, d->scaleImage(d->topLeftPixmap, topLeftRect.size()));
    painter->drawTiledPixmap(topRect, d->scaleImage(d->topPixmap, topRect.size()));
    painter->drawPixmap(topRightRect, d->scaleImage(d->topRightPixmap,topRightRect.size()));
    painter->drawTiledPixmap(rightRect, d->scaleImage(d->rightPixmap, rightRect.size()));
    painter->drawPixmap(bottomRightRect, d->scaleImage(d->bottomRightPixmap, bottomRightRect.size()));
    painter->drawTiledPixmap(bottomRect, d->scaleImage(d->bottomPixmap, bottomRect.size()));
    painter->drawPixmap(bottomLeftRect, d->scaleImage(d->bottomLeftPixmap, bottomLeftRect.size()));
    painter->drawPixmap(centerRect, d->scaleImage(d->centerPixmap, centerRect.size()));
}