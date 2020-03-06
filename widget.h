#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include "cmask.h"
#include <QPoint>
#include "cmaker.h"
#include <QThread>
#include <QTimer>

QT_BEGIN_NAMESPACE
namespace Ui { class Widget; }
QT_END_NAMESPACE

class Widget : public QWidget
{
    Q_OBJECT

private:
    QString m_imagePath;
public:
    Widget(QWidget *parent = nullptr);
    ~Widget();

    CMask* getMask(){return m_mask;}

private:
    Ui::Widget *ui;
    CMask* m_mask;

    QPoint m_lastPos;

    QThread* m_thread;
    CMaker* m_maker;

public slots:
    void slt_openBackFile();
    void slt_openFrontFile();
    void slt_alphaChange(int value);
    void slt_save();

    void slt_windowClose();
    void slt_windowMax();
    void slt_windowMin();

    void slt_load();
    void slt_make();

    void slt_threadDestory();

protected:

    virtual void mousePressEvent(QMouseEvent *event) override;
    virtual void mouseMoveEvent(QMouseEvent *event) override;
};
#endif // WIDGET_H
