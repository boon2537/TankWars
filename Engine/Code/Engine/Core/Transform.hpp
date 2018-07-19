#pragma once

#include "Engine/Math/Vector3.hpp"
#include "Engine/Math/Matrix44.hpp"

struct transform_t 
{
public:
	transform_t()
		: position(Vector3::zero)
		, euler(Vector3::zero)
		, scale(Vector3::one) {}

	Matrix44 GetMatrix() const; 
	void SetMatrix(const Matrix44& mat); 

	void SetPosition(const Vector3& pos); 
	void Translate(const Vector3& offset); 
	Vector3 GetPosition() const; 

	void SetRotationEuler(const Vector3& eulerAngle); 
	void RotateByEuler(const Vector3& eulerAngle); 
	Vector3 GetEulerAngles() const; 

	void SetScale(const Vector3& s); 
	Vector3 GetScale() const; 

public:
	Vector3 position; 
	Vector3 euler; 
	Vector3 scale; 

	// STATICS
	static transform_t const IDENTITY; 
};

class Transform 
{
public:
	~Transform() {};
	Transform();

	void MakeChild(Transform* parent);

	Matrix44 GetWorldMatrix() const;
	void SetWorldMatrix(const Matrix44& mat);
	Vector3 WorldToLocal(const Vector3& worldPos);
	Vector3 GetWorldPosition() const;

	// these just call through to the the member
	// transform_t 
	Matrix44 GetLocalMatrix() const; 
	void SetLocalMatrix(const Matrix44& mat); 

	void SetLocalPosition(const Vector3& pos); 
	void TranslateLocal(const Vector3& offset); 
	Vector3 GetLocalPosition() const; 

	void SetLocalRotationEuler(const Vector3& euler); 
	void RotateLocalByEuler(const Vector3& euler); 
	Vector3 GetLocalEulerAngles() const; 

	void SetLocalScale(const Vector3& s); 
	Vector3 GetLocalScale() const; 

	void LocalLookAt(const Vector3& localPos, const Vector3& localUp = Vector3::up);
	void WorldLookAt(const Vector3& worldPos, const Vector3& worldUp = Vector3::up);

private:
	transform_t m_localTransform; 
	Transform* m_parent = nullptr;

	// if you want to do matrix caching or 
	// anything else, feel free to add 
	// new members
};