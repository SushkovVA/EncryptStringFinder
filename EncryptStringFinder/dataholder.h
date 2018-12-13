#ifndef DATAHOLDER_H
#define DATAHOLDER_H

#include <QObject>
#include <QVector>

struct GeneratingData
{
    GeneratingData(QVector<char> _symbols, uint _length)
    {
        symbols = _symbols;
        length = _length;

        sequence.resize(length);
        for (uint i = 0; i < length; i++)
            sequence[i] = symbols[0];

        position = length - 1;

    }

    void set(QVector<char> _symbols, std::string _sequence, uint _length, uint _position)
    {
        symbols = _symbols;
        sequence = _sequence;
        length = _length;
        position = _position;
    }

    QVector<char> symbols;
    std::string sequence;
    uint length;
    uint position;
};

class DataHolder : public QObject
{
    Q_OBJECT
private:
    QVector<std::string>* dataArray;
    uint memoryLimit;
    bool memoryFlag;
    int dataSizeLimit;
    GeneratingData* generatingDataStorage;

    int recursiveSymbolsExchange(std::string& sequence, const QVector<char>& symbols, const uint position, const uint lastPosition, QVector<std::string>& outArray);
    void getAllSequenceOfSymbols(GeneratingData* generatingDataStorage);
    void continueDataGenerating();
    void checkDataSize();

public:
    explicit DataHolder(const QVector<char> symbols, uint strokeLength, uint megabytes, QObject *parent = nullptr);
    ~DataHolder();

    void saveCurrentDataArrayToFile(const QString& path);
    void setMemoryLimit(uint megabytes);

public slots:
    void generateData(const QVector<char>& symbols, const uint length);
    void generateData();
    void dataProcessed(QVector<std::string>* dataPointer, bool sendNext);

signals:
    void signalDataSend(QVector<std::string>* newData);

};

#endif // DATAHOLDER_H
