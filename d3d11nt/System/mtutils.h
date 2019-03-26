#pragma once

#include <mutex>

class DynamicConditionalLockGuard
{
public:
	DynamicConditionalLockGuard(std::mutex& mutex, bool enabled) : m_Mutex(mutex), m_Enabled(enabled)
	{
		if (m_Enabled)
		{
			m_Mutex.lock();
		}
	}

	~DynamicConditionalLockGuard()
	{
		if (m_Enabled)
		{
			m_Mutex.unlock();
		}
	}
private:
	bool m_Enabled;
	std::mutex& m_Mutex;
};