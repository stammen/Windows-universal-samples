#pragma once

#include "..\Common\DeviceResources.h"
#include "..\Common\StepTimer.h"
#include "VideoTexture.h"
#include "ShaderStructures.h"
#include <concrt.h>
#include <mutex>

namespace _360VideoPlayback
{
    // This sample renderer instantiates a basic rendering pipeline.
    class VideoRenderer
    {
    public:
        VideoRenderer();
        void CreateDeviceDependentResources();
        void ReleaseDeviceDependentResources();
        void Update(const DX::StepTimer& timer);
        void Render();
        void ComputeSphere(unsigned short tessellation, bool invertn);
        void LoadShaders();
    private:

        void UpdateCurrentVideoFrame();

        // Cached pointer to device resources.
        std::shared_ptr<DX::DeviceResources>            m_deviceResources;

        // Direct3D resources for cube geometry.
        Microsoft::WRL::ComPtr<ID3D11InputLayout>       m_inputLayout;
        Microsoft::WRL::ComPtr<ID3D11Buffer>            m_vertexBuffer;
        Microsoft::WRL::ComPtr<ID3D11Buffer>            m_indexBuffer;
        Microsoft::WRL::ComPtr<ID3D11VertexShader>      m_vertexShader;
        Microsoft::WRL::ComPtr<ID3D11GeometryShader>    m_geometryShader;
        Microsoft::WRL::ComPtr<ID3D11PixelShader>       m_pixelShader;
        Microsoft::WRL::ComPtr<ID3D11Buffer>            m_modelConstantBuffer;

        // System resources for cube geometry.
        ModelConstantBuffer                             m_modelConstantBufferData;
        uint32                                          m_indexCount = 0;

        // Variables used with the rendering loop.
        bool                                            m_loadingComplete = false;
        float                                           m_degreesPerSecond = 45.f;

        // If the current D3D Device supports VPRT, we can avoid using a geometry
// shader just to set the render target array index.
        bool                                            m_usingVprtShaders = false;

        VideoTexture^                                   m_texture1;
        VideoTexture^                                   m_texture2;
        Microsoft::WRL::ComPtr<ID3D11SamplerState>       m_quadTextureSamplerState;
        void OnVideoFrameAvailable();
        Windows::Foundation::EventRegistrationToken      m_videoFrameAvailToken;
        std::mutex m_mutex;
        bool m_frameAvailable;
    };
}
