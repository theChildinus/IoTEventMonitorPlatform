//
// Created by chenkuan on 2018/2/20.
//

#include <CustomMainWindow.h>
#include <GUIConf.h>
#include <moc_CustomMainWindow.cpp>

CustomMainWindow::CustomMainWindow(QWidget *parent) : QMainWindow(parent) {
    tabWidget = new CustomTabWidget(this);
    statusBar = new QStatusBar(this);

    setCentralWidget(tabWidget);
    setStatusBar(statusBar);

    setContentsMargins(0, 0, 0, 0);
    resize(MAIN_WINDOW_WIDTH, MAIN_WINDOW_HEIGHT);
    setWindowTitle(MAIN_WINDOW_TITLE);

    QObject::connect(tabWidget,
                     SIGNAL(showStatusMessage(
                                    const QString &)),
                     statusBar,
                     SLOT(showMessage(
                                  const QString &)));
}

void CustomMainWindow::closeEvent(QCloseEvent *event) {
    statusBar->showMessage("保存配置中...");
    tabWidget->saveConf();
    QWidget::closeEvent(event);
}
