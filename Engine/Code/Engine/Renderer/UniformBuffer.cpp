#include "Engine/Renderer/UniformBuffer.hpp"
#include "Engine/Renderer/glFunctions.hpp"
#include <stdio.h>
#include <stdlib.h>

UniformBuffer::~UniformBuffer()
{
	glDeleteBuffers(1, &m_handle); 
	m_handle = NULL; 
	CleanUp();
}

UniformBuffer::UniformBuffer()
{

}

void UniformBuffer::CleanUp()
{
	if (m_buffer != nullptr)
		free(m_buffer);
}

void UniformBuffer::SetCPU_Data(size_t byteSize, void const* data)
{
	if (m_handle == NULL)
	{
		glGenBuffers(1, &m_handle);
	}
	if (byteSize != GetSize())
	{
		CleanUp();
	}
	if (m_buffer == nullptr)
	{
		m_buffer = malloc(byteSize);
		m_bufferSize = byteSize;
	}

	if (data == nullptr)
	{
		TODO("find out why this doesn't work for 2D camera");
		memset(m_buffer, NULL, m_bufferSize);
		m_isDirty = false;
	}
	else
	{
		memcpy(m_buffer, data, m_bufferSize);
		m_isDirty = true;
	}
}

void UniformBuffer::UpdateGPU()
{
	if (m_isDirty)
	{
		// bind and copy
		// I can use any buffer binding point I want (I just always use 
		// GL_ARRAY_BUFFER, but you could try using the GL_COPY_WRITE_BUFFER slot?)
		glBindBuffer(GL_ARRAY_BUFFER, m_handle); 
		// send it to the gpu
		glBufferData(GL_ARRAY_BUFFER, m_bufferSize, m_buffer, GL_DYNAMIC_DRAW);

		m_renderBuffer.CopyToGPU(m_bufferSize, m_buffer);
		m_isDirty = false;
	}
}

void UniformBuffer::SetGPU_Data(size_t byteSize, void const* data)
{
	SetCPU_Data(byteSize, data);
	UpdateGPU();
}

void const* UniformBuffer::GetCPU_Buffer() const
{
	return m_buffer;
}

void* UniformBuffer::GetCPU_Buffer()
{
	return m_buffer;
}

size_t UniformBuffer::GetSize() const
{
	return m_bufferSize;
}

GLuint UniformBuffer::GetHandle()
{
	return m_handle;
}
