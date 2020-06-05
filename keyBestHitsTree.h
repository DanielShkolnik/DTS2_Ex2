//
// Created by Eleanor on 04/06/2020.
//

#ifndef DTS2_EX2_KEYBESTHITSTREE_H
#define DTS2_EX2_KEYBESTHITSTREE_H

class KeyBestHitsTree{
private:
    int songID;
    int artistID;
    int numOfPlays;
public:
    KeyBestHitsTree():songID(-1),artistID(-1),numOfPlays(-1){};
    KeyBestHitsTree(int songID, int artistID, int numOfPlays) : songID(songID), artistID(artistID), numOfPlays(numOfPlays){};
    ~KeyBestHitsTree() = default;
    KeyBestHitsTree(const KeyBestHitsTree& avl)= default;
    KeyBestHitsTree& operator=(const KeyBestHitsTree& avl)= default;
    bool operator<(const KeyBestHitsTree& other) const{
        return (!(this->operator>(other)))&&(this->operator!=(other));
    }
    bool operator>(const KeyBestHitsTree& other) const {
        if(this->numOfPlays > other.numOfPlays){
            return true;
        }
        else if(this->numOfPlays == other.numOfPlays){
            if (this->artistID < other.artistID) {
                return true;
            }
            else if (this->artistID == other.artistID) {
                if (this->songID < other.songID) {
                    return true;
                }
            }
        }
        return false;
    }
    bool operator==(const KeyBestHitsTree& other) const {
        if(this->numOfPlays == other.numOfPlays){
           if(this->artistID == other.artistID){
               if (this->songID == other.songID) {
                   return true;
               }
           }
        }
        return false;
    };
    bool operator!=(const KeyBestHitsTree& other) const {
        return !(this->operator==(other));
    };
};

#endif //DTS2_EX2_KEYBESTHITSTREE_H
