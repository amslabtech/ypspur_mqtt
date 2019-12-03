#include "ypspur_mqtt.h"

// static member variable
bool YPSpurMQTT::shutdown_flag;

YPSpurMQTT::YPSpurMQTT(void)
{
    std::cout << "\033[032m--- CONSTRUCT ---\033[0m" << std::endl;
    PORT = "/dev/ttyACM0";
    IPC_KEY = 28741;
    YPSPUR_COORDINATOR = "ypspur-coordinator";
    PARAM_FILE = "";
    HZ = 50;
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

void YPSpurMQTT::set_simulation_mode(void)
{
    simulation_flag = true;
}

void YPSpurMQTT::set_port(std::string port_name)
{
    PORT = port_name;
}

void YPSpurMQTT::set_param_file(const std::string& param_file)
{
    PARAM_FILE = param_file;
}

void YPSpurMQTT::sigint_handler(int sig)
{
    shutdown_flag = true;
}
