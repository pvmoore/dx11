#pragma once
///
/// Handle debug prints from within a compute shader.
/// eg.
/// Initialisation:
///     shaderPrintf.init(device, 7, true); // use uav register 7, only decode response once
///
/// Add the following to your shader code:
///     RWByteAddressBuffer PrintfBuffer : register(u7); // set to same register as above
///     #include "debug_inc.hlsl"
///     if(printBarrier()) {
///         print(3.14); // output 3.14
///     }
///
/// In your render code:
///     shaderPrintf.before(frame);
///         context.Dispatch(...) // <--- your compute shader dispatch
///     shaderPrintf.after(frame);
///     if(shaderPrintf.hasOutput()) printf("%s", shaderPrintf.getOutput().c_str());
///
namespace dx11 {

class ShaderPrintf final {
    static const uint NUM_UINTS = 1024 * 1024;
    RWByteAddressBuffer printfBuffer;
    StagingReadBuffer<uint> stagingRead;
    uint uavIndex;
    bool onceOnly;
    uint count = 0;
    std::string output;
    std::string floatFmt = "%f";
public:
    ShaderPrintf & init(ComPtr<ID3D11Device> device, uint uavIndex, bool onceOnly=true) {
        this->uavIndex = uavIndex;
        this->onceOnly = onceOnly;

        printfBuffer.init(device, NUM_UINTS);
        stagingRead.init(device, NUM_UINTS);
        return *this;
    }
    ShaderPrintf& setFloatFormat(const std::string& fmt) {
        assert(fmt.find('%')!=std::string::npos);
        floatFmt = fmt;
        return *this;
    }
    bool hasOutput() const { 
        return output.size() > 0 ; 
    }
    const std::string& getOutput() const { return output; }
    /// Call this before dispatching the compute shader.
    void before(const FrameResource& frame) {
        output.clear();
        /// Set our uav
        frame.context->CSSetUnorderedAccessViews(uavIndex, 1, printfBuffer.uav.GetAddressOf(), nullptr);
    }
    /// Call this after the computer shader dispatch has finished.
    void after(const FrameResource& frame) {
        /// Remove our uav
        ID3D11UnorderedAccessView* nulluavs[] = {nullptr};
        frame.context->CSSetUnorderedAccessViews(uavIndex, 1, nulluavs, nullptr);

        if(onceOnly && count>0) return;

        /// Copy our printf buffer to CPU staging memory
        frame.context->CopyResource(stagingRead.handle.Get(), printfBuffer.handle.Get());

        /// Reset/clear our uav buffer
        uint vals[4] = {0, 0, 0, 0};
        frame.context->ClearUnorderedAccessViewUint(printfBuffer.uav.Get(), vals);

        /// Read our printf buffer and decode it to _output_
        readAndDecode(frame.context);
        count++;
    }
private:
    void readAndDecode(ComPtr<ID3D11DeviceContext> context) {
        D3D11_MAPPED_SUBRESOURCE mappedResource = {};
        throwOnDXError(context->Map(stagingRead.handle.Get(), 0, D3D11_MAP::D3D11_MAP_READ, 0, &mappedResource));
     
        decode((uint*)mappedResource.pData);
        
        context->Unmap(stagingRead.handle.Get(), 0);
    }
    void decode(uint* data) {
        char suffix = '\n';
        uint length = data[1];
        uint* ptr   = data+2;
        float* fptr = (float*)ptr;
        core::CharBuffer buf;

        for(uint i = 0; i < length; ) {
            uint type       = ptr[i++];
            uint components = ptr[i++];

            //core::Log::write("type=", type, " components=", components);

            switch(type) {
                case 0: // char
                    buf.append((char)ptr[i++]);
                    break;
                case 1: // uint
                    buf.appendFmt("%08x", ptr[i++]);
                    while(--components) {
                        buf.appendFmt(", %08x", ptr[i++]);
                    }
                    break;
                case 2: // int
                    buf.appendFmt("%d", ptr[i++]);
                    while(--components) {
                        buf.appendFmt(", %d", ptr[i++]);
                    }
                    break;
                case 3: // float
                    buf.appendFmt(floatFmt.c_str(), fptr[i++]);
                    while(--components) {
                        buf.appendFmt((", "+ floatFmt).c_str(), fptr[i++]);
                    }
                    break;
                case 4: // ulong
                {
                    ulong v = ((ulong)ptr[i++] << 32) | (ulong)ptr[i++];
                    buf.appendFmt("%016llx", v);
                    break;
                }
                case 5: // slong 
                {
                    slong v = ((slong)ptr[i++] << 32) | (slong)ptr[i++];
                    buf.appendFmt("%lld", v);
                    break;
                }
                case 6: // matrix
                {
                    matrix temp;
                    int cols = ptr[i++];
                    int rows = ptr[i++];
                    // copy to temp
                    for(int j = 0; j < rows*cols; j++) {
                        int c = j / rows;
                        int r = j % rows;
                        temp[c][r] = fptr[i++];
                    }
                    for(auto r = 0; r < rows; r++) {
                        for(auto c = 0; c < cols; c++) {
                            if(c > 0) buf.append(',');
                            buf.appendFmt((" "+ floatFmt).c_str(), temp[c][r]);
                        }
                        buf.append('\n');
                    }
                    break;
                }
                case 7: // set suffix
                    suffix = (char)ptr[i++];
                    goto lp;
                default:
                    break;
            }
            if(suffix != 0) buf.append(suffix);
        lp:;
        }
        output = buf.std_str();
    }
};


}