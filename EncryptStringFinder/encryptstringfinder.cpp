#include "encryptstringfinder.h"
#include "dataholder.h"
#include "threadsmanager.h"

#include <QVector>


EncryptStringFinder::EncryptStringFinder(const QVector<char> symbols, uint strokeLength, uint memoryLimitMb,
                                         QByteArray (*changeData)(const std::string &), bool (*isValidValue)(const QByteArray &), QObject *parent) :
    QObject(parent)
{
    dataBank = new DataHolder(symbols, strokeLength, memoryLimitMb, this);
    workersManager = new ThreadsManager(changeData, isValidValue);

    QObject::connect(dataBank, SIGNAL(signalDataSend(QVector<std::string>*)), workersManager, SLOT(dataArrived(QVector<std::string>*)));
    QObject::connect(workersManager, SIGNAL(signalDataProcessed(QVector<std::string>*, bool)), dataBank, SLOT(dataProcessed(QVector<std::string>*, bool)));
    QObject::connect(workersManager, SIGNAL(signalSolutionFinded(QString)), this, SIGNAL(signalSolutionFinded(QString)));
}

void EncryptStringFinder::start()
{
    dataBank->generateData();
}
