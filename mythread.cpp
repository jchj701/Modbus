#include "mythread.h"
#include <QThread>
#include <QDebug>
#include "mainwindow.h"
#include <synchapi.h>
bool MainWindow::flagRecive = false;

ThreadObject ::ThreadObject (QObject *parent):QObject(parent)
{

}

ThreadObject::~ThreadObject ()
{
    qDebug() << "MyThread destroy";
    MainWindow::flagRecive = false;
}

void ThreadObject::runSomeBigWork1()
{
    QMutexLocker locker(&m_stopMutex);
    int i = 0;
    while(MainWindow::flagRecive)
    {
//        emit progress ();

        if(i++ == 100)
        {
            //emit信号，槽函数接收数据
            emit progress ();
            i = 0;
            Sleep(10);
        }
    }
}
