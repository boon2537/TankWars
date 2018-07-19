#pragma once
#include "Engine/Math/Vector2.hpp"
#include "Engine/Math/Vector3.hpp"

class Matrix44
{
public:
	Matrix44() {} // default-construct to Identity matrix (via variable initialization)
	Matrix44( const Matrix44& copyFrom );	
	explicit Matrix44(const float* sixteenValuesBasisMajor); // float[16] array in order Ix, Iy...
	explicit Matrix44(const Vector2& iBasis, const Vector2& jBasis, const Vector2& translation = Vector2(0.f, 0.f));
	explicit Matrix44(const Vector3& iBasis, const Vector3& jBasis, const Vector3& kBasis, const Vector3& translation = Vector3(0.f, 0.f, 0.f));

	// Accessors
	Vector2 TransformPosition2D(const Vector2& position2D); // Written assuming z=0, w=1
	Vector2 TransformDisplacement2D(const Vector2& displacement2D); // Written assuming z=0, w=0

	// Mutators
	void SetIdentity();
	void SetValues(const float* sixteenValuesBasisMajor); // float[16] array in order Ix, Iy...
	void Append(const Matrix44& matrixToAppend); // a.k.a. Concatenate (right-multiply)
	void RotateDegrees2D(float rotationDegreesAboutZ); // 
	void Translate2D(const Vector2& translation);
	void Translate(const Vector3& translation);
	void SetTranslation(const Vector3& translation);
	void ScaleUniform2D(float scaleXY);
	void Scale2D(float scaleX, float scaleY);

	Vector3 GetPosition() const;
	Vector3 GetForward() const;
	Vector3 GetRight() const;
	Vector3 GetUp() const;
	Vector3 GetEuler() const;
	Vector3 GetScale() const;

	// Producers
	static Matrix44 MakeRotationDegrees2D(float rotationDegreesAboutZ);
	static Matrix44 MakeYawRotationDegrees(float rotationDegreesAboutY);
	static Matrix44 MakePitchRotationDegrees(float rotationDegreesAboutX);
	static Matrix44 MakeFromEuler(const Vector3& eulerRotation);
	static Matrix44 MakeTranslation2D(const Vector2& translation);
	static Matrix44 MakeTranslation(const Vector3& translation);
	static Matrix44 MakeScaleUniform2D(float scaleXY);
	static Matrix44 MakeScale2D(float scaleX, float scaleY);
	static Matrix44 MakeScale(float scaleX, float scaleY, float scaleZ);
	static Matrix44 MakeOrtho2D(const Vector2& bottomLeft, const Vector2& topRight);
	static Matrix44 MakeOrtho(const Vector2& bottomLeft, const Vector2& topRight, float orthoNear, float orthoFar);
	static Matrix44 MakePerspectiveProjection(float fov_degrees, float aspect, float nz, float fz);
	static Matrix44 LookAt(const Vector3& position, const Vector3& target, const Vector3& up = Vector3::up);
	static Matrix44 MakeInverseFast(const Matrix44& mat);
	static Matrix44 MakeInverse(const Matrix44& mat);
	static Matrix44 TurnToward(const Matrix44& current, const Matrix44& target, float maxTurnRadians);

	// Helper
	float GetTrace3() const;

public:
	float Ix = 1;
	float Iy = 0;
	float Iz = 0;
	float Iw = 0;

	float Jx = 0;
	float Jy = 1;
	float Jz = 0;
	float Jw = 0;

	float Kx = 0;
	float Ky = 0;
	float Kz = 1;
	float Kw = 0;

	float Tx = 0;
	float Ty = 0;
	float Tz = 0;
	float Tw = 1;

	static const Matrix44 identity;
};

const Matrix44 LerpTransform(const Matrix44& a, const Matrix44& b, float t);
