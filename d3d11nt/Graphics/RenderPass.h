#pragma once
#include "GraphicsDevice.h"
#include "SceneGraph.h"
#include "ShadersCollection.h"
#include "GraphicsTextureCollection.h"
#include <set>
#include <vector>
#include <unordered_set>

class RenderPass;

class PassResource
{
public:
    PassResource(void* resource = nullptr, RenderPass* dep = nullptr) : m_Resource(resource), m_Dependency(dep) {}

    void* m_Resource;
    RenderPass* m_Dependency;
};

class RenderPass
{
public:
    RenderPass() {}
    RenderPass(const std::vector<PassResource*>& inputResources, const std::vector<PassResource*>& outputResources, const std::string& name) : m_InputResources(inputResources), m_OutputResources(outputResources), m_Name(name)
    {
        m_Dependencies = GetDependencies();
    }
    std::vector <RenderPass*> GetDependencies()
    {
        std::vector<RenderPass*> list;
        for (size_t i = 0; i < m_InputResources.size(); i++)
        {
            if (m_InputResources[i]->m_Dependency != nullptr)
                list.push_back(m_InputResources[i]->m_Dependency);
            std::vector<RenderPass*> list2 = m_InputResources[i]->m_Dependency->GetDependencies();
            list.insert(list.end(), list2.begin(), list2.end());
        }

        return list;
    }
    bool IsDependentOn(RenderPass* pass)
    {
        if (!pass)
            return false;
        for (RenderPass* curPass : m_Dependencies)
            if (curPass == pass)
                return true;
        return false;
    }
    void InitializeOutputDependencies()
    {
        for (PassResource* res : m_OutputResources)
            res->m_Dependency = this;
    }

    //use data for debug purpose only(e.g. use to pass console variables to render pass)
    virtual void Render(GraphicsDevice& device, SceneGraph& sceneGraph, ShadersCollection& shadersCollection, GraphicsTextureCollection& textureCollection, Camera& camera, GraphicsOptions& options, void* data) = 0;
    //size_t m_ID;
//private:
    std::string m_Name;
    std::vector<RenderPass*> m_Dependencies;
    std::vector<PassResource*> m_InputResources;
    std::vector<PassResource*> m_OutputResources;
};

//not sure if it is correct
std::vector<RenderPass*> BuildPassList(PassResource* outputResource)
{
    bool dep = true;
    std::unordered_set<RenderPass*> alreadyInList;
    std::vector<RenderPass*> passList = { outputResource->m_Dependency };
    size_t curIdx = 0;
    RenderPass* currentPass = outputResource->m_Dependency;
    
    while (dep)
    {
        size_t insertAfter = 0;
        for (size_t i = 0; i < currentPass->m_InputResources.size(); i++)
        {
            for (size_t j = 0; j < passList.size(); j++)
            {

                if (passList[j]->IsDependentOn(currentPass->m_InputResources[i]->m_Dependency))
                {
                    if (alreadyInList.find(currentPass->m_InputResources[i]->m_Dependency) != alreadyInList.end())
                        continue;
                    passList.insert(passList.begin() + i, currentPass->m_InputResources[i]->m_Dependency);
                    alreadyInList.insert(currentPass->m_InputResources[i]->m_Dependency);
                }
            }
        }
        curIdx++;
        if (curIdx < passList.size())
            currentPass = passList[curIdx];
        else
            break;
    }
    return passList;
}