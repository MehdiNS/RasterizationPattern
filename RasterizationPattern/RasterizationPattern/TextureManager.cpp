#include "stdafx.h"
#include "TextureManager.h"
#include "Util.h"

namespace OGL
{

	TextureManager::TextureManager()
	{
	}

	TextureManager::~TextureManager()
	{
		for (auto& t : m_texturesId) t.kill();
		m_texturesId.clear();
	}


	void TextureManager::printAll()
	{
		std::cout << "List of the textures in memory\n";
		for (auto& elt : m_texturesId) {
			std::cout << "- " << elt.getName() << "]\n";
		}
	}

	u32 TextureManager::createTexture(TextureParams& params, const std::string& filename, VerticalFlipUse flip)
	{
		if (!m_freelist.empty())
		{
			u32 free = m_freelist.back();
			m_freelist.pop_back();
			m_texturesId[free].m_id = free;
			return free;
		}
		else
		{
			u32 id = m_texturesId.size();
			m_texturesId.push_back(Texture(params, filename, flip, id));
			return id;
		}
	}

	Texture* TextureManager::getTexture(i32 id)
	{
		return m_texturesId[id].m_id == -1 ? nullptr : &m_texturesId[id];
	}

	i32 TextureManager::getTextureHandle(i32 id)
	{
		if (m_texturesId[id].m_id == -1)
			return 0;
		else
			return m_texturesId[id].m_textureObj;
	}

	void TextureManager::bindTexture(i32 id, GLenum textureUnit)
	{
		if (m_texturesId[id].m_id != -1)
			m_texturesId[id].bind(textureUnit);
		//else
		//	// Something is very wrong if we're here
		//	__debugbreak();
	}

	void TextureManager::destroyTexture(int id)
	{
		m_texturesId[id].m_id = -1;
		m_freelist.push_back(id);
	}

	u32 TextureManager::size()
	{
		return static_cast<u32>(m_texturesId.size());
	}

}