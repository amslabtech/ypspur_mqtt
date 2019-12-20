#ifndef __VELOCITY_DATA_H
#define __VELOCITY_DATA_H
#include <iostream>

namespace YPSpurWrapper{

struct VelocityData{
    int32_t sec;
    int32_t usec;
    float v;
    float w;

    void print_data(void)
    {
        std::cout
            << "sec: " << std::setw(12) << sec
            << ", " << "usec: " << std::setw(8) << usec
            << ", " << "v: " << std::setw(8) << v
            << ", " << "w: " << std::setw(8) << w
            << std::endl;
    }
};

}// namespace YPSpurWrapper

#endif// __VELOCITY_DATA_H
