#pragma once

class Camera;

class FlyingCamera
{
public:
	~FlyingCamera();
	FlyingCamera();

	void Update(float deltaSeconds);

public:
	Camera* m_camera = nullptr;
};
