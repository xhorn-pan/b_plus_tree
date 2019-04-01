#ifndef BS_TREE_H_
#define BS_TREE_H_

#include <iostream>

namespace cop5536 {
template <typename TKey = long long, typename TData = double> class bs_tree {
public:
  bs_tree() : root(nullptr){};
  ~bs_tree();
  virtual void insert(const TKey key, const TData data) { insert_(key, data); };
  virtual TData search(const TKey key) {
    node* node = search_(key);
    if (node->key == key) {
      return node->data;
    } else {
      return NULL;
    }
  };
  virtual void delete_key(const TKey key) {
    node* rnode = search_(key);
    if (rnode->key == key) {
      node* need_deleted = delete_phy(rnode);
      delete need_deleted;
    }
  };

protected:
  struct node {
    TKey key;
    TData data;
    node *left;
    node *right;
    node *parent;
    node();
    node(const TKey key, const TData data)
        : key(key), data(data), left(nullptr), right(nullptr),
          parent(nullptr){};
  };


  // search key from root, if find return the node,
  // if not, return the leaf node where search fail
  virtual node* search_(const TKey key) {
    node* current = root;
    while (true) {
      if (key == current->key) {
        return current;
      } else if (key < current->key) {
	if (current->left) {
	  current = current->left;
	} else {
	  return current;
	}
      } else {
        // current->right ? current=current->right:return current;
	if (current->right) {
	  current = current->right;
	} else {
	  return current;
	}
      }
    }
  }

  virtual node* insert__(node* inode) {
    node* snode = this->search_(inode->key);
    if (snode == nullptr){
      this->root = inode;
    } else if(snode->key == inode->key){
      if (snode->data != inode->data) {
	snode->data = inode->data;
      }
      return snode;
    } else {
      inode->parent = snode;
      if (inode->key < snode->key) {
	snode->left = inode;  
      } else {
	snode->right = inode;
      }
      return inode;
    }
  }

  virtual node* insert__(const TKey key, const TData data) {
    node* n = search_(key);
    if (n == nullptr) {
      root = new node(key, data);
      return root;
    } else if (n->key == key) {
      if (n->data != data) {
        n->data = data;
      }
      return n;
    } else if (key < n->key) {
      node* nn = new node(key, data);
      n->left = nn;
      nn->parent = n;
      return nn;
    } else {
      node* nn = new node(key, data);
      n->right = nn;
      nn->parent = n;
      return nn;
    }
  }
  
  virtual inline void cut_out(node *cut, node *child) {
    // replace cut with child,
    // child can be left/right child of cut, or nullptr
    // if cut is root (cut->parent == nullptr), root=child;
    assert(cut); // cut can not be null
    if (cut->parent == nullptr) {
      root = child;
    } else {
      if (child) {
        child->parent = cut->parent;
      }
      if (cut == cut->parent->left) {
        cut->parent->left = child;
      } else {
        cut->parent->right = child;
      }
    }
  }

  virtual inline node *find_largest_in_left_(node *lnode) {
    assert(lnode->left); // left must not null
    node* parent_ = lnode->left;
    while (true) {
      if (parent_->right) {
        parent_ = parent_->right;
      } else {
        break;
      }
    }
    return parent_;
  }

  virtual inline node* delete_phy(node *dnode) {
    if ((dnode->left) && (dnode->right)) {
      // degree 2
      node* nd = find_largest_in_left_(dnode);
      dnode->key = nd->key;
      dnode->data = nd->data;
      cut_out(nd, nd->left);
      return nd;
    } else if (dnode->left) {
      // degree 1, left child is not null
      cut_out(dnode, dnode->left);
      return dnode;
    } else if (dnode->right) {
      // degree 1, right child is not null
      cut_out(dnode, dnode->right);
      return dnode;
    } else {
      // leaf node
      cut_out(dnode, nullptr);
      return dnode;
    }
  }

  void left_rotate(node *lnode) {
    /**
     *           x (node)                  y
     *          / \                       / \
     *         a   y (rc)  ====>         x   c
     *            / \                   / \
     *           b   c                 a   b
     */
    node* rc = lnode->right;
    lnode->right = rc->left;
    if (rc->left != nullptr) {
      rc->left->parent = lnode;
    }
    rc->parent = lnode->parent;
    if (lnode == root) {
      root = rc;
    } else {
      fix_parent_pointer(lnode, rc);
    }
    lnode->parent = rc;
    rc->left = lnode;
  };
  void right_rotate(node *rnode) {
    /**
     *           y (node)                  x
     *          / \                       / \
     *    (lc) x   c       ====>         a   y
     *        / \                           / \
     *       a   b                         b   c
     */
    node* lc = rnode->left;
    rnode->left = lc->right;
    if (lc->right != nullptr) {
      lc->right->parent = rnode;
    }
    lc->parent = rnode->parent;
    if (rnode == root) {
      root = lc;
    } else {
      fix_parent_pointer(rnode, lc);
    }
    rnode->parent = lc;
    lc->right = rnode;
  };

private:
  node* root;
};
} // namespace cop5536
#endif /* BS_TREE_H_ */
