cmake_minimum_required(VERSION 3.5)

project(nfe VERSION 0.1 LANGUAGES CXX)

if(NOT CMAKE_BUILD_TYPE)
	set(CMAKE_BUILD_TYPE "Release" CACHE STRING	"Default build type: Release" FORCE)
endif()
set(CMAKE_CXX_RELEASE_FLAGS "-O2 -s -DNDEBUG -DQT_NO_DEBUG_OUTPUT -finline-functions ${CMAKE_CXX_COMMON_FLAGS}")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} ${CMAKE_CXX_RELEASE_FLAGS}")

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

configure_file("res/nfe.png" ${CMAKE_CURRENT_BINARY_DIR}/nfe.png COPYONLY)

find_package(QT NAMES Qt5 COMPONENTS Widgets Charts REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Widgets Charts REQUIRED)

set(PROJECT_SOURCES
	src/main.cpp
	src/mainwindow.cpp
	src/mainwindow.h
	src/mainwindow.ui
	src/profiles.cpp
	src/profiles.h
	src/screen.cpp
	src/screen.h
	src/controls.cpp
	src/controls.h
	src/advanced.cpp
	src/advanced.h
	src/smartstat.cpp
	src/smartstat.h
	src/afdata.h
	src/powercurvedialog.h
	src/powercurvedialog.cpp
	src/powercurvedialog.ui
	src/device.cpp
	src/device.h
)

add_executable(nfe
	${PROJECT_SOURCES}
)

target_link_libraries(nfe PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)
target_link_libraries(nfe PRIVATE Qt${QT_VERSION_MAJOR}::Charts)
target_link_libraries(nfe PRIVATE -lhidapi-hidraw)
