#include "dataholder.h"

#include <QtMath>
#include <QDebug>
#include <QFile>
#include <QDataStream>

DataHolder::DataHolder(const QVector<char>& symbols, const uint& strokeLength, const uint& megabytes, QObject *parent) :
    QObject(parent),
    dataArray(nullptr),
    memoryLimit(megabytes),
    memoryFlag(false),
    generatingDataStorage(nullptr)
{
    setMemoryLimit(megabytes);
    generatingDataStorage = new GeneratingData(symbols, strokeLength);
}

DataHolder::~DataHolder()
{
    if (dataArray != nullptr)
    {
        delete dataArray;
    }
    if (generatingDataStorage != nullptr)
    {
        delete generatingDataStorage;
    }
}

int DataHolder::recursiveSymbolsExchange(std::string &sequence, const QVector<char> &symbols, const uint position, const uint lastPosition, QVector<std::string> &outArray)
{
    // если находимся на последнем символе последовательности, добавляем в результат (outArray) все варианты символов (symbols) с текущей последовательностью
    if (position == lastPosition)
    {
        // начинаем со второго символа, т.к. первый гарантированно уже добавлен ранее (после сброса позиций на первый символ - про это ниже)
        for (int i = 1; i < symbols.size(); i++)
        {
            sequence[position] = symbols[i];
            outArray.push_back(sequence);
        }
        return static_cast<int>(position-1);
    }
    else
    {
        // если элемент в последовательности равен последнему символу из (symbols), переходим на предыдущую позицию
        if ( position != 0 && sequence[position] == symbols.last() )
            return static_cast<int>(position-1);
        // если элемент равен последнему символу и позиция равна началу последовательности, значит - готово
        else if ( position == 0 && sequence[position] == symbols.last() )
        {
            // Все перестановки сделаны
            return -1;
        }
        else
        {
            // изменяем символ на позиции на следующий 00199 -> 00299 (если: position == 3, символы 0...9)
            sequence[position] = symbols[symbols.indexOf(sequence[position])+1];
            // сбрасываем символы на всех последующих позициях на первый символ 00299 -> 00200
            for(uint i = position+1; i <= lastPosition; i++)
                sequence[i] = symbols[0];

            outArray.push_back(sequence);
            // переходим к последней позиции изменившейся последовательности
            return static_cast<int>(lastPosition);
        }
    }
}

void DataHolder::checkDataSize()
{
    if (dataArray->size() >= dataSizeLimit)
        memoryFlag = true;
}

void DataHolder::getAllSequenceOfSymbols(GeneratingData* generatingDataStorage)
{
    std::string sequence = generatingDataStorage->sequence;
    const QVector<char> &symbols = generatingDataStorage->symbols;
    const uint length = generatingDataStorage->length;
    int nextPositon = static_cast<int>(generatingDataStorage->position);

    while(nextPositon != -1 && !memoryFlag)
    {
        nextPositon = recursiveSymbolsExchange(sequence, symbols, static_cast<uint>(nextPositon), length-1, *dataArray);
        if (memoryLimit)
            checkDataSize();
    }

    if (memoryFlag)
    {
        generatingDataStorage->set(symbols, sequence, length, nextPositon);
    }
}

void DataHolder::continueDataGenerating()
{
    if (generatingDataStorage != nullptr)
    {
        memoryFlag = false;
        dataArray->clear();
        getAllSequenceOfSymbols(generatingDataStorage);

        qDebug() << "NEXT PART Data Generated! Size is: "<<dataArray->size();
        qDebug() << "From: "<<(*(dataArray))[0].data()<<" to: "<<(*(dataArray))[dataArray->size()-1].data();

        emit signalDataSend(dataArray);
    }
    else
        return;
}

void DataHolder::dataProcessed(QVector<std::string>* dataPointer, bool sendNext)
{
    if (memoryFlag && sendNext)
        continueDataGenerating();
    else if (memoryFlag)
    {
        dataArray->clear();
        delete dataArray;
        dataArray = nullptr;
    }
}

void DataHolder::saveCurrentDataArrayToFile(const QString &path)
{
    QFile fileToWrite(path);
    fileToWrite.open(QIODevice::WriteOnly);

    for (auto item : *dataArray)
    {
        fileToWrite.write(item.data());
        fileToWrite.write("\n");
    }

    fileToWrite.close();
}

void DataHolder::setMemoryLimit(uint megabytes)
{
    memoryLimit = megabytes;
    const ulong oneElementSize = sizeof(std::string);
    const uint bytesInMb = 1048576;
    dataSizeLimit = (megabytes*bytesInMb)/oneElementSize;
    //qDebug() << "Elements limit: "<<dataSizeLimit;
}

void DataHolder::generateData(const QVector<char> &symbols, const uint length)
{
    if (generatingDataStorage != nullptr)
        delete generatingDataStorage;
    else
        generatingDataStorage = new GeneratingData(symbols, length);

    generateData();
}

void DataHolder::generateData()
{
    if (dataArray != nullptr)
        delete dataArray;

    memoryFlag = false;

    dataArray = new QVector<std::string>;

    // добавляем первый элемент
    dataArray->push_back(generatingDataStorage->sequence.data());

    // добавляем остальные насколько позволит лимит памяти
    getAllSequenceOfSymbols(generatingDataStorage);

    qDebug() << "New Data Generated! Size is: "<<dataArray->size();
    qDebug() << "From: "<<(*(dataArray))[0].data()<<" to: "<<(*(dataArray))[dataArray->size()-1].data();
    emit signalDataSend(dataArray);
}


