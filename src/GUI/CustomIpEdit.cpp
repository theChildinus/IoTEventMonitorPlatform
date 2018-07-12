//
// Created by yingzi on 2017/10/8.
//

#include <CustomIpEdit.h>
#include <moc_CustomIpEdit.cpp>

CustomIpEdit::CustomIpEdit(QWidget *parent) : QWidget(parent) {
    hBoxLayout = new QHBoxLayout(this);
    for (auto &ipEdit : ipEdits) {
        ipEdit = new QLineEdit("0", this);
    }
    for (auto &pointLabel : pointLabels) {
        pointLabel = new QLabel(".", this);
    }

    // TODO 对ip框加入输入校验器
//    for (auto& ipEdit : ipEdits) {
//        ipEdit->setValidator(new QValidator());
//    }

    for (int i = 0; i < 3; ++i) {
        hBoxLayout->addWidget(ipEdits[i]);
        hBoxLayout->addWidget(pointLabels[i]);
    }
    hBoxLayout->addWidget(ipEdits.back());
    this->changeStyle();
}


void CustomIpEdit::setIp(const char *ip) {
    QString ipStr(ip);
    QStringList ipStrList = ipStr.split(".");
    for (int i = 0; i < 4; ++i) {
        ipEdits[i]->setText(std::to_string(ipStrList[i].toInt()).c_str());
    }
}

QString CustomIpEdit::getIp() {
    return ipEdits[0]->text() +
           "." + ipEdits[1]->text() +
           "." + ipEdits[2]->text() +
           "." + ipEdits[3]->text();
}

void CustomIpEdit::changeStyle() {
    this->setMaximumWidth(200);
    hBoxLayout->setContentsMargins(0, 0, 0, 0);
    hBoxLayout->setSpacing(0);

    for (auto &ipEdit : ipEdits) {
        ipEdit->setContentsMargins(0, 0, 0, 0);
    }
    for (auto &pointLabel : pointLabels) {
        pointLabel->setMaximumWidth(3);
        pointLabel->setContentsMargins(0, 0, 0, 0);
    }
}