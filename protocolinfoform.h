#ifndef PROTOCOLINFOFORM_H
#define PROTOCOLINFOFORM_H

#include <WinSock2.h>
#include <WS2spi.h>
#include <QWidget>

namespace Ui {
class ProtocolInfoForm;
}

class ProtocolInfoForm : public QWidget
{
    Q_OBJECT

public:
    explicit ProtocolInfoForm(QWidget *parent = nullptr);
    ~ProtocolInfoForm();

    void updateInfo(WSAPROTOCOL_INFOW info);

    void collectInfo(WSAPROTOCOL_INFOW *info);

private:
    Ui::ProtocolInfoForm *ui;
};

#endif // PROTOCOLINFOFORM_H
