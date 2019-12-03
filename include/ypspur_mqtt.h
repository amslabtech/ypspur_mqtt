#ifndef __YPSPUR_MQTT_H
#define __YPSPUR_MQTT_H

#include <iostream>
#include <vector>
#include <cstring>
#include <string>
#include <cstdlib>
#include <unistd.h>

#include <sys/wait.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>

#include <boost/thread.hpp>
#include <boost/atomic.hpp>

namespace YP
{
    #include "ypspur.h"
}

class YPSpurMQTT
{
public:
    YPSpurMQTT(void);
    ~YPSpurMQTT(void);

    void initialize(void);
    void set_port(std::string);
    void set_simulation_mode(void);
    void set_param_file(const std::string&);
    static void sigint_handler(int);

private:
    std::string PORT;
    int IPC_KEY;
    std::string YPSPUR_COORDINATOR;
    std::string PARAM_FILE;
    double HZ;

    static bool shutdown_flag;
    std::map<std::string, double> params_;
    pid_t pid;
    bool simulation_flag;
};

#endif// __YPSPUR_MQTT_H
