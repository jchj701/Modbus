#include "mythread.h"
#include <QThread>
#include <QDebug>
#include "mainwindow.h"

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
    qDebug() << "in run1";
//    emit progress ();
    int i = 0;
    while(MainWindow::flagRecive)
    {
        qDebug() << "in run";
        if(i++ == 500)
        {
            emit progress ();
            i = 0;
        }
    }

}
