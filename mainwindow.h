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

    //void on_Play_clicked();

    void on_Rewind_clicked();

    void on_ReadFile_clicked();
    void onTimerSignal();

private:
    Ui::MainWindow *ui;
    void addGraph();
    int *timeData;
    float *yData;
    int length;
    int lengthData;
    int nextFileTime;
    int stopFileTime;
    QTimer *dataTimer;
    QTimer *redrawTimer;
    void calculateRedrawTime();
    int nextRealTime;
    int countTimer;
    int getDelayFromFile(int currentFileTime);
    void cleanTimers();
    int indexofdata;
    bool isRewind;
};

#endif // MAINWINDOW_H
