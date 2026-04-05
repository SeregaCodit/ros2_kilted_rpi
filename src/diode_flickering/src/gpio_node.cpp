#include "rclcpp/rclcpp.hpp"
#include <gpiod.hpp>
#include <chrono>
#include <memory>
#include <unistd.h>  // for hostname check

using namespace std::chrono_literals;


class GpioNode : public rclcpp::Node
{
public:
    GpioNode() : Node("gpio_node")
    {
        this->declare_parameter<std::string>("chip_name", "gpiochip0");
        this->declare_parameter<int>("line_offset", 17);
        this->declare_parameter<int>("sleep_sec", 1);

        std::string chip_name = this->get_parameter("chip_name").as_string();
        int line_offset = this->get_parameter("line_offset").as_int();
        auto sleep_sec = this->get_parameter("sleep_sec").as_int();
        std::chrono::seconds sleep_val(sleep_sec);

        try{
            auto chip = gpiod::chip("/dev/" + chip_name);

            line_request_ = std::make_unique<gpiod::line_request>(
                chip.prepare_request()                                          // create request
                    .set_consumer(this->get_name())                             // set cunsommer
                    .add_line_settings(                                        
                        line_offset,                                             // which pin line_offset, not physical number
                        gpiod::line_settings()                                   // pin settings. if few pins - settings will be same
                            .set_direction(gpiod::line::direction::OUTPUT)       // direction
                    )
                    .do_request()                                                // do request in a core
            );                                                

            RCLCPP_INFO(this->get_logger(), "Initialized %s, line %d", chip_name.c_str(), line_offset);
            offset_ = line_offset;
            timer_ = this->create_wall_timer(sleep_val, std::bind(&GpioNode::timer_callback, this));

        } catch (const std::exception &e){
            RCLCPP_ERROR(this->get_logger(), "GPIO Error: %s", e.what());
            rclcpp::shutdown();
        }
    }

    // class destructor
    ~GpioNode()
    {
        if (line_request_){
            try{
                // set line INACTIVE
                line_request_->set_value(offset_, gpiod::line::value::INACTIVE);
                std::cout << "[GpioNode] Line " << offset_ << "set to INACTIVE" << std::endl;
                // make pin INPUT back
                auto config = gpiod::line_config();
                config.add_line_settings(offset_,
                    gpiod::line_settings().set_direction(gpiod::line::direction::INPUT)
                );

                // apply new config
                line_request_->reconfigure_lines(config);

                std::cout << "\n[GpioNode] Shutdown: GPIO " << offset_ <<" reseted to INPUT/OFF" << std::endl;
            } catch (std::exception &e){
                std::cout << "[GpioNode] Safe shutdown failed!" <<std::endl;
            }
        }
    }

private:
    void timer_callback()
    {
        static bool state = false;
        state = !state;
        line_request_->set_value(offset_, state ? gpiod::line::value::ACTIVE : gpiod::line::value::INACTIVE);  // Python: gpiod::line::value::ACTIVE if state else gpiod::line::value::INACTIVE
        RCLCPP_INFO(this->get_logger(), "GPIO %d, state %s", offset_, state ? "ON" : "OFF");
    }

    std::unique_ptr<gpiod::line_request> line_request_;
    int offset_;
    rclcpp::TimerBase::SharedPtr timer_;
};


int main(int argc, char * argv[]){
    rclcpp::init(argc, argv);
    
    // save check for protect running
    char hostname[1024];
    gethostname(hostname, 1024);
    if (std::string(hostname) != "rpi") {
        RCLCPP_ERROR(rclcpp::get_logger("rclcpp"), "Safety Error: DO NOT run GPIO nodes on dev machine!");
        return 1;
    }

    
    auto node = std::make_shared<GpioNode>();
    rclcpp::spin(node);
    rclcpp::shutdown();
    return 0; 
}