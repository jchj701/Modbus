#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QModbusDataUnit>
#include "mythread.h"

QT_BEGIN_NAMESPACE

class QModbusClient;
class QModbusReply;

namespace Ui {
    class MainWindow;
    }

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    static bool flagRecive;

private slots:
    void on_pushButton_init_clicked();

    void onStateChanged(int state);
    void on_pushButton_start_clicked();

private:
    Ui::MainWindow *ui;
    QModbusReply *lastRequest;
    QModbusClient *modbusDevice;
    QTimer *timer;

    int listRow = 0;

    QModbusDataUnit readRequest() const;
    void readReady();

    MyThread m_thread;

};

#endif // MAINWINDOW_H
