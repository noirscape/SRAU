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
    Regions regions_found;
    int profile_num = 0;
    bool fusion_mode = false;
    bool not_busy = true;
    bool save_unseen = true;
    bool fusion_unseen = true;
    bool region_autochecked = false;
    FS_Archive save_archive;
    Result res;
    SavesList saves_list;
    InstallType install_type;
    SaveStatus sstate;
    u32 lowid;
    Handle file_handle;    

    printf("Metroid: SAMUS RETURNS amiibo unlocker v1.1a\nPress A to continue.\n");

    while(aptMainLoop()) 
    {
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
                    state = VERSION_TO_EDIT;
                }
                break;

            case VERSION_TO_EDIT: // Determining region
                if (!region_autochecked)
                {
                    lowid = title_check(&regions_found, &install_type);
                    if (regions_found.total_regions == 0)
                    {
                        printf("Press START to exit.\n");
                        state = SUCCESS;
                    } 
                    else if (regions_found.total_regions == 1)
                    {
                        consoleSelect(&bottomScreenRight);
                        printf("Region/gamecard autodetected.\n");
                        consoleSelect(&topScreen);
                        state = SELECT_SAVE;
                    } 
                    else
                    {
                        region_autochecked = true;
                        // Insert code here to display stuff about region selection
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
                        state = SUCCESS;
                    }
                    
                    saves_list = save_check(&save_archive, &profile_num);
                    switch (saves_list.total_saves)
                    {
                        case 0:
                            printf("Make sure you have save data made in Samus Returns.\nPress START to exit.");
                            state = SUCCESS;
                            break;

                        case 1:
                            consoleSelect(&bottomScreenRight);
                            printf("Save autodetected.\n");
                            consoleSelect(&topScreen);
                            state = OPEN_SAVE;
                            break;

                        default:
                            printf("Multiple save data found. Please use the appropriate buttons to select a save file.\n");
                    }
                    if (saves_list.profile0)
                        printf("Press Y to select save 1.\n");
                    if (saves_list.profile1)
                        printf("Press B to select save 2.\n");
                    if (saves_list.profile2)
                        printf("Press X to select save 3.\n");
                    save_unseen = false;
                }
                if (kDown & KEY_Y && saves_list.profile0) // Save 1 was chosen
                {
                    profile_num = 0;
                    consoleSelect(&bottomScreenRight);
                    printf("Save 1 selected.\n");
                    consoleSelect(&topScreen);
                    state = OPEN_SAVE;
                }
                if (kDown & KEY_B && saves_list.profile1) // Save 2 was chosen
                {
                    profile_num = 1;
                    consoleSelect(&bottomScreenRight);
                    printf("Save 2 selected.\n");
                    consoleSelect(&topScreen);
                    state = OPEN_SAVE;
                }
                if (kDown & KEY_X && saves_list.profile2) // Save 3 was chosen
                {
                    profile_num = 2;
                    consoleSelect(&bottomScreenRight);
                    printf("Save 3 selected.\n");
                    consoleSelect(&topScreen);
                    state = OPEN_SAVE;
                }
                break;

            case OPEN_SAVE:
                res = open_file(profile_num, &save_archive, &file_handle);
                if(R_FAILED(res)) 
                {
                    fail_print(&res);
                    state = SUCCESS;
                }
                state = READ_SAVE;
                break;

            case READ_SAVE:
                read_save(profile_num, &sstate, &file_handle);
                consoleSelect(&bottomScreenLeft);
                printf("Save status:\n"
                    "Energy tank: %d:\n"
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
                    printf("\n---------------------\nEnable fusion mode for this save file?\n A for yes, B for no.\n");
                    fusion_unseen = false;
                }
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
                Result res = unlock_amiibo_content(&file_handle, fusion_mode);
                if(R_FAILED(res)) 
                {
                    fail_print(&res);
                }
                else
                {
                    res = save_and_close(&file_handle, &save_archive, lowid, install_type);
                    if(R_FAILED(res)) 
                    {
                        fail_print(&res);
                    }
                    else
                    {
                        printf("Amiibo's have been unlocked for the selected save file. Press START to exit.");                        
                    }
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

void fail_print(Result* res)
{
    printf("\x1b[s\x1b[30;16H%4lx\n\x1b[u", *res);
    printf("Something went wrong, please see the error message below.\n");
}