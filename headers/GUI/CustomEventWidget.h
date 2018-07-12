//
// Created by chenkuan on 2018/2/20.
//

#ifndef IOTEVENTMONITORPLATFORM_CUSTOMEVENTWIDGET_H
#define IOTEVENTMONITORPLATFORM_CUSTOMEVENTWIDGET_H


#include <QtWidgets/QWidget>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QTextBrowser>
#include <tinyxml2.h>

using namespace tinyxml2;

class CustomEventWidget : public QWidget {
  Q_OBJECT

  public:
    /**
     * 默认包含显示右边文字注释的
     * @param parent
     * @param containInstruction
     */
    explicit CustomEventWidget(QWidget *parent);

    CustomEventWidget(const CustomEventWidget *eventWidget);

    void loadConf(XMLElement *eventConf);

    void saveConf(XMLElement *eventConf);

    /**
     * 返回拼接的完整事件内容
     * @return
     */
    QString text();

    QString getEventHead() const;

    QString getEventBody() const;

    QString getEventTail() const;

  signals:

    void eventContentChanged();

  private:
    /**
     * 使用网格布局
     */
    QGridLayout *gridLayout = nullptr;

    QTextEdit *upperOutLayer = nullptr;
    QTextEdit *middleLayer = nullptr;
    QTextEdit *bottomOutLayer = nullptr;
    QTextBrowser *upperIndicator = nullptr;
    QTextBrowser *middleIndicator = nullptr;
    QTextBrowser *bottomIndicator = nullptr;

    void init(bool showContent, bool showInstruction);

    void initContent();

    void initInstruction();
};


#endif //IOTEVENTMONITORPLATFORM_CUSTOMEVENTWIDGET_H
