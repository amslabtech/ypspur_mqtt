#include "ypspur_mqtt.h"

int main(int argc, char** argv)
{
    std::cout << "main" << std::endl;
    YPSpurMQTT ypspur_mqtt;
    ypspur_mqtt.spin();
    return 0;
}
