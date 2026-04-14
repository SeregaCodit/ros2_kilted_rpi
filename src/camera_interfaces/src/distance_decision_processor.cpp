#include <memory>
#include <chrono>

#include "rclcpp/rclcpp.hpp"
#include "sensor_msgs/msg/range.hpp"
//#include "camera_interfaces/srv/take_snapshot.hpp"
#include "camera_interfaces/params.hpp"


using namespace std::chrono_literals;


class DistanceDecisionProcessor : public rclcpp::Node{
    public:
        DistanceDecisionProcessor(): Node(params::DDP_NODE){

            this->declare_parameter(params::SAFE_DIST_PARAM, params::SAFE_DIST_DEFOULT);

            safe_dist_ = this->get_parameter(params::SAFE_DIST_PARAM).as_double();

            // camera_client_ = this->create_client<camera_interfaces::srv::TakeSnapshot>(params::SNAPSHOT_SERVICE);

            distance_subscriber_ = this->create_subscription<sensor_msgs::msg::Range>(
                params::TOF_DISTANCE,
                10,
                std::bind(&DistanceDecisionProcessor::distance_callback, this, std::placeholders::_1)
            );

            RCLCPP_INFO(this->get_logger(), "Initialized!");
        }
        
        
        void distance_callback(const sensor_msgs::msg::Range::SharedPtr msg){

            if (msg->range < safe_dist_){
                RCLCPP_WARN(this->get_logger(), "Detection at distancr %.2f m!", msg->range);
                
            }
        }

        private:
            float safe_dist_;
            //rclcpp::Client<camera_interfaces::srv::TakeSnapshot>::SharedPtr camera_client_;
            rclcpp::Subscription<sensor_msgs::msg::Range>::SharedPtr distance_subscriber_;
};


int maint(int argc, char * argv[]){
    rclcpp::init(argc, argv);

    char hostname[1024];
    gethostname(hostname, 1024);
    if (hostname != params::ALLOWED_HOST){
        RCLCPP_ERROR(rclcpp::get_logger("HOST_CHECK"), "Don`t run this on dev host!");
        return 1;
    }

    {
        auto node = std::make_shared<DistanceDecisionProcessor>();

        try{
            rclcpp::spin(node);
        } catch (...) {
            return 1;
        }
    }

    rclcpp::shutdown();
    return 0;
}
