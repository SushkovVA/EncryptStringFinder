#ifndef THREADWORKER_H
#define THREADWORKER_H

#include <QObject>

class ThreadWorker : public QObject
{
    Q_OBJECT

private:
    static int workersIdCount;
    int workerId;

    QObject* pManager;

    QVector<std::string>* dataArray;
    int startIndex;
    int stopIndex;

    // Указатель на функцию изменяющую последовательность байтов
    QByteArray (*change)(const std::string&);
    // Указатель на функцию проверяющую получение нужной последовательности
    bool (*isValid)(const QByteArray&);

    bool stopFlag;

public:
    explicit ThreadWorker(QObject *manager, QObject *parent = nullptr);
    int getId() const;

    void setInstruments(QByteArray (*_change)(const std::string&), bool (*_isValid)(const QByteArray&));
    void setData(QVector<std::string>* _dataArray, const int& _startIndex, const int& _stopIndex);
    void setStopFlag();

public slots:
    void startWork();

signals:
    void signalWorkFinished(int);
    void signalSolutionFinded(QString);

};

#endif // THREADWORKER_H
