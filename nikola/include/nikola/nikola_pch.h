#pragma once

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

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/ext/quaternion_float.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/euler_angles.hpp>

#include <moodycamel/concurrentqueue.h>
