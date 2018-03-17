//
// Created by chenkuan on 2018/2/20.
//

#include <CustomEventWidget.h>
#include <GUIConf.h>
#include <moc_CustomEventWidget.cpp>

CustomEventWidget::CustomEventWidget(QWidget *parent) : QWidget(parent) {
    init(true, true);

    // 设置默认内容
    upperOutLayer->setText(DEFAULT_EVENT_MATCH_HEAD);
    middleLayer->setText(DEFAULT_EVENT_MATCH_BODY);
    bottomOutLayer->setText(DEFAULT_EVENT_MATCH_TAIL);
    upperIndicator->setText(EVENT_MATCH_HEAD_DESC);
    middleIndicator->setText(EVENT_MATCH_BODY_DESC);
    bottomIndicator->setText(EVENT_MATCH_TAIL_DESC);

    // 添加内容变化的响应
    QObject::connect(upperOutLayer, SIGNAL(textChanged()), this, SIGNAL(eventContentChanged()));
    QObject::connect(middleLayer, SIGNAL(textChanged()), this, SIGNAL(eventContentChanged()));
    QObject::connect(bottomOutLayer, SIGNAL(textChanged()), this, SIGNAL(eventContentChanged()));
}


void CustomEventWidget::init(bool showContent, bool showInstruction) {
    gridLayout = new QGridLayout(this);

    if (showContent) {
        initContent();
    }
    if (showInstruction) {
        initInstruction();
    }
    if (showContent && showInstruction) {
        // 设置布局比例
        gridLayout->setColumnStretch(0, 9);
        gridLayout->setColumnStretch(1, 1);
    }
    if (showContent || showInstruction) {
        gridLayout->setRowStretch(0, 2);
        gridLayout->setRowStretch(1, 6);
        gridLayout->setRowStretch(2, 2);
    }

    // 设置间距风格
    gridLayout->setContentsMargins(0, 0, 0, 0);
    gridLayout->setSpacing(0);
}

void CustomEventWidget::initContent() {
    upperOutLayer = new QTextEdit(this);
    middleLayer = new QTextEdit(this);
    bottomOutLayer = new QTextEdit(this);

    // 完成网格布局
    gridLayout->addWidget(upperOutLayer, 0, 0);
    gridLayout->addWidget(middleLayer, 1, 0);
    gridLayout->addWidget(bottomOutLayer, 2, 0);
}

void CustomEventWidget::initInstruction() {
    upperIndicator = new QTextBrowser(this);
    middleIndicator = new QTextBrowser(this);
    bottomIndicator = new QTextBrowser(this);

    // 完成网格布局
    gridLayout->addWidget(upperIndicator, 0, 1);
    gridLayout->addWidget(middleIndicator, 1, 1);
    gridLayout->addWidget(bottomIndicator, 2, 1);
}

CustomEventWidget::CustomEventWidget(const CustomEventWidget *eventWidget) {
    init(true, false);

    upperOutLayer->setReadOnly(true);
    middleLayer->setReadOnly(true);
    bottomOutLayer->setReadOnly(true);

    upperOutLayer->setText(eventWidget->getEventHead());
    middleLayer->setText(eventWidget->getEventBody());
    bottomOutLayer->setText(eventWidget->getEventTail());
}

void CustomEventWidget::loadConf(XMLElement *eventConf) {
    auto headConf = eventConf->FirstChildElement(EVENT_HEAD_TAG);
    auto bodyConf = eventConf->FirstChildElement(EVENT_BODY_TAG);
    auto tailConf = eventConf->FirstChildElement(EVENT_TAIL_TAG);

    if (headConf) upperOutLayer->setText(headConf->GetText());
    if (bodyConf) middleLayer->setText(bodyConf->GetText());
    if (tailConf) bottomOutLayer->setText(tailConf->GetText());

}

void CustomEventWidget::saveConf(XMLElement *eventConf) {
    // 创建各个事件组件节点
    XMLElement *headElement = eventConf->GetDocument()->NewElement(EVENT_HEAD_TAG);
    XMLElement *bodyElement = eventConf->GetDocument()->NewElement(EVENT_BODY_TAG);
    XMLElement *tailElement = eventConf->GetDocument()->NewElement(EVENT_TAIL_TAG);

    // 设置各个节点内容
    headElement->SetText(upperOutLayer->toPlainText().toStdString().c_str());
    bodyElement->SetText(middleLayer->toPlainText().toStdString().c_str());
    tailElement->SetText(bottomOutLayer->toPlainText().toStdString().c_str());

    // 插入各个事件组件节点
    eventConf->InsertEndChild(headElement);
    eventConf->InsertEndChild(bodyElement);
    eventConf->InsertEndChild(tailElement);
}

QString CustomEventWidget::text() {
    return upperOutLayer->toPlainText()
           + '\n'
           + middleLayer->toPlainText()
           + '\n'
           + bottomOutLayer->toPlainText();
}

QString CustomEventWidget::getEventHead() const {
    return upperOutLayer->toPlainText();
}

QString CustomEventWidget::getEventBody() const {
    return middleLayer->toPlainText();
}

QString CustomEventWidget::getEventTail() const {
    return bottomOutLayer->toPlainText();
}