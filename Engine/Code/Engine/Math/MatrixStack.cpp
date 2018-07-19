#include "MatrixStack.hpp"
#include "Engine/Core/EngineCommon.hpp"

MatrixStack::MatrixStack()
{
	m_top = Matrix44();
}

void MatrixStack::Push()
{
	m_stack.push(m_top);
}

void MatrixStack::Pop()
{
	m_top = m_stack.top();
	m_stack.pop();
}

const Matrix44& MatrixStack::GetTop()
{
	return m_top;
}

void MatrixStack::Load(const Matrix44 & mat)
{
	m_top = mat;
}

void MatrixStack::LoadIdentity()
{
	m_top.SetIdentity();
}

void MatrixStack::Transform(const Matrix44 & mat)
{
	m_top.Append(mat);
}

void MatrixStack::Rotate(const Matrix44 & mat)
{
	m_top.Append(mat);
}

void MatrixStack::Scale(const Matrix44 & mat)
{
	m_top.Append(mat);
}
