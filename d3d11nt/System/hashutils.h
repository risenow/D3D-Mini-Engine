#pragma once

typedef size_t FNVhash_t;

//taken from std
inline FNVhash_t FNV(const char *_First, size_t _Count)
{	// FNV-1a hash function for bytes in [_First, _First + _Count)

	//static_assert(sizeof(size_t) == 4, "This code is for 32-bit size_t.");
	const size_t _FNV_offset_basis = 2166136261U;
	const size_t _FNV_prime = 16777619U;

	size_t _Val = _FNV_offset_basis;
	for (size_t _Next = 0; _Next < _Count; ++_Next)
	{	// fold in another byte
		_Val ^= (size_t)_First[_Next];
		_Val *= _FNV_prime;
	}
	return (_Val);
}