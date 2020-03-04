#ifndef CMASK_H
#define CMASK_H

#include <QWidget>
#include <QImage>
#include <QRect>
#include <QMouseEvent>

class CMask : public QWidget
{
    Q_OBJECT
public:
    explicit CMask(QWidget *parent = nullptr);
    CMask(const QImage& back,const QImage& front,QWidget *parent = nullptr);

    void setBack(const QImage& image);
    void setFront(const QImage& image);
    void setAlpha(int alpha);
    void setRadius(int radius);
    void setMask(int x,int y);
private:
    QImage m_back;
    QImage m_front;
    QRect m_mask;
    int m_alpha;
    int m_radius;

    void setImageAlpha(QImage &image,const QRect& rect,int alpha);
    const QImage layerAdd(const QImage& base,const QImage& ext,QRect rect);
protected:
    virtual void paintEvent(QPaintEvent *event) override;
    virtual void mouseMoveEvent(QMouseEvent *event) override;
signals:

};

#endif // CMASK_H
