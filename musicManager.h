//
// Created by danie on 28/04/2020.
//

#ifndef DTS2_EX1_MUSICMANAGER_H
#define DTS2_EX1_MUSICMANAGER_H

#include "avl.h"
#include "node.h"
#include "artist.h"
#include "disc.h"
#include "song.h"
#include "library1.h"
#include "exception"
#include <iostream>

class MusicManager{
private:
Avl<int,Artist> artistTree;

// Ranked linked list to keep songs by number of listenings
Node<int,Avl<int,Disc>>* bestHitsListStart;
Node<int,Avl<int,Disc>>* bestHitsListFinish;
int totalSongs;

public:
    MusicManager():totalSongs(0){
        this->bestHitsListStart = new Node<int,Avl<int,Disc>>(0, new Avl<int,Disc>);
        this->bestHitsListFinish = this->bestHitsListStart;
    }

    /* Function used by MusicManager destructor to iterate over disc tree and delete all of it's nodes */
   class DiscPredicateDelete{
    public:
        void operator()(Node<int,Disc>* discNode){
            // delete disc
            delete discNode->getData();
        }
        explicit DiscPredicateDelete() = default;
        DiscPredicateDelete(const DiscPredicateDelete& a) = delete;
    };

    /* Function used by MusicManager destructor to iterate over artist tree and delete all of it's nodes */
    class ArtistPredicate{
    public:
        void operator()(Node<int,Artist>* artistNode){
            // delete artist
            delete artistNode->getData();
        }
        explicit ArtistPredicate() = default;
        ArtistPredicate(const ArtistPredicate& a) = delete;
    };

    ~MusicManager(){
        Node<int,Avl<int,Disc>>* current = this->bestHitsListStart;
        Node<int,Avl<int,Disc>>* prev = current;

        /* Go through all nodes of ranked linked list  */
        while(current!= nullptr){
            current= current->getNext();

            // get the root of disc tree of the prev node in BestHitList
            Node<int,Disc>* discNode = prev->getData()->getRoot();

            // do postorder to free data (disc) in each node
            DiscPredicateDelete discDelete;

            postorder<int,Disc,DiscPredicateDelete>(discNode,discDelete);

            // Delete node of linked list
            delete prev->getData();
            prev->removeNode();
            prev = current;
        }

        // get artist tree root
        Node<int,Artist>* artistNode = this->artistTree.getRoot();

        // create inst of artist destructor
        ArtistPredicate artistPred;

        postorder<int,Artist,ArtistPredicate>(artistNode,artistPred);
    }
    MusicManager(const MusicManager& musicManager) = delete;
    MusicManager& operator=(const MusicManager& musicManager) = delete;

    /*
     * Functionality:
     * creates new artist.
     * inserts artist into artist tree.
     * create new disc of this artist to hold all of his song which have 0 streams. Insert songs into discs song tree.
     * make all the songs point to the disc which hold them.
     * If there are no songs whith 0 streams, need to create new element in rank list and insert it in the
     * beginning of the list.
     * else the element exists, insert disc into rank's disc tree, and update discs pointer to the adequate rank.
     * update total number of songs in the system.
     *
     * Return values: INVALID_INPUT: in case illegal artistID\number of songs.
     *                ALLOCATION_ERROR.
     *                FILURE: in case artist already exists.
     * */
    StatusType AddArtist(int artistID, int numOfSongs){
        if(artistID<=0 || numOfSongs<=0){
            return INVALID_INPUT;
        }
        Artist* artist;
        try {

            // create artist node
            artist = new Artist(artistID,numOfSongs);

            // insert into tree
            this->artistTree.insert(artistID, artist);

            // create new disc
            Disc* disc = new Disc(artistID);

            // update artists song
            for(int i=0;i<artist->getNumOfSongs();i++){
                disc->addSong((artist->getSong(i))); //Need to fix
                artist->getSong(i)->setDisc(disc);
            }

            // insert disc into disc tree at rank 0 in bestHitsList

            // if there is rank 0 at BsetHitsList
            if(this->bestHitsListStart!= nullptr && this->bestHitsListStart->getKey()==0){
                this->bestHitsListStart->getData()->insert(artistID,disc);
                disc->setRankPtr(this->bestHitsListStart);
            }

            // if there isn't rank 0 at BsetHitsList - need to create one and insert it into list
            else{
                Node<int,Avl<int,Disc>>* rankNodeNew = new Node<int,Avl<int,Disc>>(0, new Avl<int,Disc>);
                rankNodeNew->setPrev(nullptr);
                rankNodeNew->setNext(this->bestHitsListStart);
                if(this->bestHitsListStart!= nullptr) this->bestHitsListStart->setPrev(rankNodeNew);
                this->bestHitsListStart=rankNodeNew;
                if(this->bestHitsListFinish == nullptr) this->bestHitsListFinish=this->bestHitsListStart;

                // insert disc into disc tree of newly created node in rank list, with ank 0 & update disc rank pointer
                rankNodeNew->getData()->insert(artistID,disc);

                disc->setRankPtr(rankNodeNew);
            }

            this->totalSongs+=numOfSongs;
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

    //Predicate on Song which updates each song DiscPtr to nullptr
    class SongPredicateDestroy{
    public:
        void operator()(Node<int,Song>* songNode){
            // delete artist
            songNode->getData()->setDisc(nullptr);
        }
        explicit SongPredicateDestroy() = default;
        SongPredicateDestroy(const SongPredicateDestroy& a) = delete;
    };


    /*
     * Functionality:
     * iterate over all artists discs once and delete them.
     * delete all vertices from all the discs trees of the different ranks in the rank list.
     * delete rank list element if it's empty.
     * delete artist - artist's D'tor deletes all songs.
     * */
    StatusType RemoveArtist(int artistID){
        if(artistID<=0) return INVALID_INPUT;
        try{
            Artist* artist= this->artistTree.find(artistID)->getData();
            int numOfSongs=artist->getNumOfSongs();
            Node<int,Avl<int,Disc>>* currentNode;

            // Go through all artist's songs
            for(int i=0; i<numOfSongs; i++){

                Disc* disc = artist->getSong(i)->getDisc();

                // if disc is null it's a mark we've visited & deleted this disc already
                if(disc== nullptr) continue;

                /*
                 * iterate over all songs of this disc and make their disc pointer to point to nullptr.
                 * we won't try to delete it second time, and we won't iterate over all of it's songs again.
                 */
                Node<int,Song>* songNode = disc->getSongTree()->getRoot();
                SongPredicateDestroy songPred;
                postorder<int,Song,SongPredicateDestroy>(songNode,songPred);

                // delete disc from disc tree of adequate node
                currentNode = disc->getRankPtr();
                delete disc;
                currentNode->getData()->deleteVertice(artistID);

                /* check if it was the last disc in this node - if node's empty we can delete it
                 * and update ptrs to the beginning and end of the list
                 */
                if(currentNode->getData()->isEmpty()){
                    if(this->bestHitsListStart->getKey() == currentNode->getKey()){
                        this->bestHitsListStart = currentNode->getNext();
                    }
                    if(this->bestHitsListFinish->getKey() == currentNode->getKey()){
                        this->bestHitsListFinish = currentNode->getPrev();
                    }
                    delete currentNode->getData();
                    currentNode->removeNode();
                }


            }
            //Delete from Artist Tree
            this->artistTree.deleteVertice(artistID);
            delete artist;

            this->totalSongs-=numOfSongs;
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
     * Moves the desired song to higher rank in the BsetHitsList list.
     * */
    StatusType AddToSongCount(int artistID, int songID){
        if(artistID<=0 || songID<0) return INVALID_INPUT;
        try{
            Artist* artist=this->artistTree.find(artistID)->getData();
            if(songID >= artist->getNumOfSongs()) return INVALID_INPUT;
            Song* song = artist->getSong(songID);
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
