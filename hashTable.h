//
// Created by danie on 04/06/2020.
//

#ifndef DTS2_EX2_HASHTABLE_H
#define DTS2_EX2_HASHTABLE_H

#include "avl.h"
#include "artist.h"
#include <iostream>

#define MAGIC_SIZE 7

/*
 * Hash Table DS implementation. Each cell at the dynamic hash table holds tree of artist's.
 * Each Node at the artist's tree hold tree of songs which belong to the artist.
 */
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
    void addArtist(Artist* artist);
    void removeArtist(int artistID);
    Artist* findArtist(int artistID);
};

/*
 * Destroy all cells of the hash table
 */
void HashTable::deleteArr(){
    for (int i=0; i<this->arrSize;i++){
        delete arr[i];
    }
}

/*
 * Initiate all cells of the hash table.
 */
void HashTable::initArr(Avl<int,Artist>** array, int size){
    for(int i=0; i<size; i++){
        array[i] = new Avl<int,Artist>;
    }
}

/*
 * Function used for postorder traversal when copying artists from one hash table to new one.
 * Used when there's need to increase\decrease table's size.
 */
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

/*
 * When Hash table is full - double it's size. in this manner we ensure the load factor is always O(1)
 */
void HashTable::increaseSize(){

    /* Allocate new table */
    int newSize = (this->arrSize) * 2;
    Avl<int, Artist> **newTable = new Avl<int, Artist> *[newSize]();
    initArr(newTable, newSize);

    /* For each cell in the old Hash table, copy the artist tree to the new Hash table */
    for (int i = 0; i < this->arrSize; i++) {
        // get the root of artist tree of the current cell of the table
        Node<int,Artist>* artistNode = this->arr[i]->getRoot();

        // predicate instantiation
        ArtistPredicate artistPredicate(newTable, newSize);

        // do postorder to copy all nodes of the artist tree
        postorder<int, Artist, ArtistPredicate>(artistNode, artistPredicate);
    }

    /* Delete old table */
    this->deleteArr();

    /* Update new table parameters */
    this->arrSize = newSize;
    this->arr = newTable;
};

void HashTable::decreaseSize(){
    /* Allocate new table */
    int prevSize = this->arrSize;
    int newSize = this->arrSize/2;
    Avl<int,Artist>** newArr = new Avl<int,Artist>*[newSize];
    initArr(newArr,newSize);

    /* For each cell in the old Hash table, copy the artist tree to the new Hash table */
    for(int i=0; i<prevSize; i++){
        // get the root of artist tree of the current cell of the table
        Node<int,Artist>* artistNode = this->arr[i]->getRoot();

        // predicate instantiation
        ArtistPredicate artistPredicate(newArr,newSize);

        // do postorder to copy all nodes of the artist tree
        postorder<int,Artist,ArtistPredicate>(artistNode,artistPredicate);
    }

    /* Delete old table */
    this->deleteArr();

    /* Update new table parameters */
    this->arrSize = newSize;
    this->arr = newArr;
};

int HashTable::hash(int artistID, int arrSize){
    return artistID%arrSize;
}

HashTable::HashTable(){
    this->arr = new Avl<int,Artist>*[MAGIC_SIZE]();
    initArr(this->arr,MAGIC_SIZE);
    this->numOfUsedCells = 0;
    this->arrSize = MAGIC_SIZE;
};

HashTable::~HashTable(){
    this->deleteArr();
}

/*
 * Each cell of the hash table contains AVL tree which holds nodes with artist as data.
 * Insert new artist into the tree at the adequate index of the hash table.
 */
void HashTable::addArtist(Artist* artist){
    // using hash table find adequate index of the artist
    int index = HashTable::hash(artist->getArtistID(),this->arrSize);

    // remove artist from the tree at the appropriate cell at the table
    this->arr[index]->insert(artist->getArtistID(),artist);

    // increase capacitance
    this->numOfUsedCells++;

    // In case the table is full - double it's size
    if(this->numOfUsedCells == this->arrSize){
        increaseSize();
    }
};

/*
 * Each cell of the hash table contains AVL tree which holds nodes with artist as data.
 * Remove artist from the tree at the adequate index of the hash table.
 */
void HashTable::removeArtist(int artistID){
    if(this->numOfUsedCells == this->arrSize/4 && this->arrSize > MAGIC_SIZE){
    /* Check if only quarter of the table is at use. If so, decrease table size to half it's size
     * Else, Do nothing.
     */
    if(this->numOfUsedCells == this->arrSize/4 && this->arrSize > MAGIC_SIZE){
        this->decreaseSize();
    }

    // using hash table find adequate index of the artist
    int index = this->hash(artistID,this->arrSize);

    // remove artist from the tree at the appropriate cell at the table
    this->arr[index]->deleteVertice(artistID);
}


Artist* HashTable::findArtist(int artistID){
    // using hash table find adequate index of the artist
    int index = this->hash(artistID,this->arrSize);

    // find artist in the tree at the appropriate cell at the table
    return this->arr[index]->find(artistID)->getData();
};

#endif //DTS2_EX2_HASHTABLE_H