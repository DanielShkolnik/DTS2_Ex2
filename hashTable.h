//
// Created by danie on 04/06/2020.
//

#ifndef DTS2_EX2_HASHTABLE_H
#define DTS2_EX2_HASHTABLE_H

#include "avl.h"
#include "artist.h"

class HashTable{
private:
    Avl<int,Artist>** arr;
    int numOfUsedCells;
    int arrSize;
    void increaseSize();
    void decreaseSize();
    void deleteArr();
    int hash(int artistID, int arrSize);

public:
    explicit HashTable();
    ~HashTable();
    HashTable(const HashTable&) = delete;
    HashTable& operator=(const HashTable&) = delete;
    void addArtist(int artistID);
    void removeArtist(int artistID);
    Artist* findArtist(int artistID);
};
//void addToArr(int index, int server_id, int DC_id, ChainNode** arr);


#endif //DTS2_EX2_HASHTABLE_H
