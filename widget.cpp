#include "widget.h"
#include "ui_widget.h"

#include <QFileDialog>
#include <QDir>
#include <QSlider>
#include <QDebug>
#include <QRgb>
#include <QColor>

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);

    m_mask = new CMask(this);

    ui->verticalLayout->insertWidget(0,m_mask);

    connect(ui->btn_openback,SIGNAL(clicked(bool)),this,SLOT(slt_openBackFile()));
    connect(ui->btn_openfront,SIGNAL(clicked(bool)),this,SLOT(slt_openFrontFile()));
    connect(ui->sld_alpha,SIGNAL(valueChanged(int)),this,SLOT(slt_alphaChange(int)));
    connect(ui->btn_save,SIGNAL(clicked(bool)),this,SLOT(slt_save()));
    //qDebug() << "min:" << ui->sld_alpha->minimum();
    //qDebug() << "max:" << ui->sld_alpha->maximum();
}

Widget::~Widget()
{
    delete ui;
}

void Widget::slt_openBackFile(){
    m_imagePath = QFileDialog::getOpenFileName(this,tr("Open File"),QDir::currentPath(),tr("Images (*.png *.jpg)"));
    m_mask->setBack(QImage(m_imagePath).convertToFormat(QImage::Format_ARGB32));
}

void Widget::slt_openFrontFile(){
    m_imagePath = QFileDialog::getOpenFileName(this,tr("Open File"),QDir::currentPath(),tr("Images (*.png *.jpg)"));
    m_mask->setFront(QImage(m_imagePath).convertToFormat(QImage::Format_ARGB32));
}

void Widget::slt_alphaChange(int value){
    //m_mask->update();
    int alpha = value*255/100;
    m_mask->setAlpha(alpha);
}

void Widget::slt_save(){
}

