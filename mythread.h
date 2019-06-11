#ifndef MYTHREAD_H
#define MYTHREAD_H

#include <QThread>
#include <QWidget>
#include <QObject>
#include <QMutex>

class ThreadObject : public QObject
{
    Q_OBJECT

public:
    explicit ThreadObject (QObject* parent = nullptr);
    ~ThreadObject ();

signals:
    void progress();
public slots:
    void runSomeBigWork1();

};

#endif // MYTHREAD_H
