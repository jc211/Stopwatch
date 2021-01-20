#pragma once
#include <string>
#include <memory>
#include <filesystem>
#include <vector>
#include "../imgui/imgui.h"
#include "../imgui/imgui_impl_opengl3.h"
#include "../imgui/imgui_impl_glfw.h"

void draw_central_dockspace();

struct GLFWwindow;
class Window {
public:
	Window(std::string window_name = "Main", int width = 640, int height = 480);
	~Window();
	void BeginFrame();
	void EndFrame();
	bool ShouldClose();
	std::pair<int, int> size();

private:
	void Clear();
	void Viewport(int x, int y, int width, int height);


public:
	GLFWwindow* glfw_window;
	ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
};
