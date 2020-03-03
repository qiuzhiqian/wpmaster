#include "cmask.h"
#include <QRect>
#include <QPainter>
#include <QRegion>
#include <QDebug>

CMask::CMask(QWidget *parent) : QWidget(parent),
    m_mask(QRect(100,100,1000,1000)),
    m_alpha(10),
    m_radius(200)
{
    setMouseTracking(true);
     m_mask = QRect(0,0,m_radius,m_radius);
}

CMask::CMask(const QImage& back,const QImage& front,QWidget *parent) :
    QWidget(parent),
    m_back(back),
    m_front(front),
    m_mask(QRect(0,0,200,200)),
    m_alpha(10),
    m_radius(200)

{
    setMouseTracking(true);
     m_mask = QRect(0,0,m_radius,m_radius);
}

void CMask::setBack(const QImage &image){
    m_back = image;
}

void CMask::setFront(const QImage &image){
    m_front = image;
}

void CMask::setAlpha(int alpha){
    m_alpha = alpha;
}

void CMask::setRadius(int radius){
    m_radius = radius;
}

void CMask::setMask(int x,int y){
    m_mask = QRect(x-m_radius/2,y-m_radius/2,m_radius,m_radius);
}

void CMask::paintEvent(QPaintEvent *event){
    QWidget::paintEvent(event);
    QPainter painter(this);
    painter.drawImage(this->rect(),m_back,this->rect());
    QImage tmpImage = m_front;
    setImageAlpha(tmpImage,m_mask,m_alpha);
    painter.drawImage(this->rect(),tmpImage,this->rect());
}

void CMask::mouseMoveEvent(QMouseEvent *event){
    QWidget::mouseMoveEvent(event);
    qDebug()<<event->pos();

    m_mask = QRect(event->pos().x()-m_radius/2,event->pos().y()-m_radius/2,m_radius,m_radius);
    update();
}

void CMask::setImageAlpha(QImage &image,const QRect& rect,int alpha){
    unsigned char *pData=image.bits();
    int width=image.width();
    int height=image.height();

    if(alpha>255) alpha = 255;
    else if(alpha<0) alpha = 0;

    QRegion region(rect,QRegion::Ellipse);

    int startx = qMax(0,rect.x());
    int starty = qMax(0,rect.y());
    int endx = qMin(width,rect.x()+rect.width());
    int endy = qMin(height,rect.y()+rect.height());

    for(int y=starty;y<endy;y++){
        for(int x=startx;x<endx;x++){
            if(region.contains(QPoint(x,y))){
                *(pData+(y*width+x)*4+3) = alpha;
            }
        }
    }
}
