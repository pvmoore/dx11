#pragma once
///
///	A 3D textured cube with lighting.
///
class Cube final {
	struct Vertex final {
		float3 pos;
		float3 normal;
		rgba color;
		float2 uv;
	}; static_assert(12*4==sizeof(Vertex));
	struct Constants final {
		XMMATRIX model;
		XMMATRIX viewProj;
		float3 lightPos;
	}; static_assert(sizeof(Constants)%16==0);

	VertexBuffer<Vertex> vertexBuffer = {};
	ConstantBuffer<Constants> constantBuffer = {};
	VertexShader vertexShader = {};
	PixelShader pixelShader = {};
	ComPtr<ID3D11InputLayout> inputLayout;
	Texture2D texture1, texture2;
	ComPtr<ID3D11SamplerState> sampler;
	bool cameraSet = false;
	bool constantsChanged = true;
	float _scale = 1;
	float3 _rotation = {0, 0, 0};
	float3 _pos = {0,0,0};
public:

	Cube& init(DX11& dx11) {
		setupPipeline(dx11);
		return *this;
	}
	Cube& camera(Camera3D cam) {
		constantBuffer.data.viewProj = cam.VP();
		cameraSet = true;
		constantsChanged = true;
		return *this;
	}
	Cube& scale(float s) {
		_scale = s;
		constantsChanged = true;
		return *this;
	}
	Cube& rotate(float3 degs) {
		_rotation = {Math::toRadians(degs.x), Math::toRadians(degs.y), Math::toRadians(degs.z)};
		constantsChanged = true;
		return *this;
	}
	Cube& move(float3 pos) {
		_pos = pos;
		constantsChanged = true;
		return *this;
	}
	void update(FrameResource& frame) {
		assert(cameraSet);
		if(constantsChanged) {
			updateConstants(frame);
		}
	}
	void render(FrameResource& frame) {
		assert(cameraSet);

		auto context = frame.context;
		context->IASetInputLayout(inputLayout.Get());
		context->VSSetShader(vertexShader, nullptr, 0);
		context->PSSetShader(pixelShader, nullptr, 0);

		uint strides = sizeof(Vertex);
		uint offsets = 0;
		context->IASetVertexBuffers(0, 1, vertexBuffer.handle.GetAddressOf(), &strides, &offsets);

		context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		context->VSSetConstantBuffers(0, 1, constantBuffer.handle.GetAddressOf());

		context->PSSetSamplers(0, 1, sampler.GetAddressOf());
		ID3D11ShaderResourceView* textures[] = {texture1.srv.Get(), texture2.srv.Get()};
		context->PSSetShaderResources(0, 2, textures);

		context->Draw(6*6, 0);
	}
private:
	void updateConstants(FrameResource& frame) {
		constantBuffer.data.lightPos = float3(1000, 1000, 1000);
		constantBuffer.data.model =
			DirectX::XMMatrixScaling(_scale, _scale, _scale) *
			DirectX::XMMatrixRotationX(_rotation.x) *
			DirectX::XMMatrixRotationY(_rotation.y) *
			DirectX::XMMatrixRotationZ(_rotation.z) *
			DirectX::XMMatrixTranslation(_pos.x, _pos.y, _pos.z);
		constantBuffer.write(frame.context);
		constantsChanged = false;
	}
	void setupPipeline(DX11& dx11) {

		const rgba c1 = DirectX::Colors::Green;
		const rgba c2 = DirectX::Colors::LightBlue;
		const rgba c3 = DirectX::Colors::Gray;
		const rgba c4 = DirectX::Colors::Aquamarine;
		const rgba c5 = DirectX::Colors::White;
		const rgba c6 = DirectX::Colors::SteelBlue;

		const Vertex vertices[] = {
			/// front
			/// y
			/// |
			/// 0 --- 1 --> x
			/// | \   |
			/// |   \ |
			/// 3 --- 2
			{{-0.5f,  0.5f, 0.5f}, {-0.577f, 0.577f, 0.577f}, c1, {0.0f, 0.0f}},	// 0
			{{ 0.5f,  0.5f, 0.5f}, { 0.577f, 0.577f, 0.577f}, c1, {1.0f, 0.0f}},	// 1
			{{ 0.5f, -0.5f, 0.5f}, { 0.577f,-0.577f, 0.577f}, c1, {1.0f, 1.0f}},	// 2

			{{-0.5f,  0.5f, 0.5f}, {-0.577f, 0.577f, 0.577f}, c1, {0.0f, 0.0f}},	// 0
			{{ 0.5f, -0.5f, 0.5f}, { 0.577f,-0.577f, 0.577f}, c1, {1.0f, 1.0f}},	// 2
			{{-0.5f, -0.5f, 0.5f}, {-0.577f,-0.577f, 0.577f}, c1, {0.0f, 1.0f}},	// 3

			// 0.707f, 0.707f, 0.0f
			// 0.577f, 0.577f, 0.577f

			/// back
			///       y
			///	      |
			/// x <-- 0 --- 1 
			///       | \   |
			///       |   \ |
			///       3 --- 2
			{{ 0.5f,  0.5f, -0.5f}, { 0.577f,  0.577f, -0.577f}, c2, {0.0f, 0.0f}},	// 0
			{{-0.5f,  0.5f, -0.5f}, {-0.577f,  0.577f, -0.577f}, c2, {1.0f, 0.0f}},	// 1
			{{-0.5f, -0.5f, -0.5f}, {-0.577f, -0.577f, -0.577f}, c2, {1.0f, 1.0f}},	// 2

			{{ 0.5f,  0.5f, -0.5f}, { 0.577f,  0.577f, -0.577f}, c2, {0.0f, 0.0f}},	// 0
			{{-0.5f, -0.5f, -0.5f}, {-0.577f, -0.577f, -0.577f}, c2, {1.0f, 1.0f}},	// 2
			{{ 0.5f, -0.5f, -0.5f}, { 0.577f, -0.577f, -0.577f}, c2, {0.0f, 1.0f}},	// 3

			/// left
			/// y
			/// |
			///	0 --- 1 --> z
			/// | \   |
			/// |   \ |
			/// 3 --- 2
			{{-0.5f,  0.5f, -0.5f}, {-0.577f,  0.577f, -0.577f}, c3, {0.0f, 0.0f}},	// 0
			{{-0.5f,  0.5f,  0.5f}, {-0.577f,  0.577f,  0.577f}, c3, {1.0f, 0.0f}},	// 1
			{{-0.5f, -0.5f,  0.5f}, {-0.577f, -0.577f,  0.577f}, c3, {1.0f, 1.0f}},	// 2

			{{-0.5f,  0.5f, -0.5f}, {-0.577f,  0.577f, -0.577f}, c3, {0.0f, 0.0f}},	// 0
			{{-0.5f, -0.5f,  0.5f}, {-0.577f, -0.577f,  0.577f}, c3, {1.0f, 1.0f}},	// 2
			{{-0.5f, -0.5f, -0.5f}, {-0.577f, -0.577f, -0.577f}, c3, {0.0f, 1.0f}},	// 3

			/// right
			///       y
			///       |
			/// z <-- 0 --- 1 
			///       | \   |
			///       |   \ |
			///       3 --- 2
			{{ 0.5f,  0.5f,  0.5f}, { 0.577f,  0.577f,  0.577f}, c4, {0.0f, 0.0f}},	// 0
			{{ 0.5f,  0.5f, -0.5f}, { 0.577f,  0.577f, -0.577f}, c4, {1.0f, 0.0f}},	// 1
			{{ 0.5f, -0.5f, -0.5f}, { 0.577f, -0.577f, -0.577f}, c4, {1.0f, 1.0f}},	// 2

			{{ 0.5f,  0.5f,  0.5f}, { 0.577f,  0.577f,  0.577f}, c4, {0.0f, 0.0f}},	// 0
			{{ 0.5f, -0.5f, -0.5f}, { 0.577f, -0.577f, -0.577f}, c4, {1.0f, 1.0f}},	// 2
			{{ 0.5f, -0.5f,  0.5f}, { 0.577f, -0.577f,  0.577f}, c4, {0.0f, 1.0f}},	// 3

			/// top
			/// 0 --- 1 --> x
			/// | \   |
			/// |   \ |
			/// 3 --- 2
			/// |
			/// z
			{{-0.5f,  0.5f, -0.5f}, {-0.577f, 0.577f, -0.577f}, c5, {0.0f, 0.0f}},	// 0
			{{ 0.5f,  0.5f, -0.5f}, { 0.577f, 0.577f, -0.577f}, c5, {1.0f, 0.0f}},	// 1
			{{ 0.5f,  0.5f,  0.5f}, { 0.577f, 0.577f,  0.577f}, c5, {1.0f, 1.0f}},	// 2

			{{-0.5f,  0.5f, -0.5f}, {-0.577f, 0.577f, -0.577f}, c5, {0.0f, 0.0f}},	// 0
			{{ 0.5f,  0.5f,  0.5f}, { 0.577f, 0.577f,  0.577f}, c5, {1.0f, 1.0f}},	// 2
			{{-0.5f,  0.5f,  0.5f}, {-0.577f, 0.577f,  0.577f}, c5, {0.0f, 1.0f}},	// 3

			/// bottom
			///	z
			/// |
			/// 0 --- 1 --> x
			/// | \   |
			/// |   \ |
			/// 3 --- 2
			{{-0.5f, -0.5f,  0.5f}, {-0.577f, -0.577f,  0.577f}, c6, {0.0f, 0.0f}},	// 0
			{{ 0.5f, -0.5f,  0.5f}, { 0.577f, -0.577f,  0.577f}, c6, {1.0f, 0.0f}},	// 1
			{{ 0.5f, -0.5f, -0.5f}, { 0.577f, -0.577f, -0.577f}, c6, {1.0f, 1.0f}},	// 2

			{{-0.5f, -0.5f,  0.5f}, {-0.577f, -0.577f,  0.577f}, c6, {0.0f, 0.0f}},	// 0
			{{ 0.5f, -0.5f, -0.5f}, { 0.577f, -0.577f, -0.577f}, c6, {1.0f, 1.0f}},	// 2
			{{-0.5f, -0.5f, -0.5f}, {-0.577f, -0.577f, -0.577f}, c6, {0.0f, 1.0f}}	// 3
		};
		vertexBuffer.initImmutable(dx11.device, _countof(vertices), vertices);

		constantBuffer.init(dx11.device);

		const auto F32x2 = DXGI_FORMAT::DXGI_FORMAT_R32G32_FLOAT;
		const auto F32x3 = DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT;
		const auto F32x4 = DXGI_FORMAT::DXGI_FORMAT_R32G32B32A32_FLOAT;

		const D3D11_INPUT_ELEMENT_DESC layout[] = {
			{"POSITION", 0, F32x3, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
			{"NORMAL",   0, F32x3, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
			{"COLOR",    0, F32x4, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
			{"TEXCOORD", 0, F32x2, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0}
		};

		vertexShader = dx11.shaders.getVS(L"../Resources/shaders/cube.hlsl");
		pixelShader  = dx11.shaders.getPS(L"../Resources/shaders/cube.hlsl");

		throwOnDXError(dx11.device->CreateInputLayout(
			layout, 4,
			vertexShader.blob->GetBufferPointer(),
			vertexShader.blob->GetBufferSize(),
			inputLayout.GetAddressOf()));

		D3D11_SAMPLER_DESC samplerDesc;
		samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
		samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
		samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
		samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
		samplerDesc.MipLODBias = 0.0f;
		samplerDesc.MaxAnisotropy = 1;
		samplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
		samplerDesc.MinLOD = -FLT_MAX;
		samplerDesc.MaxLOD = FLT_MAX;

		/// Create and set our sampler and never change it
		throwOnDXError(dx11.device->CreateSamplerState(&samplerDesc, sampler.GetAddressOf()));
		dx11.context->PSSetSamplers(0, 1, sampler.GetAddressOf());

		texture1 = dx11.textures.load(L"/pvmoore/_assets/images/dds/rock5.dds");
		texture2 = dx11.textures.load(L"/pvmoore/_assets/images/dds/seamless/hessian2.dds");
	}
};
