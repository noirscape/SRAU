#include <stdio.h>
#include <3ds.h>
#include "editprofile.h"
#include "main.h"
#include "title.h"
#include "struct.h"

typedef enum {
    /* Default sort but its commented to make it easy on _my_ mind. */
    MAIN_SCREEN,
    VERSION_TO_EDIT,
    SELECT_SAVE,
    FUSION_OR_NOT,
    THE_WIZARD_IS_BUSY,
    SUCCESS
}States;

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

    printf("Metroid: SAMUS RETURNS amiibo unlocker v0.2-alpha\nThis unlocker is in ALPHA. Bugs and stuff are expected.\nPress A to continue.\n");

    while(aptMainLoop()) {
        hidScanInput();
        u32 kDown = hidKeysDown();
        if (kDown & KEY_START && not_busy) 
        {
            printf("Exiting\n");
            break;
        }

        if (state == MAIN_SCREEN) // State 0 = introduction
        {
            if (kDown & KEY_A) 
            {
                printSaveSelect();
                state = VERSION_TO_EDIT;
            }
        }

        if (state == VERSION_TO_EDIT)
        {
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
                    // Insert code here to display stuff about region selection
                }
            }
        }

        if (state == SELECT_SAVE) // State 1 = Save select
        {
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
        }
        if (state == FUSION_OR_NOT) // State 2 = Fusion mode, yay or nay
        {
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
        }
        if (state == THE_WIZARD_IS_BUSY) // The wizard is busy installing your software, don't touch anything.
        {
            not_busy = false;
            Result res = edit_profile(profile_num, fusion_mode);
            if(R_FAILED(res)) {
                int cursorX = topScreen.cursorX;
                int cursorY = topScreen.cursorY;
                printf("\x1b[s\x1b[30;16H%4lx\n\x1b[u", res);
                topScreen.cursorX = cursorX;
                topScreen.cursorY = cursorY;
                printf("Something went wrong, please see the error message below.\n");
            } else {
                printf("Amiibo's have been unlocked for the selected save file. Press START to exit.");
            }
            not_busy = true;
            state = SUCCESS;
        }
        // State 4 is technically success, but it just means it isn't listening to anything anymore and waits for closing.

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
