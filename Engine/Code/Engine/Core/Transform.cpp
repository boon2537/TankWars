#include "Engine/Core/Transform.hpp"

Matrix44 transform_t::GetMatrix() const
{
	Matrix44 transform = Matrix44();
	transform.Append(Matrix44::MakeTranslation(position));
	transform.Append(Matrix44::MakeFromEuler(euler));
	transform.Append(Matrix44::MakeScale(scale.x, scale.y, scale.z));

	return transform;
}

void transform_t::SetMatrix(const Matrix44& mat)
{
	position = mat.GetPosition();
	euler = mat.GetEuler();
	scale = mat.GetScale();
}

void transform_t::SetPosition(const Vector3& pos)
{
	position = pos;
}

void transform_t::Translate(const Vector3& offset)
{
	position += offset;
}

Vector3 transform_t::GetPosition() const
{
	return position;
}

void transform_t::SetRotationEuler(const Vector3& eulerAngle)
{
	euler = eulerAngle;
}

void transform_t::RotateByEuler(const Vector3& eulerAngle)
{
	euler += eulerAngle;
}

Vector3 transform_t::GetEulerAngles() const
{
	return euler;
}

void transform_t::SetScale(const Vector3& s)
{
	scale = s;
}

Vector3 transform_t::GetScale() const
{
	return scale;
}

///////////////////////

Transform::Transform()
{
}

void Transform::MakeChild(Transform* parent)
{
	m_parent = parent;
}

Matrix44 Transform::GetWorldMatrix() const
{
	if (m_parent != nullptr)
	{
		Matrix44 mat = m_parent->GetWorldMatrix();
		mat.Append(GetLocalMatrix());
		return mat;
	}
	else
	{
		return GetLocalMatrix();
	}
}

void Transform::SetWorldMatrix(const Matrix44& mat)
{
 	Matrix44 inverseWorld = Matrix44();
	if (m_parent != nullptr)
	{
		inverseWorld = Matrix44::MakeInverse(m_parent->GetWorldMatrix());
	}

	inverseWorld.Append(mat);
	SetLocalMatrix(inverseWorld);
}

Vector3 Transform::WorldToLocal(const Vector3& worldPos)
{
	Matrix44 inverseWorld = Matrix44();
	if (m_parent != nullptr)
	{
		inverseWorld = Matrix44::MakeInverse(m_parent->GetWorldMatrix());
	}

	Matrix44 temp = Matrix44::MakeTranslation(worldPos);

	temp.Append(inverseWorld);
	return temp.GetPosition();
}

Vector3 Transform::GetWorldPosition() const
{
	return GetWorldMatrix().GetPosition();
}

Matrix44 Transform::GetLocalMatrix() const
{
	return m_localTransform.GetMatrix();
}

void Transform::SetLocalMatrix(const Matrix44& mat)
{
	m_localTransform.SetMatrix(mat);
}

void Transform::SetLocalPosition(const Vector3& pos)
{
	m_localTransform.position = pos;
}

void Transform::TranslateLocal(const Vector3& offset)
{
	m_localTransform.position += offset;
}

Vector3 Transform::GetLocalPosition() const
{
	return m_localTransform.position;
}

void Transform::SetLocalRotationEuler(const Vector3& euler)
{
	m_localTransform.euler = euler;
}

void Transform::RotateLocalByEuler(const Vector3& euler)
{
	m_localTransform.euler += euler;
}

Vector3 Transform::GetLocalEulerAngles() const
{
	return m_localTransform.euler;
}

void Transform::SetLocalScale(const Vector3& s)
{
	m_localTransform.scale = s;
}

Vector3 Transform::GetLocalScale() const
{
	return m_localTransform.scale;
}

void Transform::LocalLookAt(const Vector3& localPos, const Vector3& localUp)
{
	Matrix44 lookAt = Matrix44::LookAt(GetLocalPosition(), localPos, localUp);
	SetLocalMatrix(lookAt);
}

void Transform::WorldLookAt(const Vector3& worldPos, const Vector3& worldUp)
{
	Matrix44 lookAt = Matrix44::LookAt(GetWorldPosition(), worldPos, worldUp);
	SetWorldMatrix(lookAt);
}
