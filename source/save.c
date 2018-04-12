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
    buffer_store(buffer, file_handle, file_size);

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
Result buffer_store(char* buffer, Handle* file_handle, u64 file_size)
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

// Buffer is NOT freed here!
Result write_buffer(Handle* file_handle, char* buffer, u64 file_size)
{
    u32 bytes;
    Result res;
    res = FSFILE_Write(*file_handle, &bytes, 0x0, buffer, file_size, FS_WRITE_FLUSH | FS_WRITE_UPDATE_TIME);
    if(R_FAILED(res))
    {
        return res;
    }
}

Result save_and_close(Handle* file_handle, FS_Archive* save_archive, u32 lowid, InstallType install_type)
{
    Result res;
    FSFILE_Close(*file_handle);
    res = FSUSER_ControlArchive(*save_archive, ARCHIVE_ACTION_COMMIT_SAVE_DATA, NULL, 0, NULL, 0);
    if(R_FAILED(res))
    {
        FSUSER_CloseArchive(*save_archive);
        return res;
    }

    // Remove secure value if this is an SD card installation, otherwise, carry on.
    if (install_type == SD_CARD)
    {
        u64 in = ((u64)SECUREVALUE_SLOT_SD << 32) |  (((u64)lowid >> 8) << 8);
        u8 out;
        res = FSUSER_ControlSecureSave(SECURESAVE_ACTION_DELETE, &in, 8, &out, 1);
        if(R_FAILED(res))
        {
            FSUSER_CloseArchive(*save_archive);
            return res;
        }
    }

    // Close archive
    FSUSER_CloseArchive(*save_archive);
}

Result unlock_amiibo_content(Handle* file_handle, bool fusion_mode)
{
    u64 filesize;
    get_filesize(file_handle, &filesize);
    char* buffer = NULL;
    buffer = malloc(filesize);
    buffer_store(buffer, file_handle, filesize);
    if (fusion_mode)
        // Fusion mode unlock
        buffer[0x28] = 0x02;

    // Bonus energy tank unlock
    buffer[0x31] = 0x01;
    buffer[0x37] = 0x01;

    // Bonus aeion tank unlock
    buffer[0x3D] = 0x01;
    buffer[0x43] = 0x01;

    // Bonus missile tank unlock
    buffer[0x49] = 0x01;
    buffer[0x4F] = 0x01;

    // Amiibo scanning in-game unlock
    buffer[0x55] = 0x01;

    write_buffer(file_handle, buffer, filesize);
    free(buffer);
    return 0;
}