#ifndef MYTHREAD_H
#define MYTHREAD_H

#include <QThread>
#include <QWidget>
#include <QObject>
#include <QMutex>

class ThreadObject : public QObject
{
    Q_OBJECT

public:
    explicit ThreadObject (QObject* parent = nullptr);
    ~ThreadObject ();
    //void setRunCount(int count);
    //void stop();
signals:
    void progress();
public slots:
    void runSomeBigWork1();
    //void runSomeBigWork2();
private:
    int m_runCount;
    int m_runCount2;
    bool m_isStop;

    QMutex m_stopMutex;
};

#endif // MYTHREAD_H
