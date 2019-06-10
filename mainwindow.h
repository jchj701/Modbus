#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "mythread.h"

#include <QMainWindow>
#include <QModbusDataUnit>
#include <QThread>

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
signals:
    void startObjThreadWork1();
private slots:
    void on_pushButton_init_clicked();

    void onStateChanged(int state);
    void on_pushButton_start_clicked();

    void showData();
private:
    Ui::MainWindow *ui;
    QModbusReply *lastRequest;
    QModbusClient *modbusDevice;
    QTimer *timer;

    int listRow = 0;

    QModbusDataUnit readRequest() const;
    void readReady();

    ThreadObject *m_obj;
    QThread *m_objThread;

};



#endif // MAINWINDOW_H
