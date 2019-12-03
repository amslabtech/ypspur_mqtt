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

namespace YPSpurMQTT
{

class Velocity
{
public:
    Velocity(void);
    Velocity(double, double);

    double v;
    double w;
};

class Odometry
{
public:
    Odometry(void);

    double t;
    double x;
    double y;
    double yaw;
    Velocity vel;
};

class ControlMode
{
public:
    ControlMode(void);
    ControlMode(int);

    void set_mode(int);

    enum MODE{
        OPEN,
        TORQUE,
        VELOCITY
    };

    int mode;
};

std::ostream &operator<<(std::ostream &out, const Odometry &o);

class YPSpurMQTT
{
public:
    YPSpurMQTT(void);
    ~YPSpurMQTT(void);

    void initialize(void);
    void spin(void);
    void set_port(std::string);
    void set_simulation_mode(void);
    void set_param_file(const std::string&);
    void set_velocity(const Velocity&);
    void set_control_mode(int);
    static void sigint_handler(int);

private:
    std::string PORT;
    int IPC_KEY;
    std::string YPSPUR_COORDINATOR;
    std::string PARAM_FILE;
    double HZ;

    static bool shutdown_flag;

    Odometry odom;
    ControlMode control_mode;
    std::map<std::string, double> params_;
    pid_t pid;
    bool simulation_flag;
};

}// namespace YPSpurMQTT

#endif// __YPSPUR_MQTT_H
