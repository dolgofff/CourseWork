#ifndef CWW_MEMORY_H
#define CWW_MEMORY_H
#include <iostream>
#include <string>
using namespace std;
class memory
{
public:
	virtual void* allocate(size_t target_size) const = 0;

	virtual void deallocate(void* target_to_deallocate) const = 0;

public:

	virtual ~memory() = default;
};

#endif //CWW_MEMORY_H
