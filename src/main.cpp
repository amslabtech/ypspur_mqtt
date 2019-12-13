#include "ypspur_wrapper.h"
#include "mosquitto.hpp"
#include "time.h"
#include "sys/time.h"

class CommandVelocity : public Mosquitto {
  protected:
    const char* topic;
    void onConnected();
    void onError(const char* _msg) { std::cout << _msg; }
    void onMessage(std::string _topic, void* _data, int _len);
    struct timeval ts;

  public:
    CommandVelocity(const char* _topic):topic(_topic){}

    YPSpurWrapper::Velocity cmd_vel;
};

void CommandVelocity::onConnected()
{
    std::cout << "Connected to MQTT broker." << std::endl;;
    subscribe(topic);
}

void CommandVelocity::onMessage(std::string _topic, void* _data, int _len)
{
    gettimeofday(&ts, NULL);
    std::cout << (char*)_data << std::endl;
    // bcopy(_data, (char*)&cmd_vel, sizeof(cmd_vel));
    // std::cout << cmd_vel << std::endl;
}

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

    const char* ip_addr = "localhost";
    const char* cmd_vel_topic = "cmd_vel";
    const char* odom_topic = "odom";

    Mosquitto odom_publisher;
    odom_publisher.connect(ip_addr);

    CommandVelocity cmd_vel_listener(cmd_vel_topic);
    cmd_vel_listener.connect(ip_addr);
    cmd_vel_listener.loop_start();
    try{
        ypspur_wrapper->initialize();
        while(!ypspur_wrapper->is_shutdown_requested() && ypspur_wrapper->spin_once()){
            std::cout << "main loop" << std::endl;
            YPSpurWrapper::Odometry odom = ypspur_wrapper->get_odometry();
            // odom_publisher.publish(odom_topic, (void*)&odom, sizeof(odom));
            usleep(1e6);
        }
    }catch(std::exception& e){
        std::cerr << e.what() << std::endl;
    }
    delete ypspur_wrapper;
    cmd_vel_listener.cleanup_library();
    return 0;
}
