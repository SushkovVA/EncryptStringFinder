#ifndef ENCRYPTSTRINGFINDER_H
#define ENCRYPTSTRINGFINDER_H

#include <QObject>

class DataHolder;
class ThreadsManager;

class EncryptStringFinder : public QObject
{
    Q_OBJECT

private:
    DataHolder* dataBank;
    ThreadsManager* workersManager;

public:
    EncryptStringFinder(const QVector<char> symbols, uint strokeLength, uint memoryLimitMb, QByteArray (*changeData)(const std::string&), bool (*isValidValue)(const QByteArray&), QObject* parent = nullptr);

public slots:
    void start();

signals:
    void signalSolutionFinded(QString solution);
};

#endif // ENCRYPTSTRINGFINDER_H
