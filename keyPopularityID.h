//
// Created by Eleanor on 04/06/2020.
//

#ifndef DTS2_EX2_KEYPOPULARITYID_H
#define DTS2_EX2_KEYPOPULARITYID_H

class KeyPopulartiyID{
private:
    int songID;
    int numOfPlays;
public:
    KeyPopulartiyID():songID(-1),numOfPlays(-1){};
    KeyPopulartiyID(int songID, int numOfPlays) : songID(songID), numOfPlays(numOfPlays){};
    ~KeyPopulartiyID() = default;
    KeyPopulartiyID(const KeyPopulartiyID& avl)= default;
    KeyPopulartiyID& operator=(const KeyPopulartiyID& avl)= default;
    bool operator<(const KeyPopulartiyID& other) const{
        return (!(this->operator>(other)))&&(this->operator!=(other));
    }
    bool operator>(const KeyPopulartiyID& other) const {
        if(this->numOfPlays > other.numOfPlays){
            return true;
        }
        else if(this->numOfPlays == other.numOfPlays){
            if (this->songID > other.songID) {
                return true;
            }
        }
        return false;
    }
    bool operator==(const KeyPopulartiyID& other) const {
        if(this->numOfPlays == other.numOfPlays){
            if (this->songID == other.songID) {
                return true;
            }
        }
        return false;
    };
    bool operator!=(const KeyPopulartiyID& other) const {
        return !(this->operator==(other));
    };
};

#endif //DTS2_EX2_KEYPOPULARITYID_H
