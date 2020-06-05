//
// Created by danie on 05/06/2020.
//

#include "library2.h"
#include "musicManager.h"

void *Init(){
    MusicManager *DS = new MusicManager();
    return (void*)DS;
}

StatusType AddArtist(void *DS, int artistID){
    if(DS == nullptr) {return INVALID_INPUT;}
    return ((MusicManager*)DS)->AddArtist(artistID);

};

StatusType RemoveArtist(void *DS, int artistID){
    if(DS == nullptr) return INVALID_INPUT;
    return ((MusicManager*)DS)->RemoveArtist(artistID);
};

StatusType AddSong(void *DS, int artistID, int songID){
    if(DS == nullptr) return INVALID_INPUT;
    return ((MusicManager*)DS)->AddSong(artistID,songID);
};

StatusType RemoveSong(void *DS, int artistID, int songID){
    if(DS == nullptr) return INVALID_INPUT;
    return ((MusicManager*)DS)->RemoveSong(artistID,songID);
};

StatusType AddToSongCount(void *DS, int artistID, int songID, int count){
    if(DS == nullptr) return INVALID_INPUT;
    return ((MusicManager*)DS)->AddToSongCount(artistID,songID,count);
};

StatusType GetArtistBestSong(void *DS, int artistID, int *songId){
    if(DS == nullptr) return INVALID_INPUT;
    return ((MusicManager*)DS)->GetArtistBestSong(artistID,songId);
};

StatusType GetRecommendedSongInPlace(void *DS, int rank, int *artistId, int *songId){
    if(DS == nullptr) return INVALID_INPUT;
    return ((MusicManager*)DS)->GetRecommendedSongInPlace(rank,artistId,songId);
};

void Quit(void** DS){
    delete *(MusicManager **)DS;
    *DS = nullptr;
};
