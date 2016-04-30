#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "qcustomplot.h"
namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();


private slots:
    void on_Stop_clicked();
    void on_Start_clicked();
    void on_Rewind_clicked();
    void on_ReadFile_clicked();
    void onTimerSignal();

private:
    Ui::MainWindow *ui;
    void addGraph();
    int *timeData;  // хранит время из файла
    float *yData;   // хранит данные координаты х
    int length;     // длина файла
    int lengthData; // длинна массивов timeData и yData
    int nextFileTime;
    int stopFileTime;
    QTimer *dataTimer;
    void calculateRedrawTime();
    int nextRealTime;
    int countTimer;
    int getDelayFromFile(int currentFileTime);
    void cleanTimers();
    int indexofdata;
    bool isRewind;
};

#endif // MAINWINDOW_H
