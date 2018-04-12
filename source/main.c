// SR:AU - Samus Returns amiibo unlocker - Unlock amiibo gated content in SR.
// Copyright (C) 2018 Valentijn "ev1l0rd" V.
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.
#include <stdio.h>
#include <3ds.h>
#include "title.h"
#include "const.h"
#include "struct.h"
#include "save.h"
#include "main.h"

int main() 
{
    cfguInit();
    gfxInitDefault();
    PrintConsole topScreen, bottomScreenLeft, bottomScreenRight;
    consoleInit(GFX_TOP, &topScreen);
    consoleInit(GFX_BOTTOM, &bottomScreenLeft);
    consoleInit(GFX_BOTTOM, &bottomScreenRight);
    consoleSetWindow(&bottomScreenLeft, 0, 0, 20, 30);
    consoleSetWindow(&bottomScreenRight, 20, 0, 20, 30);
    consoleSelect(&topScreen);

    States state = MAIN_SCREEN;
    States previous_state = -1;
    Regions regions_found = {false, false, false, 0};
    int profile_num = 0;
    bool fusion_mode = false;
    bool not_busy = true;
    bool intro_unseen = true;
    bool save_unseen = true;
    bool fusion_unseen = true;
    bool exit_unseen = true;
    bool region_autochecked = false;
    FS_Archive save_archive;
    Result res;
    SavesList saves_list = {false, false, false, 0};
    InstallType install_type;
    SaveStatus sstate;
    u32 lowid = 0;
    Handle file_handle;
    char* main_string = "Metroid: SAMUS RETURNS amiibo unlocker v1.1a\n";

    while(aptMainLoop()) 
    {
        hidScanInput();
        u32 kDown = hidKeysDown();
        if (kDown & KEY_START && not_busy)
        {
            printf("Exiting\n");
            break;
        }

        if (kDown & KEY_L && not_busy)
        {
            profile_num = 0;
            fusion_mode = false;
            intro_unseen = true;
            save_unseen = true;
            fusion_unseen = true;
            region_autochecked = false;
            regions_found.total_regions = 0;
            saves_list.total_saves = 0;
            consoleSelect(&topScreen);
            consoleClear();
            consoleSelect(&bottomScreenRight);
            consoleClear();
            consoleSelect(&bottomScreenLeft);
            consoleClear();
            consoleSelect(&topScreen);
            if (state == MAIN_SCREEN)
                printf("%s\n\n", main_string); // Edge case in case someone presses L on the main screen since then the next block doesn't evaluate.
            state = MAIN_SCREEN;
        }

        if (previous_state != state && state != FAILURE)
        {
            consoleSelect(&topScreen);
            consoleClear();
            printf("%s\n\n", main_string);
            previous_state = state;
        }
        
        switch(state){
            case MAIN_SCREEN:
                if (intro_unseen)
                {
                    printf("Press A to continue.\nYou can press L at any time to start over.");
                    intro_unseen = false;
                }

                if (kDown & KEY_A) 
                {
                    state = VERSION_TO_EDIT;
                }
                break;

            case VERSION_TO_EDIT: // Determining region
                if (!region_autochecked)
                {
                    lowid = title_check(&regions_found, &install_type);
                    consoleSelect(&bottomScreenRight);
                    printf("Found a total of\n %i regions.\n\n", regions_found.total_regions);
                    consoleSelect(&topScreen);
                    if (regions_found.total_regions == 0)
                    {
                        printf("Unable to detect a copy of Samus Returns.\nMake sure you have the game installed or the game card inserted.\n"
                            "Press START to exit.\n");
                        state = FAILURE;
                        break;
                    }
                    else if (regions_found.total_regions == 1)
                    {
                        consoleSelect(&bottomScreenRight);
                        printf("Region/gamecard\nautodetected.\n\n");
                        consoleSelect(&topScreen);                        
                        state = SELECT_SAVE;
                    }
                    else
                    {
                        printf("Unable to automatically determine the region.\nPlease select the appropriate region.\n");
                        region_autochecked = true;
                        if(regions_found.JPN)
                            printf("JPN region detected. Press X to use this region.\n");
                        if(regions_found.USA)
                            printf("USA region detected. Press Y to use this region.\n");
                        if(regions_found.PAL)
                            printf("PAL region detected. Press A to use this region.\n");
                    }
                }
                else
                {
                    if (kDown & KEY_X && regions_found.JPN) // JPN region
                    {
                        lowid = JPN_LOWID;
                        consoleSelect(&bottomScreenRight);
                        printf("Region: JPN\n");
                        consoleSelect(&topScreen);
                        state = SELECT_SAVE;
                    }
                    if (kDown & KEY_Y && regions_found.USA) // USA region
                    {
                        lowid = USA_LOWID;
                        consoleSelect(&bottomScreenRight);
                        printf("Region: USA\n");
                        consoleSelect(&topScreen);
                        state = SELECT_SAVE;
                    }
                    if (kDown & KEY_A && regions_found.PAL) // PAL region
                    {
                        lowid = PAL_LOWID;
                        consoleSelect(&bottomScreenRight);
                        printf("Region: PAL\n");
                        consoleSelect(&topScreen);
                        state = SELECT_SAVE;
                    }
                }
                break;

            case SELECT_SAVE: // Save file selection
                if (save_unseen)
                {
                    res = open_archive(lowid, install_type, &save_archive);
                    if(R_FAILED(res)) 
                    {
                        fail_print(&res);
                        state = FAILURE;
                        break;
                    }
                    
                    saves_list = save_check(&save_archive, &profile_num);
                    switch (saves_list.total_saves)
                    {
                        case 0:
                            printf("Make sure you have save data made in Samus Returns.\nPress START to exit.");
                            state = FAILURE;
                            break;

                        case 1:
                            consoleSelect(&bottomScreenRight);
                            printf("Save autodetected.\n");
                            consoleSelect(&topScreen);
                            state = OPEN_SAVE;
                            break;

                        default:
                            printf("Multiple save data found. Please use the\nappropriate buttons to select a save file.\n");
                    }
                    if (saves_list.profile0)
                        printf("Press Y to select save 1.\n");
                    if (saves_list.profile1)
                        printf("Press B to select save 2.\n");
                    if (saves_list.profile2)
                        printf("Press X to select save 3.\n");
                    save_unseen = false;
                }
                else
                {
                    if (kDown & KEY_Y && saves_list.profile0) // Save 1 was chosen
                    {
                        profile_num = 0;
                        consoleSelect(&bottomScreenRight);
                        printf("Save 1 selected.\n\n");
                        consoleSelect(&topScreen);
                        state = OPEN_SAVE;
                    }
                    if (kDown & KEY_B && saves_list.profile1) // Save 2 was chosen
                    {
                        profile_num = 1;
                        consoleSelect(&bottomScreenRight);
                        printf("Save 2 selected.\n\n");
                        consoleSelect(&topScreen);
                        state = OPEN_SAVE;
                    }
                    if (kDown & KEY_X && saves_list.profile2) // Save 3 was chosen
                    {
                        profile_num = 2;
                        consoleSelect(&bottomScreenRight);
                        printf("Save 3 selected.\n\n");
                        consoleSelect(&topScreen);
                        state = OPEN_SAVE;
                    }
                }
                break;

            case OPEN_SAVE:
                res = open_file(profile_num, &save_archive, &file_handle);
                if(R_FAILED(res)) 
                {
                    fail_print(&res);
                    state = FAILURE;
                    break;
                }
                state = READ_SAVE;
                break;

            case READ_SAVE:
                read_save(&sstate, &file_handle);
                consoleSelect(&bottomScreenLeft);
                printf("Save status:\n"
                    "Energy tank: %d\n"
                    "Energy filled: %d\n"
                    "Missile tank: %d\n"
                    "Missile filled: %d\n"
                    "Aeion tank: %d\n"
                    "Aeion filled: %d\n"
                    "Amiibo unlocked: %d\n", 
                    sstate.energy_tank,
                    sstate.energy_filled,
                    sstate.missile_tank,
                    sstate.missile_filled,
                    sstate.aeion_tank,
                    sstate.aeion_filled,
                    sstate.amiibo_unlocked);
                consoleSelect(&topScreen);
                state = FUSION_OR_NOT;
                break;

            case FUSION_OR_NOT: // Enable fusion mode, yay or nay?
                if (fusion_unseen)
                {
                    printf("Enable fusion mode for this save file?\n A for yes, B for no.\n");
                    fusion_unseen = false;
                }
                else
                {
                    if (kDown & KEY_A)
                    {
                        fusion_mode = true;
                        consoleSelect(&bottomScreenRight);
                        printf("Enabled fusion mode.\n\n");
                        consoleSelect(&topScreen);
                        state = THE_WIZARD_IS_BUSY;
                    }
                    if (kDown & KEY_B)
                    {
                        consoleSelect(&bottomScreenRight);
                        printf("Not enabling fusion mode.\n\n");
                        consoleSelect(&topScreen);
                        state = THE_WIZARD_IS_BUSY;
                    }
                }
                break;

            case THE_WIZARD_IS_BUSY: // Here the editing happens
                not_busy = false;
                Result res = unlock_amiibo_content(&file_handle, fusion_mode);
                if(R_FAILED(res))
                {
                    fail_print(&res);
                    state = FAILURE;
                    break;
                }
                else
                {
                    res = save_and_close(&file_handle, &save_archive, lowid, install_type);
                    if(R_FAILED(res))
                    { 
                        fail_print(&res);
                        state = FAILURE;
                        break;
                    }
                }
                not_busy = true;
                state = SUCCESS;
                break;

            case SUCCESS:
                if (exit_unseen)
                {
                    printf("Amiibo's have been unlocked for the selected save file.\nPress START to exit.");
                    exit_unseen = false;
                }
                break;

            case FAILURE:
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

void fail_print(Result* res)
{
    printf("\x1b[s\x1b[30;16H%4lx\n\x1b[u", (unsigned long) *res);
    printf("Something went wrong, please see the error message below.\n");
}