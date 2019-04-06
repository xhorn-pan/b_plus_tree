#ifndef RB_TREE_1_H_
#define RB_TREE_1_H_

#include <cassert>
#include <iostream>

namespace cop5536 {

// typedef std::pair<long long, double> kv;
enum class node_color { red = 0, black };

template <class node_type> struct rb_base_ {
  node_type *left{nullptr};
  node_type *right{nullptr};
  node_type *parent{nullptr};
};

template <class data_type>
struct rb_tree_node : rb_base_<rb_tree_node<data_type>> {
  data_type data;
  node_color color;
  short int left_size;
  short int rank;

  rb_tree_node<data_type>(data_type real_data)
      : rb_base_<rb_tree_node<data_type>>(), data(real_data),
        color(node_color::red), left_size(0), rank(1){};
  // ~rb_tree_node(); // { std::cout << "destr in rb_tree_node" << std::endl; };
  friend bool operator==(const rb_tree_node<data_type> &bt1,
                         const rb_tree_node<data_type> &bt2) {
    return bt1.data == bt2.data;
  };
  friend bool operator<(const rb_tree_node<data_type> &bt1,
                        const rb_tree_node<data_type> &bt2) {
    return bt1.data < bt2.data;
  };
  friend bool operator>(const rb_tree_node<data_type> &bt1,
                        const rb_tree_node<data_type> &bt2) {
    return bt1.data > bt2.data;
  };
  inline void switch_color() {
    this->color = (this->color == node_color::black) ? node_color::red
                                                     : node_color::black;
  };
  inline bool is_red() { return color == node_color::red; }
  inline bool is_left_child_of_parent() {
    assert(this->parent);
    return this == this->parent->left;
  }
};

template <class data_type, template <class> class node_type = rb_tree_node>
class rb_tree {
public:
  rb_tree() : root(nullptr){};
  explicit rb_tree(node_type<data_type> *o) : root(o){};
  virtual ~rb_tree() {
    if (root)
      destory_(root);
  };
  void insert(data_type data) { insert_(data); };
  void print_tree() {
    if (root) {
      print_node(root, 0);
      std::cout << "=======================" << std::endl;
    } else {
      std::cout << "empty tree" << std::endl;
    }
  };
  void delete_key(data_type data) {
    if (root == nullptr) {
      return;
    }
    node_type<data_type> *i = new node_type<data_type>(data);
    delete_(i);
    delete i;
  };

  // protected:
  // for debug
  void print_node(node_type<data_type> *node_, int space) {
    if (not node_)
      return;
    space += 8;
    print_node(node_->right, space);
    for (int i = 8; i < space; i++)
      std::cout << " ";

    std::cout << node_->data << " ";
    (node_->color == node_color::red) ? std::cout << "red "
                                      : std::cout << "black ";
    std::cout << node_->rank << " " << node_->left_size << "\n";
    print_node(node_->left, space);
  }
  void insert_(data_type data);
  node_type<data_type> *insert_(node_type<data_type> *inode);
  node_type<data_type> *sibling_of_(node_type<data_type> *node_);
  node_type<data_type> *next_(node_type<data_type> *node_);
  node_type<data_type> *prev_(node_type<data_type> *node_);
  void cut_out_(node_type<data_type> *cnode, node_type<data_type> *child_of_);
  void swap_parent_(node_type<data_type> *old_node,
                    node_type<data_type> *new_node);
  void left_rotate_(node_type<data_type> *lnode);
  void right_rotate_(node_type<data_type> *rnode);
  virtual void copy_data_(node_type<data_type> *from, node_type<data_type> *to);
  virtual node_type<data_type> *search_(node_type<data_type> *np);
  virtual void delete_(node_type<data_type> *np);
  void destory_(node_type<data_type> *node_);
  void update_left_size_(node_type<data_type> *node_, int sz = 1);
  void fix_insert_(node_type<data_type> *node_);
  bool is_red_(node_type<data_type> *node_) {
    return node_ and node_->is_red();
  };
  void fix_delete_(node_type<data_type> *dnode, node_type<data_type> *pod,
                   node_type<data_type> *cod);
  void split_();
  void join_();

private:
  node_type<data_type> *root;
};

template <class data_type, template <class> class node_type>
inline void
rb_tree<data_type, node_type>::destory_(node_type<data_type> *node_) {
  if (node_->left) {
    destory_(node_->left);
  }
  if (node_->right) {
    destory_(node_->right);
  }
  delete node_;
}

template <class data_type, template <class> class node_type>
inline void
rb_tree<data_type, node_type>::update_left_size_(node_type<data_type> *node_,
                                                 int sz) {
  // node_type<data_type> *it = node_;
  while (node_ != root) {
    if (node_ == node_->parent->left) {
      node_->parent->left_size += sz;
    }
    node_ = node_->parent;
  }
}

template <class data_type, template <class> class node_type>
inline node_type<data_type> *
rb_tree<data_type, node_type>::sibling_of_(node_type<data_type> *node_) {
  if (not node_->parent) {
    return nullptr;
  } else if (node_->is_left_child_of_parent()) {
    return node_->parent->right;
  } else {
    return node_->parent->left;
  }
};

// return min element in right subtree of node_
template <class data_type, template <class> class node_type>
inline node_type<data_type> *
rb_tree<data_type, node_type>::next_(node_type<data_type> *node_) {
  assert(node_->right);
  node_type<data_type> *min = node_->right;
  while (min->left) {
    min = min->left;
  }
  return min;
};
// return max element in left subtree of node_
template <class data_type, template <class> class node_type>
inline node_type<data_type> *
rb_tree<data_type, node_type>::prev_(node_type<data_type> *node_) {
  assert(node_->left);
  node_type<data_type> *max = node_->left;
  while (max->right) {
    max = max->right;
  }
  return max;
};

template <class data_type, template <class> class node_type>
inline void rb_tree<data_type, node_type>::delete_(node_type<data_type> *np) {
  node_type<data_type> *r_ = search_(np);
  if (not(r_->data == np->data)) {
    return; // not found;
  }
  if ((r_->left) && (r_->right)) {
    node_type<data_type> *prev_node = prev_(r_);
    this->copy_data_(prev_node, r_);
    r_ = prev_node;
    fix_delete_(r_, r_->parent, r_->left);
  } else if (r_->left) {
    fix_delete_(r_, r_->parent, r_->left);
  } else if (r_->right) {
    fix_delete_(r_, r_->parent, r_->right);
  } else {
    fix_delete_(r_, r_->parent, nullptr);
  }
  delete r_;
};
// if pod is not null, it can have at most 1 child;
template <class data_type, template <class> class node_type>
inline void
rb_tree<data_type, node_type>::fix_delete_(node_type<data_type> *dnode,
                                           node_type<data_type> *pod,
                                           node_type<data_type> *cod) {
  if (root == cod) {
    // cod->color = node_color::black;
    return;
  }
  if (root == dnode) {
    if (cod) {
      root = cod;
      cod->color = node_color::black;
    } else {
      root = nullptr;
    }
    return;
  }
  node_type<data_type> *sibling = nullptr;
  if (dnode) {
    sibling = sibling_of_(dnode);
    update_left_size_(dnode, -1);
    swap_parent_(dnode, cod);
    if (is_red_(dnode)) {
      return;
    } else {
      if (is_red_(cod)) {
        cod->switch_color();
        return;
      }
    }
  } else {
    pod = cod->parent;
    sibling = sibling_of_(cod);
  }

  assert(sibling);                            // sibling must not nullptr
  if (is_red_(sibling)) {                     // Xrx 0 1 2
    if (sibling->is_left_child_of_parent()) { // Rrx
      // its rank = cod's rank + 1, so it can not be null
      node_type<data_type> *s_r = sibling->right;
      if (is_red_(s_r->right)) {
        // Rr1 case 2 or Rr2      |                            |
        //                        py[r+2]                      x [r+2]
        //                      // \                        //   \
        //                      v    y[r]       =====>      v     py[r+1]
        //                     / \                         / \    / \
        //                    a   w[r+1]            [r+1] a   w  d   y[r]
        //                       X \\                        X \ 
        //                      b   x [r+1]                 b   c
        //                         / \
        //                        c   d[r]
        // X above b means we do not care its color,
        // if b is black its Rr1 case2, if b is red, it is Rr2
        // rotate: left (w aka s_r) left (v aka sibling), right(py)
        // color change: x (s_r->right),
        // rank change: x[+1], py[-1]
        s_r->right->switch_color();
        s_r->right->rank += 1;
        pod->rank -= 1;
        left_rotate_(s_r);
        left_rotate_(sibling);
        right_rotate_(pod); // s_r is at v's place
      } else if (is_red_(s_r->left)) {
        // Rr1 case 1             |                      |
        //                        py[r+2]                w[r+2]
        //                      //  \                  //  \
        //                      v    y[r]  =====>      v    py[r+1]
        //                     / \                    / \   / \
        //                    a   w[r+1]             a   b c   y[r]
        //                      // \
        //                [r+1] b   c
        // rotate: left (v), right(py)
        // color change: b (s_r->left)
        // rank change: w[+1], py[-1]
        s_r->left->switch_color();
        s_r->rank += 1;
        pod->rank -= 1;
        left_rotate_(sibling);
        right_rotate_(pod);
      } else {
        // Rr0            |                       |
        //               py[r+2]                 v[r+2]
        //             //  \                    / \
        //   (sibling) v    y (cod)[r] =====>  a   py[r+1]
        //            / \                        // \
        //           a   b(s_r)                  b   y[r]
        // right rotate (py)
        // color change v, b
        // rank change py[-1]
        sibling->switch_color();
        s_r->switch_color();
        pod->rank -= 1;
        right_rotate_(pod);
      }
    } else { // Lrx
      node_type<data_type> *s_l = sibling->left;
      if (is_red_(s_l->left)) {
        // Lr1 case 2 or Lr2      |                           |
        //                        py[r+2]                     x [r+2]
        //                       /  \\                       /  \\
        //                 (cod)y    v(sibling) ===> [r+1] py    v
        //                          / \                    / \  / \
        //                    (s_l)w   d[r+1]          [r]y  a  w  d
        //                       // X                          / X
        //                 [r+1] x   c[r+1]                   b   c
        //                      / \
        //                 [r] a   b
        // X above c means we do not care its color,
        // if c is black its Lr1 case2, if c is red, it is Lr2
        // rotate: right (w aka s_l) right (v aka sibling), left(py)
        // color change: x (s_l->left),
        // rank change: x[+1], py[-1]
        s_l->left->switch_color();
        s_l->left->rank += 1;
        pod->rank -= 1;
        right_rotate_(s_l);
        right_rotate_(sibling);
        left_rotate_(pod);
      } else if (is_red_(s_l->right)) {
        // Lr1 case 1      |                          |
        //               py[r+2]                     w[r+2]
        //              /  \\                       /  \\
        //       (cod) y    v(sibling) ===> [r+1] py    v
        //                 / \                    / \  / \
        //           (s_l)w   c                  y  a  b  c
        //               / \\
        //              a   b[r+1]
        // rotate: right (v), left(py)
        // color change: b (s_l->right)
        // rank change: w[+1], py[-1]
        s_l->right->switch_color();
        s_l->rank += 1;
        pod->rank -= 1;
        right_rotate_(sibling);
        left_rotate_(pod);
      } else {
        // Lr0            |                          |
        //               py[r+2]                    v[r+2]
        //              /  \\                      / \
        //       (cod) y    v(sibling) ===> [r+1] py  b
        //                 / \                    / \\
        //           (s_l)a   b                  y   a
        // left rotate (py)
        // color change v, a
        // rank change py[-1]
        sibling->switch_color();
        s_l->switch_color();
        pod->rank -= 1;
        left_rotate_(pod);
      }
    }
  } else { // Xbx 0 1 2
    if ((not is_red_(sibling->left)) and (not is_red_(sibling->right))) {
      // since Xb0 have no rotate need, Lb0 is same as Rb0
      if (pod->is_red()) {
        // Rb0 case 2    ||                      |
        //               py[r+2]                 py[r+1]
        //              /  \                   // \
        //   (sibling) v    y (cod)[r] =====>  v   y[r]
        //            / \                     / \
        //           a   b[r]                a   b[r]
        // color change py, v
        // rank change py[-1]
        sibling->switch_color();
        pod->switch_color();
        pod->rank -= 1;
      } else {
        // Rb0 case 2    |                       |
        //               py[r+2]                 py[r+1]
        //              /  \                   // \
        //   (sibling) v    y (cod)[r] =====>  v   y[r]
        //            / \                     / \
        //           a   b[r]                a   b[r]
        // color change v
        // rank change py[-1]
        // continue fix
        sibling->switch_color();
        pod->rank -= 1;
        // NOTICE, pass nullptr to note this call is a continue fix
        fix_delete_(nullptr, nullptr, pod);
      }
    } else {
      if (sibling->is_left_child_of_parent()) { // Rb1 or Rb2
        if (is_red_(sibling->right)) {
          // Rb1 case 2 or Rb2      X                         X
          //                       py[r+2]                   w[r+2]
          //                      / \                       /  \
          //               [r+1] v   y[r]                  v    py[r+1]
          //                    X \\                      X \   / \
          //                   a   w[r+1]                a   b c   y[r]
          //                      / \
          //                     b   c[r]
          // rotate left(v) right(py)
          // color change w_after = py_before, py_after = black
          // rank change w[+1], py[-1]
          // left_size
          sibling->right->color = pod->color;
          pod->color = node_color::black;
          pod->rank -= 1;
          sibling->right->rank += 1;
          left_rotate_(sibling);
          right_rotate_(pod);
        } else {
          // Rb1 case 1             X                         X
          //                       py[r+2]                   v[r+2]
          //                      / \                       / \
          //               [r+1] v   y[r]                  a   py[r+1]
          //                   // \                           / \
          //             [r+1] a   b[r]                      b   y[r]
          // rotate right(py)
          // color change v_after = py_before, py_after = black, a
          // rank change v[+1], py[-1]
          sibling->left->switch_color();
          sibling->color = pod->color;
          pod->color = node_color::black;
          sibling->rank += 1;
          pod->rank -= 1;
          right_rotate_(pod);
        }
      } else { // Lb1 or Lb2
        if (is_red_(sibling->left)) {
          // Lb1 case 2 or Lb2      X                         X
          //                       py[r+2]                   w [r+2]
          //                      / \                       /  \
          //                 [r] y   v[r+1]                py   v[r+1]
          //                       // X                   / \   / X
          //                  [r+1]w   c                 y   a b   c
          //                      / \
          //                     a   b[r]
          // rotate right(v) left(py)
          // color change w_after = py_before, py_after = black
          // rank change w[+1], py[-1]
          sibling->left->color = pod->color;
          pod->color = node_color::black;
          pod->rank -= 1;
          sibling->left->rank += 1;
          right_rotate_(sibling);
          left_rotate_(pod);
        } else {
          // Lb1 case 1             X                         X
          //                       py[r+2]                   v[r+2]
          //                      / \                       / \
          //                 [r] y   v[r+1]           [r+1]py  b[r+1]
          //                        / \\                  / \
          //                   [r] a   b[r+1]            y   a[r]
          // rotate left(py)
          // color change v_after = py_before, py_after = black, a
          // rank change v[+1], py[-1]
          sibling->right->switch_color();
          sibling->color = pod->color;
          pod->color = node_color::black;
          sibling->rank += 1;
          pod->rank -= 1;
          left_rotate_(pod);
        }
      }
    }
  }
}

/**
 * depend old_node is left or right of its parent
 * update parent of old_node's left or right child ptr
 * set new_node as old_node's parent
 */
template <class data_type, template <class> class node_type>
inline void
rb_tree<data_type, node_type>::swap_parent_(node_type<data_type> *old_node,
                                            node_type<data_type> *new_node) {
  assert(old_node);
  // assert(new_node);
  if (new_node) {
    new_node->parent = old_node->parent;
  }
  if (old_node == root) {
    root = new_node;
  } else if (old_node->parent->left == old_node) {
    old_node->parent->left = new_node;
  } else {
    old_node->parent->right = new_node;
  }
  if (new_node) {
    old_node->parent = new_node;
  }
}

/**
 *           x (lnode)                 y
 *          / \                       / \
 *         a   y (rc)  ====>         x   c
 *            / \                   / \
 *           b   c                 a   b
 * y_after = y_before + x_before + 1
 */
template <class data_type, template <class> class node_type>
inline void
rb_tree<data_type, node_type>::left_rotate_(node_type<data_type> *lnode) {
  node_type<data_type> *rc = lnode->right;
  rc->left_size += (lnode->left_size + 1);
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
 * y_before = x_before + y_after + 1
 */
template <class data_type, template <class> class node_type>
inline void
rb_tree<data_type, node_type>::right_rotate_(node_type<data_type> *rnode) {
  node_type<data_type> *lc = rnode->left;
  rnode->left_size -= (lc->left_size + 1);
  rnode->left = lc->right;
  if (lc->right != nullptr) {
    lc->right->parent = rnode;
  }
  swap_parent_(rnode, lc);
  lc->right = rnode;
}

template <class data_type, template <class> class node_type>
inline node_type<data_type> *
rb_tree<data_type, node_type>::search_(node_type<data_type> *snode) {
  node_type<data_type> *it = root;

  while (true) {
    if (it->data == snode->data) {
      break;
    } else if (snode->data < it->data) {
      if (it->left) {
        it = it->left;
      } else {
        break;
      }
    } else {
      // if (snode->data > it->data) {
      if (it->right) {
        it = it->right;
      } else {
        break;
      }
    }
  }
  return it;
}

template <class data_type, template <class> class node_type>
inline node_type<data_type> *
rb_tree<data_type, node_type>::insert_(node_type<data_type> *inode) {
  if (root == nullptr) {
    root = inode;
    return root;
  }
  node_type<data_type> *r_ = search_(inode);
  //  if (r_ == inode) {
  //  if (r_->data != inode->data) {
  //    r_->data = inode->data;
  //  }
  //} else
  if (r_->data < inode->data) {
    r_->right = inode;
    inode->parent = r_;
  } else if (r_->data > inode->data) {
    r_->left = inode;
    inode->parent = r_;
  } else {
    return r_;
  }
  return inode;
};

template <class data_type, template <class> class node_type>
inline void
rb_tree<data_type, node_type>::fix_insert_(node_type<data_type> *node_) {
  if (node_ == root) {
    node_->color = node_color::black;
    return;
  }
  // if node_->parent is red, it must have parent too.
  if (node_->is_red() && node_->parent->is_red()) {
    node_type<data_type> *uncle = sibling_of_(node_->parent);
    if (uncle && uncle->is_red()) {
      /** type XYr   |                            ||
       *            gp[x]                         gp[x+1]
       *           // \\                         /  \
       *  (parent)pp   d(uncle)                 pp   d[x]
       *         //     [x]                    //
       *  (node) p[x]                          p[x]
       * no rotate, no left_size change
       * rank change: gp(+1)
       * color change: pp(r=>b), d(r=>b), gp(b=>r)
       * continue fix on gp(uncle->parent)
       */
      uncle->switch_color();
      node_->parent->switch_color();
      uncle->parent->switch_color();
      uncle->parent->rank += 1;
      fix_insert_(uncle->parent);
    } else {
      node_type<data_type> *gp = node_->parent->parent;
      if (node_ == node_->parent->left) {
        if (node_->parent == gp->left) { // LLb
          /**               |                          |
           *           (gp) z                          y
           *              // \                       // \\
           *              y   d    =====>            x    z
           *            // \                        / \  / \
           *    (node_) x   c                      a   b c  d
           *           / \
           *          a   b
           * right rotate (gp), rank unchange
           * color change, y, gp
           * left_size change: z
           *     z_before = z_after + y_before + 1
           */
          node_->parent->switch_color();
          gp->switch_color();
          // gp->left_size -= (node_->parent->left_size + 1);
          right_rotate_(gp);
        } else { // RLb
          /**               |                  |                  |
           *           (gp) z                  z                  x
           *               / \\               / \\              // \\
           *              a   y        ====> a   x       ====>  z   y
           *                 // \               / \\           / \  / \
           *         (node_) x   d             b    y         a   b c  d
           *                / \                    / \
           *               b   c                  c   d
           * right rotate(y), left rotate(z) rank unchange
           * color change z, x
           * left size change: x, y
           * x_after = x_before + z_before + 1
           * y_before = x_before + y_after + 1
           */
          gp->switch_color();
          node_->switch_color();
          // node_->parent->left_size -= (node_->left_size + 1);
          // node_->left_size += (gp->left_size + 1);
          right_rotate_(node_->parent);
          left_rotate_(gp);
        }      // XLb end
      } else { // type XRb
        if (node_->parent == gp->left) {
          /** LRb           |                 |             |
           *           (gp) z                 z             x
           *              // \              // \          // \\
           *              y   d    =====>   x   d  ====>  y    z
           *             / \\             // \           / \  / \
           *            a   x (node_)      y   c         a   b c  d
           *               / \           / \
           *              b   c         a   b
           * left rotate(y), right rotate(z) rank unchange
           * color change z, x
           * left size change: x, z
           * x_after = x_before + y_before + 1
           * z_before = x_before + z_after + y_before + 2
           */
          node_->switch_color();
          gp->switch_color();
          // gp->left_size -= (node_->left_size + node_->parent->left_size + 2);
          // node_->left_size += (node_->parent->left_size + 1);
          left_rotate_(node_->parent);
          right_rotate_(gp);
        } else {
          /** RRb           |                          |
           *           (gp) z                          y
           *               / \\                      // \\
           *              a   y            =====>    z    x
           *                 / \\                   / \  / \
           *                b   x (node_)           a   b c  d
           *                   / \
           *                  c   d
           * left rotate (gp) rank unchange
           * color change, y, gp
           * left_size change: y
           *     y_after = y_before + z_before + 1
           */
          node_->parent->switch_color();
          gp->switch_color();
          // node_->parent->left_size += (gp->left_size + 1);
          left_rotate_(gp);
        }
      } // end XLb
    }
  }
};
template <class data_type, template <class> class node_type>
inline void rb_tree<data_type, node_type>::insert_(data_type data) {
  node_type<data_type> *i = new node_type<data_type>(data);
  node_type<data_type> *ri = insert_(i);
  update_left_size_(ri);
  fix_insert_(ri);
};

template <class data_type, template <class> class node_type>
inline void
rb_tree<data_type, node_type>::copy_data_(node_type<data_type> *from,
                                          node_type<data_type> *to) {
  to->data = from->data;
}

} // namespace cop5536

#endif /* RB_TREE_1_H_ */
