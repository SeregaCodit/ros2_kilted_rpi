ARG ROS_DISTRO=jazzy
FROM ros:${ROS_DISTRO}-ros-base

ARG ROS_DISTRO=jazzy

# 1. Налаштування локалі (UTF-8 критично для ROS 2)
RUN apt-get update && apt-get install -y \
    locales \
    && locale-gen en_US en_US.UTF-8 \
    && update-locale LC_ALL=en_US.UTF-8 LANG=en_US.UTF-8 \
    && rm -rf /var/lib/apt/lists/*

ENV LANG=en_US.UTF-8
ENV LC_ALL=en_US.UTF-8

# 2. Інструменти розробки, GUI та залежності
# Об'єднуємо все в один RUN, щоб не плодити проміжні шари
RUN apt-get update && apt-get install -y \
    # GUI інструменти (для PC)
    ros-${ROS_DISTRO}-rqt \
    ros-${ROS_DISTRO}-rqt-common-plugins \
    ros-${ROS_DISTRO}-rqt-graph \
    ros-${ROS_DISTRO}-rqt-reconfigure \
    # СУЧАСНІ бібліотеки графіки для Ubuntu 24.04
    libgl1 \
    libglx-mesa0 \
    libxext6 \
    libxrender1 \
    # Інструменти збірки ROS
    python3-colcon-common-extensions \
    python3-rosdep \
    python3-argcomplete \
    # Системні інструменти
    build-essential \
    cmake \
    git \
    autoconf \
    autoconf-archive \
    libtool \
    pkg-config \
    m4 \
    python3-dev \
    python3-setuptools \
    python3-gpiozero \
    && rm -rf /var/lib/apt/lists/*

# 3. Збірка libgpiod v2.1 з сирців
WORKDIR /tmp
RUN git clone --depth 1 --branch v2.1 https://git.kernel.org/pub/scm/libs/libgpiod/libgpiod.git/ && \
    cd libgpiod && \
    ./autogen.sh --enable-bindings-cxx --enable-bindings-python --enable-tools --prefix=/usr/local && \
    make -j$(nproc) && \
    make install && \
    ldconfig && \
    cd .. && rm -rf libgpiod # Очищення після збірки

# Оновлення бази rosdep
RUN rosdep update

WORKDIR /ros2_ws
ENV ROS_DISTRO=${ROS_DISTRO}
ENV SHELL=/bin/bash

# Налаштування автоматичного сорсингу
RUN echo "source /opt/ros/${ROS_DISTRO}/setup.bash" >> ~/.bashrc && \
    echo "if [ -f /ros2_ws/install/setup.bash ]; then source /ros2_ws/install/setup.bash; fi" >> ~/.bashrc

COPY ./ros_entrypoint.sh /
RUN chmod +x /ros_entrypoint.sh

ENTRYPOINT ["/ros_entrypoint.sh"]
CMD ["bash"]
