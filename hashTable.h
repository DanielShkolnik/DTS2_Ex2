//
// Created by danie on 04/06/2020.
//

#ifndef DTS2_EX2_HASHTABLE_H
#define DTS2_EX2_HASHTABLE_H

#include "avl.h"
#include "artist.h"
#include <iostream>

class HashTable{
private:
    Avl<int,Artist>** arr;
    int numOfUsedCells;
    int arrSize;

    void initArr(Avl<int,Artist>** array, int size);
    void increaseSize();
    void decreaseSize();
    void deleteArr();
    static int hash(int artistID, int arrSize);
    friend class ArtistPredicate;

public:
    explicit HashTable();
    ~HashTable();
    HashTable(const HashTable&) = delete;
    HashTable& operator=(const HashTable&) = delete;
    void addArtist(int artistID);
    void removeArtist(int artistID);
    Artist* findArtist(int artistID);

};

void HashTable::initArr(Avl<int,Artist>** array, int size){
    for(int i=0; i<size; i++){
        array[i] = new Avl<int,Artist>;
    }
}

class ArtistPredicate{
private:
    Avl<int,Artist>** table;
    int size;
public:
    void operator()(Node<int,Artist>* artistNode){
        int artistID = artistNode->getData()->getArtistID();
        int newIndex = HashTable::hash(artistID,this->size);
        this->table[newIndex]->insert(artistID,artistNode->getData());
    }
    explicit ArtistPredicate(Avl<int,Artist>** table, int size) : table(table), size(size){};
    ArtistPredicate(const ArtistPredicate& a) = delete;
};

void HashTable::increaseSize(){
    int newSize = (this->arrSize)*2;
    Avl<int,Artist>** newTable = new Avl<int,Artist>*[newSize]();
    initArr(newTable,newSize);
    /* For each artist in the Hash table, iterate over it's songs tree, and delete all the song from the tree nodes */
    for(int i = 0; i< this->arrSize; i++){
        // get the root of artist tree of the current cell of the table
        Node<int,Artist>* artistNode = this->arr[i]->getRoot();

        // do postorder to free data (song) for each artist
        ArtistPredicate artistPredicate(newTable,newSize);

        postorder<int,Artist,ArtistPredicate>(artistNode,artistPredicate);
    }
    this->deleteArr();
    this->arrSize = newSize;
    this->arr = newTable;
};

void HashTable::decreaseSize(){
    int prevSize = this->arrSize;
    int newSize = this->arrSize/2;
    Avl<int,Artist>** newArr = new Avl<int,Artist>*[newSize];
    initArr(newArr,newSize);
    for(int i=0; i<prevSize; i++){
        Node<int,Artist>* artistNode = this->arr[i]->getRoot();

        ArtistPredicate artistPredicate(newArr,newSize);

        postorder<int,Artist,ArtistPredicate>(artistNode,artistPredicate);
    }
    this->deleteArr();
    this->arrSize = newSize;
    this->arr = newArr;
};

int HashTable::hash(int artistID, int arrSize){
    return artistID%arrSize;
}

HashTable::~HashTable(){
    this->deleteArr();
}

void HashTable::removeArtist(int artistID){
    if(this->numOfUsedCells == this->arrSize/4 && this->arrSize > MAGICSIZE){
        this->decreaseSize();
    }
    int index = this->hash(artistID,this->arrSize);
    this->arr[index]->deleteVertice(artistID);
}

Artist* HashTable::findArtist(int artistID){
    int index = this->hash(artistID,this->arrSize);
    return this->arr[index]->find(artistID)->getData();
}

#endif //DTS2_EX2_HASHTABLE_H
