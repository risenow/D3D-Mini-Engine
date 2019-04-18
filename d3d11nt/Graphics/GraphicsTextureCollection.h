#pragma once
#include "Graphics/Texture2D.h"
#include "Graphics/GraphicsDevice.h"
#define cimg_use_png
#include "Cimg.h"
#include <map>
#include <memory>

class  GraphicsTextureCollection : public std::map<std::string, std::shared_ptr<Texture2D>>
{
public:
    GraphicsTextureCollection() {}
    ~GraphicsTextureCollection() {}

    void Add(GraphicsDevice& device, const std::string& file)
    {
        using namespace cimg_library;
        const CImg<unsigned char> img = CImg<>(file.c_str());
        DXGI_SAMPLE_DESC desc;
        desc.Count = 1;
        desc.Quality = 0;
        D3D11_SUBRESOURCE_DATA subr;
        subr.pSysMem = img.data();
        subr.SysMemPitch = img.width() * sizeof(char) * 4;
        subr.SysMemSlicePitch = img.width() * img.height() * sizeof(char) * 4;
        insert({ file, std::make_shared<Texture2D>(device, img.width(), img.height(), 1, 1, DXGI_FORMAT_R8G8B8A8_UINT, desc, D3D11_USAGE_DEFAULT, D3D11_BIND_SHADER_RESOURCE, 0, 0, (D3D11_SUBRESOURCE_DATA*)& subr) });
    }

private:
};
