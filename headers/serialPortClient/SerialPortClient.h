//
// Created by yingzi on 2017/6/26.
//

#ifndef RUNTIME_VERIFICATION_SYSTEM_SERIALFORWARD_H
#define RUNTIME_VERIFICATION_SYSTEM_SERIALFORWARD_H

#include <string>
#include <Logger.h>

using std::string;

class SerialPortClient {
  public:
    SerialPortClient() = default;
    ~SerialPortClient();
    /**
     * 设置事件匹配头尾
     * @param eventHeadText
     * @param eventTailText
     */
    void setPort(const string &port);
    bool init();  // 设置串口、打开串口
    void closePort();

    /**
     * 查看串口上是否收到字节流
     * @return
     */
    bool hasMessage();

    /**
     * 从串口上获取字节流追加到缓存的字符队列后面
     * @return 是否获取到新的消息
     */
    bool getMessage();
    /**
     * 获取字符流队列的引用
     * @return
     */
    string &getMessageQueue();
    /**
     * 往串口上发字节流消息
     * @param message
     * @return
     */
    bool sendMessage(const string& message);

    int getFd();

  private:
    /**
     * 波特率
     */
    static const int baudRate = 9600;
    /**
     * 校验位
     */
    static const int parityBit = 0;
    /**
     * 数据位
     */
    static const int dataBit = 8;
    /**
     * 停止位
     */
    static const int stopBit = 1;
    /**
     * 奇偶校验位
     */
    static const char parity = 'N';
    /**
     * 单行的最大字符数
     */
    static const int MAX_LINE_SIZE = 40;
    /**
    * 消息最大长度
    */
    static const int MAX_MSG = 1024;

    /**
     * 串口名称
     */
    string port;
    /**
     * 串口的文件描述符
     */
    int fd;
    /**
    * 消息缓冲区
    */
    char buffer[MAX_MSG];
    /**
    * 用于select的客户端套接字集合
    */
    fd_set fs_read;
    /**
     * 判断select集合是否有可读的
     */
    int fs_sel;
    /**
     * 等待超时时间
     */
    static constexpr timeval defaultTv = {0, 1000};
    timeval tv = defaultTv;

    /**
     * 缓存的字符队列
     */
    string messageQueue;

    /**
     * 打开串口
     * @return
     */
    bool openPort();
    /**
     * 设置串口，设置波特率、校验位等
     * @return
     */
    bool setPort();

    Logger *logger = Logger::getLogger();
};


#endif //RUNTIME_VERIFICATION_SYSTEM_SERIALFORWARD_H
