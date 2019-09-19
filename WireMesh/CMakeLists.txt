cmake_minimum_required (VERSION 3.14)

include(ExternalProject)

set(CMAKE_CXX_STANDARD 17)

if(WIN32)
    SET(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -DGTE_USE_MSWINDOWS")
else()
    SET(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -D__LINUX__ -DGTE_DEV_OPENGL -DGTE_USE_LINUX -DGTE_DISABLE_PCH")
endif()

project(WireMesh)

##################################
# Download and install GeometricTools

ExternalProject_Add(libGTEngine
	URL https://github.com/vansweej/GeometricTools/archive/feature/upgrade_4.1.zip
	DOWNLOAD_NAME "libGTEngine.zip"
	PREFIX ${CMAKE_CURRENT_BINARY_DIR}/libGTEngine
	# BUILD_COMMAND ""
	INSTALL_COMMAND ""
	)
ExternalProject_Get_Property(libGTEngine source_dir binary_dir)
MESSAGE( STATUS "libGTEngine source_dir: " ${source_dir} )
MESSAGE( STATUS "libGTEngine binary_dir: " ${binary_dir} )
set(LIBGTENGINE_INCLUDE_DIR ${source_dir}/include)
set(libGTEngine debug ${binary_dir}/libGTEngine.a optimized ${binary_dir}/libGTEngine.a)
##################################

add_executable(
	${PROJECT_NAME}
	WireMeshMain.cpp
	WireMeshWindow3.cpp
	WireMeshWindow3.h
	)

target_include_directories( ${PROJECT_NAME} PUBLIC ${LIBGTENGINE_INCLUDE_DIR} )

target_link_libraries( ${PROJECT_NAME} PUBLIC ${libGTEngine} )
target_link_libraries( ${PROJECT_NAME} PUBLIC X11)
target_link_libraries( ${PROJECT_NAME} PUBLIC GL)
target_link_libraries( ${PROJECT_NAME} PUBLIC EGL)
target_link_libraries( ${PROJECT_NAME} PUBLIC GLX)
