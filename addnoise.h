#ifndef ADDNOISE_H
#define ADDNOISE_H
#include <qcustomplot.h>
#include <QWidget>
#include "filter.h"

namespace Ui
{
class AddNoise;
}

class AddNoise : public QWidget
{
    Q_OBJECT

public:
    AddNoise(QWidget *parent = nullptr);
    ~AddNoise();
    //定义槽函数 接收主窗口传过来的数据  需要声明 也需要实现
    double fs,amp,time,fre,snr;//作为全局变量接收到槽函数的参数
    QVector<double> arrTime;
    QVector<double> arrY;
    void plot(double&fs,QVector<double>t,QVector<double>y,double thesnr);
    //定义方差函数
    double Var (QVector<double>);//传入数组
    QCustomPlot *noiseplot;
    Filter *firfilter;
    double  gaussrand(double E,double V);
    bool on_pbn_save_clicked();
    QVector<double>e,noise,bpnoise,s_a_n;
public slots:
    void receivepara(double &,double &,double &,double &);
    void receivedata(QVector<double>,QVector<double>);
signals:
    void senddata(QVector<double> arrTime,QVector<double> s_a_n,double fs);

private:
    Ui::AddNoise *ui;
};

#endif // ADDNOISE_H
