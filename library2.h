/****************************************************************************/
/*                                                                          */
/* This file contains the interface functions                               */
/* you should use for the wet ex 1                                          */
/*                                                                          */
/****************************************************************************/

/****************************************************************************/
/*                                                                          */
/* File Name : library1.h                                                   */
/*                                                                          */
/****************************************************************************/

#ifndef _234218_WET1
#define _234218_WET1

#include "musicManager.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Return Values
 * ----------------------------------- */
typedef enum {
    SUCCESS = 0,
    FAILURE = -1,
    ALLOCATION_ERROR = -2,
    INVALID_INPUT = -3
} StatusType;


void *Init();

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
    return ((MusicManager*)DS)->GetRecommendedSongInPlace(rank,artistId,songId)
};

void Quit(void** DS){
    delete *(MusicManager **)DS;
    *DS = nullptr;
};

#ifdef __cplusplus
}
#endif

#endif    /*_234218_WET1 */
