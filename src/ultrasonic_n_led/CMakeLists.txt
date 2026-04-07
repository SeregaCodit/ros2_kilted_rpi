cmake_minimum_required(VERSION 3.8)
project(ultrasonic_n_led)

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# find dependencies
find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(std_msgs REQUIRED)

#libgpiodcxx збиралась з бінарників
# Шукаємо шлях до gpiod.hpp
find_path(GPIOD_INCLUDE_DIRS gpiod.hpp REQUIRED)
# Шукаємо саму бібліотеку libgpiodcxx.so
find_library(GPIOD_LIBRARIES gpiodcxx REQUIRED)


# executables
# ultrasonic node
add_executable(distance_publisher src/ultrasonic_node.cpp)
target_include_directories(distance_publisher PRIVATE include ${GPIOD_INCLUDE_DIRS})
target_link_libraries(distance_publisher ${GPIOD_LIBRARIES})
ament_target_dependencies(distance_publisher rclcpp std_msgs)

add_executable(distance_led src/dist_led_node.cpp)
target_include_directories(distance_led PRIVATE include ${GPIOD_INCLUDE_DIRS})
target_link_libraries(distance_led ${GPIOD_LIBRARIES})
ament_target_dependencies(distance_led rclcpp std_msgs)


install(TARGETS
  distance_publisher
  distance_led
  DESTINATION lib/${PROJECT_NAME}
)


install(DIRECTORY
  launch
  config
  DESTINATION share/${PROJECT_NAME}
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
