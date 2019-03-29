#ifndef RED_BLACK_TREE_H_
#define RED_BLACK_TREE_H_

#include <iostream>

namespace cop5536 {
enum class node_color { red = 0, black };
enum class side { left = 0, right = 1 };

template <typename TData> class red_black_tree {
private:
  struct rb_tree_node {
    rb_tree_node();
    rb_tree_node(const int key, TData data) : key(key), data(data) {
      this->left_size = 0;
      //             this->color = node_color::black;
      this->rank = 1;
      this->left = nullptr;
      this->right = nullptr;
      this->parent = nullptr;
    };
    ~rb_tree_node();

    node_color color;
    short int left_size;
    short int rank;
    rb_tree_node *left;
    rb_tree_node *right;
    rb_tree_node *parent;
    const long long key;
    TData data;
  };

  rb_tree_node *root;

public:
  red_black_tree();
  ~red_black_tree();
  // const rb_tree_node<TData>& operator[](int i);
  void insert_key_value(const int key, TData data);
  // void insert_key_value(const int key, TData&& data);
  void delete_key(const int key);
  TData search(const int key);
  void fixup_tree(rb_tree_node *node);
  void fix_parent_pointer(rb_tree_node *old_node, rb_tree_node *new_node);
  void left_rotate(rb_tree_node *node);
  void right_rotate(rb_tree_node *node);
  void print_in_order();
  void print_in_order(rb_tree_node *node);
};

template <typename TData>
inline red_black_tree<TData>::red_black_tree() : root(nullptr) {}

template <typename TData> inline red_black_tree<TData>::~red_black_tree() {
  // TODO
}

template <typename TData>
inline TData red_black_tree<TData>::search(const int key) {
  rb_tree_node *tmp = root;
  while (tmp) {
    if (tmp->key == key) {
      return tmp->data;
    } else if (tmp->key > key) {
      tmp = tmp->left;
    } else {
      tmp = tmp->right;
    }
  }
  return NULL;
}

template <typename TData> void red_black_tree<TData>::print_in_order() {
  std::cout << "print" << std::endl;
  print_in_order(root);
}

template <typename TData>
void red_black_tree<TData>::print_in_order(rb_tree_node *node) {
  if (node->left) {
    print_in_order(node->left);
  }
  std::cout << " key: " << node->key << " left size:" << node->left_size
            << " rank:" << node->rank << " data:" << node->data;
  if (node->color == node_color::black) {
    std::cout << "color: black";
  } else {
    std::cout << "color: red";
  }
  std::cout << std::endl;
  if (node->right) {
    print_in_order(node->right);
  }
}

template <typename TData>
inline void red_black_tree<TData>::left_rotate(rb_tree_node *node) {
  /**
   *           x (node)                  y
   *          / \                       / \
   *         a   y (rc)  ====>         x   c
   *            / \                   / \
   *           b   c                 a   b
   *  left_size of x should be unchanged
   *  left_size of y = left_size of x_before + left_size of y_before + 1;
   */
  auto rc = node->right;
  node->right = rc->left;
  if (rc->left != nullptr) {
    rc->left->parent = node;
  }
  rc->parent = node->parent;
  if (node == root) {
    root = rc;
  } else {
    fix_parent_pointer(node, rc);
  }
  node->parent = rc;
  rc->left = node;
  // update rc's left_size
  rc->left_size += node->left_size + 1;
}

template <typename TData>
inline void red_black_tree<TData>::right_rotate(rb_tree_node *node) {
  /**
   *           y (node)                  x
   *          / \                       / \
   *    (lc) x   c       ====>         a   y
   *        / \                           / \
   *       a   b                         b   c
   *  left_size of x should be unchanged
   *  left_size of y_before = left_size of x_before + 1 + #nodes in b
   *  left_size of y_after = #nodes in b = left_size of y_before - (left_size of
   * x_before + 1);
   */
  auto lc = node->left;
  node->left = lc->right;
  if (lc->right != nullptr) {
    lc->right->parent = node;
  }
  lc->parent = node->parent;
  if (node == root) {
    root = lc;
  } else {
    fix_parent_pointer(node, lc);
  }
  node->parent = lc;
  lc->right = node;
  // update node's left_size
  node->left_size -= (lc->left_size + 1);
}

template <typename TData>
inline void red_black_tree<TData>::fix_parent_pointer(rb_tree_node *old_node,
                                                      rb_tree_node *new_node) {
  if (old_node->parent->left == old_node) {
    old_node->parent->left = new_node;
  } else {
    old_node->parent->right = new_node;
  }
}

template <typename TData>
void red_black_tree<TData>::fixup_tree(rb_tree_node *node) {
  auto gp = node->parent;
  rb_tree_node *sibling = nullptr;
  int rotateType = 0; // LL:0, LR:1, RL:2, RR:3
  if (node->key < gp->key) {
    // node is left child of gp
    sibling = gp->right;
    rotateType &= ~2;
  } else {
    sibling = gp->left;
    rotateType |= 2;
  }
  rb_tree_node *p = nullptr;
  if ((node->left != nullptr) && (node->left->color == node_color::red)) {
    p = node->left;
    rotateType &= ~1;
  } else if ((node->right != nullptr) &&
             (node->right->color == node_color::red)) {
    p = node->right;
    rotateType |= 1;
  }
  if ((sibling == nullptr) || (sibling->color == node_color::black)) {
    // if sibling is null, it is an external node, take it as black,
    // or sibling is not null and color is black
    switch (rotateType) {
    case 0:
      /** LLb           |                          |
       *           (gp) z                          y
       *              // \                       // \\
       *      (node)  y   d    =====>        (p) x    z
       *            // \                        / \  / \
       *        (p) x   c                      a   b c  d
       *           / \
       *          a   b
       */
      right_rotate(gp);
      node->color = node_color::black;
      gp->color = node_color::red;
      break;
    case 3:
      /** RRb           |                          |
       *           (gp) z                          y
       *               / \\                      // \\
       *              a   y (node)     =====>    z    x
       *                 / \\                   / \  / \
       *                b   x (p)              a   b c  d
       *                   / \
       *                  c   d
       */
      left_rotate(gp);
      node->color = node_color::black;
      gp->color = node_color::red;
      break;
    case 1:
      /** LLb           |                 |             |
       *           (gp) z                 z             x
       *              // \              // \          // \\
       *      (node)  y   d    =====>   x   d  ====>  y    z
       *             / \\             // \           / \  / \
       *            a   x (p)         y   c         a   b c  d
       *               / \           / \
       *              b   c         a   b
       */
      left_rotate(node);
      right_rotate(gp);
      gp->color = node_color::red;
      p->color = node_color::black;
      break;
    case 2:
      /** RLb           |                  |                  |
       *           (gp) z                  z                  x
       *               / \\               / \\              // \\
       *              a   y (node) ====> a   x       ====>  z   y
       *                 // \               / \\           / \  / \
       *             (p) x   d             b    y         a   b c  d
       *                / \                    / \
       *               b   c                  c   d
       */
      right_rotate(node);
      left_rotate(gp);
      gp->color = node_color::red;
      p->color = node_color::black;
      break;
    default:
      break;
    }
  } else {
    // sibling is nullptr or sibling's color is red;
    if (sibling != nullptr) { // that leaves sibling's color must be red;
      sibling->color = node_color::black;
      node->color = node_color::black;
      // update rank, this is the only situation where rank of nodes incresed.
      gp->rank += 1;
    }
    // if gp is root, fixup termnate
    // if gp is not root, mark it as red, check gp's parent(ggp),
    // if ggp is not null and its color is red, continue fixup
    if (gp->parent != nullptr) {
      gp->color = node_color::red;
      if ((gp->parent->parent != nullptr) &&
          (gp->parent->color == node_color::red)) {
        fixup_tree(gp->parent);
      }
    }
  }
}

template <typename TData>
inline void red_black_tree<TData>::insert_key_value(const int key, TData data) {
  if (root == nullptr) {
    root = new rb_tree_node(key, data);
    root->color = node_color::black;
    // this->root = root;
    return;
  }

  rb_tree_node *current = root;
  rb_tree_node *parent = nullptr;
  while (current != nullptr) {
    if (key == current->key) {
      // update key
      current->data = data;
      return;
    }
    parent = current;
    if (key < current->key) {
      // std::cout << "left "<< key;
      current = current->left;
    } else {
      // std::cout << "right "<< key;
      current = current->right;
    }
  }

  //<< " data:" << *parent->data;
  // create new node, link it to parent
  auto new_node = new rb_tree_node(key, data);
  new_node->parent = parent;
  new_node->color = node_color::red;
  if (key > parent->key) {
    parent->right = new_node;
  } else {
    parent->left = new_node;
  }
  // DONE update left_size tracking back to root;
  rb_tree_node *it = new_node;
  while (it != root) {
    if (it->parent->left == it) {
      it->parent->left_size += 1;
    }
    it = it->parent;
  }
  if (parent->color == node_color::red) {
    fixup_tree(parent);
  }
}
} // namespace cop5536

#endif /* RED_BLACK_TREE_H_ */
