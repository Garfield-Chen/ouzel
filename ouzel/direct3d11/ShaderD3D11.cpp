// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#include <fstream>
#include "ShaderD3D11.h"
#include "Engine.h"
#include "RendererD3D11.h"
#include "FileSystem.h"
#include "Utils.h"

namespace ouzel
{
    namespace graphics
    {
        ShaderD3D11::ShaderD3D11()
        {
        }

        ShaderD3D11::~ShaderD3D11()
        {
            destroy();
        }

        void ShaderD3D11::destroy()
        {
            if (pixelShader)
            {
                pixelShader->Release();
                pixelShader = nullptr;
            }

            if (vertexShader)
            {
                vertexShader->Release();
                vertexShader = nullptr;
            }

            if (inputLayout)
            {
                inputLayout->Release();
                inputLayout = nullptr;
            }

            if (pixelShaderConstantBuffer)
            {
                pixelShaderConstantBuffer->Release();
                pixelShaderConstantBuffer = nullptr;
            }

            if (vertexShaderConstantBuffer)
            {
                vertexShaderConstantBuffer->Release();
                vertexShaderConstantBuffer = nullptr;
            }
        }

        bool ShaderD3D11::initFromBuffers(const uint8_t* newPixelShader,
                                          uint32_t newPixelShaderSize,
                                          const uint8_t* newVertexShader,
                                          uint32_t newVertexShaderSize,
                                          uint32_t newVertexAttributes,
                                          const std::string& pixelShaderFunction,
                                          const std::string& vertexShaderFunction)
        {
            if (!Shader::initFromBuffers(newPixelShader, newPixelShaderSize, newVertexShader, newVertexShaderSize, newVertexAttributes, pixelShaderFunction, vertexShaderFunction))
            {
                return false;
            }

            destroy();

            RendererD3D11* rendererD3D11 = static_cast<RendererD3D11*>(sharedEngine->getRenderer());

            HRESULT hr = rendererD3D11->getDevice()->CreatePixelShader(newPixelShader, newPixelShaderSize, NULL, &pixelShader);
            if (FAILED(hr))
            {
                log("Failed to create a D3D11 pixel shader");
                return false;
            }

            hr = rendererD3D11->getDevice()->CreateVertexShader(newVertexShader, newVertexShaderSize, NULL, &vertexShader);
            if (FAILED(hr))
            {
                log("Failed to create a D3D11 vertex shader");
                return false;
            }

            std::vector<D3D11_INPUT_ELEMENT_DESC> vertexInputElements;

            UINT offset = 0;

            if (vertexAttributes & VERTEX_POSITION)
            {
                vertexInputElements.push_back({ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, offset, D3D11_INPUT_PER_VERTEX_DATA, 0 });
                offset += 3 * sizeof(float);
            }

            if (vertexAttributes & VERTEX_COLOR)
            {
                vertexInputElements.push_back({ "COLOR", 0, DXGI_FORMAT_R8G8B8A8_UNORM, 0, offset, D3D11_INPUT_PER_VERTEX_DATA, 0 });
                offset += 4 * sizeof(uint8_t);
            }

            if (vertexAttributes & VERTEX_NORMAL)
            {
                vertexInputElements.push_back({ "NORMAL", 0, DXGI_FORMAT_R32G32_FLOAT, 0, offset, D3D11_INPUT_PER_VERTEX_DATA, 0 });
                offset += 3 * sizeof(float);
            }

            if (vertexAttributes & VERTEX_TEXCOORD0)
            {
                vertexInputElements.push_back({ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, offset, D3D11_INPUT_PER_VERTEX_DATA, 0 });
                offset += 2 * sizeof(float);
            }

            if (vertexAttributes & VERTEX_TEXCOORD1)
            {
                vertexInputElements.push_back({ "TEXCOORD", 1, DXGI_FORMAT_R32G32_FLOAT, 0, offset, D3D11_INPUT_PER_VERTEX_DATA, 0 });
                offset += 2 * sizeof(float);
            }

            hr = rendererD3D11->getDevice()->CreateInputLayout(
                vertexInputElements.data(),
                static_cast<UINT>(vertexInputElements.size()),
                newVertexShader,
                newVertexShaderSize,
                &inputLayout);

            if (FAILED(hr))
            {
                log("Failed to create D3D11 input layout for vertex shader");
                return false;
            }

            if (!createPixelShaderConstantBuffer(sizeof(Matrix4)))
            {
                return false;
            }

            if (!createVertexShaderConstantBuffer(sizeof(Matrix4)))
            {
                return false;
            }

            return true;
        }

        bool ShaderD3D11::setPixelShaderConstantInfo(const std::vector<ConstantInfo>& constantInfo, uint32_t alignment)
        {
            Shader::setPixelShaderConstantInfo(constantInfo, alignment);

            pixelShaderConstantLocations.reserve(constantInfo.size());

            uint32_t offset = 0;

            for (const ConstantInfo& info : pixelShaderConstantInfo)
            {
                pixelShaderConstantLocations.push_back(offset);
                offset += info.size;
            }

            if (offset > pixelShaderData.size())
            {
                if (pixelShaderConstantBuffer)
                {
                    pixelShaderConstantBuffer->Release();
                    pixelShaderConstantBuffer = nullptr;
                }
                createPixelShaderConstantBuffer(offset);
            }

            return true;
        }

        bool ShaderD3D11::setVertexShaderConstantInfo(const std::vector<ConstantInfo>& constantInfo, uint32_t alignment)
        {
            Shader::setVertexShaderConstantInfo(constantInfo, alignment);

            vertexShaderConstantLocations.reserve(constantInfo.size());

            uint32_t offset = 0;

            for (const ConstantInfo& info : vertexShaderConstantInfo)
            {
                vertexShaderConstantLocations.push_back(offset);
                offset += info.size;
            }

            if (offset > vertexShaderData.size())
            {
                if (vertexShaderConstantBuffer)
                {
                    vertexShaderConstantBuffer->Release();
                    vertexShaderConstantBuffer = nullptr;
                }
                createVertexShaderConstantBuffer(offset);
            }

            return true;
        }
        
        bool ShaderD3D11::setPixelShaderConstant(uint32_t index, uint32_t size, uint32_t count, const float* value)
        {
            if (index >= pixelShaderConstantLocations.size()) return false;

            uint32_t location = pixelShaderConstantLocations[index];
            memcpy(pixelShaderData.data() + location, value, size * count);

            return uploadData(pixelShaderConstantBuffer, pixelShaderData.data(), static_cast<uint32_t>(pixelShaderData.size()));
        }

        bool ShaderD3D11::setVertexShaderConstant(uint32_t index, uint32_t size, uint32_t count, const float* value)
        {
            if (index >= vertexShaderConstantLocations.size()) return false;

            uint32_t location = vertexShaderConstantLocations[index];
            memcpy(vertexShaderData.data() + location, value, size * count);
            
            return uploadData(vertexShaderConstantBuffer, vertexShaderData.data(), static_cast<uint32_t>(vertexShaderData.size()));
        }

        bool ShaderD3D11::createPixelShaderConstantBuffer(uint32_t size)
        {
            RendererD3D11* rendererD3D11 = static_cast<RendererD3D11*>(sharedEngine->getRenderer());

            D3D11_BUFFER_DESC pixelShaderConstantBufferDesc;
            memset(&pixelShaderConstantBufferDesc, 0, sizeof(pixelShaderConstantBufferDesc));

            pixelShaderConstantBufferDesc.ByteWidth = static_cast<UINT>(size);
            pixelShaderConstantBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
            pixelShaderConstantBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
            pixelShaderConstantBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

            HRESULT hr = rendererD3D11->getDevice()->CreateBuffer(&pixelShaderConstantBufferDesc, nullptr, &pixelShaderConstantBuffer);
            if (FAILED(hr))
            {
                log("Failed to create D3D11 constant buffer");
                return false;
            }

            pixelShaderData.resize(size);

            return true;
        }

        bool ShaderD3D11::createVertexShaderConstantBuffer(uint32_t size)
        {
            RendererD3D11* rendererD3D11 = static_cast<RendererD3D11*>(sharedEngine->getRenderer());

            D3D11_BUFFER_DESC vertexShaderConstantBufferDesc;
            memset(&vertexShaderConstantBufferDesc, 0, sizeof(vertexShaderConstantBufferDesc));

            vertexShaderConstantBufferDesc.ByteWidth = static_cast<UINT>(size);
            vertexShaderConstantBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
            vertexShaderConstantBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
            vertexShaderConstantBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

            HRESULT hr = rendererD3D11->getDevice()->CreateBuffer(&vertexShaderConstantBufferDesc, nullptr, &vertexShaderConstantBuffer);
            if (FAILED(hr))
            {
                log("Failed to create D3D11 constant buffer");
                return false;
            }

            vertexShaderData.resize(size);

            return true;
        }

        bool ShaderD3D11::uploadData(ID3D11Buffer* buffer, const void* data, uint32_t size)
        {
            RendererD3D11* rendererD3D11 = static_cast<RendererD3D11*>(sharedEngine->getRenderer());

            D3D11_MAPPED_SUBRESOURCE mappedSubresource;
            HRESULT hr = rendererD3D11->getContext()->Map(buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedSubresource);
            if (FAILED(hr))
            {
                log("Failed to lock D3D11 buffer");
                return false;
            }

            memcpy(mappedSubresource.pData, data, size);

            rendererD3D11->getContext()->Unmap(buffer, 0);

            return true;
        }
    } // namespace graphics
} // namespace ouzel
