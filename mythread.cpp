#include "mythread.h"
#include <QThread>
#include <QDebug>
#include <QModbusDataUnit>
#include "mainwindow.h"
#include <synchapi.h>

bool MainWindow::flagRecive = false;
MyThread::MyThread(QObject *parent) : QObject(parent)
{

}

MyThread::~MyThread ()
{
    qDebug() << "MyThread destroy";
    MainWindow::flagRecive = false;
}

void MyThread::slot_thread_doWork1 ()
{
    while(MainWindow::flagRecive)
    {
        qDebug("in slot_thread_doWork1");
//        analyseData ();

        MainWindow::test1 ();
    }
}
