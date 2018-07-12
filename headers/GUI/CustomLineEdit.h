//
// Created by chenkuan on 2018/2/24.
//

#ifndef IOTEVENTMONITORPLATFORM_CUSTOMLINEEDIT_H
#define IOTEVENTMONITORPLATFORM_CUSTOMLINEEDIT_H


#include <QtWidgets/QLineEdit>
#include <QMouseEvent>

class CustomLineEdit : public QLineEdit {
  Q_OBJECT

  public:
    explicit CustomLineEdit(QWidget *parent = nullptr);

  signals:

    void clicked();

  protected:
    virtual void mousePressEvent(QMouseEvent *event) override;


};


#endif //IOTEVENTMONITORPLATFORM_CUSTOMLINEEDIT_H
