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

    connect(ui->btn_openback,SIGNAL(clicked(bool)),this,SLOT(slt_openBackFile()));
    connect(ui->btn_openfront,SIGNAL(clicked(bool)),this,SLOT(slt_openFrontFile()));
    connect(ui->sld_alpha,SIGNAL(valueChanged(int)),this,SLOT(slt_alphaChange(int)));
    connect(ui->btn_save,SIGNAL(clicked(bool)),this,SLOT(slt_save()));
    //qDebug() << "min:" << ui->sld_alpha->minimum();
    //qDebug() << "max:" << ui->sld_alpha->maximum();

    //ui->lb_back->setFixedSize(ui->widget->size());
    //ui->lb_front->setFixedSize(ui->widget->size());
    qDebug()<<ui->widget->size();
}

Widget::~Widget()
{
    delete ui;
}

void Widget::slt_openBackFile(){
    m_imagePath = QFileDialog::getOpenFileName(this,tr("Open File"),QDir::currentPath(),tr("Images (*.png *.xpm *.jpg)"));
    QPixmap pm(m_imagePath);
    ui->lb_back->setPixmap(pm.scaled(ui->lb_back->size()));
}

void Widget::slt_openFrontFile(){
    m_imagePath = QFileDialog::getOpenFileName(this,tr("Open File"),QDir::currentPath(),tr("Images (*.png *.xpm *.jpg)"));
    QPixmap pm(m_imagePath);
    ui->lb_front->setPixmap(pm.scaled(ui->lb_front->size()));
}

void Widget::slt_alphaChange(int value){
    int alpha = 255*value/100;
    qDebug()<<"alpha:"<< alpha;
    qDebug()<<"alpha:"<<alpha;
    QImage image(m_imagePath);
    QImage tmpImage=image.convertToFormat(QImage::Format_ARGB32);
    setAlpha(tmpImage,alpha);
    ui->lb_front->setPixmap(QPixmap::fromImage(tmpImage).scaled(ui->lb_front->size()));
}

void Widget::slt_save(){
    QString name = QFileDialog::getSaveFileName(this,tr("Open File"),QDir::currentPath(),tr("Images (*.png *.xpm *.jpg)"));

    int value = ui->sld_alpha->value();
    int alpha = 255*value/100;
    qDebug()<<"alpha:"<<alpha;
    QImage image(m_imagePath);
    QImage tmpImage=image.convertToFormat(QImage::Format_ARGB32);
    setAlpha(tmpImage,alpha);
    tmpImage.save(name,"png",100);
}

void Widget::setAlpha(QImage &image,int alpha){
    qDebug()<<"format:"<<image.format();
    if(!image.hasAlphaChannel()){
        image.createAlphaMask();
    }
    unsigned char *pData=image.bits();
    int width=image.width();
    int height=image.height();
    qDebug()<<"width:"<< width;
    qDebug()<<"height:"<< height;

    for(int i=0;i<height;i++){
        for(int j=0;j<width;j++){
            //*(pData+(i*width+j)*4+0) = alpha;
            //*(pData+(i*width+j)*4+1) = alpha;
            //*(pData+(i*width+j)*4+2) = alpha;
            *(pData+(i*width+j)*4+3) = alpha;
        }
    }
}

