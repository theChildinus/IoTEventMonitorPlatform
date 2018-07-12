//
// Created by yingzi on 2017/11/8.
//

#include <EventManager.h>
#include <moc_EventManager.cpp>

EventManager::EventManager(QObject *parent) : QThread(parent) {
    netfilterClient = new NetfilterClient();
    serialPortRepeater = new SerialPortRepeater();
    memoryClient = new MemoryClient();
}

EventManager::~EventManager() {
    stop();
    wait();
    delete (serialPortRepeater);
    delete (memoryClient);
    delete (netfilterClient);
}

void EventManager::setEventConf(const QString &eventHead, const QString &eventTail) {
    this->eventHead = eventHead;
    this->eventTail = eventTail;
}

void EventManager::setNetfilterConf(const QString &vmIp, const QString &externalIp) {
    this->vmIp = vmIp;
    this->externalIp = externalIp;
}

void EventManager::setSerialPortConf(const QString &pseudoTerminal, const QString &serialPort) {
    this->pseudoTerminal = pseudoTerminal;
    this->serialPort = serialPort;
}

void EventManager::setStateMachineConf(const QString &stateMachineFilePath) {
    this->stateMachineFilePath = stateMachineFilePath;
}


void EventManager::stop() {
    threadStop = true;
}

void EventManager::run() {
    threadStop = false;

    // 先初始化模型
    Model *model = StateMachineParser::initFromFile(stateMachineFilePath.toLocal8Bit().data());
    if (model == nullptr) {
        logger->error("从文件\"%s\"初始化模型失败！", stateMachineFilePath.toLocal8Bit().data());
        emit showLogMessage(QString("从文件\"%1\"初始化模型失败！").arg(stateMachineFilePath));
        return;
    } else {
        logger->info("从状态机文件\"%s\"初始化模型完成！", stateMachineFilePath.toLocal8Bit().data());
        emit showLogMessage(QString("从状态机文件\"%1\"初始化模型完成！").arg(stateMachineFilePath));
    }

    bool hasNetfilterClient = true, hasSerialPortRepeater = true, hasMemoryClient = true;

    // 再启动netfilterClient
    netfilterClient->setEventMatchText(eventHead.toStdString(), eventTail.toStdString());
    netfilterClient->setEventMatchIp(vmIp.toStdString(), externalIp.toStdString());

    logger->info("初始化netfilter...");
    emit showLogMessage("初始化netfilter...");
    if (!netfilterClient->install()) {
        logger->error("安装netfilter失败!");
        emit showLogMessage("安装netfilter失败!");
        hasNetfilterClient = false;
    } else {
        logger->info("成功编译安装netfilter内核模块！");
        emit showLogMessage("成功编译安装netfilter内核模块！");
    }

    if (hasNetfilterClient) {
        logger->info("连接内核中的netfilter模块...");
        emit showLogMessage("连接内核中的netfilter模块...");
        if (!netfilterClient->start()) {
            logger->error("netfilter连接失败!");
            emit showLogMessage("netfilter连接失败!");
            netfilterClient->stop();
            netfilterClient->remove();
            hasNetfilterClient = false;
        } else {
            logger->info("成功连接到内核中的netfilter模块！");
            emit showLogMessage("成功连接到内核中的netfilter模块！");
            socketNetlink = netfilterClient->getFd();
        }
    }

    // 再启动serialPortClient
    serialPortRepeater->setEventMatchText(eventHead.toStdString(), eventTail.toStdString());
    serialPortRepeater->setPorts(pseudoTerminal.toStdString(), serialPort.toStdString());

    logger->info("初始化串口转发器...");
    emit showLogMessage("初始化串口转发器...");
    if (!serialPortRepeater->init()) {
        logger->error("串口转发器初始化失败！");
        emit showLogMessage("串口转发器初始化失败！");
        serialPortRepeater->closePorts();
        hasSerialPortRepeater = false;
    } else {
        logger->info("初始化串口转发器成功！");
        emit showLogMessage("初始化串口转发器成功！");
        fdPseudoTerminal = serialPortRepeater->getPseudoTerminalFd();
        fdSerialPort = serialPortRepeater->getSerialPortFd();
    }

    // 最后启动memoryClient
    logger->info("连接内存事件获取服务器...");
    emit showLogMessage("连接内存事件获取服务器...");
    if (!memoryClient->start()) {
        logger->error("内存事件获取服务器连接失败！");
        emit showLogMessage("内存事件获取服务器连接失败！");
        memoryClient->stop();
        hasMemoryClient = false;
    } else {
        logger->info("成功连接至内存事件获取服务器！");
        emit showLogMessage("成功连接至内存事件获取服务器！");
        socketMemoryClient = memoryClient->getFd();
    }

    int maxfd = 0;
    if (hasNetfilterClient) {
        maxfd = socketNetlink;
    }
    if (hasSerialPortRepeater) {
        maxfd = std::max(maxfd, std::max(fdPseudoTerminal, fdSerialPort));
    }
    if (hasMemoryClient) {
        maxfd = std::max(maxfd, socketMemoryClient);
    }
    if (maxfd == 0) {
        logger->error("一个能连接的服务器都没有!");
        emit showLogMessage("没有可用的服务器!");
    }

    time_t startWaitTime, now;
    const char *eventXML = nullptr;
    const Event *event;
    vector<const Event *> waitEventList;
    if (maxfd != 0) {
        while (!threadStop) {
            FD_ZERO(&fs_read);
            if (hasNetfilterClient) {
                FD_SET(socketNetlink, &fs_read);
            }
            if (hasSerialPortRepeater) {
                FD_SET(fdPseudoTerminal, &fs_read);
                FD_SET(fdSerialPort, &fs_read);
            }
            if (hasMemoryClient) {
                FD_SET(socketMemoryClient, &fs_read);
            }
            tv = defaultTv;

            if (select(maxfd + 1, &fs_read, nullptr, nullptr, &tv) > 0) {
                startSerial:
                if (FD_ISSET(fdPseudoTerminal, &fs_read) || FD_ISSET(fdSerialPort, &fs_read)) {
                    logger->debug("串口上有数据");
                    if (FD_ISSET(fdPseudoTerminal, &fs_read)) {
                        logger->debug("伪终端上有数据");
                        eventXML = serialPortRepeater->getEvent(fdPseudoTerminal);
                        if (eventXML == nullptr) {
                            logger->error("eventXML from pseudo is nullptr");
                            goto startMemory;
                        }
                        logger->info("采集到串口事件(虚拟机-->外部设备)：%s", eventXML);
                        emit showLogMessage(QString("采集到串口事件(虚拟机-->外部设备)：%1").arg(eventXML));
                    }
                    else if (FD_ISSET(fdSerialPort, &fs_read)) {
                        logger->debug("物理串口上有数据");
                        eventXML = serialPortRepeater->getEvent(fdSerialPort);
                        if (eventXML == nullptr) {
                            logger->error("eventXML from serial port is nullptr");
                            goto startMemory;
                        }
                        logger->info("集到串口事件(外部设备-->虚拟机)：%s", eventXML);
                        emit showLogMessage(QString("采集到串口事件(外部设备-->虚拟机)：%1").arg(eventXML));
                    }
                    event = Event::initFromXML(eventXML);
                    if (event == nullptr) {
                        logger->error("事件%s不符合规范", eventXML);
                        emit showLogMessage(QString("事件%1不符合规范").arg(eventXML));
                        goto startMemory;
                    }
                    auto result = model->verifyEvent(event).resultEnum;
                    if (event->isImportant()) {
                        logger->debug("该事件为关键事件");
                        emit showLogMessage("该事件为关键事件");
                        if (result == pass) {
                            logger->info("串口事件 \"%s\" 验证通过", eventXML);
                            emit showLogMessage("验证事件后通过此事件");
                            serialPortRepeater->passEvent();
                        }
                        else if (result == refuse) {
                            logger->info("串口事件 \"%s\"验证拦截", eventXML);
                            emit showLogMessage("验证事件后拦截此事件");
                            serialPortRepeater->interceptEvent();
                        }
                        else {
                            logger->info("串口事件 \"%s\" 暂时不能确定处理结果", eventXML);
                            emit showLogMessage("验证事件后暂时不能确定结果，等待后续事件一并验证");
                            if (waitEventList.empty()) {
                                startWaitTime = time(nullptr);
                            }
                            waitEventList.push_back(event);
                        }
                    }
                    else {
                        logger->debug("该事件为非关键事件");
                        emit showLogMessage("该事件为非关键事件");
                        if (result == pass || result == undetermined) {
                            logger->info("串口事件 \"%s\" 验证通过", eventXML);
                            emit showLogMessage("验证事件后通过该事件");
                            serialPortRepeater->passEvent();
                        }
                        else if (result == refuse) {
                            logger->info("串口事件 \"%s\"验证拦截", eventXML);
                            emit showLogMessage("验证事件后拦截该事件");
                            serialPortRepeater->interceptEvent();
                        }
                    }
                }

                startMemory:
                if (FD_ISSET(socketMemoryClient, &fs_read)) {
                    logger->debug("内存上有数据");
                    eventXML = memoryClient->getEvent();
                    if (eventXML == nullptr) {
                        logger->error("eventXML from memory is nullptr");
                        goto startNet;
                    }
                    logger->info("采集到内存事件：%s", eventXML);
                    emit showLogMessage(QString("采集到内存事件：%1").arg(eventXML));

                    event = Event::initFromXML(eventXML);
                    if (event == nullptr) {
                        logger->error("事件%s不符合规范", eventXML);
                        emit showLogMessage(QString("事件%1不符合规范").arg(eventXML));
                        goto startNet;
                    }
                    auto result = model->verifyEvent(event).resultEnum;
                    if (event->isImportant()) {
                        logger->debug("该事件为关键事件");
                        emit ("该事件为关键事件");
                        if (result == pass) {
                            logger->info("内存事件 \"%s\" 验证通过", eventXML);
                            emit showLogMessage("事件验证通过");
                        }
                        else if (result == refuse) {
                            logger->info("内存事件 \"%s\"验证拦截", eventXML);
                            emit showLogMessage("警告：事件验证未通过");
                        }
                        else {
                            logger->info("内存事件 \"%s\" 暂时不能确定处理结果", eventXML);
                            emit showLogMessage("验证事件后暂时不能确定结果，等待后续事件一并验证");
                            if (waitEventList.empty()) {
                                startWaitTime = time(nullptr);
                            }
                            waitEventList.push_back(event);
                        }
                    }
                    else {
                        logger->debug("该事件为非关键事件");
                        if (result == pass || result == undetermined) {
                            logger->info("内存事件 \"%s\" 验证通过", eventXML);
                        }
                        else if (result == refuse) {
                            logger->info("内存事件 \"%s\"验证拦截", eventXML);
                        }
                    }
                }

                startNet:
                if (FD_ISSET(socketNetlink, &fs_read)) {
                    logger->debug("网络上有数据");
                    eventXML = netfilterClient->getEvent();
                    if (eventXML == nullptr) {
                        logger->error("eventXML from net is nullptr");
                        goto endRound;
                    }
                    logger->info("采集到网络事件：%s", eventXML);
                    emit showLogMessage(QString("采集到网络事件：%1").arg(eventXML));

                    event = Event::initFromXML(eventXML);
                    if (event == nullptr) {
                        logger->error("事件%s不符合规范", eventXML);
                        emit showLogMessage(QString("事件%1不符合规范").arg(eventXML));
                        goto endRound;
                    }
                    auto result = model->verifyEvent(event).resultEnum;
                    if (event->isImportant()) {
                        logger->debug("该事件为关键事件");
                        emit ("该事件为关键事件");
                        if (result == pass) {
                            logger->info("网络事件 \"%s\" 验证通过", eventXML);
                            emit showLogMessage("验证事件后通过此事件");
                            netfilterClient->passEvent();
                        }
                        else if (result == refuse) {
                            logger->info("网络事件 \"%s\"验证拦截", eventXML);
                            emit showLogMessage("验证事件后拦截此事件");
                            netfilterClient->interceptEvent();
                        }
                        else {
                            logger->info("网络事件 \"%s\" 暂时不能确定处理结果", eventXML);
                            emit showLogMessage("验证事件后暂时不能确定结果，等待后续事件一并验证");
                            if (waitEventList.empty()) {
                                startWaitTime = time(nullptr);
                            }
                            waitEventList.push_back(event);
                        }
                    }
                    else {
                        logger->debug("该事件为非关键事件");
                        if (result == pass || result == undetermined) {
                            logger->info("网络事件 \"%s\" 验证通过", eventXML);
                            netfilterClient->passEvent();
                        }
                        else if (result == refuse) {
                            logger->info("网络事件 \"%s\"验证拦截", eventXML);
                            netfilterClient->interceptEvent();
                        }
                    }
                }

                endRound:;
            }
        }
    }

    // 先关闭netfilterClient
    if (hasNetfilterClient) {
        netfilterClient->stop();
        netfilterClient->remove();
    }

    // 再关闭serialPortClient
    if (hasSerialPortRepeater) {
        serialPortRepeater->closePorts();
    }

    // 最后关闭memoryClient
    if (hasMemoryClient) {
        memoryClient->stop();
    }
}
