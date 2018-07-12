//
// Created by chenkuan on 2018/2/20.
//

#include <CustomRunWidget.h>
#include <GUIConf.h>
#include <QtWidgets/QFileDialog>
#include <CustomEventWidget.h>
#include <moc_CustomRunWidget.cpp>

CustomRunWidget::CustomRunWidget(QWidget *parent) : QWidget(parent) {
    hBoxLayout = new QHBoxLayout(this);
    leftGridLayout = new QGridLayout();
    logTextBrowser = new QTextBrowser(this);

    // 完成左右布局
    hBoxLayout->addLayout(leftGridLayout, 2);
    hBoxLayout->addWidget(logTextBrowser, 8);

    eventLabel = new QLabel(EVENT_SELECT_LABEL, this);
    eventComboBox = new QComboBox(this);

    eventPreviewStackedWidget = new QStackedWidget(this);

    vmIpLabel = new QLabel(VM_IP_LABEL, this);
    vmIpEdit = new CustomIpEdit(this);

    externalIpLabel = new QLabel(EXTERNAL_IP_LABEL, this);
    externalIpEdit = new CustomIpEdit(this);

    pseudoTerminalLabel = new QLabel(PSEUDO_TERMINAL_LABEL, this);
    pseudoTerminalEdit = new QLineEdit(this);

    serialPortLabel = new QLabel(HOST_SERIAL_PORT_LABEL, this);
    serialPortEdit = new QLineEdit(this);

    stateMachineSelectLabel = new QLabel(STATE_MACHINE_SELECT_LABEL, this);
    stateMachineEdit = new CustomLineEdit(this);

    runButton = new QPushButton(RUN_BUTTON, this);
    clearButton = new QPushButton(CLEAR_BUTTON, this);

    // 完成左侧的网格布局
    leftGridLayout->addWidget(eventLabel, 0, 0);
    leftGridLayout->addWidget(eventComboBox, 0, 1);

    leftGridLayout->addWidget(eventPreviewStackedWidget, 1, 0, 1, 2);

    leftGridLayout->addWidget(vmIpLabel, 2, 0);
    leftGridLayout->addWidget(vmIpEdit, 2, 1);

    leftGridLayout->addWidget(externalIpLabel, 3, 0);
    leftGridLayout->addWidget(externalIpEdit, 3, 1);

    leftGridLayout->addWidget(pseudoTerminalLabel, 4, 0);
    leftGridLayout->addWidget(pseudoTerminalEdit, 4, 1);

    leftGridLayout->addWidget(serialPortLabel, 5, 0);
    leftGridLayout->addWidget(serialPortEdit, 5, 1);

    leftGridLayout->addWidget(stateMachineSelectLabel, 6, 0);
    leftGridLayout->addWidget(stateMachineEdit, 6, 1);

    leftGridLayout->addWidget(runButton, 7, 0, 1, 2);
    leftGridLayout->addWidget(clearButton, 8, 0, 1, 2);

    // 设置间距风格
    hBoxLayout->setContentsMargins(0, 0, 0, 0);
    hBoxLayout->setSpacing(0);

    leftGridLayout->setContentsMargins(0, 0, 0, 0);
    leftGridLayout->setSpacing(0);

    // 后台处理线程类
    eventManager = new EventManager(this);

    // 设置下拉框与预览框对应
    QObject::connect(eventComboBox, SIGNAL(activated(int)),
                     eventPreviewStackedWidget, SLOT(setCurrentIndex(int)));

    // 设置状态机文件输入框点击的响应
    QObject::connect(stateMachineEdit, SIGNAL(clicked()),
                     this, SLOT(selectStateMachineFile()));

    // 启动按钮响应
    QObject::connect(runButton, SIGNAL(clicked()),
                     this, SLOT(runButtonClicked()));

    // 清除按钮
    QObject::connect(clearButton, SIGNAL(clicked()),
                     logTextBrowser, SLOT(clear()));

    // 后台线程结束信号
    QObject::connect(eventManager, SIGNAL(finished()),
                     this, SLOT(threadFinished()));

    // 后台线程往GUI输出日志信号
    QObject::connect(eventManager,
                     SIGNAL(showLogMessage(
                                    const QString &)),
                     this,
                     SLOT(showLogMessage(
                                  const QString &)));

}

void CustomRunWidget::loadConf(XMLElement *runConf) {
    this->runConf = runConf;

    int currentEventIndex = eventComboBox->findText(runConf->Attribute(CURRENT_EVENT_NAME_ATTR));
    eventComboBox->setCurrentIndex(currentEventIndex);
    eventPreviewStackedWidget->setCurrentIndex(currentEventIndex);

    vmIpEdit->setIp(runConf->Attribute(VM_IP_ATTR));
    externalIpEdit->setIp(runConf->Attribute(EXTERNAL_IP_ATTR));
    pseudoTerminalEdit->setText(runConf->Attribute(PSEUDO_TERMINAL_ATTR));
    serialPortEdit->setText(runConf->Attribute(SERIAL_PORT_ATTR));
    stateMachineEdit->setText(runConf->Attribute(STATE_MACHINE_FILE_PATH_ATTR));
}

void CustomRunWidget::saveConf() {
    runConf->SetAttribute(CURRENT_EVENT_NAME_ATTR, eventComboBox->currentText().toStdString().c_str());
    runConf->SetAttribute(VM_IP_ATTR, vmIpEdit->getIp().toStdString().c_str());
    runConf->SetAttribute(EXTERNAL_IP_ATTR, externalIpEdit->getIp().toStdString().c_str());
    runConf->SetAttribute(PSEUDO_TERMINAL_ATTR, pseudoTerminalEdit->text().toStdString().c_str());
    runConf->SetAttribute(SERIAL_PORT_ATTR, serialPortEdit->text().toStdString().c_str());
    runConf->SetAttribute(STATE_MACHINE_FILE_PATH_ATTR, stateMachineEdit->text().toStdString().c_str());
}

void CustomRunWidget::insertEvent(int index, const QString &eventName, CustomEventWidget *eventWidget) {
    eventComboBox->insertItem(index, eventName);

    eventPreviewStackedWidget->insertWidget(index, new CustomEventWidget(eventWidget));
}

void CustomRunWidget::removeEvent(int index) {
    eventComboBox->removeItem(index);

    auto eventWidget = eventPreviewStackedWidget->widget(index);
    eventPreviewStackedWidget->removeWidget(eventWidget);
    delete (eventWidget);
}

void CustomRunWidget::modifyEvent(int index, const QString &eventName, CustomEventWidget *eventWidget) {
    int currentIndex = eventComboBox->currentIndex();
    removeEvent(index);
    insertEvent(index, eventName, eventWidget);
    eventComboBox->setCurrentIndex(currentIndex);
    eventPreviewStackedWidget->setCurrentIndex(currentIndex);
}

void CustomRunWidget::selectStateMachineFile() {
    QString stateMachineFilePath = QFileDialog::getOpenFileName(this,
                                                                tr(SELECT_FILE_DIALOG_TITLE),
                                                                ".",
                                                                tr("XML files (*.xml)"));
    if (!stateMachineFilePath.isEmpty()) {
        stateMachineEdit->setText(stateMachineFilePath);
    }
}

void CustomRunWidget::runButtonClicked() {
    // 判断按钮状态调用不同的函数
    if (runButton->text() == "启动") {
        run();
    } else {
        stop();
    }
}

void CustomRunWidget::showLogMessage(const QString &message) {
    logTextBrowser->insertPlainText(message);
    logTextBrowser->insertPlainText("\n");
    logTextBrowser->moveCursor(QTextCursor::End);
}


void CustomRunWidget::threadFinished() {
    // 结束执行后更改各组件状态
    changeWidgetState(true);
    runButton->setText("启动");
    emit showStatusMessage("后台线程已结束");
}

void CustomRunWidget::run() {

    emit showStatusMessage("后台线程运行中...");

    // 开始执行后更改各组件状态
    changeWidgetState(false);
    runButton->setText("停止");

    // 启动处理线程
    auto eventWidget = dynamic_cast<CustomEventWidget *>(eventPreviewStackedWidget->currentWidget());

    eventManager->setEventConf(eventWidget->getEventHead(), eventWidget->getEventTail());
    eventManager->setNetfilterConf(vmIpEdit->getIp(), externalIpEdit->getIp());
    eventManager->setSerialPortConf(pseudoTerminalEdit->text(), this->serialPortEdit->text());
    eventManager->setStateMachineConf(stateMachineEdit->text());
    eventManager->start();
}

void CustomRunWidget::stop() {
    emit showStatusMessage("后台线程结束中...");

    // 结束处理线程
    eventManager->stop();
    eventManager->wait();

    emit showStatusMessage("后台线程已结束");
}

void CustomRunWidget::changeWidgetState(bool isEnabled) {
    eventComboBox->setEnabled(isEnabled);
    vmIpEdit->setEnabled(isEnabled);
    externalIpEdit->setEnabled(isEnabled);
    pseudoTerminalEdit->setEnabled(isEnabled);
    serialPortEdit->setEnabled(isEnabled);
    stateMachineEdit->setEnabled(isEnabled);
}
