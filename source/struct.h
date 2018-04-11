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

typedef enum {
    MAIN_SCREEN,
    VERSION_TO_EDIT,
    SELECT_SAVE,
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