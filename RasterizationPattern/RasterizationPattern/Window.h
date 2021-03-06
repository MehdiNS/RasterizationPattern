#pragma once
#include "Uncopiable.h"
//#define RMT_USE_OPENGL 1
//#include <Remotery\lib\Remotery.h>

namespace OGL
{
	class Window : Uncopiable
	{
	public:
		GLFWwindow* m_glfwWindow;
		u32 m_windowWidth;
		u32 m_windowHeight;
		bool m_active;
	public:
		Window(u32 windowWidth = 640, u32 windowHeight = 480);
		~Window();
		void update();
		bool stillRunning();
	};
}