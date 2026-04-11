#ifndef PARAMS_HPP
#define PARAMS_HPP

#include <string>
#include <chrono>

namespace params {
    // keys
    inline const std::string TOF_NODE_NAME = "tof_node";
    inline const std::string TOF_TOPIC = "tof_distance";


    // str constants
    inline const std::string I2C_DEVICE = "/dev/i2c-1";
}


#endif