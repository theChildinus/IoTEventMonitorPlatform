//
// Created by chenkuan on 2018/2/20.
//

#include <CustomTabWidget.h>
#include <GUIConf.h>
#include <moc_CustomTabWidget.cpp>

CustomTabWidget::CustomTabWidget(QWidget *parent) : QTabWidget(parent) {
    eventManagerWidget = new CustomEventManagerWidget(this);
    runWidget = new CustomRunWidget(this);

    addTab(eventManagerWidget, EVENT_MANAGER_TITLE);
    addTab(runWidget, RUN_TITLE);

    // 设置间距风格
    setContentsMargins(0, 0, 0, 0);

    // 将eventManagerWidget中的事件列表变更信号与runWidget中的信号槽连接起来
    QObject::connect(eventManagerWidget,
                     SIGNAL(insertEvent(int,
                                    const QString&, CustomEventWidget *)),
                     runWidget,
                     SLOT(insertEvent(int,
                                  const QString&, CustomEventWidget *)));
    QObject::connect(eventManagerWidget,
                     SIGNAL(removeEvent(int)),
                     runWidget,
                     SLOT(removeEvent(int)));
    QObject::connect(eventManagerWidget,
                     SIGNAL(modifyEvent(int,
                                    const QString&, CustomEventWidget *)),
                     runWidget,
                     SLOT(modifyEvent(int,
                                  const QString&, CustomEventWidget *)));

    // 载入配置
    loadConf();
    // 将各页面配置下发
    eventManagerWidget->loadConf(eventsConf);
    runWidget->loadConf(runConf);

    QObject::connect(eventManagerWidget,
                     SIGNAL(showStatusMessage(
                                    const QString &)),
                     this,
                     SIGNAL(showStatusMessage(
                                    const QString &)));
    QObject::connect(runWidget,
                     SIGNAL(showStatusMessage(
                                    const QString &)),
                     this,
                     SIGNAL(showStatusMessage(
                                    const QString &)));
}

void CustomTabWidget::loadConf() {
    // 按配置文件、默认配置文件、默认配置内容顺序选择
    if (GUIConf.LoadFile(GUI_CONF_FILE) != XML_SUCCESS || !parseConf()) {
        if (GUIConf.LoadFile(GUI_CONF_DEFAULT_FILE) != XML_SUCCESS || !parseConf()) {
            GUIConf.Parse(GUI_CONF_TEMPLATE);
            parseConf();
        }
    }
}

bool CustomTabWidget::parseConf() {
    XMLElement *root = GUIConf.FirstChildElement();
    if (root == nullptr
        || strcmp(root->Name(), ROOT_TAG) != 0
        || strcmp(root->Attribute(PROJECT_NAME_ATTR),
                  PROJECT_NAME_ATTR_VALUE) != 0) {
        return false;
    }

    eventsConf = root->FirstChildElement(EVENTS_TAG);
    if (eventsConf == nullptr) {
        eventsConf = GUIConf.NewElement(EVENTS_TAG);
        root->InsertEndChild(eventsConf);
    }

    runConf = root->FirstChildElement(RUN_TAG);
    if (runConf == nullptr) {
        runConf = GUIConf.NewElement(RUN_TAG);
        root->InsertEndChild(runConf);
    }

    return true;
}

void CustomTabWidget::saveConf() {
    eventManagerWidget->saveConf();
    runWidget->saveConf();

    GUIConf.SaveFile(GUI_CONF_FILE);
}

void CustomTabWidget::paintEvent(QPaintEvent *event) {
    QWidget::paintEvent(event);
    // 只有当界面宽度变化时才进行标签的重绘
    if (currentWidth != width()) {
        currentWidth = width();
        changeTabStyle();
    }
}

void CustomTabWidget::changeTabStyle() {
    this->setStyleSheet((
                                "QTabWidget::pane {"
                                        "border: none;"
                                        "}"
                                        "QTabWidget::tab-bar {"
                                        "border: none;"
                                        "}"
                                        "QTabBar::tab {"
                                        "min-width: " + std::to_string(currentWidth / 2) + "px;"
                                        "height: " + std::to_string(MAIN_TAB_HEIGHT) + "px;"
                                        "}"
                        ).c_str());
}
