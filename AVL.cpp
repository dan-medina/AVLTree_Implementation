#include "AVL.h"

#include <cassert>
#include <iostream>
#include <string>
#include <queue>
#include <algorithm>

#include "json.hpp"

using namespace std;


AVLNode::AVLNode(int key) :
        key_(key),
        parent_(std::weak_ptr<AVLNode>()),
        left_(nullptr),
        right_(nullptr) {}

AVLNode::AVLNode(int key, std::weak_ptr<AVLNode> parent) :
        key_(key),
        parent_(parent),
        left_(nullptr),
        right_(nullptr) {}

bool AVLNode::isLeaf() const {
    return left_ == nullptr && right_ == nullptr;
}

bool AVLNode::HasLeftChild() const {
    return left_ != nullptr;
}

bool AVLNode::HasRightChild() const {
    return right_ != nullptr;
}

AVL::AVL() : root_(nullptr), size_(0) {}

int AVL::getHeight(std::shared_ptr<AVLNode> currentNode) const {
    if (currentNode == nullptr) {
        return -1;
    } else if (currentNode->left_ == nullptr && currentNode->right_ == nullptr) {
        return 0;
    } else if (currentNode->left_ == nullptr) {
        return 1 + getHeight(currentNode->right_);
    } else if (currentNode->right_ == nullptr) {
        return 1 + getHeight(currentNode->left_);
    } else {
        return 1 + max(getHeight(currentNode->left_), getHeight(currentNode->right_));
    }
}

int AVL::getBalanceFactor (std::shared_ptr<AVLNode> currentNode) const {
    return getHeight(currentNode->right_) - getHeight(currentNode->left_);
}

void AVL::rightrightRotate(std::shared_ptr<AVLNode> currentNode) {
    std::shared_ptr<AVLNode> parent = currentNode->parent_.lock();
    if (parent == root_) {
        root_ = currentNode;
        currentNode->parent_.lock() = nullptr;
    } else {
        currentNode->parent_ = parent->parent_;
        if (currentNode->key_ < parent->parent_.lock()->key_) {
            parent->parent_.lock()->left_ = currentNode;
        } else {
            parent->parent_.lock()->right_ = currentNode;
        }
    }

    parent->left_ = currentNode->right_;

    if (currentNode->right_ != nullptr) {
        std::weak_ptr<AVLNode> rightParent = parent;
        currentNode->right_->parent_ = rightParent;
    }

    currentNode->right_ = parent;
    parent->parent_ = currentNode;
}

void AVL::leftleftRotate(std::shared_ptr<AVLNode> currentNode) {
    std::shared_ptr<AVLNode> parent = currentNode->parent_.lock();
    if (parent == root_) {
        root_ = currentNode;
        currentNode->parent_.lock() = nullptr;
    } else {
        currentNode->parent_ = parent->parent_;
        if (currentNode->key_ < parent->parent_.lock()->key_) {
            parent->parent_.lock()->left_ = currentNode;
        } else {
            parent->parent_.lock()->right_ = currentNode;
        }
    }

    parent->right_ = currentNode->left_;

    if (currentNode->left_ != nullptr) {
        std::weak_ptr<AVLNode> leftParent = parent;
        currentNode->left_->parent_ = leftParent;
    }

    currentNode->left_ = parent;
    parent->parent_ = currentNode;
}

void AVL::leftrightRotate (std::shared_ptr<AVLNode> currentNode) {

    currentNode->right_->parent_ = currentNode->parent_;

    currentNode->parent_.lock()->left_ = currentNode->right_;

    currentNode->parent_ = currentNode->right_;

    currentNode->right_ = currentNode->parent_.lock()->left_;

    if (currentNode->parent_.lock()->left_ != nullptr) {

        currentNode->parent_.lock()->left_->parent_ = currentNode;
    }

    currentNode->parent_.lock()->left_ = currentNode;

    rightrightRotate(currentNode->parent_.lock());

}

void AVL::rightleftRotate(std::shared_ptr<AVLNode> currentNode) {
    currentNode->left_->parent_ = currentNode->parent_;

    currentNode->parent_.lock()->right_ = currentNode->left_;

    currentNode->parent_ = currentNode->left_;

    currentNode->left_ = currentNode->parent_.lock()->right_;

    if (currentNode->parent_.lock()->right_ != nullptr) {

        currentNode->parent_.lock()->right_->parent_ = currentNode;
    }

    currentNode->parent_.lock()->right_ = currentNode;

    leftleftRotate(currentNode->parent_.lock());

}

void AVL::Insert(int key) {
    if (root_ == nullptr) {
        root_ = std::make_shared<AVLNode>(key);
        size_++;
        return;
    }

    std::shared_ptr<AVLNode> currentNode = root_, lastNode = nullptr;
    while (currentNode != nullptr) {
        lastNode = currentNode;
        currentNode = (key < currentNode->key_) ?
                      currentNode->left_ : currentNode->right_;
    }
    if (key < lastNode->key_) {
        lastNode->left_ = std::make_shared<AVLNode>(key, lastNode);
    } else {
        lastNode->right_ = std::make_shared<AVLNode>(key, lastNode);
    }
    size_++;

    if (getHeight(root_) > 1) {
        while(1) {
            int grandparentBF = getBalanceFactor(lastNode->parent_.lock());
            int parentBF = getBalanceFactor(lastNode);

            if (grandparentBF < -1 && parentBF < 0) {
                rightrightRotate(lastNode);
                return;
            } else if (grandparentBF > 1 && parentBF > 0) {
                leftleftRotate(lastNode);
                return;
            } else if (grandparentBF < -1 && parentBF > 0) {
                leftrightRotate(lastNode);
                return;
            } else if (grandparentBF > 1 && parentBF < 0) {
                rightleftRotate(lastNode);
                return;
            }

            if (lastNode->parent_.lock() == root_) {
                break;
            }

            lastNode = lastNode->parent_.lock();
        }


    }

}

size_t AVL::size() const {
    return size_;
}

bool AVL::empty() const {
    return size_ == 0;
}

nlohmann::json AVL::JSON() const {
    nlohmann::json result;
    std::queue< std::shared_ptr<AVLNode> > nodes;
    if (root_ != nullptr) {
        result["height"] = getHeight(root_);
        result["root"] = root_->key_;
        nodes.push(root_);
        while (!nodes.empty()) {
            auto v = nodes.front();
            nodes.pop();
            std::string key = std::to_string(v->key_);
            if (v->left_ != nullptr) {
                result[key]["left"] = v->left_->key_;
                nodes.push(v->left_);
            }
            if (v->right_ != nullptr) {
                result[key]["right"] = v->right_->key_;
                nodes.push(v->right_);
            }
            if (v->parent_.lock() != nullptr && v != root_) {
                result[key]["parent"] = v->parent_.lock()->key_;
            } else {
                result[key]["root"] = true;
            }
            result[key]["balance factor"] = getBalanceFactor(v);
            result[key]["height"] = getHeight(v);
        }
    }
    result["size"] = size_;
    return result;
}
