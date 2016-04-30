#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "qcustomplot.h"
#include <QDebug>
#include <QString>
#include <QTimer>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    int key = 1000;
    ui->widget->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectAxes |
                                    QCP::iSelectLegend | QCP::iSelectPlottables);
    ui->widget->addGraph();
    // give the axes some labels:
    ui->widget->xAxis->setLabel("x");
    ui->widget->xAxis->setRange(0.0, key);
    ui->widget->yAxis->setLabel("y");
    ui->widget->yAxis->setRange(-20.0, 20.0);
    nextFileTime = 0;   // время из файла, идентификатор след. события
    stopFileTime = 0;   // время из файла, идентификатор события, на котором остановили
    nextRealTime = 0;   // реальное время по которому будет строится график
    countTimer = 0;     // счётчик для таймера, чтобы высчитывать текущее время
    isRewind = 0;       // флаг обзначающий перемотку вперёд или назад
    indexofdata = 0;    // текущий индекс даты
    dataTimer = new QTimer(this);
    connect(dataTimer, SIGNAL(timeout()), this, SLOT(onTimerSignal()));
}

void MainWindow::on_ReadFile_clicked()
{
    QFile file("C:/Users/Dasha/Downloads/accelerometer.output");
    QStringList buf;
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        qDebug()<<"There is no file";
        return;
    }
    QString str = QString(file.readAll());
    buf = str.split("\n");
    length = buf.length();
    timeData = new int[length/5];
    yData = new float [length/5];
    for(int i = 0; i < length/5; i++)
    {
        timeData[i] = 0;
        yData[i] = 0.0;
    }
    int timerange = 1000*60*60;
    int timeStart = 0;
    for(int i = 0; i < length; i++)
    {
        QString line = buf[i];
        int index = 0;
        if(line.contains("time"))
        {
            line.remove(0,10);
            int timer = timerange;
            for(int j = 0; j < 3; j++)
            {
                index = line.indexOf(":");
                timeData[i/5] += line.mid(0,index).toInt() * timer;
                timer = timer / 60;
                line.remove(0,index+1);
            }
            index = line.indexOf(",") - 1;
            timeData[i/5] += line.mid(0,index).toInt();
            if(i == 1)
            {
                timeStart = timeData[i/5];
            }
            timeData[i/5] -= timeStart;
        }
        if(line.contains("y"))
        {
            line.remove(0,7);
            index = line.indexOf(",") - 1;
            yData[i/5] = line.mid(0,index).toFloat();
            //qDebug()<<"yData" << yData[i/5];
        }
    }
    lengthData = length/5;
}

void MainWindow::on_Start_clicked()
{
    isRewind = false;
    ui->Start->setEnabled(false);
    ui->Stop->setEnabled(true);
    ui->Rewind->setEnabled(false);
    // расчитываем первое след. время
    nextFileTime = stopFileTime;
    calculateRedrawTime();
    dataTimer->start(5);

}

void MainWindow::on_Rewind_clicked()
{
    isRewind = true;
    ui->Start->setEnabled(false);
    ui->Stop->setEnabled(true);
    ui->Rewind->setEnabled(false);
    // расчитываем первое след. время
    nextFileTime = stopFileTime;
    calculateRedrawTime();
    dataTimer->start(5);
}

void MainWindow::on_Stop_clicked()
{
    ui->Start->setEnabled(true);
    ui->Stop->setEnabled(false);
    ui->Rewind->setEnabled(true);
    stopFileTime = nextFileTime;
    // далее нужно каким-то образом остановить перерисовку.
    // правильный вариант:
    //  	Отвязать сигнал от слота
    dataTimer->stop();
    // плохой вариант:
    //  	Записать в nextRealTime макс значение, чтобы if в слоте для таймера не работал

}

void MainWindow::calculateRedrawTime()
{
    if(!isRewind)
    {
        int delay = getDelayFromFile(nextFileTime);
        nextRealTime = countTimer*5 + delay;
        nextFileTime+=delay;
    }
    if(isRewind)
    {
        int delay = getDelayFromFile(nextFileTime);
        nextRealTime = countTimer*5 - delay;
        nextFileTime-=delay;
    }
}

// Получить разницу между текущим и след.значением
// из данных в файле
int MainWindow::getDelayFromFile(int currentFileTime)
{
    if(!isRewind)
    {
        int result = 0;
        if ( currentFileTime != 0)
        {
            // найти в данных из файла currentTime (оно уникально)
            int i = 0;
            for(i = 0; i < lengthData-1; i++)
            // найти следующее за ним значение
            {
                // записать в result разницу значений
                if (timeData[i] == currentFileTime)
                {
                    int delta = timeData[i+1] - currentFileTime;
                    return delta;
                }
            }
            currentFileTime = 0;
        }
        if ( currentFileTime == 0 )
        {
            // получить разницу между первым и вторым
            // записать в result разницу значений
            result = timeData[1]-timeData[0];
            nextFileTime = timeData[1];
            // сбросить таймеры
            cleanTimers();
        }
        return result;
    }
    if(isRewind)
    {
        int result = 0;
        if ( currentFileTime != timeData[lengthData])
        {
            // найти в данных из файла currentTime (оно уникально)
            int i = 1;
            for(i = 1; i < lengthData; i++)
            // найти следующее за ним значение
            {
                // записать в result разницу значений
                if (timeData[i] == currentFileTime)
                {
                    int delta = currentFileTime - timeData[i-1];
                    return delta;
                }
            }
            currentFileTime = 0;
        }
        if ( currentFileTime == 0 )
        {
            // получить разницу между первым и вторым
            // записать в result разницу значений
            result = timeData[1]-timeData[0];
            nextFileTime = timeData[1];
            // сбросить таймеры
            cleanTimers();
        }
        return result;
    }
}

void MainWindow::cleanTimers()
{
    nextFileTime = 0;
    nextRealTime = 0;
}

void MainWindow::onTimerSignal()
{
    if(!isRewind)
    {
        countTimer++;
        int currentRealTime = countTimer*5; // нужно время в мс
        dataTimer->start(5);
        if ( currentRealTime >= nextRealTime )
        {
            // расчет след. времени
            addGraph();
            calculateRedrawTime();
            if(indexofdata == lengthData)
            {
                ui->Start->setEnabled(false);
                ui->Stop->setEnabled(false);
                ui->Rewind->setEnabled(true);
                dataTimer->stop();
                addGraph();
                qDebug()<<"finish";
            }
        }
        if(countTimer == 1)
        {
            addGraph();
        }
    }
    if(isRewind)
    {
        int currentRealTime = countTimer*5; // нужно время в мс
        countTimer--;
        dataTimer->start(5);
        if ( currentRealTime <= nextRealTime )
        {
            // расчет след. времени
            addGraph();
            calculateRedrawTime();
            if(indexofdata == 0)
            {
                ui->Start->setEnabled(true);
                ui->Stop->setEnabled(false);
                ui->Rewind->setEnabled(false);
                dataTimer->stop();
                addGraph();
                qDebug()<<"finish";
            }
        }
        if(countTimer == 1)
        {
            addGraph();
        }
    }
}

void MainWindow::addGraph()
{
    if(!isRewind)
    {
        if(timeData[indexofdata] >= 500)
        {
            ui->widget->graph(0)->rescaleKeyAxis();
            ui->widget->xAxis->setRange(timeData[indexofdata]+0.25, 1000, Qt::AlignCenter);
        }
        qDebug() << "time "<<timeData[indexofdata]<< "y "<< yData[indexofdata];
        ui->widget->graph(0)->addData(timeData[indexofdata], yData[indexofdata]);
        ui->widget->replot();
        if(indexofdata != lengthData)
            indexofdata++;
    }
    if(isRewind)
    {
        if(timeData[indexofdata] >= 500)
        {
            ui->widget->graph(0)->rescaleKeyAxis();
            ui->widget->xAxis->setRange(timeData[indexofdata]-0.25, 1000, Qt::AlignCenter);
        }
        ui->widget->graph(0)->removeData(timeData[indexofdata]);
        qDebug() << "rewind time "<<timeData[indexofdata]<< "y "<< yData[indexofdata];
        ui->widget->replot();
        if(indexofdata != 0)
            indexofdata--;
    }
}

MainWindow::~MainWindow()
{
  delete ui;
}
