#include "widget.h"

#include "loghandler.h"

#include <QApplication>
#include <QDebug>
#include <QTime>
#include <QPushButton>


int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    Widget w;
    w.show();
    // [[1]] 安装消息处理函数
    LogHandler::Get().installMessageHandler();

    // [[2]] 输出测试，查看是否写入到文件
    qDebug() << "Hello";
    qDebug() << "当前时间是: " << QTime::currentTime().toString("hh:mm:ss");
    qInfo() << QString("God bless you!");

    QPushButton *button = new QPushButton("退出");
    button->show();
    QObject::connect(button, &QPushButton::clicked, [&app] {
        qDebug() << "退出";
        app.quit();
    });

    // [[3]] 取消安装自定义消息处理，然后启用
    LogHandler::Get().uninstallMessageHandler();
    qDebug() << "........"; // 不写入日志
    LogHandler::Get().installMessageHandler();

    int ret = app.exec(); // 事件循环结束

    // [[4]] 程序结束时释放 LogHandler 的资源，例如刷新并关闭日志文件
    LogHandler::Get().uninstallMessageHandler();

    return ret;

}
