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
		// 插入点成为根
		root_ = z;
		return;
	}
	// 寻找插入点
	Node_ptr y = root_;
	Node_ptr y_next = std::less<Value>()(y->value, z->value) ? 
			y->right : y->left;
	while (y_next) {
		y = y_next;
		y_next = std::less<Value>()(y->value, z->value) ? y->right : y->left;
	}
	// 插入
	std::less<Value>()(y->value, z->value) ? y->right = z : y->left = z;
	z->parent = y;
}

template<typename Value>
void Rb_tree<Value>::delete_node (const Value& v) {
	std::pair<Node_ptr, Node_ptr> pr = tree_delete(v);
	if (!pr.second) {
		// 空树，或没找到
		return;
	}
	if (pr.first == nullptr || pr.second->color == red) {
		// 删除后成为空树了，或者被删的是红结点
		delete pr.second;
		return;
	}
	rebalance_after_delete(pr.first);

}

template<typename Value>
std::pair<Rb_tree_node<Value>*, Rb_tree_node<Value>*> 
Rb_tree<Value>::tree_delete (const Value& v) {
	// 返回的是指向删除后可能破坏性质的结点，以及被删除点的指针（结点已被摘下）
	// 寻找删除点
	Node_ptr x = root_;
	while (x && x->value != v) {
		std::less<Value>()(v, x->value) ? x = x->left : x = x->right;
	}
	if (!x) {
		// 空树，或没找到
		return std::make_pair<Node_ptr, Node_ptr>(nullptr, nullptr);
	}
	// 转嫁删除点
	// 只有一个或者没有孩子，则该结点就是要实际删除的
	// x指向实际删除的结点，y指向等于v的结点
	Node_ptr y = x;
	if (x->left && x->right) {
		// 如果两个孩子，则寻找左子树里的最大结点
		x = y->left;
		while (x->right){
			x = x->right;
		}
	}
	// 现在x至多只有一个孩子
	// 获得x唯一的孩子，如果有的话
	// w是可能破坏性质的结点
	Node_ptr w = x->left ? x->left : x->right;
	// w先连到新父节点
	if (w) {
		w->parent = x->parent;
	}
	// x的父节点连到新儿子
	// 若x是根
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
	// 黑父的情况下，所有性质均满足
	if (z->parent->color == black) {
		z->color = red;
		return;
	}
	// 以下为红父
	z->color = red;
	// 祖父结点一定不是nullptr
	// 因为若是nullptr，则root是其父亲，但root是黑色
	
	while (z != root_ && z->parent->color == red) {
		if (z->parent->parent->left == z->parent) {
		// 父亲在左子树上
			// 叔叔结点可能不存在
			Node_ptr uncle = z->parent->parent->right;
			if (uncle && uncle->color == red) {
				// 红叔。沉色
				z->parent->parent->color = red;
				z->parent->color = black;
				uncle->color = black;
				z = z->parent->parent;
			}
			else {
				if (z == z->parent->right) {
				// 插入点是右子。左旋
					z = z->parent;
					left_rotate(z);
				}
				// 插入点变为左子了。沉色，右旋
				// 叔叔不存在的情况也隐含在这里
				z->parent->parent->color = red;
				z->parent->color = black;
				right_rotate (z->parent->parent);
			}
		}
		else{
			// 父亲在右子树上
			// 叔叔结点可能不存在
			Node_ptr uncle = z->parent->parent->left;
			if (uncle && uncle->color == red) {
				// 红叔。沉色
				z->parent->parent->color = red;
				z->parent->color = black;
				uncle->color = black;
				z = z->parent->parent;
			}
			else {
				if (z == z->parent->left) {
				// 插入点是右子。左旋
					z = z->parent;
					right_rotate(z);
				}
					// 插入点变为左子了。沉色，右旋
			// 叔叔不存在的情况也隐含在这里
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
	// 如果x是黑色，则向根靠拢 
	while (x != root_ && x->color == black) {
		if (x->parent->left == x) {
			// x是左孩子
			Node_ptr brathor = x->parent->right;
			if (brathor->color == red) {
				// 红兄转为黑兄：父亲染红，兄弟染黑，以父亲左旋
				brathor->color = black;
				x->parent->color = red;
				x = x->parent;
				left_rotate(x);
				// 更新指向兄弟的指针
				brathor = x->parent->right;
			}
			else {
				// 黑兄，孩子不一定存在
				// 孩子不存在的时候默认为黑色
				Color brathor_left_color = brathor->left ? brathor->left->color : black;
				Color brathor_right_color = brathor->right ? brathor->right->color : red;
				if (brathor_left_color == black && brathor_right_color == black) {
						// 黑兄，一家黑
						brathor->color = red;
						x = x->parent;
				}
				else if ((brathor->left != nullptr && brathor->left->color == red) && 
					(brathor_right_color == black)) {
					// 黑兄，左孩子为红，右孩子为黑
					// 左孩子必须存在，因为要右旋；右孩子可以不存在
					brathor->color = red;
					brathor->left->color = black;
					right_rotate(brathor);
				}
				else if (brathor->right != nullptr && brathor->right->color == red) {
					// 黑兄，右孩子为红色，左孩子随便
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
			// x是右孩子
			Node_ptr brathor = x->parent->left;
			if (brathor->color == red) {
				brathor->color = black;
				x->parent->color = red;
				x = x->parent;
				right_rotate(x);
				brathor = x->parent->left;
			}
			else {
				// 黑兄，孩子不一定存在
				// 孩子不存在的时候默认为黑色
				Color brathor_right_color = brathor->right ? brathor->right->color : black;
				Color brathor_left_color = brathor->left ? brathor->left->color : red;
				if (brathor_right_color == black && brathor_left_color == black) {
						// 黑兄，一家黑
						brathor->color = red;
						x = x->parent;
				}
				else if ((brathor->right != nullptr && brathor->right->color == red) && 
					(brathor_left_color == black)) {
					// 黑兄，左孩子为红，右孩子为黑
					// 左孩子必须存在，因为要右旋；右孩子可以不存在
					brathor->color = red;
					brathor->right->color = black;
					left_rotate(brathor);
				}
				else if (brathor->left != nullptr && brathor->left->color == red) {
					// 黑兄，右孩子为红色，左孩子随便
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
	// 如果x是红色，直接染黑
	x->color = black;
}

template<typename Value>
void Rb_tree<Value>::left_rotate (Node_ptr z) {
	// 轴点和右孩子一定存在
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
	// 轴点和左孩子一定存在
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