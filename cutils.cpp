#include "cutils.h"

CUtils::CUtils()
{

}

QColor CUtils::colorDiff(const QColor &clr1, const QColor &clr2){
    quint8 red = quint8(clr1.red()) - quint8(clr2.red());
    quint8 green = quint8(clr1.green()) - quint8(clr2.green());
    quint8 blue = quint8(clr1.blue()) - quint8(clr2.blue());

    return QColor(red,green,blue);
}

QColor CUtils::colorAdd(const QColor &clr1, const QColor &clr2){
    quint8 red = quint8(clr1.red()) + quint8(clr2.red());
    quint8 green = quint8(clr1.green()) + quint8(clr2.green());
    quint8 blue = quint8(clr1.blue()) + quint8(clr2.blue());

    return QColor(red,green,blue);
}

QImage CUtils::imageDiff(const QImage& img1,const QImage& img2){
    QImage tmpImage(img1);

    if(img1.size() != img2.size()){
        return tmpImage;
    }

    int width = img1.width();
    int height = img1.height();
    for(int y=0;y<height;y++){
        for(int x=0;x<width;x++){
            tmpImage.setPixelColor(x,y,colorDiff(img1.pixelColor(x,y),img2.pixelColor(x,y)));
        }
    }

    return tmpImage;
}

QImage CUtils::imageAdd(const QImage& img1,const QImage& img2){
    QImage tmpImage(img1);

    if(img1.size() != img2.size()){
        return tmpImage;
    }

    int width = img1.width();
    int height = img1.height();
    for(int y=0;y<height;y++){
        for(int x=0;x<width;x++){
            tmpImage.setPixelColor(x,y,colorAdd(img1.pixelColor(x,y),img2.pixelColor(x,y)));
        }
    }

    return tmpImage;
}
