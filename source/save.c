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
    Result res;
    SavesList saves_list = {false, false, false};

    for (int i = 0; i < 3; ++i)
    {
        bool exist = file_check(i, save_archive);
        if (exist)
        {
            saves_list.total_saves++;
            *profile_num = i;
        }
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

bool file_check(int i, FS_Archive* save_archive)
{
    char profile_path[0x107] = {0};
    sprintf(profile_path, "/profile%i/pkprfl.bmssv", i);
    printf("%s\n", profile_path);

    // Open file
    Handle file_handle;
    
    Result res;
    res = FSUSER_OpenFile(&file_handle, *save_archive, fsMakePath(PATH_ASCII, profile_path), FS_OPEN_WRITE | FS_OPEN_READ, 0);
    if(R_FAILED(res))
    {
        return false;
    }
    FSFILE_Close(file_handle);
    return true;
}

Result open_file()
{

}
