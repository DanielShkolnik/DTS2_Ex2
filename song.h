//
// Created by Eleanor on 27/04/2020.
//

#ifndef DTS2_EX1_SONG_H
#define DTS2_EX1_SONG_H

//#include "disc.h"
class Disc;

class Song{
private:
    int songID;
    int artistID;
    int popularity;
    Disc* disc;
public:
    Song(int songID, int artistID):songID(songID),artistID(artistID),popularity(0),disc(nullptr){};
    ~Song() = default;
    Song(const Song& song) = delete;
    Song& operator=(const Song& song) = delete;
    int getSongID() const {
        return this->songID;
    }
    int getArtistID(){
        return this->artistID;
    }
    void increasePopularity(){
        this->popularity++;
    }

    void setDisc(Disc* discToSet){
        this->disc=discToSet;
    }

    Disc* getDisc(){
        return this->disc;
    }

    int getPopularity() const {
        return this->popularity;
    }


};
#endif //DTS2_EX1_SONG_H
