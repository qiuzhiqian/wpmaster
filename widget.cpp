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
#include <QFile>
#include <QByteArray>
#include <QPixmap>

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonValue>

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
            //刷新壁纸
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

    qDebug()<<"width:"<<ui->lb_cover->width();
    //ui->lb_cover->setFixedHeight(ui->lb_cover->width());

    connect(ui->btn_openback,SIGNAL(clicked(bool)),this,SLOT(slt_openBackFile()));
    connect(ui->btn_openfront,SIGNAL(clicked(bool)),this,SLOT(slt_openFrontFile()));
    connect(ui->sld_alpha,SIGNAL(valueChanged(int)),this,SLOT(slt_alphaChange(int)));
    connect(ui->btn_save,SIGNAL(clicked(bool)),this,SLOT(slt_save()));
    connect(ui->btn_load,SIGNAL(clicked(bool)),this,SLOT(slt_load()));
    connect(ui->btn_make,SIGNAL(clicked(bool)),this,SLOT(slt_make()));

    connect(ui->btn_close,SIGNAL(clicked(bool)),this,SLOT(slt_windowClose()));
    connect(ui->btn_min,SIGNAL(clicked(bool)),this,SLOT(slt_windowMin()));
    //qDebug() << "min:" << ui->sld_alpha->minimum();
    //qDebug() << "max:" << ui->sld_alpha->maximum();

    m_maskTimer = new QTimer(this);

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

    qDebug()<<"register hook";
    mouseHook =SetWindowsHookEx( WH_MOUSE_LL,mouseProc,GetModuleHandle(NULL),NULL);//注册鼠标钩子
}

Widget::~Widget()
{
    delete ui;
}

void Widget::slt_openBackFile(){
    m_imagePath = QFileDialog::getOpenFileName(this,tr("Open File"),QDir::currentPath(),tr("Images (*.png *.jpg)"));
    m_mask->setBack(QImage(m_imagePath).convertToFormat(QImage::Format_ARGB32));
    //m_mask->update();
}

void Widget::slt_openFrontFile(){
    m_imagePath = QFileDialog::getOpenFileName(this,tr("Open File"),QDir::currentPath(),tr("Images (*.png *.jpg)"));
    m_mask->setFront(QImage(m_imagePath).convertToFormat(QImage::Format_ARGB32));
    //m_mask->update();
}

void Widget::slt_alphaChange(int value){
    //m_mask->update();
    int alpha = value*255/100;
    m_mask->setAlpha(alpha);
}

void Widget::slt_save(){
}

void Widget::slt_make(){
    QString dirpath = QFileDialog::getExistingDirectory(this, tr("Open Directory"),
                                                      QDir::currentPath(),
                                                      QFileDialog::ShowDirsOnly
                                                      | QFileDialog::DontResolveSymlinks);

    qDebug()<<"path:"<<dirpath;

    if(m_thread!=nullptr || m_maker!=nullptr){
        return;
    }
    m_thread = new QThread(this);
    m_maker = new CMaker(dirpath);

    m_maker->moveToThread(m_thread);
    connect(m_thread,SIGNAL(started()),m_maker,SLOT(start()));
    connect(m_maker,SIGNAL(end()),m_thread,SLOT(quit()));
    connect(m_thread,SIGNAL(finished()),this,SLOT(slt_threadDestory()));

    m_thread->start();
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

void Widget::slt_load(){
    QString dirpath = QFileDialog::getExistingDirectory(this, tr("Open Directory"),
                                                      QDir::currentPath(),
                                                      QFileDialog::ShowDirsOnly
                                                      | QFileDialog::DontResolveSymlinks);
    QFile file(dirpath+"/package.json");
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
    QJsonValue val = obj.value("title");
    if(!val.isString()){
        return;
    }
    ui->lb_titleContent->setText(val.toString());

    val = obj.value("auther");
    if(!val.isString()){
        return;
    }
    ui->lb_autherContent->setText(val.toString());

    val = obj.value("email");
    if(!val.isString()){
        return;
    }
    ui->lb_emailContent->setText(val.toString());

    val = obj.value("cover");
    if(!val.isString()){
        return;
    }
    QPixmap cover(dirpath+"/"+val.toString());
    ui->lb_cover->setPixmap(cover.scaled(ui->lb_cover->width(),ui->lb_cover->width()));
    //ui->lb_emailContent->setText(val.toString());
}

void Widget::slt_threadDestory(){
    m_thread->deleteLater();
    //m_thread = nullptr;
    //qDebug()<<"delete thread";

    if(m_maker!=nullptr){
        delete m_maker;
        m_maker = nullptr;
    }
    qDebug()<<"end";
}

void Widget::mousePressEvent(QMouseEvent *event){
    m_lastPos = event->globalPos();
    QWidget::mousePressEvent(event);
}

void Widget::mouseMoveEvent(QMouseEvent *event){
    move(this->pos()+(event->globalPos()-m_lastPos));
    m_lastPos = event->globalPos();
    QWidget::mouseMoveEvent(event);
}
