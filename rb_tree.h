#ifndef RB_TREE_H_
#define RB_TREE_H_

#include <cassert>
#include <iostream>
#include <type_traits>
#include <vector>

namespace cop5536 {
// typedef std::pair<long long, double> kv;
enum class node_color { red = 0, black };

template <class node_type> struct rb_base_ {
  node_type *left{nullptr};
  node_type *right{nullptr};
  node_type *parent{nullptr};
  // rb_base_(const rb_base_& o) : left(o.left), right(o.right),
  // parent(o.parent){};
};

template <class key_type, class data_type>
struct rb_tree_node : rb_base_<rb_tree_node<key_type, data_type>> {
  key_type key;
  data_type data;
  node_color color;
  short int left_size;
  short int rank;

  // rb_tree_node();
  rb_tree_node<key_type, data_type>(key_type key, data_type real_data)
      : rb_base_<rb_tree_node<key_type, data_type>>(), key(key),
        data(real_data), color(node_color::red), left_size(0), rank(1){};
  // rb_tree_node<key_type, data_type>(const rb_tree_node& rb)
  //    : rb_base_<rb_tree_node<key_type, data_type>>(), key(rb.key),
  //      data(rb.data), color(node_color::red), left_size(0), rank(1){};
  // ~rb_tree_node(); // { std::cout << "destr in rb_tree_node" << std::endl; };
  ~rb_tree_node(){};

  friend bool operator==(const rb_tree_node<key_type, data_type> &bt1,
                         const rb_tree_node<key_type, data_type> &bt2) {
    return bt1.key == bt2.key;
  };
  friend bool operator<(const rb_tree_node<key_type, data_type> &bt1,
                        const rb_tree_node<key_type, data_type> &bt2) {
    return bt1.key < bt2.key;
  };
  friend bool operator>(const rb_tree_node<key_type, data_type> &bt1,
                        const rb_tree_node<key_type, data_type> &bt2) {
    return bt1.key > bt2.key;
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

template <class key_type, class data_type,
          template <class, class> class node_type = rb_tree_node>
class rb_tree {
public:
  rb_tree() : root(nullptr){};
  explicit rb_tree(node_type<key_type, data_type> *o);
  rb_tree(const rb_tree &o) { this->root = o.root; };
  virtual ~rb_tree() {
    if (root)
      destory_(root);
  };
  bool insert(key_type key, data_type data) { return insert_(key, data); };
  void print_tree() {
    // for debug
    if (root) {
      print_node(root, 0);
      std::cout << "=======================" << std::endl;
    } else {
      std::cout << "empty tree" << std::endl;
    }
  };
  void print_node(node_type<key_type, data_type> *node_, int space) {
    if (not node_)
      return;
    space += 8;
    print_node(node_->right, space);
    for (int i = 8; i < space; i++)
      std::cout << " ";

    std::cout << node_->key << " " << node_->data;
    (node_->color == node_color::red) ? std::cout << "red "
                                      : std::cout << "black ";
    std::cout << node_->rank << " " << node_->left_size << "\n";
    print_node(node_->left, space);
  }
  void get_keys(std::vector<key_type> &vec) { get_keys__(root, vec); };
  void get_keys__(node_type<key_type, data_type> *node_,
                  std::vector<key_type> &vec) {
    if (not node_)
      return;
    get_keys__(node_->left, vec);
    vec.push_back(node_->key);
    get_keys__(node_->right, vec);
  }
  bool delete_key(key_type key) { return delete_(key); };
  node_type<key_type, data_type> *nth(short int n); // { return nth_(n); };
  // after join, b_ is destroyed
  void join_(node_type<key_type, data_type> *m_,
             rb_tree<key_type, data_type> *b_);
  short int get_size_of(node_type<key_type, data_type> *node_);
  // this emit the elments larger than s_ to b_,
  // after this, only smaller in tree
  node_type<key_type, data_type> *split_(short int n_,
                                         rb_tree<key_type, data_type> *b_);
  void split_(node_type<key_type, data_type> *node_,
              rb_tree<key_type, data_type> *b_);
  data_type search(key_type key);
  node_type<key_type, data_type> *search_eq_or_gt(key_type key);
  std::vector<node_type<key_type, data_type> *> *search_range(key_type low,
                                                              key_type high);

  // protected:

  bool insert_(key_type key, data_type data);
  bool insert_(node_type<key_type, data_type> *inode);
  node_type<key_type, data_type> *
  sibling_of_(node_type<key_type, data_type> *node_);
  node_type<key_type, data_type> *next_(node_type<key_type, data_type> *node_);
  node_type<key_type, data_type> *prev_(node_type<key_type, data_type> *node_);
  void cut_out_(node_type<key_type, data_type> *cnode,
                node_type<key_type, data_type> *child_of_);
  void swap_parent_(node_type<key_type, data_type> *old_node,
                    node_type<key_type, data_type> *new_node);
  void left_rotate_(node_type<key_type, data_type> *lnode);
  void right_rotate_(node_type<key_type, data_type> *rnode);
  virtual void copy_data_(node_type<key_type, data_type> *from,
                          node_type<key_type, data_type> *to);
  virtual node_type<key_type, data_type> *search_(key_type key);
  virtual bool delete_(key_type key);
  void destory_(node_type<key_type, data_type> *node_);
  void update_left_size_(node_type<key_type, data_type> *node_, int sz = 1);
  void fix_insert_(node_type<key_type, data_type> *node_);
  bool is_red_(node_type<key_type, data_type> *node_) {
    return node_ and node_->is_red();
  };
  void fix_delete_(node_type<key_type, data_type> *dnode,
                   node_type<key_type, data_type> *pod,
                   node_type<key_type, data_type> *cod);
  void search_range_(std::vector<node_type<key_type, data_type> *> *rs,
                     node_type<key_type, data_type> *node_, key_type low,
                     key_type high);
  //  node_type<key_type, data_type>* nth_(short int n);
  void split__(node_type<key_type, data_type> *node_, bool was_left,
               rb_tree<key_type, data_type> *s_tree,
               rb_tree<key_type, data_type> *b_tree);

private:
  node_type<key_type, data_type> *root;
};

template <class key_type, class data_type,
          template <class, class> class node_type>
rb_tree<key_type, data_type, node_type>::rb_tree(
    node_type<key_type, data_type> *node_) {
  root = node_;
  if (root) {
    root->color = node_color::black;
    root->parent = nullptr;
  }
}

template <class key_type, class data_type,
          template <class, class> class node_type>
inline short int rb_tree<key_type, data_type, node_type>::get_size_of(
    node_type<key_type, data_type> *node_) {
  short int sum = 0;
  while (node_) {
    sum += (node_->left_size + 1);
    node_ = node_->right;
  }
  return sum;
}

template <class key_type, class data_type,
          template <class, class> class node_type>
inline void rb_tree<key_type, data_type, node_type>::split_(
    node_type<key_type, data_type> *node_, rb_tree<key_type, data_type> *b_) {
  assert(node_);
  rb_tree<key_type, data_type> *s_tree =
      new rb_tree<key_type, data_type>(node_->left);
  rb_tree<key_type, data_type> *b_tree =
      new rb_tree<key_type, data_type>(node_->right);

  auto it_ = node_;
  // auto parent = nullptr;
  if (it_->parent) {
    split__(it_->parent, it_->is_left_child_of_parent(), s_tree, b_tree);
  }

  this->root = s_tree->root;
  s_tree->root = nullptr;
  b_->root = b_tree->root;
  b_tree->root = nullptr;
  // FIXME without delete s_tree,  memory leak
  // delete it will cause some error some time
  // delete s_tree;
  delete b_tree;
  // return node_;
}
template <class key_type, class data_type,
          template <class, class> class node_type>
inline node_type<key_type, data_type> *
rb_tree<key_type, data_type, node_type>::split_(
    short int n_, rb_tree<key_type, data_type> *b_) {
  // node_type<key_type, data_type>* tmp = s_->left;
  //  assert(n_ < get_size_of(root));
  auto s_ = nth(n_);
  if (not s_) {
    return nullptr;
  }
  split_(s_, b_);
  return s_;
}

template <class key_type, class data_type,
          template <class, class> class node_type>
inline void rb_tree<key_type, data_type, node_type>::split__(
    node_type<key_type, data_type> *node_, bool was_left,
    rb_tree<key_type, data_type> *s_tree,
    rb_tree<key_type, data_type> *b_tree) {
  bool cont_ = false, new_was_left;
  node_type<key_type, data_type> *gp;
  if (node_->parent) {
    cont_ = true;
    gp = node_->parent;
    new_was_left = node_->is_left_child_of_parent();
  }
  if (was_left) {
    b_tree->join_(node_, new rb_tree<key_type, data_type>(node_->right));
  } else {
    auto tmp = new rb_tree<key_type, data_type>(node_->left);
    tmp->join_(node_, s_tree);
    s_tree->root = tmp->root;
    tmp->root = nullptr;
    delete tmp;
  }
  if (cont_) {
    split__(gp, new_was_left, s_tree, b_tree);
  }
}
template <class key_type, class data_type,
          template <class, class> class node_type>
inline void rb_tree<key_type, data_type, node_type>::join_(
    node_type<key_type, data_type> *m_, rb_tree<key_type, data_type> *b_) {
  m_->left = nullptr;
  m_->right = nullptr;
  m_->parent = nullptr;
  m_->color = node_color::red;
  m_->left_size = 0;
  m_->rank = 1;
  if ((not root) and (not b_->root)) {
    root = m_;
    m_->color = node_color::black;
    m_->left_size = 0;
    m_->rank = 1;
  } else if (not root) {
    b_->insert_(m_);
    b_->fix_insert_(m_);
    // DONE fix left size;
    b_->update_left_size_(m_); // only one node insert
    root = b_->root;
  } else if (not b_->root) {
    insert_(m_);
    fix_insert_(m_);
    // DONE fox left size;
    // m_ is the largest in the tree, no left size to be update
  } else {
    if (b_->root->rank < root->rank) {
      node_type<key_type, data_type> *s__ = root->right;
      while (b_->root->rank < s__->rank) {
        s__ = s__->right;
      }
      m_->parent = s__->parent;
      s__->parent->right = m_;
      s__->parent = m_;
      m_->left = s__;
      m_->right = b_->root;
      b_->root->parent = m_;
      m_->rank = s__->rank + 1;
      m_->color = node_color::red;
      // TODO fix left_size, m_ is right child of it ancestors
      // only m_' left size need to update
      m_->left_size = get_size_of(s__);
      fix_insert_(m_);
    } else if (b_->root->rank > root->rank) {
      node_type<key_type, data_type> *b__ = b_->root->left;
      while (b__->rank > root->rank) {
        b__ = b__->left;
      }
      m_->parent = b__->parent;
      b__->parent->left = m_;
      b__->parent = m_;
      m_->right = b__;
      m_->left = root;
      root->parent = m_;
      m_->rank = root->rank + 1;
      // TODO fix left size
      m_->left_size = get_size_of(root);
      b_->update_left_size_(m_, m_->left_size + 1);
      m_->color = node_color::red;
      b_->fix_insert_(m_);
      root = b_->root;
    } else {
      m_->rank = root->rank + 1;
      m_->color = node_color::black;
      m_->left = root;
      root->parent = m_;
      m_->right = b_->root;
      b_->root->parent = m_;
      // TODO m's left_size need update
      m_->left_size = get_size_of(root);
      root = m_;
    }
  }
  // if b_ is not null, its nodes move to s_ now,
  // delete it
  if (b_->root) {
    b_->root = nullptr;
    delete b_;
  }
}

template <class key_type, class data_type,
          template <class, class> class node_type>
inline node_type<key_type, data_type> *
rb_tree<key_type, data_type, node_type>::nth(short int n) {
  // if((not root) or (n<0)) {
  //   return nullptr;
  // }
  auto tmp = root;
  while (((n >= 0) && (tmp != nullptr))) {
    if (tmp->left_size == n) {
      return tmp;
    } else if (tmp->left_size > n) {
      tmp = tmp->left;
    } else {
      n -= (tmp->left_size + 1);
      tmp = tmp->right;
    }
  }
  return nullptr;
}

template <class key_type, class data_type,
          template <class, class> class node_type>
inline void rb_tree<key_type, data_type, node_type>::destory_(
    node_type<key_type, data_type> *node_) {
  if (node_->left) {
    destory_(node_->left);
  }
  if (node_->right) {
    destory_(node_->right);
  }
  delete node_;
}

template <class key_type, class data_type,
          template <class, class> class node_type>
inline void rb_tree<key_type, data_type, node_type>::update_left_size_(
    node_type<key_type, data_type> *node_, int sz) {
  // node_type<key_type, data_type> *it = node_;
  while (node_ != root) {
    if (node_ == node_->parent->left) {
      node_->parent->left_size += sz;
    }
    node_ = node_->parent;
  }
}

template <class key_type, class data_type,
          template <class, class> class node_type>
inline node_type<key_type, data_type> *
rb_tree<key_type, data_type, node_type>::sibling_of_(
    node_type<key_type, data_type> *node_) {
  if (not node_->parent) {
    return nullptr;
  } else if (node_->is_left_child_of_parent()) {
    return node_->parent->right;
  } else {
    return node_->parent->left;
  }
};

// return min element in right subtree of node_
template <class key_type, class data_type,
          template <class, class> class node_type>
inline node_type<key_type, data_type> *
rb_tree<key_type, data_type, node_type>::next_(
    node_type<key_type, data_type> *node_) {
  // assert(node_->right);
  if (not node_->right) {
    return nullptr;
  }
  node_type<key_type, data_type> *min = node_->right;
  while (min->left) {
    min = min->left;
  }
  return min;
};
// return max element in left subtree of node_
template <class key_type, class data_type,
          template <class, class> class node_type>
inline node_type<key_type, data_type> *
rb_tree<key_type, data_type, node_type>::prev_(
    node_type<key_type, data_type> *node_) {
  // assert(node_->left);
  if (not node_->left) {
    return nullptr;
  }
  node_type<key_type, data_type> *max = node_->left;
  while (max->right) {
    max = max->right;
  }
  return max;
};

template <class key_type, class data_type,
          template <class, class> class node_type>
inline bool rb_tree<key_type, data_type, node_type>::delete_(key_type key) {
  node_type<key_type, data_type> *r_ = search_(key);
  if (not(r_->key == key)) {
    return false; // not found;
  }
  if ((r_->left) && (r_->right)) {
    node_type<key_type, data_type> *prev_node = prev_(r_);
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
  return true;
};
// if pod is not null, it can have at most 1 child;
template <class key_type, class data_type,
          template <class, class> class node_type>
inline void rb_tree<key_type, data_type, node_type>::fix_delete_(
    node_type<key_type, data_type> *dnode, node_type<key_type, data_type> *pod,
    node_type<key_type, data_type> *cod) {
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
  node_type<key_type, data_type> *sibling = nullptr;
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
      node_type<key_type, data_type> *s_r = sibling->right;
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
      node_type<key_type, data_type> *s_l = sibling->left;
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
template <class key_type, class data_type,
          template <class, class> class node_type>
inline void rb_tree<key_type, data_type, node_type>::swap_parent_(
    node_type<key_type, data_type> *old_node,
    node_type<key_type, data_type> *new_node) {
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
template <class key_type, class data_type,
          template <class, class> class node_type>
inline void rb_tree<key_type, data_type, node_type>::left_rotate_(
    node_type<key_type, data_type> *lnode) {
  node_type<key_type, data_type> *rc = lnode->right;
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
template <class key_type, class data_type,
          template <class, class> class node_type>
inline void rb_tree<key_type, data_type, node_type>::right_rotate_(
    node_type<key_type, data_type> *rnode) {
  node_type<key_type, data_type> *lc = rnode->left;
  rnode->left_size -= (lc->left_size + 1);
  rnode->left = lc->right;
  if (lc->right != nullptr) {
    lc->right->parent = rnode;
  }
  swap_parent_(rnode, lc);
  lc->right = rnode;
}

template <class key_type, class data_type,
          template <class, class> class node_type>
inline void rb_tree<key_type, data_type, node_type>::search_range_(
    std::vector<node_type<key_type, data_type> *> *rs,
    node_type<key_type, data_type> *node_, key_type low, key_type high) {
  if (node_) {
    if (node_->key < low) {
      search_range_(rs, node_->right, low, high);
    } else if (node_->key > high) {
      search_range_(rs, node_->left, low, high);
    } else {
      search_range_(rs, node_->left, low, node_->key);
      // std::cout << node_->key << " " << node_->data << ";";
      rs->push_back(node_);
      search_range_(rs, node_->right, node_->key, high);
    }
  }
}

template <class key_type, class data_type,
          template <class, class> class node_type>
inline std::vector<node_type<key_type, data_type> *> *
rb_tree<key_type, data_type, node_type>::search_range(key_type low,
                                                      key_type high) {
  std::vector<node_type<key_type, data_type> *> *rs =
      new std::vector<node_type<key_type, data_type> *>();
  search_range_(rs, root, low, high);
  return rs;
}
template <class key_type, class data_type,
          template <class, class> class node_type>
inline data_type rb_tree<key_type, data_type, node_type>::search(key_type key) {
  node_type<key_type, data_type> *it = root;
  while (it) {
    if (it->key == key) {
      return it->data;
    } else if (it->key < key) {
      it = it->right;
    } else {
      it = it->left;
    }
  }
  return NULL;
}
template <class key_type, class data_type,
          template <class, class> class node_type>
inline node_type<key_type, data_type> *
rb_tree<key_type, data_type, node_type>::search_eq_or_gt(key_type key) {
  node_type<key_type, data_type> *it = root, *min_q = root;
  while (it) {
    if (it->key == key) {
      return it;
    } else if (it->key < key) {
      min_q = it;
      it = it->right;
    } else {
      min_q = it;
      it = it->left;
    }
  }
  return min_q;
}

template <class key_type, class data_type,
          template <class, class> class node_type>
inline node_type<key_type, data_type> *
rb_tree<key_type, data_type, node_type>::search_(key_type key) {
  node_type<key_type, data_type> *it = root;

  while (true) {
    if (it->key == key) {
      break;
    } else if (key < it->key) {
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

template <class key_type, class data_type,
          template <class, class> class node_type>
inline bool rb_tree<key_type, data_type, node_type>::insert_(
    node_type<key_type, data_type> *inode) {
  if (root == nullptr) {
    root = inode;
    root->color = node_color::black;
    return true;
  }
  auto tmp = root;
  auto parent = root;
  while (tmp) {
    parent = tmp;
    if (tmp->key == inode->key) {
      if (tmp->data != inode->data) {
        tmp->data = inode->data;
        // FIX ME with key-value data, DONE I THINK
        delete inode;
      }
      return false; // update value set as false, since no node added
    } else if (tmp->key < inode->key) {
      tmp = tmp->right;
    } else {
      tmp = tmp->left;
    }
  }
  inode->parent = parent;
  if (inode->key > parent->key) {
    parent->right = inode;
  } else {
    parent->left = inode;
  }
  update_left_size_(inode);
  fix_insert_(inode);
  return true;
};

template <class key_type, class data_type,
          template <class, class> class node_type>
inline void rb_tree<key_type, data_type, node_type>::fix_insert_(
    node_type<key_type, data_type> *node_) {
  if (node_ == root) {
    node_->color = node_color::black;
    return;
  }
  // if node_->parent is red, it must have parent too.
  if (node_->is_red() && node_->parent->is_red()) {
    node_type<key_type, data_type> *uncle = sibling_of_(node_->parent);
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
      node_type<key_type, data_type> *gp = node_->parent->parent;
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

template <class key_type, class data_type,
          template <class, class> class node_type>
inline bool rb_tree<key_type, data_type, node_type>::insert_(key_type key,
                                                             data_type data) {
  // FIX ME, memory leak when insert dup data;
  node_type<key_type, data_type> *i =
      new node_type<key_type, data_type>(key, data);
  // node_type<key_type, data_type> *ri =
  return insert_(i);
  // if(i) {
  //   update_left_size_(i);
  //   fix_insert_(i);
  // }
};

template <class key_type, class data_type,
          template <class, class> class node_type>
inline void rb_tree<key_type, data_type, node_type>::copy_data_(
    node_type<key_type, data_type> *from, node_type<key_type, data_type> *to) {
  to->key = from->key;
  to->data = from->data;
}

} // namespace cop5536

#endif /* RB_TREE_1_H_ */
