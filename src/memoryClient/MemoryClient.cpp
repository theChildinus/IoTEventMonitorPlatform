//
// Created by chenkuan on 2018/3/4.
//

#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <memory.h>
#include <MemoryClient.h>

MemoryClient::~MemoryClient() {
    stop();
}

const char *MemoryClient::getEvent() {
    ssize_t recvNum = recv(clientSocket, buffer, MAX_BUFFER_SIZE, MSG_DONTWAIT);
    if (recvNum < 0) {
        logger->error("接收事件失败！%s", strerror(errno));
        return nullptr;
    } else if (recvNum == 0) {
//        logger->error("内存事件服务器已经关闭!%s", strerror(errno));
        return nullptr;
    }
    return buffer;
}

bool MemoryClient::start() {
    logger->debug("启动内存事件获取客户端");
    clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket < 0) {
        logger->error("创建套接字失败！");
        return false;
    }

    // 配置服务端套接字
    bzero(&serverAddr, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(MEMORY_SERVER_PORT);
    serverAddr.sin_addr.s_addr = inet_addr(MEMORY_SERVER_IP);

//    // 设置套接字为非阻塞
//    int flags = fcntl(clientSocket, F_GETFL, 0);
//    fcntl(clientSocket, F_SETFL, flags | O_NONBLOCK);

    // 连接服务端
    if (connect(clientSocket, (struct sockaddr *) &serverAddr, sizeof(serverAddr)) < 0) {
        logger->error("连接服务端失败！%s", strerror(errno));
        return false;
    }

    return true;
}

bool MemoryClient::stop() {
    logger->debug("停止内存事件获取客户端");
    if (clientSocket < 0) return true;
    close(clientSocket);
    return true;
}

int MemoryClient::getFd() {
    return clientSocket;
}
