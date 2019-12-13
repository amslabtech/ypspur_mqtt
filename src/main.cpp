#include "ypspur_wrapper.h"

int main(int argc, char** argv)
{
    std::cout << "=== ypspur_wrapper started ===" << std::endl;
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

    try{
        ypspur_wrapper->initialize();
        ypspur_wrapper->spin();
    }catch(std::exception& e){
        std::cerr << e.what() << std::endl;
    }
    delete ypspur_wrapper;
    return 0;
}
