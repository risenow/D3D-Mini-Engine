#pragma once
#include "Graphics/Texture2D.h"
#include "Graphics/GraphicsDevice.h"
#define cimg_use_png
#define cimg_use_jpeg
//#include "Cimg.h"
#include <map>
#include <memory>
#include "System/helperutil.h"

class  GraphicsTextureCollection : public std::map<std::string, std::shared_ptr<Texture2D>>
{
public:
    GraphicsTextureCollection() {}
    ~GraphicsTextureCollection() {}

    void Add(GraphicsDevice& device, const std::string& file, DXGI_FORMAT format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB);

  private:

};
