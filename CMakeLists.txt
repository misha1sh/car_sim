cmake_minimum_required(VERSION 3.0.0)
project(cars_sim VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

set(CMAKE_PREFIX_PATH "~/Qt/6.2.2/gcc_64/lib/cmake/")
list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")


add_subdirectory(libs/range-v3)

find_package(Osmium REQUIRED COMPONENTS xml io proj)
find_package(Boost REQUIRED)

find_package(Qt6 COMPONENTS Widgets OpenGL OpenGLWidgets REQUIRED)
find_package(OpenGL REQUIRED)
find_package(GLUT REQUIRED)

add_subdirectory(libs/fmt)



include_directories(src)
include_directories(SYSTEM ${OSMIUM_INCLUDE_DIRS})
include_directories(SYSTEM ${BOOST_INCLUDE_DIRS})

add_executable(cars_sim
        src/main.cpp
        src/main_window.cpp
        src/map/glmapwidget.cpp
        src/main_window.ui
        src/map/map_painter.cpp src/map/map_painter.h src/rasterize/roads_extractor.cpp src/rasterize/roads_extractor.h
        src/rasterize/roads_vector_map.cpp src/rasterize/roads_vector_map.h src/rasterize/projector.cpp src/rasterize/projector.h src/common/entities.h)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)

target_include_directories(cars_sim PRIVATE ${Qt6_INCLUDE_DIRS} )
target_link_libraries(cars_sim PRIVATE
    Qt6::Widgets Qt6::OpenGL Qt6::OpenGLWidgets
    range-v3::range-v3 fmt::fmt-header-only
    ${OPENGL_LIBRARIES} ${GLUT_LIBRARY} ${OSMIUM_LIBRARIES} ${BOOST_LIBRARIES})