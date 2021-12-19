cmake_minimum_required(VERSION 3.5)

project(nfe VERSION 0.2 LANGUAGES CXX)

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
#find_package(hidapi REQUIRED)

set(PROJECT_SOURCES
	src/advanced.cpp
	src/advanced.h
	src/afdata.h
	src/chartview.h
	src/chartview.cpp
	src/controls.cpp
	src/controls.h
	src/device.cpp
	src/device.h
	src/main.cpp
	src/mainwindow.cpp
	src/mainwindow.h
	src/powercurvedialog.h
	src/powercurvedialog.cpp
	src/powercurvedialog.ui
	src/profiles.cpp
	src/profiles.h
	src/rpdata.h
	src/screen.cpp
	src/screen.h
	src/smartstat.cpp
	src/smartstat.h
	src/tfrdialog.h
	src/tfrdialog.cpp
	src/tfrdialog.ui

	src/monitor/callout.h
	src/monitor/callout.cpp
	src/monitor/monitordialog.h
	src/monitor/monitordialog.cpp
	src/monitor/monitordialog.ui

	src/theme/display.h
	src/theme/display.cpp
	src/theme/renderarea.h
	src/theme/renderarea.cpp
	src/theme/iconengine.h
	src/theme/iconengine.cpp
	src/theme/themedialog.h
	src/theme/themedialog.cpp
	src/theme/themedialog.ui
	nfe.qrc
)
set(AF_SOURCES
	src/af/mainwindow.ui
)
set(RP_SOURCES
	src/rp/mainwindow.ui
)

set(LIBS
	Qt${QT_VERSION_MAJOR}::Widgets
	Qt${QT_VERSION_MAJOR}::Charts
#	hidapi::hidraw
	-lhidapi-hidraw
)

set(CMAKE_AUTOUIC_SEARCH_PATHS src/af)
add_executable(af
	${PROJECT_SOURCES}
	${AF_SOURCES}
)
target_compile_definitions(af
	PRIVATE AF
	PRIVATE S_VERSION=12
	PRIVATE OTHER_VERSION=2
)
target_include_directories(af PRIVATE src/af)
target_link_libraries(af PRIVATE ${LIBS})

set(CMAKE_AUTOUIC_SEARCH_PATHS src/rp)
add_executable(rp
	${PROJECT_SOURCES}
	${RP_SOURCES}
)
target_compile_definitions(rp
	PRIVATE RP
	PRIVATE S_VERSION=2
	PRIVATE OTHER_VERSION=12
)
target_include_directories(af PRIVATE src/af)
target_link_libraries(rp PRIVATE ${LIBS})
