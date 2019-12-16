#ifndef __VELOCITY_DATA_H
#define __VELOCITY_DATA_H
#include <iostream>

namespace YPSpurWrapper{

struct VelocityData{
    int32_t id;
    float v;
    float w;

    void print_data(void)
    {
        std::cout << "id: " << id
          << ", " << "v: " << v
          << ", " << "w: " << w
          << std::endl;
    }
};

}// namespace YPSpurWrapper

#endif// __VELOCITY_DATA_H
