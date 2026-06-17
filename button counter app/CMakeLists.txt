cmake_minimum_required(VERSION 3.16)
project(ButtonCounterProject VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Force CMake to find your Qt 6.9.3 installation
set(CMAKE_PREFIX_PATH "F:/6.9.3/mingw_64" CACHE PATH "Qt Path" FORCE)

find_package(Qt6 6.9 REQUIRED COMPONENTS Quick WebSockets)

qt_standard_project_setup(REQUIRES 6.9)

# --- ADD COVERAGE INSTRUMENTATION FLAGS HERE ---
if(CMAKE_COMPILER_IS_GNUCXX)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --coverage -g")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} --coverage")
endif()

qt_add_executable(ButtonCounterApp
    src/main.cpp
    src/counter.cpp
    src/counter.h
)

qt_add_qml_module(ButtonCounterApp
    URI Main
    VERSION 1.0
    QML_FILES src/main.qml
)

target_link_libraries(ButtonCounterApp
    PRIVATE 
    Qt6::Quick
    Qt6::WebSockets
)