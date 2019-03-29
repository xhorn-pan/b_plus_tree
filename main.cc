#include <iostream>
#include "red_black_tree.h"

using namespace cop5536;
int main() {
    red_black_tree<double> rbt = new red_black_tree<double>();
    std::cout << "insert 1, ";
    rbt->insert_key_value ( 1, 11.1 );
    rbt->print_in_order();
    std::cout << "2, ";
    rbt->insert_key_value ( 2, 119.1 );
    rbt->print_in_order();
    std::cout << "3, ";
    rbt->insert_key_value ( 3, 19.1 );
    rbt->print_in_order();
    std::cout<< "4,";
    rbt->insert_key_value ( 4, 19.2 );
    rbt->print_in_order();
    std::cout<< "5,";
    rbt->insert_key_value ( 5, 19.4 );
    rbt->print_in_order();
    std::cout<< "6,";
    rbt->insert_key_value ( 6, 1.2 );
    rbt->print_in_order();
    std::cout<< "7,";
    rbt->insert_key_value ( 7, 1.2 );
    rbt->print_in_order();
    std::cout<< "8,";
    rbt->insert_key_value ( 8, 1.2 );
    rbt->print_in_order();
    std::cout<< "9,";
    rbt->insert_key_value ( 9, 1.2 );
    rbt->print_in_order();
    std::cout<< "10,";
    rbt->insert_key_value ( 10,1.2 );
    rbt->print_in_order();
    std::cout<< "20,";
    rbt->insert_key_value ( 20,1.2 );
    rbt->print_in_order();
    std::cout<< "19,";
    rbt->insert_key_value ( 19,1.2 );
    rbt->print_in_order();
    std::cout<< "18,";
    rbt->insert_key_value ( 18,1.2 );
    rbt->print_in_order();
    std::cout<< "17,";
    rbt->insert_key_value ( 17,1.2 );
    rbt->print_in_order();
    std::cout<< "16,";
    rbt->insert_key_value ( 16,1.2 );
    rbt->print_in_order();
    std::cout<< "15,";
    rbt->insert_key_value ( 15,1.2 );
    rbt->print_in_order();
    std::cout<< "14,";
    rbt->insert_key_value ( 14,1.2 );
    rbt->print_in_order();
    std::cout<< "13,";
    rbt->insert_key_value ( 13,1.2 );
    rbt->print_in_order();
    std::cout<< "12,";
    rbt->insert_key_value ( 12,1.2 );
    rbt->print_in_order();
    std::cout<< "11,";
    rbt->insert_key_value ( 11,1.2 );
    std::cout<< std::endl;
    rbt->print_in_order();
    return 0;
}
