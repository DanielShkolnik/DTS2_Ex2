//
// Created by Eleanor on 27/04/2020.
//

#ifndef DTS2_EX1_ARTIST_H
#define DTS2_EX1_ARTIST_H

#include "node.h"
#include "song.h"

class Artist{
private:
    int artistID;
    int numOfSongs;
    Song** songArray;
public:
    Artist(int artistID,int numOfSongs):artistID(artistID),numOfSongs(numOfSongs){
        this->songArray = new Song*[numOfSongs]();
        for (int i = 0; i < this->numOfSongs; ++i) {
            songArray[i] = new Song(i, this->artistID);
        }

    };
    ~Artist(){
        for (int i = 0; i < numOfSongs; ++i) {
            delete this->songArray[i];
        }
        delete[] songArray;
    };
    Artist(const Artist& artist)= delete;
    Artist& operator=(const Artist& artist)= delete;
    void addCount(int songID){
        songArray[songID]->increasePopularity();
    }
    int getArtistID() const {
        return this->artistID;
    }

    Song* getSong(int index){
        return this->songArray[index];
    }

    int getNumOfSongs() const {
        return this->numOfSongs;
    }

    class INVALID_INPUT{};

};


#endif //DTS2_EX1_ARTIST_H
