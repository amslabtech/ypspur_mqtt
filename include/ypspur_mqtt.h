#ifndef __YPSPUR_MQTT_H
#define __YPSPUR_MQTT_H

#include <iostream>
#include <vector>
#include <cstring>
#include <string>
#include <cstdlib>
#include <unistd.h>
#include <sys/wait.h>

#include "ypspur.h"

class YPSpurMQTT
{
public:
    YPSpurMQTT(void);

    void spin(void);
    void set_port(std::string);

private:
    std::string PORT;
    int IPC_KEY;
    std::string YPSPUR_COORDINATOR;
    std::string PARAM_FILE;
    double HZ;
};

#endif// __YPSPUR_MQTT_H
