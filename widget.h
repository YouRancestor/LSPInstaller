#ifndef WIDGET_H
#define WIDGET_H

#include <WinSock2.h>
#include <WS2spi.h>

#include <QWidget>
#include <QVector>

namespace Ui {
class Widget;
}

class Widget : public QWidget
{
    Q_OBJECT

public:
    explicit Widget(QWidget *parent = nullptr);
    ~Widget();

private slots:
    void on_pushButton_RefreshCatlog_clicked();

    void on_listWidget_Catlog_currentRowChanged(int currentRow);

    void on_pushButton_AddProtocolTab_clicked();

    void on_pushButton_DelProtocolTab_clicked();

    void on_pushButton_GenerateProviderId_clicked();

    void on_pushButton_Install_clicked();

    void on_pushButton_DeinstallProvider_clicked();

    void on_pushButton_InstallAll_clicked();

    void on_pushButton_TopAll_clicked();

private:
    Ui::Widget *ui;

    QVector<WSAPROTOCOL_INFOW> protocolInfos;
};

#endif // WIDGET_H
