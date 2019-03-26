#pragma once
#include <d3d11.h>
#include <array>
#include "System/logicsafety.h"
#include "System/typesalias.h"
#include "System/hashutils.h"

template<class T>
class DX11Object
{
public:
    DX11Object()
    {
		Set(nullptr);
	}
    DX11Object(T* object)
    {
		Set(object);
	}
    DX11Object(const DX11Object& object)
    {
        if (object.Get())
            object.Get()->AddRef();
        if (m_DX11Object && IsValid())
            m_DX11Object->Release();
        Set(object.Get());
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
        Set(object.Get());

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
		InitializeDX11ObjectPointerHash();
    }

    void Release()
    {
        if (m_DX11Object)
            m_DX11Object->Release();
		Set(nullptr);
    }

private:
	unsigned long CalcDX11ObjectPointerHash()
	{
		unsigned long dx11ObjectPointerAsInt = (unsigned long)m_DX11Object;
		return FNV((char*)&dx11ObjectPointerAsInt, sizeof(dx11ObjectPointerAsInt) / sizeof(char));
	}
	void InitializeDX11ObjectPointerHash()
	{
		m_DX11ObjectPointerHash = CalcDX11ObjectPointerHash();
	}
	bool IsValid()
	{
		return m_DX11ObjectPointerHash == CalcDX11ObjectPointerHash();
	}

    T* m_DX11Object;
	unsigned long m_DX11ObjectPointerHash;
};

/*template<class T>
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
};*/

template<class T, size_t num>
class DX11MultipleObjectsHolder
{
public:
    DX11MultipleObjectsHolder()
    {}

	DX11MultipleObjectsHolder(const std::array<T*, num>& objs)
	{
		for (unsigned long i = 0; i < num; i++)
		{
			m_DX11Objects[i].Set(objs[i]);
			objs[i]->AddRef();
		}
	}
    //DX11ObjectHolder(T* obj) : m_DX11Object(obj)
    //{
    //    m_DX11Object.Get()->AddRef();
    //}

    T* GetDX11Object(index_t index = 0) const
    {
        popAssert(index < num);
        return m_DX11Objects[index].Get();
    }
    void SetDX11Object(T* object, index_t index = 0)
    {
        popAssert(index < num);
        m_DX11Objects[index].Set(object);
    }

    void ReleaseDX11Object(index_t index = 0)
    {
        m_DX11Objects[index].Release();
    }

	void ReleaseDX11Objects()
	{
		for (unsigned long i = 0; i < m_DX11Objects.size(); i++)
			m_DX11Objects[i].Release();
	}
protected:
    T*& GetDX11ObjectReference(index_t index = 0)
    {
        return m_DX11Objects[index].GetRef();
    }
    std::array<DX11Object<T>, num> m_DX11Objects;
};

template<class T>
using DX11ObjectHolder = DX11MultipleObjectsHolder<T, 1>;

template<class T>
class DX11ObjectWithNameHolder : public DX11ObjectHolder<T>
{
public:
    DX11ObjectWithNameHolder() : DX11ObjectHolder<T>() {}
	DX11ObjectWithNameHolder(T* obj) : DX11ObjectHolder<T>({ obj }) {}

    void SetDebugName(const std::string& name)
    {
        popAssert(GetDX11Object());
        GetDX11Object()->SetPrivateData(WKPDID_D3DDebugObjectName, name.size(), name.c_str());
    }
};

#ifdef _DEBUG
typedef DX11ObjectWithNameHolder<ID3D11Resource> D3D11ResourceHolder;
#else
typedef DX11ObjectHolder<ID3D11Resource> D3D11ResourceHolder;
#endif
typedef DX11ObjectHolder<IUnknown> DX11UknownHolder;