#ifndef __YPSPUR_WRAPPER_H
#define __YPSPUR_WRAPPER_H

#include <iostream>
#include <vector>
#include <cstring>
#include <string>
#include <cstdlib>
#include <unistd.h>
#include <ctime>

#include <sys/wait.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/time.h>

#include <boost/thread.hpp>
#include <boost/atomic.hpp>

#include "ypspur_mqtt/velocity_data.h"
#include "ypspur_mqtt/odometry_data.h"

namespace YP
{
    #include "ypspur.h"
}

namespace YPSpurWrapper
{

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

class YPSpurWrapper
{
public:
    YPSpurWrapper(void);
    ~YPSpurWrapper(void);

    void initialize(void);
    bool spin_once(void);
    void spin(void);
    bool is_shutdown_requested(void);
    void set_port(const std::string&);
    void set_simulation_mode(void);
    void set_param_file(const std::string&);
    void set_control_mode(int);
    void set_velocity(const VelocityData&);
    OdometryData get_odometry(void);
    static void sigint_handler(int);

private:
    void send_velocity(const VelocityData&);

    std::string PORT;
    int IPC_KEY;
    std::string YPSPUR_COORDINATOR;
    std::string PARAM_FILE;
    double HZ;
    double TIME_LIMIT;

    static bool shutdown_flag;

    OdometryData odom;
    VelocityData vel;
    ControlMode control_mode;
    std::map<std::string, double> params_;
    pid_t pid;
    bool simulation_flag;
};

}// namespace YPSpurWrapper

#endif// __YPSPUR_WRAPPER_H
