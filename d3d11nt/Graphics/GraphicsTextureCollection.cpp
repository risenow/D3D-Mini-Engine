#include "stdafx.h"
#include "Graphics/GraphicsTextureCollection.h"
#include <algorithm>
//#define STB_IMAGE_IMPLEMENTATION
//#include "stb/stb_image.h"
#include "Extern/WICTextureLoader.h"
#include "Extern/DDSTextureLoader.h"

bool IsFileOfSupported2DFormat(const std::string& file, std::string& format)
{
    format.reserve(6);
    for (size_t i = file.size() - 1; (file[i] != '.') && (i >= 0); i--)
        format.push_back(file[i]);
    std::reverse(format.begin(), format.end());

    bool supported = false;
    if (format == "jpg" || format == "png")
        supported = true;

    return supported;
}

void GraphicsTextureCollection::Add(GraphicsDevice& device, const std::string& file, DXGI_FORMAT format)
{
    std::string sformat;
    if (IsFileOfSupported2DFormat(file, sformat))
    {
        ID3D11Resource* res;
        ID3D11ShaderResourceView* srv;
        DirectX::CreateWICTextureFromFile(device.GetD3D11Device(), strtowstr_fast(file).c_str(), (ID3D11Resource * *)& res, &srv);
        insert({ file, std::make_shared<Texture2D>((ID3D11Texture2D*)res, srv) });

        return;
    }

    if (sformat == "dds")
    {
        ID3D11Resource* res;
        ID3D11ShaderResourceView* srv;
        DirectX::CreateDDSTextureFromFile(
            device.GetD3D11Device(),
            strtowstr_fast(file).c_str(),
            (ID3D11Resource * *)& res,
            &srv);

        insert({ file, std::make_shared<Texture2D>((ID3D11Texture2D*)res, srv) });
    }
    /*bool supported = IsFileOfSupported2DFormat(file);

    //stbi__context context;
    int width, height, channels;
    //stbi_set_flip_vertically_on_load(true);
    unsigned char* image = stbi_load(file.c_str(),
        &width,
        &height,
        &channels,
        STBI_rgb_alpha);
    //using namespace cimg_library;
    DXGI_SAMPLE_DESC desc;
    desc.Count = 1;
    desc.Quality = 0;
    D3D11_SUBRESOURCE_DATA subr;
    subr.pSysMem = (void*)image;//img.data();
    subr.SysMemPitch = width * sizeof(char) *4;
    subr.SysMemSlicePitch = width * height * sizeof(char) * channels;
    insert({ file, std::make_shared<Texture2D>(device, width, height, 1, 1, format, desc, D3D11_USAGE_DEFAULT, D3D11_BIND_SHADER_RESOURCE, 0, 0, (D3D11_SUBRESOURCE_DATA*)& subr) });
    */
}