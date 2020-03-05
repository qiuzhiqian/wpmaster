#ifndef CMAKER_H
#define CMAKER_H

#include <QObject>

class CMaker : public QObject
{
    Q_OBJECT
public:
    explicit CMaker(const QString& path="",QObject *parent = nullptr);

public slots:
    void start(void);

private:
    QString m_path;
signals:
    void end();
};

#endif // CMAKER_H
