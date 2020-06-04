//
// Created by Eleanor on 04/06/2020.
//

#ifndef DTS2_EX2_KEY_H
#define DTS2_EX2_KEY_H

class key{
private:
    int songID;
    int artistID;
    int numOfPlays;
public:
    key(int songID, int artistID, int numOfPlays) : songID(songID), artistID(artistID), numOfPlays(numOfPlays){};
    ~key();
    key(const key& avl)= delete;
    key& operator=(const key& avl)= delete;
    int getSongID(){
        return songID;
    }
    int getartistID(){
        return artistID;
    }
    int getnumOfPlays(){
        return numOfPlays;
    }
};

#endif //DTS2_EX2_KEY_H
