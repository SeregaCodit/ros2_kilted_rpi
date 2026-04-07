#ifndef PARAMS_HPP
#define PARAMS_HPP

#include <string>
#include <chrono>


namespace params {
    using namespace std::chrono_literals;
    
    // key constants
    inline const std::string TRIG_PIN = "trig_pin";
    inline const std::string ECHO_PIN = "echo_pin";
    inline const std::string RED_PIN = "red_pin";
    inline const std::string GREEN_PIN = "green_pin";
    inline const std::string CHIP = "chip";
    inline const std::string TRHESHOLD = "distance_threshold";
    inline const std::string DETECTED_DIST_TOPIC = "detected_distance";
    inline const std::string HOST_CHECK = "HOST CHECK";

    

    //numeric constants
    inline constexpr float SPEED_OF_SOUND = 34300.0f;
    inline constexpr int QOS = 10;
    inline constexpr float SAFE_DIST = 20.0f;
    inline constexpr std::chrono::milliseconds ULTRASONIC_CALLBACK_PERIOD = 100ms;
    inline constexpr std::chrono::microseconds TRIG_SLEEP = 10us;
    inline constexpr std::chrono::milliseconds ECHO_TIMEOUT = 10ms;

    // string constants
    inline const std::string GPIOCHOP = "/dev/gpiochip0";
    inline const std::string ULTRASONIC_NODE = "ultrasonic_node";
    inline const std::string DIST_LED_NODE = "distance_safety_led_node";
    inline const std::string HOSTNAME = "rpi";

}

#endif
