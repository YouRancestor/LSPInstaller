#include "protocolinfoform.h"
#include "ui_protocolinfoform.h"

ProtocolInfoForm::ProtocolInfoForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ProtocolInfoForm)
{
    ui->setupUi(this);
}

ProtocolInfoForm::~ProtocolInfoForm()
{
    delete ui;
}

void ProtocolInfoForm::updateInfo(WSAPROTOCOL_INFOW info)
{
    // Entry Type
    if(info.ProtocolChain.ChainLen==1)
    {
        ui->lineEdit_ProviderId->setEnabled(false);
        ui->label_EntryTypeContent->setText("Base Service Provider");
    }
    else
    {
        ui->lineEdit_ProviderId->setEnabled(true);
        ui->label_EntryTypeContent->setText("Layered Chain Entry");
    }

    // Protocol
    ui->lineEdit_ProtocolName->setText(QString::fromWCharArray(info.szProtocol));

    // Provider ID
    GUID guid=info.ProviderId;
    ui->lineEdit_ProviderId->setText(QString::asprintf("{%08x-%04hx-%04hx-%02hhx%02hhx-%02hhx%02hhx%02hhx%02hhx%02hhx%02hhx}",
                                                       (guid.Data1), (guid.Data2), (guid.Data3),
                                                       (guid.Data4[0]), (guid.Data4[1]), (guid.Data4[2]), (guid.Data4[3]),
                                                       (guid.Data4[4]), (guid.Data4[5]), (guid.Data4[6]), (guid.Data4[7])));

    // Catalog Entry ID
    ui->label_CatalogEntryIdValue->setText(QString("%1").arg(info.dwCatalogEntryId));

    // Version
    ui->spinBox_Version->setValue(info.iVersion);

    // Address Family
    ui->spinBox_AddressFamily->setValue(info.iAddressFamily);

    // Max Socket Address Length
    ui->spinBox_AddressLengthMax->setValue(info.iMaxSockAddr);
    // Min Socket Address Length
    ui->spinBox_AddressLengthMin->setValue(info.iMinSockAddr);

    // Socket Type
    ui->spinBox_SocketType->setValue(info.iSocketType);

    // Socket Protocol
    ui->spinBox_SocketProtocol->setValue(info.iProtocol);

    // Socket Protocol Max Offset
    ui->spinBox_MaxOffset->setValue(info.iProtocolMaxOffset);

    // Network Byte Order
    ui->spinBox_NetworkByteOrder->setValue(info.iNetworkByteOrder);

    // Security Scheme
    ui->spinBox_SecurityScheme->setValue(info.iSecurityScheme);

    // Max Message Size
    ui->spinBox_MaxMessageSize->setValue(info.dwMessageSize);

    // Service Flags
    ui->spinBox_ServiceFlags1->setValue(info.dwServiceFlags1);
    ui->spinBox_ServiceFlags2->setValue(info.dwServiceFlags2);
    ui->spinBox_ServiceFlags3->setValue(info.dwServiceFlags3);
    ui->spinBox_ServiceFlags4->setValue(info.dwServiceFlags4);

    // Provider Flags
    ui->spinBox_ProviderFlags->setValue(info.dwProviderFlags);

    // Protocol Chain Length
    ui->spinBox_ProtocolChainLength->setValue(info.ProtocolChain.ChainLen);
    // Protocol Chain
    ui->spinBox_ProtocolChain_1->setValue(info.ProtocolChain.ChainEntries[0]);
    ui->spinBox_ProtocolChain_2->setValue(info.ProtocolChain.ChainEntries[1]);
    ui->spinBox_ProtocolChain_3->setValue(info.ProtocolChain.ChainEntries[2]);
    ui->spinBox_ProtocolChain_4->setValue(info.ProtocolChain.ChainEntries[3]);
    ui->spinBox_ProtocolChain_5->setValue(info.ProtocolChain.ChainEntries[4]);
    ui->spinBox_ProtocolChain_6->setValue(info.ProtocolChain.ChainEntries[5]);
    ui->spinBox_ProtocolChain_7->setValue(info.ProtocolChain.ChainEntries[6]);

    // Provider Reserved
    ui->spinBox_ProviderReserved->setValue(info.dwProviderReserved);

    INT len = MAX_PATH;
    WCHAR dllPath[MAX_PATH];
    INT err = 0;
    WSCGetProviderPath(&info.ProviderId, dllPath, &len, &err);
#ifdef WIN64
    ui->lineEdit_Dll64Path->setText(QString::fromWCharArray(dllPath));
    WSCGetProviderPath32(&info.ProviderId, dllPath, &len, &err);
#endif // WIN64
    ui->lineEdit_Dll32Path->setText(QString::fromWCharArray(dllPath));
}

void ProtocolInfoForm::collectInfo(WSAPROTOCOL_INFOW *info)
{
    // Protocol
    ui->lineEdit_ProtocolName->text().toWCharArray(info->szProtocol);

    // Version
    info->iVersion = ui->spinBox_Version->value();

    // Address Family
    info->iAddressFamily = ui->spinBox_AddressFamily->value();

    // Max Socket Address Length
    info->iMaxSockAddr = ui->spinBox_AddressLengthMax->value();
    // Min Socket Address Length
    info->iMinSockAddr = ui->spinBox_AddressLengthMin->value();

    // Socket Type
    info->iSocketType = ui->spinBox_SocketType->value();

    // Socket Protocol
    info->iProtocol = ui->spinBox_SocketProtocol->value();

    // Socket Protocol Max Offset
    info->iProtocolMaxOffset = ui->spinBox_MaxOffset->value();

    // Network Byte Order
    info->iNetworkByteOrder = ui->spinBox_NetworkByteOrder->value();

    // Security Scheme
    info->iSecurityScheme = ui->spinBox_SecurityScheme->value();

    // Max Message Size
    info->dwMessageSize = ui->spinBox_MaxMessageSize->value();

    // Service Flags
    info->dwServiceFlags1 = ui->spinBox_ServiceFlags1->value();
    info->dwServiceFlags2 = ui->spinBox_ServiceFlags2->value();
    info->dwServiceFlags3 = ui->spinBox_ServiceFlags3->value();
    info->dwServiceFlags4 = ui->spinBox_ServiceFlags4->value();

    // Provider Flags
    info->dwProviderFlags = ui->spinBox_ProviderFlags->value();

    // Protocol Chain Length
    info->ProtocolChain.ChainLen = ui->spinBox_ProtocolChainLength->value();
    // Protocol Chain
    info->ProtocolChain.ChainEntries[0] = ui->spinBox_ProtocolChain_1->value();
    info->ProtocolChain.ChainEntries[1] = ui->spinBox_ProtocolChain_2->value();
    info->ProtocolChain.ChainEntries[2] = ui->spinBox_ProtocolChain_3->value();
    info->ProtocolChain.ChainEntries[3] = ui->spinBox_ProtocolChain_4->value();
    info->ProtocolChain.ChainEntries[4] = ui->spinBox_ProtocolChain_5->value();
    info->ProtocolChain.ChainEntries[5] = ui->spinBox_ProtocolChain_6->value();
    info->ProtocolChain.ChainEntries[6] = ui->spinBox_ProtocolChain_7->value();

    // Provider Reserved
    info->dwProviderReserved = ui->spinBox_ProviderReserved->value();

    // Catalog Entry ID
    info->dwCatalogEntryId = ui->label_CatalogEntryIdValue->text().toUInt();

}
