//
// Created by chenkuan on 2018/3/4.
//

#ifndef IOTEVENTMONITORPLATFORM_MEMORYCLIENT_H
#define IOTEVENTMONITORPLATFORM_MEMORYCLIENT_H

#include <netinet/in.h>
#include <string>
#include <Logger.h>
#include <MemoryClientConf.h>

using std::string;

class MemoryClient {
  public:
    MemoryClient() = default;

    ~MemoryClient();

    const char *getEvent();

    bool start();

    bool stop();

    int getFd();

  private:
    int clientSocket = -1;
    struct sockaddr_in serverAddr;

    char buffer[MAX_BUFFER_SIZE];
    unsigned bufferOffset = 0;

    Logger *logger = Logger::getLogger();
};

#endif //IOTEVENTMONITORPLATFORM_MEMORYCLIENT_H
