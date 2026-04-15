#include "camera_interfaces/vl53l0x_driver.hpp"
#include <iostream>


namespace simple_drivers{
    VL53L0X::VL53L0X(std::shared_ptr<I2CManager> i2c_bus) : bus_(i2c_bus){}

    VL53L0X::~VL53L0X()
    {
        try {
            bus_->write_byte(REG_START, 0x01);
            std::cout<< "[VL53L0X]: safe stop" << std::endl;
        } catch (...){}
    }

    bool VL53L0X::initialize(){
        try{
            bus_->write_byte(0x80, 0x01);
            bus_->write_byte(0xFF, 0x01);
            bus_->write_byte(0x00, 0x00);
            bus_->write_byte(0x91, 0x3c);
            bus_->write_byte(0x00, 0x01);
            bus_->write_byte(0xFF, 0x00);
            bus_->write_byte(0x80, 0x00);
            return true;
        } catch (...){
            return false;
        }
    }

    bool VL53L0X::start(){
        try{
            bus_->write_byte(REG_START, 0x02);
            return true;
        } catch (...){
            return false;
        }
    }

    uint16_t VL53L0X::read_mm(){
        try{
            return bus_->read_word_data(REG_RESULT);
        } catch (...) {
            return 8190;  // Out of range error
        }
    }
}