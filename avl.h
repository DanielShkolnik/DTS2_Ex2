//
// Created by danie on 26/04/2020.
//

#ifndef DTS2_EX1_AVL_H
#define DTS2_EX1_AVL_H

#include "node.h"
#include <assert.h>

template <class K, class D>
class Avl{
private:
    Node<K,D>* root;
    void fixBalanceFactor(Node<K,D>* childVertice);

    // Helper function. After rotations fix root if needed
    void updateRoot(Node<K,D>* node);

    // After rotation need to update pointer's to children\parents
    void fixRelations(Node<K,D>* parent, Node<K,D>* son);

    /*
     * All rotations are based on the algorithms we've seen in tutorial number 05. The names are taken from there.
     * */
    void rotateLL(Node<K,D>* B);
    void rotateLR(Node<K,D>* C);
    void rotateRL(Node<K,D>* C);
    void rotateRR(Node<K,D>* B);
    int getBF(Node<K,D>* node);

    // Find nearest Available place to insert new node into tree. if it's the node itself - throws exception KeyExists.
    Node<K,D>* getNextAvailable(K key);
    void removeFromParent(Node<K,D>* node);
    bool isLeftSon(Node<K,D>* node);


public:
    Avl():root(nullptr){}
    ~Avl();
    Avl(const Avl& avl)= delete;
    Avl& operator=(const Avl& avl)= delete;
    void insert(const K& key, D* data);
    void deleteVertice(const K& key);
    Node<K,D>* find(const K& key);
    Node<K,D>* getRoot(){
        return this->root;
    }
    bool isEmpty();

    Node<K,D>* getMaxNode();

    class Error{};
    class KeyExists{};
    class KeyNotFound{};


};


template <class K, class D>
void Avl<K,D>::deleteVertice(const K& key){

    Node<K,D>* vertice = nullptr;
    try{
        vertice = this->find(key);
    }
    catch(const Avl<K,D>::KeyNotFound&){
        throw Avl<K,D>::KeyNotFound();
    }

    assert(vertice != nullptr);

    // only one element
    if(vertice->isLeaf() && vertice->isRoot()){
        delete vertice;
        this->root = nullptr;
        return;
    }

    //leaf and not root => parent exists
    else if(vertice->isLeaf()){
        removeFromParent(vertice);
        this->fixBalanceFactor(vertice->getParent()); // fix balance from leaf parent
        this->updateRoot(vertice->getParent());
    }

    //no left son but not leaf => right son and parent exists
    else if(vertice->getLeft()== nullptr){
        if(vertice->getParent() != nullptr) fixRelations(vertice->getParent(),vertice->getRight());
        else vertice->getRight()->setParent(nullptr);
        fixBalanceFactor(vertice->getRight());
        this->updateRoot(vertice->getRight());
    }

    //find the left->right->right...son
    else{
        Node<K,D>* current = vertice->getLeft();
        while (current->getRight()!= nullptr){
            current = current->getRight();
        }
        // path is only one to the left
        if(current == vertice->getLeft()){
            if (vertice->getParent() != nullptr) fixRelations(vertice->getParent(),current);
            else current->setParent(nullptr); // if the vertice is the root.
            if(vertice->getRight() != nullptr) fixRelations(current,vertice->getRight());
            else current->setRight(nullptr);
            this->fixBalanceFactor(current);
            this->updateRoot(current);
        }

        else{
            Node<K,D>* changedFrom = current->getParent(); // save parent of leaf to fix balance from
            if(current->getLeft() != nullptr) fixRelations(current->getParent(),current->getLeft());
            else current->getParent()->setRight(nullptr);
            if(vertice->getParent() != nullptr) fixRelations(vertice->getParent(),current);
            else current->setParent(nullptr); // if the nearest is the root
            if(vertice->getRight() != nullptr) fixRelations(current,vertice->getRight());
            else current->setRight(nullptr);
            fixRelations(current,vertice->getLeft());
            this->fixBalanceFactor(changedFrom);
            this->updateRoot(current);
        }

    }
    delete vertice;
}

// Gets the key and returns element with the nearest existing key
template <class K, class D>
Node<K,D>* Avl<K,D>::getNextAvailable(K key){
    // Avl is empty
    if(this->root == nullptr) return nullptr;

    // Create iterator
    Node<K,D>* iter = this->root;
    Node<K,D>* previous = this->root;

    while(iter){
        previous = iter;

        // I'm the nearest to myself
        if(key == iter->getKey()){
           throw Avl<K,D>::KeyExists();
        }

        else if(key > iter->getKey()){
            iter = iter->getRight();
        }

        else if(key < iter->getKey()){
            iter = iter->getLeft();
        }

    }

    return previous;
}

template <class K, class D>
void Avl<K,D>::insert(const K& key, D* data){

    Node<K,D>* nearest;

    // find suitable place for insertion
    try {
        nearest = this->getNextAvailable(key);
    } catch(const Avl<K,D>::KeyExists&) {
        throw Avl<K, D>::KeyExists();
    }

    Node<K,D>* newNode = new Node<K,D>(key,data,nearest);
    if(nearest == nullptr){
        this->root = newNode;
        return;
    }
    if(key > nearest->getKey()) nearest->setRight(newNode);
    else if (key < nearest->getKey()) nearest->setLeft(newNode);
    this->fixBalanceFactor(newNode);
    this->updateRoot(newNode);
}

template <class K, class D>
void Avl<K,D>::fixBalanceFactor(Node<K,D>* vertice){
    if(vertice->isLeaf()){
        vertice->calcHeight();
        vertice->updateRank();
        vertice = vertice->getParent();
        updateRoot(vertice);
    }
    if(vertice == nullptr) return;

    while(vertice){

        // update vertice height
        vertice->calcHeight();

        // update vertice rank
        vertice->updateRank();

        // get vertice BF
        int currentBF = this->getBF(vertice);

        //LL
        if(currentBF == 2 && this->getBF(vertice->getLeft())>=0){
            this->rotateLL(vertice);
        }
        //LR
        else if(currentBF == 2 && this->getBF(vertice->getLeft())==-1){
            this->rotateLR(vertice);
        }
        //RL
        else if(currentBF==-2 && this->getBF(vertice->getRight())==1){
            this->rotateRL(vertice);
        }
        //RR
        else if(currentBF==-2 && this->getBF(vertice->getRight())<=0){
            this->rotateRR(vertice);
        }

        // go up in insertion path
        vertice = vertice->getParent();
    }

}

template <class K, class D>
void Avl<K,D>::rotateLL(Node<K,D>* B){
    if(B == nullptr) return;
    Node<K,D>* BParent = B->getParent();
    Node<K,D>* A = B->getLeft();
    Node<K,D>* ARight = A->getRight();
    A->setRight(B);
    B->setLeft(ARight);
    if(BParent != nullptr) fixRelations(BParent, A);
    else A->setParent(nullptr);
    fixRelations(A,B);
    if(ARight != nullptr) fixRelations(B,ARight);
    B->calcHeight();
    A->calcHeight();
    B->updateRank();
    A->updateRank();
}

template <class K, class D>
void Avl<K,D>::rotateLR(Node<K,D>* nodeC){
    Node<K,D>* nodeB = nodeC->getLeft();
    Node<K,D>* nodeA = nodeB->getRight();
    Node<K,D>* nodeRightA = nodeA->getRight();
    Node<K,D>* nodeLeftA = nodeA->getLeft();

    // left rotation
    nodeB->setRight(nodeLeftA);
    nodeA->setLeft(nodeB);

    // right rotation
    nodeC->setLeft(nodeRightA);
    nodeA->setRight(nodeC);

    // fix parent-child relations for A,B,C
    if(nodeC->getParent() == nullptr) nodeA->setParent(nullptr);
    else fixRelations(nodeC->getParent(),nodeA);
    fixRelations(nodeA,nodeC);
    fixRelations(nodeA,nodeB);

    if(nodeLeftA != nullptr) fixRelations(nodeB, nodeLeftA);
    if(nodeRightA != nullptr) fixRelations(nodeC, nodeRightA);

    // Calc new height for vertices who's height have changed
    nodeB->calcHeight();
    nodeC->calcHeight();
    nodeA->calcHeight();
    nodeB->updateRank();
    nodeC->updateRank();
    nodeA->updateRank();
}

template <class K, class D>
void Avl<K,D>::removeFromParent(Node<K,D>* node){
    if(node->isRoot()) return;
    if(isLeftSon(node)) node->getParent()->setLeft(nullptr);
    else node->getParent()->setRight(nullptr);
}

template <class K, class D>
bool Avl<K,D>::isLeftSon(Node<K,D>* node){
    if(node->getParent()->getLeft()== nullptr) return false;
    return node->getParent()->getLeft()->getKey()==node->getKey();
}

template <class K, class D>
void Avl<K,D>::updateRoot(Node<K,D>* node){
    if(node== nullptr) return;
    while(node->getParent() != nullptr){
        node = node->getParent();
    }
    this->root = node;
}

template <class K, class D>
void Avl<K,D>::fixRelations(Node<K,D>* parent, Node<K,D>* son){
    if (!parent || !son) throw Avl<K,D>::Error();
    if (parent->getKey()>son->getKey()) parent->setLeft(son);
    else parent->setRight(son);
    son->setParent(parent);
}

template <class K, class D>
Node<K,D>* Avl<K,D>::find(const K& key){
    if(this->root == nullptr) throw Avl<K,D>::KeyNotFound(); // avl is empty->
    Node<K,D>* currentNode = this->root;
    while (currentNode){
        if(key > currentNode->getKey()){
            currentNode = currentNode->getRight();
        } else if(key == currentNode->getKey()){
            return currentNode;
        } else{
            currentNode = currentNode->getLeft();
        }
    }
    throw Avl<K,D>::KeyNotFound();
}

template <class K, class D>
int Avl<K,D>::getBF(Node<K,D>* node){
    if(node->getLeft()== nullptr && node->getRight()== nullptr) return 0; // leaf
    else if(node->getRight()==nullptr) return node->getLeft()->getHeight(); // no right son
    else if(node->getLeft()==nullptr) return -(node->getRight()->getHeight()); // // no left son
    return node->getLeft()->getHeight()-node->getRight()->getHeight();
}

template <class K, class D>
void Avl<K,D>::rotateRR(Node<K,D>* B){
    if(B == nullptr) return;
    Node<K,D>* BParent=B->getParent();
    Node<K,D>* A=B->getRight();
    Node<K,D>* ALeft=A->getLeft();
    A->setLeft(B);
    B->setRight(ALeft);
    if (BParent!=nullptr)fixRelations(BParent,A);
    else A->setParent(nullptr);
    fixRelations(A,B);
    if (ALeft!= nullptr) fixRelations(B,ALeft);
    B->calcHeight();
    A->calcHeight();
    B->updateRank();
    A->updateRank();
}

template <class K, class D>
void Avl<K,D>::rotateRL(Node<K,D>* C){
    if(C == nullptr) return;

    Node<K,D>* CParent=C->getParent();
    Node<K,D>* B=C->getRight();
    Node<K,D>* A=B->getLeft();
    Node<K,D>* ALeft=A->getLeft();
    Node<K,D>* ARight=A->getRight();
    A->setLeft(C);
    C->setRight(ALeft);
    A->setRight(B);
    B->setLeft(ARight);
    if(CParent!= nullptr)fixRelations(CParent,A);
    else A->setParent(nullptr);
    if(ALeft!= nullptr)fixRelations(C,ALeft);
    fixRelations(A,B);
    fixRelations(A,C);
    if(ARight!= nullptr)fixRelations(B,ARight);
    C->calcHeight();
    B->calcHeight();
    A->calcHeight();
    C->updateRank();
    B->updateRank();
    A->updateRank();
}

template <class K, class D, class P>
void inorder(Node<K,D>* node, P& predicate){
    if (node== nullptr) return;
    inorder(node->getLeft(),predicate);
    predicate(node);
    inorder(node->getRight(),predicate);
}

template <class K, class D, class P>
void preorder(Node<K,D>* node, P& predicate){
    if (node== nullptr) return;
    predicate(node);
    preorder(node->getLeft(),predicate);
    preorder(node->getRight(),predicate);
}

template <class K, class D, class P>
void postorder(Node<K,D>* node, P& predicate){
    if (node== nullptr) return;
    postorder(node->getLeft(),predicate);
    postorder(node->getRight(),predicate);
    predicate(node);
}

template <class K, class D>
// inner destroy node without rolls (predicate for destructor)
void destroy(Node<K,D>* node){
    node->setParent(nullptr);
    node->setRight(nullptr);
    node->setLeft(nullptr);
    delete node;
}


template <class K, class D>
Avl<K,D>::~Avl(){
    postorder<K,D,void (Node<K,D>*)>(this->root,destroy);
}

template <class K, class D>
bool Avl<K,D>::isEmpty(){
    return this->root == nullptr;
}

template <class K, class D>
Node<K,D>* Avl<K,D>::getMaxNode(){
    Node<K,D>* current = this->getRoot();
    Node<K,D>* prev = current;
    while(current != nullptr){
        prev = current;
        current = current->getRight();
    }
    return prev;
}


#endif //DTS2_EX1_AVL_H
