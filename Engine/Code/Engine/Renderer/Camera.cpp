#include "Engine/Renderer/Camera.hpp"
#include "Engine/Renderer/Texture.hpp"
#include "Engine/Renderer/UniformBuffer.hpp"
#include "Engine/Debug/DebugRender.hpp"
#include "Engine/Core/Window.hpp"

Camera::~Camera()
{
	delete m_cameraBuffer;
}

Camera::Camera()
{
	camera_state_t temp;
	m_cameraBuffer = UniformBuffer::For<camera_state_t>(temp);
}

bool Camera::Finalize()
{
	return m_output.Finalize();
}

GLuint Camera::GetFrameBufferHandle()
{
	return m_output.m_handle;
}

void Camera::SetColorTarget(Texture* color_target)
{
	m_output.SetColorTarget(color_target);
}

void Camera::SetDepthStencilTarget(Texture* depth_target)
{
	m_output.SetDepthStencilTarget(depth_target);
}

bool Camera::ShouldClear()
{
	TODO("implement clear option");
	return true;
}

Vector3 Camera::GetForward() const
{
	return m_transform.GetWorldMatrix().GetForward();
}

Vector3 Camera::GetRight() const
{
	return m_transform.GetWorldMatrix().GetRight();
}

Vector3 Camera::GetUp() const
{
	return m_transform.GetWorldMatrix().GetUp();
}

Matrix44 Camera::GetViewProjection() const
{
	camera_state_t* buff = m_cameraBuffer->as<camera_state_t>();

	Matrix44 mat = buff->projection;
	mat.Append(buff->view);
	
	return mat;
}

void Camera::UpdateTransform()
{
	camera_state_t* buff = m_cameraBuffer->as<camera_state_t>();

	Matrix44 mat = m_transform.GetWorldMatrix();
	buff->view = Matrix44::MakeInverseFast(mat);
}

void Camera::LookAt(const Vector3& position, const Vector3& target, const Vector3& up)
{
	camera_state_t* buff = m_cameraBuffer->as<camera_state_t>();

	Matrix44 cameraMatrix = Matrix44::LookAt(position, target, up);
	m_transform.SetLocalMatrix(cameraMatrix);;
	buff->view = Matrix44::MakeInverseFast(cameraMatrix);
}

void Camera::SetLocalMatrix(const Matrix44& mat)
{
	camera_state_t* buff = m_cameraBuffer->as<camera_state_t>();

	m_transform.SetLocalMatrix(mat);
	buff->view = Matrix44::MakeInverseFast(mat);
}

void Camera::SetLocalPosition(const Vector3& pos)
{
	camera_state_t* buff = m_cameraBuffer->as<camera_state_t>();

	m_transform.SetLocalPosition(pos);
	buff->view = Matrix44::MakeInverseFast(m_transform.GetLocalMatrix());
}

void Camera::TranslateLocal(const Vector3& offset)
{
	camera_state_t* buff = m_cameraBuffer->as<camera_state_t>();

	m_transform.TranslateLocal(offset);
	buff->view = Matrix44::MakeInverseFast(m_transform.GetLocalMatrix());
}

void Camera::SetLocalRotationEuler(const Vector3& euler)
{
	camera_state_t* buff = m_cameraBuffer->as<camera_state_t>();

	m_transform.SetLocalRotationEuler(euler);
	buff->view = Matrix44::MakeInverseFast(m_transform.GetLocalMatrix());
}

void Camera::RotateLocalByEuler(const Vector3& euler)
{
	camera_state_t* buff = m_cameraBuffer->as<camera_state_t>();

	m_transform.RotateLocalByEuler(euler);
	buff->view = Matrix44::MakeInverseFast(m_transform.GetLocalMatrix());
}

void Camera::SetProjection(const Matrix44& projection)
{
	camera_state_t* buff = m_cameraBuffer->as<camera_state_t>();
	buff->projection = projection;
}

void Camera::SetProjectionOrtho(float width, float height, float orthoNear, float orthoFar)
{
	camera_state_t* buff = m_cameraBuffer->as<camera_state_t>();
	buff->projection = Matrix44::MakeOrtho(Vector2(-1 * width / 2, -1 * height / 2), Vector2(width / 2, height / 2), orthoNear, orthoFar);
}

void Camera::SetProjectionPerspective(float fov_degrees, float aspect, float nz, float fz)
{
	camera_state_t* buff = m_cameraBuffer->as<camera_state_t>();
	buff->projection = Matrix44::MakePerspectiveProjection(fov_degrees, aspect, nz, fz);
}

Vector2 Camera::WorldToScreen(const Vector3& worldPos)
{
	UNUSED(worldPos);
	return Vector2();
}

Vector3 Camera::ScreenToWorld(const Vector2& pixel, float depthFromCamera)
{
	float height = (float) Window::GetHeight();
	float width = (float) Window::GetWidth();

	float ndc_x = RangeMapFloat(pixel.x, 0, width, -1.f, 1.f);
	float ndc_y = RangeMapFloat(pixel.y, 0, height, 1.f, -1.f);

	Vector3 ndc = Vector3(ndc_x, ndc_y, depthFromCamera); 

	//to get view
	camera_state_t* buff = m_cameraBuffer->as<camera_state_t>();
	Matrix44 mat = buff->projection;
	mat.Append(buff->view);
	mat = Matrix44::MakeInverse(mat);

	Vector4 temp = Vector4(ndc, 1.0f);
	Vector4 homogeneousWorld = Vector4(DotProduct(temp, Vector4(mat.Ix, mat.Jx, mat.Kx, mat.Tx)),
		DotProduct(temp, Vector4(mat.Iy, mat.Jy, mat.Ky, mat.Ty)),
		DotProduct(temp, Vector4(mat.Iz, mat.Jz, mat.Kz, mat.Tz)),
		DotProduct(temp, Vector4(mat.Iw, mat.Jw, mat.Kw, mat.Tw)));

	Vector3 world_pos = homogeneousWorld.xyz() / homogeneousWorld.w;

	return world_pos;
}

ray3_t Camera::ScreenToPickRay(const Vector2& pixel)
{
	Vector3 a = ScreenToWorld(pixel, -1.f);
	Vector3 b = ScreenToWorld(pixel, 1.f);
	Vector3 dir = b - a;

	return ray3_t(a, dir.GetNormalized());
}
