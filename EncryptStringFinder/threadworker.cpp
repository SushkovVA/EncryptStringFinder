#include "threadworker.h"

#include <QDebug>

int ThreadWorker::workersIdCount = 0;

ThreadWorker::ThreadWorker(QObject *manager, QObject *parent) :
    QObject(parent),
    pManager(manager),
    stopFlag(false)
{
    workerId = workersIdCount;
    workersIdCount++;
}

int ThreadWorker::getId() const
{
    return workerId;
}

void ThreadWorker::setInstruments(QByteArray (*_change)(const std::string &), bool (*_isValid)(const QByteArray&))
{
    change = _change;
    isValid = _isValid;
}

void ThreadWorker::setData(QVector<std::string>* _dataArray, const int &_startIndex, const int &_stopIndex)
{
    dataArray = _dataArray;
    startIndex = _startIndex;
    stopIndex = _stopIndex;
}

void ThreadWorker::startWork()
{
    stopFlag = false;

    for (int i = startIndex; i < stopIndex; i++)
    {
        if (stopFlag)
            break;

        if ( isValid( change((*dataArray)[i]) ) )
        {
            qDebug() << "Me, Worker #"<<workerId<<", found a SOLUTION! Pleasure to serve.";
            emit signalSolutionFinded( QString( (*dataArray)[i].data() ) );
            break;
        }
    }

    moveToThread(pManager->thread());
    emit signalWorkFinished(workerId);
}

void ThreadWorker::setStopFlag()
{
    stopFlag = true;
}

