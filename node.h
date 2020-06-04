//
// Created by danie on 26/04/2020.
//

#ifndef DTS2_EX1_NODE_H
#define DTS2_EX1_NODE_H

/*
 * We used Node.h to implement also the vertices of the AVL tree and also the Elements of the double linked list of
 * different numbers of streams (BestHitsList).
 * */


static int max(int a, int b){
    if (a>b) return a;
    return b;
}



template <class K, class D>
class Node{
private:
    K key;
    D* data;
    Node<K,D>* parent;
    Node<K,D>* left;
    Node<K,D>* right;
    int height;

public:
    Node()= delete;
    Node(K key, D* data, Node* parent):key(key),data(data),parent(parent),left(nullptr),right(nullptr),height(1){};
    ~Node() = default;
    Node(const Node& node) = delete;
    Node& operator=(const Node& node) = delete;
    const K& getKey(){
        return this->key;
    }
    void setKey(K key){
        this->key = key;
    }
    D* getData(){
        return this->data;
    }
    void setData(D* data){
        this->data=data;
    }
    Node<K,D>* getParent(){
    return this->parent;
    }
    void setParent(Node<K,D>* node){
        this->parent=node;
    }
    Node<K,D>* getLeft(){
        return this->left;
    }
    void setLeft(Node<K,D>* node){
        this->left=node;
    }
    Node<K,D>* getRight(){
        return this->right;
    }
    void setRight(Node<K,D>* node){
        this->right=node;
    }
    int getHeight() const{
        return this->height;
    }
    void setHeight(int height){
        this->height = height;
    }
    bool isLeaf(){
        return  (this->right == nullptr && this->left == nullptr);
    }
    bool isRoot(){
        return this->parent== nullptr;
    }
    void calcHeight(){
        if(this->left== nullptr && this->right== nullptr)this->height = 1; //leaf
        else if(this->left== nullptr) this->height = this->right->height +1; // no left son
        else if(this->right== nullptr) this->height = this->left->height +1; // no right son
        else this->height= max(this->left->height,this->right->height) + 1; // max between sons
    }


    // Methods for implementing doubly linked list (BestHitsList)
    Node(K key, D* data,Node* prev,Node* next):key(key),data(data),parent(nullptr),left(prev),right(next),height(1){}
    Node(K key, D* data):key(key),data(data),parent(nullptr),left(nullptr),right(nullptr),height(1){}
    void setPrev(Node* prev){
        this->left=prev;
    }
    Node* getPrev(){
        return this->left;
    }
    void setNext(Node* next){
        this->right=next;
    }
    Node* getNext(){
        return this->right;
    }

    void removeNode(){

        // If I'm the only element
        if(this->getNext() == nullptr && this->getPrev() == nullptr){}
        else if(this->getPrev() == nullptr){
            this->getNext()->setPrev(nullptr);
        }

        // If I have previous
        else if (this->getNext() == nullptr){
            this->getPrev()->setNext(nullptr);
        }

        // If I have next
        else{
            this->getPrev()->setNext(this->getNext());
            this->getNext()->setPrev(this->getPrev());
        }
        delete this;
    }
};




#endif //DTS2_EX1_NODE_H
