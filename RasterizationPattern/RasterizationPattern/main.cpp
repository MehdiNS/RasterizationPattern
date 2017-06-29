#pragma once 
#include "stdafx.h"
#include "Engine.h"

namespace OGL
{

	Engine engine;

	static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
	{
		if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
			glfwSetWindowShouldClose(window, GL_TRUE);
		engine.handleKeypress(key, scancode, action, mods);
	}

	static void framebuffer_size_callback(GLFWwindow *window, int width, int height)
	{
		engine.resizeWindow(width, height);
	}
}

int main() {
	//Set callbacks
	using namespace OGL;
	glfwSetKeyCallback(engine.m_window.m_glfwWindow, key_callback);
	glfwSetWindowSizeCallback(engine.m_window.m_glfwWindow, framebuffer_size_callback);

	engine.run();
	return 0;
}
