/// Windows
#if defined(_WIN32) || defined(__WIN32__) || defined(WIN32)
#define SOKOL_D3D11

/// Linux
#elif defined(__linux__) || defined(__gnu_linux__)
#define SOKOL_GLCORE
#endif


#define SOKOL_APP_IMPL
#include "sokol_app.h"
