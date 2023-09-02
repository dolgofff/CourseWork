#ifndef CWW_ASSOCIATIVE_CONTAINER_H
#define CWW_ASSOCIATIVE_CONTAINER_H
template <typename tkey, typename tvalue>
class associative_container
{

public:

    struct key_value_pair {
        tkey _key;
        tvalue _value;
    };

    enum class bypass_mode {
        prefix,
        infix,
        postfix
    };


    virtual void insert(const tkey& key, const tvalue& value) = 0;

    virtual bool find(key_value_pair* target_key_and_result_value) = 0;

   
    virtual bool find_key(const tkey& key) const = 0;

    virtual tvalue remove(const tkey& key) = 0;

    bool operator[](key_value_pair* target_key_and_result_value) {
        return find(target_key_and_result_value);
    }

    void operator+=(key_value_pair& pair) {
        insert(pair._key, pair._value);
    }

    tvalue operator-=(const tkey& key) {
        return remove(key);
    }

    virtual void bypass(bypass_mode mode) const = 0;

    virtual const tvalue& get(const tkey& key) const = 0;

    virtual tkey& get_key_ref(const tkey& key) const = 0;

    virtual void update(const tkey& key, const tvalue& value) = 0;

    virtual ~associative_container() = default;

};

#endif //CWW_ASSOCIATIVE_CONTAINER_H
