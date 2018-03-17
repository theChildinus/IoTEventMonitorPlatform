//
// Created by chenkuan on 17-11-21.
//

#include "SerialPortRepeater.h"

SerialPortRepeater::~SerialPortRepeater() {
    if (pseudoTerminal != nullptr) {
        pseudoTerminal->closePort();
    }
    if (serialPort != nullptr) {
        serialPort->closePort();
    }
}

void SerialPortRepeater::setPorts(const string &pseudoTerminalName, const string &serialPortName) {
    this->pseudoTerminalName = pseudoTerminalName;
    this->serialPortName = serialPortName;
}

bool SerialPortRepeater::init() {
    delete(pseudoTerminal);
    delete(serialPort);
    pseudoTerminal = new SerialPortClient();
    serialPort = new SerialPortClient();

    pseudoTerminal->setPort(pseudoTerminalName);
    serialPort->setPort(serialPortName);
    if (!pseudoTerminal->init() || !serialPort->init()) {
        return false;
    }
    return true;
}

void SerialPortRepeater::closePorts() {
    pseudoTerminal->closePort();
    serialPort->closePort();
}

bool SerialPortRepeater::hasEvent() {
    lastEvent = "";
    if (pseudoTerminal->hasMessage()) {
        pseudoTerminal->getMessage();
        if (searchEvent(pseudoTerminal->getMessageQueue(), serialPort)){
            lastDevice = pseudoTerminal;
            return true;
        }
    }
    if (serialPort->hasMessage()) {
        serialPort->getMessage();
        if (searchEvent(serialPort->getMessageQueue(), pseudoTerminal)) {
            lastDevice = serialPort;
            return true;
        }
    }
    return false;
}

const char *SerialPortRepeater::getEvent(int fd) {
    if (pseudoTerminal->getFd() == fd) {
        if (pseudoTerminal->getMessage() && searchEvent(pseudoTerminal->getMessageQueue(), serialPort)) {
            lastDevice = pseudoTerminal;
        }
    } else if (serialPort->getFd() == fd) {
        if (serialPort->getMessage() && searchEvent(serialPort->getMessageQueue(), pseudoTerminal)) {
            lastDevice = serialPort;
        }
    }
    if (lastEvent.empty()) return nullptr;
    else return lastEvent.c_str();
}

void SerialPortRepeater::setEventMatchText(const string &eventHead, const string &eventTail) {
    this->eventHead = eventHead;
    this->eventTail = eventTail;
}

bool SerialPortRepeater::searchEvent(string &messageQueue, SerialPortClient *otherPort) {
    auto headIndex = messageQueue.find(eventHead);
    if (headIndex != string::npos) {
        auto tailIndex = messageQueue.find(eventTail);
        if (tailIndex != string::npos) {
            otherPort->sendMessage(messageQueue.substr(0, headIndex));
            lastEvent = messageQueue.substr(headIndex, tailIndex - headIndex + eventTail.size());
            messageQueue = messageQueue.substr(tailIndex + eventTail.size());
            return true;
        }
        else if (messageQueue.size() >= MAX_MESSAGE_SIZE){
            // 如果尾巴没找到但超长了，视为非事件转发
            otherPort->sendMessage(messageQueue);
            messageQueue.clear();
        }
    }
    else {
        // 如果头部没找到，视为非事件转发，注意保留队列头部长度 - 1个字节
        if (messageQueue.size() >= eventHead.size()) {
            otherPort->sendMessage(messageQueue.substr(0, messageQueue.size() - eventHead.size() + 1));
            messageQueue = messageQueue.substr(messageQueue.size() - eventHead.size() + 1);
        }
        // 队列太短时给予全部保留
//        else {
//            otherPort->sendMessage(messageQueue.substr(0, 1));
//            messageQueue = messageQueue.substr(1);
//        }
    }
    return false;
}

void SerialPortRepeater::passEvent() {
    if (lastEvent.empty()) return;

    if (lastDevice == pseudoTerminal) {
        serialPort->sendMessage(lastEvent);
    } else if (lastDevice == serialPort) {
        pseudoTerminal->sendMessage(lastEvent);
    }
    lastEvent.clear();
}

void SerialPortRepeater::interceptEvent() {
    // pass
}

int SerialPortRepeater::getPseudoTerminalFd() {
    return pseudoTerminal->getFd();
}

int SerialPortRepeater::getSerialPortFd() {
    return serialPort->getFd();
}
