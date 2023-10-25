#ifndef CWW_ALLOCATOR_BUDDIES_H
#define CWW_ALLOCATOR_BUDDIES_H
#include "memory_global_heap.h"
#include <cmath>
using namespace std;

class allocator_buddies final: public memory
{

private:
	void* _memory;

private:

	//ind ch-s

	static unsigned char* sizeof_block(void* block) {
		return reinterpret_cast<unsigned char*>(block);
	}

	static void** get_pointer_next(void* block) {
		return reinterpret_cast<void**>(reinterpret_cast<unsigned char*>(block) + 1);
	}

	static void** get_pointer_previous(void* block) {
		return reinterpret_cast<void**>(reinterpret_cast<void**>(reinterpret_cast<unsigned char*>(block) + 1) + 1);
	}

	//base ch-s

	int* get_size() const {
		return reinterpret_cast<int*>(_memory);
	}

	logger** get_logger() const {
		return reinterpret_cast<logger**>(reinterpret_cast<int*>(_memory) + 1);
	}

	memory** get_allocator() const {
		return reinterpret_cast<memory**>(reinterpret_cast<logger**>(reinterpret_cast<int*>(_memory) + 1) + 1);
	}

	void** get_pointer_first() const {
		return reinterpret_cast<void**>(reinterpret_cast<memory**>(reinterpret_cast<logger**>(reinterpret_cast<int*>(_memory) + 1) + 1) + 1);
	}

private:
	//methods to work with memory based on buddies algorithm

	static bool if_occupied(void* pb) {

		if ((static_cast<int>(*sizeof_block(pb)) & 1) == 0) {

			return true;
		}

		return false;
	}

	static void change_occupation_status(void* pb) {

		if(if_occupied(pb)){

			*reinterpret_cast<unsigned char*>(pb) = static_cast<unsigned char>(static_cast<int>(*reinterpret_cast<unsigned char*>(pb)) | 1);
		}

		else {

			*reinterpret_cast<unsigned char*>(pb) = static_cast<unsigned char>((static_cast<int>(*reinterpret_cast<unsigned char*>(pb)) >> 1) << 1);
		}
	}

	void* get_buddies(void* pb) const {

		size_t degree = static_cast<int>(*sizeof_block(pb) >> 1);

		if (degree = *get_size()) { return nullptr; }

		auto* const first_allocated = get_pointer_first() + 1;
		size_t size = 1 << degree;

		size_t pointer_reflected_block = reinterpret_cast<unsigned char*>(pb) - reinterpret_cast<unsigned char*>(first_allocated);
		size_t pointer_both_buddies = pointer_reflected_block ^ size;

		return reinterpret_cast<void*>(reinterpret_cast<unsigned char*>(first_allocated) + pointer_both_buddies);
	}

public:

	explicit allocator_buddies(size_t size, logger* logged = nullptr, memory* allocated = nullptr) {

		int degree = static_cast<int>(ceil(log2(size)));

		if (logged != nullptr) {

			logged->log("allocator_buddies: Welcome to the constructor.", logger::severity::debug);
		}

		if (allocated == nullptr) {

			_memory = ::operator new ((1 << degree) + sizeof(int) + sizeof(void*) + sizeof(logger*) + sizeof(memory*));
		}

		else {

			_memory = allocated->allocate((1 << degree) + sizeof(int) + sizeof(void*) + sizeof(logger*) + sizeof(memory*));
		}

		if (logged != nullptr) {

			logged->log("memory: " + to_string(1 << degree), logger::severity::debug);
		}

		*reinterpret_cast<int*>(_memory) = degree;
		*reinterpret_cast<logger**>(reinterpret_cast<int*>(_memory) + 1) = logged;
		*reinterpret_cast<memory**>(reinterpret_cast<logger**>(reinterpret_cast<int*>(_memory) + 1) + 1) = allocated;

		auto* block = reinterpret_cast<void**>(reinterpret_cast<memory**>(reinterpret_cast<logger**>(reinterpret_cast<int*>(_memory) + 1) + 1) + 1) + 1;

		*reinterpret_cast<void**>(reinterpret_cast<memory**>(reinterpret_cast<logger**>(reinterpret_cast<int*>(_memory) + 1) + 1) + 1) = block;
		*reinterpret_cast<unsigned char*>(block) = degree * 2;
		*reinterpret_cast<void**>(reinterpret_cast<unsigned char*>(block) + 1) = nullptr;
		*reinterpret_cast<void**>(reinterpret_cast<void**>(reinterpret_cast<unsigned char*>(block) + 1) + 1) = nullptr;

	}

	public:

	~allocator_buddies() override {

		logger* logged = *get_logger();
		memory* allocated = *get_allocator();

		if (logged != nullptr) {

			logged->log("allocator buddies: Welcome to the destructor.", logger::severity::debug);
		}

		if (allocated == nullptr) {

			::operator delete(_memory);
		}

		else {

			allocated->deallocate(_memory);
		}
	}

	public:

		void* allocate(size_t requested_size) const override {

			logger* logged = *get_logger();

			size_t real_size = requested_size + sizeof(unsigned char) + (sizeof(void*) << 1);

			auto* pointer_block = *get_pointer_first();

			while (pointer_block != nullptr) {

				int degree = static_cast<int>((*sizeof_block(pointer_block)) >> 1);

				if (real_size <= (1 << degree)) {

					auto* previous_block = *get_pointer_previous(pointer_block);
					auto* next_block = *get_pointer_next(pointer_block);

					while (real_size < (1 << degree) >> 1) {

						degree--;

						void* buddy = reinterpret_cast<void*>(reinterpret_cast<unsigned char*>(pointer_block) + (1 << degree));

						if (next_block != nullptr) {

							*get_pointer_previous(next_block) = buddy;
						}

						*sizeof_block(buddy) = degree << 1;
						*get_pointer_previous(buddy) = reinterpret_cast<void*>(pointer_block);
						*get_pointer_next(buddy) = reinterpret_cast<void*>(next_block);

						*sizeof_block(pointer_block) = degree << 1;
						*get_pointer_next(pointer_block) = buddy;

						next_block = buddy;
					}

					change_occupation_status(pointer_block);

					if (previous_block == nullptr) {

						*get_pointer_first() = next_block;
					}

					else {

						*get_pointer_next(previous_block) = next_block;
					}

					if (next_block != nullptr) {

						*get_pointer_previous(next_block) = previous_block;
					}

					if (logged != nullptr) {

						logged->log("allocator_buddies: Allocation finished successfully!", logger::severity::debug);
					}

					return sizeof_block(pointer_block) + 1;
				}

				pointer_block = *get_pointer_next(pointer_block);
			}

			if (logged != nullptr) {

				logged->log("allocator_buddies: Unable to allocate requested size of memory.", logger::severity::debug);
			}

			return nullptr;
		}

		void deallocate(void* result) const override {

			logger* logged = *get_logger();

			if (result == nullptr) {

				return;
			}

			auto* pointer_block = reinterpret_cast<void*>(reinterpret_cast<unsigned char*>(result) - 1);

			size_t size = static_cast<int>((*sizeof_block(pointer_block)) >> 1);

			void* next_block = *get_pointer_first();
			void* previous_block = nullptr;

			while (next_block != nullptr && reinterpret_cast<unsigned char*>(pointer_block) - reinterpret_cast<unsigned char*>(next_block) > 0) {

				previous_block = next_block;
				next_block = *get_pointer_next(next_block);
			}

			change_occupation_status(pointer_block);

			*get_pointer_next(pointer_block) = next_block;
			*get_pointer_previous(pointer_block) = previous_block;

			if (previous_block == nullptr) {

				*get_pointer_first() = pointer_block;
			}

			else {

				*get_pointer_next(previous_block) = pointer_block;
			}

			if (next_block != nullptr) {

				*get_pointer_previous(next_block) = pointer_block;
			}

			void* buddy = get_buddies(pointer_block);

			while (buddy != nullptr && if_occupied(buddy) && static_cast<int>((*sizeof_block(pointer_block)) >> 1) == static_cast<int>((*sizeof_block(buddy)) >> 1)) {

				if (reinterpret_cast<unsigned char*>(buddy) - reinterpret_cast<unsigned char*>(pointer_block) < 0) {

					auto* chg = pointer_block;
					pointer_block = buddy;
					buddy = chg;
				}

				*get_pointer_next(pointer_block) = *get_pointer_next(buddy);
				*sizeof_block(pointer_block) = (static_cast<int>((*sizeof_block(pointer_block)) >> 1) + 1) << 1;

				if (*get_pointer_next(buddy) != nullptr) {

					*get_pointer_previous(*get_pointer_next(buddy)) = pointer_block;
				}

				buddy = get_buddies(pointer_block);
			}

			if (logged != nullptr) {

				logged->log("allocator_buddies: Deallocation finished successfully!", logger::severity::debug);
			}
		}
};


#endif // CWW_ALLOCATOR_BUDDIES_H