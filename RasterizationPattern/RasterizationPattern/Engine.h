#pragma once
#include "Window.h"
#include "Framebuffer.h"
#include "TextureManager.h"
#include "ShaderManager.h"
#include "Gui.h"

namespace OGL
{
	enum OptionValues
	{
		FULLSCREEN_TRI = 0,
		FULLSCREEN_QUAD
	};

	class Engine : Uncopiable
	{
	public:
		Engine();
		~Engine();
		void printPlatformInfo();
		void initGL();
		void run();
		void renderScene();
		void handleKeypress(i32 key, i32 scancode, i32 action, i32 mods);
		void resizeWindow(i32 w, i32 h);
		void windowIconify(i32 iconified);
		
		void renderFirstPass();
		void renderSecondPass();
		void prepareImgui();
		void finalize();

	public:
		Window m_window;
		TextureManager m_textureManager;
		ShaderManager m_shaderManager;
		OptionValues m_optionValues;
		std::unique_ptr<FrameBuffer> m_frameBuffer;
		u32 m_atomicsBuffer;
		u32 m_maxCounter;
		u32 m_time;
		u32 m_sampleTexture;
		i32 m_animationSpeed;
		bool m_forceNewFirstPass;
	};
}