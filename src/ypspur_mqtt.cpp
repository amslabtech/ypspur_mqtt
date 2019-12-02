#include "ypspur_mqtt.h"

YPSpurMQTT::YPSpurMQTT(void)
{
    PORT = "/dev/ttyACM0";
    IPC_KEY = 28741;
    YPSPUR_COORDINATOR = "ypspur-coordinator";
    PARAM_FILE = "";
    HZ = 50;
}

void YPSpurMQTT::spin(void)
{
    std::vector<std::string> args = {
        YPSPUR_COORDINATOR,
        "-d", PORT,
        "--msq-key", std::to_string(IPC_KEY),
    };
    if(PARAM_FILE != ""){
        args.push_back("-p");
        args.push_back(PARAM_FILE);
    }
    std::string command = "";
    char** argv = new char*[args.size() + 1];
    for(unsigned int i=0;i<args.size();i++){
        argv[i] = new char[args[i].size() + 1];
        memcpy(argv[i], args[i].c_str(), args[i].size());
        argv[i][args[i].size()] = 0;
        std::cout << argv[i] << std::endl;
    }
    argv[args.size()] = nullptr;

    pid_t pid = fork();
    int status = 0;
    std::cout << pid << std::endl;
    if(pid == 0){
        std::cout << "succeeded to fork process" << std::endl;
        execvp(YPSPUR_COORDINATOR.c_str(), argv);
        throw(std::runtime_error("failed to start ypspur-coordinator"));
    }else if(pid == -1){
        throw(std::runtime_error("failed to fork process"));
    }else{
        std::cout << "parent process" << std::endl;
    }
    for(unsigned int i=0;i<args.size()+1;i++){
        delete argv[i];
    }
    delete argv;

    if(waitpid(pid, &status, WNOHANG) == pid){
        throw(std::runtime_error("ypspur-coodinator dead immediately"));
    }
}

void YPSpurMQTT::set_port(std::string port_name)
{
    PORT = port_name;
}
