#include "rclcpp/rclcpp.hpp"
#include "sensor_msgs/msg/range.hpp"
#include "camera_interfaces/vl53l0x_driver.hpp"
#include "camera_interfaces/params.hpp"
#include <unistd.h>

using namespace std::chrono_literals;

class TofNode : public rclcpp::Node
{
public:
    TofNode() : Node(params::TOF_NODE_NAME)
    {
        auto i2c_bus = std::make_shared<I2CManager>(params::I2C_DEVICE, params::VL5310x_ADDRESS);
        sensor_ = std::make_unique<simple_drivers::VL53L0X>(i2c_bus);

        if (sensor_->initialize() && sensor_->start()){
            RCLCPP_INFO(this->get_logger(), "Hardware VL53L0X is ONLINE!");
        }

        publisher_ = this->create_publisher<sensor_msgs::msg::Range>(params::TOF_TOPIC, 10);

        timer_ = this->create_wall_timer(50ms, std::bind(&TofNode::dist_callback, this));

    }

private:
    void dist_callback(){
        uint16_t dist_mm = sensor_->read_mm();
        RCLCPP_DEBUG(this->get_logger(), "Distanse: %d", dist_mm);
        auto msg = sensor_msgs::msg::Range();
        
        msg.header.stamp = this->get_clock()->now();
        msg.range = static_cast<float>(dist_mm) / 1000.0f; // ROS standart in meters

        publisher_->publish(msg);

    }

std::unique_ptr<simple_drivers::VL53L0X> sensor_;
rclcpp::Publisher<sensor_msgs::msg::Range>::SharedPtr publisher_;
rclcpp::TimerBase::SharedPtr timer_;
};


int main(int argc, char * argv[]){
    rclcpp::init(argc, argv);
    
    char hostname[1024];
    gethostname(hostname, 1024);
    if (hostname != params::ALLOWED_HOST){
        RCLCPP_ERROR(rclcpp::get_logger("HOST_CHECK"), "Don`t run this on dev host!");
        return 1;
    }

    {
        auto node = std::make_shared<TofNode>();

        try{
            rclcpp::spin(node);
        } catch (...) {
            return 1;
        }
    }

    rclcpp::shutdown();
    return 0;
}