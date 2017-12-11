#pragma once

#include "..\Common\DeviceResources.h"
#include <concrt.h>

namespace _360VideoPlayback
{
    // This sample renderer instantiates a basic rendering pipeline.
    ref class VideoTexture sealed
    {
    public:
        VideoTexture();
        void CreateDeviceDependentResources(UINT width, UINT height);
        void ReleaseDeviceDependentResources();

    internal:
        Microsoft::WRL::ComPtr<ID3D11Texture2D>         GetTexture() { return m_texture; };
        Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>        GetShaderResourceView () { return m_textureView; };
        Windows::Graphics::DirectX::Direct3D11::IDirect3DSurface^       GetSurface() { return m_surface; };

    private:
        // Cached pointer to device resources.
        std::shared_ptr<DX::DeviceResources>            m_deviceResources;

        uint32                                          m_indexCount = 0;

        Microsoft::WRL::ComPtr<ID3D11Texture2D>         m_texture;
        Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_textureView;
        Windows::Graphics::DirectX::Direct3D11::IDirect3DSurface^ m_surface;
        Concurrency::critical_section m_critical;
    };
}
