#include "widget.h"
#include "ui_widget.h"
#include "windows.h"

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

    m_mask = new CMask(nullptr);
    m_mask->setWindowFlags(Qt::FramelessWindowHint);
    m_mask->setFixedSize(1200,1000);

    //ui->verticalLayout->insertWidget(0,m_mask);

    connect(ui->btn_openback,SIGNAL(clicked(bool)),this,SLOT(slt_openBackFile()));
    connect(ui->btn_openfront,SIGNAL(clicked(bool)),this,SLOT(slt_openFrontFile()));
    connect(ui->sld_alpha,SIGNAL(valueChanged(int)),this,SLOT(slt_alphaChange(int)));
    connect(ui->btn_save,SIGNAL(clicked(bool)),this,SLOT(slt_save()));
    //qDebug() << "min:" << ui->sld_alpha->minimum();
    //qDebug() << "max:" << ui->sld_alpha->maximum();

    HWND background = NULL;
    HWND hwnd = ::FindWindowA("progman","Program Manager");
    HWND worker = NULL;
    do{
        worker = ::FindWindowExA(NULL,worker,"WorkerW",NULL);
        if(worker!=NULL){
            char buff[200] = {0};

            int ret = GetClassName(worker,(WCHAR*)buff,sizeof(buff)*2);
            if(ret == 0){
                int err = GetLastError();
                qDebug()<<"err:"<<err;
            }
            QString className = QString::fromUtf16((char16_t*)buff);
            qDebug()<<"classname:"<<className;
        }
        if(GetParent(worker) == hwnd){
            background = worker;
        }
    }while(worker !=NULL);

    HWND current = (HWND)m_mask->winId();
    SetParent(current,background);
    m_mask->show();
}

Widget::~Widget()
{
    qDebug()<<"exit1";
    m_mask->deleteLater();
    qDebug()<<"exit2";
    delete m_mask;
    qDebug()<<"exit3";
    delete ui;
    qDebug()<<"exit4";
}

void Widget::slt_openBackFile(){
    m_imagePath = QFileDialog::getOpenFileName(this,tr("Open File"),QDir::currentPath(),tr("Images (*.png *.jpg)"));
    m_mask->setBack(QImage(m_imagePath).convertToFormat(QImage::Format_ARGB32));
    m_mask->update();
}

void Widget::slt_openFrontFile(){
    m_imagePath = QFileDialog::getOpenFileName(this,tr("Open File"),QDir::currentPath(),tr("Images (*.png *.jpg)"));
    m_mask->setFront(QImage(m_imagePath).convertToFormat(QImage::Format_ARGB32));
    m_mask->update();
}

void Widget::slt_alphaChange(int value){
    //m_mask->update();
    int alpha = value*255/100;
    m_mask->setAlpha(alpha);
}

void Widget::slt_save(){
}

