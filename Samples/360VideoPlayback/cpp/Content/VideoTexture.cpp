#include "pch.h"
#include "VideoTexture.h"
#include "Common\DirectXHelper.h"
#include "Windows.Graphics.DirectX.Direct3D11.interop.h"
#include "360VideoPlaybackMain.h"

#include <Mfapi.h>

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
    : m_mediaDevice(nullptr)
    , m_uiDeviceResetToken(0)
    , m_texture(nullptr)
    , m_sharedTexture(nullptr)
    , m_textureView(nullptr)
    , m_surface(nullptr)

{
    m_deviceResources = _360VideoPlaybackMain::GetDeviceResources();
}


void VideoTexture::CreateDeviceDependentResources(UINT width, UINT height)
{
    //Initialize the DirectX device for the MediaPlayer
    DX::ThrowIfFailed(InitializeMediaDevice());

    // Create the Texture and ShaderResource
    D3D11_TEXTURE2D_DESC texDesc = { 0 };
    texDesc.Width = width;
    texDesc.Height = height;
    texDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
    texDesc.MipLevels = 1;
    texDesc.ArraySize = 1;
    texDesc.SampleDesc.Count = 1;
    texDesc.SampleDesc.Quality = 0;
    texDesc.Usage = D3D11_USAGE_DEFAULT;
    texDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
    texDesc.CPUAccessFlags = 0;
    texDesc.MiscFlags = D3D11_RESOURCE_MISC_SHARED;

    DX::ThrowIfFailed(m_deviceResources->GetD3DDevice()->CreateTexture2D(&texDesc,nullptr, &m_texture));
    DX::ThrowIfFailed(m_deviceResources->GetD3DDevice()->CreateShaderResourceView(m_texture.Get(), nullptr, &m_textureView));

    // create a shared texture handle to the texture
    ComPtr<IDXGIResource> dxgiResource;
    HANDLE sharedHandle;
    DX::ThrowIfFailed(m_texture.As(&dxgiResource));
    DX::ThrowIfFailed(dxgiResource->GetSharedHandle(&sharedHandle));

    // open the shared texture on the directx device assigned to the Media Player
    DX::ThrowIfFailed(m_mediaDevice->OpenSharedResource(sharedHandle, __uuidof(ID3D11Texture2D), (LPVOID*)m_sharedTexture.GetAddressOf()));
    Microsoft::WRL::ComPtr<IDXGISurface> spDXGIInterfaceAccess;
    DX::ThrowIfFailed(m_sharedTexture->QueryInterface(IID_PPV_ARGS(&spDXGIInterfaceAccess)));
    m_surface = CreateDirect3DSurface(spDXGIInterfaceAccess.Get());
}

Windows::Graphics::DirectX::Direct3D11::IDirect3DSurface^ VideoTexture::GetSurface()
{
    return m_surface;
}

void VideoTexture::ReleaseDeviceDependentResources()
{
    MFUnlockDXGIDeviceManager();
    m_sharedTexture.Reset();
    m_texture.Reset();
    m_textureView.Reset();
    m_surface = nullptr;
    m_mediaDevice.Reset();
    m_mediaDevice = nullptr;
}

HRESULT VideoTexture::CreateMediaDevice(
    IDXGIAdapter* pDXGIAdapter,
    ID3D11Device** ppDevice)
{
    // Create the Direct3D 11 API device object and a corresponding context.
    D3D_FEATURE_LEVEL featureLevel;

    // This flag adds support for surfaces with a different color channel ordering
    // than the API default. It is required for compatibility with Direct2D.
    UINT creationFlags = D3D11_CREATE_DEVICE_VIDEO_SUPPORT | D3D11_CREATE_DEVICE_BGRA_SUPPORT;

#if defined(_DEBUG)
    if (SdkLayersAvailable())
    {
        // If the project is in a debug build, enable debugging via SDK Layers with this flag.
        creationFlags |= D3D11_CREATE_DEVICE_DEBUG;
    }
#endif
    D3D_FEATURE_LEVEL featureLevels[] =
    {
        D3D_FEATURE_LEVEL_11_1,
        D3D_FEATURE_LEVEL_11_0,
        D3D_FEATURE_LEVEL_10_1,
        D3D_FEATURE_LEVEL_10_0,
    };

    ComPtr<ID3D11Device> spDevice;
    ComPtr<ID3D11DeviceContext> spContext;

    D3D_DRIVER_TYPE driverType = (nullptr != pDXGIAdapter) ? D3D_DRIVER_TYPE_UNKNOWN : D3D_DRIVER_TYPE_HARDWARE;

    // Create a device using the hardware graphics driver if adapter is not supplied
    HRESULT hr = D3D11CreateDevice(
        pDXGIAdapter,               // if nullptr will use default adapter.
        driverType,
        0,                          // Should be 0 unless the driver is D3D_DRIVER_TYPE_SOFTWARE.
        creationFlags,              // Set debug and Direct2D compatibility flags.
        featureLevels,              // List of feature levels this app can support.
        ARRAYSIZE(featureLevels),   // Size of the list above.
        D3D11_SDK_VERSION,          // Always set this to D3D11_SDK_VERSION for Windows Store apps.
        &spDevice,                  // Returns the Direct3D device created.
        &featureLevel,              // Returns feature level of device created.
        &spContext                  // Returns the device immediate context.
    );

    if (FAILED(hr))
    {
        // fallback to WARP if we are not specifying an adapter
        if (nullptr == pDXGIAdapter)
        {
            // If the initialization fails, fall back to the WARP device.
            // For more information on WARP, see: 
            // http://go.microsoft.com/fwlink/?LinkId=286690
            hr = D3D11CreateDevice(
                nullptr,
                D3D_DRIVER_TYPE_WARP, // Create a WARP device instead of a hardware device.
                0,
                creationFlags,
                featureLevels,
                ARRAYSIZE(featureLevels),
                D3D11_SDK_VERSION,
                &spDevice,
                &featureLevel,
                &spContext);
        }

        DX::ThrowIfFailed(hr);
    }
    else
    {
        // workaround for nvidia GPU's, cast to ID3D11VideoDevice
        Microsoft::WRL::ComPtr<ID3D11VideoDevice> videoDevice;
        spDevice.As(&videoDevice);
    }

    // Turn multithreading on 
    ComPtr<ID3D10Multithread> spMultithread;
    if (SUCCEEDED(spContext.As(&spMultithread)))
    {
        spMultithread->SetMultithreadProtected(TRUE);
    }

    *ppDevice = spDevice.Detach();

    return S_OK;
}

HRESULT VideoTexture::InitializeMediaDevice()
{
    m_mediaDevice = nullptr;

    // ref count passed in device
    ComPtr<ID3D11Device> spDevice(m_deviceResources->GetD3DDevice());

    // make sure creation of the device is on the same adapter
    ComPtr<IDXGIDevice> spDXGIDevice;
    DX::ThrowIfFailed(spDevice.As(&spDXGIDevice));

    ComPtr<IDXGIAdapter> spAdapter;
    DX::ThrowIfFailed(spDXGIDevice->GetAdapter(&spAdapter));

    // create dx device for media pipeline
    ComPtr<ID3D11Device> spMediaDevice;
    DX::ThrowIfFailed(CreateMediaDevice(spAdapter.Get(), &spMediaDevice));

    // lock the shared dxgi device manager
    // will keep lock open for the life of object
    // call MFUnlockDXGIDeviceManager when unloading
    if (!m_spDeviceManager)
    {
        DX::ThrowIfFailed(MFLockDXGIDeviceManager(&m_uiDeviceResetToken, &m_spDeviceManager));
    }

    // associtate the device with the manager
    DX::ThrowIfFailed(m_spDeviceManager->ResetDevice(spMediaDevice.Get(), m_uiDeviceResetToken));


    m_mediaDevice.Attach(spMediaDevice.Detach());

    Microsoft::WRL::ComPtr<ID3D11VideoDevice> videoDevice;
    m_mediaDevice.As(&videoDevice);

#if 0
    // No hardware decoding support at all? 
    if (videoDevice == nullptr)
    {
        m_noHWDecoding = true;
    }
    else // ok, it supports hadrware decoding in general, let's check in it supports 4K decoding 
    {
        D3D11_VIDEO_DECODER_DESC desc = { 0 };
        D3D11_VIDEO_DECODER_CONFIG config = { 0 };

        desc.Guid = D3D11_DECODER_PROFILE_H264_VLD_NOFGT;
        desc.OutputFormat = DXGI_FORMAT_NV12;
        desc.SampleHeight = 4096u;
        desc.SampleWidth = 4096u;

        config.guidConfigBitstreamEncryption = DXVA_NoEncrypt;
        config.guidConfigMBcontrolEncryption = DXVA_NoEncrypt;
        config.guidConfigResidDiffEncryption = DXVA_NoEncrypt;
        config.ConfigBitstreamRaw = 1;
        config.ConfigResidDiffAccelerator = 1;
        config.ConfigHostInverseScan = 1;
        config.ConfigSpecificIDCT = 2;

        Microsoft::WRL::ComPtr<ID3D11VideoDecoder> videoDecoder;
        videoDevice->CreateVideoDecoder(&desc, &config, &videoDecoder);
        if (!videoDecoder)
        {
            m_noHWDecoding = true;
        }
    }
#endif
    return S_OK;
}




