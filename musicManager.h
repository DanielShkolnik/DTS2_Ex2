//
// Created by danie on 28/04/2020.
//

#ifndef DTS2_EX1_MUSICMANAGER_H
#define DTS2_EX1_MUSICMANAGER_H

#include "avl.h"
#include "node.h"
#include "artist.h"
#include "song.h"
#include "exception"
#include "library2.h"
#include "keyBestHitsTree.h"
#include "hashtable.h"
#include <iostream>

class MusicManager{
private:
Avl<int,Song>* bestHitsTree;
HashTable artistHashTable;
int totalSongs;

public:
    MusicManager(): bestHitsTree(new Avl<int,Song>), artistHashTable() {};

    /* Function used by MusicManager destructor to iterate over song tree and delete all of the data stored in the nodes */
    class SongPredicate{
    public:
        void operator()(Node<int,Song>* songNode){
            // delete song
            delete songNode->getData();
        }
        explicit SongPredicate() = default;
        SongPredicate(const SongPredicate& a) = delete;
    };

    /* Function used by MusicManager destructor to iterate over artist tree and delete all of it's nodes */
    class ArtistPredicate{
    public:
        void operator()(Node<int,Artist>* artistNode){
            // get the root of song tree of the current artist
            Node<int,Song>* songNode = artistNode->getData()->getRootInSongTreeByID();

            // do postorder to free data (song) in each node
            SongPredicate songDelete;

            postorder<int,Song,SongPredicate>(songNode,songDelete);
        }
        explicit ArtistPredicate() = default;
        ArtistPredicate(const ArtistPredicate& a) = delete;
    };

    ~MusicManager(){
        /* For each artist in the Hash table, iterate over it's songs tree, and delete all the song from the tree nodes */
        for(int i = 0; i<artistHashTable.getTableSize(); i++){
            // get the root of artist tree of the current cell of the table
            Node<int,Artist>* artistNode = this->artistHashTable.getArray()[i]->getRoot();

            // do postorder to free data (song) for each artist
            ArtistPredicate artistPredicate;

            postorder<int,Artist,ArtistPredicate>(artistNode,artistPredicate);

        }
    }
    MusicManager(const MusicManager& musicManager) = delete;
    MusicManager& operator=(const MusicManager& musicManager) = delete;

    /*
     * Functionality:
     * Creates new artist and inserts him into artist table.
     *
     * Return values: INVALID_INPUT: in case illegal artistID\number of songs.
     *                ALLOCATION_ERROR.
     *                FAILURE: in case artist already exists.
     * */
    StatusType AddArtist(int artistID){
        if(artistID<=0){
            return INVALID_INPUT;
        }
        Artist* artist;
        try {
            // create artist node
            Artist* artist = new Artist(artistID);

            // insert into table
            this->artistHashTable.addArtist(artist);

            return SUCCESS;
        }
        catch (std::bad_alloc& e) {
            return ALLOCATION_ERROR;
        }
        catch(const Avl<int,Artist>::KeyExists& e) {
            delete artist;
            return FAILURE;
        }
    }


    /*
     * Functionality:
     * Remove artist from table if he has 0 songs.
     */
    StatusType RemoveArtist(int artistID){
        if(artistID<=0) return INVALID_INPUT;
        try{
            // find artist in the table
            Artist* artist= this->artistHashTable.findArtist(artistID);

            // check if he has any songs
            if(artist->getRootInSongTreeByID() != nullptr) return FAILURE;

            // delete artist from table
            this->artistHashTable.removeArtist(artistID);

            return SUCCESS;
        }
        catch(std::bad_alloc& e) {
            return ALLOCATION_ERROR;
        }
        catch(Avl<int,Artist>::KeyNotFound&){
            return FAILURE;
        }
    };

    /* Add Song */
    StatusType AddSong(int artistID, int songID){
        if(artistID<=0 || songID<=0) { return INVALID_INPUT; }
        try {
            // find artist in table
            Artist *artist = this->artistHashTable.findArtist(artistID);

            // create new song
            Song* song = new Song(songID,artistID);

            // add song to artist's song tree
            artist->addSong(song);

            this->totalSongs++;

            return SUCCESS;
        }
        catch(std::bad_alloc& e) {
            return ALLOCATION_ERROR;
        }
        // In case artist not found
        catch(Avl<int,Artist>::KeyNotFound&){
            return FAILURE;
        }
    }

    /* Remove Song - Dont forget to delete data */
    //Remember not Delete song before this function
    StatusType RemoveSong(int artistID, int songID){
        if(artistID<=0 || songID<=0) { return INVALID_INPUT; }
        try {
            // find artist in table
            Artist *artist = this->artistHashTable.findArtist(artistID);
            // get song to delete
            Song* song = artist->getSongByID(songID)->getData();
            artist->removeSong(songID);
            this->totalSongs--;
            return SUCCESS;
        }
        catch(std::bad_alloc& e) {
            return ALLOCATION_ERROR;
        }
            // In case artist not found
        catch(Avl<int,Artist>::KeyNotFound&){
            return FAILURE;
        }
    }

    /*
     * Updates song's number of plays and update's it's rank in the bestHitsTree accordingly.
     */
    StatusType AddToSongCount(int artistID, int songID, int count){
        if(artistID<=0 || songID<0) return INVALID_INPUT;
        try{
            // find artist in table
            Artist* artist = this->artistHashTable.findArtist(artistID);
            artist->addCount(songID,count);
            return SUCCESS;
        }
        catch(std::bad_alloc&) {
            return ALLOCATION_ERROR;
        }
        catch(Avl<int,Artist>::KeyNotFound&){
            return FAILURE;
        }
    }

    /*
     * Go to songs and return the number of it's streams
     */
    StatusType  GetArtistBestSong(int artistID, int *songID){
        if(artistID<=0 || songID == nullptr) return INVALID_INPUT;
        try {
            Artist* artist= this->artistHashTable.findArtist(artistID);

            // artist has no songs
            if(artist->getRootInSongTreeByID() == nullptr) return FAILURE;

            *songID = artist->getMostPlayed()->getSongID();
            return SUCCESS;
        }
        catch (std::bad_alloc& e) {
            return ALLOCATION_ERROR;
        }
        catch(const Avl<int,Artist>::KeyNotFound& e) {
            return FAILURE;
        }
    }

    StatusType GetRecommendedSongInPlace(int rank, int *artistID, int *songID){
        if(rank <= 0) return INVALID_INPUT;
        if(this->totalSongs<rank) return FAILURE;
        try {
            Node<int,Song>* current = this->bestHitsTree->getRoot();
            int k=rank;
            while(current!= nullptr){
                if(current->getLeft()->getRank()==k-1) {
                    *artistID=current->getData()->getArtistID();
                    *songID=current->getData()->getSongID();
                    return SUCCESS;
                }
                else if(current->getLeft()->getRank()>k-1){
                    current=current->getLeft();
                }
                else if(current->getLeft()->getRank()<k-1){
                    current=current->getRight();
                    k = k-current->getLeft()->getRank()-1;
                }
            }

            return FAILURE;
        }
        catch(std::bad_alloc&) {
            return ALLOCATION_ERROR;
        }
    }

};


#endif //DTS2_EX1_MUSICMANAGER_H
