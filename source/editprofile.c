#include <3ds.h>
#include <stdio.h>
#include <stdlib.h>

Result edit_profile(int profile_num, bool fusion_mode, u32 lowid)
{

    // Determine region for the TID
    u8 region_code;

    Result res = CFGU_SecureInfoGetRegion(&region_code);
    if(R_FAILED(res)) return res;

    u32 highid = 0x00040000;

    // Open save archive
    const u32 path[3] = { MEDIATYPE_SD, lowid, highid };
    FS_Archive save_archive;
    
    res = FSUSER_OpenArchive(&save_archive, ARCHIVE_USER_SAVEDATA, (FS_Path){PATH_BINARY, 12, path});
    if(R_FAILED(res)) return res;

    char profile_path[0x107] = {0};
    sprintf(profile_path, "/profile%i/pkprfl.bmssv", profile_num);
    printf("%s\n", profile_path);

    // Open file
    Handle file_handle;
    
    res = FSUSER_OpenFile(&file_handle, save_archive, fsMakePath(PATH_ASCII, profile_path), FS_OPEN_WRITE | FS_OPEN_READ, 0);
    if(R_FAILED(res))
    {
        FSUSER_CloseArchive(save_archive);
        return res;
    }
    
    // Get filesize
    u64 file_size;
    res = FSFILE_GetSize(file_handle, &file_size);
    if(R_FAILED(res))
    {
        FSFILE_Close(file_handle);
        FSUSER_CloseArchive(save_archive);
        return res;
    }

    // Buffer file to memory
    char* buffer = NULL;
    buffer = malloc(file_size);

    u32 bytes;
    res = FSFILE_Read(file_handle, &bytes, 0x0, buffer, file_size);
    if(R_FAILED(res))
    {
        free(buffer);
        FSFILE_Close(file_handle);
        FSUSER_CloseArchive(save_archive);
        return res;
    }
    printf("Succesfully read save data into memory.\n");

    // Editing time
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

    printf("Succesfully edited values in memory. Writing to disk now.\n");

    // Write buffer
    res = FSFILE_Write(file_handle, &bytes, 0x0, buffer, file_size, FS_WRITE_FLUSH | FS_WRITE_UPDATE_TIME);
    if(R_FAILED(res))
    {
        free(buffer);
        FSFILE_Close(file_handle);
        FSUSER_CloseArchive(save_archive);
        return res;
    }
    free(buffer);

    // Commit save data/close file
    FSFILE_Close(file_handle);
    res = FSUSER_ControlArchive(save_archive, ARCHIVE_ACTION_COMMIT_SAVE_DATA, NULL, 0, NULL, 0);
    if(R_FAILED(res))
    {
        FSUSER_CloseArchive(save_archive);
        printf("Couldn't update save archive.\n");
        return res;
    }

    // Remove secure value
    u64 in = ((u64)SECUREVALUE_SLOT_SD << 32) |  (((u64)lowid >> 8) << 8);
    u8 out;
    res = FSUSER_ControlSecureSave(SECURESAVE_ACTION_DELETE, &in, 8, &out, 1);
    if(R_FAILED(res))
    {
        FSUSER_CloseArchive(save_archive);
        printf("Couldn't remove secure value.\n");
        return res;
    }

    // Close archive
    FSUSER_CloseArchive(save_archive);

    return 0;
}