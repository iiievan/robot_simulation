cmake_minimum_required(VERSION 3.26)
project(gazebo_encoder_plugin)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Ручное указание путей (адаптируйте под свою систему)
set(GZ_INCLUDE_DIRS
        /usr/include/gz
        /usr/include/gz/sim10
        /usr/include/gz/transport15
        /usr/include/gz/msgs12
        /usr/include/gz/common7
        /usr/include/gz/math9
        /usr/include/gz/math9/gz/math  # Для Graph.hh
        /usr/include/gz/utils4
        /usr/include/gz/utils4/gz/utils  # Критически важный путь
        /usr/include/gz/sdformat16  # Для sdf/Element.hh
        /usr/include/gz/sdformat16/sdf  # Для SDF версии 13
        /usr/include/gz/sdformat16  # Для sdf/Element.hh
        /usr/include/gz/sdformat16/sdf  # Для SDF версии 13
        /usr/include/gz/plugin4
        /usr/include/gz/plugin4/gz/plugin
)

set(GZ_LIBRARIES
        gz-sim
        gz-transport
        gz-msgs
        gz-common
        gz-math
        gz-utils
        gz-plugin
        sdformat
        protobuf
)

# Проверка наличия заголовочных файлов
foreach(DIR ${GZ_INCLUDE_DIRS})
    if(NOT EXISTS ${DIR})
        message(WARNING "Directory ${DIR} does not exist")
    endif()
endforeach()

# Поиск Protobuf
find_package(Protobuf REQUIRED)

# Библиотека плагина энкодера
add_library(encoder_plugin SHARED
        EncoderPlugin.cc
        EncoderPlugin.hh
)

target_include_directories(encoder_plugin
        PRIVATE
        ${GZ_INCLUDE_DIRS}
        ${Protobuf_INCLUDE_DIRS}
)

# Добавляем пути к библиотекам
target_link_directories(encoder_plugin
        PRIVATE
        /usr/lib/x86_64-linux-gnu
        /usr/local/lib
        /usr/lib
)

target_link_libraries(encoder_plugin
        PRIVATE
        ${GZ_LIBRARIES}
        ${Protobuf_LIBRARIES}
)

# Добавляем стандартные флаги компиляции для Gazebo
target_compile_definitions(encoder_plugin
        PRIVATE
        -DIGN_PROFILER_ENABLE
)

target_compile_options(encoder_plugin
        PRIVATE
        -Wall
        -Wextra
        -Wno-deprecated-declarations
        -fvisibility=default
)

find_program(CLANG_FORMAT_EXE NAMES clang-format clang-format-15 clang-format-14 clang-format-13)
if(CLANG_FORMAT_EXE)
    message(STATUS "Found clang-format for ${PROJECT_NAME}: ${CLANG_FORMAT_EXE}")

    add_custom_target(${PROJECT_NAME}_format
            COMMAND ${CLANG_FORMAT_EXE} -i --style=file ${SOURCE_FILES} ${HEADER_FILES}
            WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
            COMMENT "Formatting source files with clang-format"
    )
else()
    message(WARNING "Could not locate clang-format for ${PROJECT_NAME} - formatting target will not be available")
endif()

# Установка плагина
install(TARGETS encoder_plugin
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

# Копирование модели в пользовательскую директорию Gazebo
install(DIRECTORY models/
        DESTINATION ${CMAKE_INSTALL_PREFIX}/share/gz/sim/models
)