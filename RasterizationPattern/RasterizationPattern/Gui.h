#pragma once

namespace OGL
{
		template <typename T>
		void drawLabel(const char* label, T value)
		{
			string temp = std::to_string(value);
			ImGui::LabelText(label, temp.c_str());
		}

		void newFrame();
		void drawLabel(const char* label, const char* value);
		void drawSlider(const char* label, f32* value, f32 min, f32 max);
		void drawSlider(const char* label, i32* value, i32 min, i32 max);
		void renderGui();
}