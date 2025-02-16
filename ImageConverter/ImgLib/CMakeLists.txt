# CMakelists.txt для ImgLib

cmake_minimum_required(VERSION 3.11)

project(ImgLib CXX)
set(CMAKE_CXX_STANDARD 17)

# Укажите путь к библиотеке libjpeg
set(LIBJPEG_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../libjpeg")

if(NOT LIBJPEG_DIR)
    message(FATAL_ERROR "Please, specify LibJPEG directory via -DLIBJPEG_DIR=<dir>")
endif()
message(STATUS "LibJPEG dir is ${LIBJPEG_DIR}, change via -DLIBJPEG_DIR=<dir>")

# Добавляем директорию с заголовочными файлами
include_directories(${LIBJPEG_DIR}/include)

# Указываем исходные файлы для библиотеки
set(IMGLIB_MAIN_FILES img_lib.h img_lib.cpp)

set(IMGLIB_FORMAT_FILES ppm_image.h ppm_image.cpp
                        jpeg_image.h jpeg_image.cpp
                        bmp_image.h bmp_image.cpp)

# Создаем статическую библиотеку ImgLib
add_library(ImgLib STATIC ${IMGLIB_MAIN_FILES} ${IMGLIB_FORMAT_FILES})

# Указываем директорию для включаемых файлов
target_include_directories(ImgLib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

# Добавляем зависимости для релизной и отладочной версий
target_link_directories(ImgLib INTERFACE
    "${LIBJPEG_DIR}/lib/$<IF:$<CONFIG:Debug>,Debug,Release>"
    )

# В качестве зависимости указано jpeg. Компоновщик будет искать файл libjpeg.a
target_link_libraries(ImgLib INTERFACE jpeg)
