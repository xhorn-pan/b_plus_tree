#include <iostream>
#include <fstream>
#include <string>
#include <regex>
#include "bp_tree.h"

using namespace cop5536;

int main(int argc, char *argv[])
{
  if(argc != 2){
    std::cout << "Invalid param, Usage: ./bplustree <input_filename>" << std::endl;
    exit(-1);
  }

  std::ifstream fi(argv[1]);
  if(not fi) {
    std::cout << "file not found, check " << argv[1] << std::endl;
    exit(-1);
  }
  std::string line;
  // int m; //order
  // // get B+ tree order
  // getline(fi, line);
  
  // std::cout << "init tree" << m  << std::endl;
  bp_tree* b;// create B+ tree
  std::regex e_cmd("^\\s*([Ii]ntitialize|[Ii]nsert|[Dd]elete|[Ss]earch)\\((.*)\\)\\s*$");
  //std::regex s_cmd(","); //search for ","
  // std::smatch m_e;
  std::smatch m_s;
  while(std::getline(fi, line)) {
    //std::cout << line << std::endl;
    if(std::regex_match(line, m_s, e_cmd)){
      const std::string cmd = m_s[1].str();
      const std::string kv = m_s[2].str();
      if(cmd == "Intitialize") {
        int m;
        try {
          m = std::stoi(kv);
        } catch (...) {
          std::cout << "can not parse B+ tree order." << kv << std::endl;
          exit(-1);
        }
        if(m<3) {
          std::cout << "B+ tree must have a order great than 2, you give " << m << std::endl;
          exit(-1);
        }
        b = new bp_tree(m);
        //b->open_output();
      } else if (cmd == "Insert") {
        std::size_t pos = kv.find(",");
        int key = std::atoi(kv.substr(0, pos).c_str());
        float value = std::atof(kv.substr(pos+1, kv.size()-1).c_str());
        b->insert(key, value);
        // TODO delete this line
        // b->print();
      } else if (cmd == "Delete"){
        int key = atof(kv.c_str());
        //std::cout << "deleting key: " << key << std::endl;
        // b->print();
        // b->remove(key);
        // b->print();
      } else if (cmd == "Search") {
        std::size_t pos = kv.find(",");
        if (pos != std::string::npos) {
          int key_low = atof(kv.substr(0, pos).c_str());
          int key_high = atof(kv.substr(pos+1, kv.size()-1).c_str());
          if(key_low < key_high) {
            b->search(key_low, key_high);
          } else {
            b->search(key_low, key_high);
          }
        } else {
          int key = atof(kv.c_str());
          b->search(key);
        }
      }
      // std::cout << m_cmd[1] << " " ;
      // std::cout << m_cmd[2] << std::endl;
    }
  }
  // b->print();
  delete b;
  return 0;
}

