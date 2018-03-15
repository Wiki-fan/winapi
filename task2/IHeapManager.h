#pragma once
#include <exception>
#include <string>

struct HeapException : public std::exception {
	HeapException( const std::string _descr )
	{
		descr = _descr;
	}

	const char* what() const noexcept
	{
		return descr.c_str();
	}
	std::string descr;
};

class IHeapManager {
public:
	virtual void Create( int minSize, int maxSize ) = 0;
	virtual void* Alloc( int size ) = 0;
	virtual void Free( void* mem ) = 0;
	virtual void Destroy() = 0; // ¬озвращает количество зан€тых блоков (0, если таковых нет, и деаллокаци€ успешна).
};
