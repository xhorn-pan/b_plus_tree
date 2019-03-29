#ifndef B_PLUS_TREE_H_
#define B_PLUS_TREE_H_

#include <iostream>
#include <vector>
#include <stack>

namespace cop5536 {
  class BPlusTree {
  public:
    class Node;
    struct NodeData {
      int key;
    };
    
    struct InternalNodeData : NodeData {
      Node* child;
    };

    struct LeafNodeData : NodeData {
      float value;
    };

    class Node {
    public:
      Node();
      virtual bool IsLeaf() = 0;
      virtual Node* Split(int* keyOfNewNode) = 0;
      virtual void Insert(const int key, float value);
      virtual void Insert(const int key, Node* newNode);
    };
    
    class InternalNode : public Node {
    public:
      InternalNode();
      void Insert(const int key, Node* newNode) override;
      bool IsLeaf() override {return false;};
      Node* Split(int* keyOfNewNode) override;

    private:
      unsigned degree;
      Node* leftestChild;
      std::vector<Node*> children;
    };

    class LeafNode : public Node {
    public:
      LeafNode();
      void Insert(const int key, float value) override;
      bool IsLeaf() override {return true;};
      Node* Split(int* keyOfNewNode) override;
    private:
      LeafNode* next;
      LeafNode* prev;
      std::vector<LeafNode*> data;
    };
    
  BPlusTree(unsigned m) : order(m) {};
    void Insert(const int key, float value);
    void Search(const int key);
    void Search(const int low, const int high);
    void Delete(const int key);
    ~BPlusTree();
  private:
    void SearchPath(Node* node, const int key, std::stack<Node*>* path);
    unsigned order;
    Node* root;
  };
}


#endif /* B_PLUS_TREE_H_ */
