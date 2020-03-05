#ifndef CUTILS_H
#define CUTILS_H

#include <QtCore>
#include <QImage>

class CUtils
{
public:
    CUtils();
    static QColor colorDiff(const QColor& clr1,const QColor& clr2);
    static QImage imageDiff(const QImage& img1,const QImage& img2);
};

#endif // CUTILS_H
