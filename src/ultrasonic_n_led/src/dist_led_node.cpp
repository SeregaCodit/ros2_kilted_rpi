#include <memory>
#include <unistd.h>

#include <gpiod.hpp>

#include "rclcpp/rclcpp.hpp"
#include "std_msgs/msg/float32.hpp"

#include "ultrasonic_n_led/params.hpp"


using namespace std::chrono_literals;
using namespace std::placeholders;


class DistanceSafetyLedNode : public rclcpp::Node
{
public:
    DistanceSafetyLedNode() : Node(params::DIST_LED_NODE)
    {
        this->declare_parameter(params::RED_PIN, 0);
        this->declare_parameter(params::GREEN_PIN, 0);
        this->declare_parameter(params::CHIP, params::GPIOCHOP);
        this->declare_parameter(params::TRHESHOLD, params::SAFE_DIST);

        safe_dist_ = this->get_parameter(params::TRHESHOLD).as_double();
        auto red_line = this->get_parameter(params::RED_PIN).as_int();
        auto green_line = this->get_parameter(params::GREEN_PIN).as_int();
        auto chip = gpiod::chip(this->get_parameter(params::CHIP).as_string());

        line_request_ = std::make_unique<gpiod::line_request>(
            chip.prepare_request()
                .set_consumer(params::DIST_LED_NODE)
                .add_line_settings(red_line, gpiod::line_settings().set_direction(gpiod::line::direction::OUTPUT))
                .add_line_settings(green_line, gpiod::line_settings().set_direction(gpiod::line::direction::OUTPUT))
                .do_request()
        );

        red_line_ = red_line; green_line_ = green_line;
        
        
        subscription_ = this->create_subscription<std_msgs::msg::Float32>(
            params::DETECTED_DIST_TOPIC,
            params::QOS,
            std::bind(&DistanceSafetyLedNode::dist_callback, this, _1)
        );
        
        RCLCPP_INFO(this->get_logger(), "Initialized!");
    }
    
    ~DistanceSafetyLedNode()
    {
        if (line_request_){
            try{
                line_request_->set_values({
                    {red_line_, gpiod::line::value::INACTIVE},
                    {green_line_, gpiod::line::value::INACTIVE}
                });
                
                RCLCPP_INFO(this->get_logger(), "All lines reset to INACTIVE");

                auto config = gpiod::line_config();
                config.add_line_settings(red_line_, gpiod::line_settings().set_direction(gpiod::line::direction::INPUT));
                config.add_line_settings(green_line_, gpiod::line_settings().set_direction(gpiod::line::direction::INPUT));

                RCLCPP_INFO(this->get_logger(), "All lines direction reseted to INPUT");

            } catch (std::exception &e) {
                RCLCPP_ERROR(this->get_logger(), "Safe shutdown failed!\n%s", e.what());
            }
        }
    }
private:
    void dist_callback(const std_msgs::msg::Float32::SharedPtr msg){
        float distance = msg->data;

        if (distance > safe_dist_){
            line_request_->set_values({
                {red_line_, gpiod::line::value::INACTIVE},
                {green_line_, gpiod::line::value::ACTIVE}
            });

            RCLCPP_INFO(this->get_logger(), "distance is safe: %2f sm", distance);
        } else {
            line_request_->set_values({
                {red_line_, gpiod::line::value::ACTIVE},
                {green_line_, gpiod::line::value::INACTIVE}
            });

            RCLCPP_WARN(this->get_logger(), "distance is DANGEROUS: %.2f sm", distance);
        }
    }

    int red_line_, green_line_;
    float safe_dist_;
    std::unique_ptr<gpiod::line_request> line_request_;
    rclcpp::Subscription<std_msgs::msg::Float32>::SharedPtr subscription_;
};


int main(int argc, char * argv[]){
    rclcpp::init(argc, argv);

    char hostname[1024];
    gethostname(hostname, 1024);
    if (std::string(hostname) != params::HOSTNAME){
        RCLCPP_ERROR(rclcpp::get_logger(params::HOST_CHECK), "Safety Error: DO NOT run GPIO nodes on dev host!");
        return 1;
    }

    {
        auto node = std::make_shared<DistanceSafetyLedNode>();
        rclcpp::spin(node);
    }

    rclcpp::shutdown();
    return 0;
}