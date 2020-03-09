#include "widget.h"
#include "ui_widget.h"

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

#ifdef _WIN32
#include "windows.h"
#else
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#endif

#ifdef _WIN32
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
#endif

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
        m_mask->setFixedSize(screenList.at(0)->size());
    }else{
        return;
    }

    qDebug()<<"width:"<<ui->lb_cover->width();
    ui->lb_cover->setFixedHeight(432);

    connect(ui->btn_load,SIGNAL(clicked(bool)),this,SLOT(slt_load()));
    connect(ui->btn_make,SIGNAL(clicked(bool)),this,SLOT(slt_make()));

    connect(ui->btn_close,SIGNAL(clicked(bool)),this,SLOT(slt_windowClose()));
    connect(ui->btn_min,SIGNAL(clicked(bool)),this,SLOT(slt_windowMin()));

    //qDebug() << "winid:" << this->winId();
}

Widget::~Widget()
{
    delete ui;
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

#ifdef _WIN32
    //调用该函数刷新壁纸，使得壁纸恢复
    SystemParametersInfoA(SPI_SETDESKWALLPAPER,0,NULL,SPIF_SENDWININICHANGE);
#endif

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

    val = obj.value("meta");
    if(!val.isObject()){
        return;
    }

    QJsonObject metaObj = val.toObject();
    if(metaObj.isEmpty()){
        return;
    }

    QJsonValue metaBase = metaObj.value("base");
    if(!metaBase.isString()){
        return;
    }

    QJsonValue metaCanvas = metaObj.value("canvas");
    if(!metaCanvas.isString()){
        return;
    }

    QString maskStr = "";
    QJsonValue metaMask = metaObj.value("mask");
    if(metaMask.isString()){
        maskStr = dirpath+"/"+metaMask.toString();
    }

    m_mask->loadPackage(QImage(dirpath+"/"+metaBase.toString()),QImage(dirpath+"/"+metaCanvas.toString()),maskStr);

    setBackGroundWMChild(m_mask);

    m_mask->show();

#ifdef _WIN32
    registerGolbalMouseEvent();
#endif
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

#ifdef _WIN32
void Widget::setBackGroundWMChild(QWidget* widget){
    HWND background = NULL;
    HWND hwnd = ::FindWindowA("progman","Program Manager");
    int retry = 0;
    while(1){
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

        if(background == NULL){
            SendMessageA(hwnd,0x052C,0,0);
            qDebug()<<"send message to create WorkerW";
        }else{
            break;
        }
        if(retry++ > 3){
            return;
        }
    }


    HWND current = (HWND)widget->winId();
    SetParent(current,background);
}

void Widget::registerGolbalMouseEvent(){
    qDebug()<<"register hook";
    mouseHook =SetWindowsHookEx( WH_MOUSE_LL,mouseProc,GetModuleHandle(NULL),NULL);//注册鼠标钩子
}
#else
extern Window Window_With_Name(Display *dpy,Window top, char *name);
void Widget::setBackGroundWMChild(QWidget* widget){
    Q_UNUSED(widget);

    auto display = XOpenDisplay(nullptr);
    auto root_window = DefaultRootWindow(display);
    Window w = Window_With_Name(display,root_window,"桌面");
    qDebug()<<"XID:"<<w;
}

Window Window_With_Name(Display *dpy,Window top, char *name)
{
    Window *children, dummy;
    unsigned int nchildren;
    int i;
    Window w=0;
    XClassHint *class_hint;
    class_hint = XAllocClassHint();
    int ret = 0;

    // find by WM_CLASS(STRING) = "mywindow", "MyWindow"
    if(XGetClassHint(dpy, top, class_hint)){
        if(strstr(class_hint->res_name, name) == class_hint->res_name){
            ret = 1;
        }
        if(!class_hint->res_class){
            XFree(class_hint->res_class);
            XFree(class_hint->res_name);
        }
        if(ret){
            return(top);
        }
    }

    if(!XQueryTree(dpy, top, &dummy, &dummy, &children, &nchildren))
    return(0);

    for(i=0; i<nchildren; i++)
    {
        w = Window_With_Name(dpy, children[i], name);
        if (w)
        break;
    }
    if(children) XFree ((char *)children);
    return(w);
}
#endif
