#include "cmask.h"
#include <QRect>
#include <QPainter>
#include <QRegion>
#include <QDebug>

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonValue>

#include "cutils.h"

CMask::CMask(QWidget *parent) : QWidget(parent),
    m_currentIndex(0),
    m_oldIndex(0),
    m_mask(QRect(100,100,1000,1000)),
    m_alpha(10),
    m_radius(200)
{
    setMouseTracking(true);
     m_mask = QRect(0,0,m_radius,m_radius);
     m_switchTimer = new QTimer(this);
}

CMask::CMask(const QImage& back,const QImage& front,QWidget *parent) :
    QWidget(parent),
    m_currentIndex(0),
    m_oldIndex(0),
    m_back(back),
    m_front{front},
    m_mask(QRect(0,0,200,200)),
    m_alpha(10),
    m_radius(200)

{
    setMouseTracking(true);
     m_mask = QRect(0,0,m_radius,m_radius);
     m_switchTimer = new QTimer(this);
}

CMask::~CMask(){
    if(m_switchTimer!=nullptr){
        delete m_switchTimer;
        m_switchTimer = nullptr;
    }
}

void CMask::loadMaskFile(const QString maskPath,const QImage& base){
    if(maskPath == ""){
        return;
    }

    QFile file(maskPath+"/config.json");
    if(!file.exists()){
        return;
    }

    bool ok = file.open(QIODevice::ReadOnly);
    if(!ok){
        return;
    }

    QByteArray ba = file.readAll();
    file.close();

    QJsonParseError err;
    QJsonDocument doc = QJsonDocument::fromJson(ba,&err);
    if(doc.isNull()){
        qDebug()<<"err:"<<err.errorString();
        return;
    }

    if(!doc.isObject()){
        return;
    }

    QJsonObject obj = doc.object();
    QJsonValue val = obj.value("interval");
    if(!val.isDouble()){
        return;
    }

    val = obj.value("list");
    if(!val.isArray()){
        return;
    }

    QStringList maskList;
    QJsonArray array = val.toArray();
    for(auto item:array){
        if(item.isString()){
            maskList.append(item.toString());
        }
    }

    QImage img2=base;
    for(auto item:maskList){
        QImage img1(maskPath+"/"+item);
        if(!img1.isNull()){
            QImage tmpImage = CUtils::imageAdd(img1,img2);
            m_front.append(tmpImage);
            img2=tmpImage;
        }
    }
}

void CMask::loadPackage(const QImage& base,const QImage& canvas,const QString& maskPath){
    m_back = canvas;
    if(!m_front.isEmpty()){
        m_front.clear();
    }

    m_front.append(base);

    loadMaskFile(maskPath,base);

    if(m_front.size()>1){
        connect(m_switchTimer,SIGNAL(timeout()),this,SLOT(slt_switch()));
        m_switchTimer->start(60);
    }
    m_currentIndex = 0;
    m_oldIndex = 0;
}

void CMask::setBack(const QImage &image){
    m_back = image;
}

void CMask::addFront(const QImage &image){
    m_front.append(image);
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

    if(m_front.size()>0 && m_currentIndex<m_front.size()){
        QPainter painter(this);
        painter.drawImage(this->rect(),layerAdd(m_front.at(m_currentIndex),m_back,m_mask),this->rect());
    }
}

void CMask::mouseMoveEvent(QMouseEvent *event){
    QWidget::mouseMoveEvent(event);

    m_mask = QRect(event->pos().x()-m_radius/2,event->pos().y()-m_radius/2,m_radius,m_radius);
    update();
}

const QImage CMask::layerAdd(const QImage& base,const QImage& ext,QRect rect){
    QImage retImage(base);
    int startx = qMax(0,rect.x());
    int starty = qMax(0,rect.y());
    int endx = qMin(base.size().width(),rect.x()+rect.width());
    int endy = qMin(base.size().height(),rect.y()+rect.height());

    QRegion regionExt(ext.rect(),QRegion::Rectangle);
    QRegion regionCursor(rect,QRegion::Ellipse);


    for(int y=starty;y<endy;y++){
        for(int x=startx;x<endx;x++){
            if(regionCursor.contains(QPoint(x,y))&&regionExt.contains(QPoint(x,y))){
                //*(pData+(y*width+x)*4+3) = alpha;
                retImage.setPixelColor(x,y,ext.pixelColor(x,y));
            }
        }
    }
    return retImage;
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

void CMask::slt_switch(){
    if(m_currentIndex > m_oldIndex){
        if(m_currentIndex>=m_front.size()-1){
            m_oldIndex = m_front.size() - 1;
            m_currentIndex = m_front.size() - 2;
        }else{
            m_oldIndex = m_currentIndex;
            m_currentIndex++;
        }
    }else if(m_currentIndex < m_oldIndex){
        if(m_currentIndex == 0){
            m_oldIndex = 0;
            m_currentIndex = 1;
        }else{
            m_oldIndex = m_currentIndex;
            m_currentIndex--;
        }
    }else{
        m_oldIndex = 0;
        m_currentIndex = 1;
    }
    update();
}
