#ifndef RB_TREE_H
#define RB_TREE_H
#include <functional>
#include <queue>
#include <utility>
#include <algorithm> 

enum Color:char{red, black};

template<typename Value>
struct Rb_tree_node {
    typedef Rb_tree_node<Value> Node;
    typedef Node* Node_ptr;
    Rb_tree_node (): parent(nullptr),
        left(nullptr), right(nullptr) {}
    Rb_tree_node (Value v) : parent(nullptr),
        left(nullptr), right(nullptr), value(v) {}
    Color color;
    Node_ptr parent;
    Node_ptr left;
    Node_ptr right;
    Value value;
};

template<typename Value>
class Rb_tree {
public:
    typedef Rb_tree_node<Value> Node;
    typedef Node* Node_ptr;
	Rb_tree () : root_(nullptr) {}
    void insert_node (const Value& v);
	void delete_node (const Value& v);
	void Bfs ();
private:
    void tree_insert (Node_ptr z);
	std::pair<Node_ptr, Node_ptr> tree_delete (const Value& v);
	void rebalance_after_insert (Node_ptr z);
	void rebalance_after_delete (Node_ptr z);
	void left_rotate (Node_ptr z);
	void right_rotate (Node_ptr z);
	Node_ptr root_;
};

template<typename Value>
void Rb_tree<Value>::insert_node (const Value& v) {
    Node_ptr x = new Node(v);
    tree_insert (x);
	rebalance_after_insert (x);
}

template<typename Value> 
void Rb_tree<Value>::tree_insert (Node_ptr z) {
	if (root_ == nullptr) {
		// ������Ϊ��
		root_ = z;
		return;
	}
	// Ѱ�Ҳ����
	Node_ptr y = root_;
	Node_ptr y_next = std::less<Value>()(y->value, z->value) ? 
			y->right : y->left;
	while (y_next) {
		y = y_next;
		y_next = std::less<Value>()(y->value, z->value) ? y->right : y->left;
	}
	// ����
	std::less<Value>()(y->value, z->value) ? y->right = z : y->left = z;
	z->parent = y;
}

template<typename Value>
void Rb_tree<Value>::delete_node (const Value& v) {
	std::pair<Node_ptr, Node_ptr> pr = tree_delete(v);
	if (!pr.second) {
		// ��������û�ҵ�
		return;
	}
	if (pr.first == nullptr || pr.second->color == red) {
		// ɾ�����Ϊ�����ˣ����߱�ɾ���Ǻ���
		delete pr.second;
		return;
	}
	rebalance_after_delete(pr.first);

}

template<typename Value>
std::pair<Rb_tree_node<Value>*, Rb_tree_node<Value>*> 
Rb_tree<Value>::tree_delete (const Value& v) {
	// ���ص���ָ��ɾ��������ƻ����ʵĽ�㣬�Լ���ɾ�����ָ�루����ѱ�ժ�£�
	// Ѱ��ɾ����
	Node_ptr x = root_;
	while (x && x->value != v) {
		std::less<Value>()(v, x->value) ? x = x->left : x = x->right;
	}
	if (!x) {
		// ��������û�ҵ�
		return std::make_pair<Node_ptr, Node_ptr>(nullptr, nullptr);
	}
	// ת��ɾ����
	// ֻ��һ������û�к��ӣ���ý�����Ҫʵ��ɾ����
	// xָ��ʵ��ɾ���Ľ�㣬yָ�����v�Ľ��
	Node_ptr y = x;
	if (x->left && x->right) {
		// ����������ӣ���Ѱ����������������
		x = y->left;
		while (x->right){
			x = x->right;
		}
	}
	// ����x����ֻ��һ������
	// ���xΨһ�ĺ��ӣ�����еĻ�
	// w�ǿ����ƻ����ʵĽ��
	Node_ptr w = x->left ? x->left : x->right;
	// w�������¸��ڵ�
	if (w) {
		w->parent = x->parent;
	}
	// x�ĸ��ڵ������¶���
	// ��x�Ǹ�
	if (x == root_) {
		root_ = w;
	}
	else {
		if (x == x->parent->left) {
			x->parent->left = w;
		}
		else {
			x->parent->right = w;
		}
	}
	if (x != y) {
		std::swap(y->color, x->color);
		std::swap(y->value, x->value);
	}
	x->parent = nullptr;
	x->left = nullptr;
	x->right = nullptr;
	return std::make_pair<Node_ptr, Node_ptr>(w, x);
}

template<typename Value>
void Rb_tree<Value>::rebalance_after_insert (Node_ptr z) {
	if (z == root_) {
		z->color = black;
		return;
	}
	// �ڸ�������£��������ʾ�����
	if (z->parent->color == black) {
		z->color = red;
		return;
	}
	// ����Ϊ�츸
	z->color = red;
	// �游���һ������nullptr
	// ��Ϊ����nullptr����root���丸�ף���root�Ǻ�ɫ
	
	while (z != root_ && z->parent->color == red) {
		if (z->parent->parent->left == z->parent) {
		// ��������������
			// ��������ܲ�����
			Node_ptr uncle = z->parent->parent->right;
			if (uncle && uncle->color == red) {
				// ���塣��ɫ
				z->parent->parent->color = red;
				z->parent->color = black;
				uncle->color = black;
				z = z->parent->parent;
			}
			else {
				if (z == z->parent->right) {
				// ����������ӡ�����
					z = z->parent;
					left_rotate(z);
				}
				// ������Ϊ�����ˡ���ɫ������
				// ���岻���ڵ����Ҳ����������
				z->parent->parent->color = red;
				z->parent->color = black;
				right_rotate (z->parent->parent);
			}
		}
		else{
			// ��������������
			// ��������ܲ�����
			Node_ptr uncle = z->parent->parent->left;
			if (uncle && uncle->color == red) {
				// ���塣��ɫ
				z->parent->parent->color = red;
				z->parent->color = black;
				uncle->color = black;
				z = z->parent->parent;
			}
			else {
				if (z == z->parent->left) {
				// ����������ӡ�����
					z = z->parent;
					right_rotate(z);
				}
					// ������Ϊ�����ˡ���ɫ������
			// ���岻���ڵ����Ҳ����������
				z->parent->parent->color = red;
				z->parent->color = black;
				left_rotate (z->parent->parent);
			}
		} // if-else
	} //while
	root_->color = black;
}

template<typename Value>
void Rb_tree<Value>::rebalance_after_delete (Node_ptr x) {
	// ���x�Ǻ�ɫ���������£ 
	while (x != root_ && x->color == black) {
		if (x->parent->left == x) {
			// x������
			Node_ptr brathor = x->parent->right;
			if (brathor->color == red) {
				// ����תΪ���֣�����Ⱦ�죬�ֵ�Ⱦ�ڣ��Ը�������
				brathor->color = black;
				x->parent->color = red;
				x = x->parent;
				left_rotate(x);
				// ����ָ���ֵܵ�ָ��
				brathor = x->parent->right;
			}
			else {
				// ���֣����Ӳ�һ������
				// ���Ӳ����ڵ�ʱ��Ĭ��Ϊ��ɫ
				Color brathor_left_color = brathor->left ? brathor->left->color : black;
				Color brathor_right_color = brathor->right ? brathor->right->color : red;
				if (brathor_left_color == black && brathor_right_color == black) {
						// ���֣�һ�Һ�
						brathor->color = red;
						x = x->parent;
				}
				else if ((brathor->left != nullptr && brathor->left->color == red) && 
					(brathor_right_color == black)) {
					// ���֣�����Ϊ�죬�Һ���Ϊ��
					// ���ӱ�����ڣ���ΪҪ�������Һ��ӿ��Բ�����
					brathor->color = red;
					brathor->left->color = black;
					right_rotate(brathor);
				}
				else if (brathor->right != nullptr && brathor->right->color == red) {
					// ���֣��Һ���Ϊ��ɫ���������
					brathor->color = x->parent->color;
					x->parent->color = black;
					if (brathor->left) {
						brathor->left->color = black;
					}
					x = x->parent;
					left_rotate(x);
				}
			}
		}
		else {
			// x���Һ���
			Node_ptr brathor = x->parent->left;
			if (brathor->color == red) {
				brathor->color = black;
				x->parent->color = red;
				x = x->parent;
				right_rotate(x);
				brathor = x->parent->left;
			}
			else {
				// ���֣����Ӳ�һ������
				// ���Ӳ����ڵ�ʱ��Ĭ��Ϊ��ɫ
				Color brathor_right_color = brathor->right ? brathor->right->color : black;
				Color brathor_left_color = brathor->left ? brathor->left->color : red;
				if (brathor_right_color == black && brathor_left_color == black) {
						// ���֣�һ�Һ�
						brathor->color = red;
						x = x->parent;
				}
				else if ((brathor->right != nullptr && brathor->right->color == red) && 
					(brathor_left_color == black)) {
					// ���֣�����Ϊ�죬�Һ���Ϊ��
					// ���ӱ�����ڣ���ΪҪ�������Һ��ӿ��Բ�����
					brathor->color = red;
					brathor->right->color = black;
					left_rotate(brathor);
				}
				else if (brathor->left != nullptr && brathor->left->color == red) {
					// ���֣��Һ���Ϊ��ɫ���������
					brathor->color = x->parent->color;
					x->parent->color = black;
					if (brathor->right) {
						brathor->right->color = black;
					}
					x = x->parent;
					right_rotate(x);
				}
			}
		}
	}
	// ���x�Ǻ�ɫ��ֱ��Ⱦ��
	x->color = black;
}

template<typename Value>
void Rb_tree<Value>::left_rotate (Node_ptr z) {
	// �����Һ���һ������
	Node_ptr w = z->right;
	if (w->left) {
		w->left->parent = z;
	}
	z->right = w->left;

	w->parent = z->parent;
	if (z->parent) {
		z->parent->left == z ? 
			z->parent->left = w : z->parent->right = w;
	}
	else {
		root_ = w;
	}

	z->parent = w;
	w->left = z;
}

template<typename Value>
void Rb_tree<Value>::right_rotate (Node_ptr z) {
	// ��������һ������
	Node_ptr w = z->left;
	if (w->right) {
		w->right->parent = z;
	}
	z->left = w->right;
	w->parent = z->parent;
	if (z->parent) {
		z->parent->left == z ? 
			z->parent->left = w : z->parent->right = w;
	}
	else {
		root_ = w;
	}
	z->parent = w;
	w->right = z;
}

template<typename Value>
void Rb_tree<Value>::Bfs () {
	std::queue<Node_ptr> ptrQueue;
	ptrQueue.push(root_);
	while (!ptrQueue.empty()) {
		Node_ptr p = ptrQueue.front();
		std::cout << p->value << ",";
		p->color == red ? std::cout<< "r" : std::cout<< "b";
		std::cout << std::endl;
		if (p->left)
			ptrQueue.push(p->left);
		if (p->right)
			ptrQueue.push(p->right);
		ptrQueue.pop();
	}
}

#endif // RB_TREE_HPP