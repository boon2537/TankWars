#pragma once

#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Core/Image.hpp"

class TextureCube
{
public:
	~TextureCube();
	TextureCube();

	void CleanUp();

	// make a cube map from 6 images
	// +x -x +y -y +z -z
	bool MakeFromImages(const Image* images);
	bool MakeFromImages(const Image& image);
	bool MakeFromImages(const char* fileName); 

	inline uint GetWidth() const { return m_size; }
	inline uint GetHeight() const { return m_size; }
	inline uint GetHandle() const { return m_handle; }
	inline bool IsValid() const { return (m_handle != NULL); }

public:
	uint m_size; // cube maps are equal length on all sizes 
	uint m_handle; 
	uint m_format;
};
