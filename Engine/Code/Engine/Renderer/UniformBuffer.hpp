#pragma once

#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Renderer/RenderBuffer.hpp"

class UniformBuffer
{
public:
	~UniformBuffer(); 
	UniformBuffer();

	void CleanUp(); 

	// copy data to the CPU and dirty the buffer
	void SetCPU_Data(size_t byteSize, void const *data); 

	// update the gpu buffer from the local cpu buffer if dirty
	// and clears the dirty flag
	void UpdateGPU(); 

	// sets the cpu and gpu buffers - clears the dirty flag
	void SetGPU_Data(size_t byteSize, void const *data); 

	// gets a pointer to the cpu buffer (const - so does not dirty)
	void const* GetCPU_Buffer() const; 

	// get a mutable pointer to the cpu buffer.  Sets the dirty flag
	// as it expects the user to change it.
	void* GetCPU_Buffer(); 

	// get this buffer's size in bytes
	size_t GetSize() const; 

	GLuint GetHandle(); 


// template helpers
public:
	//------------------------------------------------------------------------
	// let's me just set a structure, and it'll figure out the size
	template <typename T> 
	void Set(T const &v) 
	{ 
		SetCPU_Data(sizeof(T), &v); 
	}

	//------------------------------------------------------------------------
	// Get a constant reference to the CPU buffer as known struct 
	// would get the same as saying (T*) uniform_buffer->get_cpu_buffer(); 
	template <typename T>
	T const* as() const
	{
		return (T const*) GetCPU_Buffer(); 
	}

	//------------------------------------------------------------------------
	template <typename T>
	T* as() 
	{
		ASSERT_RECOVERABLE(m_bufferSize == sizeof(T), "Data size mismatch");
		m_isDirty = true;
		return (T*) GetCPU_Buffer();

		//usage: m_uniformBuffer->as<camera_state_t>()->projection = m_proj_matrix;
	}

	//Why these don't work for 2D camera?
	/*template <typename T>
	void InitFor()
	{
		SetCPU_Data(sizeof(T), nullptr);
	}

	template <typename T>
	static UniformBuffer* For()
	{
		UniformBuffer* ubo = new UniformBuffer();
		ubo->SetCPU_Data(sizeof(T), nullptr);
		return ubo;
	}*/

	template <typename T>
	static UniformBuffer* For(T const &v) 
	{
		UniformBuffer* ubo = new UniformBuffer();
		ubo->SetCPU_Data(sizeof(T), &v);
		return ubo;
	}

public:
	GLuint m_handle;

	// render buffer backing this constant buffer (gpu memory)
	RenderBuffer m_renderBuffer; 

	// cpu copy of the data stored in m_render-buffer
	void* m_buffer = nullptr;
	size_t m_bufferSize = 0;

	// uses to check if we should update gpu from cpu
	bool m_isDirty = true;
};
