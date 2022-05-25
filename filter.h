#ifndef FILTER_H
#define FILTER_H
#include "qcustomplot.h"
#include <QMainWindow>

namespace Ui
{
    class Filter;
}

class Filter : public QMainWindow
{
    Q_OBJECT

public:
    Filter(QWidget *parent = nullptr);
    ~Filter();
    int n,band,wn,fln,fhn;
    double fs;

    QVector<double>hh,yy;
    QVector<double>output;
    double beta;

    void convolution(QVector<double> input1,QVector<double> input2);
    int min(int a, int b);

    QCustomPlot *denoiseplot;
    double bessel0(double x);
    double kaiser(int i, int n, double beta);
    double window(int type, int n, int i, double beta);
    void firwin(int n, int band, int wn, int fs, double kaiser, int fln, int fhn);

public slots:
    //接收信号槽函数
    void receivedata(QVector<double>,double);
private:
    Ui::Filter *ui;
};

#endif // FILTER_H
