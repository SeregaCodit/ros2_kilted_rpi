from launch import LaunchDescription
from launch.actions import DeclareLaunchArgument
from launch.substitutions import LaunchConfiguration

from launch_ros.actions import Node


def generate_launch_description():
    return LaunchDescription([
        DeclareLaunchArgument(
            "chip_name", default_value="gpiochip0",
            description="a name of gpiochip. Use: gpiodetect command"
        ),
        DeclareLaunchArgument(
            "line_offset", default_value="17",
            description="line offset valule. Use: pinout command or gpioinfo command"
        ),
        DeclareLaunchArgument(
            "sleep_sec", default_value="1",
            description="Blinking interval in seconds"
        ),
        Node(
            package="diode_flickering",
            executable="gpio_node",
            name="gpio_node",
            parameters=[{
                "chip_name": LaunchConfiguration("chip_name"),
                "line_offset": LaunchConfiguration("line_offset"),
                "sleep_sec": LaunchConfiguration("sleep_sec"),
            }],
            output="screen"
        )
    ])