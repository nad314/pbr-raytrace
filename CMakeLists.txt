cmake_minimum_required (VERSION 3.10)
project(pbr-raytrace)

if (EXISTS "${PROJECT_SOURCE_DIR}/../liboven")
    add_subdirectory(../liboven liboven)
endif()

if (NOT TARGET oven-core)
    find_package(Oven REQUIRED)
endif()

set(SRCS
	source/controller/controller.cpp 
	source/controller/controller.render.cpp 
	source/hdrloader/hdrloader.cpp 
	source/mainWindow/mainWindow.cpp 
	source/menuBar/menuBar.cpp 
	source/program/main.cpp 
	source/program/coreTest.cpp 
	source/render/imageRenderTask.cpp 
	source/render/progRenderTask.cpp 
	source/render/renderShader.cpp 
	source/render/pbsShader.cpp 
	source/render/renderTask.cpp 
	source/render/subRenderTask.cpp 
	source/render/volumetricShader.cpp 
	source/renderWindow/renderWindow.cpp 
	source/settings/settings.cpp 
	source/sidebar/sidebar.cpp 
	source/statusbar/statusbar.cpp 
	source/storage/storage.cpp)

add_executable(${PROJECT_NAME} ${SRCS})
target_include_directories(${PROJECT_NAME} PUBLIC source)
set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 14)

target_link_libraries(${PROJECT_NAME} PRIVATE oven-avx oven-forms)