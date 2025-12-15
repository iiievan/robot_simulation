#!/bin/bash

mkdir -p ~/gazebo_docker_nvidia
cd ~/gazebo_docker_nvidia

cat > Dockerfile << 'EOF'
FROM ubuntu:22.04

ENV DEBIAN_FRONTEND=noninteractive
ENV PROTOBUF_VERSION=3.12.4

RUN apt-get update && apt-get install -y \
    curl \
    gnupg \
    lsb-release \
    software-properties-common \
    build-essential \
    autoconf \
    automake \
    libtool \
    unzip \
    libgl1-mesa-glx \
    libgl1-mesa-dri \
    mesa-utils \
    x11-apps \
    && rm -rf /var/lib/apt/lists/*

RUN curl -sSL https://packages.osrfoundation.org/gazebo.gpg \
    -o /usr/share/keyrings/pkgs-osrf-archive-keyring.gpg

RUN echo "deb [arch=$(dpkg --print-architecture) signed-by=/usr/share/keyrings/pkgs-osrf-archive-keyring.gpg] \
    https://packages.osrfoundation.org/gazebo/ubuntu-stable $(lsb_release -cs) main" \
    | tee /etc/apt/sources.list.d/gazebo-stable.list > /dev/null

RUN cd /tmp && \
    curl -LO https://github.com/protocolbuffers/protobuf/releases/download/v${PROTOBUF_VERSION}/protobuf-cpp-${PROTOBUF_VERSION}.tar.gz && \
    tar -xzf protobuf-cpp-${PROTOBUF_VERSION}.tar.gz && \
    cd protobuf-${PROTOBUF_VERSION} && \
    ./configure --prefix=/usr && \
    make -j$(nproc) && \
    make install && \
    ldconfig && \
    rm -rf /tmp/protobuf*

# 4. Установка Gazebo Harmonic
RUN apt-get update && apt-get install -y \
    gz-harmonic \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /workspace

CMD ["bash"]
EOF

echo "Сборка образа с поддержкой NVIDIA..."
docker build -t gazebo_harmonic_nvidia .

echo "Проверка:"
docker images | grep gazebo_harmonic_nvidia

echo "Для теста NVIDIA:"
echo "docker run --rm --gpus all gazebo_harmonic_nvidia nvidia-smi 2>/dev/null || echo 'NVIDIA не доступна'"