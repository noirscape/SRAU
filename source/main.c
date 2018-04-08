#include <stdio.h>
#include <3ds.h>
#include "editprofile.h"
#include "main.h"

typedef enum {
    /* Default sort but its commented to make it easy on _my_ mind. */
    MAIN_SCREEN = 0,
    SELECT_SAVE = 1,
    FUSION_OR_NOT = 2,
    THE_WIZARD_IS_BUSY = 3,
    SUCCESS = 4
}States;

int main() {
    cfguInit();
    gfxInitDefault();
    PrintConsole topScreen, bottomScreen;
    consoleInit(GFX_TOP, &topScreen);
    consoleInit(GFX_BOTTOM, &bottomScreen);
    consoleSelect(&topScreen);

    States state = MAIN_SCREEN;
    int profile_num;
    int fusion_mode = 0;
    int not_busy = 1;

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
                state = SELECT_SAVE;
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
                fusion_mode = 1;
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
            not_busy = 0;
            Result res = edit_profile(profile_num, fusion_mode);
            if(R_FAILED(res)) {
                int cursorX = topScreen.cursorX;
                int cursorY = topScreen.cursorY;
                printf("\x1b[30;16H%4lx\n", res);
                topScreen.cursorX = cursorX;
                topScreen.cursorY = cursorY;
                printf("Something went wrong, please see the error message below.\n");
            } else {
                printf("Amiibo's have been unlocked for the selected save file. Press START to exit.");
            }
            not_busy = 1;
            state = SUCCESS;
        }
        // State 4 is technically success, but it just means it isn't listening to anything anymore and waits for closing.

        gfxFlushBuffers();
        gfxSwapBuffers();
        gspWaitForVBlank();
    }

    cfguExit();
    fsExit();
    gfxExit();
    return 0;
}

void printSaveSelect() {
    printf("\n---------------------\nSelect a save file to modify.\nY for save 1, B for save 2, X for save 3.\nPress START to exit.\n");
}

void printFusionSelect() {
    printf("\n---------------------\nEnable fusion mode for this save file?\n A for yes, B for no.\n");
}
