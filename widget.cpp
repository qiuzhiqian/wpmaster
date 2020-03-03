#include "widget.h"
#include "ui_widget.h"
#include "windows.h"

#include <QFileDialog>
#include <QDir>
#include <QSlider>
#include <QDebug>
#include <QRgb>
#include <QColor>
#include <QScreen>

HHOOK mouseHook=NULL;

extern Widget* w;
LRESULT CALLBACK mouseProc(int nCode,WPARAM wParam,LPARAM lParam )
{
    if(nCode == HC_ACTION) //当nCode等于HC_ACTION时，要求得到处理
    {
       if(wParam==WM_MOUSEMOVE)//鼠标的移动
       {
           POINT p;
           GetCursorPos(&p);//获取鼠标坐标
            CMask* mask = w->getMask();
            mask->setMask(p.x,p.y);
            //双薪壁纸
            mask->update();
       }
    }
    //qDebug()<<nCode<<","<<wParam<<","<<lParam;
    return CallNextHookEx(mouseHook,nCode,wParam,lParam);//返回给下一个钩子子程处理
}

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);

    this->setWindowFlags(Qt::FramelessWindowHint);

    m_mask = new CMask(nullptr);
    m_mask->setWindowFlags(Qt::FramelessWindowHint);

    QList<QScreen *> screenList = QGuiApplication::screens();

    if(screenList.size()>0){
        m_mask->setFixedSize(screenList.at(0)->availableSize());
    }else{
        return;
    }

    connect(ui->btn_openback,SIGNAL(clicked(bool)),this,SLOT(slt_openBackFile()));
    connect(ui->btn_openfront,SIGNAL(clicked(bool)),this,SLOT(slt_openFrontFile()));
    connect(ui->sld_alpha,SIGNAL(valueChanged(int)),this,SLOT(slt_alphaChange(int)));
    connect(ui->btn_save,SIGNAL(clicked(bool)),this,SLOT(slt_save()));
    connect(ui->btn_test,SIGNAL(clicked(bool)),this,SLOT(slt_test()));

    connect(ui->btn_close,SIGNAL(clicked(bool)),this,SLOT(slt_windowClose()));
    connect(ui->btn_min,SIGNAL(clicked(bool)),this,SLOT(slt_windowMin()));
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
            //QString className = QString::fromUtf16((char16_t*)buff);
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
    delete ui;
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

void Widget::slt_windowClose(){
    delete m_mask;

    //调用该函数刷新壁纸，使得壁纸恢复
    SystemParametersInfoA(SPI_SETDESKWALLPAPER,0,NULL,SPIF_SENDWININICHANGE);

    //关闭窗口
    this->close();
}

void Widget::slt_windowMax(){

}

void Widget::slt_windowMin(){
    this->showMinimized();
}

void Widget::slt_test(){
    qDebug()<<"register hook";
    mouseHook =SetWindowsHookEx( WH_MOUSE_LL,mouseProc,GetModuleHandle(NULL),NULL);//注册鼠标钩子
}
