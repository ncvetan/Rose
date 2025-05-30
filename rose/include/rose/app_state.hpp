// =============================================================================
//   non platform specific application state
// =============================================================================

#ifndef ROSE_INCLUDE_APP_STATE
#define ROSE_INCLUDE_APP_STATE

#include <rose/camera.hpp>
#include <rose/entities.hpp>
#include <rose/core/types.hpp>

#include <GLFW/glfw3.h>
#include <imgui.h>

struct WindowState {
        
    GLFWwindow* window_handle = nullptr;
    u32 width = 1920;
    u32 height = 1080;
    ImGuiID dock_id = 0;
    std::string name = "Rose";

    vec2f last_xy;
    vec2f mouse_xy;

    f32 gamma = 2.2f;
    f32 exposure = 1.0f;
    bool vsync_enabled = true;

    Rectf vp_rect;
    bool vp_focused = false;
    bool vp_captured = true; // indicates whether events should be processed in the viewport
};

struct AppState {
    WindowState window_state;
    Camera camera;
    Entities entities;

    bool bloom_enabled = true;
    i32 n_bloom_passes = 5;
    f32 bloom_threshold = 1.0f;
};

#endif