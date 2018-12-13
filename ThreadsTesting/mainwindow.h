#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

class EncryptStringFinder;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();


public slots:
    void generateData();

private:
    Ui::MainWindow *ui;
    EncryptStringFinder* stringFinder;

private slots:
    void solutionFinded(QString answer);
};

QByteArray changeData(const std::string& bytes);
bool isValidValue(const QByteArray& bytes);

#endif // MAINWINDOW_H
