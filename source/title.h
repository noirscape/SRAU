#ifndef _TITLEH_
#define _TITLEH_

#include <3ds.h>
#include <stdio.h>
#include "struct.h"
#include "const.h"

u32 title_check(Regions *regions_found);
int valid_title(u64 title_id, u32 *lowid, Regions *regions_found);

#endif