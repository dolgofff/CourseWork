#ifndef CWW_MEMORY_GLOBAL_HEAP_H
#define CWW_MEMORY_GLOBAL_HEAP_H
#include <iostream>
#include <string>
#include <cstring>
#include <strstream>
#include <sstream>

#include "C:\Users\Sergey\source\repos\MyCourseWork\MyCourseWork\logger_concrete.h"
#include "C:\Users\Sergey\source\repos\MyCourseWork\MyCourseWork\memory.h"

using namespace std;
class memory_global_heap final : public memory
{
private:
	logger* logger_c;
public:
	memory_global_heap(logger* logger_c = nullptr)
	{
		this->logger_c = logger_c;
	};
public:
	void* allocate(size_t target_size) const override
	{
		auto* result = ::operator new(sizeof(size_t) + target_size);
		if (result == nullptr) {
			throw logic_error("Bad Alloc!");
		}
		*reinterpret_cast<size_t*>(result) = target_size;

		if (logger_c != nullptr)
		{
			ostringstream optr;
			optr << reinterpret_cast<size_t*>(result) + sizeof(size_t);
			logger_c->log(optr.str(), logger::severity::debug);
		}

		return reinterpret_cast<void*>(reinterpret_cast<size_t*>(result) + 1);
	}

	void deallocate(void* target_to_deallocate) const override
	{
		target_to_deallocate = reinterpret_cast<size_t*>(target_to_deallocate) - 1;

		if (logger_c != nullptr)
		{
			string rstr = "[ ";
			auto mem_size = *reinterpret_cast<size_t*>(target_to_deallocate);
			for (int i = 0; i < mem_size; ++i) {
				unsigned char r = *(reinterpret_cast<unsigned char*>(target_to_deallocate) + sizeof(size_t) + i);
				rstr = rstr + to_string(static_cast<unsigned short>(r)) + " ";
			}
			rstr = "Deallocated block: " + rstr + "]";
			ostringstream optr;
			optr << reinterpret_cast<size_t*>(target_to_deallocate) + sizeof(size_t);
			logger_c->log(optr.str(), logger::severity::debug)
				->log(rstr, logger::severity::debug);
		}

		::operator delete(target_to_deallocate);
	}
};

#endif //CWW_MEMORY_GLOBAL_HEAP_H
