//
// Created by yingzi on 2017/11/7.
//

#include <NetfilterClient.h>
#include <NetfilterConf.h>

NetfilterClient::NetfilterClient() {
    netlink = new Netlink();
}

NetfilterClient::~NetfilterClient() {
    stop();
    remove();
    delete(netlink);
}

void NetfilterClient::setEventMatchText(const string &eventHead, const string &eventTail) {
    this->eventHead = eventHead;
    this->eventTail = eventTail;
}


bool NetfilterClient::install() {
    logger->debug("安装netfilter内核模块");
    FILE *fp = nullptr;

    // 先移除可能未移除的netfilter模块
    remove();

    // 将配置字符串根据需要进行转义
    escapeAllConfStr();

    // 再修改netfilter的配置文件并make
    static char cmd[1024];
    sprintf(cmd,
            R"delimiter(
        cd %s;
        awk '{
            if($2=="SOURCE_IP"){ print $1,$2,"\"%s\"" > "%s" }
            else if($2=="TARGET_IP"){ print $1,$2,"\"%s\"" > "%s" }
            else if($2=="TAG_HEAD"){ print $1,$2,"\"%s\"" > "%s"}
            else if($2=="TAG_TAIL"){ print $1,$2,"\"%s\"" > "%s"}
            else { print $0 > "%s"}
        }' %s;
        make;
    )delimiter",
            NETFILTER_SRC_PATH,
            vmIp.c_str(),
            NETFILTER_CONF_FILENAME,
            externalIp.c_str(),
            NETFILTER_CONF_FILENAME,
            eventHead.c_str(),
            NETFILTER_CONF_FILENAME,
            eventTail.c_str(),
            NETFILTER_CONF_FILENAME,
            NETFILTER_CONF_FILENAME,
            NETFILTER_CONF_FILENAME);

    logger->debug("尝试对Netfilter配置文件进行更改并编译Netfilter内核模块");
    fp = popen(cmd, "r");
    if (fp == nullptr) {
        logger->error("无法使用popen执行shell指令来修改和编译netfilter代码");
        return false;
    }

    static char buffer[1024];
    string result;
    while (!feof(fp)) {
        if (fgets(buffer, sizeof(buffer), fp) != nullptr) {
            result += buffer;
        }
    }
    logger->debug("after modify and make\n:%s", result.c_str());
    pclose(fp);
    fp = nullptr;

    // 最后安装模块
    sprintf(cmd, "cd %s;sudo insmod netfilter.ko", NETFILTER_SRC_PATH);
    fp = popen(cmd, "r");
    if (fp == nullptr) {
        logger->error("无法使用popen执行shell指令来安装netfilter模块");
        return false;
    }

    result.clear();
    while (!feof(fp)) {
        if (fgets(buffer, sizeof(buffer), fp) != nullptr) {
            result += buffer;
        }
    }
    logger->debug("after insmod:%s", result.c_str());

    pclose(fp);
    fp = nullptr;

    return true;
}

void NetfilterClient::remove() {
    FILE *fp = nullptr;
    logger->debug("尝试移除已有的Netfilter内核模块");
    fp = popen("sudo rmmod netfilter", "r");
    if (fp == nullptr) {
        logger->error("无法使用popen执行shell指令来移除已有的netfilter模块");
        return;
    }
    pclose(fp);
    fp = nullptr;
}

bool NetfilterClient::start() {
    return netlink->init();
}

void NetfilterClient::stop() {
    netlink->closeConnection();
}

void NetfilterClient::setEventMatchIp(const string &vmIp, const string &externalIp) {
    this->vmIp = vmIp;
    this->externalIp = externalIp;
}

bool NetfilterClient::hasEvent() {
    return netlink->hasMessage();
}

const char *NetfilterClient::getEvent() {
    return netlink->getMessage();
}

bool NetfilterClient::passEvent() {
    return netlink->sendAcceptMessage();
}

bool NetfilterClient::interceptEvent() {
    return netlink->sendDropMessage();
}

int NetfilterClient::getFd() {
    return netlink->getFd();
}

void NetfilterClient::escapeAllConfStr() {
    logger->debug("将Netfilter配置字符串进行转义");
    escapeStr(vmIp);
    escapeStr(externalIp);
    escapeStr(eventHead);
    escapeStr(eventTail);
}

void NetfilterClient::escapeStr(string &str) {
    logger->debug("开始转义字符串 %s", str.c_str());
    string strBackup;
    for (const auto &c : str) {
        if (c == '"') {
            strBackup.append(R"(\\\")");
        } else {
            strBackup.push_back(c);
        }
    }
    logger->debug("字符串 %s 转义成了 %s", str.c_str(), strBackup.c_str());
    str = strBackup;
}
