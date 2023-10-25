#ifndef CWW_ALLOCATOR_SORTED_LIST_H
#define CWW_ALLOCATOR_SORTED_LIST_H
#include "memory_global_heap.h"
#include <list>
using namespace std;

class allocator_sorted_list final : public memory
{

private:

	void* _memory;

public:
    enum class status {
        first_available,
        best_available,
        worst_available
    };

private:
    //base ch-s
    void** get_pointer_first() const {
        return reinterpret_cast<void**>(reinterpret_cast<size_t*>(reinterpret_cast<status*>(reinterpret_cast<memory**>(reinterpret_cast<logger**>(_memory) + 1) + 1) + 1) + 1);
    }

    size_t* get_size() const {
        return reinterpret_cast<size_t*>(reinterpret_cast<status*>(reinterpret_cast<memory**>(reinterpret_cast<logger**>(_memory) + 1) + 1) + 1);
    }

    status* get_status() const {
        return reinterpret_cast<status*>(reinterpret_cast<memory**>(reinterpret_cast<logger**>(_memory) + 1) + 1);
    }

    memory** get_allocator() const {
        return reinterpret_cast<memory**>(reinterpret_cast<logger**>(_memory) + 1);
    }

    logger** get_logger() const {
        return reinterpret_cast<logger**>(_memory);
    }

    //individual ch-s

    size_t* sizeof_block(void* block) const {
        return reinterpret_cast<size_t*>(block);
    }

    void** get_pointer_block(void* block) const {
        return reinterpret_cast<void**>(reinterpret_cast<size_t*>(block) + 1);
    }

public:

    allocator_sorted_list(size_t size, status state = status::first_available, logger* logged = nullptr, memory* allocated = nullptr) {

        if (logged != nullptr) {
            logged->log("allocator_sorted_list: Calling for constructor.", logger::severity::debug);
        }

        if (allocated == nullptr) {
            _memory = ::operator new(size + sizeof(void*) + sizeof(status) + sizeof(logger*) + sizeof(memory*) + sizeof(size_t));
        }
        else {
            _memory = allocated->allocate(size + sizeof(void*) + sizeof(status) + sizeof(logger*) + sizeof(memory*) + sizeof(size_t));
        }

        *reinterpret_cast<logger**>(_memory) = logged;
        *reinterpret_cast<memory**>(reinterpret_cast<logger**>(_memory) + 1) = allocated;
        *reinterpret_cast<status*>(reinterpret_cast<memory**>(reinterpret_cast<logger**>(_memory) + 1) + 1) = state;
        *reinterpret_cast<size_t*>(reinterpret_cast<status*>(reinterpret_cast<memory**>(reinterpret_cast<logger**>(_memory) + 1) + 1) + 1) = size;
        
        void* block = reinterpret_cast<void*>(reinterpret_cast<void**>(reinterpret_cast<size_t*>(reinterpret_cast<status*>(reinterpret_cast<memory**>(reinterpret_cast<logger**>(_memory) + 1) + 1) + 1) + 1) + 1);

        *reinterpret_cast<void**>(reinterpret_cast<size_t*>(reinterpret_cast<status*>(reinterpret_cast<memory**>(reinterpret_cast<logger**>(_memory) + 1) + 1) + 1) + 1) = block;
        *reinterpret_cast<size_t*>(block) = size - sizeof(size_t) - sizeof(void*) - sizeof(memory*) - sizeof(logger*);
        *reinterpret_cast<void**>(reinterpret_cast<size_t*>(block) + 1) = nullptr;
    }

public:

    ~allocator_sorted_list() {

        logger* logged = *get_logger();
        memory* allocated = *get_allocator();

        if (logged != nullptr)
        {
            logged->log("allocator_sorted_list: Calling for destructor.", logger::severity::debug);
        }

        if (allocated != nullptr) {
            allocated->deallocate(_memory);
        }
        else {
            ::operator delete(_memory);
        }
    }

private:

    void merge() const {

        logger* logged = *get_logger();

        if (logged != nullptr) {
            logged->log("allocator_sorted_list: Calling for merge process.", logger::severity::debug);
        }

        void* pointer_current_block = *get_pointer_first();
        void* pointer_next_block = nullptr;

        while (pointer_current_block != nullptr) {
            pointer_next_block = *get_pointer_block(pointer_current_block);

            if (reinterpret_cast<unsigned char*>(pointer_next_block) - reinterpret_cast<unsigned char*>(pointer_current_block) == (*sizeof_block(pointer_current_block))) {
                
                size_t* size_keep = reinterpret_cast<size_t*>(pointer_next_block);
                void** next_keep = reinterpret_cast<void**>(size_keep + 1);

                *get_pointer_block(pointer_current_block) = *next_keep;
                size_t current_size_keep = *sizeof_block(pointer_current_block);
                *sizeof_block(pointer_current_block) = current_size_keep + (*sizeof_block(pointer_next_block));

                *get_pointer_block(pointer_next_block) = nullptr;

                *next_keep = nullptr;
                *size_keep = 0;
            }

            else {
                pointer_current_block = pointer_next_block;
            }
        }

        if (logged != nullptr) {
            logged->log("allocator_sorted_list: Merge process has been finished.",logger::severity::debug);
        }

    }

    public:

    void* allocate(size_t requested_size) const override {

        logger* logged = *get_logger();

        size_t sizeof_allocated = sizeof(void*) + sizeof(size_t) + requested_size;

        void* allocated_block = nullptr;
        void* previous_block = nullptr;
        void* ex_previous_block;
        void* current_block = *get_pointer_first();

        if (*get_status() == status::first_available) {

            while (current_block != nullptr) {

                if (sizeof_allocated <= (*sizeof_block(current_block))) {

                    allocated_block = current_block;
                    ex_previous_block = previous_block;
                    break;
                }

                previous_block = current_block;
                current_block = *get_pointer_block(current_block);
            }
        }

        else if (*get_status() == status::best_available) {

            size_t minimal_size = *get_size();

            while (current_block != nullptr) {

                if ((sizeof_allocated <= *sizeof_block(current_block)) && (*sizeof_block(current_block) < minimal_size)) {

                    allocated_block = current_block;
                    ex_previous_block = previous_block;
                    minimal_size = *sizeof_block(current_block);
                }

                previous_block = current_block;
                current_block = *get_pointer_block(current_block);
            }
        }

        else if (*get_status() == status::worst_available) {

            size_t maximum_size = 0;

            while (current_block != nullptr) {

                if ((sizeof_allocated <= *sizeof_block(current_block)) && (*sizeof_block(current_block) > maximum_size)) {

                    allocated_block = current_block;
                    ex_previous_block = previous_block;
                    maximum_size = *sizeof_block(current_block);
                }

                previous_block = current_block;
                current_block = *get_pointer_block(current_block);
            }
        }
       
        if (allocated_block == nullptr) {
            throw std::bad_alloc();
        }
        
        size_t allocation_result_size = *sizeof_block(allocated_block);


        if (ex_previous_block == nullptr) {

            if (allocation_result_size - sizeof_allocated < sizeof(void*) + sizeof(size_t)) {

                *get_pointer_first() = *get_pointer_block(allocated_block);
            }

            else {

                void* next_block = reinterpret_cast<void*>(reinterpret_cast<unsigned char*>(allocated_block) + sizeof_allocated);

                *sizeof_block(allocated_block) = requested_size;
                *sizeof_block(next_block) = allocation_result_size - sizeof_allocated;
                *get_pointer_block(next_block) = *get_pointer_block(allocated_block);
                *get_pointer_first() = next_block;
            }
            *get_pointer_block(allocated_block) = nullptr;
        }

        else {
            if (allocation_result_size - sizeof_allocated < sizeof(void*) + sizeof(size_t)) {

                *get_pointer_block(ex_previous_block) = *get_pointer_block(allocated_block);
            }

            else {

                void* next_block = reinterpret_cast<void*>(reinterpret_cast<unsigned char*>(allocated_block) + sizeof_allocated);

                *sizeof_block(allocated_block) = requested_size;
                *sizeof_block(next_block) = allocation_result_size - sizeof_allocated;
                *get_pointer_block(next_block) = *get_pointer_block(allocated_block);
                *get_pointer_block(ex_previous_block) = next_block;
            }
            *get_pointer_block(allocated_block) = nullptr;
        }
        
        if (logged != nullptr) {

            logged->log("allocator_sorted_list: Allocation finished successfully.", logger::severity::debug);
        }

        return reinterpret_cast<void*>(reinterpret_cast<size_t*>(allocated_block) + 1);
    }

    public:

        void deallocate(void* allocation_result) const override {

            logger* logged = *get_logger();

            if (allocation_result == nullptr) {

                return;
            }

            size_t* block_got = reinterpret_cast<size_t*>(allocation_result) - 1;
            void* cur_block = *get_pointer_first();
            void* prev_block = nullptr;

            size_t size_block = *sizeof_block(block_got);

            while ((reinterpret_cast<unsigned char*>(block_got) - reinterpret_cast<unsigned char*>(cur_block) > 0) && cur_block != nullptr) {

                prev_block = cur_block;
                cur_block = *get_pointer_block(cur_block);
            }

            if (prev_block == nullptr) {

                *get_pointer_first() = block_got;
                *get_pointer_block(block_got) = cur_block;
            }

            else {

                *get_pointer_block(prev_block) = block_got;
                *get_pointer_block(block_got) = cur_block;
            }

            merge();

            if (logged != nullptr) {

                logged->log("allocator_sorted_list: Deallocation finished successfully.", logger::severity::debug);
            }
        }
};

































































#endif //CWW_ALLOCATOR_SORTED_LIST_H