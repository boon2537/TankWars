#pragma once

#include "Engine/Math/Matrix44.hpp"
#include "Engine/Renderer/FrameBuffer.hpp"
#include "Engine/Renderer/glFunctions.hpp"
#include "Engine/Core/Transform.hpp"

class Texture;
class UniformBuffer;

struct camera_state_t
{
	Matrix44 view;
	Matrix44 projection;
};

struct ray3_t
{
	ray3_t(const Vector3& start, const Vector3& dir)
		: start(start), direction(dir) {}

	Vector3 start; 
	Vector3 direction; 
};

class Camera
{
public:
	~Camera();
	Camera();

	bool Finalize();
	GLuint GetFrameBufferHandle();
	void SetColorTarget(Texture* color_target);
	void SetDepthStencilTarget(Texture* depth_target);

	//options inquire
	bool ShouldClear();

	//convenience
	Vector3 GetForward() const;
	Vector3 GetRight() const;
	Vector3 GetUp() const;
	Matrix44 GetViewProjection() const;

	// model setters
	void UpdateTransform();
	void LookAt(const Vector3& position, const Vector3& target, const Vector3& up = Vector3::up);
	void SetLocalMatrix(const Matrix44& mat); 
	void SetLocalPosition(const Vector3& pos); 
	void TranslateLocal(const Vector3& offset); 
	void SetLocalRotationEuler(const Vector3& euler); 
	void RotateLocalByEuler(const Vector3& euler); 

	// projection settings
	void SetProjection(const Matrix44& projection); 
	void SetProjectionOrtho(float width, float height, float orthoNear, float orthoFar); 
	void SetProjectionPerspective(float fov_degrees, float aspect, float nz, float fz);

	// screen spaces
	Vector2 WorldToScreen(const Vector3& worldPos); 
	Vector3 ScreenToWorld(const Vector2& pixel, float depthFromCamera);
	ray3_t ScreenToPickRay(const Vector2& pixel); 

public:
	FrameBuffer m_output;
	Transform m_transform;
	UniformBuffer* m_cameraBuffer = nullptr;
}; 
