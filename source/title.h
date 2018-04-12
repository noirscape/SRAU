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
#ifndef _TITLEH_
#define _TITLEH_

#include <3ds.h>
#include <stdio.h>
#include <stdlib.h>
#include "title.h"
#include "const.h"
#include "struct.h"

u32 title_check(Regions *regions_found, InstallType *install_type);
int valid_title(u64 title_id, u32 *lowid, Regions *regions_found);

#endif