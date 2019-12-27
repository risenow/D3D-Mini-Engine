#pragma once
#include <array>
#include <memory>
#include "System/Camera.h"
#include "Graphics/GraphicsDevice.h"
#include "Graphics/ShadersCollection.h"
#include "Graphics/GraphicsObjectHandler.h"
#include "Graphics/GraphicsObjectManager.h"
#include "Graphics/SerializableGraphicsObject.h"
#include "Graphics/GraphicsTextureCollection.h"
#include "Graphics/LightingMaterial.h"
#include "Extern/tiny_obj_loader.h"
#include <unordered_map>
#include <set>

//SHOULD I EVEN BOTHER

bool HandleTexture(GraphicsDevice&, GraphicsTextureCollection&, ShadersCollection&, GraphicsMaterialsManager*, tinyxml2::XMLElement*);
bool HandleObj(GraphicsDevice&, GraphicsTextureCollection&, ShadersCollection&, GraphicsMaterialsManager*, tinyxml2::XMLElement*);
bool HandleMaterial(GraphicsDevice&, GraphicsTextureCollection&, ShadersCollection&, GraphicsMaterialsManager*, tinyxml2::XMLElement*);

//do we need to init obj materials here?

//mb release not necessary data?
//load resources async in the future
typedef bool(*GraphicsResourceHandleFunc)(GraphicsDevice&, GraphicsTextureCollection&, ShadersCollection&, GraphicsMaterialsManager*, tinyxml2::XMLElement*);
class GraphicsResourceManager : public GraphicsObjectManager
{
public:
    GraphicsResourceManager() {}

    virtual HandleResult Handle(GraphicsDevice& device, GraphicsTextureCollection& textureCollection, ShadersCollection& shadersCollection, GraphicsMaterialsManager* materialsManager, tinyxml2::XMLElement* sceneGraphElement) override
    {
        for (GraphicsResourceHandleFunc func : m_Handlers)
            func(device, textureCollection, shadersCollection, materialsManager, sceneGraphElement);
    }
private:
    std::vector<GraphicsResourceHandleFunc> m_Handlers;
};