cmake_minimum_required(VERSION 3.16)
project(CartoTypeMaps VERSION 1.0 LANGUAGES C CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 REQUIRED COMPONENTS Gui PrintSupport Widgets OpenGLWidgets)

qt_standard_project_setup()

qt_add_executable(CartoTypeMaps WIN32 MACOSX_BUNDLE
    ../../main/base/cartotype.h
    ../../main/base/cartotype_address.h
    ../../main/base/cartotype_arithmetic.h
    ../../main/base/cartotype_base.h
    ../../main/base/cartotype_bidi.h
    ../../main/base/cartotype_bitmap.h
    ../../main/base/cartotype_char.h
    ../../main/base/cartotype_color.h
    ../../main/base/cartotype_epsg.h
    ../../main/base/cartotype_errors.h
    ../../main/base/cartotype_find_param.h
    ../../main/base/cartotype_framework.h
    ../../main/base/cartotype_graphics_context.h
    ../../main/base/cartotype_iter.h
    ../../main/base/cartotype_legend.h
    ../../main/base/cartotype_map_object.h
    ../../main/base/cartotype_navigation.h
    ../../main/base/cartotype_path.h
    ../../main/base/cartotype_stream.h
    ../../main/base/cartotype_string.h
    ../../main/base/cartotype_transform.h
    ../../main/base/cartotype_types.h
    ../../main/base/pstdint.h
    ../../main/library/rapidxml-1.13/rapidxml.hpp
    ../../main/library/rapidxml-1.13/rapidxml_iterators.hpp
    ../../main/library/rapidxml-1.13/rapidxml_print.hpp
    ../../main/library/rapidxml-1.13/rapidxml_utils.hpp
    attributedialog.cpp attributedialog.h attributedialog.ui
    findaddressdialog.cpp findaddressdialog.h findaddressdialog.ui
    finddialog.cpp finddialog.h finddialog.ui
    layerdialog.cpp layerdialog.h layerdialog.ui
    linestyledialog.cpp linestyledialog.h linestyledialog.ui
    locationdialog.cpp locationdialog.h locationdialog.ui
    main.cpp
    mainwindow.cpp mainwindow.h mainwindow.ui
    mapchildwindow.cpp mapchildwindow.h
    mapform.cpp mapform.h mapform.ui
    onlinemapdialog.cpp onlinemapdialog.h onlinemapdialog.ui
    polygonstyledialog.cpp polygonstyledialog.h polygonstyledialog.ui
    routedialog.cpp routedialog.h routedialog.ui
    routeinstructionsdialog.cpp routeinstructionsdialog.h routeinstructionsdialog.ui
    routeprofiledialog.cpp routeprofiledialog.h routeprofiledialog.ui
    scaledialog.cpp scaledialog.h scaledialog.ui
    styledialog.cpp styledialog.h styledialog.ui
    styleitem.cpp styleitem.h
    stylemodel.cpp stylemodel.h
    svgdialog.cpp svgdialog.h svgdialog.ui
    util.cpp util.h
)
target_include_directories(CartoTypeMaps PRIVATE
    ../../main/base
    ../../main/library/rapidxml-1.13
    ../../main/platform/qt/CartoTypeQtMapRenderer
)

target_compile_definitions(CartoTypeMaps PRIVATE
    NDEBUG
    SQLITE_ENABLE_RTREE
    SQLITE_RTREE_INT_ONLY
    SQLITE_THREADSAFE=0
    XML_STATIC
)

target_link_libraries(CartoTypeMaps PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
    Qt6::OpenGLWidgets
    Qt6::PrintSupport
)

# Resources:
set(icons_resource_files
    "CT_Arrow-RGB-256x256.png"
)

qt_add_resources(CartoTypeMaps "icons"
    PREFIX
        "/"
    FILES
        ${icons_resource_files}
)

if((QT_VERSION_MAJOR GREATER 4))
    target_link_libraries(CartoTypeMaps PRIVATE
        Qt::Widgets
    )
endif()

if((QT_VERSION_MAJOR GREATER 5))
    target_link_libraries(CartoTypeMaps PRIVATE
        Qt::OpenGLWidgets
    )
endif()

if(WIN32 AND CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_link_directories(CartoTypeMaps PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/../../../bin/17.0/x64/DebugDLL/
        ${CMAKE_CURRENT_SOURCE_DIR}/../../main/platform/qt/CartoTypeQtMapRenderer/build/debug
    )

    target_link_libraries(CartoTypeMaps PRIVATE
        CartoTypeQtMapRenderer
        cartotype
        winhttp
    )
endif()

if(WIN32 AND CMAKE_BUILD_TYPE STREQUAL "Release")
    target_link_directories(CartoTypeMaps PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/../../../bin/17.0/x64/ReleaseDLL/
        ${CMAKE_CURRENT_SOURCE_DIR}/../../main/platform/qt/CartoTypeQtMapRenderer/build/release
    )

    target_link_libraries(CartoTypeMaps PRIVATE
        CartoTypeQtMapRenderer
        cartotype
        winhttp
    )
endif()

if(UNIX AND NOT MACOS AND CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_link_directories(CartoTypeMaps PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/../../main/platform/qt/CartoTypeQtMapRenderer/build/debug/
        ${CMAKE_CURRENT_SOURCE_DIR}/../../main/single_library/unix/bin/Debug/
    )

    target_link_libraries(CartoTypeMaps PRIVATE
        CartoTypeQtMapRenderer
        cartotype
        curl
        dl
    )
endif()

if(UNIX AND NOT MACOS AND CMAKE_BUILD_TYPE STREQUAL "Release")
    target_link_directories(CartoTypeMaps PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/../../main/platform/qt/CartoTypeQtMapRenderer/build/release/
        ${CMAKE_CURRENT_SOURCE_DIR}/../../main/single_library/unix/bin/ReleaseLicensed/
    )

    target_link_libraries(CartoTypeMaps PRIVATE
        CartoTypeQtMapRenderer
        cartotype
        curl
        dl
    )
endif()

if(MACOS AND CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_link_directories(CartoTypeMaps PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/../../main/platform/qt/CartoTypeQtMapRenderer/build/debug/
        ${CMAKE_CURRENT_SOURCE_DIR}/../../main/single_library/mac/CartoType/build/Debug/
    )

    target_link_libraries(CartoTypeMaps PRIVATE
        CartoType
        CartoTypeQtMapRenderer
    )
endif()

if(MACOS AND CMAKE_BUILD_TYPE STREQUAL "Release")
    target_link_directories(CartoTypeMaps PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/../../main/platform/qt/CartoTypeQtMapRenderer/build/release/
        ${CMAKE_CURRENT_SOURCE_DIR}/../../main/single_library/mac/CartoType/build/Release/
    )

    target_link_libraries(CartoTypeMaps PRIVATE
        CartoType
        CartoTypeQtMapRenderer
    )
endif()

install(TARGETS CartoTypeMaps
    BUNDLE DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

qt_generate_deploy_app_script(
    TARGET CartoTypeMaps
    OUTPUT_SCRIPT deploy_script
    NO_UNSUPPORTED_PLATFORM_ERROR
)
install(SCRIPT ${deploy_script})
