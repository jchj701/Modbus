#ifndef MYTHREAD_H
#define MYTHREAD_H

#include <QThread>
#include <QWidget>
#include <QObject>
#include <QMutex>

class MyThread : public QObject
{
    Q_OBJECT
public:
    explicit MyThread(QObject *parent = nullptr);
    ~MyThread ();
signals:
    void sig_thread_start();
public slots:
    void slot_thread_doWork1();
};

#endif // MYTHREAD_H
