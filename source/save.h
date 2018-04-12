#ifndef _SAVEH_
#define _SAVEH_

#include <3ds.h>
#include <stdlib.h>
#include <stdio.h>
#include "struct.h"

Result open_archive(u32 lowid, InstallType install_type, FS_Archive* save_archive);
SavesList save_check(FS_Archive* save_archive, int* profile_num);
Result open_file(int profile_num, FS_Archive* save_archive, Handle* file_handle);
Result read_save(int profile_num, SaveStatus* sstate, Handle* file_handle);
Result buffer_store(char* buffer, Handle* file_handle, u64 file_size);
Result get_filesize(Handle* file_handle, u64* file_size);
Result save_and_close(Handle* file_handle, FS_Archive* save_archive, u32 lowid, InstallType install_type);
Result unlock_amiibo_content(Handle* file_handle, bool fusion_mode);
Result write_buffer(Handle* file_handle, char* buffer, u64 file_size);

#endif