ARG ROS_DISTRO=jazzy
FROM ros:${ROS_DISTRO}-ros-base

ARG ROS_DISTRO=jazzy

# 1. Локалі
RUN apt-get update && apt-get install -y \
    locales \
    && locale-gen en_US en_US.UTF-8 \
    && update-locale LC_ALL=en_US.UTF-8 LANG=en_US.UTF-8 \
    && rm -rf /var/lib/apt/lists/*

ENV LANG=en_US.UTF-8
ENV LC_ALL=en_US.UTF-8

# 2. Встановлюємо інструменти збірки та залежності ROS
RUN apt-get update && apt-get install -y \
    python3-colcon-common-extensions \
    python3-rosdep \
    python3-argcomplete \
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

# Збірка libgpiod v2.1 з сирців
WORKDIR /tmp
RUN git clone --depth 1 --branch v2.1 https://git.kernel.org/pub/scm/libs/libgpiod/libgpiod.git/ && \
    cd libgpiod && \
    ./autogen.sh --enable-bindings-cxx --enable-bindings-python --enable-tools --prefix=/usr/local && \
    make -j$(nproc) && \
    make install && \
    ldconfig

# Ініціалізація rosdep
RUN rosdep update

WORKDIR /ros2_ws
ENV ROS_DISTRO=${ROS_DISTRO}
ENV SHELL=/bin/bash

# Налаштування bashrc
RUN echo "source /opt/ros/${ROS_DISTRO}/setup.bash" >> ~/.bashrc && \
    echo "if [ -f /ros2_ws/install/setup.bash ]; then source /ros2_ws/install/setup.bash; fi" >> ~/.bashrc

COPY ./ros_entrypoint.sh /
RUN chmod +x /ros_entrypoint.sh

ENTRYPOINT ["/ros_entrypoint.sh"]
CMD ["bash"]
