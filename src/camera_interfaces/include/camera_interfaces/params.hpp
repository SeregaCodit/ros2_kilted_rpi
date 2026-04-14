#ifndef PARAMS_HPP
#define PARAMS_HPP

#include <string>
#include <chrono>

namespace params {
    // keys
    inline const std::string TOF_NODE_NAME = "tof_node";
    inline const std::string TOF_DISTANCE = "tof_distance";
    inline const std::string ALLOWED_HOST = "rpi";
    inline const std::string DDP_NODE = "distance_decision_processor_node";
    inline const std::string SNAPSHOT_SERVICE = "snaphot_service";
    inline const std::string SAFE_DIST_PARAM = "safe_distance";


    // str constants
    inline const std::string I2C_DEVICE = "/dev/i2c-1";
    
    inline constexpr std::int8_t VL5310x_ADDRESS = 0x29;
    inline constexpr float SAFE_DIST_DEFOULT = 0.20f;

}


#endif