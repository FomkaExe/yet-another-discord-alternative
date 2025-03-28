cmake_minimum_required(VERSION 3.19)

set(PROJECT_NAME client)

project(${PROJECT_NAME} LANGUAGES CXX)

set(SOURCE_FILES
    main.cpp
    mainwindow.cpp
    client.cpp
)

set(HEADER_FILES
    mainwindow.h
    client.h
)

set(RESOURCE_FILES
    resources.qrc
)

set(QT_LIBRARIES
    Qt::Core
    Qt::Widgets
    Qt6::Network
)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 6.5 REQUIRED COMPONENTS Core Widgets Network)

qt_standard_project_setup()

qt_add_resources(PROJECT_RESOURCES ${RESOURCE_FILES})

qt_add_executable(${PROJECT_NAME}
#WIN32 MACOSX_BUNDLE
    ${SOURCE_FILES}
    ${HEADER_FILES}
    ${PROJECT_RESOURCES}
)

target_link_libraries(${PROJECT_NAME} PRIVATE ${QT_LIBRARIES})

include(GNUInstallDirs)

install(TARGETS ${PROJECT_NAME}
    BUNDLE  DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

qt_generate_deploy_app_script(
    TARGET ${PROJECT_NAME}
    OUTPUT_SCRIPT deploy_script
    NO_UNSUPPORTED_PLATFORM_ERROR
)
install(SCRIPT ${deploy_script})
