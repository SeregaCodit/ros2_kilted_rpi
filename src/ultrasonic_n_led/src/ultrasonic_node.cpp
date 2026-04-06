#include <chrono>
#include <memory>
#include <unistd.h>

#include <gpiod.hpp>
#include "rclcpp/rclcpp.hpp"
#include "std_msgs/msg/float32.hpp"

#include "ultrasonic_n_led/params.hpp"


using namespace std::chrono_literals;


class UltrasonicNode : public rclcpp::Node
{
public: 
    UltrasonicNode() : Node(params::ULTRASONIC_NODE)
    {
        this->declare_parameter(params::TRIG_PIN, 0);
        this->declare_parameter(params::ECHO_PIN, 0);
        this->declare_parameter(params::CHIP, params::GPIOCHOP);

        auto trig = this->get_parameter(params::TRIG_PIN).as_int();
        auto echo = this->get_parameter(params::ECHO_PIN).as_int();
        auto chip = gpiod::chip(this->get_parameter(params::CHIP).as_string());

        // set up trig as OUTPUT echo as INPUT
        line_request_ = std::make_unique<gpiod::line_request>(
            chip.prepare_request()
                .set_consumer(this->get_name())
                .add_line_settings(trig, gpiod::line_settings().set_direction(gpiod::line::direction::OUTPUT))
                .add_line_settings(echo, gpiod::line_settings().set_direction(gpiod::line::direction::INPUT))
                .do_request()
        );

        trig_ = trig; echo_ = echo;
        publisher_ = this->create_publisher<std_msgs::msg::Float32>(params::DETECTED_DIST, params::QOS);
        timer_ = this->create_wall_timer(
            params::ULTRASONIC_CALLBACK_PERIOD,
            std::bind(&UltrasonicNode::timer_callback, this)  
        );
        RCLCPP_DEBUG(this->get_logger(), "initialized!");
    }

    ~UltrasonicNode()
    {
        if (line_request_){
            try{
                line_request_->set_values({
                    {trig_, gpiod::line::value::INACTIVE},
                    {echo_, gpiod::line::value::INACTIVE}
                });
                
                RCLCPP_INFO(this->get_logger(), "All lines reset to INACTIVE");
            } catch(){

            }
        }
    }
private:
    void timer_callback(){
        // trigger pulse
        line_request_->set_value(trig_, gpiod::line::value::ACTIVE);
        rclcpp::sleep_for(params::TRIG_SLEEP);
        line_request_->set_value(trig_, gpiod::line::value::INACTIVE);


        //wait for echo start
        auto start = std::chrono::steady_clock::now();
        while (line_request_->get_value(echo_) == gpiod::line::value::INACTIVE){
            if (std::chrono::steady_clock::now() - start < params::ECHO_TIMEOUT){
                break;
            }
        }

        auto echo_start = std::chrono::steady_clock::now();

        // wait for echo end
        while (line_request_->get_value(echo_) == gpiod::line::value::ACTIVE){
            if (std::chrono::steady_clock::now() - echo_start < params::ECHO_TIMEOUT){
                break;
            }
        }

        auto echo_end = std::chrono::steady_clock::now();

        // calc dist
        std::chrono::duration<double> duration = echo_end - echo_start;
        float distance = (duration.count() * params::SPEED_OF_SOUND) / 2.0;
        RCLCPP_INFO(this->get_logger(), "distance: %f", distance);
        // publish
        auto msg = std_msgs::msg::Float32();
        msg.data = distance;
        publisher_->publish(msg);


    }



    std::unique_ptr<gpiod::line_request> line_request_;
    int trig_, echo_;
    rclcpp::Publisher<std_msgs::msg::Float32>::SharedPtr publisher_;
    rclcpp::TimerBase::SharedPtr timer_;
};


int main (int argc, char * argv[]){
    rclcpp::init(argc, argv);

    char hostname[1024];
    gethostname(hostname, 1024);
    if (std::string(hostname) != params::HOSTNAME){
        RCLCPP_ERROR(rclcpp::get_logger("rclcpp"), "Safety Error: DO NOT run GPIO nodes on dev host!");
        return 1;
    }

    {   
        auto node = std::make_shared<UltrasonicNode>();
        rclcpp::spin(node);
    }

    rclcpp::shutdown();
    return 0;
}