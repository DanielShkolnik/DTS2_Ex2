//
// Created by Eleanor on 27/04/2020.
//

#ifndef DTS2_EX1_SONG_H
#define DTS2_EX1_SONG_H


class Song{
private:
    int songID;
    int artistID;
    int popularity;
public:
    Song(int songID, int artistID):songID(songID),artistID(artistID),popularity(0){};
    ~Song() = default;
    Song(const Song& song) = delete;
    Song& operator=(const Song& song) = delete;
    int getSongID() const {
        return this->songID;
    }
    int getArtistID(){
        return this->artistID;
    }
    void increasePopularity(int count){
        this->popularity+=count;
    }
    int getPopularity() const {
        return this->popularity;
    }
};
#endif //DTS2_EX1_SONG_H
