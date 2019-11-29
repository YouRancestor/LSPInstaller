#include "widget.h"
#include "ui_widget.h"
#include <QMessageBox>
#include <wchar.h>
#include <SpOrder.h>
#include "protocolinfoform.h"

Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    ui->setupUi(this);

    // Initialize WinSock
    WSADATA wsaData;
    int result = 0;
    result = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (result != 0) {
        QMessageBox::information(this, "error", QString::asprintf("WSAStartup failed with error code: %d.", result));
        throw "Initialize WinSock failed";
    }

    on_pushButton_RefreshCatlog_clicked();
}

Widget::~Widget()
{
    delete ui;

    // Deinitialize WinSock
    WSACleanup();
}

void Widget::on_pushButton_RefreshCatlog_clicked()
{
    // Clear the current list
    ui->listWidget_Catlog->clear();
    protocolInfos.clear();

    // Get protocol buffer length
    int err = 0;
    DWORD bufferLen = 0;
    WSCEnumProtocols(NULL, NULL, &bufferLen, &err);
    protocolInfos.resize(bufferLen/sizeof(WSAPROTOCOL_INFOW));

    // Enum protocols
    int infoNumber = WSCEnumProtocols(NULL, protocolInfos.data(), &bufferLen, &err);
    if(infoNumber == SOCKET_ERROR)
    {
        QMessageBox::information(this, "error", QString::asprintf("WSAEnumProtocols failed with error: %d.", err));
        return;
    }

#ifdef WIN64
    // Enum protocols 32bit
    WSCEnumProtocols32(NULL, NULL, &bufferLen, &err);
    int oldSize=protocolInfos.size();
    protocolInfos.resize(protocolInfos.size() + bufferLen/sizeof(WSAPROTOCOL_INFOW));
    infoNumber = WSCEnumProtocols32(NULL, &protocolInfos[oldSize], &bufferLen, &err);

    for (int i = 0; i < infoNumber; ++i) {
        wcscat_s(protocolInfos[oldSize+i].szProtocol, L" (32bit)");
    }

    if(infoNumber == SOCKET_ERROR)
    {
        QMessageBox::information(this, "error", QString::asprintf("WSAEnumProtocols32 failed with error: %d.", err));
        return;
    }
#endif // WIN64

    for(int i = 0; i<protocolInfos.size(); i++)
    {
        ui->listWidget_Catlog->addItem(QString::fromWCharArray(protocolInfos[i].szProtocol));
    }

}

void Widget::on_listWidget_Catlog_currentRowChanged(int currentRow)
{
    if(currentRow < 0)
        return ;

    ProtocolInfoForm* current_tab = dynamic_cast<ProtocolInfoForm*>(ui->tabWidget->currentWidget());
    if(!current_tab)
        return;
    current_tab->updateInfo(protocolInfos[currentRow]);
}

void Widget::on_pushButton_AddProtocolTab_clicked()
{
    static int count = 1;
    ui->tabWidget->insertTab(ui->tabWidget->currentIndex()+1, new ProtocolInfoForm, QString("Protocol %1").arg(++count));
    ui->tabWidget->setCurrentIndex(ui->tabWidget->currentIndex()+1);
}

void Widget::on_pushButton_DelProtocolTab_clicked()
{
    ui->tabWidget->removeTab(ui->tabWidget->currentIndex());
}

void Widget::on_pushButton_GenerateProviderId_clicked()
{
    GUID guid;
    CoCreateGuid(&guid);
    ui->lineEdit_ProviderId->setText(QString::asprintf("{%08x-%04hx-%04hx-%02hhx%02hhx-%02hhx%02hhx%02hhx%02hhx%02hhx%02hhx}",
                                                       (guid.Data1), (guid.Data2), (guid.Data3),
                                                       (guid.Data4[0]), (guid.Data4[1]), (guid.Data4[2]), (guid.Data4[3]),
                                                       (guid.Data4[4]), (guid.Data4[5]), (guid.Data4[6]), (guid.Data4[7])));

}

void Widget::on_pushButton_Install_clicked()
{
    WCHAR dll64Path[MAX_PATH] = {0};
    ui->lineEdit_DllPath64->text().toWCharArray(dll64Path);

    ProtocolInfoForm* currentTab = dynamic_cast<ProtocolInfoForm*>(ui->tabWidget->currentWidget());
    WSAPROTOCOL_INFOW info = {0};
    currentTab->collectInfo(&info);

    // Transform GUID string to GUID structure
    GUID guid;
    sscanf_s(ui->lineEdit_ProviderId->text().toStdString().c_str(), "{%8x-%4hx-%4hx-%2hhx%2hhx-%2hhx%2hhx%2hhx%2hhx%2hhx%2hhx}",
            &(guid.Data1), &(guid.Data2), &(guid.Data3),
            &(guid.Data4[0]), &(guid.Data4[1]), &(guid.Data4[2]), &(guid.Data4[3]),
            &(guid.Data4[4]), &(guid.Data4[5]), &(guid.Data4[6]), &(guid.Data4[7]));

    // Install Protocol
    int ret = 0;
    int err = 0;
    info.ProviderId = guid;
    info.dwCatalogEntryId = 0;

#ifdef WIN64
    ret = WSCInstallProvider64_32(&guid, dll64Path, &info, 1, &err);
    if(ret)
    {
        // failed
        QMessageBox::information(this, "error", QString::asprintf("WSCInstallProvider64_32 failed with error code: %d.", err));
    }
#else
    ret = WSCInstallProvider(&guid, dll64Path, &info, 1, &err);
    if(ret)
    {
        // failed
        QMessageBox::information(this, "error", QString::asprintf("WSCInstallProvider failed with error code: %d.", err));
    }
#endif

    on_pushButton_RefreshCatlog_clicked();
}

void Widget::on_pushButton_DeinstallProvider_clicked()
{
    // Transform GUID string to GUID structure
    GUID guid;
    sscanf_s(ui->lineEdit_ProviderId->text().toStdString().c_str(), "{%8x-%4hx-%4hx-%2hhx%2hhx-%2hhx%2hhx%2hhx%2hhx%2hhx%2hhx}",
            &(guid.Data1), &(guid.Data2), &(guid.Data3),
            &(guid.Data4[0]), &(guid.Data4[1]), &(guid.Data4[2]), &(guid.Data4[3]),
            &(guid.Data4[4]), &(guid.Data4[5]), &(guid.Data4[6]), &(guid.Data4[7]));
    int err;
    int ret;
    ret = WSCDeinstallProvider(&guid, &err);
    if(ret)
        QMessageBox::information(this, "error", QString::asprintf("WSCDeinstallProvider failed with error code: %d.", err));
#ifdef WIN64
    ret = WSCDeinstallProvider32(&guid, &err);
    if(ret)
        QMessageBox::information(this, "error", QString::asprintf("WSCDeinstallProvider32 failed with error code: %d.", err));
#endif
    on_pushButton_RefreshCatlog_clicked();

}

void Widget::on_pushButton_InstallAll_clicked()
{
    WCHAR dll64Path[MAX_PATH];
    ui->lineEdit_DllPath64->text().toWCharArray(dll64Path);

    // Collect all protocolinfos
    int protocolCount = ui->tabWidget->count();
    QVector<WSAPROTOCOL_INFOW> infos(protocolCount);
    for (int i = 0; i < protocolCount; ++i) {
        QWidget* w = ui->tabWidget->widget(i);
        ProtocolInfoForm* infoTab = dynamic_cast<ProtocolInfoForm*>(w);
        infoTab->collectInfo(&infos[i]);
    }

    // Transform GUID string to GUID structure
    GUID guid;
    sscanf_s(ui->lineEdit_ProviderId->text().toStdString().c_str(), "{%8x-%4hx-%4hx-%2hhx%2hhx-%2hhx%2hhx%2hhx%2hhx%2hhx%2hhx}",
            &(guid.Data1), &(guid.Data2), &(guid.Data3),
            &(guid.Data4[0]), &(guid.Data4[1]), &(guid.Data4[2]), &(guid.Data4[3]),
            &(guid.Data4[4]), &(guid.Data4[5]), &(guid.Data4[6]), &(guid.Data4[7]));

    // Install Protocol
    int ret = 0;
    int err = 0;

#ifdef WIN64
    ret = WSCInstallProvider64_32(&guid, dll64Path, infos.data(), infos.size(), &err);
    if(ret)
    {
        // failed
        QMessageBox::information(this, "error", QString::asprintf("WSCInstallProvider64_32 failed with error code: %d.", err));
    }
#else
    ret = WSCInstallProvider(&guid, dll64Path, infos.data(), infos.size(), &err);
    if(ret)
    {
        // failed
        QMessageBox::information(this, "error", QString::asprintf("WSCInstallProvider failed with error code: %d.", err));
    }
#endif


    on_pushButton_RefreshCatlog_clicked();
}

void Widget::on_pushButton_TopAll_clicked()
{
    int count = ui->tabWidget->count();
    QVector<DWORD> entryIds(count);

    for (int i = 0; i < count; ++i) {
        ProtocolInfoForm* infoTab = dynamic_cast<ProtocolInfoForm*>(ui->tabWidget->widget(i));
        WSAPROTOCOL_INFOW info;
        infoTab->collectInfo(&info);
        entryIds[i] = info.dwCatalogEntryId;
    }

    DWORD bufferLen;
    INT err;
    WSCEnumProtocols(NULL, NULL, &bufferLen, &err);
    QVector<WSAPROTOCOL_INFOW> infos(bufferLen/sizeof(WSAPROTOCOL_INFOW));

    // Enum protocols
    int infoNumber = WSCEnumProtocols(NULL, infos.data(), &bufferLen, &err);
    if(infoNumber == SOCKET_ERROR)
    {
        QMessageBox::information(this, "error", QString::asprintf("WSAEnumProtocols failed with error: %d.", err));
        return;
    }

    // Reorder protocols with entryIds to first
    QVector<DWORD> order;
    order.append(entryIds);
    for (int i = 0; i < infoNumber; ++i) {
        DWORD id = infos[i].dwCatalogEntryId;
        if(!entryIds.contains(id))
            order.append(id);
    }

    err = WSCWriteProviderOrder(order.data(), order.size());
    if( err != ERROR_SUCCESS )
    {
        QMessageBox::information(this, "error", QString::asprintf("WSCWriteProviderOrder failed with error: %d.", err));
    }

#ifdef WIN64
    // // Reorder protocols for 32bit apps
    WSCEnumProtocols32(NULL, NULL, &bufferLen, &err);
    infos.clear();
    infos.resize(bufferLen/sizeof(WSAPROTOCOL_INFOW));
    infoNumber = WSCEnumProtocols32(NULL, infos.data(), &bufferLen, &err);
    if(infoNumber == SOCKET_ERROR)
    {
        QMessageBox::information(this, "error", QString::asprintf("WSAEnumProtocols32 failed with error: %d.", err));
        return;
    }
    order.clear();
    order.append(entryIds);
    for (int i = 0; i < infoNumber; ++i) {
        DWORD id = infos[i].dwCatalogEntryId;
        if(!entryIds.contains(id))
            order.append(id);
    }
    err = WSCWriteProviderOrder32(order.data(), order.size());
    if( err != ERROR_SUCCESS )
    {
        QMessageBox::information(this, "error", QString::asprintf("WSCWriteProviderOrder32 failed with error: %d.", err));
    }

#endif

    on_pushButton_RefreshCatlog_clicked();

}
