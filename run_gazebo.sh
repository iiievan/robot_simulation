#!/bin/bash

# run_gazebo.sh
PROJECT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

echo "Запуск Gazebo Harmonic с проектом: $PROJECT_DIR"

# Проверяем наличие мира
if [ ! -f "$PROJECT_DIR/esipovo.sdf" ]; then
    echo "Ошибка: Файл esipovo.sdf не найден в $PROJECT_DIR"
    exit 1
fi

# Разрешаем доступ к X11
xhost +local:docker

# Запускаем контейнер с доступом к GPU
docker run -it --rm \
  --name gazebo-harmonic-esipovo \
  --env="DISPLAY" \
  --env="QT_X11_NO_MITSHM=1" \
  --env="NVIDIA_DRIVER_CAPABILITIES=all" \
  --env="NVIDIA_VISIBLE_DEVICES=all" \
  --env="GAZEBO_MODEL_PATH=$PROJECT_DIR/models:/usr/share/gz-sim/models" \
  --env="GAZEBO_RESOURCE_PATH=$PROJECT_DIR" \
  --volume="/tmp/.X11-unix:/tmp/.X11-unix:rw" \
  --volume="$PROJECT_DIR:/workspace:rw" \
  --device /dev/dri:/dev/dri \
  --gpus all \
  gazebo_harmonic_dockerimage \
  gz sim -v4 /workspace/esipovo.sdf

# После закрытия Gazebo
xhost -local:docker
echo "Gazebo закрыт"