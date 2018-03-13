#pragma once

namespace dx11 {

using namespace core;
using namespace DirectX;

class Camera {
public:
	virtual XMMATRIX V() = 0;
	virtual XMMATRIX P() = 0;
	virtual XMMATRIX VP() = 0;
};

class Camera2D final : public Camera {
	XMVECTOR _position;
	XMVECTOR _up;
	XMMATRIX view;
	XMMATRIX proj;
	XMMATRIX viewProj;
	float _zoomFactor = 1, minZoom = 0.01, maxZoom = 100;
	float rotationRads = 0;
	bool recalculateView = true;
	bool recalculateProj = true;
	bool recalculateViewProj = true;
public:
	uint2 windowSize = {};
	inline constexpr float2 position() const { return float2(_position); }
	inline constexpr float zoomFactor() { return 1/_zoomFactor; }

	string toString() const {
		string s =
			"[position=" + ::toString(position()) + "]";
		return s;
	}

	void init(uint2 windowSize) {
		this->windowSize = windowSize;
		this->_position  = XMVECTOR{(float)windowSize.x/2, (float)windowSize.y/2, -1,0};
		this->_up	     = XMVECTOR{0,1,0,0};
	}
	auto moveTo(float2 pos) {
		_position = {pos.x, pos.y, -1, 0};
		recalculateView = true;
		return *this;
	}
	auto moveBy(float2 pos) {
		_position += {pos.x, pos.y, 0, 0};
		recalculateView = true;
		return *this;
	}
	// TODO - can we do zoom using Z?
	/// 0.5 = zoomed out (50%), 1 = (100%) no zoom, 2 = (200%) zoomed in
	auto setZoom(float z, float minZoom, float maxZoom) {
		_zoomFactor = 1/z;
		this->minZoom = minZoom;
		this->maxZoom = maxZoom;
		recalculateProj = true;
		return *this;
	}
	auto zoomOut(float z) {
		if(_zoomFactor==maxZoom) return *this;
		_zoomFactor += z;
		if(_zoomFactor > maxZoom) {
			_zoomFactor = maxZoom;
		}
		recalculateProj = true;
		return *this;
	}
	auto zoomIn(float z) {
		if(_zoomFactor==minZoom) return *this;
		_zoomFactor -= z;
		if(_zoomFactor < minZoom) {
			_zoomFactor = minZoom;
		}
		recalculateProj = true;
		return *this;
	}
	XMMATRIX P() final override {
		if(recalculateProj) {
			float width  = windowSize.x*_zoomFactor;
			float height = windowSize.y*_zoomFactor;

			proj = DirectX::XMMatrixOrthographicLH(width, -height, 0.0f, 100.0f);

			/*proj = XMMatrixOrthographicOffCenterLH(
				-width/2, width/2,
				height/2, -height/2,
				0.0f, 100.0f
			);*/

			recalculateProj = false;
			recalculateViewProj = true;
		}
		return proj;
	}
	XMMATRIX V() final override {
		if(recalculateView) {
			/// Z points out of the screen
			XMVECTOR lookAt = DirectX::XMVectorAdd(_position, {0,0,1,0});
			view = DirectX::XMMatrixLookAtLH(
				_position,		/// camera _position in World Space
				lookAt,			/// look at the _position
				_up				/// head is up
			);
			recalculateView = false;
			recalculateViewProj = true;
		}
		return view;
	}
	XMMATRIX VP() final override {
		if(recalculateView || recalculateProj || recalculateViewProj) {
			V();
			P();
			viewProj = view * proj;
			recalculateViewProj = false;
		}
		return viewProj;
	}
};
///=================================================================================== Camera3D
class Camera3D final : public Camera {
	XMVECTOR _position = {0,1,0,0};
	XMVECTOR _up	   = {0,1,0,0};
	XMVECTOR _forward  = {0,0,0,0};
	XMVECTOR focalPoint() const { return _position+_forward*focalLength; }
	XMMATRIX view;
	XMMATRIX proj;
	XMMATRIX viewProj;
	bool recalculateView = true;
	bool recalculateProj = true;
	bool recalculateViewProj = true;
	float focalLength = 0;
	float _fov = Math::toRadians(60);	
	float _near = 0.1f;
	float _far = 100.0f;
public:
	uint2 windowSize = {};
	
	inline constexpr float3 position() const { return float3(_position); }
	inline constexpr float3 up() const { return float3(_up); }
	inline constexpr float3 forward() const { return float3(_forward); }
	inline constexpr float aspectRatio() const { return (float)windowSize.x/windowSize.y; }
	inline constexpr float fov() const { return _fov; }

	string toString() const {
		string s =
			"[position=" + ::toString(position()) +
			", up=" + ::toString(up()) +
			", forward=" + ::toString(forward()) +
			"]";
		return s;
	}

	void init(uint2 windowSize, float3 pos, float3 up, float3 focalPoint) {
		this->windowSize  = windowSize;
		this->_position   = (XMVECTOR)pos;
		this->_up         = DirectX::XMVector4Normalize((XMVECTOR)(up));
		this->_forward    = DirectX::XMVector4Normalize((XMVECTOR)(focalPoint)-(XMVECTOR)(pos));
		this->focalLength = DirectX::XMVector4Length((XMVECTOR)(focalPoint)-(XMVECTOR)(pos)).m128_f32[0];
	}
	Camera3D& fovNearFar(float fovInRadians, float nr, float fr) {
		this->_fov = fovInRadians;
		this->_near = nr;
		this->_far = fr;
		recalculateProj = true;
		return *this;
	}
	auto moveForward(float f) {
		auto dist = _forward * f;
		_position += dist;
		recalculateView = true;
		return this;
	}
	auto movePositionRelative(float3 newpos) {
		_position += (XMVECTOR)newpos;
		recalculateView = true;
		return this;
	}
	/// move focal point up/down (around x plane)
	auto pitch(float f) {
		auto right = DirectX::XMVector3Cross(_forward, _up);
		auto dist = _up * f;
		_forward = DirectX::XMVector3Normalize(_forward + dist);
		_up      = DirectX::XMVector3Cross(right, _forward);
		recalculateView = true;
		return this;
	}
	/// move focal point left/right (around y plane)
	auto yaw(float f) {
		auto right = DirectX::XMVector3Cross(_forward, _up);
		auto dist = right * f;
		_forward = DirectX::XMVector3Normalize(_forward + dist);
		recalculateView = true;
		return this;
	}
	/// tip (around z plane)
	auto roll(float f) {
		auto right = DirectX::XMVector3Cross(_forward, _up);
		auto dist = right * f;
		_up = DirectX::XMVector3Normalize(_up + dist);
		recalculateView = true;
		return this;
	}
	XMMATRIX V() final override {
		if(recalculateView) {
			view = DirectX::XMMatrixLookAtRH(
				_position,		/// camera _position in World Space
				focalPoint(),	/// position we are looking at
				_up);			/// head is up

			recalculateView = false;
			recalculateViewProj = true;
		}
		return view;
	}
	XMMATRIX invV() {
		return inverse(V());
	}
	XMMATRIX P() final override {
		if(recalculateProj) {
			proj = DirectX::XMMatrixPerspectiveFovRH(
				_fov,
				aspectRatio(),
				_near,
				_far
			);
			recalculateProj = false;
			recalculateViewProj = true;
		}
		return proj;
	}
	XMMATRIX VP() final override {
		if(recalculateView || recalculateProj || recalculateViewProj) {
			V();
			P();
			viewProj = view * proj;
			recalculateViewProj = false;
		}
		return viewProj;
	}
};

} /// dx11