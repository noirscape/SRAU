#ifndef _SAVEH_
#define _SAVEH_

#include <3ds.h>
#include <stdlib.h>
#include <stdio.h>
#include "struct.h"

Result open_archive(u32 lowid, InstallType install_type, FS_Archive* save_archive);
SavesList save_check(FS_Archive* save_archive, int* profile_num);
bool file_check(int i, FS_Archive* save_archive);

#endif