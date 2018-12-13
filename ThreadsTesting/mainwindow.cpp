#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QDebug>
#include <QCryptographicHash>

#include "../EncryptStringFinder/encryptstringfinder.h"

static QByteArray HASH_KEY_VALUE = "";

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    QObject::connect(ui->pbGenerate, SIGNAL(clicked()), this, SLOT(generateData()));

    QVector<char> symbols = {'0','1','2','3','4','5','6','7','8','9','a','b','c','d','e','f','g','h','i','j','k','l','m','n','o','p', 'q','r','s','t','u','v','w','x','y','z'};
    uint strokeLength = 6;

    stringFinder = new EncryptStringFinder(symbols, strokeLength, 600, changeData, isValidValue, this);
    QObject::connect(stringFinder, SIGNAL(signalSolutionFinded(QString)), this, SLOT(solutionFinded(QString)));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::generateData()
{
    QString line = ui->hash->text();
    line.remove(' ');
    int hashSize = line.size();
    if (hashSize%2 != 0)
    {
        ui->answer->setText("Wrong HASH!");
        return;
    }
    QByteArray fromString;

    for (int i = 0; i < hashSize; i+=2)
    {
        QString num(line.mid(i, 2));
        bool ok;
        fromString.append(num.toInt(&ok, 16));
        if (!ok)
        {
            ui->answer->setText("Wrong HASH!");
            return;
        }
    }
    ui->answer->setText(fromString.data());

    HASH_KEY_VALUE = fromString;
    stringFinder->start();
}

void MainWindow::solutionFinded(QString answer)
{
    ui->answer->setText(answer);
}

// Функции работы с наборами байт которые используются в потоках
QByteArray changeData(const std::string &string)
{
    return QCryptographicHash::hash(string.data(), QCryptographicHash::Sha1);
}

bool isValidValue(const QByteArray& bytes)
{
    return (bytes == HASH_KEY_VALUE);
}
// ================================================================
