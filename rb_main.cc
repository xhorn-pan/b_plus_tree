#include "rb_tree.h"
#include <iostream>
#include <cstdlib> /* srand, rand */
#include <ctime>   /* time */
//#include <vector>

using namespace cop5536;

int main(int argc, char *argv[]) {
  srand(time(0));
  //std::vector<int> keys;
  rb_tree<int, int> *bst1 = new rb_tree<int, int>();
  for (int i = 0; i < rand() % 250; ++i) {
    int r = rand() % 200 - 200;
    //keys.push_back(r);
    //std::cout << "inserting: " << r << std::endl;
    bst1->insert(r, r+1);
  }
  bst1->print_tree();
  rb_tree<int, int> *bst2 = new rb_tree<int, int>();
  for (int i = 0; i < rand() % 50; ++i) {
    int r = rand() % 200 + 100;
    //keys.push_back(r);
    //std::cout << "inserting: " << r << std::endl;
    bst2->insert(r, r+2);
  }
  bst2->print_tree();
  rb_tree_node<int, int> *p = new rb_tree_node<int, int>(50, 55); 
  bst1->join_(p, bst2);

  bst1->print_tree();

  bst2 = new rb_tree<int, int>();

  //  rb_tree_node<int>* md = bst1->nth((short int)4);
  
  
  std::cout << bst1->split_((short int) 4, bst2);

  bst1->print_tree();

  bst2->print_tree();
  
  
  delete bst2;
  delete bst1;
}
  
