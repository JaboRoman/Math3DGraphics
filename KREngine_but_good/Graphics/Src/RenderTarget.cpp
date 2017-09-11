#include "Precompiled.h"
#include "RenderTarget.h"
#include "GraphicsSystem.h"


using namespace Graphics;

namespace
{
	DXGI_FORMAT GetFormat(RenderTarget::Format format)
	{
		switch (format)
		{
		case RenderTarget::Format::RGBA_U8: return DXGI_FORMAT_R8G8B8A8_UNORM;
		case RenderTarget::Format::R_F16: return DXGI_FORMAT_R16_FLOAT;
		case RenderTarget::Format::RGBA_F16: return DXGI_FORMAT_R16G16B16A16_FLOAT;
		case RenderTarget::Format::R_S32: return DXGI_FORMAT_R32_SINT;
		case RenderTarget::Format::RGBA_U32: return DXGI_FORMAT_R32G32B32A32_UINT;
		default:
			ASSERT(false, "[RenderTarget] Invalid Format");
			break;
		}
		return DXGI_FORMAT_R8G8B8A8_UNORM;
	}
}

RenderTarget::RenderTarget()
	: mShaderResourceView(nullptr)
	, mRenderTargetView(nullptr)
	, mDepthStencilView(nullptr)
{
}

RenderTarget::~RenderTarget()
{
	ASSERT(mShaderResourceView == nullptr, "[RenderTarget] Shader Resource not released");
	ASSERT(mRenderTargetView == nullptr, "[RenderTarget] Render Target not released");
	ASSERT(mDepthStencilView == nullptr, "[RenderTarget] Depth Stencil View not released");
}

void RenderTarget::Initialize(uint32_t width, uint32_t height, Format format)
{
	D3D11_TEXTURE2D_DESC desc{};
	desc.Width = width;
	desc.Height = height;
	desc.MipLevels = 1;
	desc.ArraySize = 1;
	desc.Format = GetFormat(format);
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	desc.CPUAccessFlags = 0;
	desc.MiscFlags = 0;

	ID3D11Device* device = GraphicsSystem::Get()->GetDevice();

	ID3D11Texture2D* texture = nullptr;
	HRESULT hr = device->CreateTexture2D(&desc, nullptr, &texture);
	ASSERT(SUCCEEDED(hr), "[RenderTarget] Failed to create render texture");

	hr = device->CreateShaderResourceView(texture, nullptr, &mShaderResourceView);
	ASSERT(SUCCEEDED(hr), "[RenderTarget] Failed to create shader resource view");

	hr = device->CreateRenderTargetView(texture, nullptr, &mRenderTargetView);
	ASSERT(SUCCEEDED(hr), "[RenderTarget] Failed to create render target view");
	SafeRelease(texture);

	desc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	desc.BindFlags = D3D11_BIND_DEPTH_STENCIL;

	hr = device->CreateTexture2D(&desc, nullptr, &texture);
	ASSERT(SUCCEEDED(hr), "[RenderTarget] Failed to create depth stencil texture");

	hr = device->CreateDepthStencilView(texture, nullptr, &mDepthStencilView);
	ASSERT(SUCCEEDED(hr), "[RenderTarget] Failed to create depth stencil view");
	SafeRelease(texture);

	mViewport.TopLeftX = 0.0f;
	mViewport.TopLeftY = 0.0f;
	mViewport.Width = static_cast<float>(width);
	mViewport.Height = static_cast<float>(height);
	mViewport.MinDepth = 0.0f;
	mViewport.MaxDepth = 1.0f;
}

void RenderTarget::Terminate()
{
	SafeRelease(mShaderResourceView);
	SafeRelease(mRenderTargetView);
	SafeRelease(mDepthStencilView);
}

void Graphics::RenderTarget::BeginRender()
{
	float ClearColor[4] = { 0.1f, 0.1f, 0.1f, 1.0f };
	ID3D11DeviceContext* context = GraphicsSystem::Get()->GetContext();
	context->ClearRenderTargetView(mRenderTargetView, ClearColor);
	context->ClearDepthStencilView(mDepthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);
	context->OMSetRenderTargets(1, &mRenderTargetView, mDepthStencilView);
	context->RSSetViewports(1, &mViewport);
}

void Graphics::RenderTarget::EndRender()
{
	GraphicsSystem::Get()->ResetRenderTarget();
	GraphicsSystem::Get()->ResetViewport();
}

void Graphics::RenderTarget::BindPS(uint32_t index)
{
	GraphicsSystem::Get()->GetContext()->PSSetShaderResources(index, 1, &mShaderResourceView);
}

void Graphics::RenderTarget::UnbindPS(uint32_t index)
{
	static ID3D11ShaderResourceView* temp = nullptr;
	GraphicsSystem::Get()->GetContext()->PSGetShaderResources(index, 1, &temp);
}
