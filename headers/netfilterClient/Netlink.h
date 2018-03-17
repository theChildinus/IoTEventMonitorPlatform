//
// Created by chenkuan on 17-11-13.
//

#ifndef IOTEVENTMONITORPLATFORM_NETLINK_H
#define IOTEVENTMONITORPLATFORM_NETLINK_H

#include <linux/netlink.h>
#include <string>
#include <Logger.h>
using std::string;

class Netlink {
  public:
    Netlink() = default;
    ~Netlink();
    bool init();
    void closeConnection();
    bool hasMessage();

    const char *getMessage();
    bool sendAcceptMessage();
    bool sendDropMessage();

    int getFd();

  private:
    /**
     * 自定义的netlink协议号
     */
    static const int NETLINK_TEST = 20;
    /**
     * 自定义的netlink客户端发送连接请求时type
     */
    static const int NETLINK_TEST_CONNECT = 0x10;
    /**
     * 自定义的netlink客户端发送断开连接请求时type
     */
    static const int NETLINK_TEST_DISCONNECT = 0x11;
    /**
     * 自定义的netlink内核回复的消息类型
     */
    static const int NETLINK_TEST_REPLY = 0x12;
    /**
     * 自定义的netlink发送的通过指令
     */
    static const int NETLINK_TEST_ACCEPT = 0x13;
    /**
     * 自定义的netlink发送的丢弃指令
     */
    static const int NETLINK_TEST_DISCARD = 0x14;
    /**
     * 消息最大长度
     */
    static const int MAX_MSG = 100000;
    /**
     * 通信消息结构
     */
    struct NetlinkMessage {
        struct nlmsghdr hdr;
        char data[MAX_MSG];
    };
    /**
     * 消息缓冲区
     */
    NetlinkMessage recvMessage;
    /**
     * 客户端套接字
     */
    int socketClient = -1;
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
     * 内核目标地址数据结构
     */
    struct sockaddr_nl destAddr;

    Logger *logger = Logger::getLogger();
};


#endif //IOTEVENTMONITORPLATFORM_NETLINK_H
