#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QModbusTcpClient>
#include <QModbusRtuSerialMaster>
#include <QStandardItemModel>
#include <QStatusBar>
#include <QUrl>
#include <QDebug>
#include <QDateTime>
#include <QTimer>
#include <QSerialPort>

#include "mythread.h"

enum ModbusConnection {
    Serial,
    Tcp
};

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    modbusDevice(nullptr)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    if(modbusDevice)
    {
        modbusDevice->disconnectDevice ();
    }
    delete modbusDevice;
    delete ui;
}

void MainWindow::on_pushButton_init_clicked()
{
    //TCP modbus 初始化
    modbusDevice = new QModbusTcpClient(this);
//    timer = new QTimer(this);

    m_objThread= new QThread();
    m_obj = new ThreadObject();
    m_obj->moveToThread(m_objThread);

    if (ui->lineEdit_port->text().isEmpty())
    {
        ui->lineEdit_port->setText(QLatin1Literal("127.0.0.1:502"));
    }

    connect(modbusDevice, &QModbusClient::errorOccurred, [this](QModbusDevice::Error) {
        statusBar()->showMessage(modbusDevice->errorString(), 5000);
    });

    if(!modbusDevice)
    {
        qDebug() << "!modbusDevice";
        return;
    }

    statusBar ()->clearMessage ();
    if(modbusDevice->state () != QModbusDevice::ConnectedState)
    {
        const QUrl url = QUrl::fromUserInput(ui->lineEdit_port->text());
        qDebug() << "url.port = " << url.port() ;
        qDebug() << "url.host = " << url.host() ;
        modbusDevice->setConnectionParameter(QModbusDevice::NetworkPortParameter, url.port());
        modbusDevice->setConnectionParameter(QModbusDevice::NetworkAddressParameter, url.host());
        modbusDevice->setTimeout(1000);
        modbusDevice->setNumberOfRetries(3);


        modbusDevice->setConnectionParameter(QModbusDevice::SerialParityParameter, QSerialPort::MarkParity);
        modbusDevice->setConnectionParameter(QModbusDevice::SerialBaudRateParameter, QSerialPort::Baud115200);
        modbusDevice->setConnectionParameter(QModbusDevice::SerialDataBitsParameter, QSerialPort::Data8);
        modbusDevice->setConnectionParameter(QModbusDevice::SerialStopBitsParameter, QSerialPort::TwoStop);

//        connect(timer, SIGNAL(timeout()), this, SLOT(on_pushButton_start_clicked()));

        qDebug() << "connectDevice()";
        if (!modbusDevice->connectDevice()) {
            statusBar()->showMessage(tr("Connect failed: ") + modbusDevice->errorString(), 5000);
        }
        else {
            connect(modbusDevice, &QModbusClient::stateChanged,
                    this, &MainWindow::onStateChanged);
        }
    }
    else {
            modbusDevice->disconnectDevice();
            qDebug() << "disconnectDevice()";
    }
}

void MainWindow::onStateChanged(int state)
{
//    bool connected = (state != QModbusDevice::UnconnectedState);
//    ui->actionConnect->setEnabled(!connected);
//    ui->actionDisconnect->setEnabled(connected);

    if (state == QModbusDevice::ConnectedState)
        ui->conect_info->setText(tr("Connect Now"));
    else if (state == QModbusDevice::UnconnectedState)
        ui->conect_info->setText(tr("Disconnect Now"));
}

void MainWindow::on_pushButton_start_clicked()
{
    if(!modbusDevice)
    {
        qDebug() << "!modbusDevice";
        return;
    }


    connect(m_objThread,&QThread::finished,m_objThread,&QObject::deleteLater);
    connect(m_objThread,&QThread::finished,m_obj,&QObject::deleteLater);

    connect(this,&MainWindow::startObjThreadWork1, m_obj, &ThreadObject::runSomeBigWork1);
    connect(m_obj,&ThreadObject::progress, this, &MainWindow::showData);


    m_objThread->start();
    flagRecive = true;
    emit startObjThreadWork1();
//    m_objThread->wait ();
    //timer->start(10);


#if 0
    //不能使用while(1)
    while(1)
    {
        QModbusDataUnit readUnit(QModbusDataUnit::HoldingRegisters,0,10);
         if (auto *reply = modbusDevice->sendReadRequest(readUnit, 1))//0的话错误0x1
        {
            qDebug() << "in reply";
            if (!reply->isFinished())
                connect(reply, &QModbusReply::finished, this, &MainWindow::readReady);
            else
                delete reply; // broadcast replies return immediately
        }
        else {
            statusBar()->showMessage(tr("Read error: ") + modbusDevice->errorString(), 5000);
        }
    }


#endif
}

void MainWindow::showData()
{
    statusBar ()->clearMessage ();
    qDebug() << "in showData";
#if 1
    QModbusDataUnit readUnit(QModbusDataUnit::HoldingRegisters,0,10);
//    if(auto *reply = modbusDevice->sendReadRequest (readUnit, ui->serverEdit->value ()))
//    if(auto *reply = modbusDevice->sendReadRequest (readRequest(), ui->serverEdit->value ()))
     if (auto *reply = modbusDevice->sendReadRequest(readUnit, 1))//0的话错误0x1
    {
        qDebug() << "in reply";
        if (!reply->isFinished())
            connect(reply, &QModbusReply::finished, this, &MainWindow::readReady);
        else
            delete reply; // broadcast replies return immediately
    }
    else {
        statusBar()->showMessage(tr("Read error: ") + modbusDevice->errorString(), 5000);
    }

#endif
}


void MainWindow::readReady()
{
    auto reply = qobject_cast<QModbusReply *>(sender());
    if (!reply)
        return;
    qDebug() << "in readReady()";

    if (reply->error() == QModbusDevice::NoError) {
           const QModbusDataUnit unit = reply->result();
#if 0
           for (uint i = 0; i < unit.valueCount(); i++) {
               const QString entry = tr("Address: %1, Value: %2").arg(unit.startAddress())
                                        .arg(QString::number(unit.value(i),
                                             unit.registerType() <= QModbusDataUnit::Coils ? 10 : 16));
               qDebug() << "no error additem";
               ui->listWidget_recive->addItem(entry);
           }
#endif
           QDateTime currentDateTime =QDateTime::currentDateTime();
           QString currentDate =currentDateTime.toString("hh:mm:ss.zzz");
           const QString entry = tr("%1 - result:%2 %3 %4 %5").arg (currentDate)
                                 .arg (unit.value(0))
                                 .arg (unit.value(1))
                                 .arg (unit.value(2))
                                 .arg (unit.value(3));
          // ui->listWidget_recive->insertPlainText (entry);

           //ui->listWidget_recive->insertItem (listRow, entry);
           //ui->listWidget_recive->addItem (entry);
           ui->listWidget_recive->append (entry);
           m_objThread->quit ();
           //qDebug() << listRow;
           //listRow++;



       } else if (reply->error() == QModbusDevice::ProtocolError) {
           statusBar()->showMessage(tr("Read response error: %1 (Mobus exception: 0x%2)").
                                       arg(reply->errorString()).
                                       arg(reply->rawResult().exceptionCode(), -1, 16), 5000);
       } else {
           statusBar()->showMessage(tr("Read response error: %1 (code: 0x%2)").
                                       arg(reply->errorString()).
                                       arg(reply->error(), -1, 16), 5000);
       }
    reply->deleteLater();
}


