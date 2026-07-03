/***************************************************************************

Star Wars Life 3.0a
Dragon Skills and other Racial Shit

For credits please see 'help credits' in game.

(c) FRPG Ltd; 2001, 2002
All rights reserved
****************************************************************************/

#include <math.h> 
#include <sys/types.h>
#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include "mud.h"

/* Written or installed by Raijen, force rank removed by Arcturus */

void gain_honour( CHAR_DATA *ch, int hincrease )
{
    ch->pcdata->status += hincrease;
}

void loose_honour( CHAR_DATA *ch, int hdecrease )
{
    ch->pcdata->status -= hdecrease;
}

