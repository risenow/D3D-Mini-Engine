#pragma once
#include <d3d11.h>

template<class T>
class DX11Object
{
public:
    DX11Object() : m_DX11Object(nullptr)
    {}
    DX11Object(T* object) : m_DX11Object(object)
    {}
    DX11Object(DX11Object& object)
    {
        if (object.Get())
            object.Get()->AddRef();
        if (m_DX11Object)
            m_DX11Object->Release();
        m_DX11Object = object.Get();
    }

    ~DX11Object()
    {
        Release();
    }

    DX11Object& operator = (const DX11Object& object)
    {
        if (object.Get())
            object.Get()->AddRef();
        if (m_DX11Object)
            m_DX11Object->Release();
        m_DX11Object = object.Get();

        return *this;
    }

    T* Get() const
    {
        return m_DX11Object;
    }

    T*& GetRef()
    {
        return m_DX11Object;
    }

    void Set(T* obj)
    {
        m_DX11Object = obj;
    }

    void Release()
    {
        if (m_DX11Object)
            m_DX11Object->Release();
        m_DX11Object = nullptr;
    }
private:
    T* m_DX11Object;
};

template<class T>
class DX11ObjectHolder
{
public:
    DX11ObjectHolder()
    {}

    DX11ObjectHolder(T* obj) : m_DX11Object(obj)
    {
        m_DX11Object.Get()->AddRef();
    }

    T* GetDX11Object() const
    {
        return m_DX11Object.Get();
    }
    void SetDX11Object(T* object)
    {
        m_DX11Object.Set(object);
    }

    void ReleaseDX11Object()
    {
        m_DX11Object.Release();
    }
protected:
    T*& GetDX11ObjectReference() 
    {
        return m_DX11Object.GetRef();
    }
    DX11Object<T> m_DX11Object;
};

template<class T>
class DX11ObjectWithNameHolder : public DX11ObjectHolder<T>
{
public:
    DX11ObjectWithNameHolder() : DX11ObjectHolder<T>() {}
    DX11ObjectWithNameHolder(T* obj) : DX11ObjectHolder<T>(obj) {}

    void SetDebugName(const std::string& name)
    {
        popAssert(m_DX11Object.Get());
        m_DX11Object.Get()->SetPrivateData(WKPDID_D3DDebugObjectName, name.size(), name.c_str());
    }
};

#ifdef _DEBUG
typedef DX11ObjectWithNameHolder<ID3D11Resource> D3D11ResourceHolder;
#else
typedef DX11ObjectHolder<ID3D11Resource> D3D11ResourceHolder;
#endif