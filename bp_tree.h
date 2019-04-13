#ifndef BP_TREE_H_
#define BP_TREE_H_
#include "rb_tree.h"
#include <future>
#include <iostream>
#include <vector>
#include <fstream>

#define OUTPUT "output_file.txt"

namespace cop5536 {
class bp_node {
public:
  unsigned short node_size; // number of keys in rb_tree
  // public:
  bp_node() : node_size(0){};
  explicit bp_node(unsigned short ns) : node_size(ns){};
  virtual ~bp_node(){};
  virtual void destory_() = 0;
  virtual void print() = 0;
  virtual bool is_leaf() = 0;
  bool is_full(unsigned short degree) { return this->node_size >= degree; };
 
  virtual float search(int key) = 0;
  virtual std::vector<float>* search(int low, int high) = 0;
  virtual bool remove(int key) = 0;
  virtual bool splitable() {return this->node_size >= 2;};
  virtual rb_tree_node<int, bp_node*>* split(bool is_left) = 0;
  // virtual void split();
  virtual rb_tree_node<int, bp_node *>* insert_(int key, float value,
                                                unsigned short degree) = 0;
};

// internal node

class bp_node_i : public bp_node {
public:
  bp_node_i() = delete; //: bp_node<int, bp_node<int, float>>(),
                        //first_child(nullptr), children(nullptr) {};
  explicit bp_node_i(unsigned short ns, bp_node *fc,
                     rb_tree<int, bp_node *> *children)
      : bp_node(ns), first_child(fc), children(children){};
  virtual ~bp_node_i() override { this->destory_();};
  virtual void destory_() override;
  virtual void print() override;
  virtual bool is_leaf() override { return false; };
  virtual float search(int key) override;
  virtual std::vector<float>* search(int low, int high) override;
  virtual bool remove(int key) override;
  virtual rb_tree_node<int, bp_node*>* split(bool is_left) override;
  virtual rb_tree_node<int, bp_node*>* insert_(int key, float value,
                                        unsigned short degree) override;
  bp_node* first_child;              // pointer to subtree with smallest key
  rb_tree<int, bp_node*> *children; // all other children, in a red black tree
};

// leaf node
class bp_node_l : public bp_node {
public:
  bp_node_l() : bp_node(), data(nullptr){}; // , next(nullptr), prev(nullptr){};
  explicit bp_node_l(unsigned short ns, rb_tree<int, float> *ds)
      : bp_node(ns), data(ds){}; //, next(nullptr),
  // prev(nullptr){};
  virtual void print() override;
  virtual bool is_leaf() override { return true; };
  // void insert(int key, float value) override;
  virtual float search(int key) override;
  virtual std::vector<float>* search(int low, int high) override;
  virtual ~bp_node_l() override { this->destory_();};
  virtual void destory_() override;
  virtual bool remove(int key) override;
  virtual rb_tree_node<int, bp_node*>* split(bool is_left) override;
  virtual rb_tree_node<int, bp_node*>* insert_(int key, float value,
                                        unsigned short degree) override;
  rb_tree<int, float> *data;
};
// B plus tree

class bp_tree {
public:
  bp_tree(){};
  explicit bp_tree(unsigned short m) {
    assert(m > 2);
    degree = m;
    root = nullptr;
    open_output();
  };
  ~bp_tree(){
    close_output();
    delete root;
  };
  void insert(int key, float value);
  void search(int key);
  void search(int low, int high);
  void remove(int key);
  void print();
  void open_output() {
    output.open(OUTPUT, std::ios::out | std::ios::trunc);
  };
  void close_output() {
    output.close();
  }
private:
  std::ofstream output;
  unsigned short degree;
  bp_node *root;
};

inline void bp_node_l::destory_() {
  delete data;
}
inline void bp_node_i::destory_() {
  if(this->first_child->is_leaf()) {
    delete this->first_child;
  } else {
    this->first_child->destory_();
  }
 
  if(this->children) {
    auto it = this->children->nth(0);
    while(it) {
      if(it->data->is_leaf()) {
        delete it->data;
      } else {
        it->data->destory_();
        it->data = nullptr;
      }
      it = this->children->next_(it);
    }
  }
  delete this->children;
}
void bp_tree::insert(int key, float value) {
  if (root == nullptr) {
    rb_tree<int, float> *rb_i = new rb_tree<int, float>();
    rb_i->insert(key, value);
    bp_node_l *bp_l = new bp_node_l(2, rb_i);
    root = bp_l;
    return;
  }
  if (root->is_leaf()) {
    rb_tree_node<int, bp_node *> *n_bp_i_node = nullptr;
    // call the function of real insert
    std::future<rb_tree_node<int, bp_node *> *> fu =
        std::async(std::launch::async, &bp_node::insert_, this->root, key,
                   value, this->degree);
    n_bp_i_node = fu.get();
    if (n_bp_i_node) {
      // create new bp internal node
      rb_tree<int, bp_node*>* new_root_data =
          new rb_tree<int, bp_node*>(n_bp_i_node);
      bp_node_i* new_root =
          new bp_node_i(2, root, new_root_data);
      root = new_root;
    }
  } else {
    bp_node_i* i_node = static_cast<bp_node_i*>(root);
    rb_tree_node<int, bp_node*>* ins_to = i_node->children->search_eq_or_gt(key);
    bp_node* key_node;
    if (ins_to->key > key) {
      key_node = i_node->first_child;
      // ins_to->data = i_node->first_child;
    } else {
      key_node = ins_to->data;
    }
    rb_tree_node<int, bp_node *> *n_bp_i_node = nullptr;
    // call the function of real insert
    std::future<rb_tree_node<int, bp_node *> *> fu =
        std::async(std::launch::async, &bp_node::insert_, key_node, key,
                   value, this->degree);
    n_bp_i_node = fu.get();
    if (n_bp_i_node) {
      if(i_node->children->insert_(n_bp_i_node)) {
        i_node->node_size += 1;
      }
      if (degree < i_node->node_size) {
        auto rp_ = i_node->split(false);
        rb_tree<int, bp_node*>* new_root_data =  new rb_tree<int, bp_node*>(rp_);
        bp_node_i* new_root = new bp_node_i(2, root, new_root_data);
        root = new_root;
      }
    }
  }
}


inline rb_tree_node<int, bp_node *> *bp_node_i::insert_(int key, float value,
                                                        unsigned short degree) {
  rb_tree_node<int, bp_node*> *ins_to = this->children->search_eq_or_gt(key);
  bp_node* key_node;
  if (ins_to->key > key) {
    key_node = this->first_child;
  } else {
    key_node = ins_to->data;
  }
  rb_tree_node<int, bp_node *> *n_bp_i_node = nullptr;
  // call the function of real insert
  std::future<rb_tree_node<int, bp_node *> *> fu = std::async(
      std::launch::async, &bp_node::insert_, key_node, key, value, degree);
  n_bp_i_node = fu.get();
  if (n_bp_i_node) {
    if(this->children->insert_(n_bp_i_node)) {
      this->node_size += 1;
    }
    if (degree < this->node_size) {
      // need split
      return this->split(false);
    }
  }
  return nullptr;
}

inline rb_tree_node<int, bp_node *> *bp_node_l::insert_(int key, float value,
                                                        unsigned short degree) {
  if(data->insert(key, value)) {
    this->node_size += 1;
  }
  if (this->node_size > degree) { // full node split;
    return this->split(false);
  } else {
    return nullptr;
  }
}

inline rb_tree_node<int, bp_node*>* bp_node_l::split(bool is_left) {
  // does not matter left or right
  rb_tree<int, float> *n_rb_node = new rb_tree<int, float>();
  unsigned short l_before = this->node_size;
  auto rp_ = this->data->split_(l_before/2 - 1, n_rb_node);
  this->node_size /= 2;
  int l_key = rp_->key;
  // insert the split key value to new leaf node
  n_rb_node->insert(rp_->key, rp_->data);
  // should delete rp_
  // create new leaf node
  bp_node_l* bp_l = new bp_node_l(l_before/2+1, n_rb_node);
  // create new internal bp node, for insertion to its parent node;
  rb_tree_node<int, bp_node*>* n_bp_node =
      new rb_tree_node<int, bp_node*>(l_key, bp_l);
  return n_bp_node;
}

inline rb_tree_node<int, bp_node*>* bp_node_i::split(bool is_left) {
  // need to know from left or right
  // FIX ME, splited node size_node not right
  assert(this->node_size > 2);
  rb_tree<int, bp_node*> *n_rb_node = new rb_tree<int, bp_node*>();
  unsigned short l_before = this->node_size;
  auto rp_ = this->children->split_(l_before/2 - 1, n_rb_node);
  if (is_left) {
    bp_node_i* r_bp = new bp_node_i(l_before/2, this->first_child, this->children);
    rb_tree_node<int, bp_node*>* r_rb_node = new rb_tree_node<int, bp_node*>(rp_->key, r_bp);
    // FIX ME not right
    this->first_child = rp_->data;
    this->children = n_rb_node;
    this->node_size = l_before/2;
    return r_rb_node;
  } else {
    bp_node_i* r_bp = new bp_node_i(l_before/2, rp_->data, n_rb_node);
    rb_tree_node<int, bp_node*>* r_rb_node = new rb_tree_node<int, bp_node*>(rp_->key, r_bp);
    this->node_size = l_before/2;
    return r_rb_node;
  }
}

inline bool bp_node_l::remove(int key) {
  if(this->data->delete_key(key)) {
    this->node_size -= 1;
  }
  return this->node_size == 1;
}

inline bool bp_node_i::remove(int key) {
  // fxxx, so many copy and paste
  rb_tree_node<int, bp_node*>* ins_to = this->children->search_eq_or_gt(key);
  bp_node* key_node;
  if (ins_to->key > key) {
    key_node = this->first_child;
  } else {
    key_node = ins_to->data;
  }
  bool deficient = false;
  std::future<bool> fu = std::async(std::launch::async, &bp_node::remove, key_node, key);
  deficient = fu.get();
  if(deficient) {
    if(key_node->is_leaf()) {
      //empty leaf
      // check sibling
      if (key_node == this->first_child) {
        if(this->first_child->is_leaf()) {
          auto rp_ = ins_to->data->split(true);
          delete this->first_child;
          this->first_child = rp_->data;
          if(ins_to->data) {
            ins_to->key = rp_->key;
          } else {
            this->children->delete_(ins_to->key);
          }
        } else {
          
        }
        
        return this->children == nullptr;
      } else {
        // try left sibling first
        bp_node* sibling;
        rb_tree_node<int, bp_node*>* sibling_rb_node = this->children->prev_(ins_to);
        if(not sibling_rb_node) {
          sibling = this->first_child;
        } else {
          sibling = sibling_rb_node->data;
        }
        if(sibling->splitable()) {
          // split left sibling
          // auto rp_ = sibling->split(true);
          this->children->insert_(sibling->split(true));
          this->node_size += 1;
        } else {
          // try right sibling
          sibling_rb_node = this->children->next_(ins_to);
          if(sibling_rb_node) {
            sibling = sibling_rb_node->data;
            if(sibling->splitable()) {
              //split right child
              this->children->insert_(sibling->split(true));
              this->node_size += 1;
            }
          } 
        }
        // TODO let rb tree can delete node without search first
        this->children->delete_key(ins_to->key);
        this->node_size -= 1;
      }
    } else { // deficient index node
      //internal node, only first child left
      // find sibling, if sibling can split, split it
      // if not
      bp_node_i* i = static_cast<bp_node_i*>(key_node);
      if(key_node == this->first_child) {
        if (ins_to->data->splitable()) {
          rb_tree_node<int, bp_node*>* rp_ = ins_to->data->split(true);
          int tmp = ins_to->key;
          ins_to->key = rp_->key;
          bp_node_i* rpi = static_cast<bp_node_i*>(rp_->data);
          i->children = rpi->children;
          i->children->insert(tmp, rpi->first_child);
          i->node_size += rpi->node_size;
          
          rpi->children=nullptr;
          rpi->first_child=nullptr;
          delete rpi;
          rp_->data = nullptr;
          delete rp_;
          return false;
        } else { // sibling can not split, merge
          bp_node_i* ins_data = static_cast<bp_node_i*>(ins_to->data);
          i->children = ins_data->children;
          i->children->insert(ins_to->key, ins_data->first_child);
          i->node_size += ins_data->node_size;
          
          ins_data->children = nullptr;
          ins_data->first_child = nullptr;
          delete ins_data;
          ins_to->data = nullptr;
          // TODO rewrite when rb tree can delete by node addr;
          this->children->delete_key(ins_to->key);
          // return this->children == nullptr;
        }
      } else {
        bp_node *sibling_l;
        rb_tree_node<int, bp_node*>* sibling_rb_node_l = this->children->prev_(ins_to);
        if(not sibling_rb_node_l) {
          sibling_l = this->first_child;
        } else {
          sibling_l = sibling_rb_node_l->data;
        }
        if(sibling_l->splitable()) {
          rb_tree_node<int, bp_node*>* rp_ = sibling_l->split(false);
          int tmp = ins_to->key;
          ins_to->key = rp_->key;
          bp_node_i* rpi = static_cast<bp_node_i*>(rp_->data);
          
          i->children = rpi->children;
          i->children->insert(tmp, i->first_child);
          i->node_size += rpi->node_size;
          i->first_child = rpi->first_child;
          i->node_size += rpi->node_size;
          
          rpi->children=nullptr;
          rpi->first_child=nullptr;
          delete rpi;
          rp_->data = nullptr;
          delete rp_;
          return false;
        } else {
          bp_node_i* sibling_i = static_cast<bp_node_i*>(sibling_l);
          sibling_i->children->insert(ins_to->key, i->first_child);

          i->first_child = nullptr;
          this->children->delete_key(ins_to->key);
          //return this->children == nullptr;
        }  
      }
    }
  }
  return  this->children == nullptr; // must be the first child
}

void bp_tree::remove(int key) {
  if (root == nullptr) {
    return;
  }
  if(root->is_leaf()) {
    bp_node_l* l_node = static_cast<bp_node_l*>(root);
    l_node->data->delete_key(key);
  } else {
    bp_node_i* i_node = static_cast<bp_node_i*>(root);
    rb_tree_node<int, bp_node*>* ins_to = i_node->children->search_eq_or_gt(key);
    bp_node* key_node;
    if (ins_to->key > key) {
      key_node = i_node->first_child;
      // ins_to->data = i_node->first_child;
    } else {
      key_node = ins_to->data;
    }
    bool child_is_empty = false;
    std::future<bool> fu = std::async(std::launch::async, &bp_node::remove, key_node, key);
    child_is_empty = fu.get();
    if(child_is_empty) {
      //
    }
  }
}

inline float bp_node_i::search(int key) {
  rb_tree_node<int, bp_node*> *ins_to = this->children->search_eq_or_gt(key);
  if (ins_to->key > key) {
    return this->first_child->search(key);
  } else {
    return ins_to->data->search(key);
  }
};

inline float bp_node_l::search(int key) {
  // std::cout << "searching in leaf" << std::endl;
  return this->data->search(key);
  
};


inline std::vector<float>* bp_node_l::search(int low, int high) {
  // std::cout << "search in leaf" << std::endl;
  std::vector<rb_tree_node<int, float>*>* rb_nodes = this->data->search_range(low, high);
  std::vector<float>* rs_ = new std::vector<float>();
  for(auto& node : *rb_nodes) {
    rs_->push_back(node->data);
  }
  return rs_;
  // delete rb_nodes;
}

inline std::vector<float>* bp_node_i::search(int low, int high) {
  //  std::cout << "search in first child" << std::endl;
  std::vector<float>* rs_ = new std::vector<float>();
  auto fc = this->first_child->search(low, high);
  rs_->insert(rs_->end(), fc->begin(), fc->end());
  delete fc;
  //std::cout << "search in children" << std::endl;
  auto rb_nodes = this->children->search_range(low, high);
  for (auto& it: *rb_nodes) {
    // std::cout << "search in internal with key " << it->key << std::endl;
    auto rs__ = it->data->search(low, high);
    rs_->insert(rs_->end(), rs__->begin(), rs__->end());
    delete rs__;
  }

  return rs_;
  // delete rb_nodes;
  // TODO
}

inline  void bp_tree::search(int key) {
  if (root == nullptr) {
    this->output << "Null" << std::endl;
  }
  auto s_ = root->search(key);
  if(s_ != NULL) {
    this->output << s_ << std::endl;
  } else {
    this->output << "Null" << std::endl;
  }
}

void bp_tree::search(int low, int high) {
  if (root == nullptr) {
    this->output << "Null" << std::endl;
    return;
  } 
  // std::vector<float>* rs_ = new std::vector<float>();
  auto rs_ = root->search(low, high);
  std::ostringstream ss;
  for(auto r:*rs_) {
    ss << r << ",";
  }
  this->output << ss.str() << std::endl;
}

void bp_tree::print() {
  if(root == nullptr) {
    std::cout << "empty bp tree" << std::endl;
  }
  root->print();
}
inline void bp_node_i::print() {
  std::cout<< std::endl << "idx: ";
  std::vector<int> keys;
  this->children->get_keys(keys);
  for(auto key: keys) {
    std::cout << key << ", ";
  }
  std::cout << "ns: " << this->node_size;
  std::cout << std::endl;

  this->first_child->print();
  auto rb_nodes = this->children->search_range(-10000000, 10000000);
  for(auto &it : *rb_nodes) {
    it->data->print();
  }
  std::cout << "+++++++++++" << std::endl;
}

inline void bp_node_l::print() {
  std::cout << "l: ns="<< this->node_size << std::endl;
  this->data->print_tree();
  // auto it = this->data->nth(0);
  // while(it) {
  //   std::cout << it->key << ": " << it->data << ", ";
  //   it = this->data->next_(it);
  // }
  // std::cout << ";  ";
}
} // namespace cop5536
#endif /* BP_TREE_H_ */
