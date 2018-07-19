#pragma once

#include "Engine/Math/Matrix44.hpp"
#include <stack>

class MatrixStack 
{
public:
	MatrixStack(); // set m_top to IDENTITY

	void Push();   // push m_top to m_stack
	void Pop();    // Set m_top to the top of m_stack, and pop m_stack

	const Matrix44& GetTop(); // returns m_top

	void Load(const Matrix44& mat); // set m_top to mat; 
	void LoadIdentity(); // set m_top to the IDENTITY matrix
	void Transform(const Matrix44& mat); // set top to the top transformed by mat. 
	void Rotate(const Matrix44& mat);
	void Scale(const Matrix44& mat);

private:
	Matrix44 m_top; // Prefer to keep top separate since it's used a lot
	std::stack<Matrix44> m_stack; // really, a history
};
