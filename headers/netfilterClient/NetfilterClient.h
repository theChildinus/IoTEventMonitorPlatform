//
// Created by yingzi on 2017/11/7.
//

#ifndef IOTEVENTMONITORPLATFORM_NETFILTERCLIENT_H
#define IOTEVENTMONITORPLATFORM_NETFILTERCLIENT_H

#include <QtCore/QThread>
#include <string>
#include <Netlink.h>

using std::string;

/**
 * 提供对netfilter的配置安装与卸载
 * 从netfilter获取事件
 */
class NetfilterClient {
  public:
    NetfilterClient();
    ~NetfilterClient() ;

    void setEventMatchText(const string &eventHead, const string &eventTail);
    void setEventMatchIp(const string &vmIp, const string &externalIp);
    bool hasEvent();

    const char *getEvent();
    bool passEvent();
    bool interceptEvent();
    bool install();
    void remove();
    bool start();
    void stop();

    int getFd();

  private:
    bool threadStop = false;

    string eventHead;
    string eventTail;
    string vmIp;
    string externalIp;

    Netlink *netlink;
    string event;

    Logger *logger = Logger::getLogger();

    /**
    * 转义所有配置字符串
    */
    void escapeAllConfStr();

    /**
     * 转义指定的配置字符串
     * @param str
     */
    void escapeStr(string &str);
};


#endif //IOTEVENTMONITORPLATFORM_NETFILTERCLIENT_H
