#ifndef _STRUCTH_
#define _STRUCTH_

typedef struct {
    bool PAL;
    bool USA;
    bool JPN;
    int total_regions;
} Regions;

typedef enum {
    MAIN_SCREEN,
    VERSION_TO_EDIT,
    SELECT_SAVE,
    FUSION_OR_NOT,
    THE_WIZARD_IS_BUSY,
    SUCCESS
} States;

#endif