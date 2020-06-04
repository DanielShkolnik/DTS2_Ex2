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
#include <iostream>

class MusicManager{
private:
Avl<int,Song>* bestHitsTree;
Hash<int,AVL<int,Artist>>* artistTable;

public:
    MusicManager(){
        // init hash table of size 2 in O(1)
        this->bestHitsTree = new Avl<int,Song>();
    }

    /* Function used by MusicManager destructor to iterate over disc tree and delete all of it's nodes */
    class SongPredicate{
    public:
        void operator()(Node<int,Song>* songNode){
            // delete disc
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
            Node<int,Song>* songNode = artistNode->getData()->getSongRoot();

            // do postorder to free data (song) in each node
            SongPredicate songDelete;

            postorder<int,Song,SongPredicate>(songNode,songDelete);
        }
        explicit ArtistPredicate() = default;
        ArtistPredicate(const ArtistPredicate& a) = delete;
    };

    ~MusicManager(){
        /* For each artist in the Hash table, iterate over it's songs tree, and delete all the song from the tree nodes */
        for(int i = 0; i<this->artistTable->getTableSize; i++){
            // get the root of artist tree of the current cell of the table
            Node<int,Artist>* artistNode = artistTable[i]->getRoot;

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
    StatusType AddArtist(int artistID, int numOfSongs){
        if(artistID<=0 || numOfSongs<=0){
            return INVALID_INPUT;
        }
        Artist* artist;
        try {

            // create artist node
            Artist* artist = new Artist(artistID);

            // insert into table
            this->artistTable->insertTable(artistID,artist);

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
            Node<int,Artist>* artist= this->artistTable.getRoot.find(artistID);

            // check if he has any songs
            if(artist->getData()->getSongRoot() != nullptr) return FAILURE;

            this->artistTable->deleteFromTable();

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
        try {
            // find artist in table
            Avl<int, Artist> *artistTree = this->artistTable[artistID].getRoot;
            Artist *artist = artistTree->find(artistID)->getData();
            Song* song = new Song(songID,artistID,0);
            artist->addSong(song);
        } catch () {

        }
    }

    /* Remove Song - Dont forget to delete data */
    StatusType RemoveSong(int artistID, int songID){
        try {
            // find artist in table
            Avl<int, Artist> *artistTree = this->artistTable[artistID].getRoot;
            Artist *artist = artistTree->find(artistID)->getData();
            Song* song = new Song(songID,artistID,0);
            artist->addSong(song);
        } catch () {

        }
    }

    /*
     * Updates song's number of plays and update's it's rank in the bestHitsTree accordingly.
     */
    StatusType AddToSongCount(int artistID, int songID, int count){
        if(artistID<=0 || songID<0) return INVALID_INPUT;
        try{
            // find artist in table
            Node<int,Artist>* artistNode = this->artistTable.find(artistID)->getData();
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
            Artist* artist= this->artistTree.find(artistID)->getData();
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


    /*
     * Function iterates over the BestHitsList from the end and fills the artists and songs arrays in a sorted manner
     * First comparison by artist and second comparison by song.
     * Since each element in rank list hold disc tree & each disc hold song tree -
     * Need to do inorder traversal on every song tree of each disc,and on the disc tree of each rank list element
     * Until we filled the required number of indices. then, as soon as our counter is zeroed, we stop the traversal.
     * */
    StatusType GetRecommendedSongs(int numOfSongs, int *artists, int *songs){
        //if(numOfSongs <= 0) return INVALID_INPUT;
        if(numOfSongs > this->totalSongs) return FAILURE;
        try {
            int i = 0, j=numOfSongs;
            int *counter = &j;
            int *index = &i;
            // set iterator to end of list & init counter
            Node<int, Avl<int, Disc>> *iter = this->bestHitsListFinish;

            // loop over nodes of list
            for (; (iter != nullptr && *counter > 0); iter = iter->getPrev()) {

                // get disc root
                Node<int, Disc> *discIter = iter->getData()->getRoot();

                // create inst of predicate for disc
                DiscPredicate discPred(artists, counter, index, songs);

                // traverse tree for current rank
                inorder<int, Disc, DiscPredicate>(discIter, discPred);
            }
            return SUCCESS;
        }
        catch (Artist::INVALID_INPUT& e) {
            return INVALID_INPUT;
        }
        catch(std::bad_alloc&) {
            return ALLOCATION_ERROR;
        }
        catch(Avl<int,Artist>::KeyExists& e) {
            return FAILURE;
        }
    }

};


#endif //DTS2_EX1_MUSICMANAGER_H
