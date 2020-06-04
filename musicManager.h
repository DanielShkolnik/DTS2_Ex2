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
            Node<int,Artist>* artistNode = this->artistHashTable.find(artistID)->getData();
            Song* song = artistNode->getData()->getSong(songID);
            Disc* discOld = song->getDisc();
            Node<int,Avl<int,Disc>>* rankNodeOld = discOld->getRankPtr();

            // take out song from old disc since it's number of plays doesn't match to the discs anymore
            discOld->removeSong(songID);

            // if there are no songs in the disc - we can delete it
            if(discOld->getSongTree()->isEmpty()){
                delete discOld;
                rankNodeOld->getData()->deleteVertice(artistID);
            }

            // update songs num of plays
            artist->addCount(songID);

            int popularity = song->getPopularity();

            /* if the next element in list exists & matches the new popularity rank of the song:
             * check if there's a disc which matches to the artist of this song in the next element of the list
             * if there is - insert song into adequate disc & update song's disc ptr
             */
            if(rankNodeOld->getNext()!= nullptr && rankNodeOld->getNext()->getKey()==popularity){
                try{
                    Disc* disc=rankNodeOld->getNext()->getData()->find(artistID)->getData();
                    disc->addSong(song);
                    song->setDisc(disc);
                }
                // if there isn't appropriate disc - create one. insert it into the disc tree of the next rank.
                // insert song into disc's song tree & update disc's ptr to rank element & song's ptr to disc
                catch(Avl<int,Disc>::KeyNotFound& e){
                    Disc* discNew = new Disc(artistID);
                    rankNodeOld->getNext()->getData()->insert(artistID,discNew);
                    discNew->setRankPtr(rankNodeOld->getNext());
                    discNew->addSong(song);
                    song->setDisc(discNew);
                }
            }


            /* In case rank doesn't exist - create new one. create new disc. insert disc into disc's tree of rank
             * element & update disc's pointer to the rank. insert song into disc's song tree & update song's ptr
             * to disc.
             */
            else{
                Node<int,Avl<int,Disc>>* rankNodeNew = new Node<int,Avl<int,Disc>>(popularity, new Avl<int,Disc>);
                rankNodeNew->setPrev(rankNodeOld);
                rankNodeNew->setNext(rankNodeOld->getNext());
                if(rankNodeOld->getNext()!= nullptr) rankNodeOld->getNext()->setPrev(rankNodeNew);
                rankNodeOld->setNext(rankNodeNew);
                Disc* discNew = new Disc(artistID);
                rankNodeNew->getData()->insert(artistID,discNew);
                discNew->setRankPtr(rankNodeNew);
                discNew->addSong(song);
                song->setDisc(discNew);
            }

            /* check if the old rank, from which we removed the song is empty.
             * if so - update start and end pointers of the list and delete this rank.
             * */
            if(rankNodeOld->getData()->isEmpty()){
                if(this->bestHitsListStart->getKey() == rankNodeOld->getKey()){
                    this->bestHitsListStart = rankNodeOld->getNext();
                }
                if(this->bestHitsListFinish->getKey() == rankNodeOld->getKey()){
                    this->bestHitsListFinish = rankNodeOld->getNext();
                }

                delete rankNodeOld->getData();
                rankNodeOld->removeNode();
            }

            if(this->bestHitsListFinish->getNext()!= nullptr){
                this->bestHitsListFinish=this->bestHitsListFinish->getNext();

            }

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
    StatusType NumberOfStreams(int artistID, int songID, int *streams){
        if(artistID<=0 || songID<0) return INVALID_INPUT;
        try {
            Artist* artist= this->artistHashTable.find(artistID)->getData();
            if(artist->getNumOfSongs()<=songID) return INVALID_INPUT;
            *streams = artist->getSong(songID)->getPopularity();
            return SUCCESS;
        }
        catch (std::bad_alloc& e) {
            return ALLOCATION_ERROR;
        }
        catch(const Avl<int,Artist>::KeyNotFound& e) {
            return FAILURE;
        }
    }

    /* Helper function. For each disc, we go over all of it's songs by order and update the adequate indices in
     * the artists and songs array. See DiscPredicate below for more info.
     */
    class SongPredicate{
    private:
        int* artistsArray;
        int artistID;
        int* songsArray;
        int* index;
        int* counter;
    public:
        void operator()(Node<int,Song>* songNode){

            // finish when counter equal to zero
            if(*counter > 0) {

                // add to artists array
                artistsArray[*index] = artistID;

                // add to songs list
                songsArray[*index] = songNode->getData()->getSongID();

                // update index
                (*index) = (*index) + 1;
                (*counter) = (*counter) - 1;
            }
            else return;
        }
        explicit SongPredicate(int* artistsArray,int artistID,int* songsArray, int* index,int* counter):artistsArray(artistsArray),artistID(artistID), songsArray(songsArray), index(index),counter(counter){};
        SongPredicate(const SongPredicate& a) = delete;
    };

    /* Helper function. For each Node in the rank linked list, we go over all of the discs in it's disc tree.
     * For each disc we call the song predicate function which updates the arrays.
     */
    class DiscPredicate{
    private:
        int* artistsArray;
        int* counter;
        int* index;
        int* songsArray;
    public:
        // for each node in disc tree do inorder traverse on song tree + add song to song array & artist to artist array
        void operator()(Node<int,Disc>* disc){

            /* always check counter is greater than zero - so we won't iterate over unnecessary nodes of the disc tree
             * once we finished filling the songs and artists array up to index numOfSongs
             */
            if(*counter > 0){
            // get songTree root
            Node<int,Song>* song = (disc->getData()->getSongTree()->getRoot());


            // create inst of predicate for song
            SongPredicate songPred(artistsArray,song->getData()->getArtistID(),songsArray,index,counter);

            // traverse song tree
            inorder<int,Song,SongPredicate>(song, songPred);
            }
            else return;
        }
        explicit DiscPredicate(int* artistsArray,int* counter,int* index,int* songsArray):artistsArray(artistsArray),counter(counter),index(index),songsArray(songsArray){};
        DiscPredicate(const DiscPredicate& a) = delete;
    };



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
