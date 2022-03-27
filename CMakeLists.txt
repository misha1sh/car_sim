cmake_minimum_required(VERSION 3.0.0)
project(cars_sim VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
#set(CMAKE_CXX_FLAGS_DEBUG "-g")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")

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
find_package(OpenCV REQUIRED )

add_subdirectory(libs/fmt)


include_directories(src)
include_directories(SYSTEM ${OSMIUM_INCLUDE_DIRS})
include_directories( ${OpenCV_INCLUDE_DIRS} )

add_executable(cars_sim
        src/main.cpp
        src/ui/main_window.cpp
        src/ui/map/glmapwidget.cpp
        src/ui/main_window.ui
        src/ui/map/map_painter.cpp src/ui/map/map_painter.h src/rasterize/roads_map_reader.cpp src/rasterize/roads_map_reader.h
        src/common/roads_vector_map.h src/projections/lon_lat_to_meters_projector.cpp src/projections/lon_lat_to_meters_projector.h src/common/entities.h src/common/raster_map.h src/common/raster_data.h src/rasterize/roads_rasterizer.cpp src/rasterize/roads_rasterizer.h src/projections/meters_to_image_projector.cpp src/projections/meters_to_image_projector.h src/main/raster_map_builder.cpp src/main/raster_map_builder.h src/main/simulator.cpp src/main/simulator.h src/utils/verify.h src/utils/math_utils.h src/ui/map/camera.h src/ui/draw_settings.h src/utils/guard_holder.h src/rasterize/roads_rasterizer_utils.h src/rasterize/roads_rasterizer_deprecated.h src/rasterize/roads_rasterizer_segmenter.h src/rasterize/roads_rasterizer_handler.h)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)

target_include_directories(cars_sim PRIVATE ${Qt6_INCLUDE_DIRS} )
target_include_directories(cars_sim PUBLIC SYSTEM ${BOOST_INCLUDE_DIRS})

target_link_libraries(cars_sim PRIVATE
    Qt6::Widgets Qt6::OpenGL Qt6::OpenGLWidgets
    range-v3::range-v3 fmt::fmt-header-only
    ${OPENGL_LIBRARIES} ${GLUT_LIBRARY} ${OSMIUM_LIBRARIES} ${BOOST_LIBRARIES} ${OpenCV_LIBS})