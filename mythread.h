#ifndef MYTHREAD_H
#define MYTHREAD_H

#include <QThread>
#include <QWidget>

class MyThread : public QThread
{
    Q_OBJECT
private:
    void run();

};

#endif // MYTHREAD_H
