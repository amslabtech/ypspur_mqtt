#include "ypspur_mqtt.h"

namespace YPSpurMQTT
{

Velocity::Velocity(void)
:v(0), w(0)
{

}

Velocity::Velocity(double v_, double w_)
:v(v_), w(w_)
{

}

Odometry::Odometry(void)
:t(0), x(0), y(0)
{

}

std::ostream &operator<<(std::ostream &out, const Odometry &o)
{
    out << "(t: " << o.t << ", x: " << o.x << ", y: " << o.y << ", yaw: " << o.yaw << ", v: " << o.vel.v << ", w: " << o.vel.w << ")";
    return out;
}

ControlMode::ControlMode(void)
:mode(MODE::OPEN)
{

}

ControlMode::ControlMode(int mode_)
{
    set_mode(mode_);
}

void ControlMode::set_mode(int mode_)
{
    if(mode_ == MODE::OPEN || mode_ == MODE::TORQUE || mode_ == MODE::VELOCITY){
        mode = mode_;
    }else{
        std::cout << "\033[31mmode " << mode_ << " is invalid !!!\033[0m" << std::endl;
        std::cout << "\033[31set to OPEN mode\033[0m" << std::endl;
        mode = MODE::OPEN;
    }
}

// static member variable
bool YPSpurMQTT::shutdown_flag;

YPSpurMQTT::YPSpurMQTT(void)
{
    std::cout << "\033[032m--- CONSTRUCT ---\033[0m" << std::endl;
    PORT = "/dev/ttyACM0";
    IPC_KEY = 28741;
    YPSPUR_COORDINATOR = "ypspur-coordinator";
    PARAM_FILE = "";
    HZ = 1;
    shutdown_flag = false;
    pid = -1;
    simulation_flag = false;
}

YPSpurMQTT::~YPSpurMQTT(void)
{
    std::cout << "\033[033m--- DESTRUCT ---\033[0m" << std::endl;
    if(pid > 0){
        kill(pid, SIGINT);
        int status;
        waitpid(pid, &status, 0);
        std::cout << "ypspur-coordinator is killed (status: " << status << ")" << std::endl;
    }
}

void YPSpurMQTT::initialize(void)
{
    pid = 0;
    do{
        std::cout << "trying to start ypspur-coordinator..." << std::endl;
        // for communication with ypspur-coordinator
        int msq = msgget(IPC_KEY, 0666 | IPC_CREAT);
        msgctl(msq, IPC_RMID, nullptr);

        std::vector<std::string> args = {
            YPSPUR_COORDINATOR,
            "-d", PORT,
            "--msq-key", std::to_string(IPC_KEY),
        };
        if(PARAM_FILE != ""){
            args.push_back("-p");
            args.push_back(PARAM_FILE);
        }
        if(simulation_flag){
            std::cout << "\033[31mwithout device mode\033[0m" << std::endl;
            args.push_back("--without-device");
        }
        char** argv = new char*[args.size() + 1];
        for(unsigned int i=0;i<args.size();i++){
            argv[i] = new char[args[i].size() + 1];
            memcpy(argv[i], args[i].c_str(), args[i].size());
            argv[i][args[i].size()] = 0;
            std::cout << argv[i] << std::endl;
        }
        argv[args.size()] = nullptr;

        pid = fork();
        int status = 0;
        std::cout << pid << std::endl;
        if(pid == 0){
            std::cout << pid << ": succeeded to fork process" << std::endl;
            std::cout << pid << ": start ypspur-coordinator" << std::endl;
            execvp(YPSPUR_COORDINATOR.c_str(), argv);
            throw(std::runtime_error("failed to start ypspur-coordinator"));
        }else if(pid == -1){
            throw(std::runtime_error("failed to fork process"));
        }else{
            std::cout << pid << ": parent process" << std::endl;
            sleep(1);
        }
        for(unsigned int i=0;i<args.size()+1;i++){
            delete argv[i];
        }
        delete argv;

        if(waitpid(pid, &status, WNOHANG) == pid){
            throw(std::runtime_error("ypspur-coordinator dead immediately"));
            std::cout << pid << ": ypspur-coordinator dead immediately" << std::endl;
        }
    }while(YP::YPSpur_initex(IPC_KEY) < 0);

    double ret;
    boost::atomic<bool> done(false);
    auto get_vel_thread = [&ret, &done]
    {
        double v, w;
        ret = YP::YPSpur_get_vel(&v, &w);
        done = true;
    };
    std::cout << pid << ": start get_vel_thread" << std::endl;
    boost::thread spur_thread = boost::thread(get_vel_thread);
    sleep(1);
    std::cout << pid << ": done: " << done << std::endl;
    if(!done){
        std::cerr << pid << ": ypspur-coordinator seems to be down" << std::endl;
        spur_thread.detach();
        throw(std::runtime_error("ypspur-coordinator seems to be down"));
    }
    std::cout << pid << ": join" << std::endl;
    spur_thread.join();
    std::cout << pid << ": ret: " << ret << std::endl;
    if(ret < 0){
        std::cerr << pid << ": ypspur-coordinator returns error" << std::endl;
        throw(std::runtime_error("ypspur-coordinator returns error"));
    }
    std::cout << pid << ": ypspur-coordinator launched" << std::endl;

    signal(SIGINT, YPSpurMQTT::sigint_handler);

    // parameter setting
    YP::YP_get_parameter(YP::YP_PARAM_MAX_VEL, &params_["vel"]);
    YP::YP_get_parameter(YP::YP_PARAM_MAX_ACC_V, &params_["acc"]);
    YP::YP_get_parameter(YP::YP_PARAM_MAX_W, &params_["angvel"]);
    YP::YP_get_parameter(YP::YP_PARAM_MAX_ACC_W, &params_["angacc"]);

    YP::YPSpur_set_vel(params_["vel"]);
    YP::YPSpur_set_accel(params_["acc"]);
    YP::YPSpur_set_angvel(params_["angvel"]);
    YP::YPSpur_set_angaccel(params_["angacc"]);

    std::cout << "\033[32mypspur_mqtt successfully initialized\033[0m" << std::endl;
}

void YPSpurMQTT::spin(void)
{
    std::cout << "\033[32mypspur_mqtt main loop started\033[0m" << std::endl;
    while(!shutdown_flag){
        std::cout << "--- ypspur_mqtt ---" << std::endl;

        double x(0), y(0), yaw(0), v(0), w(0);
        double t = YP::YPSpur_get_pos(YP::CS_BS, &x, &y, &yaw);
        double t_ = YP::YPSpur_get_vel(&v, &w);
        std::cout << "t: " << t << std::endl;
        std::cout << "t_: " << t_ << std::endl;
        if(t < 0 || t_ < 0){
            std::cerr << "\033[31minvalid time: " << t << "\033[0m" << std::endl;
            break;
        }
        odom.t = t;
        odom.x = x;
        odom.y = y;
        odom.yaw = yaw;
        odom.vel.v = v;
        odom.vel.w = w;
        std::cout << odom << std::endl;

        double force_x(0), torque_z(0);
        double t__ = YP::YPSpur_get_force(&force_x, &torque_z);
        if(t__ < 0){
            std::cerr << "\033[31minvalid time: " << t << "\033[0m" << std::endl;
            break;
        }
        std::cout << "f_x: " << force_x << ", tau_z: " << torque_z << std::endl;

        set_control_mode(ControlMode::MODE::VELOCITY);
        send_velocity(vel);

        static int count = 0;
        std::cout << count << std::endl;
        count++;
        if(YP::YP_get_error_state()){
            break;
        }
        int status;
        if(waitpid(pid, &status, WNOHANG) == pid){
            if(WIFEXITED(status)){
                std::cerr << "\033[31mypspur-coordinator exited\033[0m" << std::endl;;
            }else{
                if(WIFSTOPPED(status)){
                    std::cerr << "\033[31mypspur-coordinator dead with signal " << WSTOPSIG(status) << "\033[0m" << std::endl;;
                }else{
                    std::cerr << "\033[31mypspur-coordinator died\033[0m" << std::endl;;
                }
            }
            break;
        }
        sleep(1 / HZ);
    }
    std::cout << "ypspur_mqtt main loop terminated" << std::endl;
}

void YPSpurMQTT::set_simulation_mode(void)
{
    simulation_flag = true;
}

void YPSpurMQTT::set_param_file(const std::string& param_file)
{
    PARAM_FILE = param_file;
}

void YPSpurMQTT::send_velocity(const Velocity& vel_)
{
    if(control_mode.mode == ControlMode::MODE::VELOCITY){
        YP::YPSpur_vel(vel_.v, vel_.w);
    }
}

void YPSpurMQTT::set_velocity(const Velocity& vel_)
{
    vel = vel_;
}

void YPSpurMQTT::set_control_mode(int mode_)
{
    control_mode.set_mode(mode_);
    switch(control_mode.mode){
        case ControlMode::MODE::OPEN:
            YP::YP_openfree();
            break;
        case ControlMode::MODE::TORQUE:
            YP::YPSpur_free();
            break;
        case ControlMode::MODE::VELOCITY:
            // default
            break;
    }
}

void YPSpurMQTT::set_port(const std::string& port_)
{
    PORT = port_;
}

Odometry YPSpurMQTT::get_odometry(void)
{
    return odom;
}

void YPSpurMQTT::sigint_handler(int sig)
{
    shutdown_flag = true;
}

}// namespace YPSpurMQTT
