//
// Created by Eleanor on 27/04/2020.
//

#ifndef DTS2_EX1_ARTIST_H
#define DTS2_EX1_ARTIST_H

#include "node.h"
#include "avl.h"
#include "song.h"

class Artist{
private:
    int artistID;
    Song* mostPlayedSong;
    Avl<int,Song>* bestHitsTree;
public:
    Artist(int artistID,int numOfSongs):artistID(artistID){
        this->bestHitsTree = new Avl<int,Song>();
        mostPlayedSong = nullptr;
    };
    ~Artist(){
        delete bestHitsTree;
    };
    Artist(const Artist& artist)= delete;
    Artist& operator=(const Artist& artist)= delete;
    void addCount(int songID){
        this->bestHitsTree->find(songID)->getData()->increasePopularity();
    }
    int getArtistID() const {
        return this->artistID;
    }

    Node<int,Song>* getSong(int songID){
        return this->bestHitsTree->find(songID);
    }

    class INVALID_INPUT{};
};


#endif //DTS2_EX1_ARTIST_H
