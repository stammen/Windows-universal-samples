#include "pch.h"
#include "VideoTexture.h"
#include "Common\DirectXHelper.h"
#include "Windows.Graphics.DirectX.Direct3D11.interop.h"
#include "360VideoPlaybackMain.h"

using namespace _360VideoPlayback;
using namespace concurrency;
using namespace DirectX;
using namespace Microsoft::WRL;
using namespace std::placeholders;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Numerics;
using namespace Windows::Graphics::DirectX::Direct3D11;
using namespace Windows::Media::Core;
using namespace Windows::Media::Playback;
using namespace Windows::UI::Input::Spatial;

// Loads vertex and pixel shaders from files and instantiates the cube geometry.
VideoTexture::VideoTexture()

{
    m_deviceResources = _360VideoPlaybackMain::GetDeviceResources();

}


void VideoTexture::CreateDeviceDependentResources(UINT width, UINT height)
{
    // Create the Texture, ShaderResource and Sampler state

    DX::ThrowIfFailed(
        m_deviceResources->GetD3DDevice()->CreateTexture2D(
            &CD3D11_TEXTURE2D_DESC(
                DXGI_FORMAT_R8G8B8A8_UNORM,
                width,        
                height,      
                1,          // MipLevels
                1,          // ArraySize
                D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET
            ),
            nullptr,
            &m_texture
        )
    );

    DX::ThrowIfFailed(
        m_deviceResources->GetD3DDevice()->CreateShaderResourceView(
            m_texture.Get(), nullptr,
            &m_textureView
        )
    );

    Microsoft::WRL::ComPtr<IDXGISurface> spDXGIInterfaceAccess;
    DX::ThrowIfFailed(m_texture->QueryInterface(IID_PPV_ARGS(&spDXGIInterfaceAccess)));
    m_surface = CreateDirect3DSurface(spDXGIInterfaceAccess.Get());
}



void VideoTexture::ReleaseDeviceDependentResources()
{
    m_texture.Reset();
    m_textureView.Reset();
    m_surface = nullptr;
}




