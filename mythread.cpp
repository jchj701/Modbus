#include "mythread.h"
#include <QThread>
#include <QDebug>
#include "mainwindow.h"
#include <synchapi.h>
bool MainWindow::flagRecive = false;

ThreadObject ::ThreadObject (QObject *parent):QObject(parent)
  ,m_runCount(100)
  ,m_runCount2(std::numeric_limits<int>::max())
  ,m_isStop(true)
{

}

ThreadObject::~ThreadObject ()
{
    qDebug() << "MyThread destroy";
    MainWindow::flagRecive = false;
}

void ThreadObject::runSomeBigWork1()
{
//    qDebug() << "in run1";
//    emit progress ();
    int i = 0;
    while(MainWindow::flagRecive)
    {
        emit progress ();
//        qDebug() << "in run";
        if(i++ == 100)
        {
//            emit progress ();
            i = 0;
            Sleep(500);
        }
    }

}
