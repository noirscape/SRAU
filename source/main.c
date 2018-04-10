#include <stdio.h>
#include <3ds.h>
#include "editprofile.h"
#include "main.h"
#include "title.h"
#include "struct.h"

int main() {
    cfguInit();
    gfxInitDefault();
    PrintConsole topScreen, bottomScreen;
    consoleInit(GFX_TOP, &topScreen);
    consoleInit(GFX_BOTTOM, &bottomScreen);
    consoleSelect(&topScreen);

    States state = MAIN_SCREEN;
    Regions regions_found = {false, false, false, 0};
    int profile_num = 0;
    bool fusion_mode = false;
    bool not_busy = true;
    bool version_undetermined = true;
    u32 lowid;

    printf("Metroid: SAMUS RETURNS amiibo unlocker v1.1a\nPress A to continue.\n");

    while(aptMainLoop()) {
        hidScanInput();
        u32 kDown = hidKeysDown();
        if (kDown & KEY_START && not_busy) 
        {
            printf("Exiting\n");
            break;
        }

        switch(state){
            case MAIN_SCREEN:
                if (kDown & KEY_A) 
                {
                    printSaveSelect();
                    state = VERSION_TO_EDIT;
                }
                break;

            case VERSION_TO_EDIT: // Determining region
                if (version_undetermined)
                {
                    lowid = title_check(&regions_found);
                    if (regions_found.total_regions == 0)
                    {
                        printf("Press START to exit.\n");
                        state = SUCCESS;
                    } else if (regions_found.total_regions == 1)
                    {
                    state = SELECT_SAVE;
                    } else {
                        version_undetermined = false;
                        // Insert code here to display stuff about region selection
                        if(regions_found.JPN)
                            printf("JPN region detected. Press X to use this region.\n");
                        if(regions_found.USA)
                            printf("USA region detected. Press Y to use this region.\n");
                        if(regions_found.PAL)
                            printf("PAL region detected. Press A to use this region.\n");
                    }
                } else {
                    if (kDown & KEY_X) // JPN region
                    {
                        printf("JPN region selected.\n");
                        lowid = JPN_LOWID;
                        state = SELECT_SAVE;
                    }
                    if (kDown & KEY_Y) // USA region
                    {
                        printf("USA region selected.\n");
                        lowid = USA_LOWID;
                        state = SELECT_SAVE;
                    }
                    if (kDown & KEY_A) // PAL region
                    {
                        printf("PAL region selected.\n");
                        lowid = PAL_LOWID;
                        state = SELECT_SAVE;
                    }
                }
                break;

            case SELECT_SAVE: // Save file selection
                if (kDown & KEY_Y) // Save 1 was chosen
                {
                    profile_num = 0;
                    printf("You selected the 1st save file.\n");
                    printFusionSelect();
                    state = FUSION_OR_NOT;
                }
                if (kDown & KEY_B) // Save 2 was chosen
                {
                    profile_num = 1;
                    printf("You selected the 2nd save file.\n");
                    printFusionSelect();
                    state = FUSION_OR_NOT;
                }
                if (kDown & KEY_X) // Save 3 was chosen
                {
                    profile_num = 2;
                    printf("You selected the 3rd save file.\n");
                    printFusionSelect();
                    state = FUSION_OR_NOT;
                }
                break;

            case FUSION_OR_NOT: // Enable fusion mode, yay or nay?
                if (kDown & KEY_A)
                {
                    fusion_mode = true;
                    printf("Enabled fusion mode.\n");
                    state = THE_WIZARD_IS_BUSY;
                }
                if (kDown & KEY_B)
                {
                    printf("Not enabling fusion mode.\n");
                    state = THE_WIZARD_IS_BUSY;
                }
                break;

            case THE_WIZARD_IS_BUSY: // Here the editing happens
                not_busy = false;
                Result res = edit_profile(profile_num, fusion_mode, lowid);
                if(R_FAILED(res)) {
                    printf("\x1b[s\x1b[30;16H%4lx\n\x1b[u", res);
                    printf("Something went wrong, please see the error message below.\n");
                } else {
                    printf("Amiibo's have been unlocked for the selected save file. Press START to exit.");
                }
                not_busy = true;
                state = SUCCESS;
                break;

            case SUCCESS:
                break;
        }

        gfxFlushBuffers();
        gfxSwapBuffers();
        gspWaitForVBlank();
    }

    cfguExit();
    gfxExit();
    return 0;
}

void printSaveSelect() {
    printf("\n---------------------\nSelect a save file to modify.\nY for save 1, B for save 2, X for save 3.\nPress START to exit.\n");
}

void printFusionSelect() {
    printf("\n---------------------\nEnable fusion mode for this save file?\n A for yes, B for no.\n");
}
