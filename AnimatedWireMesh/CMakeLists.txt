cmake_minimum_required (VERSION 3.14)

include(ExternalProject)

set(CMAKE_CXX_STANDARD 17)

if(WIN32)
	SET(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -DGTE_USE_DIRECTX -DGTE_USE_MSWINDOWS -DGTE_USE_ROW_MAJOR -DGTE_USE_MAT_VEC -D_SILENCE_ALL_CXX17_DEPRECATION_WARNINGS -DUNICODE -D_UNICODE")
else()
    SET(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -D__LINUX__ -DGTE_DEV_OPENGL -DGTE_USE_LINUX -DGTE_DISABLE_PCH")
endif()

project(AnimatedWireMesh)

##################################
# Download and install GeometricTools

ExternalProject_Add(libGTEngineProj
	URL https://github.com/vansweej/GeometricTools/archive/feature/fix_windows_build.zip
	DOWNLOAD_NAME "libGTEngine.zip"
	PREFIX ${CMAKE_CURRENT_BINARY_DIR}/libGTEngine
	# BUILD_COMMAND ""
	CMAKE_ARGS ${CMAKE_CXX_FLAGS}
	BUILD_BYPRODUCTS <BINARY_DIR>/GTEngine.lib
	INSTALL_COMMAND ""
	)
ExternalProject_Get_Property(libGTEngineProj source_dir binary_dir)
MESSAGE( STATUS "libGTEngine source_dir: " ${source_dir} )
MESSAGE( STATUS "libGTEngine binary_dir: " ${binary_dir} )
set(LIBGTENGINE_INCLUDE_DIR ${source_dir}/include)
if(WIN32)
	set(libGTEngine debug ${binary_dir}/GTEngine.lib optimized ${binary_dir}/GTEngine.lib)
else()
    if(UNIX AND NOT APPLE)
		set(libGTEngine debug ${binary_dir}/libGTEngine.a optimized ${binary_dir}/libGTEngine.a)
    else()
		set(libGTEngine debug ${binary_dir}/libGTEngine.a optimized ${binary_dir}/libGTEngine.a)
    endif()
endif(WIN32)


##################################

add_executable(
	${PROJECT_NAME}
	WireMeshMain.cpp
	WireMeshWindow3.cpp
	WireMeshWindow3.h
	)

add_dependencies(${PROJECT_NAME} libGTEngineProj)

target_include_directories( ${PROJECT_NAME} PUBLIC ${LIBGTENGINE_INCLUDE_DIR} )

target_link_libraries( ${PROJECT_NAME} PUBLIC ${libGTEngine} )

if(WIN32)
	target_link_libraries( ${PROJECT_NAME} PUBLIC d3d11.lib)
	target_link_libraries( ${PROJECT_NAME} PUBLIC d3dcompiler.lib)
	target_link_libraries( ${PROJECT_NAME} PUBLIC dxgi.lib)
	target_link_libraries( ${PROJECT_NAME} PUBLIC dxguid.lib)
	target_link_libraries( ${PROJECT_NAME} PUBLIC Windowscodecs.lib)
else()
    if(UNIX AND NOT APPLE)
		target_link_libraries( ${PROJECT_NAME} PUBLIC X11)
		target_link_libraries( ${PROJECT_NAME} PUBLIC GL)
		target_link_libraries( ${PROJECT_NAME} PUBLIC EGL)
		target_link_libraries( ${PROJECT_NAME} PUBLIC GLX)
	else()
	endif()
endif(WIN32)

