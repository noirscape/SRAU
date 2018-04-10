#include <3ds.h>
#include <stdio.h>
#include <stdlib.h>
#include "title.h"

/*
* title_check
* returns lowid if it can be automagically determined. otherwise returns 0
*/
u32 title_check()
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
        if (card_type == CARD_CTR) {
            // Yay! It's a CTR card, let's check the title.
            AM_GetTitleList(&titles_read, MEDIATYPE_GAME_CARD, 1, &title_id);
            int title_valid = valid_title(title_id, &lowid);
            if (title_valid)
            {
                printf("Located a game card copy of SR, using that.\n");
                // Title valid? No point in continuing, lets give the lowid back.
                return lowid;
            }
        }
    }

    // No/Invalid card found.
    u64* sd_titles;
    int title_valid;
    AM_GetTitleCount(MEDIATYPE_SD, &title_count);
    sd_titles = malloc(title_count * sizeof(u64));
    AM_GetTitleList(&titles_read, MEDIATYPE_SD, title_count, sd_titles);
    for (unsigned int i = 0; i < title_count; ++i)
    {
        title_valid = valid_title(sd_titles[i], &lowid);
        if (title_valid) {
            printf("Located game copy with lowid: %ld\n", lowid);
            break;
        }
    }

    amExit();
    return 0;
}

int valid_title(u64 title_id, u32 *lowid)
{
    switch(title_id)
    {
        case 0x00040000001BB200:
            *lowid = 0x001BB200;
            return 1;

        case 0x00040000001BFB00:
            *lowid = 0x001BFB00;
            return 1;

        case 0x00040000001BFC00:
            *lowid = 0x001BFC00;
            return 1;

        default:
            return 0;
    }
}