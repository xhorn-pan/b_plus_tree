#ifndef RED_BLACK_TREE_H_
#define RED_BLACK_TREE_H_

#include <iostream>
#include <cassert>
#include "bs_tree.h"

namespace cop5536 {
  enum class node_color { red = 0, black };
  enum class side { left = 0, right = 1 };

  template <typename TKey = long long, typename TData = double>
  class rb_tree : protected bs_tree<TKey, TData>{
  public:
    rb_tree(): root(nullptr){};
    ~rb_tree();
    void insert(const TKey key, const TData data) override;
    TData search(const TKey key) override;
    void delete_key(const TKey key) override;
  protected:
    struct rb_node :  bs_tree<TKey, TData>::node {
      node_color color;
      short int left_size;
      short int rank;
      rb_node();
      rb_node(TKey key, TData data)
	: bs_tree<TKey, TData>::node(key, data),
	color(node_color::red), left_size(0), rank(1){};
    };
    inline rb_node* sibling_of_(rb_node* node) {
      if (node == node->parent->left) {
	return node->parent->right;
      } else {
	return node->paretn->left;
      }
    }
    inline void switch_color_(rb_node* node) {
      node->color = (node->color == node_color::black) ? node_color::red
                                                       : node_color::black;
    }
    inline void update_left_size_(rb_node* node) {
      while(node != root) {
	if (node == node->parent->left) {
	  node->parent->left_size += 1;
	}
	node = node->parent;
      }
    }
    
    inline void insert_fix_(rb_node* node) {
      if (node == root) {
	node->color = node_color::black;
	return;
      }
      if ((node->color == node_color::red) &&
          (node->parent->color == node_color::red)) {
	//node->parent->parent must not null
	rb_node* sibling_of_parent = sibling_of_(node->parent);
        if ((sibling_of_parent) &&
            (sibling_of_parent->color == node_color::red)) {
	  /** type XYr   |                            ||
	   *            gp[x]                         gp[x+1]
	   *           // \\                         /  \
	   *  (parent)pp   d(sibling_of parent)     pp   d[x]
	   *         //     [x]                    //
	   *  (node) p[x]                          p[x]
	   * no rotate, no left_size change
	   * rank change: gp(+1)
	   * color change: pp(r=>b), d(r=>b), gp(b=>r)
	   * continue fix on gp(sibling_of_parent->parent)
	   */
	  // sibling_of_parent->color = node_color::black;
	  // node->parent->color = node_color::black;
	  // sibling_of_parent->parent->color = node_color::red;
	  switch_color_(sibling_of_parent);
	  switch_color_(node->parent);
	  switch_color_(sibling_of_parent->parent);
	  
	  sibling_of_parent->parent->rank += 1;
	  insert_fix_(sibling_of_parent->parent);
	} else {
	  rb_node* gp = node->parent->parent;
	  if(node == node->parent->left) {//type XLb
	    if (node->parent == gp->left) {
	      /** LLb           |                          |
	       *           (gp) z                          y
	       *              // \                       // \\
	       *              y   d    =====>            x    z
	       *            // \                        / \  / \
	       *     (node) x   c                      a   b c  d
	       *           / \
	       *          a   b
	       * right rotate (gp), rank unchange
	       * color change, y, gp
	       * left_size change: z
	       *     z_before = z_after + y_before + 1
	       */
	      this->right_rotate(gp);
	      switch_color_(node->parent);
	      switch_color_(gp);
	      gp->left_size -= (node->parent->left_size + 1);
	    } else {
	      /** RLb           |                  |                  |
	       *           (gp) z                  z                  x
	       *               / \\               / \\              // \\
	       *              a   y        ====> a   x       ====>  z   y
	       *                 // \               / \\           / \  / \
	       *          (node) x   d             b    y         a   b c  d
	       *                / \                    / \
	       *               b   c                  c   d
	       * right rotate(y), left rotate(z) rank unchange
	       * color change z, x
	       * left size change: x, y
	       * x_after = x_before + z_before + 1
	       * y_before = x_before + y_after + 1
	       */
	      this->right_rotate(node->parent);
	      this->left_rotate(gp);
	      switch_color_(gp);
	      switch_color_(node);
	      node->parent->left_size -= (node->left_size + 1);
	      node->left_size += (gp->left_size + 1);
	    }
	  } else {//type XRb
	    if (node->parent == gp->left) {
	      /** LRb           |                 |             |
	       *           (gp) z                 z             x
	       *              // \              // \          // \\
	       *              y   d    =====>   x   d  ====>  y    z
	       *             / \\             // \           / \  / \
	       *            a   x (node)      y   c         a   b c  d
	       *               / \           / \
	       *              b   c         a   b
	       * left rotate(y), right rotate(z) rank unchange
	       * color change z, x
	       * left size change: x, z
	       * x_after = x_before + y_before + 1
	       * z_before = x_before + z_after + y_before + 2
	       */
	      left_rotate(node->parent);
	      right_rotate(gp);
	      switch_color_(node);
	      switch_color_(gp);
	      gp->left_size -= (node->left_size + node->parent->left_size + 2);
	      node->left_size += (node->parent->left_size + 1);
	    } else {
	      /** RRb           |                          |
	       *           (gp) z                          y
	       *               / \\                      // \\
	       *              a   y            =====>    z    x
	       *                 / \\                   / \  / \
	       *                b   x (node)           a   b c  d
	       *                   / \
	       *                  c   d
	       * left rotate (gp) rank unchange
	       * color change, y, gp
	       * left_size change: y
	       *     y_after = y_before + z_before + 1
	       */
	      left_rotate(gp);
	      switch_color_(node->parent);
	      switch_color_(gp);
	      node->parent->left_size += (gp->left_size + 1);
	    }
	  }
	}
      }
    }
  private:
    rb_node* root;
  };



  // template <typename TKey, typename TData>
  // inline void rb_tree<TKey, TData>::delete_key(const TKey key) {
  //   // rb_node* rnode = this->search_(key);
  //   // if (node->key == key) {
  //   //   // auto need_deleted = this->delete_phy(node);
  //   //   // TODO fix
  //   // }
  // }

  template <typename TKey, typename TData>
  void rb_tree<TKey, TData>::insert(const TKey key, const TData data) {
    rb_node *ins = new rb_node(key, data);
    // rb_node* insr = this->insert__(static_cast<typename bs_tree<TKey, TData>::node*>(ins));
    rb_node* insr = this->insert__(ins);
    update_left_size_(insr);
    insert_fix_(insr);
  }

// template <typename TData>
// inline void red_black_tree<TData>::delete_fix(rb_tree_node *parent,
//                                               rb_tree_node *to_delete,
//                                               rb_tree_node *deficient) {
//   if(deficient == root) {// continued fix, deficient must be black;
//     return;
//   }
//   // to_delete is nullptr when continue fix after a type Rb0 fix
//   if (to_delete) {
//     if (to_delete == root) {
//       delete to_delete; //delete root, tree is empty now
//       return;
//     }
//     deficient->parent = parent;
//     // to_delete is not root, parent must not null, no need to check null
//     if (to_delete == parent->left) {
//       parent->left = deficient;
//     } else {
//       parent->right = deficient;
//     }
//     auto d_color = to_delete->color;
//     delete to_delete; // TODO: safe to delete now?
//     if (d_color == node_color::red) {
//       return;
//     }
//     if (deficient->color == node_color::red) {
//       // if deficient node is red, make it black. done
//       deficient->color = node_color::black;
//       return;
//     }
//   }

//   rb_tree_node *sibling = nullptr;
//   side side;
//   if (deficient == parent->left) {
//     side = side::left; // type L
//     sibling = parent->right;
//   } else {
//     side = side::right; //type R
//     sibling = parent->left;
//   }
//   if (sibling->color == node_color::black) {//type X_b_x
//     if ((sibling->left && sibling->left->color == node_color::red) &&
//         (sibling->right && sibling->right->color == node_color::red)) {
//       if (side == side::left) {
// 	/**        Lb2       
// 	 *  
// 	 */
//       } else { //Rb2
	
//       }
//     } else if ((sibling->left) && (sibling->color == node_color::red)) {
//       if (side == side::left) { // Lb1 case 2
	
//       } else { //Rb1 case 1
	
//       }
//     } else if ((sibling->right && sibling->right->color == node_color::red)) {
//       if (side == side::left) { // Lb1 case 1
	
//       } else { //Rb1 case 2
	
//       }
//     } else { // Xb0
//       if (side == side::left) { 
	
//       } else { 
	
//       }
//     }
//   } else { // type X_r_x
//     auto sr = sibling->right;
//     if ((sr->left && (sr->left->color == node_color::red)) &&
//         (sr->right && (sr->right->color == node_color::red))) {
//       //Xr2
//     } else if (sr->left && (sr->left->color == node_color::red)) {
//       //Xr1 case 1
//     } else if (sr->right && (sr->right->color == node_color::red)) {
//       //Xr1 case 2
//       if (side == side::left) {
// 	/**  Lr1 case 2   |                             |
// 	 *                py(parent)[r+2]               x [r+2]
// 	 *              // \                         //   \
// 	 *    (sibling) v   y [r]                    v     py [r+1]
// 	 *             / \         =====>           / \   /  \
// 	 *      [r+1] a   w(sr)[r+1]         [r+1] a   w d    y [r]
// 	 *               / \\                         / \
// 	 *          [r] b   x (srr)[r+1]             b   c[r]
// 	 *                 / \
// 	 *             [r]c   d[r]
// 	 * rotate: L(w) L(v) R(py)
// 	 * left_size change : py, x
// 	 *    py_before = v_before + w_before + x_before + 3 + py_after
// 	 *    x_after = x_before + w_before + v_before + 2
// 	 * rank change: x(+1), py(-1)
// 	 * color change: x(r=>b), 
// 	 */
// 	auto srr = sr->right;
// 	left_rotate(sr); left_rotate(sibling); right_rotate(parent);
// 	srr->color = node_color::black;
// 	srr->rank += 1;
// 	parent->rank -= 1;
// 	parent->left_size -= (sibling->left_size + sr->left_size + srr->left_size + 3);
// 	srr->left_size += (sr->left_size + sibling->left_size + 2);
//       } else {
// 	/**  Rr1 case 2   |                             |
// 	 *                py(parent)[r+2]               x [r+2]
// 	 *              // \                         //   \
// 	 *    (sibling) v   y [r]                    v     py [r+1]
// 	 *             / \         =====>           / \   /  \
// 	 *      [r+1] a   w(sr)[r+1]         [r+1] a   w d    y [r]
// 	 *               / \\                         / \
// 	 *          [r] b   x (srr)[r+1]             b   c[r]
// 	 *                 / \
// 	 *             [r]c   d[r]
// 	 * rotate: L(w) L(v) R(py)
// 	 * left_size change : py, x
// 	 *    py_before = v_before + w_before + x_before + 3 + py_after
// 	 *    x_after = x_before + w_before + v_before + 2
// 	 * rank change: x(+1), py(-1)
// 	 * color change: x(r=>b), 
// 	 */
// 	auto srr = sr->right;
// 	left_rotate(sr); left_rotate(sibling); right_rotate(parent);
// 	srr->color = node_color::black;
// 	srr->rank += 1;
// 	parent->rank -= 1;
// 	parent->left_size -= (sibling->left_size + sr->left_size + srr->left_size + 3);
// 	srr->left_size += (sr->left_size + sibling->left_size + 2); 
//       }
//     } else {
//       if (side == side::left) {
// 	/** Lr0          |                               |
// 	 *      (parent) py [x+1]                        v [x+1]
// 	 *              / \\                            / \
// 	 * (deficient) y   v(sibling)[x+1]  ====    [x]py  b
// 	 *                / \                          / \\
// 	 *               a   b[x]                     y   a[x] 
// 	 * left_size change: v_after = py_before + v_before + 1
// 	 * rank change: py(-1)
// 	 * color change: v(r=>b), a(b=>r), after rotate, it is right child of py
// 	 */
// 	left_rotate(parent);
// 	sibling->left_size += (parent->left_size + 1);
// 	sibling->color = node_color::black;
// 	parent->rank -= 1;
// 	if(parent->right) {	  
// 	  // looks like node a must not null, but no harm to check
// 	  // then it must be black, change to red
// 	  parent->right->color = node_color::red;
// 	}
//       } else { 
// 	/** Rr0          |                              |
// 	 *      (parent) py[x+1]                        v [x+1]
// 	 *             // \                            / \
// 	 *   (sibling) v   y (deficient) ====>        a  py [x]
// 	 *            / \                               // \
// 	 *           a   b [x]                       [x]b   y
// 	 * left_size change: py_before = v_before + py_after + 1
// 	 * rank change: py(-1)
// 	 * color change: v(r=>b), b(b=>r), after rotate, its left child of py
// 	 */
// 	right_rotate(parent);
// 	parent->left_size -= (sibling->left_size + 1);
// 	parent->rank -= 1;
// 	sibling->color = node_color::black;
// 	if (parent->left) {
// 	  // looks like node b must not null, but no harm to check
// 	  // then it must be black, change to red
// 	  parent->left->color = node_color::red;
// 	}
//       }
//     }
//   }
// }

// template <typename TData>
// void red_black_tree<TData>::fixup_tree(rb_tree_node *node) {
//   auto gp = node->parent;
//   rb_tree_node *sibling = nullptr;
//   int rotateType = 0; // LL:0, LR:1, RL:2, RR:3
//   if (node->key < gp->key) {
//     // node is left child of gp
//     sibling = gp->right;
//     rotateType &= ~2;
//   } else {
//     sibling = gp->left;
//     rotateType |= 2;
//   }
//   rb_tree_node *p = nullptr;
//   if ((node->left != nullptr) && (node->left->color == node_color::red)) {
//     p = node->left;
//     rotateType &= ~1;
//   } else if ((node->right != nullptr) &&
//              (node->right->color == node_color::red)) {
//     p = node->right;
//     rotateType |= 1;
//   }
//   if ((sibling == nullptr) || (sibling->color == node_color::black)) {
//     // if sibling is null, it is an external node, take it as black,
//     // or sibling is not null and color is black
//     switch (rotateType) {
//     case 0:
//       /** LLb           |                          |
//        *           (gp) z                          y
//        *              // \                       // \\
//        *      (node)  y   d    =====>        (p) x    z
//        *            // \                        / \  / \
//        *        (p) x   c                      a   b c  d
//        *           / \
//        *          a   b
//        */
//       right_rotate(gp);
//       node->color = node_color::black;
//       gp->color = node_color::red;
//       break;
//     case 3:
//       /** RRb           |                          |
//        *           (gp) z                          y
//        *               / \\                      // \\
//        *              a   y (node)     =====>    z    x
//        *                 / \\                   / \  / \
//        *                b   x (p)              a   b c  d
//        *                   / \
//        *                  c   d
//        */
//       left_rotate(gp);
//       node->color = node_color::black;
//       gp->color = node_color::red;
//       break;
//     case 1:
//       /** LRb           |                 |             |
//        *           (gp) z                 z             x
//        *              // \              // \          // \\
//        *      (node)  y   d    =====>   x   d  ====>  y    z
//        *             / \\             // \           / \  / \
//        *            a   x (p)         y   c         a   b c  d
//        *               / \           / \
//        *              b   c         a   b
//        */
//       left_rotate(node);
//       right_rotate(gp);
//       gp->color = node_color::red;
//       p->color = node_color::black;
//       break;
//     case 2:
//       /** RLb           |                  |                  |
//        *           (gp) z                  z                  x
//        *               / \\               / \\              // \\
//        *              a   y (node) ====> a   x       ====>  z   y
//        *                 // \               / \\           / \  / \
//        *             (p) x   d             b    y         a   b c  d
//        *                / \                    / \
//        *               b   c                  c   d
//        */
//       right_rotate(node);
//       left_rotate(gp);
//       gp->color = node_color::red;
//       p->color = node_color::black;
//       break;
//     default:
//       break;
//     }
//   } else {
//     // sibling is nullptr or sibling's color is red;
//     if (sibling != nullptr) { // that leaves sibling's color must be red;
//       sibling->color = node_color::black;
//       node->color = node_color::black;
//       // update rank, this is the only situation where rank of nodes incresed.
//       gp->rank += 1;
//     }
//     // if gp is root, fixup termnate
//     // if gp is not root, mark it as red, check gp's parent(ggp),
//     // if ggp is not null and its color is red, continue fixup
//     if (gp->parent != nullptr) {
//       gp->color = node_color::red;
//       if ((gp->parent->parent != nullptr) &&
//           (gp->parent->color == node_color::red)) {
//         fixup_tree(gp->parent);
//       }
//     }
//   }
// }

} // namespace cop5536

#endif /* RED_BLACK_TREE_H_ */
