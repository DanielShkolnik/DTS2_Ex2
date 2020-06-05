//
// Created by Eleanor on 27/04/2020.
//

#ifndef DTS2_EX1_ARTIST_H
#define DTS2_EX1_ARTIST_H

#include "node.h"
#include "avl.h"
#include "song.h"
#include "keyPopularityID.h"

class Artist{
private:
    int artistID;
    Song* mostPlayedSong;
    Avl<int,Song>* songTreeByID;
    Avl<KeyPopulartiyID,Song>* songTreeByPlays;
    
public:
    Artist(int artistID):artistID(artistID){
        this->songTreeByID = new Avl<int,Song>();
        this->songTreeByPlays = new Avl<KeyPopulartiyID,Song>();
        mostPlayedSong = nullptr;
    };
    ~Artist(){
        delete songTreeByID;
        delete songTreeByPlays;
    };
    Artist(const Artist& artist)= delete;
    Artist& operator=(const Artist& artist)= delete;

    void addCount(int songID,int count){
        Song* song = this->songTreeByID->find(songID)->getData();
        KeyPopulartiyID key = KeyPopulartiyID(songID,song->getPopularity());
        this->songTreeByPlays->deleteVertice(key);
        song->increasePopularity(count);
        key = KeyPopulartiyID(songID,song->getPopularity());
        this->songTreeByPlays->insert(key,song);
        KeyPopulartiyID keyMostPlayedSong = KeyPopulartiyID(this->mostPlayedSong->getSongID(),this->mostPlayedSong->getPopularity());
        if(key > keyMostPlayedSong){
            this->mostPlayedSong = song;
        }
    }

    int getArtistID() const {
        return this->artistID;
    }

    Node<int,Song>* getSongByID(int songID){
        return this->songTreeByID->find(songID);
    }


    Node<int,Song>* getRootInSongTreeByID(){
        return this->songTreeByID->getRoot();
    }


    void addSong(Song* song){
        this->songTreeByID->insert(song->getSongID(),song);
        KeyPopulartiyID key = KeyPopulartiyID(song->getSongID(),song->getPopularity());
        this->songTreeByPlays->insert(key,song);
        if(this->mostPlayedSong!= nullptr){
            KeyPopulartiyID keyMostPlayedSong = KeyPopulartiyID(this->mostPlayedSong->getSongID(),this->mostPlayedSong->getPopularity());
            if(key > keyMostPlayedSong){
                this->mostPlayedSong = song;
            }
        }
        else this->mostPlayedSong = song;

    }

    //Remember not Delete song before this function
    void removeSong(int songID){
        Song* song = this->getSongByID(songID)->getData();
        this->songTreeByID->deleteVertice(song->getSongID());
        KeyPopulartiyID key = KeyPopulartiyID(song->getSongID(),song->getPopularity());
        this->songTreeByPlays->deleteVertice(key);
        KeyPopulartiyID keyMostPlayedSong = KeyPopulartiyID(this->mostPlayedSong->getSongID(),this->mostPlayedSong->getPopularity());
        if(key == keyMostPlayedSong){
            Node<KeyPopulartiyID,Song>* bestSongNode = this->songTreeByPlays->getMaxNode();
            if(bestSongNode != nullptr) this->mostPlayedSong = bestSongNode->getData();
            else this->mostPlayedSong = nullptr;
        }
    }

    Song* getMostPlayed() const {
        return this->mostPlayedSong;
    }

};


#endif //DTS2_EX1_ARTIST_H
