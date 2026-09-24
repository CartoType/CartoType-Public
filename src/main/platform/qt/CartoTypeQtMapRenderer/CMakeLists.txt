cmake_minimum_required(VERSION 3.16)
project(CartoTypeQtMapRenderer VERSION 1.0 LANGUAGES C CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

find_package(Qt6 REQUIRED COMPONENTS Gui)

qt_standard_project_setup()

qt_add_library(CartoTypeQtMapRenderer STATIC
    ../../../graphics/cartotype_opengl_es_render.cpp
    ../../../library/libtess2/bucketalloc.c
    ../../../library/libtess2/dict.c
    ../../../library/libtess2/geom.c
    ../../../library/libtess2/mesh.c
    ../../../library/libtess2/priorityq.c
    ../../../library/libtess2/sweep.c
    ../../../library/libtess2/tess.c
    cartotype_qt_map_renderer.cpp cartotype_qt_map_renderer.h
)
target_include_directories(CartoTypeQtMapRenderer PUBLIC
    ../../../base
    ../../../framework
    ../../../graphics
    ../../../library/libtess2
    ../../../library/opengl_es
    ../../../mapdata
    ../../../projection
    ../../../svg
    ../../../xml
)

target_compile_definitions(CartoTypeQtMapRenderer PUBLIC
    CARTOTYPE_FOR_QT
    NDEBUG
)

target_link_libraries(CartoTypeQtMapRenderer PUBLIC
    Qt::Core
    Qt::Gui
)

install(TARGETS CartoTypeQtMapRenderer
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    FRAMEWORK DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
