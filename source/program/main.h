#ifdef _MBCS
#undef _MBCS
#endif
#define _UNICODE
#define _CRT_SECURE_NO_WARNINGS
#define _USE_MATH_DEFINES
#define CORE_PROFILE

#include <thread>
#include <mutex>
#include <atomic>
#include <sstream>

#include <liboven/oven-core>
#include <liboven/oven-forms>
#include <liboven/oven-avx>
using namespace coreTypes;
using namespace oven::opengl;

//core component
#include <render/shader.h>
#include <render/brdf.h>
#include <render/renderTask.h>
#include <render/subRenderTask.h>
#include <render/progRenderTask.h>
#include <render/imageRenderTask.h>

//CoreTest components
#include <settings/settings.h>
#include <storage/storage.h>
#include <controller/controller.h>

#include <sidebar/sidebar.h>
#include <menuBar/menuBar.h>
#include <statusbar/statusbar.h>
#include <renderWindow/renderWindow.h>
#include <mainWindow/mainWindow.h>
#include <program/coreTest.h>

