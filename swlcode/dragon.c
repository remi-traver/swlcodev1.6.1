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

ch_ret  one_hit             args( ( CHAR_DATA *ch, CHAR_DATA *victim, int dt ) );

void do_dbreath( CHAR_DATA *ch, char *argument )
{
	CHAR_DATA *victim;
    char arg1 [MAX_INPUT_LENGTH];
    int dam;
    
    if ( ch->race != RACE_DUINUOGWUIN )
    {
      send_to_char( "Only dragons can breathe fire Moron!!\n\r" , ch );
      return;
    }
        if ( get_age(ch) <= 40)
    {
      send_to_char( "You can't breathe deep enough yet\n\r" , ch );
      return;
    }

    argument = one_argument( argument, arg1 );

    if ( arg1[0] == '\0' )
    {
   send_to_char( "Breathe on whom?\n\r", ch );
	return;
    }

    if ( ( victim = get_char_room( ch, arg1 ) ) == NULL )
    {
      send_to_char( "They aren't here.\n\r", ch );
      return;
    }

    if ( victim == ch )
    {
      send_to_char( "That's pointless.\n\r", ch );
      return;
    }

    if ( IS_SET( ch->in_room->room_flags, ROOM_SAFE ) )
    {
      set_char_color( AT_MAGIC, ch );
      send_to_char( "This isn't a good place to do that.\n\r", ch );
      return;
    }

    if ( ch->position <= POS_SLEEPING )
    {
      send_to_char( "In your dreams or what?\n\r" , ch );
      return;
    }

    
    if ( ch->move < ( ( ch->max_move / 10 ) + 1) )
    {
      send_to_char( "You're too tired to breathe hard enough!\n\r" , ch );
      return;
    }

    WAIT_STATE( ch,( skill_table[gsn_kick]->beats *3 ));
    dam = (ch->hit / 2.5) ;
    dam = ( dam * ( ( number_percent() * 2 ) / 100 ) ) - ( ch->hit / 8 );
    ch->move = ch->move - ( ch->max_move / 10 );
    act( AT_RED, "You take a deep breath, and breathe fire all over $N!!!\n\r", ch, NULL, victim, TO_CHAR );
    act( AT_RED, "$n takes a deep breath, and breathe fire all over you!!!\n\r", ch, NULL, victim, TO_VICT    );
    act( AT_RED, "$n takes a deep breath and spews forth intense flames at $N!!!\n\r",  ch, NULL, victim, TO_NOTVICT );
    noparry_damage( ch, victim, dam, gsn_fireball );
    return;
}


void do_slash( CHAR_DATA *ch, char *argument )
{
	CHAR_DATA *victim;
    char arg1 [MAX_INPUT_LENGTH];
    int dam;

    if ( ch->race != RACE_DUINUOGWUIN )
    {
      send_to_char( "Only dragons can slash with claws Moron!!\n\r" , ch );
      return;
    }
    
    if (get_age(ch) <= 30 )
    {
      send_to_char( "Your claws haven't hardened enough yet\n\r" , ch );
      return;
    }


    argument = one_argument( argument, arg1 );

    if ( arg1[0] == '\0' )
    {
   send_to_char( "Slash whom?\n\r", ch );
	return;
    }

    if ( ( victim = get_char_room( ch, arg1 ) ) == NULL )
    {
      send_to_char( "They aren't here.\n\r", ch );
      return;
    }

    if ( victim == ch )
    {
      send_to_char( "That's pointless.\n\r", ch );
      return;
    }

    if ( IS_SET( ch->in_room->room_flags, ROOM_SAFE ) )
    {
      set_char_color( AT_MAGIC, ch );
      send_to_char( "This isn't a good place to do that.\n\r", ch );
      return;
    }

    if ( ch->position <= POS_SLEEPING )
    {
      send_to_char( "In your dreams or what?\n\r" , ch );
      return;
    }

    
    if ( ch->move < ( ( ch->max_move / 30 ) + 1) )
    {
      send_to_char( "You're too tired to swing hard enough!\n\r" , ch );
      return;
    }

    WAIT_STATE( ch,( skill_table[gsn_kick]->beats));

    dam = ( ch->hit / 4);
    dam = ( dam * ( ( number_percent() * 2 ) / 100 ) ) - ( ch->hit / 8 );
    ch->move = ch->move - ( ch->max_move / 30 );
    act( AT_CYAN, "You rake $N with your claws!!!!\n\r", ch, NULL, victim, TO_CHAR );
    act( AT_CYAN,"$n slashes you with their claws!!!!!\n\r", ch, NULL, victim, TO_VICT    );
    act( AT_CYAN,"$n rakes $N with their claws!!!!!\n\r",  ch, NULL, victim, TO_NOTVICT );
    noparry_damage( ch, victim, dam, TYPE_UNDEFINED );
    return;
}

