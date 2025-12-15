#!/bin/bash

export LD_LIBRARY_PATH=/home/mbokastnev/code/robot_simulation/cmake-build-debug:$LD_LIBRARY_PATH
export GZ_SIM_SYSTEM_PLUGIN_PATH=/home/mbokastnev/code/robot_simulation/cmake-build-debug

echo "GZ_SIM_SYSTEM_PLUGIN_PATH: $GZ_SIM_SYSTEM_PLUGIN_PATH"

env -u WAYLAND_DISPLAY gz sim -v4 esipovo.sdf