#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "mythread.h"

#include <QModbusTcpClient>
#include <QModbusRtuSerialMaster>
#include <QStandardItemModel>
#include <QStatusBar>
#include <QUrl>
#include <QDebug>
#include <QDateTime>
#include <QTimer>
#include <QSerialPort>

enum ModbusConnection {
    Tcp,
    Serial
};//通信类型枚举

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    modbusDevice(nullptr)
{
    ui->setupUi(this);

    //默认TCP
    modbusDevice = new QModbusTcpClient(this);

    timer = new QTimer(this);

    m_objThread= new QThread();
    m_obj = new ThreadObject();
    m_obj->moveToThread(m_objThread);
}//构造函数

MainWindow::~MainWindow()
{
    if(modbusDevice)
    {
        modbusDevice->disconnectDevice ();
    }
    delete modbusDevice;
    delete ui;
    delete timer;
    m_objThread->quit ();
    m_objThread->wait ();
    qDebug() << "~MainWindow()";
}

/*
 * pushButton
 */
void MainWindow::on_pushButton_init_clicked()
{

    if(!modbusDevice)
    {
        //初始化失败
        qDebug() << "modbusDevice init fault";
        return;
    }

    ui->pushButton_init->setEnabled (false);
    ui->pushButton_destroy->setEnabled (true);
    ui->pushButton_start->setEnabled (true);
    ui->pushButton_stop->setEnabled (false);

    //设置默认地址
    if (ui->lineEdit_port->text().isEmpty())
    {
        ui->lineEdit_port->setText(QLatin1Literal("127.0.0.1:502"));
    }

    connect(modbusDevice, &QModbusClient::errorOccurred, [this](QModbusDevice::Error) {
        statusBar()->showMessage(modbusDevice->errorString(), 5000);
    });

    statusBar ()->clearMessage ();
    if(modbusDevice->state () != QModbusDevice::ConnectedState)
    {
        if(static_cast<ModbusConnection> (ui->comboBox_chooseMode->currentIndex()) == Tcp)
        {
            const QUrl url = QUrl::fromUserInput(ui->lineEdit_port->text());
            ui->listWidget_log->addItem (tr("端口号：%1").arg(url.port ()));
            ui->listWidget_log->addItem (tr("地址：%1").arg(url.host ()));
            modbusDevice->setConnectionParameter(QModbusDevice::NetworkPortParameter, url.port());
            modbusDevice->setConnectionParameter(QModbusDevice::NetworkAddressParameter, url.host());
            modbusDevice->setTimeout(1000);
            modbusDevice->setNumberOfRetries(3);
        }
        else
        {
            //设置通信参数
            modbusDevice->setConnectionParameter(QModbusDevice::SerialParityParameter, QSerialPort::EvenParity);
            modbusDevice->setConnectionParameter(QModbusDevice::SerialBaudRateParameter, QSerialPort::Baud115200);//Baud1200
            modbusDevice->setConnectionParameter(QModbusDevice::SerialDataBitsParameter, QSerialPort::Data8);
            modbusDevice->setConnectionParameter(QModbusDevice::SerialStopBitsParameter, QSerialPort::OneStop);
        }

        //销毁线程
        connect(m_objThread,&QThread::finished,m_objThread,&QObject::deleteLater);
        connect(m_objThread,&QThread::finished,m_obj,&QObject::deleteLater);

        //启用线程任务 runSomeBigWork1
        connect(this,&MainWindow::startObjThreadWork1, m_obj, &ThreadObject::runSomeBigWork1);

        //收到runSomeBigWork1中的信号，启用接收函数 showData，已调整位置至start按钮
//        connect(m_obj,&ThreadObject::progress, this, &MainWindow::showData);

        //启用记频率定时器
        connect(timer, SIGNAL(timeout()), this, SLOT(showFrequence()));

        //连接状态显示
        connect(modbusDevice, &QModbusClient::stateChanged, this, &MainWindow::onStateChanged);

        if (!modbusDevice->connectDevice()) {
            statusBar()->showMessage(tr("Connect failed: ") + modbusDevice->errorString(), 5000);
        }
    }
    else {
        //处理连接时断开的操作
        modbusDevice->disconnectDevice();
        qDebug() << "disconnectDevice()";
        ui->listWidget_log->addItem (tr("disconnect Device"));
    }
}//初始化按钮，根据不同模式启用不同设置，启用各种信号槽


void MainWindow::on_pushButton_start_clicked()
{
    if(!modbusDevice)
    {
        qDebug() << "!modbusDevice";
        return;
    }

    ui->pushButton_init->setEnabled (false);
    ui->pushButton_destroy->setEnabled (true);
    ui->pushButton_start->setEnabled (false);
    ui->pushButton_stop->setEnabled (true);

    m_objThread->start();

    flagRecive = true;

    timer->start(1000);

    //启用接收事件
    connect(m_obj,&ThreadObject::progress, this, &MainWindow::showData);

    emit startObjThreadWork1();

}//开启线程、定时器、接收flag，emit信号

void MainWindow::on_pushButton_stop_clicked()
{
    ui->pushButton_init->setEnabled (false);
    ui->pushButton_destroy->setEnabled (true);
    ui->pushButton_start->setEnabled (true);
    ui->pushButton_stop->setEnabled (false);

    disconnect(m_obj,&ThreadObject::progress, this, &MainWindow::showData);

    flagRecive = false;
    timer->stop ();
//    m_objThread->quit ();
//    m_objThread->wait ();
}//停止按钮

void MainWindow::on_pushButton_destroy_clicked()
{
    //避免未使用stop直接destroy
    on_pushButton_stop_clicked ();

    //断开连接
    modbusDevice->disconnectDevice();

    ui->pushButton_destroy->setEnabled (false);
    ui->pushButton_init->setEnabled (true);
    ui->pushButton_start->setEnabled (false);

    ui->listWidget_log->clear ();
}//destroy按钮，断开连接

/*
 * function
 */
void MainWindow::onStateChanged(int state)
{
    qDebug() << "in onStateChanged";
    if (state == QModbusDevice::ConnectedState)
    {
        ui->conect_info->setText(tr("Connect Now"));
        ui->listWidget_log->addItem (tr("Connect Now"));
    }
    else if (state == QModbusDevice::UnconnectedState)
    {
        ui->conect_info->setText(tr("Disconnect Now"));
        ui->listWidget_log->addItem (tr("Disconnect Now"));
        if(ui->radioButton->isChecked ())
        {
            //开始重连
            qDebug() << "in is check reconnect";
            ui->listWidget_log->addItem (tr("reconnect in  onStateChanged"));
            qDebug() << modbusDevice->state ();
            ui->radioButton->setChecked (false);
            on_pushButton_init_clicked ();
            ui->pushButton_init->setEnabled (false);
            ui->pushButton_destroy->setEnabled (true);
            ui->pushButton_start->setEnabled (false);
            ui->pushButton_stop->setEnabled (true);
        }
    }
}//更改连接状态

void MainWindow::showData()
{
    statusBar ()->clearMessage ();
//    qDebug() << "in showData";

#if 1
    //HoldingRegister 类型
    QModbusDataUnit readUnit(QModbusDataUnit::HoldingRegisters,0,10);

    if (auto *reply = modbusDevice->sendReadRequest(readUnit, 1))//客户端id
    {
//        qDebug() << "in reply";
        if (!reply->isFinished())
            connect(reply, &QModbusReply::finished, this, &MainWindow::readReady);
        else
            delete reply; // broadcast replies return immediately
    }
    else {
        qDebug() << "read error in showdata";
        statusBar()->showMessage(tr("Read error: ") + modbusDevice->errorString(), 5000);
    }
#endif
}//设置客户端，启用信号槽，用来显示接收到的数据

void MainWindow::readReady()
{
    auto reply = qobject_cast<QModbusReply *>(sender());
    if (!reply)
        return;

    if (reply->error() == QModbusDevice::NoError) {
           const QModbusDataUnit unit = reply->result();

#if 1
           QDateTime currentDateTime =QDateTime::currentDateTime();
           QString currentDate =currentDateTime.toString("hh:mm:ss.zzz");

           const QString entry = tr("%1 - result:%2 %3 %4 %5")
                                 .arg (currentDate)
                                 .arg (unit.value(0))
                                 .arg (unit.value(1))
                                 .arg (unit.value(2))
                                 .arg (unit.value(3));

           ui->listWidget_recive->append (entry);

//           qDebug() << "append ok";
           listRow++;

       } else if (reply->error() == QModbusDevice::ProtocolError) {
           qDebug() << "error ProtocolError";
           statusBar()->showMessage(tr("Read response error: %1 (Mobus exception: 0x%2)").
                                       arg(reply->errorString()).
                                       arg(reply->rawResult().exceptionCode(), -1, 16), 5000);
           ui->listWidget_log->addItem (tr("Read response error: %1 (Mobus exception: 0x%2)").
                                        arg(reply->errorString()).
                                        arg(reply->rawResult().exceptionCode(), -1, 16));
       } else {
           qDebug() << "error else";
           statusBar()->showMessage(tr("Read response error: %1 (code: 0x%2)").
                                       arg(reply->errorString()).
                                       arg(reply->error(), -1, 16), 5000);
           ui->listWidget_log->addItem (tr("Read response error: %1 (code: 0x%2)").
                                        arg(reply->errorString()).
                                        arg(reply->error(), -1, 16));
           if(ui->radioButton->isChecked ())
           {
               //重新连接
               on_pushButton_init_clicked ();
               ui->pushButton_init->setEnabled (false);
               ui->pushButton_destroy->setEnabled (true);
               ui->pushButton_start->setEnabled (false);
               ui->pushButton_stop->setEnabled (true);
           }
#endif
       }
    reply->deleteLater();
}//收到信息的处理与显示函数

void MainWindow::showFrequence()
{
    ui->label_frequence->setText (tr("帧频：%1").arg (listRow));
    listRow = 0;
}//显示帧频，定时器显示所使用的间隔为1s，故更新有延迟

void MainWindow::on_comboBox_chooseMode_currentIndexChanged(int index)
{
    if (modbusDevice) {
        modbusDevice->disconnectDevice();
        delete modbusDevice;
        modbusDevice = nullptr;
    }

    auto type = static_cast<ModbusConnection> (index);

    if (type == Tcp) {
            modbusDevice = new QModbusTcpClient(this);
            if (ui->lineEdit_port->text().isEmpty())
                ui->lineEdit_port->setText(QLatin1Literal("127.0.0.1:502"));
            ui->listWidget_log->addItem (tr("TCP mode"));
    }
    else if (type == Serial) {
        modbusDevice = new QModbusRtuSerialMaster(this);
        ui->listWidget_log->addItem (tr("RTU mode"));
    }
}//更改通信模式，TCP于RTU的切换，默认使用TCP

