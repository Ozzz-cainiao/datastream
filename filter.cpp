#include "filter.h"
#include "ui_filter.h"
#include <QtMath>
#include <QPushButton>

Filter::Filter(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::Filter)
{
    ui->setupUi(this);
    denoiseplot=new QCustomPlot(this);

    ui->denoiseplot->addGraph();
    ui->denoiseplot->xAxis->setRange(0,10);
    ui->denoiseplot->yAxis->setRange(-50,50);
    //设置坐标轴标签
    ui->denoiseplot->xAxis->setLabel("time/s");
    ui->denoiseplot->yAxis->setLabel("amp/V");


    //点击按钮获取滤波器参数
    connect(ui->design,&QPushButton::clicked,[=]()
    {
        n=ui->lineEditn->text().toInt();

        beta=ui->lineEditbeta->text().toDouble();
        fln=ui->lineEditfln->text().toInt();
        fhn=ui->lineEditfhn->text().toInt();
        wn=ui->comboBox_2->currentIndex()+1;//窗函数的类型，取值1-7，分别对应矩形窗、图基窗、三角窗、汉宁窗、海明窗、布拉克曼窗和凯塞窗
        band=ui->comboBox->currentIndex()+1;//滤波器的类型,取值1-4,分别为低通、带通、带阻、高通滤波器
        //调用函数获得滤波器系数
        firwin(n,band,wn,fs,beta,fln,fhn);




    });

    //画图
    connect(ui->plot,&QPushButton::clicked,[=]()
    {
        //将系数与传过来的参数卷积
        convolution(hh,yy);

    });
}
 double Filter::bessel0(double x)
{
    int i;
    double d;
    double y;
    double d2;
    double sum;
    y = x / 2.0;
    d = 1.0;
    sum = 1.0;
    for (i=1; i<=25; i++)
    {
        d = d * y / i;
        d2 = d * d;
        sum = sum + d2;
        if (d2 < sum*(1.0e-8))
        {
            break;
        }
    }
    return(sum);
}
 double Filter::kaiser(int i, int n, double beta)
{
    double a;
    double w;
    double a2;
    double b1;
    double b2;
    double beta1;

    b1 = bessel0(beta);
    a = 2.0 * i / (double)(n - 1) - 1.0;
    a2 = a * a;
    beta1 = beta * sqrt(1.0 - a2);
    b2 = bessel0(beta1);
    w = b2 / b1;
    return(w);
}
//n：窗口长度 type：选择窗函数的类型 beta：生成凯塞窗的系数
 double Filter::window(int type, int n, int i, double beta)
{
    int k;
    double pi;
    double w;
    pi = 4.0 * atan(1.0);  //pi=PI;
    w = 1.0;

    switch (type)
    {
    case 1:
    {
        w = 1.0;  //矩形窗
        break;
    }
    case 2:
    {
        k = (n - 2) / 10;
        if (i <= k)
        {
            w = 0.5 * (1.0 - cos(i * pi / (k + 1)));  //图基窗
        }
        if (i > n-k-2)
        {
            w = 0.5 * (1.0 - cos((n - i - 1) * pi / (k + 1)));
        }
        break;
    }
    case 3:
    {
        w = 1.0 - fabs(1.0 - 2 * i / (n - 1.0));//三角窗
        break;
    }
    case 4:
    {
        w = 0.5 * (1.0 - cos( 2 * i * pi / (n - 1)));//汉宁窗
        break;
    }
    case 5:
    {
        w = 0.54 - 0.46 * cos(2 * i * pi / (n - 1));//海明窗
        break;
    }
    case 6:
    {
        w = 0.42 - 0.5 * cos(2 * i * pi / (n - 1)) + 0.08 * cos(4 * i * pi / (n - 1));//布莱克曼窗
        break;
    }
    case 7:
    {
        w = kaiser(i, n, beta);//凯塞窗
        break;
    }
    }
    return(w);
}
//n：滤波器的阶数
//band：滤波器的类型,取值1-4,分别为低通、带通、带阻、高通滤波器
//wn：窗函数的类型，取值1-7，分别对应矩形窗、图基窗、三角窗、汉宁窗、海明窗、布拉克曼窗和凯塞窗
//fs：采样频率
//h：存放滤波器的系数
//kaiser：beta值
//fln：带通下边界频率
//fhn：带通上边界频率

void Filter::firwin(int n, int band, int wn, int fs, double beta, int fln, int fhn)
{
    int i;
    int n2;
    int mid;
    double s;
    double pi;
    double wc1;
    double wc2;

    double delay;



    pi = 4.0 * qAtan(1.0);   //pi=PI;

    if ((n % 2) == 0)/*如果阶数n是偶数*/
    {
        n2 = (n / 2) - 1;/**/
        mid = 1;//
    }
    else
    {
        n2 = n / 2;//n是奇数,则窗口长度为偶数
        mid = 0;
    }

    delay = n / 2.0;
    wc1 = 2 * pi * fln;
    wc2 = 2 * pi * fhn;

    double h[n2];
    switch (band)
    {
    case 1:
    {
        for (i=0; i<=n2; ++i)
        {
            s = i - delay;
            h[i] = (sin(wc1 * s / fs) / (pi * s)) * window(wn, n+1, i, beta);//低通,窗口长度=阶数+1，故为n+1
            h[n - i] = h[i];
        }
        if (mid == 1)
        {
            h[n / 2] = wc1 / pi;//n为偶数时，修正中间值系数
        }
        break;
    }
    case 2:
    {
        for (i=0; i<=n2; i++)
        {
            s = i - delay;
            h[i] = (sin(wc2 * s / fs) - sin(wc1 * s / fs)) / (pi * s);//带通
            h[i] = h[i] * window(wn, n+1, i, beta);
            h[n-i] = h[i];
        }
        if (mid == 1)
        {
            h[n / 2] = (wc2 - wc1) / pi;
        }
        break;
    }
    case 3:
    {
        for (i=0; i<=n2; ++i)
        {
            s = i - delay;
            h[i] = (sin(wc1 * s / fs) + sin(pi * s) - sin(wc2 * s / fs)) / (pi * s);//带阻
            h[i] = h[i] * window(wn, n+1, i, beta);
            h[n - i] = h[i];
        }
        if (mid == 1)
        {
            h[n / 2] = (wc1 + pi - wc2) / pi;
        }
        break;
    }
    case 4:
    {
        for (i=0; i<=n2; i++)
        {
            s = i - delay;
            h[i] = (sin(pi * s) - sin(wc1 * s / fs)) / (pi * s);//高通
            h[i] = h[i] * window(wn, n+1, i, beta);
            h[n-i] = h[i];
        }
        if (mid == 1)
        {
            h[n / 2] = 1.0 - wc1 / pi;
        }
        break;
    }
    }
    //将获得的滤波器系数放到Vector中
    for(int i=0;i<n2;i++)
    {
        hh.append(h[i]);
    }

}

int Filter::min(int a, int b)
{
    return a < b ? a : b;
}
void Filter::convolution(QVector<double> input1,QVector<double> input2)
{
    int mm=input1.size();
    int nn=input2.size();
    int len=mm+nn-1;
    double xx[len];
    double tempinput2[len];
    for (int i = 0; i < nn; i++)
    {
        tempinput2[i] = input2[i];
    }
    for (int i = nn; i < len; i++)
    {
        tempinput2[i] = 0.0;
    }
    // do convolution
    for (int i = 0; i < len; i++)
    {
        xx[i] = 0.0;
        int tem = (min(i, mm)) == mm ? mm-1 : min(i, mm);
        for (int j = 0; j <= tem; j++)
        {
            xx[i] += (input1[j] * tempinput2[i - j]);
        }
    }
    // set value to the output array
    for (int i = 0; i < len; i++)
    {
        output.append(xx[i]);
    }
}
 void Filter::receivedata(QVector<double>san,double ffs)
 {
     fs=ffs;
     for(int i=0;i<san.size();i++)
     {
         yy.append(san[i]);
     }

 }
Filter::~Filter()
{
    delete ui;
}
