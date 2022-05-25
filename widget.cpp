#include "widget.h"
#include "ui_widget.h"
#include <QPushButton>
#include <QString>
#include <QLineEdit>
#include "qcustomplot.h"

#define PI 3.14
Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);

    arrX.reserve(10000);
    arrY.reserve(10000);
    arrTime.reserve(10000);

    //    实例化加噪窗口
    AddNoise *addnoise=new AddNoise;

    //创建画布,设置画布上的点数据
    QCustomPlot *customPlot=new QCustomPlot;
    customPlot->setParent(this);
    ui->customPlot->addGraph();
    ui->customPlot->xAxis->setRange(0,10);
    ui->customPlot->yAxis->setRange(-50,50);
    //设置坐标轴标签
    ui->customPlot->xAxis->setLabel("time/s");
    ui->customPlot->yAxis->setLabel("amp/V");
    connect(ui->setparbtn,&QPushButton::clicked,[=]()
    {
        //点击按钮 获得设置的幅度和频率值
        amp=(ui->ampEdit->text()).toDouble();
        fre=(ui->freEdit->text()).toDouble();
        time=(ui->timeEdit->text()).toDouble();
        fs=(ui->fsEdit->text()).toDouble();
        qDebug()<<"设置的采样频率为："<<fs;
        qDebug()<<"设置的幅度为："<<amp;
        qDebug()<<"设置的频率为："<<fre;
        qDebug()<<"设置的时间为："<<time;


        //根据频率幅度画正弦曲线

        for(int i=0;i<=time*fs;i++)
        {
            arrX.append(i);
            arrY.append(amp*cos(2*PI*fre*arrX[i]/fs));
            arrTime.append(i*1.00/fs);
        }

        ui->customPlot->graph(0)->setData(arrTime,arrY);
        ui->customPlot->replot();
        ui->customPlot->graph(0)->rescaleAxes();

    });

    connect(this,SIGNAL(senddata(QVector<double>,QVector<double>)),addnoise,SLOT(receivedata(QVector<double>,QVector<double>)));
    connect(this,SIGNAL(sendpara(double &,double &,double &,double &)),addnoise,SLOT(receivepara(double &,double &,double &,double&)));

    //监听加噪信号按钮
    connect(ui->addnoisebtn,&QPushButton::clicked,[=]()
    {
        //点击加噪按钮进入新页面加噪 同时将数组数据传入新页面中
        //新页面 可设定信噪比 显示加噪完的图像
        qDebug()<<"点击了加噪按钮";
        //自身隐藏
//        this->hide();
        addnoise->setWindowTitle("加噪窗口");

        addnoise->show();
        sendsignal();



    });
}

void Widget::sendsignal()
{
    //将主窗口的数据传入
    emit senddata(this->arrTime,this->arrY);
    emit sendpara(this->fs,this->amp,this->fre,this->time);
}


Widget::~Widget()
{
    delete ui;
}
