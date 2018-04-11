#include <3ds.h>
#include <stdlib.h>
#include <stdio.h>
#include "struct.h"
#include "save.h"

Result open_archive(u32 lowid, InstallType install_type, FS_Archive* save_archive)
{
    FS_MediaType media_type = (install_type == SD_CARD) ? MEDIATYPE_SD : MEDIATYPE_GAME_CARD;
    const u32 path[3] = { media_type, lowid, 0x00040000 };

    FS_ArchiveID archive_id = (install_type == SD_CARD) ? ARCHIVE_USER_SAVEDATA : ARCHIVE_GAMECARD_SAVEDATA;

    Result res;
    res = FSUSER_OpenArchive(save_archive, archive_id, (FS_Path){PATH_BINARY, 12, path});
    if(R_FAILED(res)) return res;

    return 0;
}

SavesList save_check(FS_Archive* save_archive, int* profile_num)
{
    bool exist;
    Result res;
    SavesList saves_list = {false, false, false};

    for (int i = 0; i < 3; ++i)
    {
        Handle file_handle;
        res = open_file(i, save_archive, &file_handle);
        if (!R_FAILED(res))
        {
            saves_list.total_saves++;
            *profile_num = i;
            exist = true;
            FSFILE_Close(file_handle);
        }
        else
            exist = false;

        switch(i)
        {
            case 0:
                saves_list.profile0 = exist;
                break;

            case 1:
                saves_list.profile1 = exist;
                break;

            case 2:
                saves_list.profile2 = exist;
                break;
        }
    }

    return saves_list;
}

Result open_file(int profile_num, FS_Archive* save_archive, Handle* file_handle)
{
    char profile_path[0x107] = {0};
    sprintf(profile_path, "/profile%i/pkprfl.bmssv", profile_num);
    printf("%s\n", profile_path);

    // Open file
    Result res;
    res = FSUSER_OpenFile(file_handle, *save_archive, fsMakePath(PATH_ASCII, profile_path), FS_OPEN_WRITE | FS_OPEN_READ, 0);
    if(R_FAILED(res))
    {
        return res;
    }
    return 0;
}

Result read_save(int profile_num, SaveStatus* sstate, Handle* file_handle)
{
    u64 file_size;
    get_filesize(file_handle, &file_size);
    char* buffer = NULL;
    buffer = malloc(file_size);
    buffer_save(buffer, file_handle, file_size);

    // Difficulty
    switch(buffer[0x28])
    {
        case 0x02:
            sstate->difficulty = 2;
            break;

        case 0x01:
            sstate->difficulty = 1;
            break;

        case 0x00:
            sstate->difficulty = 0;
            break;
    }

    // Energy tank
    switch(buffer[0x31])
    {
        case 0x01:
            sstate->energy_tank = true;

        case 0x00:
            sstate->energy_tank = false;
    }

    switch(buffer[0x37])
    {
        case 0x01:
            sstate->energy_filled = true;

        case 0x00:
            sstate->energy_filled = false;
    }

    // Aeion tank unlock
    switch(buffer[0x3D])
    {
        case 0x01:
            sstate->aeion_tank = true;

        case 0x00:
            sstate->aeion_tank = false;            
    }

    switch(buffer[0x43])
    {
        case 0x01:
            sstate->aeion_filled = true;

        case 0x00:
            sstate->aeion_filled = false;            
    }

    // Bonus missile tank unlock
    switch(buffer[0x49])
    {
        case 0x01:
            sstate->missile_tank = true;

        case 0x00:
            sstate->missile_tank = false;            
    }

    switch(buffer[0x4F])
    {
        case 0x01:
            sstate->missile_filled = true;

        case 0x00:
            sstate->missile_filled = false;            
    }

    // Amiibo scanning in-game
    switch(buffer[0x55])
    {
        case 0x01:
            sstate->amiibo_unlocked = true;

        case 0x00:
            sstate->amiibo_unlocked = false;            
    }

    free(buffer);
    return 0;
}

// NOTE: THIS FUNCTION DOES _NOT_ FREE UP (UNLESS IT FAILS) OR ALLOCATE A BUFFER. DO THAT YOURSELF.
Result buffer_save(char* buffer, Handle* file_handle, u64 file_size)
{
    Result res;
    u32 bytes;
    res = FSFILE_Read(*file_handle, &bytes, 0x0, buffer, file_size);
    if(R_FAILED(res))
    {
        free(buffer);
        return res;
    }
    return 0;
}

Result get_filesize(Handle* file_handle, u64* file_size)
{
    Result res;
    // Get filesize
    res = FSFILE_GetSize(*file_handle, file_size);
    if(R_FAILED(res))
    {
        return res;
    }
    return 0;
}