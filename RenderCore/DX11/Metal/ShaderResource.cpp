// Copyright 2015 XLGAMES Inc.
//
// Distributed under the MIT License (See
// accompanying file "LICENSE" or the website
// http://www.opensource.org/licenses/mit-license.php)

#include "ShaderResource.h"
#include "DeviceContext.h"
#include "../../Resource.h"
#include "DX11Utils.h"

namespace RenderCore { namespace Metal_DX11
{

    ShaderResourceView::ShaderResourceView(UnderlyingResource resource, NativeFormat::Enum format, int arrayCount, bool forceSingleSample)
    {
        if (resource) {
            intrusive_ptr<ID3D::ShaderResourceView> srv;
            if (format == NativeFormat::Unknown) {
                srv = ObjectFactory(*resource).CreateShaderResourceView(resource);
            } else {
                D3D11_SHADER_RESOURCE_VIEW_DESC viewDesc;
                viewDesc.Format = AsDXGIFormat(format);

                TextureDesc2D textureDesc(resource);
                if (textureDesc.Width > 0) {
                    if (arrayCount == 0) {
                        if (textureDesc.MiscFlags & D3D11_RESOURCE_MISC_TEXTURECUBE) {
                            viewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;
                            viewDesc.TextureCube.MostDetailedMip = 0;
                            viewDesc.TextureCube.MipLevels = (UINT)-1;
                        } else if (textureDesc.ArraySize > 1) {
                            viewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
                            viewDesc.Texture2DArray.MostDetailedMip = 0;
                            viewDesc.Texture2DArray.MipLevels = (UINT)-1;
                            viewDesc.Texture2DArray.FirstArraySlice = 0;
                            viewDesc.Texture2DArray.ArraySize = textureDesc.ArraySize;
                        } else {
                            if (textureDesc.SampleDesc.Count > 1 && !forceSingleSample) {
                                viewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DMS;
                            } else {
                                viewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
                                viewDesc.Texture2D.MostDetailedMip = 0;
                                viewDesc.Texture2D.MipLevels = (UINT)-1;
                            }
                        }
                    } else {
                        if (textureDesc.SampleDesc.Count > 1 && !forceSingleSample) {
                            viewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DMSARRAY;
                            viewDesc.Texture2DArray.FirstArraySlice = 0;
                            viewDesc.Texture2DArray.ArraySize = arrayCount;
                        } else {
                            viewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
                            viewDesc.Texture2DArray.MostDetailedMip = 0;
                            viewDesc.Texture2DArray.MipLevels = (UINT)-1;
                            viewDesc.Texture2DArray.FirstArraySlice = 0;
                            viewDesc.Texture2DArray.ArraySize = arrayCount;
                        }
                    }
                } else {

                    TextureDesc3D textureDesc(resource);
                    if (textureDesc.Width > 0) {
                        viewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE3D;
                        viewDesc.Texture3D.MostDetailedMip = 0;
                        viewDesc.Texture3D.MipLevels = (UINT)-1;
                    } else {

                        TextureDesc1D textureDesc(resource);
                        if (textureDesc.Width > 0) {
                            viewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE1D;
                            viewDesc.Texture1D.MostDetailedMip = 0;
                            viewDesc.Texture1D.MipLevels = (UINT)-1;
                        } else {
                            D3DBufferDesc bufferDesc(resource);
                            viewDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFEREX;
                            viewDesc.BufferEx.FirstElement = 0;
                            viewDesc.BufferEx.NumElements = bufferDesc.StructureByteStride ? (bufferDesc.ByteWidth / bufferDesc.StructureByteStride) : (bufferDesc.ByteWidth/4);
                            viewDesc.BufferEx.Flags = D3D11_BUFFEREX_SRV_FLAG_RAW;
                        }
                    }

                }

                srv = ObjectFactory(*resource).CreateShaderResourceView(resource, &viewDesc);
            }
            _underlying = std::move(srv);
        }
    }

    ShaderResourceView::ShaderResourceView(UnderlyingResource resource, NativeFormat::Enum format, const MipSlice& mipSlice)
    {
        intrusive_ptr<ID3D::ShaderResourceView> srv;
        if (format == NativeFormat::Unknown) {
            srv = ObjectFactory(*resource).CreateShaderResourceView(resource);
        } else {
            D3D11_SHADER_RESOURCE_VIEW_DESC viewDesc;
            viewDesc.Format = AsDXGIFormat(format);

            TextureDesc2D textureDesc(resource);
            if (textureDesc.Width > 0) {
                if (textureDesc.ArraySize > 1) {
                    viewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
                    viewDesc.Texture2DArray.MostDetailedMip = mipSlice._mostDetailedMip;
                    viewDesc.Texture2DArray.MipLevels = mipSlice._mipLevels;
                    viewDesc.Texture2DArray.FirstArraySlice = 0;
                    viewDesc.Texture2DArray.ArraySize = textureDesc.ArraySize;
                } else {
                    viewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
                    viewDesc.Texture2D.MostDetailedMip = mipSlice._mostDetailedMip;
                    viewDesc.Texture2D.MipLevels = mipSlice._mipLevels;
                }
            } else {
                assert(0);
            }

            srv = ObjectFactory(*resource).CreateShaderResourceView(resource, &viewDesc);
        }
        _underlying = std::move(srv);
    }

    ShaderResourceView ShaderResourceView::RawBuffer(UnderlyingResource res, unsigned sizeBytes, unsigned offsetBytes)
    {
        D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
        srvDesc.Format = DXGI_FORMAT_R32_TYPELESS;
        srvDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFEREX;
        srvDesc.BufferEx.FirstElement = offsetBytes / 4;
        srvDesc.BufferEx.NumElements = sizeBytes / 4;
        srvDesc.BufferEx.Flags = D3D11_BUFFEREX_SRV_FLAG_RAW;
        return ShaderResourceView(
            ObjectFactory(*res).CreateShaderResourceView(res, &srvDesc));
    }

    auto ShaderResourceView::GetResource() const -> intrusive_ptr<ID3D::Resource>
    {
        return ExtractResource<ID3D::Resource>(_underlying.get());
    }

    ShaderResourceView::ShaderResourceView(intrusive_ptr<ID3D::ShaderResourceView>&& resource)
    : _underlying(resource)
    {
    }

    ShaderResourceView::ShaderResourceView(MovePTRHelper<ID3D::ShaderResourceView> resource)
    : _underlying(resource)
    {
    }

    ShaderResourceView::ShaderResourceView() {}

    ShaderResourceView::~ShaderResourceView() {}

    ShaderResourceView::ShaderResourceView(const ShaderResourceView& cloneFrom) : _underlying(cloneFrom._underlying) {}
    ShaderResourceView& ShaderResourceView::operator=(const ShaderResourceView& cloneFrom) { _underlying = cloneFrom._underlying; return *this; }

    ShaderResourceView::ShaderResourceView(ShaderResourceView&& moveFrom) never_throws : _underlying(std::move(moveFrom._underlying)) {}
    ShaderResourceView& ShaderResourceView::operator=(ShaderResourceView&& moveFrom) never_throws { _underlying = std::move(moveFrom._underlying); return *this; }

}}

