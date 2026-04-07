from launch import LaunchDescription
from launch.actions import DeclareLaunchArgument
from launch.substitutions import LaunchConfiguration, PathJoinSubstitution

from launch_ros.actions import Node
from launch_ros.substitutions import FindPackageShare


PACKAGE = "ultrasonic_n_led"


def generate_launch_description():
    params = PathJoinSubstitution([
        FindPackageShare(PACKAGE), "config", "params.yaml"
    ])
    return LaunchDescription([
        DeclareLaunchArgument("distance_threshold", default_value="20.0",
                              description="Safe distance to ultrasonic sensor"),
        Node(
            package=PACKAGE,
            executable="distance_led",
            name="distance_safety_led_node",
            parameters=[
                params,
                {
                    "distance_threshold": LaunchConfiguration("distance_threshold")
                }
            ]
        )
    ])