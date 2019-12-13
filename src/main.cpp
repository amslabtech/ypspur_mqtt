#include "ypspur_mqtt.h"

int main(int argc, char** argv)
{
    std::cout << "=== ypspur_mqtt started ===" << std::endl;
    YPSpurMQTT::YPSpurMQTT* ypspur_mqtt = new YPSpurMQTT::YPSpurMQTT();

    int opt;
    while((opt = getopt(argc, argv, "wp:d:")) != -1){
        switch(opt){
            case 'w':
                ypspur_mqtt->set_simulation_mode();
                break;
            case 'p':
                ypspur_mqtt->set_param_file(optarg);
                break;
            case 'd':
                ypspur_mqtt->set_port(optarg);
                break;
        }
    }

    try{
        ypspur_mqtt->initialize();
        ypspur_mqtt->spin();
    }catch(std::exception& e){
        std::cerr << e.what() << std::endl;
    }
    delete ypspur_mqtt;
    return 0;
}
