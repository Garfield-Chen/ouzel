// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "MeshBufferD3D11.h"
#include "core/Engine.h"
#include "RendererD3D11.h"
#include "utils/Utils.h"

namespace ouzel
{
    namespace graphics
    {
        MeshBufferD3D11::MeshBufferD3D11()
        {
        }

        MeshBufferD3D11::~MeshBufferD3D11()
        {
            if (indexBuffer)
            {
                indexBuffer->Release();
            }

            if (vertexBuffer)
            {
                vertexBuffer->Release();
            }
        }

        void MeshBufferD3D11::free()
        {
            MeshBuffer::free();

            if (indexBuffer)
            {
                indexBuffer->Release();
                indexBuffer = nullptr;
            }

            if (vertexBuffer)
            {
                vertexBuffer->Release();
                vertexBuffer = nullptr;
            }
        }

        bool MeshBufferD3D11::uploadBuffer(ID3D11Buffer* buffer, const std::vector<uint8_t>& data)
        {
            std::shared_ptr<RendererD3D11> rendererD3D11 = std::static_pointer_cast<RendererD3D11>(sharedEngine->getRenderer());

            D3D11_MAPPED_SUBRESOURCE mappedSubResource;
            HRESULT hr = rendererD3D11->getContext()->Map(buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedSubResource);
            if (FAILED(hr))
            {
                log(LOG_LEVEL_ERROR, "Failed to lock Direct3D 11 buffer");
                return false;
            }

            memcpy(mappedSubResource.pData, data.data(), data.size());

            rendererD3D11->getContext()->Unmap(buffer, 0);

            return true;
        }

        bool MeshBufferD3D11::upload()
        {
            if (dirty)
            {
                std::shared_ptr<RendererD3D11> rendererD3D11 = std::static_pointer_cast<RendererD3D11>(sharedEngine->getRenderer());

                if (dirty & INDEX_SIZE_DIRTY)
                {
                    switch (uploadData.indexSize)
                    {
                    case 2:
                        indexFormat = DXGI_FORMAT_R16_UINT;
                        break;
                    case 4:
                        indexFormat = DXGI_FORMAT_R32_UINT;
                        break;
                    default:
                        indexFormat = DXGI_FORMAT_UNKNOWN;
                        log(LOG_LEVEL_ERROR, "Invalid index size");
                        return false;
                    }

                    dirty &= ~INDEX_SIZE_DIRTY;
                }

                if (dirty & INDEX_BUFFER_DIRTY)
                {
                    if (!uploadData.indexData.empty())
                    {
                        if (!indexBuffer || uploadData.indexData.size() > indexBufferSize)
                        {
                            if (indexBuffer) indexBuffer->Release();

                            indexBufferSize = static_cast<UINT>(uploadData.indexData.size());

                            D3D11_BUFFER_DESC indexBufferDesc;
                            memset(&indexBufferDesc, 0, sizeof(indexBufferDesc));

                            indexBufferDesc.ByteWidth = indexBufferSize;
                            indexBufferDesc.Usage = uploadData.dynamicIndexBuffer ? D3D11_USAGE_DYNAMIC : D3D11_USAGE_DEFAULT;
                            indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
                            indexBufferDesc.CPUAccessFlags = dynamicIndexBuffer ? D3D11_CPU_ACCESS_WRITE : 0;

                            D3D11_SUBRESOURCE_DATA indexBufferResourceData;
                            memset(&indexBufferResourceData, 0, sizeof(indexBufferResourceData));
                            indexBufferResourceData.pSysMem = uploadData.indexData.data();

                            HRESULT hr = rendererD3D11->getDevice()->CreateBuffer(&indexBufferDesc, &indexBufferResourceData, &indexBuffer);
                            if (FAILED(hr))
                            {
                                log(LOG_LEVEL_ERROR, "Failed to create Direct3D 11 index buffer");
                                return false;
                            }
                        }
                        else
                        {
                            if (!uploadBuffer(indexBuffer, uploadData.indexData))
                            {
                                return false;
                            }
                        }
                    }

                    dirty &= ~INDEX_BUFFER_DIRTY;
                }

                if (dirty & VERTEX_BUFFER_DIRTY)
                {
                    if (!uploadData.vertexData.empty())
                    {
                        if (!vertexBuffer || uploadData.vertexData.size() > vertexBufferSize)
                        {
                            if (vertexBuffer) vertexBuffer->Release();

                            vertexBufferSize = static_cast<UINT>(uploadData.vertexData.size());

                            D3D11_BUFFER_DESC vertexBufferDesc;
                            memset(&vertexBufferDesc, 0, sizeof(vertexBufferDesc));

                            vertexBufferDesc.ByteWidth = vertexBufferSize;
                            vertexBufferDesc.Usage = uploadData.dynamicVertexBuffer ? D3D11_USAGE_DYNAMIC : D3D11_USAGE_DEFAULT;
                            vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
                            vertexBufferDesc.CPUAccessFlags = uploadData.dynamicVertexBuffer ? D3D11_CPU_ACCESS_WRITE : 0;

                            D3D11_SUBRESOURCE_DATA vertexBufferResourceData;
                            memset(&vertexBufferResourceData, 0, sizeof(vertexBufferResourceData));
                            vertexBufferResourceData.pSysMem = uploadData.vertexData.data();

                            HRESULT hr = rendererD3D11->getDevice()->CreateBuffer(&vertexBufferDesc, &vertexBufferResourceData, &vertexBuffer);
                            if (FAILED(hr))
                            {
                                log(LOG_LEVEL_ERROR, "Failed to create Direct3D 11 vertex buffer");
                                return false;
                            }
                        }
                        else
                        {
                            if (!uploadBuffer(vertexBuffer, uploadData.vertexData))
                            {
                                return false;
                            }
                        }
                    }

                    dirty &= ~VERTEX_BUFFER_DIRTY;
                }

                dirty = 0;
                ready = (indexBuffer && vertexBuffer);
            }

            return true;
        }
    } // namespace graphics
} // namespace ouzel
