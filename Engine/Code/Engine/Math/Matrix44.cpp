#include "Engine/Math/Matrix44.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Debug/DebugRender.hpp"
#include <math.h> 

const Matrix44 Matrix44::identity = Matrix44();

Matrix44::Matrix44(const Matrix44 & copyFrom)
{
	Ix = copyFrom.Ix;
	Iy = copyFrom.Iy;
	Iz = copyFrom.Iz;
	Iw = copyFrom.Iw;
	Jx = copyFrom.Jx;
	Jy = copyFrom.Jy;
	Jz = copyFrom.Jz;
	Jw = copyFrom.Jw;
	Kx = copyFrom.Kx;
	Ky = copyFrom.Ky;
	Kz = copyFrom.Kz;
	Kw = copyFrom.Kw;
	Tx = copyFrom.Tx;
	Ty = copyFrom.Ty;
	Tz = copyFrom.Tz;
	Tw = copyFrom.Tw;
}

Matrix44::Matrix44(const float* sixteenValuesBasisMajor)
{
	Ix = *sixteenValuesBasisMajor; sixteenValuesBasisMajor++;
	Iy = *sixteenValuesBasisMajor; sixteenValuesBasisMajor++;
	Iz = *sixteenValuesBasisMajor; sixteenValuesBasisMajor++;
	Iw = *sixteenValuesBasisMajor; sixteenValuesBasisMajor++;
	Jx = *sixteenValuesBasisMajor; sixteenValuesBasisMajor++;
	Jy = *sixteenValuesBasisMajor; sixteenValuesBasisMajor++;
	Jz = *sixteenValuesBasisMajor; sixteenValuesBasisMajor++;
	Jw = *sixteenValuesBasisMajor; sixteenValuesBasisMajor++;
	Kx = *sixteenValuesBasisMajor; sixteenValuesBasisMajor++;
	Ky = *sixteenValuesBasisMajor; sixteenValuesBasisMajor++;
	Kz = *sixteenValuesBasisMajor; sixteenValuesBasisMajor++;
	Kw = *sixteenValuesBasisMajor; sixteenValuesBasisMajor++;
	Tx = *sixteenValuesBasisMajor; sixteenValuesBasisMajor++;
	Ty = *sixteenValuesBasisMajor; sixteenValuesBasisMajor++;
	Tz = *sixteenValuesBasisMajor; sixteenValuesBasisMajor++;
	Tw = *sixteenValuesBasisMajor;
}

Matrix44::Matrix44(const Vector2& iBasis, const Vector2& jBasis, const Vector2& translation)
{
	Ix = iBasis.x;
	Iy = iBasis.y;
	Iz = 0;
	Iw = 0;

	Jx = jBasis.x;
	Jy = jBasis.y;
	Jz = 0;
	Jw = 0;

	Kx = 0;
	Ky = 0;
	Kz = 1;
	Kw = 0;

	Tx = translation.x;
	Ty = translation.y;
	Tz = 0;
	Tw = 1;
}

Matrix44::Matrix44(const Vector3& iBasis, const Vector3& jBasis, const Vector3& kBasis, const Vector3& translation)
{
	Ix = iBasis.x;
	Iy = iBasis.y;
	Iz = iBasis.z;
	Iw = 0;

	Jx = jBasis.x;
	Jy = jBasis.y;
	Jz = jBasis.z;
	Jw = 0;

	Kx = kBasis.x;
	Ky = kBasis.y;
	Kz = kBasis.z;
	Kw = 0;

	Tx = translation.x;
	Ty = translation.y;
	Tz = translation.z;
	Tw = 1;
}

Vector2 Matrix44::TransformPosition2D(const Vector2 & position2D)
{
	return Vector2((position2D.x * Ix) + (position2D.y * Jx) + Tx, (position2D.x * Iy) + (position2D.y * Jy) + Ty);
}

Vector2 Matrix44::TransformDisplacement2D(const Vector2 & displacement2D)
{
	return Vector2((displacement2D.x * Ix) + (displacement2D.y * Jx), (displacement2D.x * Iy) + (displacement2D.y * Jy));
}

void Matrix44::SetIdentity()
{
	Ix = 1;
	Iy = 0;
	Iz = 0;
	Iw = 0;

	Jx = 0;
	Jy = 1;
	Jz = 0;
	Jw = 0;

	Kx = 0;
	Ky = 0;
	Kz = 1;
	Kw = 0;

	Tx = 0;
	Ty = 0;
	Tz = 0;
	Tw = 1;
}

void Matrix44::SetValues(const float * sixteenValuesBasisMajor)
{
	Ix = *sixteenValuesBasisMajor; sixteenValuesBasisMajor++;
	Iy = *sixteenValuesBasisMajor; sixteenValuesBasisMajor++;
	Iz = *sixteenValuesBasisMajor; sixteenValuesBasisMajor++;
	Iw = *sixteenValuesBasisMajor; sixteenValuesBasisMajor++;
	Jx = *sixteenValuesBasisMajor; sixteenValuesBasisMajor++;
	Jy = *sixteenValuesBasisMajor; sixteenValuesBasisMajor++;
	Jz = *sixteenValuesBasisMajor; sixteenValuesBasisMajor++;
	Jw = *sixteenValuesBasisMajor; sixteenValuesBasisMajor++;
	Kx = *sixteenValuesBasisMajor; sixteenValuesBasisMajor++;
	Ky = *sixteenValuesBasisMajor; sixteenValuesBasisMajor++;
	Kz = *sixteenValuesBasisMajor; sixteenValuesBasisMajor++;
	Kw = *sixteenValuesBasisMajor; sixteenValuesBasisMajor++;
	Tx = *sixteenValuesBasisMajor; sixteenValuesBasisMajor++;
	Ty = *sixteenValuesBasisMajor; sixteenValuesBasisMajor++;
	Tz = *sixteenValuesBasisMajor; sixteenValuesBasisMajor++;
	Tw = *sixteenValuesBasisMajor;
}

void Matrix44::Append(const Matrix44 & matrixToAppend)
{	
	float oldIx = Ix;
	float oldIy = Iy;
	float oldIz = Iz;
	float oldIw = Iw;
	float oldJx = Jx;
	float oldJy = Jy;
	float oldJz = Jz;
	float oldJw = Jw;
	float oldKx = Kx;
	float oldKy = Ky;
	float oldKz = Kz;
	float oldKw = Kw;
	float oldTx = Tx;
	float oldTy = Ty;
	float oldTz = Tz;
	float oldTw = Tw;

	Ix = (oldIx * matrixToAppend.Ix) + (oldJx * matrixToAppend.Iy) + (oldKx * matrixToAppend.Iz) + (oldTx * matrixToAppend.Iw);
	Iy = (oldIy * matrixToAppend.Ix) + (oldJy * matrixToAppend.Iy) + (oldKy * matrixToAppend.Iz) + (oldTy * matrixToAppend.Iw);
	Iz = (oldIz * matrixToAppend.Ix) + (oldJz * matrixToAppend.Iy) + (oldKz * matrixToAppend.Iz) + (oldTz * matrixToAppend.Iw);
	Iw = (oldIw * matrixToAppend.Ix) + (oldJw * matrixToAppend.Iy) + (oldKw * matrixToAppend.Iz) + (oldTw * matrixToAppend.Iw);
	Jx = (oldIx * matrixToAppend.Jx) + (oldJx * matrixToAppend.Jy) + (oldKx * matrixToAppend.Jz) + (oldTx * matrixToAppend.Jw);
	Jy = (oldIy * matrixToAppend.Jx) + (oldJy * matrixToAppend.Jy) + (oldKy * matrixToAppend.Jz) + (oldTy * matrixToAppend.Jw);
	Jz = (oldIz * matrixToAppend.Jx) + (oldJz * matrixToAppend.Jy) + (oldKz * matrixToAppend.Jz) + (oldTz * matrixToAppend.Jw);
	Jw = (oldIw * matrixToAppend.Jx) + (oldJw * matrixToAppend.Jy) + (oldKw * matrixToAppend.Jz) + (oldTw * matrixToAppend.Jw);
	Kx = (oldIx * matrixToAppend.Kx) + (oldJx * matrixToAppend.Ky) + (oldKx * matrixToAppend.Kz) + (oldTx * matrixToAppend.Kw);
	Ky = (oldIy * matrixToAppend.Kx) + (oldJy * matrixToAppend.Ky) + (oldKy * matrixToAppend.Kz) + (oldTy * matrixToAppend.Kw);
	Kz = (oldIz * matrixToAppend.Kx) + (oldJz * matrixToAppend.Ky) + (oldKz * matrixToAppend.Kz) + (oldTz * matrixToAppend.Kw);
	Kw = (oldIw * matrixToAppend.Kx) + (oldJw * matrixToAppend.Ky) + (oldKw * matrixToAppend.Kz) + (oldTw * matrixToAppend.Kw);
	Tx = (oldIx * matrixToAppend.Tx) + (oldJx * matrixToAppend.Ty) + (oldKx * matrixToAppend.Tz) + (oldTx * matrixToAppend.Tw);
	Ty = (oldIy * matrixToAppend.Tx) + (oldJy * matrixToAppend.Ty) + (oldKy * matrixToAppend.Tz) + (oldTy * matrixToAppend.Tw);
	Tz = (oldIz * matrixToAppend.Tx) + (oldJz * matrixToAppend.Ty) + (oldKz * matrixToAppend.Tz) + (oldTz * matrixToAppend.Tw);
	Tw = (oldIw * matrixToAppend.Tx) + (oldJw * matrixToAppend.Ty) + (oldKw * matrixToAppend.Tz) + (oldTw * matrixToAppend.Tw);
}

void Matrix44::RotateDegrees2D(float rotationDegreesAboutZ)
{
	Append(MakeRotationDegrees2D(rotationDegreesAboutZ));
}

void Matrix44::Translate2D(const Vector2 & translation)
{
	Append(MakeTranslation2D(translation));
}

void Matrix44::Translate(const Vector3& translation)
{
	Append(MakeTranslation(translation));
}

void Matrix44::SetTranslation(const Vector3& translation)
{
	Tx = translation.x;
	Ty = translation.y;
	Tz = translation.z;
	Tw = 1.f;
}

void Matrix44::ScaleUniform2D(float scaleXY)
{
	Append(MakeScaleUniform2D(scaleXY));
}

void Matrix44::Scale2D(float scaleX, float scaleY)
{
	Append(MakeScale2D(scaleX, scaleY));
}

Vector3 Matrix44::GetPosition() const
{
	return Vector3(Tx, Ty, Tz);
}

Vector3 Matrix44::GetForward() const
{
	return Vector3(Kx, Ky, Kz);
}

Vector3 Matrix44::GetRight() const
{
	return Vector3(Ix, Iy, Iz);
}

Vector3 Matrix44::GetUp() const
{
	return Vector3(Jx, Jy, Jz);
}

Vector3 Matrix44::GetEuler() const
{
	//Might have to normalize matrix
	float xRad;
	float yRad;
	float zRad;
 
	float sx = Ky;
	sx = ClampFloat(sx, -1.0f, 1.0f);
	xRad = asinf(-sx);

	float cx = cosf(xRad);

	if (!IsMostlyEqual(cx, 0.0f))
	{
		yRad = atan2f(Kx, Kz);
		zRad = atan2f(Iy, Jy);
	}
	else
	{
		//gimble lock case?
		zRad = 0.f;
		yRad = atan2f(-Kx, Ix);
	}

	return Vector3(xRad, yRad, zRad);
}

Vector3 Matrix44::GetScale() const
{
	return Vector3(Vector3(Ix, Iy, Iz).GetLength(), Vector3(Jx, Jy, Jz).GetLength(), Vector3(Kx, Ky, Kz).GetLength());
}

Matrix44 Matrix44::MakeRotationDegrees2D(float rotationDegreesAboutZ)
{
	return Matrix44(Vector2(CosDegrees(rotationDegreesAboutZ), SinDegrees(rotationDegreesAboutZ)),
		Vector2(-1 * SinDegrees(rotationDegreesAboutZ), CosDegrees(rotationDegreesAboutZ)));
}

Matrix44 Matrix44::MakeYawRotationDegrees(float rotationDegreesAboutY)
{
	return Matrix44(Vector3(CosDegrees(rotationDegreesAboutY), 0, SinDegrees(rotationDegreesAboutY)),
		Vector3(0, 1, 0),
		Vector3(-SinDegrees(rotationDegreesAboutY), 0, CosDegrees(rotationDegreesAboutY)));
}

Matrix44 Matrix44::MakePitchRotationDegrees(float rotationDegreesAboutX)
{
	return Matrix44(Vector3(1, 0, 0),
		Vector3(0, CosDegrees(rotationDegreesAboutX), SinDegrees(rotationDegreesAboutX)),
		Vector3(0, -SinDegrees(rotationDegreesAboutX), CosDegrees(rotationDegreesAboutX)));
}

Matrix44 Matrix44::MakeFromEuler(const Vector3& eulerRotation)
{
	float cx = cosf(eulerRotation.x);
	float sx = sinf(eulerRotation.x);
	float cy = cosf(eulerRotation.y);
	float sy = sinf(eulerRotation.y);
	float cz = cosf(eulerRotation.z);
	float sz = sinf(eulerRotation.z);

	float output[16] = {
		 cz*cy + sz*sx*sy,	sz*cx,				-cz*sy + sz*sx*cy,		0,
		-sz*cy + cz*sx*sy,	cz*cx,				 sz*sy + cz*sx*cy,		0,
		 cx*sy,				-sx,				 cx*cy,					0,
			0,				0,					 0,						1 };

	return Matrix44(output);
}

Matrix44 Matrix44::MakeTranslation2D(const Vector2& translation)
{
	return Matrix44(Vector2(1, 0), Vector2(0, 1), translation);
}

Matrix44 Matrix44::MakeTranslation(const Vector3& translation)
{
	return Matrix44(Vector3(1, 0, 0), Vector3(0, 1, 0), Vector3(0, 0, 1), translation);
}

Matrix44 Matrix44::MakeScaleUniform2D(float scaleXY)
{
	return Matrix44(Vector2(scaleXY, 0), Vector2(0, scaleXY));
}

Matrix44 Matrix44::MakeScale2D(float scaleX, float scaleY)
{
	return Matrix44(Vector2(scaleX, 0), Vector2(0, scaleY));
}

Matrix44 Matrix44::MakeScale(float scaleX, float scaleY, float scaleZ)
{
	return Matrix44(Vector3(scaleX, 0, 0), Vector3(0, scaleY, 0), Vector3(0, 0, scaleZ));
}

Matrix44 Matrix44::MakeOrtho2D(const Vector2 & bottomLeft, const Vector2 & topRight)
{
	float Ix = 2 / (topRight.x - bottomLeft.x);
	float Jy = 2 / (topRight.y - bottomLeft.y);
	float Tx = -1 * (topRight.x + bottomLeft.x) / (topRight.x - bottomLeft.x);
	float Ty = -1 * (topRight.y + bottomLeft.y) / (topRight.y - bottomLeft.y);

	float output[16] =
	{ Ix,	0,	0,	0,
	  0,	Jy,	0,	0,
	  0,	0,	-2,	0,
	  Tx,	Ty,	-1,	1 };

	return Matrix44(output);
}

Matrix44 Matrix44::MakeOrtho(const Vector2 & bottomLeft, const Vector2 & topRight, float orthoNear, float orthoFar)
{
	float Ix = 2 / (topRight.x - bottomLeft.x);
	float Jy = 2 / (topRight.y - bottomLeft.y);
	float Kz = 2 / (orthoFar - orthoNear);
	float Tx = -1 * (topRight.x + bottomLeft.x) / (topRight.x - bottomLeft.x);
	float Ty = -1 * (topRight.y + bottomLeft.y) / (topRight.y - bottomLeft.y);
	float Tz = -1 * (orthoFar + orthoNear) / (orthoFar - orthoNear);

	float output[16] =
	  {	Ix,	0,	0,	0,
		0,	Jy,	0,	0,
		0,	0,	Kz,	0,
		Tx,	Ty,	Tz,	1 };

	return Matrix44(output);
}

Matrix44 Matrix44::MakePerspectiveProjection(float fov_degrees, float aspect, float nz, float fz)
{
	float d = 1.0f / tanf(fov_degrees);
	float q = 1.0f / (fz - nz); 

	float Ix = d / aspect;
	float Jy = d;
	float Kz = (nz + fz) * q;
	float Tz = -2.0f * nz * fz * q;

	float output[16] =
	{ Ix,	0,	0,	0,
		0,	Jy,	0,	0,
		0,	0,	Kz,	1,
		0,	0,	Tz,	0 };

	return Matrix44(output);
}

Matrix44 Matrix44::LookAt(const Vector3& position, const Vector3& target, const Vector3& up)
{
	Vector3 direction = target - position;
	Vector3 forward = direction.GetNormalized();
	Vector3 right = CrossProduct(up, forward);
	right.NormalizeAndGetLength();
	Vector3 upResult = CrossProduct(forward, right);
	Matrix44 lookAt = Matrix44(right, upResult, forward, position);

	return lookAt;
}

Matrix44 Matrix44::MakeInverseFast(const Matrix44 & mat)
{
	float transposeData[16] =
	{	mat.Ix,	mat.Jx,	mat.Kx, 0,
		mat.Iy,	mat.Jy,	mat.Ky, 0,
		mat.Iz,	mat.Jz,	mat.Kz, 0,
		0, 0, 0, 1 };

	Matrix44 rotationalTranspose = Matrix44(transposeData);
	Matrix44 translation = MakeTranslation(Vector3(-mat.Tx, -mat.Ty, -mat.Tz));

	rotationalTranspose.Append(translation);
	return rotationalTranspose;
}

Matrix44 Matrix44::MakeInverse(const Matrix44& mat)
{
	float inv[16];
	float det;

	inv[0] = mat.Jy * mat.Kz * mat.Tw - 
		mat.Jy  * mat.Kw * mat.Tz - 
		mat.Ky  * mat.Jz  * mat.Tw + 
		mat.Ky  * mat.Jw  * mat.Tz +
		mat.Ty * mat.Jz  * mat.Kw - 
		mat.Ty * mat.Jw  * mat.Kz;

	inv[4] = -mat.Jx  * mat.Kz * mat.Tw + 
		mat.Jx  * mat.Kw * mat.Tz + 
		mat.Kx  * mat.Jz  * mat.Tw - 
		mat.Kx  * mat.Jw  * mat.Tz - 
		mat.Tx * mat.Jz  * mat.Kw + 
		mat.Tx * mat.Jw  * mat.Kz;

	inv[8] = mat.Jx  * mat.Ky * mat.Tw - 
		mat.Jx  * mat.Kw * mat.Ty - 
		mat.Kx  * mat.Jy * mat.Tw + 
		mat.Kx  * mat.Jw * mat.Ty + 
		mat.Tx * mat.Jy * mat.Kw - 
		mat.Tx * mat.Jw * mat.Ky;

	inv[12] = -mat.Jx  * mat.Ky * mat.Tz + 
		mat.Jx  * mat.Kz * mat.Ty +
		mat.Kx  * mat.Jy * mat.Tz - 
		mat.Kx  * mat.Jz * mat.Ty - 
		mat.Tx * mat.Jy * mat.Kz + 
		mat.Tx * mat.Jz * mat.Ky;

	inv[1] = -mat.Iy  * mat.Kz * mat.Tw + 
		mat.Iy  * mat.Kw * mat.Tz + 
		mat.Ky  * mat.Iz * mat.Tw - 
		mat.Ky  * mat.Iw * mat.Tz - 
		mat.Ty * mat.Iz * mat.Kw + 
		mat.Ty * mat.Iw * mat.Kz;

	inv[5] = mat.Ix  * mat.Kz * mat.Tw - 
		mat.Ix  * mat.Kw * mat.Tz - 
		mat.Kx  * mat.Iz * mat.Tw + 
		mat.Kx  * mat.Iw * mat.Tz + 
		mat.Tx * mat.Iz * mat.Kw - 
		mat.Tx * mat.Iw * mat.Kz;

	inv[9] = -mat.Ix  * mat.Ky * mat.Tw + 
		mat.Ix  * mat.Kw * mat.Ty + 
		mat.Kx  * mat.Iy * mat.Tw - 
		mat.Kx  * mat.Iw * mat.Ty - 
		mat.Tx * mat.Iy * mat.Kw + 
		mat.Tx * mat.Iw * mat.Ky;

	inv[13] = mat.Ix  * mat.Ky * mat.Tz - 
		mat.Ix  * mat.Kz * mat.Ty - 
		mat.Kx  * mat.Iy * mat.Tz + 
		mat.Kx  * mat.Iz * mat.Ty + 
		mat.Tx * mat.Iy * mat.Kz - 
		mat.Tx * mat.Iz * mat.Ky;

	inv[2] = mat.Iy  * mat.Jz * mat.Tw - 
		mat.Iy  * mat.Jw * mat.Tz - 
		mat.Jy  * mat.Iz * mat.Tw + 
		mat.Jy  * mat.Iw * mat.Tz + 
		mat.Ty * mat.Iz * mat.Jw - 
		mat.Ty * mat.Iw * mat.Jz;

	inv[6] = -mat.Ix  * mat.Jz * mat.Tw + 
		mat.Ix  * mat.Jw * mat.Tz + 
		mat.Jx  * mat.Iz * mat.Tw - 
		mat.Jx  * mat.Iw * mat.Tz - 
		mat.Tx * mat.Iz * mat.Jw + 
		mat.Tx * mat.Iw * mat.Jz;

	inv[10] = mat.Ix  * mat.Jy * mat.Tw - 
		mat.Ix  * mat.Jw * mat.Ty - 
		mat.Jx  * mat.Iy * mat.Tw + 
		mat.Jx  * mat.Iw * mat.Ty + 
		mat.Tx * mat.Iy * mat.Jw - 
		mat.Tx * mat.Iw * mat.Jy;

	inv[14] = -mat.Ix  * mat.Jy * mat.Tz + 
		mat.Ix  * mat.Jz * mat.Ty + 
		mat.Jx  * mat.Iy * mat.Tz - 
		mat.Jx  * mat.Iz * mat.Ty - 
		mat.Tx * mat.Iy * mat.Jz + 
		mat.Tx * mat.Iz * mat.Jy;

	inv[3] = -mat.Iy * mat.Jz * mat.Kw + 
		mat.Iy * mat.Jw * mat.Kz + 
		mat.Jy * mat.Iz * mat.Kw - 
		mat.Jy * mat.Iw * mat.Kz - 
		mat.Ky * mat.Iz * mat.Jw + 
		mat.Ky * mat.Iw * mat.Jz;

	inv[7] = mat.Ix * mat.Jz * mat.Kw - 
		mat.Ix * mat.Jw * mat.Kz - 
		mat.Jx * mat.Iz * mat.Kw + 
		mat.Jx * mat.Iw * mat.Kz + 
		mat.Kx * mat.Iz * mat.Jw - 
		mat.Kx * mat.Iw * mat.Jz;

	inv[11] = -mat.Ix * mat.Jy * mat.Kw + 
		mat.Ix * mat.Jw * mat.Ky + 
		mat.Jx * mat.Iy * mat.Kw - 
		mat.Jx * mat.Iw * mat.Ky - 
		mat.Kx * mat.Iy * mat.Jw + 
		mat.Kx * mat.Iw * mat.Jy;

	inv[15] = mat.Ix * mat.Jy * mat.Kz - 
		mat.Ix * mat.Jz * mat.Ky - 
		mat.Jx * mat.Iy * mat.Kz + 
		mat.Jx * mat.Iz * mat.Ky + 
		mat.Kx * mat.Iy * mat.Jz - 
		mat.Kx * mat.Iz * mat.Jy;

	det = mat.Ix * inv[0] + mat.Iy * inv[4] + mat.Iz * inv[8] + mat.Iw * inv[12];

	if (det == 0)
		return Matrix44();

	det = 1.f / det;

	for (int i = 0; i < 16; i++)
	{
		inv[i] = inv[i] * det;
	}

	return Matrix44(inv);
}

Matrix44 Matrix44::TurnToward(const Matrix44& current, const Matrix44& target, float maxTurnRadians)
{
	Matrix44 ci = Matrix44::MakeInverse(current); 
	Matrix44 r = target;

	r.Append(ci);

	// trace is 1 + 2 cos(theta) == sum of diagonal
	float trace = r.GetTrace3(); 

	float inner = (trace - 1.0f) * .5f; 
	trace = ClampFloat(inner, -1.0f, 1.0f); 
	float theta = acos(trace - 1.0f); 

	float t = MinFloat(maxTurnRadians / theta, 1.0f);
	
	TODO("find out why theta is sometimes nan");
	if (isnan(theta) == 1)
	{
// 		DebugLogf("trace: " + std::to_string(trace), Rgba::red, 10);
// 		DebugLogf("theta: " + std::to_string(theta), Rgba::red, 10);
// 		DebugLogf("t: " + std::to_string(t), Rgba::red, 10);

		t = 0.1f;
	}

// 	DebugLogf("trace: " + std::to_string(trace), Rgba::white, 0);
// 	DebugLogf("theta: " + std::to_string(theta), Rgba::white, 0);
// 	DebugLogf("t: " + std::to_string(t), Rgba::white, 0);

	Matrix44 ret = LerpTransform(current, target, t); 
	ret.SetTranslation(current.GetPosition()); 

	return ret; 
}

float Matrix44::GetTrace3() const
{
	float sum = 0.0f; 

	sum += Ix;
	sum += Jy;
	sum += Kz;

	return sum; 
}

const Matrix44 LerpTransform(const Matrix44& a, const Matrix44& b, float t)
{
	Vector3 a_right = a.GetRight();
	Vector3 b_right = b.GetRight(); 
	Vector3 a_up = a.GetUp();
	Vector3 b_up = b.GetUp(); 
	Vector3 a_forward = a.GetForward(); 
	Vector3 b_forward = b.GetForward();
	Vector3 a_translation = a.GetPosition();
	Vector3 b_translation = b.GetPosition(); 

	Vector3 right = Slerp( a_right, b_right, t ); 
	Vector3 up = Slerp( a_up, b_up, t ); 
	Vector3 forward = Slerp( a_forward, b_forward, t ); 
	Vector3 translation = Interpolate( a_translation, b_translation, t ); 

	return Matrix44( right, up, forward, translation ); 
}
