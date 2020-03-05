#include "cmaker.h"

#include <QDebug>
#include <QDir>
#include <QFileInfo>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonValue>
#include <QImage>
#include "cutils.h"

CMaker::CMaker(const QString &path, QObject *parent) : QObject(parent),
    m_path(path)
{

}

void CMaker::start(){
    //设置要遍历的目录
    //QDir dir(dirpath);
    //设置文件过滤器
    //QStringList nameFilters;
    //设置文件过滤格式
    //nameFilters << "*.png";
    //将过滤后的文件名称存入到files列表中
    //QStringList files = dir.entryList(nameFilters, QDir::Files|QDir::Readable, QDir::Name);
    //files.sort();
    //qDebug()<<"file:"<<files;
    //Load config
    QFile file(m_path+"/config.json");
    bool ok = file.open(QIODevice::ReadOnly);
    if(!ok){
        emit end();
        return;
    }

    QByteArray ba = file.readAll();
    file.close();

    QJsonParseError err;
    QJsonDocument doc = QJsonDocument::fromJson(ba,&err);
    if(doc.isNull()){
        qDebug()<<"err:"<<err.errorString();
        emit end();
        return;
    }

    if(!doc.isObject()){
        return;
    }

    QJsonObject obj = doc.object();
    QJsonValue val = obj.value("interval");
    if(!val.isDouble()){
        emit end();
        return;
    }

    QStringList picList;
    QJsonValue listval = obj.value("list");
    if(!listval.isArray()){
        emit end();
        return;
    }
    QJsonArray jsonarray = listval.toArray();
    for(auto item : jsonarray){
        if(!item.isString()){
            emit end();
            return;
        }

        picList.append(item.toString());
    }
    qDebug()<<picList;

    int i=0;
    QImage before(m_path+"/"+picList.at(0));
    qDebug()<<"last:"<<m_path+"/"+picList.at(0);
    for(auto item:picList){
        //qDebug()<<"i:"<<i <<" file:"<<item;
        if(i>0){
            QFileInfo fileinfo(m_path+"/"+item);
            qDebug()<<"info:"<<m_path+"/"+item;
            QImage end(m_path+"/"+item);
            QImage tmpImage = CUtils::imageDiff(end,before);
            QString saveName = fileinfo.absolutePath()+"/"+fileinfo.baseName()+"_diff."+fileinfo.completeSuffix();
            qDebug()<<"saveName:"<<saveName;
            tmpImage.save(saveName,"PNG");
            before = end;
        }
        i++;
    }

    emit end();
}
