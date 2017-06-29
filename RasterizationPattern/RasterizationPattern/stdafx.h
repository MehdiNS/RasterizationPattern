#pragma once

// Because windows.h ...
#define WIN32_LEAN_AND_MEAN

// GLAD
#ifdef _WIN32
#define APIENTRY __stdcall
#endif
#include <glad\glad.h>

// GLFW
#include <glfw/glfw3.h>
#ifdef _WIN32
#undef APIENTRY
#define GLFW_EXPOSE_NATIVE_WIN32
#define GLFW_EXPOSE_NATIVE_WGL
#include <glfw/glfw3native.h>
#endif

// Standard library
#include <memory>
#include <algorithm>
#include <iostream>
#include <stdlib.h>
#include <string>
#include <fstream>
#include <assert.h>
#include <stdio.h>
#include <vector>
#include <memory>
#include <unordered_map>
#include <ctime>
#include <iostream>
#include <functional>
#include <chrono>
#include <thread>
#include <random>
#include <map>
#include <array>	
#include <cstddef>
#include <experimental/filesystem>
namespace fs = std::experimental::filesystem;

// Imgui
#include <imgui/imgui.h>
#include "imgui_impl_glfw_gl3.h"

// Common type alias
#include "Types.h"
