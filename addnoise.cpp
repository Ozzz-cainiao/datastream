#include "addnoise.h"
#include "ui_addnoise.h"
#include <QPushButton>
#include <QRandomGenerator>
#include <QVector>
#include <QtGlobal>

AddNoise::AddNoise(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::AddNoise)
{
    ui->setupUi(this);
//    //初始化随机数种子
//    srand((unsigned int)(time(NULL)));
    //创建画布,设置画布上的点数据
    noiseplot=new QCustomPlot(this);
    Filter *firfilter=new Filter;

    ui->noiseplot->addGraph();
    ui->noiseplot->xAxis->setRange(0,10);
    ui->noiseplot->yAxis->setRange(-50,50);
    //设置坐标轴标签
    ui->noiseplot->xAxis->setLabel("time/s");
    ui->noiseplot->yAxis->setLabel("amp/V");
    connect(ui->plotbtn,&QPushButton::clicked,[=]()
    {
        snr=(ui->snrEdit->text()).toDouble();
        qDebug()<<"snr="<<snr;
        plot(fs,arrTime,arrY,snr);


    });
    connect(ui->pbn_save,&QPushButton::clicked,[=]()
    {
        on_pbn_save_clicked();
    });
    connect(ui->denoisebtn,&QPushButton::clicked,[=]()
    {
        //点击降噪按钮进入新页面降噪 同时将数组数据传入新页面中
        //新页面 可设定信噪比 显示加噪完的图像
        qDebug()<<"点击了降噪按钮";
        //自身隐藏
//        this->hide();
        firfilter->setWindowTitle("降噪窗口");

        firfilter->show();
        emit senddata(this->arrTime,this->s_a_n,fs);
    });





}

//定义接收数据槽函数
void AddNoise::receivepara(double &N_fs,double &N_amp,double &N_fre,double &N_time)
{

    qDebug()<<"receivepara";

    fs=N_fs;//全局变量获得赋值
    amp=N_amp;
    fre=N_fre;
    time=N_time;
    qDebug()<<"槽函数fs="<<fs;







}
void AddNoise::receivedata(QVector<double> arrtime,QVector<double> arry)
{

    qDebug()<<"receivedata";
//    QVector<double>::iterator it;

//    for(it=arrtime.begin();it!=arrtime.end();++it)
    for(int i=0; i<arrtime.size(); i++)
    {
        arrTime.append(arrtime[i]);
    }
    for(int i=0; i<arry.size(); i++)
    {
        arrY.append(arry[i]);
    }
//    QVector<double>Temp(arrtime);
//    arrTime.swap(Temp);

//    QVector<double>Temp2(arry);
//    arrY.swap(Temp2);

//    double a;
//    foreach(a,arrtime)
//    {
//        arrTime.append(a);
//    }

//    double b;

//    foreach(b,arry)
//    {
//        arrY.append(b);
//    }
    qDebug()<<" ";





}

void AddNoise::plot(double&fs,QVector<double> t,QVector<double> y,double thesnr)
{

    qDebug()<<"画图函数";
    qDebug()<<"画图函数fs="<<fs;
    qDebug()<<"画图函数 snr="<<thesnr;
    //画图

    double sum=0;//能量和
    double ypower=0;//功率
    double vv;

    int n=y.size();
    for (int i=0;i<n;i++)
    {
        e.append(y[i]*y[i]);//平方和
        sum+=e[i];//信号能量
    }
   ypower=sum/n;
   vv=ypower/(pow(10,snr/10));
   qsrand(QTime(0,0,0).secsTo(QTime::currentTime()));
   for(int j=0;j<n;j++)
   {
       noise.append(gaussrand(0,1));
   }
   double varnoise=Var(noise);

   qDebug()<<"";
   for(int j=0;j<n;j++)
   {
       bpnoise.append(noise[j]/sqrt(varnoise)*sqrt(vv));
       s_a_n.append(y[j]+bpnoise[j]);
   }
   qDebug()<<"";
   ui->noiseplot->graph(0)->setData(t,s_a_n);
   ui->noiseplot->graph(0)->rescaleAxes();
   ui->noiseplot->replot();











}

double AddNoise::Var(QVector<double> x)
{
    int length=x.size();//vector长度
    double sum=0,psum=0;
    for(int i=0;i<length;i++)
    {
        sum+=x[i];//求和
        psum+=x[i]*x[i];//平方和

    }
    double var=psum/length-(sum/length)*(sum/length);


    return var;
}
double  AddNoise::gaussrand(double E,double V)
{
     static  double  V1, V2, S;
     static  int  phase = 0;
     double  X;

     if  ( phase == 0 ) {
         do  {
             double  U1 = ( double ) rand () / RAND_MAX;
             double  U2 = ( double ) rand () / RAND_MAX;

             V1 = 2 * U1 - 1;
             V2 = 2 * U2 - 1;
             S = V1 * V1 + V2 * V2;
         }  while (S >= 1 || S == 0);

         X = V1 *  sqrt (-2 *  log (S) / S);
     }  else
         X = V2 *  sqrt (-2 *  log (S) / S);

     phase = 1 - phase;
     X = X * V + E;

     return  X;
}
bool AddNoise::on_pbn_save_clicked()
{
    QString filename = QFileDialog::getSaveFileName();

    if( filename == "" ){
        QMessageBox::information(this,"fail","保存失败");
         return false;
     }
     if( filename.endsWith(".png") ){
         QMessageBox::information(this,"success","成功保存为png文件");
         return ui->noiseplot->savePng( filename, ui->noiseplot->width(), ui->noiseplot->height() );

     }
     if( filename.endsWith(".jpg")||filename.endsWith(".jpeg") ){
         QMessageBox::information(this,"success","成功保存为jpg文件");
         return ui->noiseplot->saveJpg( filename, ui->noiseplot->width(), ui->noiseplot->height() );

     }
     if( filename.endsWith(".bmp") ){
         QMessageBox::information(this,"success","成功保存为bmp文件");
         return ui->noiseplot->saveBmp( filename, ui->noiseplot->width(), ui->noiseplot->height() );

     }
     if( filename.endsWith(".pdf") ){
         QMessageBox::information(this,"success","成功保存为pdf文件");
         return ui->noiseplot->savePdf( filename, ui->noiseplot->width(), ui->noiseplot->height() );

     }
    else{
     //否则追加后缀名为.png保存文件
         QMessageBox::information(this,"success","保存成功,已默认保存为png文件");
     return ui->noiseplot->savePng(filename.append(".png"), ui->noiseplot->width(), ui->noiseplot->height() );

    }
}

AddNoise::~AddNoise()
{
    delete ui;
}
