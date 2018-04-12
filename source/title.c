#include <3ds.h>
#include <stdio.h>
#include <stdlib.h>
#include "title.h"
#include "const.h"
#include "struct.h"

/*
* title_check: check for the correct lowid
* arg: 
*  Regions *regions_found pointer to output found regions to.
*  FS_MediaType *media_type pointer to output the detected media_type to.
* returns: 0 or lowid
*/
u32 title_check(Regions *regions_found, InstallType *install_type)
{
    u64 title_id; // This is for checking the title ID
    u32 lowid = 0; // This is to be returned at the end.

    u32 titles_read;
    u32 title_count;

    // Start by checking for the inserted card.
    bool card_inserted;
    FSUSER_CardSlotIsInserted(&card_inserted);
    amInit();
    if (card_inserted)
    {
        // Card inserted, let's verify it's a CTR (3DS) card and not a TWL (DS) card.
        FS_CardType card_type;
        FSUSER_GetCardType(&card_type);
        if (card_type == CARD_CTR) 
        {
            // Yay! It's a CTR card, let's check the title.
            AM_GetTitleList(&titles_read, MEDIATYPE_GAME_CARD, 1, &title_id);
            int title_valid = valid_title(title_id, &lowid, regions_found);
            if (title_valid)
            {
                // Title valid? No point in continuing, lets give the lowid back.
                *install_type = GAME_CARD;
                return lowid;
            }
        }
    }

    // No/Invalid card found.
    u64* sd_titles;
    AM_GetTitleCount(MEDIATYPE_SD, &title_count);
    sd_titles = malloc(title_count * sizeof(u64));
    AM_GetTitleList(&titles_read, MEDIATYPE_SD, title_count, sd_titles);
    for (unsigned int i = 0; i < title_count; ++i)
    {
        valid_title(sd_titles[i], &lowid, regions_found);
    }
    free(sd_titles);

    if (regions_found->total_regions > 1)
    {
        lowid = 0;
    }

    amExit();
    *install_type = SD_CARD;
    return lowid;
}

/*
* valid_title: Checks if the title is valid
* args: 
*  title_id = the title id to check
*  *lowid = pointer to the lowid to set
*  *regions_found = pointer to store found regions in
*/
int valid_title(u64 title_id, u32 *lowid, Regions *regions_found)
{
    switch(title_id)
    {
        case PAL_TITLEID:
            *lowid = PAL_LOWID;
            regions_found->PAL = true;
            regions_found->total_regions++;
            return 1;

        case USA_TITLEID:
            *lowid = USA_LOWID;
            regions_found->USA = true;
            regions_found->total_regions++;
            return 1;

        case JPN_TITLEID:
            *lowid = JPN_LOWID;
            regions_found->JPN = true;
            regions_found->total_regions++;
            return 1;

        default:
            return 0;
    }
}