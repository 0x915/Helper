cmake_minimum_required(VERSION 3.21)

set(Project Helper)

project(${Project})

set(CMAKE_CXX_STANDARD 20)
include_directories(/)

set(OpenCV_DIR C:/Package/OpenCV)
find_package(OpenCV 4.5.5 REQUIRED)

link_directories("C:/Package/spdlog/lib")
link_libraries("C:/Package/spdlog/lib/spdlog.lib")
include_directories(C:/Package/spdlog/include)

link_directories("C:/Package/FMT/lib")
link_libraries("C:/Package/FMT/lib/fmt.lib")
include_directories(C:/Package/FMT/include)

link_directories("C:/Package/pugixml/lib")
link_libraries("C:/Package/pugixml/lib/pugixml.lib")
include_directories(C:/Package/pugixml/include)

add_executable(${Project} main.cpp emulator.cpp global.cpp
        logger.cpp core.cpp match.cpp basic.cpp App.PCR.cpp)

target_link_libraries(${Project} ${OpenCV_LIBS})
#target_link_libraries(${Project} C:/Package/spdlog/lib/spdlog.lib)
#target_link_libraries(${Project} C:/Package/FMT/lib/fmt.lib)

set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /STACK:32000000000")
