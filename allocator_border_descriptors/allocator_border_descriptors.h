#ifndef CWW_ALLOCATOR_BORDER_DESCRIPTORS_H
#define CWW_ALLOCATOR_BORDER_DESCRIPTORS_H
#include "C:\Users\Sergey\source\repos\MyCourseWork\MyCourseWork\memory_global_heap.h"
#include <iostream>
#include <limits.h>
using namespace std;

class allocator_border_descriptors final : public memory {
private:

    void* _memory;

public:

    enum class status {
        first_available,
        best_available,
        worst_available
    };

private:

    size_t* sizeof_block(void* block) const {
        return reinterpret_cast<size_t*>(block);

    }

    void** get_pointer_next(void* block) const {
        return reinterpret_cast<void**>(reinterpret_cast<size_t*>(block) + 1);
    }

    void** get_pointer_previous(void* block) const {
        return reinterpret_cast<void**>(reinterpret_cast<void**>(reinterpret_cast<size_t*>(block) + 1) + 1);
    }


    memory** get_allocator()const {
        return reinterpret_cast<memory**>(_memory);
    }

    logger** get_logger()const {
        return reinterpret_cast<logger**>(reinterpret_cast<memory**>(_memory) + 1);
    }

    allocator_border_descriptors::status* get_status()const {
        return reinterpret_cast<status*>(reinterpret_cast<logger**>(reinterpret_cast<memory**>(_memory) + 1) + 1);
    }

    size_t* get_size()const {
        return reinterpret_cast<size_t*>(reinterpret_cast<status*>(reinterpret_cast<logger**>(reinterpret_cast<memory**>(_memory) + 1) + 1) + 1);
    }

    void** get_pointer()const {
        return reinterpret_cast<void**>(reinterpret_cast<size_t*>(reinterpret_cast<status*>(reinterpret_cast<logger**>(reinterpret_cast<memory**>(_memory) + 1) + 1) + 1) + 1);
    }




public:

    explicit allocator_border_descriptors(size_t size, logger* logger1 = nullptr, memory* allocated = nullptr, allocator_border_descriptors::status state = status::first_available) {

        if (allocated == nullptr) {
            _memory = :: operator new(size + sizeof(logger*) + sizeof(size_t) + sizeof(void*) + sizeof(memory*) + sizeof(status));
        }
        else {
            _memory = allocated->allocate(size + sizeof(logger*) + sizeof(size_t) + sizeof(void*) + sizeof(memory*) + sizeof(status));
        }

        *reinterpret_cast<memory**>(_memory) = allocated;
        *reinterpret_cast<logger**>(reinterpret_cast<memory**>(_memory) + 1) = logger1;
        *reinterpret_cast<status*>(reinterpret_cast<logger**>(reinterpret_cast<memory**>(_memory) + 1) + 1) = state;
        *reinterpret_cast<size_t*>(reinterpret_cast<status*>(reinterpret_cast<logger**>(reinterpret_cast<memory**>(_memory) + 1) + 1) + 1) = size;
        *reinterpret_cast<void**>(reinterpret_cast<size_t*>(reinterpret_cast<status*>(reinterpret_cast<logger**>(reinterpret_cast<memory**>(_memory) + 1) + 1) + 1) + 1) = nullptr;
    }

    void* allocate(size_t requested_size)const override {

        if (*get_pointer() == nullptr) {

            if (*get_size() >= requested_size + sizeof(size_t) + sizeof(void*) + sizeof(void*)) {

                auto block_new = reinterpret_cast<void*>(get_pointer() + 1);
                *sizeof_block(block_new) = requested_size;
                *get_pointer_previous(block_new) = nullptr;
                *get_pointer_next(block_new) = nullptr;
                *get_pointer() = block_new;

                return reinterpret_cast<void*>(get_pointer_previous(block_new) + 1);
            }
            else {
                return nullptr;
            }
        }


        switch (*get_status()) {

        case status::first_available: {

            auto current_block = *get_pointer();

            if (reinterpret_cast<unsigned char*>(*get_pointer()) - reinterpret_cast<unsigned char*>(reinterpret_cast<void**>(get_pointer()) + 1) >= requested_size + sizeof(size_t) + (sizeof(void*) << 1)) {

                auto block_new = reinterpret_cast<void*>(reinterpret_cast<void**>(get_pointer()) + 1);

                *sizeof_block(block_new) = requested_size;
                *get_pointer_previous(block_new) = nullptr;
                *get_pointer_previous(*get_pointer()) = block_new;
                *get_pointer_next(block_new) = *get_pointer();
                *get_pointer() = block_new;

                return reinterpret_cast<void*>(get_pointer_previous(block_new) + 1);

            }
            while (*get_pointer_next(current_block) != nullptr) {

                if (reinterpret_cast<unsigned char*>(*get_pointer_next(current_block)) - (reinterpret_cast<unsigned char*>(get_pointer_previous(current_block) + 1) + (*sizeof_block(current_block))) >= requested_size + sizeof(size_t) + (sizeof(void*) << 1)) {

                    auto pointer = *get_pointer_next(current_block);

                    *sizeof_block(reinterpret_cast<unsigned char*>(get_pointer_previous(current_block) + 1) + (*sizeof_block(current_block))) = requested_size;
                    *get_pointer_next(reinterpret_cast<unsigned char*>(get_pointer_previous(current_block) + 1) + *sizeof_block(current_block)) = *get_pointer_next(current_block);
                    *get_pointer_previous(reinterpret_cast<unsigned char*>(get_pointer_previous(current_block) + 1) + *sizeof_block(current_block)) = current_block;
                    *get_pointer_next(current_block) = reinterpret_cast<unsigned char*>(get_pointer_previous(current_block) + 1) + *sizeof_block(current_block);
                    *get_pointer_previous(pointer) = reinterpret_cast<unsigned char*>(get_pointer_previous(current_block) + 1) + *sizeof_block(current_block);

                    return reinterpret_cast<void*>(get_pointer_previous(reinterpret_cast<unsigned char*>(get_pointer_previous(current_block) + 1) + *sizeof_block(current_block)) + 1);
                }
                else {
                    current_block = *get_pointer_next(current_block);
                }
            }
            if (*get_pointer_next(current_block) == nullptr && (reinterpret_cast<unsigned char*>(reinterpret_cast<void**>(get_pointer()) + 1) + *get_size() - reinterpret_cast<unsigned char*>(get_pointer_previous(current_block) + 1) - *sizeof_block(current_block)) >= requested_size + sizeof(size_t) + (sizeof(void*) << 1)) {
                *sizeof_block((reinterpret_cast<unsigned char*>(get_pointer_previous(current_block) + 1) + *sizeof_block(current_block))) = requested_size;

                *get_pointer_next((reinterpret_cast<unsigned char*>(get_pointer_previous(current_block) + 1) + *sizeof_block(current_block))) = nullptr;

                *get_pointer_previous((reinterpret_cast<unsigned char*>(get_pointer_previous(current_block) + 1)) + *sizeof_block(current_block)) = current_block;

                *get_pointer_next(current_block) = reinterpret_cast<unsigned char*>(get_pointer_previous(current_block) + 1) + *sizeof_block(current_block);

                return reinterpret_cast<void*>(get_pointer_previous(reinterpret_cast<unsigned char*>(get_pointer_previous(current_block) + 1) + *sizeof_block(current_block)) + 1);
            }
            return nullptr;

        }

        case status::best_available: {

            void* res = nullptr;
            int min_size = INT_MAX;
            auto current_block = *get_pointer();

            if (reinterpret_cast<unsigned char*>(*get_pointer()) - reinterpret_cast<unsigned char*>(reinterpret_cast<void**>(get_pointer()) + 1) >= requested_size + sizeof(size_t) + (sizeof(void*) << 1) && reinterpret_cast<unsigned char*>(*get_pointer()) - reinterpret_cast<unsigned char*>(reinterpret_cast<void**>(get_pointer()) + 1) < min_size) {

                auto block_new = reinterpret_cast<void*>(reinterpret_cast<void**>(get_pointer()) + 1);

                *sizeof_block(block_new) = requested_size;
                *get_pointer_previous(block_new) = nullptr;
                *get_pointer_previous(*get_pointer()) = block_new;
                *get_pointer_next(block_new) = *get_pointer();
                *get_pointer() = block_new;

                res = reinterpret_cast<void*>(get_pointer_previous(block_new) + 1);
                min_size = reinterpret_cast<unsigned char*>(*get_pointer()) - reinterpret_cast<unsigned char*>(reinterpret_cast<void**>(get_pointer()) + 1);


            }
            while (*get_pointer_next(current_block) != nullptr) {

                if (reinterpret_cast<unsigned char*>(*get_pointer_next(current_block)) - (reinterpret_cast<unsigned char*>(get_pointer_previous(current_block) + 1) + (*sizeof_block(current_block))) >= requested_size + sizeof(size_t) + (sizeof(void*) << 1)) {

                    if (reinterpret_cast<unsigned char*>(*get_pointer_next(current_block)) - (reinterpret_cast<unsigned char*>(get_pointer_previous(current_block) + 1) + (*sizeof_block(current_block))) < min_size) {

                        if (res != nullptr) {

                            auto block = reinterpret_cast<void*>(reinterpret_cast<unsigned char*>(res) - (sizeof(void*) << 1) - sizeof(size_t));

                            if (*get_pointer_previous(block) != nullptr && *get_pointer_next(block) != nullptr) {
                                *get_pointer_previous(*get_pointer_next(block)) = *get_pointer_previous(block);
                                *get_pointer_next(*get_pointer_previous(block)) = *get_pointer_next(block);
                            }
                            else if (*get_pointer_previous(block) == nullptr && *get_pointer_next(block) != nullptr) {
                                *get_pointer_previous(*get_pointer_next(block)) = nullptr;
                                *get_pointer() = *get_pointer_next(block);
                            }
                            else if (*get_pointer_previous(block) != nullptr && *get_pointer_next(block) == nullptr) {
                                *get_pointer_next(*get_pointer_previous(block)) = nullptr;
                            }
                            else {
                                *get_pointer() = nullptr;
                            }
                        }

                        auto pointer = *get_pointer_next(current_block);
                        *sizeof_block(reinterpret_cast<unsigned char*>(get_pointer_previous(current_block) + 1) + (*sizeof_block(current_block))) = requested_size;

                        *get_pointer_next(reinterpret_cast<unsigned char*>(get_pointer_previous(current_block) + 1) + *sizeof_block(current_block)) = *get_pointer_next(current_block);

                        *get_pointer_previous(reinterpret_cast<unsigned char*>(get_pointer_previous(current_block) + 1) + *sizeof_block(current_block)) = current_block;

                        *get_pointer_next(current_block) = reinterpret_cast<unsigned char*>(get_pointer_previous(current_block) + 1) + *sizeof_block(current_block);

                        *get_pointer_previous(pointer) = reinterpret_cast<unsigned char*>(get_pointer_previous(current_block) + 1) + *sizeof_block(current_block);

                        min_size = reinterpret_cast<unsigned char*>(*get_pointer_next(current_block)) - (reinterpret_cast<unsigned char*>(get_pointer_previous(current_block) + 1) + (*sizeof_block(current_block)));

                        res = reinterpret_cast<void*>(get_pointer_previous(reinterpret_cast<unsigned char*>(get_pointer_previous(current_block) + 1) + *sizeof_block(current_block)) + 1);
                        current_block = *get_pointer_next(current_block);
                    }
                    else {
                        current_block = *get_pointer_next(current_block);
                    }
                }
                else {
                    current_block = *get_pointer_next(current_block);
                }
            }
            if (*get_pointer_next(current_block) == nullptr && (reinterpret_cast<unsigned char*>(reinterpret_cast<void**>(get_pointer()) + 1) + *get_size() - reinterpret_cast<unsigned char*>(get_pointer_previous(current_block) + 1) - *sizeof_block(current_block)) >= requested_size + sizeof(size_t) + (sizeof(void*) << 1)) {

                if ((reinterpret_cast<unsigned char*>(reinterpret_cast<void**>(get_pointer()) + 1) + *get_size() - reinterpret_cast<unsigned char*>(get_pointer_previous(current_block) + 1) - *sizeof_block(current_block)) < min_size) {

                    if (res != nullptr) {

                        auto block = reinterpret_cast<void*>(reinterpret_cast<unsigned char*>(res) - (sizeof(void*) << 1) - sizeof(size_t));

                        if (*get_pointer_previous(block) != nullptr && *get_pointer_next(block) != nullptr) {
                            *get_pointer_previous(*get_pointer_next(block)) = *get_pointer_previous(block);
                            *get_pointer_next(*get_pointer_previous(block)) = *get_pointer_next(block);
                        }
                        else if (*get_pointer_previous(block) == nullptr && *get_pointer_next(block) != nullptr) {
                            *get_pointer_previous(*get_pointer_next(block)) = nullptr;
                            *get_pointer() = *get_pointer_next(block);
                        }
                        else if (*get_pointer_previous(block) != nullptr && *get_pointer_next(block) == nullptr) {
                            *get_pointer_next(*get_pointer_previous(block)) = nullptr;
                        }
                        else {
                            *get_pointer() = nullptr;
                        }
                    }

                    *sizeof_block((reinterpret_cast<unsigned char*>(get_pointer_previous(current_block) + 1) + *sizeof_block(current_block))) = requested_size;

                    *get_pointer_next((reinterpret_cast<unsigned char*>(get_pointer_previous(current_block) + 1) + *sizeof_block(current_block))) = nullptr;

                    *get_pointer_previous((reinterpret_cast<unsigned char*>(get_pointer_previous(current_block) + 1)) + *sizeof_block(current_block)) = current_block;

                    *get_pointer_next(current_block) = reinterpret_cast<unsigned char*>(get_pointer_previous(current_block) + 1) + *sizeof_block(current_block);

                    return reinterpret_cast<void*>(get_pointer_previous(reinterpret_cast<unsigned char*>(get_pointer_previous(current_block) + 1) + *sizeof_block(current_block)) + 1);
                }

            }
            return res;

        }
        case status::worst_available: {

            void* result = nullptr;
            size_t max_size = 0;
            auto current_block = *get_pointer();

            if (reinterpret_cast<unsigned char*>(*get_pointer()) - reinterpret_cast<unsigned char*>(reinterpret_cast<void**>(get_pointer()) + 1) >= requested_size + sizeof(size_t) + (sizeof(void*) << 1) && (reinterpret_cast<unsigned char*>(*get_pointer()) - reinterpret_cast<unsigned char*>(reinterpret_cast<void**>(get_pointer()) + 1) > max_size)) {

                auto block_new = reinterpret_cast<void*>(reinterpret_cast<void**>(get_pointer()) + 1);

                *sizeof_block(block_new) = requested_size;
                *get_pointer_previous(block_new) = nullptr;
                *get_pointer_previous(*get_pointer()) = block_new;
                *get_pointer_next(block_new) = *get_pointer();
                *get_pointer() = block_new;

                result = reinterpret_cast<void*>(get_pointer_previous(block_new) + 1);
                max_size = reinterpret_cast<unsigned char*>(*get_pointer()) - reinterpret_cast<unsigned char*>(reinterpret_cast<void**>(get_pointer()) + 1);


            }
            while (*get_pointer_next(current_block) != nullptr) {

                if (reinterpret_cast<unsigned char*>(*get_pointer_next(current_block)) - (reinterpret_cast<unsigned char*>(get_pointer_previous(current_block) + 1) + (*sizeof_block(current_block))) >= requested_size + sizeof(size_t) + (sizeof(void*) << 1)) {

                    if (reinterpret_cast<unsigned char*>(*get_pointer_next(current_block)) - (reinterpret_cast<unsigned char*>(get_pointer_previous(current_block) + 1) + (*sizeof_block(current_block))) > max_size) {

                        if (result != nullptr) {

                            auto block = reinterpret_cast<void*>(reinterpret_cast<unsigned char*>(result) - (sizeof(void*) << 1) - sizeof(size_t));

                            if (*get_pointer_previous(block) != nullptr && *get_pointer_next(block) != nullptr) {
                                *get_pointer_previous(*get_pointer_next(block)) = *get_pointer_previous(block);
                                *get_pointer_next(*get_pointer_previous(block)) = *get_pointer_next(block);
                            }
                            else if (*get_pointer_previous(block) == nullptr && *get_pointer_next(block) != nullptr) {
                                *get_pointer_previous(*get_pointer_next(block)) = nullptr;
                                *get_pointer() = *get_pointer_next(block);
                            }
                            else if (*get_pointer_previous(block) != nullptr && *get_pointer_next(block) == nullptr) {
                                *get_pointer_next(*get_pointer_previous(block)) = nullptr;
                            }
                            else {
                                *get_pointer() = nullptr;
                            }
                        }

                        auto pointer = *get_pointer_next(current_block);
                        *sizeof_block(reinterpret_cast<unsigned char*>(get_pointer_previous(current_block) + 1) + (*sizeof_block(current_block))) = requested_size;

                        *get_pointer_next(reinterpret_cast<unsigned char*>(get_pointer_previous(current_block) + 1) + *sizeof_block(current_block)) = *get_pointer_next(current_block);

                        *get_pointer_previous(reinterpret_cast<unsigned char*>(get_pointer_previous(current_block) + 1) + *sizeof_block(current_block)) = current_block;

                        *get_pointer_next(current_block) = reinterpret_cast<unsigned char*>(get_pointer_previous(current_block) + 1) + *sizeof_block(current_block);

                        *get_pointer_previous(pointer) = reinterpret_cast<unsigned char*>(get_pointer_previous(current_block) + 1) + *sizeof_block(current_block);

                        max_size = reinterpret_cast<unsigned char*>(*get_pointer_next(current_block)) - (reinterpret_cast<unsigned char*>(get_pointer_previous(current_block) + 1) + (*sizeof_block(current_block)));

                        result = reinterpret_cast<void*>(get_pointer_previous(reinterpret_cast<unsigned char*>(get_pointer_previous(current_block) + 1) + *sizeof_block(current_block)) + 1);
                        current_block = *get_pointer_next(current_block);

                    }
                    else {
                        current_block = *get_pointer_next(current_block);
                    }
                }
                else {
                    current_block = *get_pointer_next(current_block);
                }
            }
            if ((*get_pointer_next(current_block) == nullptr) && ((reinterpret_cast<unsigned char*>(reinterpret_cast<void**>(get_pointer()) + 1) + *get_size() - reinterpret_cast<unsigned char*>(get_pointer_previous(current_block) + 1) - *sizeof_block(current_block)) >= requested_size + sizeof(size_t) + (sizeof(void*) << 1)) && (reinterpret_cast<unsigned char*>(reinterpret_cast<void**>(get_pointer()) + 1) + *get_size() - reinterpret_cast<unsigned char*>(get_pointer_previous(current_block) + 1) - *sizeof_block(current_block) > max_size)) {

                if (result != nullptr) {

                    auto block = reinterpret_cast<void*>(reinterpret_cast<unsigned char*>(result) - (sizeof(void*) << 1) - sizeof(size_t));

                    if (*get_pointer_previous(block) != nullptr && *get_pointer_next(block) != nullptr) {
                        *get_pointer_previous(*get_pointer_next(block)) = *get_pointer_previous(block);
                        *get_pointer_next(*get_pointer_previous(block)) = *get_pointer_next(block);
                    }
                    else if (*get_pointer_previous(block) == nullptr && *get_pointer_next(block) != nullptr) {
                        *get_pointer_previous(*get_pointer_next(block)) = nullptr;
                        *get_pointer() = *get_pointer_next(block);
                    }
                    else if (*get_pointer_previous(block) != nullptr && *get_pointer_next(block) == nullptr) {
                        *get_pointer_next(*get_pointer_previous(block)) = nullptr;
                    }
                    else {
                        *get_pointer() = nullptr;
                    }
                }

                *sizeof_block((reinterpret_cast<unsigned char*>(get_pointer_previous(current_block) + 1) + *sizeof_block(current_block))) = requested_size;

                *get_pointer_next((reinterpret_cast<unsigned char*>(get_pointer_previous(current_block) + 1) + *sizeof_block(current_block))) = nullptr;

                *get_pointer_previous((reinterpret_cast<unsigned char*>(get_pointer_previous(current_block) + 1)) + *sizeof_block(current_block)) = current_block;

                *get_pointer_next(current_block) = reinterpret_cast<unsigned char*>(get_pointer_previous(current_block) + 1) + *sizeof_block(current_block);

                result = reinterpret_cast<void*>(get_pointer_previous(reinterpret_cast<unsigned char*>(get_pointer_previous(current_block) + 1) + *sizeof_block(current_block)) + 1);


            }
            return result;
        }
        }
    }

    void deallocate(void* deallocated) const override {

        if (deallocated != nullptr) {

            auto blockt = reinterpret_cast<void*>(reinterpret_cast<unsigned char*>(deallocated) - (sizeof(void*) << 1) - sizeof(size_t));

            if (*get_pointer_previous(blockt) != nullptr && *get_pointer_next(blockt) != nullptr) {
                *get_pointer_previous(*get_pointer_next(blockt)) = *get_pointer_previous(blockt);
                *get_pointer_next(*get_pointer_previous(blockt)) = *get_pointer_next(blockt);
            }
            else if (*get_pointer_previous(blockt) == nullptr && *get_pointer_next(blockt) != nullptr) {
                *get_pointer_previous(*get_pointer_next(blockt)) = nullptr;
                *get_pointer() = *get_pointer_next(blockt);
            }
            else if (*get_pointer_previous(blockt) != nullptr && *get_pointer_next(blockt) == nullptr) {
                *get_pointer_next(*get_pointer_previous(blockt)) = nullptr;
            }
            else {
                *get_pointer() = nullptr;
            }

            auto block = reinterpret_cast<void*>(reinterpret_cast<unsigned char*>(deallocated) - (sizeof(void*) << 1) - sizeof(size_t));

            size_t size = *sizeof_block(block);
            string log_final = "[ ";

            for (int i = 0; i < size; i++) {
                unsigned char a = *(reinterpret_cast<unsigned char*>(deallocated) + i);
                log_final += to_string(static_cast<unsigned short>(a));
                log_final += " ";
            }

            string message = "dealocated block: " + log_final + "]";

            if (*get_logger() != nullptr) {

                (*get_logger())->log(message, logger::severity::debug);

            }
        }

    }


    ~allocator_border_descriptors() {
        if (*get_allocator() == nullptr) {
            ::operator delete(_memory);
        }
        else {
            (*get_allocator())->deallocate(_memory);
        }

    }

};

#endif //CWW_ALLOCATOR_BORDER_DESCRIPTORS_H
