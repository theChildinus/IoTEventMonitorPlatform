//
// Created by chenkuan on 2018/2/20.
//

#ifndef IOTEVENTMONITORPLATFORM_CUSTOMRUNWIDGET_H
#define IOTEVENTMONITORPLATFORM_CUSTOMRUNWIDGET_H


#include <QtWidgets/QWidget>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QTextBrowser>
#include <QtWidgets/QLabel>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QPushButton>
#include <CustomIpEdit.h>
#include <QtWidgets/QStackedWidget>
#include <tinyxml2.h>
#include <CustomLineEdit.h>
#include <CustomEventWidget.h>
#include <EventManager.h>

using namespace tinyxml2;

class CustomRunWidget : public QWidget {
  Q_OBJECT

  public:
    explicit CustomRunWidget(QWidget *parent = nullptr);

    void loadConf(XMLElement *runConf);

    void saveConf();

  signals:

    void showStatusMessage(const QString &);

  protected slots:

    void insertEvent(int index, const QString &eventName, CustomEventWidget *eventWidget);

    void removeEvent(int index);

    void modifyEvent(int index, const QString &eventName, CustomEventWidget *eventWidget);

    void selectStateMachineFile();

    void runButtonClicked();

    void showLogMessage(const QString &message);

    /**
     * 后台线程结束事件，进行页面内恢复工作
     */
    void threadFinished();

  private:
    /**
     * 整体采用左右布局，左右可设2:8宽度比
     */
    QHBoxLayout *hBoxLayout = nullptr;

    /**
     * 左侧采用网格布局
     */
    QGridLayout *leftGridLayout = nullptr;
    // 事件选择下拉框
    QLabel *eventLabel = nullptr;
    QComboBox *eventComboBox = nullptr;
    // 事件预览
    QStackedWidget *eventPreviewStackedWidget = nullptr;
    // 网络监控配置
    QLabel *vmIpLabel = nullptr;
    CustomIpEdit *vmIpEdit = nullptr;
    QLabel *externalIpLabel = nullptr;
    CustomIpEdit *externalIpEdit = nullptr;
    // 串口监控配置
    QLabel *pseudoTerminalLabel = nullptr;
    QLineEdit *pseudoTerminalEdit = nullptr;
    QLabel *serialPortLabel = nullptr;
    QLineEdit *serialPortEdit = nullptr;
    // 状态机文件选择
    QLabel *stateMachineSelectLabel = nullptr;
    CustomLineEdit *stateMachineEdit = nullptr;

    // 启动与停止
    QPushButton *runButton = nullptr;
    // 清屏按钮
    QPushButton *clearButton = nullptr;

    /**
     * 右侧使用一个QTextBrowser显示信息
     */
    QTextBrowser *logTextBrowser = nullptr;

    XMLElement *runConf = nullptr;

    /**
     * 后台处理线程，用来与各个客户端连接并验证事件
     */
    EventManager *eventManager;
    /**
     * 启动后台处理线程
     */
    void run();

    /**
     * 停止后台处理线程
     */
    void stop();

    /**
     * 改变页面所有可编辑控件的状态
     * @param isEnabled
     */
    void changeWidgetState(bool isEnabled);
};


#endif //IOTEVENTMONITORPLATFORM_CUSTOMRUNWIDGET_H
