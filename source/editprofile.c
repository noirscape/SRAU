#include <3ds.h>
#include <stdio.h>
#include <stdlib.h>
#include "struct.h"

Result edit_profile(int profile_num, bool fusion_mode, u32 lowid, InstallType install_type)
{
    Result res;
    char* buffer;
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

    // Remove secure value if this is an SD card installation, otherwise, carry on.
    if (install_type == SD_CARD)
    {
        u64 in = ((u64)SECUREVALUE_SLOT_SD << 32) |  (((u64)lowid >> 8) << 8);
        u8 out;
        res = FSUSER_ControlSecureSave(SECURESAVE_ACTION_DELETE, &in, 8, &out, 1);
        if(R_FAILED(res))
        {
            FSUSER_CloseArchive(save_archive);
            printf("Couldn't remove secure value.\n");
            return res;
        }
    }        

    // Close archive
    FSUSER_CloseArchive(save_archive);

    return 0;
}