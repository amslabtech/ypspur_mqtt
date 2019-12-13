#include "ypspur_wrapper.h"
#include "mosquitto.hpp"

int main(int argc, char** argv)
{
    std::cout << "=== ypspur_mqtt started ===" << std::endl;
    YPSpurWrapper::YPSpurWrapper* ypspur_wrapper = new YPSpurWrapper::YPSpurWrapper();

    int opt;
    while((opt = getopt(argc, argv, "wp:d:")) != -1){
        switch(opt){
            case 'w':
                ypspur_wrapper->set_simulation_mode();
                break;
            case 'p':
                ypspur_wrapper->set_param_file(optarg);
                break;
            case 'd':
                ypspur_wrapper->set_port(optarg);
                break;
        }
    }

    Mosquitto mqtt;
    const char* ip_addr  = "localhost";
    const char* cmd_vel_topic    = "cmd_vel";
    mqtt.connect(ip_addr);

    try{
        ypspur_wrapper->initialize();
        while(!ypspur_wrapper->is_shutdown_requested() && ypspur_wrapper->spin_once()){
            std::cout << "main loop" << std::endl;
            usleep(1e6);
        }
    }catch(std::exception& e){
        std::cerr << e.what() << std::endl;
    }
    delete ypspur_wrapper;
    return 0;
}
