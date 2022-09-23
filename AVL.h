#include <memory>
#include <string>

#include "json.hpp"

class AVL;

class AVLNode{
public:
    AVLNode(int key);
    AVLNode(int key, std::weak_ptr<AVLNode> parent);
    bool isLeaf() const;
    bool IsMissingChild() const;
    bool HasLeftChild() const;
    bool HasRightChild() const;

private:
    int key_;
    std::weak_ptr<AVLNode> parent_;
    std::shared_ptr<AVLNode> left_;
    std::shared_ptr<AVLNode> right_;

    friend AVL;
};

class AVL {
public:
    AVL();

    int getHeight(std::shared_ptr<AVLNode> currentNode) const;
    int getBalanceFactor(std::shared_ptr<AVLNode> currentNode) const;
    void rightrightRotate(std::shared_ptr<AVLNode> currentNode);
    void leftleftRotate(std::shared_ptr<AVLNode> currentNode);
    void leftrightRotate(std::shared_ptr<AVLNode> currentNode);
    void rightleftRotate(std::shared_ptr<AVLNode> currentNode);
    void Insert(int key);
    nlohmann::json JSON() const;
    size_t size() const;
    bool empty() const;
private:
    std::shared_ptr<AVLNode> root_;
    size_t size_;
};