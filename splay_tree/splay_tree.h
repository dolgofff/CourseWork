#ifndef CWW_SPLAY_TREE_H
#define CWW_SPLAY_TREE_H
#include "binary_search_tree.h"
using namespace std;
/*template <typename tkey, typename tvalue, typename tkey_comparer>
class splay_tree final : public binary_search_tree<tkey, tvalue, tkey_comparer>
{

private:

	struct splay_node : public binary_search_tree<tkey, tvalue, tkey_comparer>::node {
		splay_node() = default;

		splay_node(const tkey& k, const tvalue& v, avl_node* l, avl_node* r) :binary_search_tree<tkey, tvalue, tkey_comparer>::node(k, v, l, r) {}

		virtual ~splay_node() = default;

	};

private:
											   //#### INSERTING ####//
	class splay_tree_insert_template_method final: public binary_search_tree<tkey, tvalue, tkey_comparer>::insert_template_method
	{

	public:
		explicit splay_tree_insert_template_method(splay_tree<tkey, tvalue, tkey_comparer>* new_st) :
			binary_search_tree<tkey,tvalue,tkey_comparer>::insert_template_method(new_st) {}

	private:
		splay_tree<tkey, tvalue, tkey_comparer>* _this;

	private:
		size_t get_node_size() const override {
			return sizeof(splay_node);
		}

		void get_node_constructor(typename binary_search_tree<tkey, tvalue, tkey_comparer>::node** x, const tkey& key, const tvalue& value) const override {
			new (*(reinterpret_cast<splay_node**>(x))) splay_node{ key,value,nullptr,nullptr};
		}

	protected:

		void after_insert(const tkey& key, const tvalue& value, typename binary_search_tree<tkey, tvalue, tkey_comparer>::node** root, logger* logger, stack<typename binary_search_tree<tkey, tvalue, tkey_comparer>::node*>* _i_path) override {
			auto** st_root = reinterpret_cast<splay_node**>(root);
			auto* _insert_path = reinterpret_cast<stack<splay_node*>*>(_i_path);

			_this->splay(_insert_path, st_root);
			if (logger != nullptr) {
				logger->log("after_insert:Tree was splayed!", logger::severity::debug);
			}
		}
	};

private:
											   //#### REMOVING ####//
	class splay_tree_remove_template_method final : public binary_search_tree<tkey, tvalue, tkey_comparer>::remove_template_method
	{

	public:
		explicit splay_tree_remove_template_method(splay_tree<tkey, tvalue, tkey_comparer>* this_st) :
			binary_search_tree<tkey, tvalue, tkey_comparer>::remove_template_method(this_st) {}

	private:

		splay_tree<tkey, tvalue, tkey_comparer>* _this;

	private:

		void get_node_destructor(typename binary_search_tree<tkey, tvalue, tkey_comparer>::node* x) const override {
			auto* y = reinterpret_cast<splay_node*>(x);
			y->~splay_node();
		}

	private:
		tvalue remove_concrete(const tkey& key, node** root, memory* alloc, logger* logger, stack<node*>* _remove_path, size_t& side, size_t& additional) override {

			if (root == nullptr)
			{
				logger->log("Invalid key to remove",logger::severity::debug);
			}
			if (comparer(key, root->key) == 0)
			{
				tvalue removing_value = root->value;

				_this->splay(_remove_path,root);

				typename binary_search_tree<tkey, tvalue, tkey_comparer>::node* left_subtree = root->left;
				typename binary_search_tree<tkey, tvalue, tkey_comparer>::node* right_subtree = root->right;

				root->~node();
				_this->deallocate(root);
				root = nullptr;

				_this->_root = _this->merge(left_subtree, right_subtree);

				return removing_value;
			}

			typename binary_search_tree<tkey, tvalue, tkey_comparer>::node* next_node = _tree->comparer(key, root->key) > 0 ? root->right : root->left;
			_remove_path->push(&root);
			tvalue removing_value = remove_inner(key, next_node, path_to_subtree_root_exclusive);

			return removing_value;
		}

	protected:
		void after_remove(const tkey& key, node** root, logger* logger, stack<node*>* _remove_path, size_t& side, size_t& additional) override {}
	};

private:
											  // SEARCHING //
	class splay_tree_find_template_method final : public binary_search_tree<tkey, tvalue, tkey_comparer>::find_template_method
	{

	public:
		explicit splay_tree_find_template_method(splay_tree<tkey,tvalue,tkey_comparer>* this_st) :
			binary_search_tree<tkey,tvalue,tkey_comparer>::find_template_method(this_st) {}

	private:

		splay_tree<tkey, tvalue, tkey_comparer>* _this;

	protected:
		void after_find(typename associative_container<tkey, tvalue>::key_value_pair* target_key_and_result_value, node* root, stack<node*>* _insert_path) const override {

			_this->splay(_insert_path,root);
		}
	};

											 // SPLAYING //
private:

	void splay(stack <splay_node*>* _insert_path, splay_node** st_root) {

		splay_node* parent = nullptr;
		splay_node* grand_parent = nullptr;
		splay_node* great_grand_parent = nullptr;
		splay_node* current_node =  _insert_path->top();
		_insert_path->pop();

		while (!_insert_path->empty())
		{
			//definition
			parent = _insert_path->top();
			_insert_path->pop();

			if (!_insert_path->empty())
			{
				grand_parent = _insert_path->top();
				_insert_path->pop();

				if (!_insert_path->empty())
				{
					great_grand_parent = _insert_path->top();
					_insert_path->pop();
				}
			}

			//���� �������� ����� ��������
			if ((_insert_path->top()->left) == current_node){

				//Zig: � ���� ��� �����������, �� �������� ����� �������� ����� => ������ �������
				if (grand_parent == nullptr){
					_this->rotate_right(reinterpret_cast<typename binary_search_tree<tkey, tvalue, tkey_comparer>::node**>(parent));
					current_node = parent;
				}

				//Zig-zag: ���� �������� ����� �������� ��������,� �������� �������� ������ �������� �����������
				else {

					if (grand_parent->right == parent)
					{
						_this->rotate_right(reinterpret_cast<typename binary_search_tree<tkey, tvalue, tkey_comparer>::node**>(&grand_parent->right));

						if (great_grand_parent != nullptr) {

							if (great_grand_parent->right = grand_parent) {
								_this->rotate_left(reinterpret_cast<typename binary_search_tree<tkey, tvalue, tkey_comparer>::node**>(&great_grand_parent->right));
							}

							else {
								_this->rotate_left(reinterpret_cast<typename binary_search_tree<tkey, tvalue, tkey_comparer>::node**>(&great_grand_parent->left));
							}

							current_node = grand_parent;
						}

						else {
							_this->rotate_left(reinterpret_cast<typename binary_search_tree<tkey, tvalue, tkey_comparer>::node**>(&grand_parent));
							current_node = grand_parent;
						}
					}

					//Zig-zig: ���� �������� ����� �������� ��������, � �������� ����� �����������
					else {
						_this->rotate_right(reinterpret_cast<typename binary_search_tree<tkey, tvalue, tkey_comparer>::node**>(&grand_parent->left));

						if (great_grand_parent != nullptr) {

							if (great_grand_parent->right = grand_parent) {
								_this->rotate_right(reinterpret_cast<typename binary_search_tree<tkey, tvalue, tkey_comparer>::node**>(&great_grand_parent->right));
							}

							else {
								_this->rotate_right(reinterpret_cast<typename binary_search_tree<tkey, tvalue, tkey_comparer>::node**>(&great_grand_parent->left));
							}

							current_node = grand_parent;
						}

						else {
							_this->rotate_right(reinterpret_cast<typename binary_search_tree<tkey, tvalue, tkey_comparer>::node**>(&grand_parent));
							current_node = grand_parent;
						}
					}
				}
			}



			//���� �������� ������ ��������
			else
			{
				//Zag: � ���� ��� �����������, �� �������� ������ �������� ����� => ����� �������
				if (grand_parent == nullptr) {
					_this->rotate_left(reinterpret_cast<typename binary_search_tree<tkey, tvalue, tkey_comparer>::node**>(parent));
					current_node = parent;
				}

				//Zag-zig: ���� �������� ������ �������� ��������,� �������� �������� ����� �������� �����������
				else {

					if (grand_parent->left == parent)
					{
						_this->rotate_left(reinterpret_cast<typename binary_search_tree<tkey, tvalue, tkey_comparer>::node**>(&grand_parent->left));

						if (great_grand_parent != nullptr) {

							if (great_grand_parent->right = grand_parent) {
								_this->rotate_right(reinterpret_cast<typename binary_search_tree<tkey, tvalue, tkey_comparer>::node**>(&great_grand_parent->right));
							}

							else {
								_this->rotate_right(reinterpret_cast<typename binary_search_tree<tkey, tvalue, tkey_comparer>::node**>(&great_grand_parent->left));
							}

							current_node = grand_parent;
						}

						else {
							_this->rotate_right(reinterpret_cast<typename binary_search_tree<tkey, tvalue, tkey_comparer>::node**>(&grand_parent));
							current_node = grand_parent;
						}
					}

					//Zag-zag: ���� �������� ������ �������� ��������, � �������� ������ �����������
					else {
						_this->rotate_left(reinterpret_cast<typename binary_search_tree<tkey, tvalue, tkey_comparer>::node**>(&grand_parent->right));

						if (great_grand_parent != nullptr) {

							if (great_grand_parent->right = grand_parent) {
								_this->rotate_left(reinterpret_cast<typename binary_search_tree<tkey, tvalue, tkey_comparer>::node**>(&great_grand_parent->right));
							}

							else {
								_this->rotate_left(reinterpret_cast<typename binary_search_tree<tkey, tvalue, tkey_comparer>::node**>(&great_grand_parent->left));
							}

							current_node = grand_parent;
						}

						else {
							_this->rotate_left(reinterpret_cast<typename binary_search_tree<tkey, tvalue, tkey_comparer>::node**>(&grand_parent));
							current_node = grand_parent;
						}
					}
				}
			}

			if (great_grand_parent != nullptr){

				_insert_path->push(great_grand_parent);
			}

			parent = nullptr;
			grand_parent = nullptr;
			great_grand_parent = nullptr;

		}

	}

	private:

	typename binary_search_tree<tkey, tvalue, tkey_comparer>::node* merge(typename binary_search_tree<tkey, tvalue, tkey_comparer>::node* tree_first, typename binary_search_tree<tkey, tvalue, tkey_comparer>::node* tree_second)
	{
		if (tree_first == nullptr && tree_second != nullptr)
		{
			return tree_second;
		}

		if (tree_first != nullptr && tree_second == nullptr)
		{
			return tree_first;
		}

		if (tree_first != nullptr && tree_second != nullptr)
		{
			stack<typename binary_search_tree<tkey, tvalue, tkey_comparer>::node*>* path;
			typename binary_search_tree<tkey, tvalue, tkey_comparer>::node* max_node = tree_first;
			typename binary_search_tree<tkey, tvalue, tkey_comparer>::node** parent = &tree_first;

			while (max_node->right_subtree != nullptr)
			{
				path->push(parent);
				parent = &(max_node->right_subtree);
				max_node = max_node->right_subtree;
			}

			splay(path,max_node);

			tree_first->right_subtree = tree_second;

			return tree_first;
		}

		return nullptr;
	}



	void copy_additional_data(typename binary_search_tree<tkey, tvalue, tkey_comparer>::node* copied, typename binary_search_tree<tkey, tvalue, tkey_comparer>::node* to_copy) const override {

		auto* st_copied_node = reinterpret_cast<splay_node*>(copied);
		auto* st_to_copy = reinterpret_cast<splay_node*>(to_copy);
	}

	size_t get_node_size() const override {
		return sizeof(splay_node);
	}

	void get_node_constructor(typename binary_search_tree<tkey, tvalue, tkey_comparer>::node* copied, typename binary_search_tree<tkey, tvalue, tkey_comparer>::node* to_copy) const override {

		auto* st_copied_node = reinterpret_cast<splay_node*>(copied);
		auto* st_to_copy = reinterpret_cast<splay_node*>(to_copy);

		new (st_copied_node) splay_node{ st_to_copy->key,st_to_copy->value,reinterpret_cast<splay_node*>(this->copy_inner(st_to_copy->left)),reinterpret_cast<splay_node*>(this->copy_inner(st_to_copy->right))};
	}



public:

		  // REWRITE 4 FIND !!!!
	explicit splay_tree(memory* alloc = nullptr, logger* logger = nullptr) :
		binary_search_tree<tkey, tvalue, tkey_comparer>(alloc, logger, new typename binary_search_tree<tkey, tvalue, tkey_comparer>::find_template_method(), new splay_tree_insert_template_method(this), new splay_tree_remove_template_method(this))
	{
		if (logger != nullptr) {
			logger->log("SPLAY_tree CREATED!", logger::severity::debug);
		}
	}

	splay_tree(const splay_tree<tkey, tvalue, tkey_comparer>& tree) {
		this->_logger = tree._logger;
		this->_allocator = tree._allocator;
		this->_find = new  splay_tree_find_template_method(this);
		this->_insert = new splay_tree_insert_template_method(this);
		this->_remove = new splay_tree_remove_template_method(this);
		this->_root = tree.copy();
		if (this->_logger != nullptr) {
			this->_logger->log("SPLAY_tree COPIED!", logger::severity::debug);
		}
	}

	avl_tree(avl_tree<tkey, tvalue, tkey_comparer>&& tree) noexcept {
		this->_root = tree._root;
		this->_allocator = tree._allocator;
		this->_logger = tree._logger;
		this->_find = new splay_tree_find_template_method(this);
		this->_insert = new splay_tree_insert_template_method(this);
		this->_remove = new splay_tree_remove_template_method(this);
		delete tree._find;
		delete tree._insert;
		delete tree._remove;
		tree._allocator = nullptr;
		tree._logger = nullptr;
		tree._insert = nullptr;
		tree._find = nullptr;
		tree._remove = nullptr;
		tree._root = nullptr;
		if (this->_logger) {
			this->_logger->log("SPLAY_tree MOVED!", logger::severity::debug);
		}
	}

	splay_tree<tkey, tvalue, tkey_comparer>& operator=(const splay_tree<tkey, tvalue, tkey_comparer>& tree) {
		if (this != &tree) {
			this->clear();
			this->_root = tree.copy();
		}
		if (this->_logger != nullptr) {
			this->_logger->log("SPLAY_tree ASSIGNED!", logger::severity::debug);
		}
		return *this;
	}

	splay_tree<tkey, tvalue, tkey_comparer>& operator=(splay_tree<tkey, tvalue, tkey_comparer>&& tree) noexcept {
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
			this->_logger->log("SPLAY_tree MOVED!", logger::severity::debug);
		}
		return *this;
	}

	~splay_tree() {
		if (this->_logger) this->_logger->log("SPLAY_tree DELETED!", logger::severity::debug);
	}
};*/

template<typename tkey, typename tvalue, typename tkey_comparer>
class splay_tree final : public binary_search_tree<tkey, tvalue, tkey_comparer> {


	// #### INSERTION #### //

	class splay_tree_insert_class final : public binary_search_tree<tkey, tvalue, tkey_comparer>::insert_template_method {

	private:

		splay_tree<tkey, tvalue, tkey_comparer>* _tree;

	public:

		explicit splay_tree_insert_class(splay_tree<tkey, tvalue, tkey_comparer>* tree) : binary_search_tree<tkey, tvalue, tkey_comparer>::insert_template_method(tree) {}

	public:

		void after_insert(const tkey& key, const tvalue& value, typename binary_search_tree<tkey, tvalue, tkey_comparer>::node** current_node, logger* logger, stack<typename binary_search_tree<tkey, tvalue, tkey_comparer>::node*>* insert_path) override {
			_tree->splay(current_node, insert_path);
		}
	};


	// #### SEARCH #### //

	class splay_tree_find_class final : public binary_search_tree<tkey, tvalue, tkey_comparer>::find_template_method {
	private:

		splay_tree<tkey, tvalue, tkey_comparer>* _tree;

	public:

		explicit splay_tree_find_class(splay_tree<tkey, tvalue, tkey_comparer>* tree) : binary_search_tree<tkey, tvalue, tkey_comparer>::find_template_method(tree) {}

	public:

		void after_find(typename associative_container<tkey, tvalue>::key_value_pair* target_key_value_pair, typename binary_search_tree<tkey, tvalue, tkey_comparer>::node** root, logger* logger, stack<typename binary_search_tree<tkey, tvalue, tkey_comparer>::node*>* find_path) override {
			_tree->splay(root, find_path);
		}
	};

	// #### REMOVING #### //
	class splay_tree_remove_class final : public binary_search_tree<tkey, tvalue, tkey_comparer>::remove_template_method {
	private:
		splay_tree<tkey, tvalue, tkey_comparer>* _tree;

	public:
		explicit splay_tree_remove_class(splay_tree<tkey, tvalue, tkey_comparer>* tree) : binary_search_tree<tkey, tvalue, tkey_comparer>::remove_template_method(tree) {}

		tvalue remove_concrete(const tkey& key, typename binary_search_tree<tkey, tvalue, tkey_comparer>::node** current_node, memory* allocator, logger* logger, stack<typename binary_search_tree<tkey, tvalue, tkey_comparer>::node*>* remove_path, size_t& side, size_t& additional) override {

			tkey_comparer comparator = tkey_comparer();

			typename binary_search_tree<tkey, tvalue, tkey_comparer>::node* cur_node = *current_node;

			if (*current_node == nullptr) {
				throw logic_error("Tree is empty!");
			}
			typename binary_search_tree<tkey, tvalue, tkey_comparer>::node* left_subtree = cur_node->left;
			typename binary_search_tree<tkey, tvalue, tkey_comparer>::node* right_subtree = cur_node->right;


			
			//tvalue removing_value;

			if (comparator(key, cur_node->key) == 0) {

				tvalue removing_value = cur_node->value;



				cur_node->~node();
				allocator->deallocate(cur_node);
				cur_node = nullptr;

				*current_node = _tree->merge(left_subtree, right_subtree, logger);


				return removing_value;
			}

			remove_path->push(cur_node);
			while ((cur_node->right != nullptr) && (comparator(cur_node->key, key) < 0) || (cur_node->left != nullptr) && (comparator(cur_node->key, key) > 0)) {

				if (comparator(cur_node->key, key) > 0) {

					cur_node = cur_node->left;
					remove_path->push(cur_node);
				}

				else {

					cur_node = cur_node->right;
					remove_path->push(cur_node);
				}

				if (comparator(key, cur_node->key) == 0) {

					_tree->splay(current_node, remove_path);

					cur_node = *current_node;
					left_subtree = cur_node->left;
					right_subtree = cur_node->right;
					tvalue removing_value = cur_node->value;



					cur_node->~node();
					allocator->deallocate(cur_node);
					cur_node = nullptr;

					*current_node = _tree->merge(left_subtree, right_subtree, logger);

					return removing_value;

				}
			}

			throw logic_error("splay_tree(removind process): Nothing to delete.");
		}

		void after_remove(const tkey& key, typename binary_search_tree<tkey, tvalue, tkey_comparer>::node** current_node, logger* logger, stack<typename binary_search_tree<tkey, tvalue, tkey_comparer>::node*>* remove_path, size_t& side, size_t& additional) override {}
	};

public:

	void splay(typename binary_search_tree<tkey, tvalue, tkey_comparer>::node** root, stack<typename binary_search_tree<tkey, tvalue, tkey_comparer>::node*>* splay_path) {


		if (splay_path->empty()) {
			return;
		}

		auto* cur_node = splay_path->top();
		splay_path->pop();

		//������������ �� ���������. ���� ��������.
		if (splay_path->empty()) {
			return;
		}

		while (!splay_path->empty()) {

			auto* parent = splay_path->top();
			splay_path->pop();

			//Zig: ���� �������� �������� �� ��������� � �����,������ ��� ����� 1 ������� �����/������
			if (splay_path->empty()) {

				if (parent->right == cur_node) {

					this->rotate_left(root);
				}

				else {

					this->rotate_right(root);
				}
			}

			//� ���� ���� �������� � �����������
			else {

				auto* grand_parent = splay_path->top();
				splay_path->pop();

				//�������� ����� �� ����
				if (grand_parent->left == parent) {

					//Zag-zig: ���� �������� ������ �������� ��������, �������� - ����� ������� �����������
					if (parent->right == cur_node) {

						this->rotate_left(&(grand_parent->left));

						if (splay_path->empty()) {

							this->rotate_right(root);
						}

						else {

							if (splay_path->top()->left == grand_parent) {

								this->rotate_right(&(splay_path->top()->left));
							}

							else {

								this->rotate_right(&(splay_path->top()->right));
							}
						}
					}

					//Zig-zig: ���� �������� ����� �������� ��������, �������� - ����� ������� �����������
					else {

						this->rotate_right(&(grand_parent->left));

						if (splay_path->empty()) {

							this->rotate_right(root);
						}
						else {

							if (splay_path->top()->left == grand_parent) {

								this->rotate_right(&(splay_path->top()->left));
							}

							else {

								this->rotate_right(&(splay_path->top()->right));
							}
						}
					}
				}

				//�������� ������ �� ����
				else {

					//Zig-zag: ���� �������� ����� �������� ��������, �������� - ������ ������� �����������
					if (parent->left == cur_node) {

						this->rotate_right(&(grand_parent->right));

						if (splay_path->empty()) {

							this->rotate_left(root);
						}

						else {
							if (splay_path->top()->left == grand_parent) {

								this->rotate_left(&(splay_path->top()->left));
							}

							else {

								this->rotate_left(&(splay_path->top()->right));
							}
						}
					}

					//Zag-zag: ���� �������� ������ �������� ��������, �������� - ������ ������� �����������
					else {

						this->rotate_left(&(grand_parent->right));

						if (splay_path->empty()) {

							this->rotate_left(root);
						}

						else {
							if (splay_path->top()->left == grand_parent) {

								this->rotate_left(&(splay_path->top()->left));
							}

							else {

								this->rotate_left(&(splay_path->top()->right));

							}
						}
					}
				}

			}
		}
	}
public:

	typename binary_search_tree<tkey, tvalue, tkey_comparer>::node* merge(typename binary_search_tree<tkey, tvalue, tkey_comparer>::node* left_subtree, typename binary_search_tree<tkey, tvalue, tkey_comparer>::node* right_subtree, logger* logged) {

		if (right_subtree == nullptr && left_subtree != nullptr) {

			return left_subtree;
		}

		if (left_subtree == nullptr && right_subtree != nullptr) {

			return right_subtree;
		}

		if (right_subtree != nullptr && left_subtree != nullptr) {

			typename binary_search_tree<tkey, tvalue, tkey_comparer>::node* curr_node = left_subtree;

			auto* merge_path = new stack<typename binary_search_tree<tkey, tvalue, tkey_comparer>::node*>();

			while (curr_node->right != nullptr) {

				merge_path->push(curr_node);
				curr_node = curr_node->right;
			}

			merge_path->push(curr_node);
			splay(&left_subtree, merge_path);
			left_subtree->right = right_subtree;

			return left_subtree;
		}

		return nullptr;
	}

public:
	explicit splay_tree(memory* allocated = nullptr, logger* logged = nullptr) : binary_search_tree<tkey, tvalue, tkey_comparer>(allocated, logged, new splay_tree_find_class(this), new splay_tree_insert_class(this), new splay_tree_remove_class(this)) {}

	//������� 5, ������ !!!find!!!

	splay_tree(const splay_tree<tkey, tvalue, tkey_comparer>& tree) {
		this->_logger = tree._logger;
		this->_allocator = tree._allocator;
		this->_find = new splay_tree_find_class(this);
		this->_insert = new splay_tree_insert_class(this);
		this->_remove = new splay_tree_remove_class(this);
		this->_root = tree.copy();
		if (this->_logger != nullptr) {
			this->_logger->log("SPLAY_tree COPIED!", logger::severity::debug);
		}
	}

	splay_tree(splay_tree<tkey, tvalue, tkey_comparer>&& tree) noexcept {
		this->_root = tree._root;
		this->_allocator = tree._allocator;
		this->_logger = tree._logger;
		this->_find = new splay_tree_find_class(this);
		this->_insert = new splay_tree_insert_class(this);
		this->_remove = new splay_tree_remove_class(this);
		delete tree._find;
		delete tree._insert;
		delete tree._remove;
		tree._allocator = nullptr;
		tree._logger = nullptr;
		tree._insert = nullptr;
		tree._find = nullptr;
		tree._remove = nullptr;
		tree._root = nullptr;
		if (this->_logger) {
			this->_logger->log("SPLAY_tree MOVED!", logger::severity::debug);
		}
	}

	splay_tree<tkey, tvalue, tkey_comparer>& operator=(const splay_tree<tkey, tvalue, tkey_comparer>& tree) {
		if (this != &tree) {
			this->clear();
			this->_root = tree.copy();
		}
		if (this->_logger != nullptr) {
			this->_logger->log("SPLAY_tree ASSIGNED!", logger::severity::debug);
		}
		return *this;
	}

	splay_tree<tkey, tvalue, tkey_comparer>& operator=(splay_tree<tkey, tvalue, tkey_comparer>&& tree) noexcept {
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
			this->_logger->log("SPLAY_tree MOVED!", logger::severity::debug);
		}
		return *this;
	}

	~splay_tree() {
		if (this->_logger) this->_logger->log("SPLAY_tree DELETED!", logger::severity::debug);
	}
};


























#endif // CWW_SPLAY_TREE_H