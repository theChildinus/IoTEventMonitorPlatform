#include <Logger.h>
#include <QApplication>
#include <CustomMainWindow.h>

int main(int argc, char *argv[])
{
    Logger *logger = Logger::getLogger();
    logger->setLogLevel(debug);
    logger->debug("开始程序测试");

    if (argc == 2) {
        if (strcmp(argv[1], "debug") == 0) {
            logger->setLogLevel(debug);
        } else if (strcmp(argv[1], "info") == 0) {
            logger->setLogLevel(info);
        } else if (strcmp(argv[1], "warning") == 0) {
            logger->setLogLevel(warning);
        } else if (strcmp(argv[1], "error") == 0) {
            logger->setLogLevel(error);
        } else {
            logger->error("Usage: sudo ./IoTEventMonitorPlatform [debug|info|warning|error]\n default is info");
            return -1;
        }
    } else if (argc > 2) {
        logger->error("Usage: sudo ./IoTEventMonitorPlatform [debug|info|warning|error]\n default is info");
        return -1;
    }

    QApplication a(argc, argv);

    CustomMainWindow mainWindow;
    mainWindow.show();

    return a.exec();
}


