#!/bin/bash
set -e

# Сорсимо основний ROS 2
source "/opt/ros/$ROS_DISTRO/setup.bash"

# Сорсимо наш воркспейс, якщо він вже зібраний
# Аналогія: додавання поточного проєкту у PYTHONPATH
if [ -f "/ros2_ws/install/setup.bash" ]; then
  source "/ros2_ws/install/setup.bash"
fi

exec "$@"
