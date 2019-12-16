#ifndef __ODOMETRY_DATA_H
#define __ODOMETRY_DATA_H
#include <iostream>

namespace YPSpurWrapper{

struct OdometryData{
    int32_t id;
    float x;
    float y;
    float yaw;
    float v;
    float w;

    void print_data(void)
    {
        std::cout << "id: " << id
          << ", " << "x: " << x
          << ", " << "y: " << y
          << ", " << "yaw: " << yaw
          << ", " << "v: " << v
          << ", " << "w: " << w
          << std::endl;
    }
};

}// namespace YPSpurWrapper

#endif// __ODOMETRY_DATA_H
