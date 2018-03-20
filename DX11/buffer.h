#pragma once

namespace dx11 {

class Buffer {
protected:
	uint _size = 0;
	uint _cpuAccess = 0;
	uint _bindFlags = 0;
	uint _usage = D3D11_USAGE_DEFAULT;
	uint _misc = 0;
	uint _stride = 0;
	bool isInitialised = false;
public:
	ComPtr<ID3D11Buffer> handle;

	void write(ComPtr<ID3D11DeviceContext> context, const void* data, uint offset = 0, uint length = 0) {
		assert(data && "data is null");
		assert(isInitialised);
		if(length==0) length = _size;
		if(_usage==D3D11_USAGE::D3D11_USAGE_DYNAMIC) {
			D3D11_MAPPED_SUBRESOURCE mappedResource{};
			throwOnDXError(context->Map(handle.Get(), 0, D3D11_MAP::D3D11_MAP_WRITE_DISCARD, 0, &mappedResource));
			memcpy((ubyte*)mappedResource.pData+offset, data, length);
			context->Unmap(handle.Get(), 0);
		} else if(_usage==D3D11_USAGE::D3D11_USAGE_DEFAULT) {
			if(length==_size) {
				context->UpdateSubresource(handle.Get(), 0, nullptr, data, 0, 0);
			} else {
				//UINT left;
				//UINT top;
				//UINT front;
				//UINT right;
				//UINT bottom;
				//UINT back;
				D3D11_BOX box = {offset, 0, 0, offset+length, 1, 1};
				context->UpdateSubresource(handle.Get(), 0, &box, data, 0, 0);
			}
		} else assert(false);
	}
protected:
	void init(ComPtr<ID3D11Device> device, uint size, const void* initialData) {
		_size = size;
		assert(_size>0);

		D3D11_BUFFER_DESC bufferDesc = {};
		bufferDesc.ByteWidth = _size;
		bufferDesc.Usage = (D3D11_USAGE)_usage;
		bufferDesc.BindFlags = _bindFlags;
		bufferDesc.CPUAccessFlags = _cpuAccess;
		bufferDesc.MiscFlags = _misc;
		bufferDesc.StructureByteStride = _stride;

		if(initialData) {
			D3D11_SUBRESOURCE_DATA data = {initialData};
			device->CreateBuffer(&bufferDesc, &data, handle.GetAddressOf());
		} else {
			device->CreateBuffer(&bufferDesc, nullptr, handle.GetAddressOf());
		}
		isInitialised = true;
	}
};
//=========================================================================== VertexBuffer
template<class T>
class VertexBuffer final : public Buffer {
public:
	VertexBuffer() {
		_bindFlags = D3D11_BIND_FLAG::D3D11_BIND_VERTEX_BUFFER;
	}
	void initDynamic(ComPtr<ID3D11Device> device, uint numVertices, const T* initialData=nullptr) {
		_usage = D3D11_USAGE::D3D11_USAGE_DEFAULT;
		Buffer::init(device, numVertices * sizeof(T), initialData);
	}
	void initImmutable(ComPtr<ID3D11Device> device, uint numVertices, const T* initialData)  {
		assert(initialData);
		_usage = D3D11_USAGE::D3D11_USAGE_IMMUTABLE;
		Buffer::init(device, numVertices*sizeof(T), initialData);
	}
};
//=========================================================================== IndexBuffer
template<typename T>
class IndexBuffer final : public Buffer {
public:
	IndexBuffer() {
		_bindFlags = D3D11_BIND_FLAG::D3D11_BIND_INDEX_BUFFER;
	}
	void initDynamic(ComPtr<ID3D11Device> device, uint numIndices, const T* initialData=nullptr) {
		_usage = D3D11_USAGE::D3D11_USAGE_DEFAULT;
		Buffer::init(device, numIndices * sizeof(T), initialData);
	}
	void initImmutable(ComPtr<ID3D11Device> device, uint numIndices, const T* initialData) {
		assert(initialData);
		_usage = D3D11_USAGE::D3D11_USAGE_IMMUTABLE;
		Buffer::init(device, numIndices * sizeof(T), initialData);
	}
};
//=========================================================================== StagingReadBuffer
template<typename T>
class StagingReadBuffer final : public Buffer {
public:
	StagingReadBuffer() {
		_bindFlags = 0;
		_usage     = D3D11_USAGE::D3D11_USAGE_STAGING;
		_cpuAccess = D3D11_CPU_ACCESS_FLAG::D3D11_CPU_ACCESS_READ;
	}
	void init(ComPtr<ID3D11Device> device, uint numElements) {
		Buffer::init(device, numElements * sizeof(T), nullptr);
	}
	/// Use context->CopyResource(stagingBuffer, deviceBuffer) to fill staging buffer
	void read(ComPtr<ID3D11DeviceContext> context, T* dest) {
		assert(isInitialised);
		assert(dest);
		D3D11_MAPPED_SUBRESOURCE mappedResource = {};
		throwOnDXError(context->Map(handle.Get(), 0, D3D11_MAP::D3D11_MAP_READ, 0, &mappedResource));
		memcpy(dest, (ubyte*)mappedResource.pData, _size);
		context->Unmap(handle.Get(), 0);
	}
};
//=========================================================================== ConstantBuffer
template<class T>
class ConstantBuffer final : public Buffer {
public:
	T data;

	ConstantBuffer() {
		_bindFlags = D3D11_BIND_FLAG::D3D11_BIND_CONSTANT_BUFFER;
		_usage	   = D3D11_USAGE::D3D11_USAGE_DYNAMIC;
		_cpuAccess = D3D11_CPU_ACCESS_FLAG::D3D11_CPU_ACCESS_WRITE;

		assert(sizeof(T)%16==0 && "Constant buffers must be a multiple of 16 bytes");
	}
	void init(ComPtr<ID3D11Device> device) {
		Buffer::init(device, sizeof(T), &data);
	}
	void write(ComPtr<ID3D11DeviceContext> context) {
		assert(isInitialised);
		Buffer::write(context, &data, 0, sizeof(T));
	}
};
//=========================================================================== StructuredBuffer
template<class ELE>
class StructuredBuffer final : public Buffer {
public:
	ComPtr<ID3D11ShaderResourceView> view;

	StructuredBuffer() {
		_bindFlags = D3D11_BIND_FLAG::D3D11_BIND_SHADER_RESOURCE |
				     D3D11_BIND_FLAG::D3D11_BIND_UNORDERED_ACCESS;
		_usage = D3D11_USAGE::D3D11_USAGE_DEFAULT;
		_misc = D3D11_RESOURCE_MISC_FLAG::D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
		_stride = sizeof(ELE);
	}
	void init(ComPtr<ID3D11Device> device, uint numElements) {
		Buffer::init(device, numElements * sizeof(ELE), nullptr);

		D3D11_SHADER_RESOURCE_VIEW_DESC desc = {};
		desc.ViewDimension = D3D11_SRV_DIMENSION_BUFFEREX;
		desc.BufferEx.FirstElement = 0;
		desc.Format = DXGI_FORMAT_UNKNOWN;
		desc.BufferEx.NumElements = numElements;

		throwOnDXError(device->CreateShaderResourceView(handle.Get(), &desc, view.GetAddressOf()));
	}
};
//=========================================================================== RWStructuredBuffer
template<class ELE>
class RWStructuredBuffer final : public Buffer {
public:
	ComPtr<ID3D11UnorderedAccessView> uav;

	RWStructuredBuffer() {
		_bindFlags = D3D11_BIND_FLAG::D3D11_BIND_SHADER_RESOURCE |
					 D3D11_BIND_FLAG::D3D11_BIND_UNORDERED_ACCESS;
		_usage = D3D11_USAGE::D3D11_USAGE_DEFAULT;
		_misc = D3D11_RESOURCE_MISC_FLAG::D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
		_stride = sizeof(ELE);
	}
	void init(ComPtr<ID3D11Device> device, uint numElements) {
		Buffer::init(device, numElements * sizeof(ELE), nullptr);

		D3D11_UNORDERED_ACCESS_VIEW_DESC desc = {};
		desc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
		desc.Buffer.FirstElement = 0;
		desc.Format = DXGI_FORMAT_UNKNOWN;      
		desc.Buffer.NumElements = numElements;

		throwOnDXError(device->CreateUnorderedAccessView(handle.Get(), &desc, uav.GetAddressOf()));
	}
};
//=========================================================================== ByteAddressBuffer
class ByteAddressBuffer final : public Buffer {
public:
	ComPtr<ID3D11ShaderResourceView> view;

	ByteAddressBuffer() {
		_bindFlags = D3D11_BIND_FLAG::D3D11_BIND_SHADER_RESOURCE;
		_usage = D3D11_USAGE::D3D11_USAGE_DEFAULT;
		_misc = D3D11_RESOURCE_MISC_FLAG::D3D11_RESOURCE_MISC_BUFFER_ALLOW_RAW_VIEWS;
	}
	void init(ComPtr<ID3D11Device> device, uint numUnits) {
		Buffer::init(device, numUnits*4, nullptr);

		D3D11_SHADER_RESOURCE_VIEW_DESC desc = {};
		desc.ViewDimension = D3D_SRV_DIMENSION::D3D11_SRV_DIMENSION_BUFFEREX;
		desc.Format = DXGI_FORMAT::DXGI_FORMAT_R32_TYPELESS;
		desc.BufferEx.FirstElement = 0;
		desc.BufferEx.Flags = D3D11_BUFFEREX_SRV_FLAG::D3D11_BUFFEREX_SRV_FLAG_RAW;
		desc.BufferEx.NumElements = numUnits;

		throwOnDXError(device->CreateShaderResourceView(handle.Get(), &desc, view.GetAddressOf()));
	}
};
//=========================================================================== RWByteAddressBuffer
class RWByteAddressBuffer final : public Buffer {
public:
    ComPtr<ID3D11UnorderedAccessView> uav;

    RWByteAddressBuffer() {
        _bindFlags = D3D11_BIND_FLAG::D3D11_BIND_SHADER_RESOURCE |
                     D3D11_BIND_FLAG::D3D11_BIND_UNORDERED_ACCESS;
        _usage = D3D11_USAGE::D3D11_USAGE_DEFAULT;
        _misc = D3D11_RESOURCE_MISC_FLAG::D3D11_RESOURCE_MISC_BUFFER_ALLOW_RAW_VIEWS;
    }
    void init(ComPtr<ID3D11Device> device, uint numUints) {
        Buffer::init(device, numUints * 4, nullptr);

        D3D11_UNORDERED_ACCESS_VIEW_DESC desc = {};
        desc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
        desc.Format = DXGI_FORMAT::DXGI_FORMAT_R32_TYPELESS;
        desc.Buffer.FirstElement = 0;
        desc.Buffer.NumElements = numUints;
        desc.Buffer.Flags = D3D11_BUFFEREX_SRV_FLAG::D3D11_BUFFEREX_SRV_FLAG_RAW;
        throwOnDXError(device->CreateUnorderedAccessView(handle.Get(), &desc, uav.GetAddressOf()));
    }
};
} /// dx11