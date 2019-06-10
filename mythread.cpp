#include "mythread.h"
#include <QThread>
#include <QDebug>
#include "mainwindow.h"

bool MainWindow::flagRecive = false;
void MyThread::run()
{
    while(MainWindow::flagRecive)
    {
        qDebug() << "in run";

    }

}
