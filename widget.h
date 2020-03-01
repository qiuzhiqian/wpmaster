#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>

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

private:
    Ui::Widget *ui;

    void setAlpha(QImage& image,int alpha);

public slots:
    void slt_openBackFile();
    void slt_openFrontFile();
    void slt_alphaChange(int value);
    void slt_save();
};
#endif // WIDGET_H
