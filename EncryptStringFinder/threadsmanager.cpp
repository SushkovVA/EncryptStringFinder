#include "threadsmanager.h"
#include "threadworker.h"

#include <QThread>
#include <QDebug>

ThreadsManager::ThreadsManager(QByteArray (*changeData)(const std::string&), bool (*isValidValue)(const QByteArray&),QObject *parent) : QObject(parent),
    MAX_WORKERS_COUNT(QThread::idealThreadCount()),
    currentData(nullptr),
    readyWorkersCount(0),
    solutionFindedFlag(false)

{
    initializeWorkers(changeData, isValidValue);
}

ThreadsManager::~ThreadsManager()
{
    if (!workers.isEmpty())
    {
        for (auto worker: workers)
        {
            delete worker;
        }
    }
}

void ThreadsManager::initializeWorkers(QByteArray (*changeData)(const std::string&), bool (*isValidValue)(const QByteArray&))
{
    for (int i = 0; i < MAX_WORKERS_COUNT; i++)
    {
        //                                          this в данном случае не указатель на parent (т.к. нельзя переместить в другой поток объект с родителем)
        //                                           |   а указатель на менеджера (для возвращения в его(менеджера) поток)
        ThreadWorker * newWorker = new ThreadWorker(this);
        newWorker->setInstruments(changeData, isValidValue);
        connect(newWorker, SIGNAL(signalWorkFinished(int)), this, SLOT(workerDone(int)));
        connect(newWorker, SIGNAL(signalSolutionFinded(QString)), this, SLOT(workerFindSolution(QString)));
        workers.push_back(newWorker);
    }
}

void ThreadsManager::startWorkers()
{
    timer.start();
    readyWorkersCount = 0;
    int currentStartIndex = 0;
    const int dataSize = currentData->size();
    const int dataChunkSize = dataSize/MAX_WORKERS_COUNT;
    int currentStopIndex = dataChunkSize;
    for (int i = 0; i < MAX_WORKERS_COUNT; i++)
    {
        // Для последнего работника индекс остановки указываем напрямую,
        // чтобы избежать проблем неровного деления (ему достанется чуть больший кусок)
        if (i == MAX_WORKERS_COUNT - 1)
        {
           currentStopIndex = dataSize - 1;
        }

        workers[i]->setData(currentData, currentStartIndex, currentStopIndex);

        QThread* thread = new QThread(this);
        workers[i]->moveToThread(thread);

        connect(thread, SIGNAL(started()), workers[i], SLOT(startWork()));
        connect(workers[i], SIGNAL(signalWorkFinished(int)), thread, SLOT(quit()));
        connect(thread, SIGNAL(finished()), thread, SLOT(deleteLater()));

        thread->start();

        currentStartIndex = currentStopIndex;
        currentStopIndex += dataChunkSize;
    }
}

void ThreadsManager::dataArrived(QVector<std::string>* newData)
{
    if (currentData != nullptr)
        unprocessedData.push_back(newData);
    else
    {
        currentData = newData;
        startWorkers();
    }
}

void ThreadsManager::workerDone(int workerId)
{
    qDebug() << "Worker #"<<workerId<<" finish work!";
    qDebug()<<"Time Passed: "<<timer.elapsed()/1000.<<" sec";

    readyWorkersCount++;
    if (readyWorkersCount == MAX_WORKERS_COUNT)
    {
        QVector<std::string>* pData = currentData;
        currentData = nullptr;
        emit signalDataProcessed(pData, !solutionFindedFlag);

        if (!unprocessedData.isEmpty())
        {
            currentData = unprocessedData.last();
            unprocessedData.removeLast();
            startWorkers();
        }
    }
}

void ThreadsManager::workerFindSolution(QString solutionData)
{
    solutionFindedFlag = true;
    emit signalSolutionFinded(solutionData);

    qDebug() << "SOLUTION FINDED! ANSWER IS: "<<solutionData;
    qDebug() << "SOLUTION Time Passed: "<<timer.elapsed()/1000.<<" sec";
}
