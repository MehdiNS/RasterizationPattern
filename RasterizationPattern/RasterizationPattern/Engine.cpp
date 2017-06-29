#include "stdafx.h"
#include "Engine.h"
#include "Util.h"
#include "Quad.h"

namespace OGL
{
	Engine::Engine() :
		m_window{},
		m_frameBuffer{ nullptr },
		m_textureManager{},
		m_shaderManager{},
		m_optionValues{ OptionValues::FULLSCREEN_QUAD },
		m_time{ 0 },
		m_animationSpeed{ 20 },
		m_maxCounter{ 1 }, // Should be 0, but if a problem occurs, a div by 0 error can happen
		m_forceNewFirstPass{ false }
	{
	}

	Engine::~Engine()
	{
	}

	void Engine::initGL()
	{
		// Start GLAD extension handler
		if (!gladLoadGL())
			exit(-1);

		printPlatformInfo();

		// Debug Output
		i32 flags; 
		glGetIntegerv(GL_CONTEXT_FLAGS, &flags);
		if (flags & GL_CONTEXT_FLAG_DEBUG_BIT)
		{
			glEnable(GL_DEBUG_OUTPUT);
			glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
			glDebugMessageCallback((GLDEBUGPROC)glDebugOutput, nullptr);
			glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);
		}

		glEnable(GL_CULL_FACE); 
		glCullFace(GL_BACK);
		glFrontFace(GL_CCW);
		glEnable(GL_DEPTH_TEST); 
		glDepthFunc(GL_LESS);
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

		// Prepare framebuffer
		m_frameBuffer = std::make_unique<FrameBuffer>(m_textureManager, m_window.m_windowWidth, m_window.m_windowHeight);

		// Load shaders
		m_shaderManager.getShader("FirstPassQuad");
		m_shaderManager.getShader("FirstPassTri");
		m_shaderManager.getShader("SecondPass");
		m_shaderManager.getShader("Blit");

		// Create & initalize an atomic buffer
		m_atomicsBuffer = 0;
		glGenBuffers(1, &m_atomicsBuffer);
		glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, m_atomicsBuffer);
		glBufferData(GL_ATOMIC_COUNTER_BUFFER, sizeof(u32), NULL, GL_DYNAMIC_DRAW);
		glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, 0);
		glBindBufferBase(GL_ATOMIC_COUNTER_BUFFER, 0, m_atomicsBuffer);

		glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, m_atomicsBuffer);
		u32* ptr = (u32*)glMapBufferRange(GL_ATOMIC_COUNTER_BUFFER, 0, sizeof(u32),
			GL_MAP_WRITE_BIT |
			GL_MAP_INVALIDATE_BUFFER_BIT |
			GL_MAP_UNSYNCHRONIZED_BIT);
		ptr[0] = 0;
		glUnmapBuffer(GL_ATOMIC_COUNTER_BUFFER);
		glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, 0);

		//i32 blocks = 0;
		//glGetIntegerv(GL_MAX_VERTEX_ATOMIC_COUNTER_BUFFERS, &blocks);
		//printf("GL_MAX_VERTEX_ATOMIC_COUNTER_BUFFERS = %i\n", blocks);
		//glGetIntegerv(GL_MAX_VERTEX_ATOMIC_COUNTERS, &blocks);
		//printf("GL_MAX_VERTEX_ATOMIC_COUNTERS = %i\n", blocks);
		//glGetIntegerv(GL_MAX_FRAGMENT_ATOMIC_COUNTER_BUFFERS, &blocks);
		//printf("GL_MAX_FRAGMENT_ATOMIC_COUNTER_BUFFERS = %i\n", blocks);
		//glGetIntegerv(GL_MAX_FRAGMENT_ATOMIC_COUNTERS, &blocks);
		//printf("GL_MAX_FRAGMENT_ATOMIC_COUNTERS = %i\n", blocks);

		m_sampleTexture = m_textureManager.createTexture(ASSET_TEXTURE2D_MIP_ON, "image.png", OGL::DO_VFLIP);
	}

	void Engine::run()
	{
		initGL();
		ImGui_ImplGlfwGL3_Init(m_window.m_glfwWindow, true);
		renderScene();
		ImGui_ImplGlfwGL3_Shutdown();
	}


	void Engine::renderScene()
	{
		renderFirstPass();

		while (m_window.stillRunning())
		{
			if (m_forceNewFirstPass)
			{
				m_forceNewFirstPass = false;
				m_time = 0;
				renderFirstPass();
			}

			m_frameBuffer->startFrame(m_textureManager);
			newFrame();
			{
				// Compute framerate, frametime
				static f64 previous_seconds = glfwGetTime();
				static u32 frame_count;
				static u32 fps;
				static f64 frametime;
				f64 current_seconds = glfwGetTime();
				f32 dt = static_cast<f32>(current_seconds - previous_seconds);
				if (dt > 0.25) {
					previous_seconds = current_seconds;
					fps = (u32)((f64)frame_count / dt);
					frame_count = 0;
					frametime = 1000. / fps;
				}
				frame_count++;
			}

			renderSecondPass();

			prepareImgui();
			renderGui();

			// Process input
			glfwPollEvents();

			// Finalize frame
			finalize();
			m_time = (m_time + m_animationSpeed) % m_maxCounter;
			/* Render frame, update window stuff*/
			m_window.update();
		}
	}

	void Engine::printPlatformInfo()
	{
		const u8* renderer;
		const u8* version;
		renderer = glGetString(GL_RENDERER);
		version = glGetString(GL_VERSION);
		std::cout << "Renderer:" << renderer << "\n";
		std::cout << "OpenGL version supported " << version << std::endl;
	}

	void Engine::handleKeypress(i32 key, i32 scancode, i32 action, i32 mods)
	{
		if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		{
			glfwSetWindowShouldClose(m_window.m_glfwWindow, GL_TRUE);
		}
		else if (key == GLFW_KEY_F5 && action == GLFW_PRESS)
		{
			m_shaderManager.updateAll();
		}
	}

	void Engine::resizeWindow(i32 w, i32 h)
	{
		if (w != 0 && h != 0)
		{
			m_window.m_active = true;
			m_window.m_windowWidth = w;
			m_window.m_windowHeight = h;
		}
		else
		{
			m_window.m_active = false;
		}
	}

	void Engine::prepareImgui()
	{
		drawSlider("AnimationSpeed", &m_animationSpeed, 1, 300);
		const i8* listbox_items[] = { "fullscreen tri","fullscreen quad" };
		static i32 listbox_item_current = m_optionValues;
		ImGui::ListBox("Mesh", &listbox_item_current, listbox_items, NB_ELEMS(listbox_items), NB_ELEMS(listbox_items));

		if (listbox_item_current != m_optionValues)
		{
			m_optionValues = static_cast<OptionValues>(listbox_item_current);
			m_forceNewFirstPass = true;
		}
	}

	void Engine::finalize()
	{
		glDepthMask(GL_TRUE);
		glEnable(GL_DEPTH_TEST);
		auto& shader = m_shaderManager.getShader("Blit");
		shader.bind();
		m_textureManager.bindTexture(m_frameBuffer->m_finalTextureId, GL_TEXTURE0);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glViewport(0, 0, m_window.m_windowWidth, m_window.m_windowHeight);
		drawQuad();
	}

	void Engine::renderFirstPass()
	{
		glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, m_atomicsBuffer);
		u32* ptr = (u32*)glMapBufferRange(GL_ATOMIC_COUNTER_BUFFER, 0, sizeof(u32),
			GL_MAP_WRITE_BIT |
			GL_MAP_INVALIDATE_BUFFER_BIT |
			GL_MAP_UNSYNCHRONIZED_BIT);
		ptr[0] = 0;
		glUnmapBuffer(GL_ATOMIC_COUNTER_BUFFER);
		glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, 0);

		m_frameBuffer->bindForFirstPass(m_textureManager);
		m_textureManager.bindTexture(m_sampleTexture, GL_TEXTURE0);

		switch (m_optionValues)
		{
		case OptionValues::FULLSCREEN_QUAD:
		{
			m_shaderManager.getShader("FirstPassQuad").bind();
			Quad q{};
			q.render();
			break;
		}
		case OptionValues::FULLSCREEN_TRI:
		{
			m_shaderManager.getShader("FirstPassTri").bind();
			drawQuad();
			break;
		}
		}
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		// Retrieve the value of the atomic counter
		u32 userCounters[1];
		glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, m_atomicsBuffer);
		glGetBufferSubData(GL_ATOMIC_COUNTER_BUFFER, 0, sizeof(u32), userCounters);
		glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, 0);
		m_maxCounter = userCounters[0];
	}

	void Engine::renderSecondPass()
	{
		m_frameBuffer->bindForSecondPass(m_textureManager);
		auto& shader = m_shaderManager.getShader("SecondPass");
		shader.bind();
		m_textureManager.bindTexture(m_frameBuffer->m_atomicCountersId, GL_TEXTURE0);
		m_textureManager.bindTexture(m_sampleTexture, GL_TEXTURE1);
		glUniform1uiv(shader.getUniformLocation("currentTime"), 1, &m_time);
		drawQuad();
	}
}