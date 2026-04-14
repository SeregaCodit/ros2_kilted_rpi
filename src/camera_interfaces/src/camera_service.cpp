#include <memory>
#include <vector>



#include "rclcpp/rclcpp.hpp"
#include "camera_interfaces/srv/take_snapshot.hpp"
#include "camera_interfaces/params.hpp"
#include <opencv2/opencv.hpp>


using namespace std::placeholders;


class CameraService : public rclcpp::Node
{
public:
    CameraService(): Node(params::CAMERA_SRV)
    {
        srv_ = this->create_service<camera_interfaces::srv::TakeSnapshot>(
            params::SNAPSHOT_SRV,
            std::bind(&CameraService::on_snaphot, this, _1, _2)
            
        );

        RCLCPP_INFO(this->get_logger(), "Initialized!");
    }

private:
    void on_snaphot(
        const std::shared_ptr<camera_interfaces::srv::TakeSnapshot::Request> request,
        std::shared_ptr<camera_interfaces::srv::TakeSnapshot::Response> response)
    {


    }

    rclcpp::Service<camera_interfaces::srv::TakeSnapshot>::SharedPtr srv_;
};