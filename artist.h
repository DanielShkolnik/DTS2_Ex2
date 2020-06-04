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
        songTreeByPlays->deleteVertice(key);
        song->increasePopularity(count);
        songTreeByPlays->insert(key,song);
        KeyPopulartiyID keyMostPlayedSong = KeyPopulartiyID(mostPlayedSong->getSongID(),mostPlayedSong->getPopularity());
        if(key > keyMostPlayedSong){
            mostPlayedSong = song;
        }
    }
    int getArtistID() const {
        return this->artistID;
    }
    Node<int,Song>* getSong(int songID){
        return this->songTreeByID->find(songID);
    }
    // maybe return tree??
    Node<int,Song>* getSongRoot(){
        return this->songTreeByID->getRoot();
    }
    void addSong(Song* song){
        this->songTreeByID->insert(song->getSongID(),song);
        if(song->getPopularity() > mostPlayedSong->getPopularity()){
            mostPlayedSong = song;
        }
        else if(song->getPopularity() == mostPlayedSong->getPopularity()){
            if(song->getSongID() > mostPlayedSong->getSongID()){
                mostPlayedSong = song;
            }
        }
    }
    void removeSong(Song* song){
        // Dont forget to update mostPopular!
        this->songTree->deleteVertice(song->getSongID());
    }
    Song* getMostPlayed() const {
        return this->mostPlayedSong;
    }

    class INVALID_INPUT{};
};


#endif //DTS2_EX1_ARTIST_H
