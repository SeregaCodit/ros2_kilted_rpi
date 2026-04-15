#include <memory>
#include <vector>
#include <format>

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
        this->declare_parameter(params::SAVE_PATH_KEY, params::SAVE_PATH_VAL);

        save_path_ = this->get_parameter(params::SAVE_PATH_KEY).as_string();

        srv_ = this->create_service<camera_interfaces::srv::TakeSnapshot>(
            params::SNAPSHOT_SERVICE,
            std::bind(&CameraService::on_snaphot, this, _1, _2)
            
        );

        RCLCPP_INFO(this->get_logger(), "Initialized!");
    }

private:
    void on_snaphot(
        const std::shared_ptr<camera_interfaces::srv::TakeSnapshot::Request> request,
        std::shared_ptr<camera_interfaces::srv::TakeSnapshot::Response> response)
    {
        const char * message;
        cv::VideoCapture cap(0);
        if(!cap.isOpened()){
            message = "Unable to open camera!";
            RCLCPP_ERROR(this->get_logger(), message);
            response->success = false;
            response->message = message;
            return;
        }

        cv::Mat frame;
        cap >> frame;
        cap.release();

        if (frame.empty()){
            message = "Captured frame is empty";
            RCLCPP_INFO(this->get_logger(), message);
            response->success = false;
            response->message = message;
            return;
        }

        std::string full_path = save_path_ + request->filename + ".jpg";

        try
        {
            bool result = cv::imwrite(full_path, frame);

            if (result)
            {   
                
                RCLCPP_INFO(this->get_logger(), "Saved: %s", full_path.c_str());
                response->success = true;
                response->message = full_path + "saved";
            } else {
                throw std::runtime_error("OpenCV imwrite returned false!");
            }
        } catch (const std::exception &e) {
            RCLCPP_ERROR(this->get_logger(), e.what());
            response->success = false;
            response->message = e.what();
        }
    }

    std::string save_path_;
    rclcpp::Service<camera_interfaces::srv::TakeSnapshot>::SharedPtr srv_;
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
        auto node = std::make_shared<CameraService>();

        try{
            rclcpp::spin(node);
        } catch (...) {
            return 1;
        }
    }

    rclcpp::shutdown();
    return 0;
}