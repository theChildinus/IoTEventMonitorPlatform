//
// Created by yingzi on 2017/6/26.
//

#include <SerialPortClient.h>
#include <string>
#include <vector>
#include <iterator>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <sys/select.h>

using std::string;
using std::vector;

SerialPortClient::~SerialPortClient() {
    closePort();
}

void SerialPortClient::setPort(const string &port) {
    this->port = port;
}

bool SerialPortClient::init() {
    if (!openPort()) {
        logger->error("打开端口 %s 失败！", port.c_str());
        return false;
    }
    if (!setPort()) {
        logger->error("设置端口 %s 失败！", port.c_str());
        return false;
    }
    return true;
}

void SerialPortClient::closePort() {
    if (fd != -1) {
        close(fd);
        fd = -1;
    }
}

bool SerialPortClient::hasMessage() {
    FD_ZERO(&fs_read);
    FD_SET(fd, &fs_read);
    tv = defaultTv;
    fs_sel = select(fd + 1, &fs_read, NULL, NULL, &tv);
    return fs_sel > 0;
}

bool SerialPortClient::getMessage() {
    int len = 0, recvNum = 0;
    while (true) {
        recvNum = read(fd, buffer + len, MAX_MSG - len);
        if (recvNum < 0) {
            // 读不到数据了
            break;
        }
        len += recvNum;
        if (len > MAX_LINE_SIZE) {
            break;
        }
    }

    if (len > 0) {
        messageQueue.append(string(std::begin(buffer), std::begin(buffer) + len));
        return true;
    } else return false;
}

bool SerialPortClient::openPort() {
    fd = open(port.c_str(), O_RDWR|O_NOCTTY|O_NDELAY);
    if (fd < 0){
        logger->error("无法打开端口 %s！", port.c_str());
        return false;
    }
    if(fcntl(fd, F_SETFL, 0) < 0){
        logger->error("端口 %s fcntl 设置失败！", port.c_str());
        return false;
    }
/*
    if(0 == isatty(STDIN_FILENO)){
        cerr << port << " is not a terminal device" << endl;
        return false;
    }
*/
    return true;
}

bool SerialPortClient::setPort() {
    struct termios options;
    if (tcgetattr(fd, &options) != 0) {
        logger->error("获取端口 %s 终端参数失败！", port.c_str());
        return false;
    }

    vector<int> baudRates{38400, 19200, 9600, 4800, 2400, 1200, 300, 38400, 19200, 9600, 4800, 2400, 1200, 300};
    vector<int> baudRatesBin{B38400, B19200, B9600, B4800, B2400, B1200, B300, B38400, B19200, B9600, B4800, B2400, B1200, B300};

    for (auto it = baudRates.cbegin(), itb = baudRatesBin.cbegin(); it != baudRates.cend(); ++it, ++itb) {
        if (*it == baudRate) {
            cfsetispeed(&options, *itb);
            cfsetospeed(&options, *itb);
        }
    }

    options.c_cflag |= CLOCAL;
    options.c_cflag |= CREAD;
    switch(parityBit){
        case 0 :
            options.c_cflag &= ~CRTSCTS;
            break;
        case 1 :
            options.c_cflag |= CRTSCTS;
            break;
        case 2 :
            options.c_cflag |= IXON | IXOFF | IXANY;
            break;
    }

    options.c_cflag &= ~CSIZE;
    switch (dataBit){
        case 5 :
            options.c_cflag |= CS5;
            break;
        case 6	:
            options.c_cflag |= CS6;
            break;
        case 7	:
            options.c_cflag |= CS7;
            break;
        case 8:
            options.c_cflag |= CS8;
            break;
        default:
            logger->error("不支持数据大小");
            return false;
    }

    switch (parity) {
        case 'n':
        case 'N':
            options.c_cflag &= ~PARENB;
            options.c_iflag &= ~INPCK;
            break;
        case 'o':
        case 'O':
            options.c_cflag |= (PARODD | PARENB);
            options.c_iflag |= INPCK;
            break;
        case 'e':
        case 'E':
            options.c_cflag |= PARENB;
            options.c_cflag &= ~PARODD;
            options.c_iflag |= INPCK;
            break;
        case 's':
        case 'S':
            options.c_cflag &= ~PARENB;
            options.c_cflag &= ~CSTOPB;
            break;
        default:
            logger->error("不支持的校验位");
            return false;
    }

    switch (stopBit){
        case 1:
            options.c_cflag &= ~CSTOPB;
            break;
        case 2:
            options.c_cflag |= CSTOPB;
            break;
        default:
            logger->error("不支持的停止位");
            return false;
    }
    options.c_oflag &= ~OPOST;

    options.c_cc[VTIME] = 1;
    options.c_cc[VMIN] = 1;

    tcflush(fd,TCIFLUSH);

    if (tcsetattr(fd, TCSANOW, &options) != 0) {
        logger->error("端口 %s 终端参数设置失败!", port.c_str());
        return false;
    }
    return true;
}

bool SerialPortClient::sendMessage(const string &message) {
    if (message.empty()) return true;
    auto len = write(fd, message.c_str(), message.size());
    if (len == message.size()) {
        logger->debug("成功发送%d字节到端口%s，内容为：%s", len, port.c_str(), message.c_str());
        return true;
    }
    else {
        logger->error("端口%s发送数据不完整，仅发送%d/%d字节", port.c_str(), len, message.size());
        tcflush(fd, TCOFLUSH);
        return false;
    }

}

string &SerialPortClient::getMessageQueue() {
    return messageQueue;
}

int SerialPortClient::getFd() {
    return fd;
}
