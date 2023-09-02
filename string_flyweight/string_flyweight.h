#include "C:\Users\Sergey\source\repos\MyCourseWork\MyCourseWork\red_black_tree.h"
#ifndef CWW_STRING_FLYWEIGHT_H
#define CWW_STRING_FLYWEIGHT_H




class string_flyweight final
{
private:

    class string_comparer
    {
    public:
        string_comparer() = default;
        int operator()(const string& s_c1, const string& s_c2) const {
            return s_c1.compare(s_c2);
        }
    };

    string_flyweight() :
        _alloc(new allocator_border_descriptors(500000)), _strings_pool(new red_black_tree<string, size_t, string_comparer>(_alloc))
    {};

    memory* _alloc;
    red_black_tree<string, size_t, string_comparer>* _strings_pool;

public:

    string_flyweight(const string_flyweight& other) = delete;
    string_flyweight(string_flyweight&& other) = delete;
    string_flyweight& operator=(const string_flyweight& other) = delete;
    string_flyweight& operator=(string_flyweight&& other) = delete;

    static string_flyweight& get_instance() {
        static string_flyweight _instance;
        return _instance;
    }

    string& get_flyweight(const string& str) {
        if (!_strings_pool->find_key(str)) {
            _strings_pool->insert(str, 1);
        }
        else {
            size_t k = _strings_pool->get(str);
            k++;
            _strings_pool->update(str, k);
        }

        return _strings_pool->get_key_ref(str);
    }

    void check_flyweight(const string& str) {
        if (_strings_pool->get(str) <= 1) {
            _strings_pool->remove(str);
        }
        else {
            size_t k = _strings_pool->get(str);
            k--;
            _strings_pool->update(str, k);
        }
    }

    ~string_flyweight() {
        if (_strings_pool != nullptr) {
            delete _strings_pool;
        }
        if (_alloc != nullptr) {
            delete _alloc;
        }
    }

};


#endif //CWW_STRING_FLYWEIGHT_H