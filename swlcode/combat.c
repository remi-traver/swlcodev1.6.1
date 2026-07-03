/***************************************************************************
*                           STAR WARS REALITY 1.0                          *
*--------------------------------------------------------------------------*
* Star Wars Reality Code Additions and changes from the Smaug Code         *
* copyright (c) 1997 by Sean Cooper                                        *
* -------------------------------------------------------------------------*
* Starwars and Starwars Names copyright(c) Lucasfilm Ltd.                  *
*--------------------------------------------------------------------------*
* SMAUG 1.0 (C) 1994, 1995, 1996 by Derek Snider                           *
* SMAUG code team: Thoric, Altrag, Blodkai, Narn, Haus,                    *
* Scryn, Rennard, Swordbearer, Gorog, Grishnakh and Tricops                *
* ------------------------------------------------------------------------ *
* Merc 2.1 Diku Mud improvments copyright (C) 1992, 1993 by Michael        *
* Chastain, Michael Quan, and Mitchell Tse.                                *
* Original Diku Mud copyright (C) 1990, 1991 by Sebastian Hammer,          *
* Michael Seifert, Hans Henrik St{rfeldt, Tom Madsen, and Katja Nyboe.     *
* ------------------------------------------------------------------------ *
*		   New Star Wars Skills Unit    			   *   
****************************************************************************/

#include <math.h> 
#include <sys/types.h>
#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include "mud.h"

ch_ret  one_hit             args( ( CHAR_DATA *ch, CHAR_DATA *victim, int dt ) );
ROOM_INDEX_DATA *generate_exit( ROOM_INDEX_DATA *in_room, EXIT_DATA **pexit );
CHAR_DATA *get_char_room_mp( CHAR_DATA *ch, char *argument );
ch_ret noparry_damage( CHAR_DATA *ch, CHAR_DATA *victim, int dam, int dt);

extern int      top_affect;

void do_mine( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    char logbuf[MAX_INPUT_LENGTH];
    OBJ_DATA *obj;
    bool shovel;
    sh_int move;

   if ( ch->pcdata->learned[gsn_mine] <= 0 )
    {
      ch_printf( ch, "You have no idea how to do that.\n\r" );
      return;
    }

    one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {    
        send_to_char( "And what will you mine the room with?\n\r", ch );
        return;
    }
    
    if ( ms_find_obj(ch) )
        return;
 
    shovel = FALSE;
    for ( obj = ch->first_carrying; obj; obj = obj->next_content )
      if ( obj->item_type == ITEM_SHOVEL )
      {
	  shovel = TRUE;
	  break;
      }

    obj = get_obj_list_rev( ch, arg, ch->in_room->last_content );
    if ( !obj )
    {
        send_to_char( "You don't see on here.\n\r", ch );
        return;
    }

    separate_obj(obj);
    if ( obj->item_type != ITEM_LANDMINE )
    {
	act( AT_PLAIN, "That's not a landmine!", ch, obj, 0, TO_CHAR );
        return;
    }

    if ( !CAN_WEAR(obj, ITEM_TAKE) )
    {
	act( AT_PLAIN, "You cannot bury $p.", ch, obj, 0, TO_CHAR );
        return;
    }
  
    switch( ch->in_room->sector_type )
    {
	case SECT_INSIDE:
	    send_to_char( "The floor is too hard to dig through.\n\r", ch );
	    return;
	case SECT_WATER_SWIM:
	case SECT_WATER_NOSWIM:
	case SECT_UNDERWATER:
	    send_to_char( "You cannot bury a mine in the water.\n\r", ch );
	    return;
	case SECT_AIR:
	    send_to_char( "What?  Bury a mine in the air?!\n\r", ch );
	    return;
    }

    if ( obj->weight > (UMAX(5, (can_carry_w(ch) / 10)))
    &&  !shovel )
    {
	send_to_char( "You'd need a shovel to bury something that big.\n\r", ch );
	return;
    }
    
    move = (obj->weight * 50 * (shovel ? 1 : 5)) / UMAX(1, can_carry_w(ch));
    move = URANGE( 2, move, 1000 );
    if ( move > ch->move )
    {
	send_to_char( "You don't have the energy to bury something of that size.\n\r", ch );
	return;
    }
    ch->move -= move;
    
    SET_BIT( obj->extra_flags, ITEM_BURRIED );
    WAIT_STATE( ch, URANGE( 10, move / 2, 100 ) );
       
    STRFREE ( obj->armed_by ); 
    obj->armed_by = STRALLOC ( ch->name );

    ch_printf( ch, "You arm and bury %s.\n\r", obj->short_descr );
    act( AT_PLAIN, "$n arms and buries $p.", ch, obj, NULL, TO_ROOM );


    sprintf ( logbuf, "%s has laid a mine in room %d", ch->name, ch->in_room->vnum );
    log_string (logbuf);
    learn_from_success( ch, gsn_mine );
    
    return;
}

void do_snipe( CHAR_DATA *ch, char *argument )
{
   OBJ_DATA        * wield;
   char              arg[MAX_INPUT_LENGTH];  
   char              arg2[MAX_INPUT_LENGTH];
   sh_int            dir, dist;
   sh_int            max_dist = 3;
   EXIT_DATA       * pexit;
   ROOM_INDEX_DATA * was_in_room;
   ROOM_INDEX_DATA * to_room;
   CHAR_DATA       * victim;
   int               chance;
   ch_ret	     dummy;
   char              buf[MAX_STRING_LENGTH];
   bool              pfound = FALSE;
   if(!IS_NPC(ch))
   {
      if(IS_SET(ch->pcdata->flags, PCFLAG_BOUND))
      {
         send_to_char("Not when you are bound up.\n\r", ch);
         return;
      }
   }

   if ( ch->subclass == SUBCLASS_SNIPER)
	max_dist = 10;
   
   if ( IS_SET( ch->in_room->room_flags, ROOM_SAFE ) )
    {
	set_char_color( AT_MAGIC, ch );
	send_to_char( "You'll have to do that elswhere.\n\r", ch );
	return;
    }
     
   if ( get_eq_char( ch, WEAR_DUAL_WIELD ) != NULL )
   {
         send_to_char( "You can't do that while wielding two weapons.",ch );
         return;
   }
    
   wield = get_eq_char( ch, WEAR_WIELD );
   if ( !wield || wield->item_type != ITEM_WEAPON || (
   wield->value[3] != WEAPON_BLASTER && wield->value[3] != WEAPON_SLUGTHROWER && wield->value[3] != WEAPON_BOWCASTER ))
   {
         send_to_char( "You don't seem to be holding a projectile weapon.",ch );
         return;
   }  

   argument = one_argument( argument, arg );
   argument = one_argument( argument, arg2 );
   
   if ( ( dir = get_door( arg ) ) == -1 || arg2[0] == '\0' )
   {
     send_to_char( "Usage: snipe <dir> <target>\n\r", ch );
     return;
   }
 
   if ( ( pexit = get_exit( ch->in_room, dir ) ) == NULL )
   {
     send_to_char( "Are you expecting to fire through a wall!?\n\r", ch );
     return;
   }

   if ( IS_SET( pexit->exit_info, EX_CLOSED ) )
   {
     send_to_char( "Are you expecting to fire through a door!?\n\r", ch );
     return;
   }

   was_in_room = ch->in_room;
   
   for ( dist = 0; dist <= max_dist; dist++ )   
   {
     if ( IS_SET( pexit->exit_info, EX_CLOSED ) )
        break; 
     
     if ( !pexit->to_room )
        break;
     
    to_room = NULL;
    if ( pexit->distance > 1 )
       to_room = generate_exit( ch->in_room , &pexit );
    
    if ( to_room == NULL )
       to_room = pexit->to_room;
    
     char_from_room( ch );
     char_to_room( ch, to_room );    
     

     if ( IS_NPC(ch) && ( victim = get_char_room_mp( ch, arg2 ) ) != NULL )
     {
        pfound = TRUE;
        break;
     }
     else if ( !IS_NPC(ch) && ( victim = get_char_room( ch, arg2 ) ) != NULL )
     {
        pfound = TRUE;
        break;
     }


     if ( ( pexit = get_exit( ch->in_room, dir ) ) == NULL )
        break;
            
   }
   
   char_from_room( ch );
   char_to_room( ch, was_in_room );    
       
   if ( !pfound )
   {
       ch_printf( ch, "You don't see that person to the %s!\n\r", dir_name[dir] );
       char_from_room( ch );
       char_to_room( ch, was_in_room );    
       return;
   }
   
    if ( victim == ch )
    {
	send_to_char( "Shoot yourself ... really?\n\r", ch );
	return;
    }
    
    if ( IS_SET( victim->in_room->room_flags, ROOM_SAFE ) )
    {
	set_char_color( AT_MAGIC, ch );
	send_to_char( "You can't shoot them there.\n\r", ch );
	return;
    }
 
    if ( is_safe( ch, victim ) )
	return;
    
    if ( IS_AFFECTED(ch, AFF_CHARM) && ch->master == victim )
    {
        act( AT_PLAIN, "$N is your beloved master.", ch, NULL, victim, TO_CHAR );
	return;
    }

    if ( ch->position == POS_FIGHTING )
    {
	send_to_char( "You do the best you can!\n\r", ch );
	return;
    }
    
    if ( !IS_NPC( victim ) && IS_SET( ch->act, PLR_NICE ) )
    {
      send_to_char( "You feel too nice to do that!\n\r", ch );
      return;
    }
    
    chance = IS_NPC(ch) ? 100
           : (int)  (ch->pcdata->learned[gsn_snipe]) ; 
    
    switch ( dir )
    {
        case 0:
        case 1:
           dir += 2;
           break;
        case 2:
        case 3:
           dir -= 2;
           break;
        case 4:
        case 7:
           dir += 1;
           break;
        case 5:
        case 8:
           dir -= 1;
           break;
        case 6:
           dir += 3;
           break;
        case 9:
           dir -=3;
           break;
    }
    
    char_from_room( ch );
    char_to_room( ch, victim->in_room );    
                
    if ( number_percent() < chance )
    {                         
	if (ch->subclass != SUBCLASS_SNIPER )
{
       sprintf( buf , "A blaster shot fires at you from the %s." , dir_name[dir] );
       act( AT_ACTION, buf , victim, NULL, ch, TO_CHAR );      
       act( AT_ACTION, "You fire at $N.", ch, NULL, victim, TO_CHAR );         
       sprintf( buf, "A blaster shot fires at $N from the %s." , dir_name[dir] );
       act( AT_ACTION, buf, ch, NULL, victim, TO_NOTVICT );  
                                                       
       one_hit( ch, victim, TYPE_UNDEFINED );  
 } else
{
	if (ch->sniperweapon != 0 && number_percent() <= 5)
	{
		act( AT_ACTION, "You Whisper, one Shot, One Kill as you line $N in your scope.",ch,NULL,victim,TO_CHAR);
		if (!IS_IMMORTAL(victim))
			raw_kill(ch, victim);
		else
			dummy = noparry_damage (ch, victim, 32700, TYPE_HIT + wield->value[3]);
                char_from_room( ch );
                char_to_room( ch, was_in_room );
		return;
	}

       sprintf( buf , "A flurry of shots fire at you from the %s." , dir_name[dir] );
       act( AT_ACTION, buf , victim, NULL, ch, TO_CHAR );      
       act( AT_ACTION, "You squeeze three shots off at $N.", ch, NULL, victim, TO_CHAR );         
       sprintf( buf, "A trio of shots fires at $N from the %s." , dir_name[dir] );
       act( AT_ACTION, buf, ch, NULL, victim, TO_NOTVICT );  
                                                   
       one_hit( ch, victim, TYPE_UNDEFINED );  
       one_hit( ch, victim, TYPE_UNDEFINED );  
       one_hit( ch, victim, TYPE_UNDEFINED );  
}      
       if ( char_died(ch) ) 
          return;
          
       stop_fighting( ch , TRUE );
       
       learn_from_success( ch, gsn_snipe );              
    }
    else
    {
       act( AT_ACTION, "You fire at $N but don't even come close.", ch, NULL, victim, TO_CHAR );         
       sprintf( buf, "A blaster shot fired from the %s barely misses you." , dir_name[dir] );
       act( AT_ACTION, buf, ch, NULL, victim, TO_ROOM );  
       learn_from_failure( ch, gsn_snipe );
    }
            
    char_from_room( ch );
    char_to_room( ch, was_in_room );    
     
   if ( IS_NPC(ch) )                               
      WAIT_STATE( ch, 1 * PULSE_VIOLENCE );
   else
   {
   	if ( number_percent() < ch->pcdata->learned[gsn_third_attack] )
   	     WAIT_STATE( ch, 1 * PULSE_PER_SECOND );
   	else if ( number_percent() < ch->pcdata->learned[gsn_second_attack] )
   	     WAIT_STATE( ch, 2 * PULSE_PER_SECOND );
   	else 
   	     WAIT_STATE( ch, 3 * PULSE_PER_SECOND );
   }
   if ( IS_NPC( victim ) && !char_died(victim) )
   {
      if ( IS_SET( victim->act , ACT_SENTINEL ) )
      {
         victim->was_sentinel = victim->in_room;
         REMOVE_BIT( victim->act, ACT_SENTINEL );
      }
      start_hating( victim , ch );
      start_hunting( victim, ch );
   } 
}

/* syntax throw <obj> [direction] [target] */

void do_throw( CHAR_DATA *ch, char *argument )
{
   OBJ_DATA        * obj;
   OBJ_DATA        * tmpobj;
   char              arg[MAX_INPUT_LENGTH];  
   char              arg2[MAX_INPUT_LENGTH];
   char              arg3[MAX_INPUT_LENGTH];
   sh_int            dir;
   EXIT_DATA       * pexit;
   ROOM_INDEX_DATA * was_in_room;
   ROOM_INDEX_DATA * to_room;
   CHAR_DATA       * victim;
   char              buf[MAX_STRING_LENGTH];


   argument = one_argument( argument, arg );
   argument = one_argument( argument, arg2 );
   argument = one_argument( argument, arg3 );
   
   was_in_room = ch->in_room;
      
   if ( arg[0] == '\0' )
   {
     send_to_char( "Usage: throw <object> [direction] [target]\n\r", ch );
     return;
   }

     
   obj = get_eq_char( ch, WEAR_MISSILE_WIELD );
   if ( !obj || !nifty_is_name( arg, obj->name ) )
      obj = get_eq_char( ch, WEAR_HOLD );
      if ( !obj || !nifty_is_name( arg, obj->name ) )
          obj = get_eq_char( ch, WEAR_WIELD );
          if ( !obj || !nifty_is_name( arg, obj->name ) )
              obj = get_eq_char( ch, WEAR_DUAL_WIELD );
              if ( !obj || !nifty_is_name( arg, obj->name ) )
   if ( !obj || !nifty_is_name_prefix( arg, obj->name ) )
      obj = get_eq_char( ch, WEAR_HOLD );
      if ( !obj || !nifty_is_name_prefix( arg, obj->name ) )
          obj = get_eq_char( ch, WEAR_WIELD );
          if ( !obj || !nifty_is_name_prefix( arg, obj->name ) )
              obj = get_eq_char( ch, WEAR_DUAL_WIELD );
              if ( !obj || !nifty_is_name_prefix( arg, obj->name ) )
   {
         ch_printf( ch, "You don't seem to be holding or wielding %s.\n\r", arg );
         return;
   }  

    if ( IS_OBJ_STAT(obj, ITEM_NOREMOVE) )
    {
	act( AT_PLAIN, "You can't throw $p.", ch, obj, NULL, TO_CHAR );
	return;
    }

   if ( ch->position == POS_FIGHTING )
   {
       victim = who_fighting( ch );
       if ( char_died ( victim ) )
           return;
       act( AT_ACTION, "You throw $p at $N.", ch, obj, victim, TO_CHAR );
       act( AT_ACTION, "$n throws $p at $N.", ch, obj, victim, TO_NOTVICT );
       act( AT_ACTION, "$n throw $p at you.", ch, obj, victim, TO_VICT );        
   }
   else if ( arg2[0] == '\0' )
   {
       sprintf( buf, "$n throws %s at the floor." , obj->short_descr );
       act( AT_ACTION, buf, ch, NULL, NULL, TO_ROOM );  
       ch_printf( ch, "You throw %s at the floor.\n\r", obj->short_descr );
       
       victim = NULL;
   }
   else  if ( ( dir = get_door( arg2 ) ) != -1 )
   {
      if ( ( pexit = get_exit( ch->in_room, dir ) ) == NULL )
      {
         send_to_char( "Are you expecting to throw it through a wall!?\n\r", ch );
         return;
      }

      
      if ( IS_SET( pexit->exit_info, EX_CLOSED ) )
      {
          send_to_char( "Are you expecting to throw it  through a door!?\n\r", ch );
          return;
      }
      
      
      switch ( dir )
      {
        case 0:
        case 1:
           dir += 2;
           break;
        case 2:
        case 3:
           dir -= 2;
           break;
        case 4:
        case 7:
           dir += 1;
           break;
        case 5:
        case 8:
           dir -= 1;
           break;
        case 6:
           dir += 3;
           break;
        case 9:
           dir -=3;
           break;
      }

      to_room = NULL;
      if ( pexit->distance > 1 )
       to_room = generate_exit( ch->in_room , &pexit );
    
      if ( to_room == NULL )
       to_room = pexit->to_room;
    

      char_from_room( ch );
      char_to_room( ch, to_room );    
     
      victim = get_char_room( ch, arg3 );

      if ( victim )
      { 
        if ( is_safe( ch, victim ) )
	return;
    
        if ( IS_AFFECTED(ch, AFF_CHARM) && ch->master == victim )
        {
        act( AT_PLAIN, "$N is your beloved master.", ch, NULL, victim, TO_CHAR );
	return;
        }
    
        if ( !IS_NPC( victim ) && IS_SET( ch->act, PLR_NICE ) )
        {
        send_to_char( "You feel too nice to do that!\n\r", ch );
        return;
        }
    
        char_from_room( ch );
        char_to_room( ch, was_in_room );    

      
        if ( IS_SET( ch->in_room->room_flags, ROOM_SAFE ) )
        {
	set_char_color( AT_MAGIC, ch );
	send_to_char( "You'll have to do that elswhere.\n\r", ch );
	return;
        }
       
        to_room = NULL;
        if ( pexit->distance > 1 )
           to_room = generate_exit( ch->in_room , &pexit );
    
        if ( to_room == NULL )
           to_room = pexit->to_room;
    
       
        char_from_room( ch );
        char_to_room( ch, to_room );    
        
        sprintf( buf , "Someone throws %s at you from the %s." , obj->short_descr , dir_name[dir] );
        act( AT_ACTION, buf , victim, NULL, ch, TO_CHAR );      
        act( AT_ACTION, "You throw $p at $N.", ch, obj, victim, TO_CHAR );         
        sprintf( buf, "%s is thrown at $N from the %s." , obj->short_descr , dir_name[dir] );
        act( AT_ACTION, buf, ch, NULL, victim, TO_NOTVICT );  


      }
      else
      {   
         ch_printf( ch, "You throw %s %s.\n\r", obj->short_descr , dir_name[get_dir( arg2 )] );
         sprintf( buf, "%s is thrown from the %s." , obj->short_descr , dir_name[dir] );
         act( AT_ACTION, buf, ch, NULL, NULL, TO_ROOM );  

      }
   }
   else if ( ( victim = get_char_room( ch, arg2 ) ) != NULL )
   {
        if ( is_safe( ch, victim ) )
	return;
    
        if ( IS_AFFECTED(ch, AFF_CHARM) && ch->master == victim )
        {
        act( AT_PLAIN, "$N is your beloved master.", ch, NULL, victim, TO_CHAR );
	return;
        }
    
        if ( !IS_NPC( victim ) && IS_SET( ch->act, PLR_NICE ) )
        {
        send_to_char( "You feel too nice to do that!\n\r", ch );
        return;
        }
    
   }
   else
   {
       ch_printf( ch, "They don't seem to be here!\n\r");
       return;         
   }

   
   if ( obj == get_eq_char( ch, WEAR_WIELD )
   && ( tmpobj = get_eq_char( ch, WEAR_DUAL_WIELD)) != NULL )
       tmpobj->wear_loc = WEAR_WIELD;

   unequip_char( ch, obj );
   separate_obj( obj );
   obj_from_char( obj );
   obj = obj_to_room( obj, ch->in_room );
   
   if ( obj->item_type != ITEM_GRENADE )
       damage_obj ( obj );
   
/* NOT NEEDED UNLESS REFERING TO OBJECT AGAIN 

   if( obj_extracted(obj) )
      return;
*/
   if ( ch->in_room !=  was_in_room )
   {
     char_from_room( ch );
     char_to_room( ch, was_in_room );    
   }
   
   if ( !victim || char_died( victim ) )
       learn_from_failure( ch, gsn_throw );
   else
   {
       
       WAIT_STATE( ch, skill_table[gsn_throw]->beats );
       if ( IS_NPC(ch) || number_percent( ) < ch->pcdata->learned[gsn_throw] )
       {
	 learn_from_success( ch, gsn_throw );
	 global_retcode = damage( ch, victim, number_range( obj->weight*2 , (obj->weight*2 + ch->perm_str) ), TYPE_HIT );
       }
       else
       {
	 learn_from_failure( ch, gsn_throw );
	 global_retcode = damage( ch, victim, 0, TYPE_HIT );
       }
       if ( IS_NPC( victim ) && !char_died ( victim) )
       {
          if ( IS_SET( victim->act , ACT_SENTINEL ) )
          {
             victim->was_sentinel = victim->in_room;
             REMOVE_BIT( victim->act, ACT_SENTINEL );
          }
          start_hating( victim , ch );
          start_hunting( victim, ch );
       } 
   }
   return;                                   
}

void do_slice( CHAR_DATA *ch, char *argument )
{   
    CHAR_DATA *victim;
    char arg1 [MAX_INPUT_LENGTH];
    OBJ_DATA *obj;

    if ( IS_NPC(ch) )
	return;

    if ( ch->subclass != SUBCLASS_BLADEMASTER )
    {
      send_to_char( "Only Swordsmen can execute a proper slice attack!!\n\r" , ch );
      return;
    }

    argument = one_argument( argument, arg1 );

    if ( arg1[0] == '\0' )
    {
	send_to_char( "Slice whom?\n\r", ch );
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

    if( (obj = get_eq_char(ch, WEAR_WIELD)) == NULL)
    {
       send_to_char("With what weapon?\n\r", ch);
       return;
    }
    else
    {
       if(obj->value[3] != WEAPON_SWORD && obj->value[3] != WEAPON_LIGHTSABER && obj->value[3] != WEAPON_VIBRO_SWORD )
       {
          send_to_char("You can only slice with a lightsaber or a sword.\n\r", ch);
          return;
       }
    }
    global_retcode = multi_hit( ch, victim, gsn_slice );
    WAIT_STATE(ch, PULSE_VIOLENCE);
    return; 
}


