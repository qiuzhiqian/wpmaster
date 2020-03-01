#ifndef CMASK_H
#define CMASK_H

#include <QWidget>

class CMask : public QWidget
{
    Q_OBJECT
public:
    explicit CMask(QWidget *parent = nullptr);

protected:
    virtual void paintEvent(QPaintEvent *event) override;
signals:

};

#endif // CMASK_H
