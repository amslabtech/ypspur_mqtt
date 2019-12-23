#include "ypspur_wrapper.h"
#include "mosquitto.hpp"
#include "time.h"
#include "sys/time.h"

class CommandVelocityMQTT : public Mosquitto {
  protected:
    const char* topic;
    void onConnected();
    void onError(const char* _msg) { std::cout << _msg; }
    void onMessage(std::string _topic, void* _data, int _len);
    struct timeval ts;

  public:
    CommandVelocityMQTT(const char* _topic):topic(_topic){}

    YPSpurWrapper::VelocityData cmd_vel;
};

void CommandVelocityMQTT::onConnected()
{
    std::cout << "Connected to MQTT broker." << std::endl;;
    subscribe(topic);
}

void CommandVelocityMQTT::onMessage(std::string _topic, void* _data, int _len)
{
    std::cout << "cmd_vel received!" << std::endl;
    gettimeofday(&ts, NULL);
    bcopy(_data, (char*)&cmd_vel, sizeof(cmd_vel));
    cmd_vel.print_data();
}

int main(int argc, char** argv)
{
    std::cout << "=== ypspur_mqtt started ===" << std::endl;
    YPSpurWrapper::YPSpurWrapper* ypspur_wrapper = new YPSpurWrapper::YPSpurWrapper();

    double hz = 50;

    int opt;
    while((opt = getopt(argc, argv, "wp:d:s:")) != -1){
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
            case 's':
                std::cout << optarg << std::endl;
                double hz_ = std::stoi(optarg);
                std::cout << hz_ << std::endl;
                if(hz_ > 0.0){
                    hz = hz_;
                }else{
                    return -1;
                }
                break;
        }
    }
    std::cout << "main loop rate: " << hz << "[hz]" << std::endl;

    const char* ip_addr = "localhost";
    const char* cmd_vel_topic = "cmd_vel";
    const char* odom_topic = "odom";

    Mosquitto odom_publisher;
    odom_publisher.connect(ip_addr);

    CommandVelocityMQTT cmd_vel_listener(cmd_vel_topic);
    cmd_vel_listener.connect(ip_addr);
    cmd_vel_listener.loop_start();
    try{
        ypspur_wrapper->initialize();
        while(!ypspur_wrapper->is_shutdown_requested() && ypspur_wrapper->spin_once()){
            std::cout << "main loop" << std::endl;
            ypspur_wrapper->set_velocity(cmd_vel_listener.cmd_vel);
            YPSpurWrapper::OdometryData odom = ypspur_wrapper->get_odometry();
            odom_publisher.publish(odom_topic, (void*)&odom, sizeof(odom));
            usleep((1 / hz) * 1e6);
        }
    }catch(std::exception& e){
        std::cerr << e.what() << std::endl;
    }
    delete ypspur_wrapper;
    cmd_vel_listener.cleanup_library();
    return 0;
}
