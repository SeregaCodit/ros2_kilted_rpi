from launch import LaunchDescription
from launch.substitutions import PathJoinSubstitution

from launch_ros.actions import Node
from launch_ros.substitutions import FindPackageShare


PACKAGE = "ultrasonic_n_led"

def generate_launch_description():
    params = PathJoinSubstitution([
        FindPackageShare(PACKAGE), "config", "params.yaml"
    ])

    return LaunchDescription([
        Node(
            package=PACKAGE,
            executable="distance_publisher",
            name="ultrasonic_node",
            parameters=[params]
        )
    ])