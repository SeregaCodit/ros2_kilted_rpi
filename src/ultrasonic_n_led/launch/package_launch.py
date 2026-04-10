from launch import LaunchDescription
from launch.actions import IncludeLaunchDescription, DeclareLaunchArgument
from launch.substitutions import PathJoinSubstitution, LaunchConfiguration

from launch_ros.substitutions import FindPackageShare


def generate_launch_description():
    launch_dir = PathJoinSubstitution([
        FindPackageShare("ultrasonic_n_led"), "launch"
    ])
    ultrasonic_node_launch = PathJoinSubstitution([launch_dir, "ultrasonic_node_launch.py"])
    led_node_launch = PathJoinSubstitution([launch_dir, "led_node_launch.py"])
    return LaunchDescription([
        IncludeLaunchDescription(ultrasonic_node_launch),

        DeclareLaunchArgument("distance_threshold", default_value="20.0",
                              description="Safe distance to ultrasonic sensor"),

        IncludeLaunchDescription(
            led_node_launch,
            launch_arguments={
                "distance_threshold": LaunchConfiguration("distance_threshold")
            }.items()
        )
        
    ])