#ifndef CWW_RED_BLACK_TREE_H
#define CWW_RED_BLACK_TREE_H
#include "C:\Users\Sergey\source\repos\MyCourseWork\MyCourseWork\binary_search_tree.h"
using namespace std;
template <typename tkey, typename tvalue, typename tkey_comparer>
class red_black_tree : public binary_search_tree<tkey, tvalue, tkey_comparer>
{
private:

    enum class color {
        red,
        black
    };

    struct rb_node final : public binary_search_tree<tkey, tvalue, tkey_comparer>::node {
        rb_node() = default;
        rb_node(const tkey& k, const tvalue& v, rb_node* l, rb_node* r, color c) : binary_search_tree<tkey, tvalue, tkey_comparer>::node(k, v, l, r) {
            node_color = c;
        }

        color node_color;

        virtual ~rb_node() = default;
    };

    //Вставка

private:
    class rb_insert_template_method final : public binary_search_tree<tkey, tvalue, tkey_comparer>::insert_template_method
    {

    public:
        explicit rb_insert_template_method(red_black_tree<tkey, tvalue, tkey_comparer>* new_rbt) :
            binary_search_tree<tkey, tvalue, tkey_comparer>::insert_template_method(new_rbt) {}

    private:
        red_black_tree<tkey, tvalue, tkey_comparer>* _this;

    private:
        size_t get_node_size() const override {
            return sizeof(rb_node);
        }
        void get_node_constructor(typename binary_search_tree<tkey, tvalue, tkey_comparer>::node** x, const tkey& key, const tvalue& value) const override {
            new(*(reinterpret_cast<rb_node**>(x))) rb_node{ key,value,nullptr,nullptr,color::red };
        }

        void insert_balance(stack <rb_node*>* _insert_way, rb_node** rb_root) {

            rb_node* target = _insert_way->top();
            _insert_way->pop();
            //случай 0
            if (_insert_way->empty()) {
                target->node_color = color::black;
            }

            else {
                if (_insert_way->top()->node_color != color::black) {
                    rb_node* dad = _insert_way->top();
                    rb_node* uncle;
                    _insert_way->pop();
                    if (_insert_way->top()->left == dad) {
                        uncle = reinterpret_cast<rb_node*>(_insert_way->top()->right);
                    }
                    else {
                        uncle = reinterpret_cast<rb_node*>(_insert_way->top()->left);
                    }
                    //случай 1: красный дядя
                    if (uncle != nullptr && uncle->node_color == color::red) {
                        dad->node_color = color::black;
                        uncle->node_color = color::black;
                        _insert_way->top()->node_color = color::red; //ДЕД КРАСНЫЙ
                        insert_balance(_insert_way, rb_root); //вызываем рекурскию,так как корень может быть чёрным
                    }
                    //Развилка для следующих случаев-чёрный дядя
                    else {
                        rb_node* grandpa = _insert_way->top();
                        rb_node* greatgr;
                        if (_insert_way->size() > 1) {
                            _insert_way->pop();
                            greatgr = _insert_way->top();
                        }
                        else {
                            greatgr = nullptr;
                        }
                        //случай 2.1: Дед и отец в разных сторонах- приводим к левой
                        if (grandpa->left == dad) {

                            if (dad->right == target) {
                                //Нужно выполнить малый поворот от сына к его отцу
                                _this->rotate_left(reinterpret_cast<typename binary_search_tree<tkey, tvalue, tkey_comparer>::node**>(&grandpa->left));
                                //теперь имеем отца и деда по одну сторону.начинаем 3.1
                                if (greatgr != nullptr) {
                                    if (greatgr->left == grandpa) {
                                        _this->rotate_right(reinterpret_cast<typename binary_search_tree<tkey, tvalue, tkey_comparer>::node**>(&greatgr->left));
                                    }
                                    else {
                                        _this->rotate_right(reinterpret_cast<typename binary_search_tree<tkey, tvalue, tkey_comparer>::node**>(&greatgr->right));
                                    }
                                }
                                else {
                                    _this->rotate_right(reinterpret_cast<typename binary_search_tree<tkey, tvalue, tkey_comparer>::node**>(rb_root));
                                }
                                //меняем цвет target а не отца,т.к. они поменялись позициями,и теперь target - сам отец
                                target->node_color = color::black;
                                grandpa->node_color = color::red;
                                //конец 3.1
                            }
                            //конец 2.1
                            //если сразу 3.1
                            else {
                                if (greatgr != nullptr) {
                                    if (greatgr->left == grandpa) {
                                        _this->rotate_right(reinterpret_cast<typename binary_search_tree<tkey, tvalue, tkey_comparer>::node**>(&greatgr->left));
                                    }
                                    else {
                                        _this->rotate_right(reinterpret_cast<typename binary_search_tree<tkey, tvalue, tkey_comparer>::node**>(&greatgr->right));
                                    }
                                }
                                else {
                                    _this->rotate_right(reinterpret_cast<typename binary_search_tree<tkey, tvalue, tkey_comparer>::node**>(rb_root));
                                }
                                dad->node_color = color::black;
                                grandpa->node_color = color::red;

                            }
                            //конец моментального 3.1
                        }
                        //случай 2.2: Дед и отец в разных сторонах - приводим к правой
                        else {
                            if (dad->left == target) {
                                //малый поворот от сына к его отцу
                                _this->rotate_right(reinterpret_cast<typename binary_search_tree<tkey, tvalue, tkey_comparer>::node**>(&grandpa->right));
                                //теперь имеем отца и деда по одну сторону.начинаем 3.2
                                if (greatgr != nullptr) {
                                    if (greatgr->left == grandpa) _this->rotate_left(
                                        reinterpret_cast<typename binary_search_tree<tkey, tvalue, tkey_comparer>::node**>(&greatgr->left));
                                    else _this->rotate_left(
                                        reinterpret_cast<typename binary_search_tree<tkey, tvalue, tkey_comparer>::node**>(&greatgr->right));
                                }
                                else {
                                    _this->rotate_left(
                                        reinterpret_cast<typename binary_search_tree<tkey, tvalue, tkey_comparer>::node**>(rb_root));
                                }
                                target->node_color = color::black;
                                grandpa->node_color = color::red;
                                //конец 3.2
                            }
                            //конец 2.2
                            //если сразу 3.2
                            else {
                                if (greatgr != nullptr) {
                                    if (greatgr->left == grandpa) _this->rotate_left(
                                        reinterpret_cast<typename binary_search_tree<tkey, tvalue, tkey_comparer>::node**>(&greatgr->left));
                                    else _this->rotate_left(
                                        reinterpret_cast<typename binary_search_tree<tkey, tvalue, tkey_comparer>::node**>(&greatgr->right));
                                }
                                else {
                                    _this->rotate_left(
                                        reinterpret_cast<typename binary_search_tree<tkey, tvalue, tkey_comparer>::node**>(rb_root));
                                }
                                dad->node_color = color::black;
                                grandpa->node_color = color::red;
                            }
                            //конец моментального 3.2
                        }
                    }
                }
            }
        }
    public:

        void after_insert(const tkey& key, const tvalue& value, typename binary_search_tree<tkey, tvalue, tkey_comparer>::node** root, logger* logger, stack<typename binary_search_tree<tkey, tvalue, tkey_comparer>::node*>* _i_path) override {
            auto** rb_root = reinterpret_cast<rb_node**>(root);
            auto* _insert_way = reinterpret_cast<stack<rb_node*>*>(_i_path);
            if (_insert_way->size() == 1) {
                _insert_way->top()->node_color = color::black;
                if (logger != nullptr) {
                    logger->log("after_insert: tree was balanced!", logger::severity::debug);
                }
                return;
            }
            else {
                _insert_way->top()->node_color = color::red;
                insert_balance(_insert_way, rb_root);
                if (logger != nullptr) {
                    logger->log("after_insert: tree was balanced!", logger::severity::debug);
                }
                return;
            }
        }

    };

    //Удаление

    class rb_remove_template_method final : public binary_search_tree<tkey, tvalue, tkey_comparer>::remove_template_method
    {
    public:

        explicit rb_remove_template_method(red_black_tree<tkey, tvalue, tkey_comparer>* new_rbt) :
            binary_search_tree<tkey, tvalue, tkey_comparer>::remove_template_method(new_rbt) {}

    private:

        red_black_tree<tkey, tvalue, tkey_comparer>* _this;

    private:

        void get_node_destructor(typename binary_search_tree<tkey, tvalue, tkey_comparer>::node* x) const override {
            auto* y = reinterpret_cast<rb_node*>(x);
            y->~rb_node();
        }

        void additional_work(typename binary_search_tree<tkey, tvalue, tkey_comparer>::node* current, typename binary_search_tree<tkey, tvalue, tkey_comparer>::node* dad, size_t& side, size_t& additional) const override {
            auto* rb_current = reinterpret_cast<rb_node*>(current);
            auto* rb_dad = reinterpret_cast<rb_node*>(dad);
            rb_dad->left == rb_current ? side = 0 : side = 1; //0-расположен слева от бати 1- расположен справа от бати
            rb_current->node_color == color::red ? additional = 0 : additional = 1; //0 - additional красный | 1 - additional чёрный
        }

        void remove_balance(stack<rb_node*>* _remove_path, size_t& side, size_t& additional, rb_node** rb_root) {
            rb_node* dad = _remove_path->top();
            _remove_path->pop();
            rb_node* brother;
            side == 0 ? brother = reinterpret_cast<rb_node*>(dad->right) : brother = reinterpret_cast<rb_node*>(dad->left);

            if (side == 1) {
                //Реализация логики при условии,что удалённый сын был правым

                //КЧ1 — родитель красный, левый ребёнок чёрный с чёрными внуками.
                if (dad->node_color == color::red && brother->node_color == color::black && (brother->left == nullptr || reinterpret_cast<rb_node*>(brother->left)->node_color == color::black) && ((brother->right == nullptr || reinterpret_cast<rb_node*>(brother->right)->node_color == color::black))) {
                    dad->node_color = color::black;
                    brother->node_color = color::red;
                }

                //КЧ2 — родитель красный, левый ребёнок чёрный с левым красным внуком.
                else if (dad->node_color == color::red && brother->node_color == color::black && brother->left != nullptr && reinterpret_cast<rb_node*>(brother->left)->node_color == color::red) {
                    dad->node_color = color::black;
                    brother->node_color = color::red;
                    reinterpret_cast<rb_node*>(brother->left)->node_color = color::black;
                    //нужен малый поворот направо
                    if (!_remove_path->empty()) {
                        if (_remove_path->top()->left == dad) {
                            _this->rotate_right(reinterpret_cast<typename binary_search_tree<tkey, tvalue, tkey_comparer>::node**>(&(_remove_path->top()->left)));
                        }
                        else {
                            _this->rotate_right(reinterpret_cast<typename binary_search_tree<tkey, tvalue, tkey_comparer>::node**>(&(_remove_path->top()->right)));
                        }
                    }
                    else {
                        _this->rotate_right(reinterpret_cast<typename binary_search_tree<tkey, tvalue, tkey_comparer>::node**>(rb_root));
                    }
                }

                //КЧ2 — родитель красный, левый ребёнок чёрный с правым красным внуком.(сводится к чч5)
                else if (dad->node_color == color::red && brother->node_color == color::black && brother->right != nullptr && reinterpret_cast<rb_node*>(brother->right)->node_color == color::red) {
                    brother->node_color = color::red;
                    reinterpret_cast<rb_node*>(brother->right)->node_color = color::black;
                    _this->rotate_left(reinterpret_cast<typename binary_search_tree<tkey, tvalue, tkey_comparer>::node**>(&(dad->left)));
                    brother = reinterpret_cast<rb_node*>(dad->left);


                    dad->node_color = color::black;
                    brother->node_color = color::red;
                    reinterpret_cast<rb_node*>(brother->left)->node_color = color::black;
                    if (!_remove_path->empty()) {
                        if (_remove_path->top()->left == dad) {
                            _this->rotate_right(reinterpret_cast<typename binary_search_tree<tkey, tvalue, tkey_comparer>::node**>(&(_remove_path->top()->left)));
                        }
                        else {
                            _this->rotate_right(reinterpret_cast<typename binary_search_tree<tkey, tvalue, tkey_comparer>::node**>(&(_remove_path->top()->right)));
                        }
                    }
                    else {
                        _this->rotate_right(reinterpret_cast<typename binary_search_tree<tkey, tvalue, tkey_comparer>::node**>(rb_root));
                    }
                }

                //ЧК3 — родитель чёрный, левый сын красный, у правого внука чёрные правнуки
                else if (dad->node_color == color::black && brother->node_color == color::red && brother->right != nullptr && reinterpret_cast<rb_node*>(brother->right)->node_color == color::black && (reinterpret_cast<rb_node*>(brother->right)->left == nullptr || reinterpret_cast<rb_node*>(reinterpret_cast<rb_node*>(brother->right)->left)->node_color == color::black) && (reinterpret_cast<rb_node*>(brother->right)->right == nullptr || reinterpret_cast<rb_node*>(reinterpret_cast<rb_node*>(brother->right)->right)->node_color == color::black)) {
                    brother->node_color = color::black;
                    reinterpret_cast<rb_node*>(brother->right)->node_color = color::red;
                    //теперь малый направо
                    if (!_remove_path->empty()) {
                        if (_remove_path->top()->left == dad) {
                            _this->rotate_right(reinterpret_cast<typename binary_search_tree<tkey, tvalue, tkey_comparer>::node**>(&(_remove_path->top()->left)));
                        }
                        else {
                            _this->rotate_right(reinterpret_cast<typename binary_search_tree<tkey, tvalue, tkey_comparer>::node**>(&(_remove_path->top()->right)));
                        }
                    }
                    else {
                        _this->rotate_right(reinterpret_cast<typename binary_search_tree<tkey, tvalue, tkey_comparer>::node**>(rb_root));
                    }
                }

                //ЧК4 — родитель чёрный, левый сын красный, у правого внука левый правнук красный
                else if (dad->node_color == color::black && brother->node_color == color::red && brother->right != nullptr && reinterpret_cast<rb_node*>(brother->right)->node_color == color::black && (reinterpret_cast<rb_node*>(brother->right)->left != nullptr && reinterpret_cast<rb_node*>(reinterpret_cast<rb_node*>(brother->right)->left)->node_color == color::red)) {

                    reinterpret_cast<rb_node*>(reinterpret_cast<rb_node*>(brother->right)->left)->node_color = color::black;
                    //Большой поворот за счёт двух малых
                    //от внука к его отцу
                    _this->rotate_left(reinterpret_cast<typename binary_search_tree<tkey, tvalue, tkey_comparer>::node**>(&(dad->left)));
                    //от нового отца к константному деду
                    if (!_remove_path->empty()) {
                        if (_remove_path->top()->left == dad) {
                            _this->rotate_right(reinterpret_cast<typename binary_search_tree<tkey, tvalue, tkey_comparer>::node**>(&(_remove_path->top()->left)));
                        }
                        else {
                            _this->rotate_right(reinterpret_cast<typename binary_search_tree<tkey, tvalue, tkey_comparer>::node**>(&(_remove_path->top()->right)));
                        }
                    }
                    else {
                        _this->rotate_right(reinterpret_cast<typename binary_search_tree<tkey, tvalue, tkey_comparer>::node**>(rb_root));
                    }
                }

                //ЧК4 — родитель чёрный, левый сын красный, у правого внука правый правнук красный
                else if (dad->node_color == color::black && brother->node_color == color::red && brother->right != nullptr && reinterpret_cast<rb_node*>(brother->right)->node_color == color::black && (reinterpret_cast<rb_node*>(brother->right)->right != nullptr && reinterpret_cast<rb_node*>(reinterpret_cast<rb_node*>(brother->right)->right)->node_color == color::red)) {

                    reinterpret_cast<rb_node*>(reinterpret_cast<rb_node*>(brother->right)->right)->node_color = color::black;

                    reinterpret_cast<rb_node*>(brother->right)->node_color = color::red;

                    _this->rotate_left(reinterpret_cast<typename binary_search_tree<tkey, tvalue, tkey_comparer>::node**>(&(brother->right)));


                    reinterpret_cast<rb_node*>(reinterpret_cast<rb_node*>(brother->right)->left)->node_color = color::black;
                    _this->rotate_left(reinterpret_cast<typename binary_search_tree<tkey, tvalue, tkey_comparer>::node**>(&(dad->left)));

                    if (!_remove_path->empty()) {
                        if (_remove_path->top()->left == dad) {
                            _this->rotate_right(reinterpret_cast<typename binary_search_tree<tkey, tvalue, tkey_comparer>::node**>(&(_remove_path->top()->left)));
                        }
                        else {
                            _this->rotate_right(reinterpret_cast<typename binary_search_tree<tkey, tvalue, tkey_comparer>::node**>(&(_remove_path->top()->right)));
                        }
                    }
                    else {
                        _this->rotate_right(reinterpret_cast<typename binary_search_tree<tkey, tvalue, tkey_comparer>::node**>(rb_root));
                    }
                }

                //ЧЧ5 — родитель чёрный, левый сын чёрный с правым красным внуком.
                else if (dad->node_color == color::black && brother->node_color == color::black && brother->right != nullptr && reinterpret_cast<rb_node*>(brother->right)->node_color == color::red) {
                    reinterpret_cast<rb_node*>(brother->right)->node_color = color::black;
                    //Большой поворот налево как в ЧЧ4
                    _this->rotate_left(reinterpret_cast<typename binary_search_tree<tkey, tvalue, tkey_comparer>::node**>(&(dad->left)));

                    if (!_remove_path->empty()) {
                        if (_remove_path->top()->left == dad) {
                            _this->rotate_right(reinterpret_cast<typename binary_search_tree<tkey, tvalue, tkey_comparer>::node**>(&(_remove_path->top()->left)));
                        }
                        else {
                            _this->rotate_right(reinterpret_cast<typename binary_search_tree<tkey, tvalue, tkey_comparer>::node**>(&(_remove_path->top()->right)));
                        }
                    }
                    else {
                        _this->rotate_right(reinterpret_cast<typename binary_search_tree<tkey, tvalue, tkey_comparer>::node**>(rb_root));
                    }
                }

                //ЧЧ5 — родитель чёрный, левый сын чёрный с левым красным внуком.
                else if (dad->node_color == color::black && brother->node_color == color::black && brother->left != nullptr && reinterpret_cast<rb_node*>(brother->left)->node_color == color::red) {

                    reinterpret_cast<rb_node*>(brother->left)->node_color = color::black;

                    if (!_remove_path->empty()) {
                        if (_remove_path->top()->left == dad) {
                            _this->rotate_right(reinterpret_cast<typename binary_search_tree<tkey, tvalue, tkey_comparer>::node**>(&(_remove_path->top()->left)));
                        }
                        else {
                            _this->rotate_right(reinterpret_cast<typename binary_search_tree<tkey, tvalue, tkey_comparer>::node**>(&(_remove_path->top()->right)));
                        }
                    }
                    else {
                        _this->rotate_right(reinterpret_cast<typename binary_search_tree<tkey, tvalue, tkey_comparer>::node**>(rb_root));
                    }
                }

                //Ч6 — родитель чёрный, левый сын чёрный, его внуки тоже чёрные
                else if (dad->node_color == color::black && brother->node_color == color::black && (brother->left == nullptr || reinterpret_cast<rb_node*>(brother->left)->node_color == color::black) && ((brother->right == nullptr || reinterpret_cast<rb_node*>(brother->right)->node_color == color::black))) {
                    brother->node_color = color::red;
                    if (!_remove_path->empty()) {
                        if (_remove_path->top()->left == dad) {
                            side = 0;
                        }
                        else {
                            side = 1;
                        }
                        try {
                            remove_balance(_remove_path, side, additional, rb_root);
                        }
                        catch (const logic_error& ex) {
                            throw ex;
                        }
                    }
                }
                else {
                    throw logic_error(" *** Not expected situation ***");
                }
            }

            else {
                //Реализация логики при условии,что удалённый сын был правым
                //КЧ1 — родитель красный, левый ребёнок чёрный с чёрными внуками.
                if (dad->node_color == color::red && brother->node_color == color::black && (brother->left == nullptr || reinterpret_cast<rb_node*>(brother->left)->node_color == color::black) && ((brother->right == nullptr || reinterpret_cast<rb_node*>(brother->right)->node_color == color::black))) {
                    dad->node_color = color::black;
                    brother->node_color = color::red;
                }

                //КЧ2 — родитель красный, левый ребёнок чёрный с левым красным внуком.
                else if (dad->node_color == color::red && brother->node_color == color::black && brother->right != nullptr && reinterpret_cast<rb_node*>(brother->right)->node_color == color::red) {
                    dad->node_color = color::black;
                    brother->node_color = color::red;
                    reinterpret_cast<rb_node*>(brother->right)->node_color = color::black;
                    if (!_remove_path->empty()) {
                        if (_remove_path->top()->left == dad) {
                            _this->rotate_left(reinterpret_cast<typename binary_search_tree<tkey, tvalue, tkey_comparer>::node**>(&(_remove_path->top()->left)));
                        }
                        else {
                            _this->rotate_left(reinterpret_cast<typename binary_search_tree<tkey, tvalue, tkey_comparer>::node**>(&(_remove_path->top()->right)));
                        }
                    }
                    else {
                        _this->rotate_left(reinterpret_cast<typename binary_search_tree<tkey, tvalue, tkey_comparer>::node**>(rb_root));
                    }
                }

                //КЧ2 — родитель красный, левый ребёнок чёрный с правым красным внуком.
                else if (dad->node_color == color::red && brother->node_color == color::black && brother->left != nullptr && reinterpret_cast<rb_node*>(brother->left)->node_color == color::red) {
                    brother->node_color = color::red;
                    reinterpret_cast<rb_node*>(brother->left)->node_color = color::black;
                    _this->rotate_right(reinterpret_cast<typename binary_search_tree<tkey, tvalue, tkey_comparer>::node**>(&(dad->right)));
                    brother = reinterpret_cast<rb_node*>(dad->right);

                    dad->node_color = color::black;
                    brother->node_color = color::red;
                    reinterpret_cast<rb_node*>(brother->right)->node_color = color::black;
                    if (!_remove_path->empty()) {
                        if (_remove_path->top()->left == dad) {
                            _this->rotate_left(reinterpret_cast<typename binary_search_tree<tkey, tvalue, tkey_comparer>::node**>(&(_remove_path->top()->left)));
                        }
                        else {
                            _this->rotate_left(reinterpret_cast<typename binary_search_tree<tkey, tvalue, tkey_comparer>::node**>(&(_remove_path->top()->right)));
                        }
                    }
                    else {
                        _this->rotate_left(reinterpret_cast<typename binary_search_tree<tkey, tvalue, tkey_comparer>::node**>(rb_root));
                    }
                }

                //ЧК3 — родитель чёрный, левый сын красный, у правого внука чёрные правнуки
                else if (dad->node_color == color::black && brother->node_color == color::red && brother->left != nullptr && reinterpret_cast<rb_node*>(brother->left)->node_color == color::black && (reinterpret_cast<rb_node*>(brother->left)->left == nullptr || reinterpret_cast<rb_node*>(reinterpret_cast<rb_node*>(brother->left)->left)->node_color == color::black) && (reinterpret_cast<rb_node*>(brother->left)->right == nullptr || reinterpret_cast<rb_node*>(reinterpret_cast<rb_node*>(brother->left)->right)->node_color == color::black)) {
                    brother->node_color = color::black;
                    reinterpret_cast<rb_node*>(brother->left)->node_color = color::red;
                    if (!_remove_path->empty()) {
                        if (_remove_path->top()->left == dad) {
                            _this->rotate_left(reinterpret_cast<typename binary_search_tree<tkey, tvalue, tkey_comparer>::node**>(&(_remove_path->top()->left)));
                        }
                        else {
                            _this->rotate_left(reinterpret_cast<typename binary_search_tree<tkey, tvalue, tkey_comparer>::node**>(&(_remove_path->top()->right)));
                        }
                    }
                    else {
                        _this->rotate_left(reinterpret_cast<typename binary_search_tree<tkey, tvalue, tkey_comparer>::node**>(rb_root));
                    }
                }

                //ЧК4 — родитель чёрный, левый сын красный, у правого внука левый правнук красный
                else if (dad->node_color == color::black && brother->node_color == color::red && brother->left != nullptr && reinterpret_cast<rb_node*>(brother->left)->node_color == color::black && reinterpret_cast<rb_node*>(brother->left)->right != nullptr && reinterpret_cast<rb_node*>(reinterpret_cast<rb_node*>(brother->left)->right)->node_color == color::red) {
                    reinterpret_cast<rb_node*>(reinterpret_cast<rb_node*>(brother->left)->right)->node_color = color::black;
                    _this->rotate_right(reinterpret_cast<typename binary_search_tree<tkey, tvalue, tkey_comparer>::node**>(&(dad->right)));
                    if (!_remove_path->empty()) {
                        if (_remove_path->top()->left == dad) {
                            _this->rotate_left(reinterpret_cast<typename binary_search_tree<tkey, tvalue, tkey_comparer>::node**>(&(_remove_path->top()->left)));
                        }
                        else {
                            _this->rotate_left(reinterpret_cast<typename binary_search_tree<tkey, tvalue, tkey_comparer>::node**>(&(_remove_path->top()->right)));
                        }
                    }
                    else {
                        _this->rotate_left(reinterpret_cast<typename binary_search_tree<tkey, tvalue, tkey_comparer>::node**>(rb_root));
                    }
                }

                //ЧК4 — родитель чёрный, левый сын красный, у правого внука правый правнук красный
                else if (dad->node_color == color::black && brother->node_color == color::red && brother->left != nullptr && reinterpret_cast<rb_node*>(brother->left)->node_color == color::black && reinterpret_cast<rb_node*>(brother->left)->left != nullptr && reinterpret_cast<rb_node*>(reinterpret_cast<rb_node*>(brother->left)->left)->node_color == color::red) {
                    reinterpret_cast<rb_node*>(reinterpret_cast<rb_node*>(brother->left)->left)->node_color = color::black;
                    reinterpret_cast<rb_node*>(brother->left)->node_color = color::red;
                    _this->rotate_right(reinterpret_cast<typename binary_search_tree<tkey, tvalue, tkey_comparer>::node**>(&(brother->left)));

                    reinterpret_cast<rb_node*>(reinterpret_cast<rb_node*>(brother->left)->right)->node_color = color::black;
                    _this->rotate_right(reinterpret_cast<typename binary_search_tree<tkey, tvalue, tkey_comparer>::node**>(&(dad->right)));
                    if (!_remove_path->empty()) {
                        if (_remove_path->top()->left == dad) {
                            _this->rotate_left(reinterpret_cast<typename binary_search_tree<tkey, tvalue, tkey_comparer>::node**>(&(_remove_path->top()->left)));
                        }
                        else {
                            _this->rotate_left(reinterpret_cast<typename binary_search_tree<tkey, tvalue, tkey_comparer>::node**>(&(_remove_path->top()->right)));
                        }
                    }
                    else {
                        _this->rotate_left(reinterpret_cast<typename binary_search_tree<tkey, tvalue, tkey_comparer>::node**>(rb_root));
                    }
                }

                //ЧЧ5 — родитель чёрный, левый сын чёрный с правым красным внуком.
                else if (dad->node_color == color::black && brother->node_color == color::black && brother->left != nullptr && reinterpret_cast<rb_node*>(brother->left)->node_color == color::red) {
                    reinterpret_cast<rb_node*>(brother->left)->node_color = color::black;
                    _this->rotate_right(reinterpret_cast<typename binary_search_tree<tkey, tvalue, tkey_comparer>::node**>(&(dad->right)));
                    if (!_remove_path->empty()) {
                        if (_remove_path->top()->left == dad) {
                            _this->rotate_left(reinterpret_cast<typename binary_search_tree<tkey, tvalue, tkey_comparer>::node**>(&(_remove_path->top()->left)));
                        }
                        else {
                            _this->rotate_left(reinterpret_cast<typename binary_search_tree<tkey, tvalue, tkey_comparer>::node**>(&(_remove_path->top()->right)));
                        }
                    }
                    else {
                        _this->rotate_left(reinterpret_cast<typename binary_search_tree<tkey, tvalue, tkey_comparer>::node**>(rb_root));
                    }
                }

                //ЧЧ5 — родитель чёрный, левый сын чёрный с левым красным внуком.
                else if (dad->node_color == color::black && brother->node_color == color::black && brother->right != nullptr && reinterpret_cast<rb_node*>(brother->right)->node_color == color::red) {
                    reinterpret_cast<rb_node*>(brother->right)->node_color = color::black;

                    if (!_remove_path->empty()) {
                        if (_remove_path->top()->left == dad) {
                            _this->rotate_left(reinterpret_cast<typename binary_search_tree<tkey, tvalue, tkey_comparer>::node**>(&(_remove_path->top()->left)));
                        }
                        else {
                            _this->rotate_left(reinterpret_cast<typename binary_search_tree<tkey, tvalue, tkey_comparer>::node**>(&(_remove_path->top()->right)));
                        }
                    }
                    else {
                        _this->rotate_left(reinterpret_cast<typename binary_search_tree<tkey, tvalue, tkey_comparer>::node**>(rb_root));
                    }
                }

                //Ч6 — родитель чёрный, левый сын чёрный, его внуки тоже чёрные
                else if (dad->node_color == color::black && brother->node_color == color::black && (brother->left == nullptr || reinterpret_cast<rb_node*>(brother->left)->node_color == color::black) && ((brother->right == nullptr || reinterpret_cast<rb_node*>(brother->right)->node_color == color::black))) {
                    brother->node_color = color::red;
                    if (!_remove_path->empty()) {
                        if (_remove_path->top()->left == dad) {
                            side = 0;
                        }
                        else {
                            side = 1;
                        }
                        try {
                            remove_balance(_remove_path, side, additional, rb_root);
                        }
                        catch (const logic_error& ex) {
                            throw ex;
                        }
                    }
                }
                else {
                    throw logic_error("*** Not expected situation ***");
                }
            }
        }

    public:

        void after_remove(const tkey& key, typename binary_search_tree<tkey, tvalue, tkey_comparer>::node** root, logger* logger, stack<typename binary_search_tree<tkey, tvalue, tkey_comparer>::node*>* _r_path, size_t& side, size_t& additional) override {
            auto** rb_root = reinterpret_cast<rb_node**>(root);
            auto* _remove_path = reinterpret_cast<stack<rb_node*>*>(_r_path);

            if ((*rb_root) != nullptr) {
                if ((*rb_root)->left == nullptr && (*rb_root)->right == nullptr) {
                    (*rb_root)->node_color = color::black; // корень
                }
                else {
                    if (additional != 0) { //чёрный
                        if (side == 0) { //слева от бати
                            if (reinterpret_cast<typename binary_search_tree<tkey, tvalue, tkey_comparer>::node*>(_remove_path->top())->left != nullptr) {

                                reinterpret_cast<rb_node*>(reinterpret_cast<typename binary_search_tree<tkey, tvalue, tkey_comparer>::node*>(_remove_path->top())->left)->node_color = color::black;
                                if (logger != nullptr) {
                                    logger->log("after_remove: tree was balanced!", logger::severity::debug);
                                }
                                return;
                            }
                            else {
                                try {
                                    remove_balance(_remove_path, side, additional, rb_root);
                                }
                                catch (const logic_error& ex) {
                                    if (logger != nullptr) {
                                        logger->log(ex.what(), logger::severity::warning);
                                    }
                                }
                                if (logger != nullptr) {
                                    logger->log("after_remove: tree was balanced!", logger::severity::debug);
                                }
                                return;
                            }
                        }
                        else {
                            if (reinterpret_cast<typename binary_search_tree<tkey, tvalue, tkey_comparer>::node*>(_remove_path->top())->right != nullptr) {

                                reinterpret_cast<rb_node*>(reinterpret_cast<typename binary_search_tree<tkey, tvalue, tkey_comparer>::node*>(_remove_path->top())->right)->node_color = color::black;
                                if (logger != nullptr) {
                                    logger->log("after_remove: tree was balanced!", logger::severity::debug);
                                }
                                return;
                            }
                            else {
                                try {
                                    remove_balance(_remove_path, side, additional, rb_root);
                                }
                                catch (const logic_error& ex) {
                                    if (logger != nullptr) {
                                        logger->log(ex.what(), logger::severity::warning);
                                    }
                                }
                                if (logger != nullptr) {
                                    logger->log("after_remove: tree was balanced!", logger::severity::debug);
                                }
                                return;
                            }
                        }
                    }
                }
            }

            if (logger != nullptr) {
                logger->log("after_remove: tree was balanced!", logger::severity::debug);
            }
        }

    };

private:

    void copy_additional_data(typename binary_search_tree<tkey, tvalue, tkey_comparer>::node* copied_node, typename binary_search_tree<tkey, tvalue, tkey_comparer>::node* to_copy) const override {
        auto* rb_copied_node = reinterpret_cast<rb_node*>(copied_node);
        auto* rb_to_copy = reinterpret_cast<rb_node*>(to_copy);
        rb_copied_node->node_color = rb_to_copy->node_color;
    }

    size_t get_node_size() const override {
        return sizeof(rb_node);
    }

    void get_node_constructor(typename binary_search_tree<tkey, tvalue, tkey_comparer>::node* copied_node, typename binary_search_tree<tkey, tvalue, tkey_comparer>::node* to_copy) const override {
        auto* rb_copied_node = reinterpret_cast<rb_node*>(copied_node);
        auto* rb_to_copy = reinterpret_cast<rb_node*>(to_copy);
        new (rb_copied_node) rb_node{ rb_to_copy->key, rb_to_copy->value, reinterpret_cast<rb_node*>(this->copy_inner(rb_to_copy->left)), reinterpret_cast<rb_node*>(this->copy_inner(rb_to_copy->right)), rb_to_copy->node_color };
    }

public:

    explicit red_black_tree(memory* alloc = nullptr, logger* logger = nullptr) :
        binary_search_tree<tkey, tvalue, tkey_comparer>(alloc, logger, new typename binary_search_tree<tkey, tvalue, tkey_comparer>::find_template_method(), new rb_insert_template_method(this), new rb_remove_template_method(this))
    {
        if (logger != nullptr) {
            logger->log("red_black_tree was successfully created!", logger::severity::debug);
        }
    }

    red_black_tree(const red_black_tree<tkey, tvalue, tkey_comparer>& tree) {
        this->_logger = tree._logger;
        this->_allocator = tree._allocator;
        this->_find = new typename binary_search_tree<tkey, tvalue, tkey_comparer>::find_template_method();
        this->_insert = new rb_insert_template_method(this);
        this->_remove = new rb_remove_template_method(this);
        this->_root = tree.copy();
        if (this->_logger != nullptr) {
            this->_logger->log("red_black_tree was copied!", logger::severity::debug);
        }
    }

    red_black_tree(red_black_tree<tkey, tvalue, tkey_comparer>&& tree) noexcept {
        this->_root = tree._root;
        this->_allocator = tree._allocator;
        this->_logger = tree._logger;
        this->_find = new typename binary_search_tree<tkey, tvalue, tkey_comparer>::find_template_method();
        this->_insert = new rb_insert_template_method(this);
        this->_remove = new rb_remove_template_method(this);
        delete tree._find;
        delete tree._insert;
        delete tree._remove;
        tree._allocator = nullptr;
        tree._logger = nullptr;
        tree._insert = nullptr;
        tree._find = nullptr;
        tree._remove = nullptr;
        tree._root = nullptr;
        if (this->_logger) this->_logger->log("red_black_tree movement is complete!", logger::severity::debug);
    }

    red_black_tree<tkey, tvalue, tkey_comparer>& operator=(const red_black_tree<tkey, tvalue, tkey_comparer>& tree) {
        if (this != &tree) {
            this->clear();
            this->_root = tree.copy();
        }
        if (this->_logger != nullptr) this->_logger->log("red_black_tree was assigned!", logger::severity::debug);
        return *this;
    }

    red_black_tree<tkey, tvalue, tkey_comparer>& operator=(red_black_tree<tkey, tvalue, tkey_comparer>&& tree) noexcept {
        if (&tree != this) {
            this->clear();
            this->_root = tree._root;
            this->_logger = tree._logger;
            this->_allocator = tree._allocator;
            delete tree._find;
            delete tree._insert;
            delete tree._remove;
            tree._root = nullptr;
            tree._allocator = nullptr;
            tree._logger = nullptr;
            tree._insert = nullptr;
            tree._find = nullptr;
            tree._remove = nullptr;
        }
        if (this->_logger != nullptr) {
            this->_logger->log("red_black_tree movement is complete!", logger::severity::debug);
        }
        return *this;
    }

    ~red_black_tree() {
        if (this->_logger) {
            this->_logger->log("red_black_tree was succesfully deleted!", logger::severity::debug);
        }
    }
};

#endif //CWW_RED_BLACK_TREE_H
