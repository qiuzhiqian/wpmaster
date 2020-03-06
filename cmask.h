#ifndef CMASK_H
#define CMASK_H

#include <QWidget>
#include <QImage>
#include <QRect>
#include <QMouseEvent>
#include <QTimer>

class CMask : public QWidget
{
    Q_OBJECT
public:
    explicit CMask(QWidget *parent = nullptr);
    CMask(const QImage& back,const QImage& front,QWidget *parent = nullptr);
    ~CMask();

    void loadPackage(const QImage& base,const QImage& canvas,const QString& maskPath);
    void setBack(const QImage& image);
    void addFront(const QImage& image);
    void setAlpha(int alpha);
    void setRadius(int radius);
    void setMask(int x,int y);
private:
    int m_currentIndex;
    int m_oldIndex;
    QImage m_back;
    QList<QImage> m_front;
    QRect m_mask;
    int m_alpha;
    int m_radius;

    QTimer* m_switchTimer;

    void setImageAlpha(QImage &image,const QRect& rect,int alpha);
    const QImage layerAdd(const QImage& base,const QImage& ext,QRect rect);
protected:
    virtual void paintEvent(QPaintEvent *event) override;
    virtual void mouseMoveEvent(QMouseEvent *event) override;
signals:

public slots:
    void slt_switch();
};

#endif // CMASK_H
