#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "mythread.h"

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
    void test();
    static bool flagRecive;
    static void test1();

signals:
    void startThread();
    void sig_analyseData();
    void sig_readReady();

private slots:
    void on_pushButton_init_clicked();
    void onStateChanged(int state);
    void on_pushButton_start_clicked();
    void on_pushButton_stop_clicked();
    void on_pushButton_destroy_clicked();
    void on_comboBox_chooseMode_currentIndexChanged(int index);


private:
    Ui::MainWindow *ui;
    QModbusReply *lastRequest;
    QModbusClient *modbusDevice;

    QTimer *timer;

    MyThread *m_thread;
    QThread *m_qThread;

    int listRow = 0;
    void analyseData();
    void readReady();
    void showFrequence();
};

#endif // MAINWINDOW_H
