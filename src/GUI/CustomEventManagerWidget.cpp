//
// Created by chenkuan on 2018/2/20.
//

#include <CustomEventManagerWidget.h>
#include <CustomEventWidget.h>
#include <GUIConf.h>
#include <GUIConf.h>
#include <moc_CustomEventManagerWidget.cpp>

CustomEventManagerWidget::CustomEventManagerWidget(QWidget *parent) : QWidget(parent) {
    hBoxLayout = new QHBoxLayout(this);
    leftVBoxLayout = new QVBoxLayout();
    eventStackedWidget = new QStackedWidget(this);

    // 完成左右布局
    hBoxLayout->addLayout(leftVBoxLayout);
    hBoxLayout->addWidget(eventStackedWidget);

    eventListWidget = new QListWidget(this);
    addButton = new QPushButton(ADD_EVENT, this);
    deleteButton = new QPushButton(DELETE_EVENT, this);

    // 完成左侧的上下布局
    leftVBoxLayout->addWidget(eventListWidget);
    leftVBoxLayout->addWidget(addButton);
    leftVBoxLayout->addWidget(deleteButton);

    // 风格设置
    hBoxLayout->setContentsMargins(0, 0, 0, 0);
    hBoxLayout->setSpacing(0);

    leftVBoxLayout->setContentsMargins(0, 0, 0, 0);
    leftVBoxLayout->setSpacing(0);

    eventListWidget->setFixedWidth(EVENT_TAB_WIDTH);

    // 添加和删除事件的响应
    QObject::connect(addButton, SIGNAL(clicked()), this, SLOT(addEvent()));
    QObject::connect(deleteButton, SIGNAL(clicked()), this, SLOT(deleteEvent()));

    // 双击list的item响应，设置item为可编辑
    QObject::connect(eventListWidget,
                     SIGNAL(itemDoubleClicked(QListWidgetItem * )),
                     this,
                     SLOT(editListItem(QListWidgetItem * )));

    // list的item内容变化响应，item内容变化时emit信号
    QObject::connect(eventListWidget,
                     SIGNAL(itemChanged(QListWidgetItem * )),
                     this,
                     SLOT(afterEditEventName(QListWidgetItem * )));

    // 使list与stackedWidget对应
    QObject::connect(eventListWidget,
                     SIGNAL(currentRowChanged(int)),
                     eventStackedWidget,
                     SLOT(setCurrentIndex(int)));
}

void CustomEventManagerWidget::loadConf(XMLElement *eventsConf) {
    this->eventsConf = eventsConf;

    // 遍历所有<event>节点
    for (auto eventConf = eventsConf->FirstChildElement(EVENT_TAG);
         eventConf != nullptr;
         eventConf = eventConf->NextSiblingElement(EVENT_TAG)) {
        // 将事件名称添加到list中
        const char *eventName = eventConf->Attribute(EVENT_NAME_ATTR);
        eventListWidget->addItem(eventName);
        // 添加一个eventWidget并传入事件内容
        auto eventWidget = new CustomEventWidget(this);
        eventWidget->loadConf(eventConf);
        eventStackedWidget->addWidget(eventWidget);
        // 为每个添加的eventWiget添加一个内容变化的消息响应
        QObject::connect(eventWidget, SIGNAL(eventContentChanged()),
                         this, SLOT(afterEditEventContent()));

        // 发送添加事件的信号
        emit insertEvent(eventListWidget->count() - 1, eventName, eventWidget);
    }

    // 根据属性currentEventName选择到相应的事件显示
    int currentRow = 0;
    const char *currentEventName = eventsConf->Attribute(CURRENT_EVENT_NAME_ATTR);
    for (int row = 0; row < eventListWidget->count(); ++row) {
        auto item = eventListWidget->item(row);
        if (item->text() == currentEventName) {
            currentRow = row;
            break;
        }
    }
    eventListWidget->setCurrentRow(currentRow);
    eventStackedWidget->setCurrentIndex(currentRow);
}

void CustomEventManagerWidget::saveConf() {
    // 先清空存储的旧的events
    eventsConf->DeleteChildren();

    auto currentItem = eventListWidget->currentItem();
    if (currentItem != nullptr) {
        eventsConf->SetAttribute(CURRENT_EVENT_NAME_ATTR, currentItem->text().toStdString().c_str());
    } else {
        eventsConf->SetAttribute(CURRENT_EVENT_NAME_ATTR, "");
    }

    for (int row = 0; row < eventListWidget->count(); ++row) {
        // 插入一个event节点
        XMLElement *eventElement = eventsConf->GetDocument()->NewElement(EVENT_TAG);
        eventsConf->InsertEndChild(eventElement);

        // 节点中添加事件名称
        eventElement->SetAttribute(EVENT_NAME_ATTR, eventListWidget->item(row)->text().toStdString().c_str());
        // 节点中添加事件内容
        dynamic_cast<CustomEventWidget *>(eventStackedWidget->widget(row))->saveConf(eventElement);
    }
}

void CustomEventManagerWidget::addEvent() {
    int row = eventListWidget->count();
    QString eventName = EVENT + QString::number(row + 1);

    emit showStatusMessage("添加事件：\"" + eventName + "\"");

    eventListWidget->addItem(eventName);
    eventListWidget->setCurrentRow(row);

    auto eventWidget = new CustomEventWidget(this);
    eventStackedWidget->addWidget(eventWidget);
    eventStackedWidget->setCurrentIndex(row);
    // 为每个添加的eventWiget添加一个内容变化的消息响应
    QObject::connect(eventWidget, SIGNAL(eventContentChanged()),
                     this, SLOT(afterEditEventContent()));

    emit insertEvent(row, eventName, eventWidget);
}

void CustomEventManagerWidget::deleteEvent() {
    int currentRow = eventListWidget->currentRow();
    if (currentRow < 0) return;

    emit showStatusMessage("移除事件：\"" + eventListWidget->currentItem()->text() + "\"");

    delete eventListWidget->takeItem(currentRow);

    QWidget *currentEventWidget = eventStackedWidget->widget(currentRow);
    eventStackedWidget->removeWidget(currentEventWidget);
    delete currentEventWidget;

    emit removeEvent(currentRow);
}

void CustomEventManagerWidget::editListItem(QListWidgetItem *item) {
    emit showStatusMessage("编辑事件：\"" + item->text() + "\"");
    item->setFlags(item->flags() | Qt::ItemIsEditable);
}

void CustomEventManagerWidget::afterEditEventName(QListWidgetItem *item) {
    // 非当前行发生变化则忽略处理
    if (eventListWidget->currentItem() != item) return;

    logger->debug("after modify, item->text:%s", item->text().toLocal8Bit().data());
    int row = eventListWidget->currentRow();
    if (row < 0) return;
    auto eventWidget = dynamic_cast<CustomEventWidget *>(eventStackedWidget->widget(row));
    emit modifyEvent(row, item->text(), eventWidget);
}

void CustomEventManagerWidget::afterEditEventContent() {
    int row = eventListWidget->currentRow();
    if (row < 0) return;

    auto item = eventListWidget->item(row);
    auto eventWidget = dynamic_cast<CustomEventWidget *>(eventStackedWidget->widget(row));
    logger->debug("after modify, item->event->text:\n%s", eventWidget->text().toLocal8Bit().data());

    emit modifyEvent(row, item->text(), eventWidget);
}
