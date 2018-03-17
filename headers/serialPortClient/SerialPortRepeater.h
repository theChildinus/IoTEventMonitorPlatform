//
// Created by chenkuan on 17-11-21.
//

#ifndef IOTEVENTMONITORPLATFORM_SERIALPORTREPEATER_H
#define IOTEVENTMONITORPLATFORM_SERIALPORTREPEATER_H

#include <SerialPortClient.h>

/**
 * 串口中继器，管理两个串口设备，进行事件采集与转发
 */
class SerialPortRepeater {
public:
    SerialPortRepeater() = default;
    ~SerialPortRepeater();

    /**
     * 设置事件匹配头尾
     * @param eventHead
     * @param eventTail
     */
    void setEventMatchText(const string &eventHead, const string &eventTail);
    /**
     * 设置两个串口名称，进行初始化
     * @param pseudoTerminal
     * @param serialPort
     */
    void setPorts(const string &pseudoTerminalName, const string &serialPortName);
    /**
     * 初始化两个串口
     * @return
     */
    bool init();
    /**
     * 关闭两个串口
     */
    void closePorts();
    /**
     * 串口上有没有事件
     * @return
     */
    bool hasEvent();
    /**
     * 从串口上获取事件
     * @return
     */
    const char *getEvent(int fd);
    /**
     * 通过上一个获取的事件
     */
    void passEvent();
    /**
     * 拦截上一个获取的事件
     */
    void interceptEvent();

    int getPseudoTerminalFd();

    int getSerialPortFd();

  private:
    string pseudoTerminalName;
    string serialPortName;
    SerialPortClient *pseudoTerminal = nullptr;
    SerialPortClient *serialPort = nullptr;
    SerialPortClient *lastDevice = nullptr;

    string eventHead;
    string eventTail;

    string lastEvent;
    /**
     * 允许的最长字符消息缓冲
     */
    const int MAX_MESSAGE_SIZE = 1024;

    /**
     * 在字符串中搜寻事件
     * 如果查找到了事件，则将事件前面字节转发，事件暂存，更新messageQueue
     * @param messageQueue
     * @param otherPort 对端串口
     * @return
     */
    bool searchEvent(string &messageQueue, SerialPortClient *otherPort);
};


#endif //IOTEVENTMONITORPLATFORM_SERIALPORTREPEATER_H
