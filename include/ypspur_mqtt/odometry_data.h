#ifndef __ODOMETRY_DATA_H
#define __ODOMETRY_DATA_H
#include <iostream>

namespace YPSpurWrapper{

struct OdometryData{
    int32_t sec;
    int32_t usec;
    float x;
    float y;
    float yaw;
    float v;
    float w;

    void print_data(void)
    {
        std::cout
            << "sec: " << std::setw(12) << sec
            << ", " << "usec: " << std::setw(8) << usec
            << ", " << "x: " << std::setw(8) << x
            << ", " << "y: " << std::setw(8) << y
            << ", " << "yaw: " << std::setw(8) << yaw
            << ", " << "v: " << std::setw(8) << v
            << ", " << "w: " << std::setw(8) << w
            << std::endl;
    }
};

}// namespace YPSpurWrapper

#endif// __ODOMETRY_DATA_H
