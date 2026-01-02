#pragma once

#include <array>
#include <vector>
#include <string>
#include <sstream>
#include <filesystem> 
#include <fstream>
#include <unordered_map>
#include <random>
#include <thread>
#include <mutex>
#include <queue>
#include <stack>
#include <deque>
#include <functional>
#include <memory>

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/ext/quaternion_float.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/euler_angles.hpp>

#ifdef GLM_FORCE_INTRINSICS
  #include <glm/simd/matrix.h>
#endif

#include <moodycamel/concurrentqueue.h>

// @TODO(PCH): Please don't add this here. Find a way to hide it so that 
// we don't have problems in the future. Thanks.

#include <entt/entt.hpp>
#include <RmlUi/Core.h>
