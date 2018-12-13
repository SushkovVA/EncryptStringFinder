#ifndef THREADSMANAGER_H
#define THREADSMANAGER_H

#include <QObject>
#include <QVector>
#include <QMap>
#include <QTime>

class ThreadWorker;
class DataHolder;

class ThreadsManager : public QObject
{
    Q_OBJECT

private:
    const int MAX_WORKERS_COUNT;

    QVector<ThreadWorker*> workers;
    QVector<QVector<std::string>*> unprocessedData;
    QVector<std::string>* currentData;
    int readyWorkersCount;
    bool solutionFindedFlag;

    // ОТЛАДКА
    QTime timer;

    // передаем работникам инструменты для обработки массивов
    void initializeWorkers(QByteArray (*change)(const std::string&), bool (*isValid)(const QByteArray&));

public:
    explicit ThreadsManager(QByteArray (*change)(const std::string&), bool (*isValid)(const QByteArray&), QObject *parent = nullptr);
    ~ThreadsManager();

    void startWorkers();

public slots:
    void dataArrived(QVector<std::string>* newData);
    void workerFindSolution(QString solutionData);

private slots:
    void workerDone(int workerId);

signals:
    void signalDataProcessed(QVector<std::string>* dataPointer, bool sendNext);
    void signalSolutionFinded(QString solution);

};

#endif // THREADSMANAGER_H
