#include <iostream>
#include "bp_tree.h"
#include <cstdlib> /* srand, rand */
#include <ctime>   /* time */

using namespace cop5536;

int main(int argc, char *argv[]) {
  srand(time(0));
  bp_tree* bpt = new bp_tree(4);
  for (int i=0; i < 20; ++i) {
    int r = rand() %1000 - 500;
    bpt->insert(r, r*1.25);
  }
  // bpt->insert(1, 1.1);
  // bpt->insert(2, 1.1);

  // bpt->insert(3, 1.1);
  // bpt->insert(4, 1.1);

  // bpt->print();
  // bpt->insert(5, 1.1);
  
  // bpt->print();
  // bpt->insert(6, 1.1);

  // bpt->print();
  // bpt->insert(7, 1.1);
  
  // bpt->print();
  // bpt->insert(8, 1.1);
  
  
  bpt->print();
  delete bpt;
  return 0;
}
