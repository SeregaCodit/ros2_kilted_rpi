#pragma once
#include <memory>
#include <cstdint>
#include "camera_interfaces/i2c_helper.hpp"

namespace simple_drivers{  // простір назв аналог модуля в python

class VL53L0X
{
public:
    explicit VL53L0X(std::shared_ptr<I2CManager> i2c_bus);
    
    ~VL53L0X();

    bool initialize();
    bool start();
    uint16_t read_mm();

private:
    std::shared_ptr<I2CManager> bus_;

    static constexpr uint8_t REG_START = 0x00;
    static constexpr uint8_t REG_RESULT = 0x1E;

};
}