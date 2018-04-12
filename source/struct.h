#ifndef _STRUCTH_
#define _STRUCTH_

typedef struct {
    bool PAL;
    bool USA;
    bool JPN;
    int total_regions;
} Regions;

typedef struct {
    bool profile0;
    bool profile1;
    bool profile2;
    int total_saves;
} SavesList;

typedef struct {
    int difficulty;
    bool energy_tank;
    bool energy_filled;
    bool missile_tank;
    bool missile_filled;
    bool aeion_tank;
    bool aeion_filled;
    bool amiibo_unlocked;
} SaveStatus;

typedef enum {
    MAIN_SCREEN,
    VERSION_TO_EDIT,
    SELECT_SAVE,
    OPEN_SAVE,
    READ_SAVE,
    FUSION_OR_NOT,
    THE_WIZARD_IS_BUSY,
    SUCCESS
} States;

typedef enum {
    SD_CARD,
    GAME_CARD
} InstallType;

#endif