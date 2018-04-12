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
#ifndef _SAVEH_
#define _SAVEH_

#include <3ds.h>
#include <stdlib.h>
#include <stdio.h>
#include "struct.h"

Result open_archive(u32 lowid, InstallType install_type, FS_Archive* save_archive);
SavesList save_check(FS_Archive* save_archive, int* profile_num);
Result open_file(int profile_num, FS_Archive* save_archive, Handle* file_handle);
Result read_save(SaveStatus* sstate, Handle* file_handle);
Result buffer_store(char* buffer, Handle* file_handle, u64 file_size);
Result get_filesize(Handle* file_handle, u64* file_size);
Result save_and_close(Handle* file_handle, FS_Archive* save_archive, u32 lowid, InstallType install_type);
Result unlock_amiibo_content(Handle* file_handle, bool fusion_mode);
Result write_buffer(Handle* file_handle, char* buffer, u64 file_size);

#endif