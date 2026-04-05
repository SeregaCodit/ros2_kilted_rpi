cmake_minimum_required(VERSION 3.8)
project(diode_flickering)

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# find dependencies
find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)

# 2. Шукаємо libgpiodcxx (наша кастомна збірка)
# Шукаємо шлях до gpiod.hpp
find_path(GPIOD_INCLUDE_DIRS gpiod.hpp REQUIRED)
# Шукаємо саму бібліотеку libgpiodcxx.so
find_library(GPIOD_LIBRARIES gpiodcxx REQUIRED)
# uncomment the following section in order to fill in
# further dependencies manually.
# find_package(<dependency> REQUIRED)

add_executable(gpio_node src/gpio_node.cpp)
target_include_directories(gpio_node PRIVATE ${GPIOD_INCLUDE_DIRS})
ament_target_dependencies(gpio_node rclcpp)
target_link_libraries(gpio_node ${GPIOD_LIBRARIES})

install(TARGETS
  gpio_node
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
