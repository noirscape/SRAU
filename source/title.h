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