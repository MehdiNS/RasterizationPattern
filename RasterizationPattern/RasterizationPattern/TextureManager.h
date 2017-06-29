#pragma once
#include "Texture.h"

namespace OGL
{
	class TextureManager : Uncopiable
	{
	private:
		std::vector<Texture>	m_texturesId;
		std::vector<u32>		m_freelist;

	public:
		TextureManager();
		~TextureManager();
		
		void printAll();
		u32 createTexture(TextureParams& params, const std::string& filename = std::string(), VerticalFlipUse flip = NO_VFLIP);
		Texture* getTexture(i32 id);
		i32 getTextureHandle(i32 id);
		void bindTexture(i32 id, GLenum textureUnit);
		void destroyTexture(i32 id);
		u32 size();
	};
}