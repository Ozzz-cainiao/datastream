#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include "addnoise.h"
QT_BEGIN_NAMESPACE
namespace Ui
{
    class Widget;
}
QT_END_NAMESPACE

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();
    AddNoise *addnoise;
    QVector<double> arrX;
    QVector<double> arrY;
    QVector<double> arrTime;
    double fs;
    double amp;
    double fre;
    double time;

    void sendsignal();


signals:
    //自定义信号  需要声明 不需要实现
    void senddata(QVector<double> arrTime,QVector<double> arrY);
    void sendpara(double &fs,double &amp,double &fre,double &time);




private:
    Ui::Widget *ui;
};
#endif // WIDGET_H
