cmake_minimum_required(VERSION 3.8)
project(camera_interfaces)

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# find dependencies
find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(sensor_msgs REQUIRED)
find_package(rosidl_default_generators REQUIRED)
find_package(OpenCV REQUIRED)



rosidl_generate_interfaces(${PROJECT_NAME}
  "srv/TakeSnapshot.srv"
  "srv/SaveImage.srv"
)
rosidl_get_typesupport_target(cpp_typesupport_target "${PROJECT_NAME}" "rosidl_typesupport_cpp")

# -----camera node-----
add_executable(camera_node src/camera_service.cpp)
add_dependencies(camera_node ${cpp_typesupport_target})
target_link_libraries(camera_node ${cpp_typesupport_target} ${OpenCV_LIBRARIES})
target_include_directories(camera_node PRIVATE
  include
  ${OpenCV_INCLUDE_DIRS}
)
ament_target_dependencies(camera_node rclcpp)

# -----distance_decision_processor -----
add_executable(distance_decision_processor src/distance_decision_processor.cpp)
add_dependencies(distance_decision_processor ${cpp_typesupport_target})
target_link_libraries(distance_decision_processor ${cpp_typesupport_target})
target_include_directories(distance_decision_processor PRIVATE include)
ament_target_dependencies(distance_decision_processor rclcpp sensor_msgs)


# -----tof_node-----
add_executable(tof_distance_publisher
  src/tof_node.cpp
  src/vl53l0x_driver.cpp
  )
target_include_directories(tof_distance_publisher PRIVATE include)
ament_target_dependencies(tof_distance_publisher rclcpp sensor_msgs)












install(TARGETS
  tof_distance_publisher
  distance_decision_processor
  camera_node
  DESTINATION lib/${PROJECT_NAME}
)

if(BUILD_TESTING)
  find_package(ament_lint_auto REQUIRED)
  # the following line skips the linter which checks for copyrights
  # comment the line when a copyright and license is added to all source files
  set(ament_cmake_copyright_FOUND TRUE)
  # the following line skips cpplint (only works in a git repo)
  # comment the line when this package is in a git repo and when
  # a copyright and license is added to all source files
  set(ament_cmake_cpplint_FOUND TRUE)
  ament_lint_auto_find_test_dependencies()
endif()

ament_package()
