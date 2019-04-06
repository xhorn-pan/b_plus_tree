#ifndef RB_TREE_H_
#define RB_TREE_H_

#include <cassert>
#include <iostream>

namespace cop5536 {
enum class node_color { red = 0, black };
typedef std::pair<long long, double> kv;

  struct node_ptr {
    node_ptr* left{nullptr};
    node_ptr* right{nullptr};
    node_ptr* parent{nullptr};
    node_ptr() : left(nullptr), right(nullptr), parent(nullptr) {};
    explicit node_ptr(node_ptr* parent) : left(nullptr), right(nullptr), parent(parent) {}
  };

  template <typename KV = kv> struct binary_tree_node : public node_ptr{
    KV kv_pair;
    friend bool operator==(const binary_tree_node<KV> &bt1,
			   const binary_tree_node<KV> &bt2) {
      return bt1.kv_pair.first == bt2->kv_pair.first;
    };
    friend bool operator<(const binary_tree_node<KV> &bt1,
			  const binary_tree_node<KV> &bt2) {
      return bt1.kv_pair.first < bt2.kv_pair.first;
    };
    friend bool operator>(const binary_tree_node<KV> &bt1,
			  const binary_tree_node<KV> &bt2) {
      return bt1.kv_pair.first > bt2.kv_pair.first;
    };
    binary_tree_node<KV>(KV kv): kv_pair(kv) {
      //this->left = dynamic_cast<binary_tree_node<KV> >
    };
  };

  template <typename KV = kv> struct rb_tree_node : binary_tree_node<KV> {
    node_color color;
    short int left_size;
    short int rank;
  };

class binary_tree {
public:
  binary_tree() : root(nullptr){};

protected:
  node_ptr *sibling_of_(node_ptr *node_);
  node_ptr *next_(node_ptr *node_);
  node_ptr *prev_(node_ptr *node_);
  void cut_out_(node_ptr *cnode, node_ptr *child_of_);
  void swap_parent_(node_ptr *old_node, node_ptr *new_node);
  void left_rotate(node_ptr *lnode);
  void right_rotate(node_ptr *rnode);
  virtual void copy_data(node_ptr *from, node_ptr *to);
  virtual node_ptr *search_(node_ptr *np);
  virtual node_ptr *delete_(node_ptr *np);

private:
  node_ptr *root;
};

inline node_ptr *binary_tree::sibling_of_(node_ptr *node_) {
  if (not node_->parent) {
    return nullptr;
  } else if (node_ == node_->parent->left) {
    return node_->parent->right;
  } else {
    return node_->parent->left;
  }
};
// return min element in right subtree of node_
inline node_ptr *binary_tree::next_(node_ptr *node_) {
  assert(node_->right);
  node_ptr *min = node_->right;
  while (min->right) {
    min = min->right;
  }
  return min;
};
// return max element in left subtree of node_
inline node_ptr *binary_tree::prev_(node_ptr *node_) {
  assert(node_->left);
  node_ptr *max = node_->left;
  while (max->left) {
    max = max->left;
  }
  return max;
};

inline node_ptr *binary_tree::delete_(node_ptr *np) {
  node_ptr *r_ = this->search_(np);
  if (not(r_ == np)) {
    return nullptr;
  }
  if ((r_->left) && (r_->right)) {
    node_ptr *prev_node = prev_(r_);
    this->copy_data(prev_node, r_);
    cut_out_(prev_node, prev_node->left);
    return prev_node;
  } else if (r_->left) {
    cut_out_(r_, r_->left);
    return r_;
  } else if (r_->right) {
    cut_out_(r_, r_->right);
    return r_;
  } else {
    cut_out_(r_, nullptr);
    return r_;
  }
};
/**
 * replace cut with child,
 * child can be left/right child of cut, or nullptr
 * if cut is root (cut->parent == nullptr), root=child;
 */
inline void binary_tree::cut_out_(node_ptr *cnode, node_ptr *child_of_) {
  assert(cnode);
  if (cnode->parent == nullptr) {
    root = child_of_;
  } else {
    if (child_of_) {
      child_of_->parent = cnode->parent;
    }
    if (cnode == cnode->parent->left) {
      cnode->parent->left = child_of_;
    } else {
      cnode->parent->right = child_of_;
    }
  }
}

/**
 * depend old_node is left or right of its parent
 * update parent of old_node's left or right child ptr
 * set new_node as old_node's parent
 */
inline void binary_tree::swap_parent_(node_ptr *old_node, node_ptr *new_node) {
  assert(old_node);
  assert(new_node);
  new_node->parent = old_node->parent;
  if (old_node == root) {
    root = new_node;
  } else if (old_node->parent->left == old_node) {
    old_node->parent->left = new_node;
  } else {
    old_node->parent->right = new_node;
  }
  old_node->parent = new_node;
}
/**
 *           x (lnode)                 y
 *          / \                       / \
 *         a   y (rc)  ====>         x   c
 *            / \                   / \
 *           b   c                 a   b
 */
inline void binary_tree::left_rotate(node_ptr *lnode) {
  node_ptr *rc = lnode->right;
  lnode->right = rc->left;
  if (rc->left != nullptr) {
    rc->left->parent = lnode;
  }
  swap_parent_(lnode, rc);
  rc->left = lnode;
}
/**
 *           y (node)                  x
 *          / \                       / \
 *    (lc) x   c       ====>         a   y
 *        / \                           / \
 *       a   b                         b   c
 */
inline void binary_tree::right_rotate(node_ptr *rnode) {
  node_ptr *lc = rnode->left;
  rnode->left = lc->right;
  if (lc->right != nullptr) {
    lc->right->parent = rnode;
  }
  swap_parent_(rnode, lc);
  lc->right = rnode;
}

template <typename KV = kv>
class binary_search_tree : protected binary_tree {
public:
  binary_search_tree() : root(nullptr){};
  void insert(KV kv) {insert_(kv);};
protected:
  void copy_data(binary_tree_node<KV> *from,
                 binary_tree_node<KV> *to) {
    to->kv_pair = from->kv_pair;
  }
  // return node whose key equals snode's, or the parent of failure node
  binary_tree_node<KV> *search_(node_ptr* snode) {
    auto *it = this->root;
    if (it == snode) {
      return it;
    } else {
      while (true) {
        if (snode < it) {
          if (it->left) {
            it = it->left;
          } else {
            break;
          }
        }
        if (snode > it) {
          if (it->right) {
            it = it->right;
          } else {
            break;
          }
        }
      }
      return it;
    }
  };
  
  void insert_(binary_tree_node<KV> *inode) {
    binary_tree_node<KV> *r_ = this->search_(inode);
    if (r_ == inode) {
      if (r_->kv_pair.second != inode->kv_pair.second) {
        r_->kv_pair.second = inode->kv_pair.second;
      }
    } else if (r_ < inode) {
      r_->right = inode;
      inode->parent = r_;
    } else {
      r_->left = inode;
      inode->parent = r_;
    }
  };
  void insert_(KV kv) {
    binary_tree_node<KV>* i = new binary_tree_node<KV>(kv);
    insert_(i);
  };
private:
  binary_tree_node<KV> *root;
};
} // namespace cop5536

#endif /* RB_TREE_H_ */
