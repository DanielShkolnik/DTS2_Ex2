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
#include "hashTable.h"
#include <iostream>


class MusicManager{
private:
Avl<KeyBestHitsTree,Song>* bestHitsTree;
HashTable artistHashTable;
int totalSongs;

public:
    /* Create empty Tree and empty hash table */
    MusicManager(): bestHitsTree(new Avl<KeyBestHitsTree,Song>), artistHashTable(), totalSongs(0) {};

    /* Function used by MusicManager destructor to iterate over song tree and delete all of the data stored in the nodes */
    /*class SongPredicate{
    public:
        void operator()(Node<int,Song>* songNode){
            // delete song
            delete songNode->getData();
        }
        explicit SongPredicate() = default;
        SongPredicate(const SongPredicate& a) = delete;
    }; */

    /*Function used by MusicManager destructor to iterate over artist tree and delete all of the songs  */
    /*class ArtistPredicate{
    public:
        void operator()(Node<int,Artist>* artistNode){
            // get the root of song tree of the current artist
            Node<int,Song>* songNode = artistNode->getData()->getRootInSongTreeByID();

            // do postorder to free data (song) in each node
            SongPredicate songDelete;

            postorder<int,Song,SongPredicate>(songNode,songDelete);

            delete artistNode->getData();
        }
        explicit ArtistPredicate() = default;
        ArtistPredicate(const ArtistPredicate& a) = delete;
    };*/

    class SongPredicate{
    public:
        void operator()(Node<KeyBestHitsTree,Song>* songNode){
            // delete song
            delete songNode->getData();
        }
        explicit SongPredicate() = default;
        SongPredicate(const SongPredicate& a) = delete;
    };

    /* Function used by MusicManager destructor to iterate over artist tree and delete all of the songs  */
    class ArtistPredicate{
    public:
        void operator()(Node<int,Artist>* artistNode){
            delete artistNode->getData();
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

        // get the root of song tree of the current artist
        Node<KeyBestHitsTree,Song>* songNode = bestHitsTree->getRoot();

        // do postorder to free data (song) in each node
        SongPredicate songDelete;

        /* do postorder on song tree and delete it's data (songs) */
        postorder<KeyBestHitsTree,Song,SongPredicate>(songNode,songDelete);

        delete bestHitsTree;
        // this->artistHashTable.~HashTable();
    }
    MusicManager(const MusicManager& musicManager) = delete;
    MusicManager& operator=(const MusicManager& musicManager) = delete;

    /*
     * Functionality:
     * Creates new artist and inserts him into artist table.
     *
     * Return values: INVALID_INPUT: in case illegal artistID.
     *                ALLOCATION_ERROR.
     *                FAILURE: in case artist already exists.
     * */
    StatusType AddArtist(int artistID){
        if(artistID<=0) return INVALID_INPUT;
        Artist* artist;
        try {
            // create artist node
            artist = new Artist(artistID);

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

            // delete artist node from the tree table
            this->artistHashTable.removeArtist(artistID);

            // delete artist
            delete artist;

            return SUCCESS;
        }
        catch(std::bad_alloc& e) {
            return ALLOCATION_ERROR;
        }
        catch(Avl<int,Artist>::KeyNotFound&){
            return FAILURE;
        }
    };

    /* Add Song - insert song into adequate artist's song tree */
    StatusType AddSong(int artistID, int songID){
        if(artistID<=0 || songID<=0) return INVALID_INPUT;
        Song* song;
        try {
            // find artist in table
            Artist *artist = this->artistHashTable.findArtist(artistID);

            // create new song
            song = new Song(songID,artistID);

            // add song to artist's song tree
            artist->addSong(song);

            // create key for current song to insert into bestHitsTree
            KeyBestHitsTree key = KeyBestHitsTree(songID,artistID,0);

            // add song to bestHitsTree
            this->bestHitsTree->insert(key,song);

            // update total number of songs stored in the system
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
        // In case song exists
        catch(Avl<int,Song>::KeyExists&){
            delete song;
            return FAILURE;
        }
        catch(Avl<KeyBestHitsTree,Song>::KeyExists&){
            delete song;
            return FAILURE;
        }
        catch(Avl<KeyPopulartiyID,Song>::KeyExists&) {
            delete song;
            assert(false);
            return FAILURE;
        }
    }

    /* Remove song from artist song tree */
    StatusType RemoveSong(int artistID, int songID){
        if(artistID<=0 || songID<=0) return INVALID_INPUT;
        try {
            // find artist in table
            Artist *artist = this->artistHashTable.findArtist(artistID);
            // get song to delete
            Song* song = artist->getSongByID(songID)->getData();
            // artist deletes the adequate node which holds the song we need to delete
            artist->removeSong(songID);

            // create bestHitsTree key
            KeyBestHitsTree key = KeyBestHitsTree(songID,artistID,song->getPopularity());

            // delete song from tree
            this->bestHitsTree->deleteVertice(key);

            delete song;
            // update total number of songs stored in the system
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
        // In case song not found
        catch(Avl<int,Song>::KeyNotFound&){
            return FAILURE;
        }
        catch(Avl<KeyPopulartiyID,Song>::KeyNotFound&){
            return FAILURE;
        }
        catch(Avl<KeyPopulartiyID,Song>::KeyExists&){
            assert(false);
            return FAILURE;
        }
    }

    /*
     * Function used to Update song's number of plays and to update it's rank in the bestHitsTree accordingly.
     */
    StatusType AddToSongCount(int artistID, int songID, int count){
        if(artistID<=0 || songID<=0 || count<=0) return INVALID_INPUT;
        try{
            // find artist in table
            Artist* artist = this->artistHashTable.findArtist(artistID);

            Song* song = artist->getSongByID(songID)->getData();

            // remove from bestHitsTree, update number of plays, and insert back
            KeyBestHitsTree key = KeyBestHitsTree(songID,artistID,song->getPopularity());

            // delete song from tree
            this->bestHitsTree->deleteVertice(key);

            // update songs number of plays
            artist->addCount(songID,count);

            // create updated key for song
            key = KeyBestHitsTree(songID,artistID,song->getPopularity());

            // insert back into bestHitsTree
            this->bestHitsTree->insert(key,song);

            return SUCCESS;
        }
        catch(std::bad_alloc&) {
            return ALLOCATION_ERROR;
        }
        catch(Avl<int,Artist>::KeyNotFound&){
            return FAILURE;
        }
        catch(Avl<int,Song>::KeyNotFound&){
            return FAILURE;
        }
        // if song isn't in bestHitsTree
        catch(Avl<KeyBestHitsTree,Song>::KeyNotFound&){
            return FAILURE;
        }
    }

    /*
     * Find artist and get from his song tree the most played song.
     */
    StatusType  GetArtistBestSong(int artistID, int *songID){
        if(artistID<=0 || songID == nullptr) return INVALID_INPUT;
        try {
            // find artist in hash table
            Artist* artist= this->artistHashTable.findArtist(artistID);

            // artist has no songs
            if(artist->getMostPlayed() == nullptr) return FAILURE;

            // get number of streams of the most popular song
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

    /*
     * Use Select(k) algorithm as seen in the tutorial in order to find the song in rank "rank".
     */
    StatusType GetRecommendedSongInPlace(int rank, int *artistID, int *songID){
        if(rank <= 0 || artistID == nullptr || songID == nullptr) return INVALID_INPUT;
        if(this->totalSongs<rank) return FAILURE;
        try {
            Node<KeyBestHitsTree,Song>* current = this->bestHitsTree->getRoot();
            int k=rank;
            int rightSonRank;
            while(current!= nullptr){
                if(current->getRight()!= nullptr) rightSonRank = current->getRight()->getRank();
                else rightSonRank=0;
                if(current->isLeaf() || rightSonRank==k-1) {
                    *artistID=current->getData()->getArtistID();
                    *songID=current->getData()->getSongID();
                    return SUCCESS;
                }
                else if(rightSonRank>k-1){
                    current=current->getRight();
                }
                else if(rightSonRank<k-1){
                    k = k-rightSonRank-1;
                    current=current->getLeft();
                }

            }

            return SUCCESS;
        }
        catch(std::bad_alloc&) {
            return ALLOCATION_ERROR;
        }
    }

};


#endif //DTS2_EX1_MUSICMANAGER_H
