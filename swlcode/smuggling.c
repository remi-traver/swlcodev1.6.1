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

int     xp_compute                ( CHAR_DATA *ch , CHAR_DATA *victim );

extern int      top_affect;
extern bool    check_blind             args( ( CHAR_DATA *ch ) );
extern void    show_condition          args( ( CHAR_DATA *ch, CHAR_DATA *victim ) );

void           get_obj_palm            args( ( CHAR_DATA *ch, OBJ_DATA *obj, OBJ_DATA *container ) );

void do_disguise( CHAR_DATA *ch, char *argument )
{
    int chance;
    char arg1[MAX_INPUT_LENGTH];
    if ( IS_NPC(ch) )
	return;
    argument = one_argument(argument, arg1);

    if ( IS_SET( ch->pcdata->flags, PCFLAG_NOTITLE ))
    {
        send_to_char( "You try but the Force resists you.\n\r", ch );
        return;
    }
    
    if ( arg1[0] == '\0' )
    {
	send_to_char( "Change your name to what?\n\r", ch );
	return;
    }
    if(!str_cmp(arg1, "off"))
    {
        if(IS_SET(ch->pcdata->flags, PCFLAG_DISGUISED))
        {
           send_to_char( "You remove your disguise.\n\r", ch);
           REMOVE_BIT(ch->pcdata->flags, PCFLAG_DISGUISED);
           set_title(ch, ch->name);
           return;
        }
        else
        {
           send_to_char("You aren't disguised.\n\r", ch);
           return;
        }
    }
    if(argument[0] == '\0')
    {
        send_to_char("Change your title to what?\n\r", ch);
        return;
    }

    if ((get_trust(ch) <= LEVEL_IMMORTAL) && (!nifty_is_name(arg1, stripclr( argument ) ) ) )
     {
       send_to_char("You must include your name/disguise name somewhere in your title!", ch);
       return;
     }

    chance = (int) (ch->pcdata->learned[gsn_disguise]);
    
    if ( number_percent( ) > chance )
    {
        send_to_char( "You try to disguise yourself but fail.\n\r", ch );
        learn_from_failure(ch, gsn_disguise);
        return;
    }
    learn_from_success(ch, gsn_disguise);             
    if ( strlen(argument) > 50 )
	argument[50] = '\0';
    if(!IS_SET(ch->pcdata->flags, PCFLAG_DISGUISED))
       SET_BIT(ch->pcdata->flags, PCFLAG_DISGUISED);    
    else
       STRFREE(ch->pcdata->disguisename);
    smash_tilde( argument );
    smash_tilde( arg1 );
    bookcap(arg1);
    ch->pcdata->disguisename = STRALLOC(arg1);
    set_title( ch, argument );
    send_to_char( "Ok.\n\r", ch );
    act( AT_PLAIN, "$n changes $s appearance!", ch, NULL, argument, TO_ROOM );
    learn_from_success( ch, gsn_disguise );
}

void do_beg( CHAR_DATA *ch, char *argument )
{
    char buf  [MAX_STRING_LENGTH];
    char arg1 [MAX_INPUT_LENGTH];
    CHAR_DATA *victim;
    int percent, xp;
    int amount;

    if ( IS_NPC (ch) ) return;
    
    argument = one_argument( argument, arg1 );

    if ( ch->mount )
    {
	send_to_char( "You can't do that while mounted.\n\r", ch );
	return;
    }

    if ( arg1[0] == '\0' )
    {
	send_to_char( "Beg for money from whom?\n\r", ch );
	return;
    }

    if ( ms_find_obj(ch) )
	return;

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

    if ( ch->position == POS_FIGHTING )
    {
        send_to_char( "Interesting combat technique.\n\r" , ch );
        return;
    }
    
    if ( victim->position == POS_FIGHTING )
    {
        send_to_char( "They're a little busy right now.\n\r" , ch );
        return;
    }
    
    if ( ch->position <= POS_SLEEPING )
    {
        send_to_char( "In your dreams or what?\n\r" , ch );
        return;
    }
    
    if ( victim->position <= POS_SLEEPING )
    {
        send_to_char( "You might want to wake them first...\n\r" , ch );
        return;
    }

    if ( !IS_NPC( victim ) )
    {
	send_to_char( "You beg them for money.\n\r", ch );
	act( AT_ACTION, "$n begs you to give $s some change.\n\r", ch, NULL, victim, TO_VICT    );
	act( AT_ACTION, "$n begs $N for change.\n\r",  ch, NULL, victim, TO_NOTVICT );
	return;
    }
    
    WAIT_STATE( ch, skill_table[gsn_beg]->beats );
    percent  = number_percent( ) - ch->skill_level[SMUGGLING_ABILITY] + victim->top_level;

    if ( percent > ch->pcdata->learned[gsn_beg]  ) 
    {
	/*
	 * Failure.
	 */
	send_to_char( "You beg them for money but don't get any!\n\r", ch );
	act( AT_ACTION, "$n is really getting on your nerves with all this begging!\n\r", ch, NULL, victim, TO_VICT    );
	act( AT_ACTION, "$n begs $N for money.\n\r",  ch, NULL, victim, TO_NOTVICT );

        if ( victim->alignment < 0 && victim->top_level >= ch->top_level+5 )
	{
	  sprintf( buf, "%s is an annoying beggar and needs to be taught a lesson!", ch->name );
	  do_yell( victim, buf );
          global_retcode = multi_hit( victim, ch, TYPE_UNDEFINED );
	}
	
	learn_from_failure( ch, gsn_beg );

	return;
    }

    
    act( AT_ACTION, "$n begs $N for money.\n\r",  ch, NULL, victim, TO_NOTVICT );
    act( AT_ACTION, "$n begs you for money!\n\r", ch, NULL, victim, TO_VICT    );

    amount = UMIN( victim->gold , number_range(1, 10) );
    if ( amount <= 0 )
    {
        do_look( victim , ch->name );
	do_say( victim , "Sorry I have nothing to spare.\n\r" );
	learn_from_failure( ch, gsn_beg );
	return;
    }

	ch->gold     += amount;
	victim->gold -= amount;
	ch_printf( ch, "%s gives you %d credits.\n\r", victim->short_descr , amount );
	learn_from_success( ch, gsn_beg );
	xp = UMIN( amount*10 , ( exp_level( ch->skill_level[SMUGGLING_ABILITY]+1) - exp_level( ch->skill_level[SMUGGLING_ABILITY])  )  );    
        xp = UMIN( xp , xp_compute( ch, victim ) );
        gain_exp( ch, xp, SMUGGLING_ABILITY );  
        ch_printf( ch, "&WYou gain %ld smuggling experience points!\n\r", xp );
        act( AT_ACTION, "$N gives $n some money.\n\r",  ch, NULL, victim, TO_NOTVICT );
        act( AT_ACTION, "You give $n some money.\n\r", ch, NULL, victim, TO_VICT    );
	return;

}

void do_pickshiplock( CHAR_DATA *ch, char *argument )
{
   do_pick( ch, argument );
}

void do_hijack( CHAR_DATA *ch, char *argument )
{
    int chance; 
    SHIP_DATA *ship, *onship;
    char buf[MAX_STRING_LENGTH];
    char logbuf[MAX_STRING_LENGTH];
            
    	        if ( (ship = ship_from_cockpit(ch->in_room->vnum)) == NULL )  
    	        {
    	            send_to_char("&RYou must be in the cockpit of a ship to do that!\n\r",ch);
    	            return;
    	        }
    	        
    	        if ( ship->class > SHIP_PLATFORM )
    	        {
    	            send_to_char("&RThis isn't a spacecraft!\n\r",ch);
    	            return;
    	        }
    	        
    	        if ( (ship = ship_from_pilotseat(ch->in_room->vnum)) == NULL )  
    	        {
    	            send_to_char("&RYou don't seem to be in the pilot seat!\n\r",ch);
    	            return;
    	        }
    	        
    	        if ( check_pilot( ch , ship ) )
    	        {
    	            send_to_char("&RWhat would be the point of that!\n\r",ch);
    	            return;
    	        }
    	        
    	        if ( ship->type == MOB_SHIP && get_trust(ch) < 102 )
    	        {
    	            send_to_char("&RThis ship isn't pilotable by mortals at this point in time...\n\r",ch);
    	            return;
    	        }
    	        
                if  ( ship->class == SHIP_PLATFORM )
                {
                   send_to_char( "You can't do that here.\n\r" , ch );
                   return;
                }   
    
    	        if ( ship->lastdoc != ship->location )
                {
                     send_to_char("&rYou don't seem to be docked right now.\n\r",ch);
                     return;
                }
    
    	        if ( ship->shipstate != SHIP_DOCKED && ship->shipstate != SHIP_DISABLED )
    	        {
    	            send_to_char("The ship is not docked right now.\n\r",ch);
    	            return;
    	        }
           if ( (( onship = ship_from_hanger( ship->location ) )  != NULL )
           && !(IS_SET(ship->flags, SHIPFLAG_SIMULATOR) ? 1
                                                        : onship->bayopen) )
           {
                send_to_char("&RThe hangar is closed!\n\r",ch);
                return;
           }

                if ( ship->shipstate == SHIP_DISABLED )
    	        {
    	            send_to_char("The ships drive is disabled .\n\r",ch);
    	            return;
    	        }
                
                chance = IS_NPC(ch) ? ch->top_level
	                 : (int)  (ch->pcdata->learned[gsn_hijack]) ;
                if ( number_percent( ) > chance )
    		{  
    		    send_to_char("You fail to figure out the correct launch code.\n\r",ch);
                   learn_from_failure( ch, gsn_hijack );
    	            return;
                }
                
    	        if ( ship->class == FIGHTER_SHIP )
                    chance = IS_NPC(ch) ? ch->top_level
	                 : (int)  (ch->pcdata->learned[gsn_starfighters]) ;
                if ( ship->class == MIDSIZE_SHIP )
                    chance = IS_NPC(ch) ? ch->top_level
	                 : (int)  (ch->pcdata->learned[gsn_midships]) ;
                if ( ship->class == CAPITAL_SHIP )
                    chance = IS_NPC(ch) ? ch->top_level
	                 : (int) (ch->pcdata->learned[gsn_capitalships]);
                if ( number_percent( ) < chance )
    		{  
                
    		   if (ship->hatchopen)
    		   {
    		     ship->hatchopen = FALSE;
    		     sprintf( buf , "The hatch on %s closes." , ship->name);  
       	             echo_to_room( AT_YELLOW , get_room_index(ship->location) , buf );
       	             echo_to_room( AT_YELLOW , get_room_index(ship->entrance) , "The hatch slides shut." );
       	           }
    		   set_char_color( AT_GREEN, ch );
    		   send_to_char( "Launch sequence initiated.\n\r", ch);
    		   act( AT_PLAIN, "$n starts up the ship and begins the launch sequence.", ch,
		        NULL, argument , TO_ROOM );
		   echo_to_ship( AT_YELLOW , ship , "The ship hums as it lifts off the ground.");
    		   sprintf( buf, "%s begins to launch.", ship->name );
    		   echo_to_room( AT_YELLOW , get_room_index(ship->location) , buf );
    		   ship->shipstate = SHIP_LAUNCH;
    		   ship->currspeed = ship->realspeed;
    		   if ( ship->class == FIGHTER_SHIP )
                      learn_from_success( ch, gsn_starfighters );
                   if ( ship->class == MIDSIZE_SHIP )
                      learn_from_success( ch, gsn_midships );
                   if ( ship->class == CAPITAL_SHIP )
                      learn_from_success( ch, gsn_capitalships );
                   
                   learn_from_success( ch, gsn_hijack );
                   /* sprintf( buf, "%s has been hijacked!", ship->name );
    		   echo_to_all( AT_RED , buf, 0 ); */
                   sprintf( logbuf, "%s has been hijacked by %s.", ship->name, ch->name);
    		   log_string( logbuf );
                   return;   	   	
                }
                set_char_color( AT_RED, ch );
	        send_to_char("You fail to work the controls properly!\n\r",ch);
	        if ( ship->class == FIGHTER_SHIP )
                    learn_from_failure( ch, gsn_starfighters );
                if ( ship->class == MIDSIZE_SHIP )
    	            learn_from_failure( ch, gsn_midships );
                if ( ship->class == CAPITAL_SHIP )
                    learn_from_failure( ch, gsn_capitalships );
    	   	return;	
    	
}

void do_besiege( CHAR_DATA *ch, char *argument )
{
    int chance; 
    SHIP_DATA *ship;
    char logbuf[MAX_STRING_LENGTH];
    char buf[MAX_STRING_LENGTH];
            
    	        if ( (ship = ship_from_cockpit(ch->in_room->vnum)) == NULL )  
    	        {
    	            send_to_char("&RYou must be in the cockpit of a ship to do that!\n\r",ch);
    	            return;
    	        }
    	        
    	        if ( ship->class > SHIP_PLATFORM )
    	        {
    	            send_to_char("&RThis isn't a spacecraft!\n\r",ch);
    	            return;
    	        }
    	        
    	        if ( (ship = ship_from_pilotseat(ch->in_room->vnum)) == NULL )  
    	        {
    	            send_to_char("&RYou don't seem to be in the pilot seat!\n\r",ch);
    	            return;
    	        }
    	        
    	        if ( check_pilot( ch , ship ) )
    	        {
    	            send_to_char("&RWhat would be the point of that!?\n\r",ch);
    	            return;
    	        }
    	        
    	        if ( ship->type == MOB_SHIP )
    	        {
    	            send_to_char("&RThis ship isn't pilotable at this point in time...\n\r",ch);
    	            return;
    	        }
    	        
                if  ( ship->class == SHIP_PLATFORM )
                {
                   send_to_char( "You can't do that here.\n\r" , ch );
                   return;
                }   
    
		if ( ship->autopilot==FALSE )
		{
		    send_to_char( "This ship's autopilot is already off.\n\r", ch);
		    return;
		}

                chance = IS_NPC(ch) ? ch->top_level
	                 : (int)  (ch->pcdata->learned[gsn_besiege]) ;
                if ( number_percent( ) > chance )
    		    {  
    		    send_to_char("You fail to disable the autopilot.\n\r",ch);
                   learn_from_failure( ch, gsn_besiege );
    	            return;
                }
                
    	        if ( ship->class == FIGHTER_SHIP )
                    chance = IS_NPC(ch) ? ch->top_level
	                 : (int)  (ch->pcdata->learned[gsn_starfighters]) ;
                if ( ship->class == MIDSIZE_SHIP )
                    chance = IS_NPC(ch) ? ch->top_level
	                 : (int)  (ch->pcdata->learned[gsn_midships]) ;
                if ( ship->class == CAPITAL_SHIP )
                    chance = IS_NPC(ch) ? ch->top_level
	                 : (int) (ch->pcdata->learned[gsn_capitalships]);
                if ( number_percent( ) < chance )
    		{  
                
    		   set_char_color( AT_GREEN, ch );
    		   send_to_char( "Autopilot disabled.\n\r", ch);
    		   act( AT_PLAIN, "$n starts up the ship and begins the launch sequence.", ch,
		        NULL, argument , TO_ROOM );
		   echo_to_ship( AT_YELLOW , ship , "The ship's autopilot has been overridden.");
    		   sprintf( buf, "%s disables the autopilot.", ship->name );
    		   echo_to_room( AT_YELLOW , get_room_index(ship->location) , buf );
    		   ship->currspeed = ship->realspeed;
		   ship->autopilot = FALSE;
    		   if ( ship->class == FIGHTER_SHIP )
                      learn_from_success( ch, gsn_starfighters );
                   if ( ship->class == MIDSIZE_SHIP )
                      learn_from_success( ch, gsn_midships );
                   if ( ship->class == CAPITAL_SHIP )
                      learn_from_success( ch, gsn_capitalships );
                   
                   learn_from_success( ch, gsn_besiege );
                   sprintf( buf, "%s has been sieged!!", ship->name );
    		   echo_to_all( AT_RED , buf, 0 );
                   sprintf( logbuf, "%s has been sieged by %s!", ship->name, ch->name);
                   log_string( logbuf );
    		   
                   return;   	   	
                }
                set_char_color( AT_RED, ch );
	        send_to_char("You fail to work the controls properly!\n\r",ch);
	        if ( ship->class == FIGHTER_SHIP )
                    learn_from_failure( ch, gsn_starfighters );
                if ( ship->class == MIDSIZE_SHIP )
    	            learn_from_failure( ch, gsn_midships );
                if ( ship->class == CAPITAL_SHIP )
                    learn_from_failure( ch, gsn_capitalships );
    	   	return;	
    	
}

void do_peek(CHAR_DATA *ch, char *argument )
{
  char arg1 [MAX_INPUT_LENGTH];
  CHAR_DATA *victim;
  int chance;
  OBJ_DATA *obj;
  int iWear;
  bool found;

  if ( !ch->desc )
    return;

  if ( ch->position < POS_SLEEPING )
  {
    send_to_char( "You can't see anything but stars!\n\r", ch );
    return;
  }

  if ( ch->position == POS_SLEEPING )
  {
    send_to_char( "You can't see anything, you're sleeping!\n\r", ch );
    return;
  }

  if ( !check_blind( ch ) )
    return;

  argument = one_argument( argument, arg1 );

  if ( arg1[0] == '\0' )
  {
      send_to_char("Peek at who?\n\r", ch);
      return;
  }

  if ( ( victim = get_char_room( ch, arg1 ) ) == NULL )
  {
    send_to_char( "They're not here.", ch );
    return;
  }
  else
  {
    chance = IS_NPC(ch) ? ch->top_level
         : (int)  (ch->pcdata->learned[gsn_peek]);
    if(number_percent() > chance)
    {
      act( AT_ACTION, "$n looks at you.", ch, NULL, victim, TO_VICT    );
      act( AT_ACTION, "$n looks at $N.",  ch, NULL, victim, TO_NOTVICT );
    }
    ch_printf(ch, "You peek at %s.\n\r", NAME(victim) );
        if ( victim->description[0] != '\0' )
    {
        send_to_char( victim->description, ch );
    }
    else
    {
    act( AT_PLAIN, "You see nothing special about $M.", ch, NULL, victim, TO_CHAR );
    }

    show_condition( ch, victim );

    found = FALSE;
    for ( iWear = 0; iWear < MAX_WEAR; iWear++ )
    {
        if ( ( obj = get_eq_char( victim, iWear ) ) != NULL
        &&   can_see_obj( ch, obj ) )
        {
            if ( !found )
            {
                send_to_char( "\n\r", ch );
                act( AT_PLAIN, "$N is using:", ch, NULL, victim, TO_CHAR );
                found = TRUE;
            }
            send_to_char( "&g", ch );
            send_to_char( where_name[iWear], ch );
            send_to_char( format_obj_to_char( obj, ch, TRUE ), ch );
            send_to_char( "\n\r", ch );
        }
    }

    /*
     * Crash fix here by Thoric
     */
    if ( IS_NPC(ch) || victim == ch )
      return;

    if ( number_percent( ) < ch->pcdata->learned[gsn_peek] )
    {
        send_to_char( "\n\rYou peek at the inventory:\n\r", ch );
        show_list_to_char( victim->first_carrying, ch, TRUE, TRUE );
        learn_from_success( ch, gsn_peek );
    }
    else
      if ( ch->pcdata->learned[gsn_peek] )
        learn_from_failure( ch, gsn_peek );

    return;
  }
}

/* The command used to attempt a masked get- Arcturus */
void do_palm( CHAR_DATA *ch, char *argument )
{
    char arg1[MAX_INPUT_LENGTH];
    char arg2[MAX_INPUT_LENGTH];
    OBJ_DATA *obj;
    OBJ_DATA *obj_next;
    OBJ_DATA *container;
    sh_int number;
    bool found;

    argument = one_argument( argument, arg1 );
    if ( is_number(arg1) )
    {
	number = atoi(arg1);
	if ( number < 1 )
	{
	    send_to_char( "That was easy...\n\r", ch );
	    return;
	}
	if ( (ch->carry_number + number) > can_carry_n(ch) )
	{
	    send_to_char( "You can't carry that many.\n\r", ch );
	    return;
	}
	argument = one_argument( argument, arg1 );
    }
    else
	number = 0;
    argument = one_argument( argument, arg2 );
    /* munch optional words */
    if ( !str_cmp( arg2, "from" ) && argument[0] != '\0' )
	argument = one_argument( argument, arg2 );

    /* Get type. */
    if ( arg1[0] == '\0' )
    {
	send_to_char( "Palm what?\n\r", ch );
	return;
    }

    if ( ms_find_obj(ch) )
	return;

    if ( arg2[0] == '\0' )
    {
	if ( number <= 1 && str_cmp( arg1, "all" ) && str_prefix( "all.", arg1 ) )
	{
	    /* 'get obj' */
	    obj = get_obj_list( ch, arg1, ch->in_room->first_content );
	    if ( !obj )
	    {
		act( AT_PLAIN, "I see no $T here.", ch, NULL, arg1, TO_CHAR );
		return;
	    }
	    separate_obj(obj);
	    get_obj_palm( ch, obj, NULL );
	    if ( char_died(ch) )
		return;
	    if ( IS_SET( sysdata.save_flags, SV_GET ) )
		save_char_obj( ch );
	}
	else
	{
	    sh_int cnt = 0;
	    bool fAll;
	    char *chk;

	    if ( IS_SET( ch->in_room->room_flags, ROOM_DONATION ) )
	    {
		send_to_char( "The gods frown upon such a display of greed!\n\r", ch );
		return;
	    }
	    if ( !str_cmp(arg1, "all") )
		fAll = TRUE;
	    else
		fAll = FALSE;
	    if ( number > 1 )
		chk = arg1;
	    else
		chk = &arg1[4];
	    /* 'get all' or 'get all.obj' */
	    found = FALSE;
	    for ( obj = ch->in_room->first_content; obj; obj = obj_next )
	    {
		obj_next = obj->next_content;
		if ( ( fAll || nifty_is_name( chk, obj->name ) )
		&&   can_see_obj( ch, obj ) )
		{
		    found = TRUE;
		    if ( number && (cnt + obj->count) > number )
			split_obj( obj, number - cnt );
		    cnt += obj->count;
		    get_obj_palm( ch, obj, NULL );
		    if ( char_died(ch)
		    ||   ch->carry_number >= can_carry_n( ch )
		    ||   ch->carry_weight >= can_carry_w( ch )
		    ||   (number && cnt >= number) )
		    {
			if ( IS_SET(sysdata.save_flags, SV_GET)
			&&  !char_died(ch) )
			    save_char_obj(ch);
			return;
		    }
		}
	    }

	    if ( !found )
	    {
		if ( fAll )
		  send_to_char( "I see nothing here.\n\r", ch );
		else
		  act( AT_PLAIN, "I see no $T here.", ch, NULL, chk, TO_CHAR );
	    }
	    else
	    if ( IS_SET( sysdata.save_flags, SV_GET ) )
		save_char_obj( ch );
	}
    }
    else
    {
	/* 'get ... container' */
	if ( !str_cmp( arg2, "all" ) || !str_prefix( "all.", arg2 ) )
	{
	    send_to_char( "You can't do that.\n\r", ch );
	    return;
	}

	if ( ( container = get_obj_here( ch, arg2 ) ) == NULL )
	{
	    act( AT_PLAIN, "I see no $T here.", ch, NULL, arg2, TO_CHAR );
	    return;
	}

	switch ( container->item_type )
	{
	default:
	    if ( !IS_OBJ_STAT( container, ITEM_COVERING ) )
	    {
		send_to_char( "That's not a container.\n\r", ch );
		return;
	    }
	    if ( ch->carry_weight + container->weight > can_carry_w( ch ) )
	    {
		send_to_char( "It's too heavy for you to lift.\n\r", ch );
		return;
	    }
	    break;

	case ITEM_CONTAINER:
	case ITEM_DROID_CORPSE:
	case ITEM_CORPSE_PC:
	case ITEM_CORPSE_NPC:
	    break;
	}

	if ( !IS_OBJ_STAT(container, ITEM_COVERING )
	&&    IS_SET(container->value[1], CONT_CLOSED) )
	{
	    act( AT_PLAIN, "The $d is closed.", ch, NULL, container->name, TO_CHAR );
	    return;
	}

	if ( number <= 1 && str_cmp( arg1, "all" ) && str_prefix( "all.", arg1 ) )
	{
	    /* 'get obj container' */
	    obj = get_obj_list( ch, arg1, container->first_content );
	    if ( !obj )
	    {
		act( AT_PLAIN, IS_OBJ_STAT(container, ITEM_COVERING) ?
			"I see nothing like that beneath the $T." :
			"I see nothing like that in the $T.",
			ch, NULL, arg2, TO_CHAR );
		return;
	    }
	    separate_obj(obj);
	    get_obj_palm( ch, obj, container );

	    check_for_trap( ch, container, TRAP_GET );
	    if ( char_died(ch) )
	      return;
	    if ( IS_SET( sysdata.save_flags, SV_GET ) )
		save_char_obj( ch );
	}
	else
	{
	    int cnt = 0;
	    bool fAll;
	    char *chk;

	    /* 'get all container' or 'get all.obj container' */
/*	    if ( IS_OBJ_STAT( container, ITEM_DONATION ) )
	    {
		send_to_char( "The gods frown upon such an act of greed!\n\r", ch );
		return;
	    }*/
	    if ( !str_cmp(arg1, "all") )
		fAll = TRUE;
	    else
		fAll = FALSE;
	    if ( number > 1 )
		chk = arg1;
	    else
		chk = &arg1[4];
	    found = FALSE;
	    for ( obj = container->first_content; obj; obj = obj_next )
	    {
		obj_next = obj->next_content;
		if ( ( fAll || nifty_is_name( chk, obj->name ) )
		&&   can_see_obj( ch, obj ) )
		{
		    found = TRUE;
		    if ( number && (cnt + obj->count) > number )
			split_obj( obj, number - cnt );
		    cnt += obj->count;
		    get_obj_palm( ch, obj, container );
		    if ( char_died(ch)
		    ||   ch->carry_number >= can_carry_n( ch )
		    ||   ch->carry_weight >= can_carry_w( ch )
		    ||   (number && cnt >= number) )
		      return;
		}
	    }

	    if ( !found )
	    {
		if ( fAll )
		  act( AT_PLAIN, IS_OBJ_STAT(container, ITEM_COVERING) ?
			"I see nothing beneath the $T." :
			"I see nothing in the $T.",
			ch, NULL, arg2, TO_CHAR );
		else
		  act( AT_PLAIN, IS_OBJ_STAT(container, ITEM_COVERING) ?
			"I see nothing like that beneath the $T." :
			"I see nothing like that in the $T.",
			ch, NULL, arg2, TO_CHAR );
	    }
	    else
	      check_for_trap( ch, container, TRAP_GET );
	    if ( char_died(ch) )
		return;
	    if ( found && IS_SET( sysdata.save_flags, SV_GET ) )
		save_char_obj( ch );
	}
    }
    return;
}

/* Palming it anyone? Masked Get */
void get_obj_palm( CHAR_DATA *ch, OBJ_DATA *obj, OBJ_DATA *container )
{
    CLAN_DATA *clan;
    int weight;
    int chance, percent;
    CHAR_DATA *rch;
    chance = IS_NPC(ch) ? 50
                        : ch->pcdata->learned[gsn_palm]*.75+ch->mod_dex/2;
    if ( !CAN_WEAR(obj, ITEM_TAKE)
       && (ch->top_level < sysdata.level_getobjnotake )  )
    {
        send_to_char( "You can't take that.\n\r", ch );
        return;
    }
    if ( IS_OBJ_STAT( obj, ITEM_PROTOTYPE )
    &&  !can_take_proto( ch ) )
    {
        send_to_char( "A godly force prevents you from getting close to it.\n\r", ch );
        return;
    }

    if ( ch->carry_number + get_obj_number( obj ) > can_carry_n( ch ) )
    {
        act( AT_PLAIN, "$d: you can't carry that many items.",
                ch, NULL, obj->name, TO_CHAR );
        return;
    }

    if ( IS_OBJ_STAT( obj, ITEM_COVERING ) )
      weight = obj->weight;
    else
      weight = get_obj_weight( obj );

    if ( ch->carry_weight + weight > can_carry_w( ch ) )
    {
        act( AT_PLAIN, "$d: you can't carry that much weight.",
                ch, NULL, obj->name, TO_CHAR );
        return;
    }
    if ( container )
    {
        act( AT_ACTION, IS_OBJ_STAT(container, ITEM_COVERING) ?
                "You palm $p from beneath $P." : "You palm $p from $P",
                ch, obj, container, TO_CHAR );
       /* Not everyone can always catch a failed palm attempt, we'll limit it. */
        for ( rch = ch->in_room->first_person; rch; rch = rch->next_in_room )
        {
          percent = number_percent();
          if ( !can_see( rch, ch ) )
            percent -= 25; /* Kinda hard to detect someone you can't see. */
          if( rch == ch ) /* blah that I forgot this. */
            continue;
          if(IS_AWAKE(rch))
          {
            if(IS_AFFECTED(rch, AFF_SIT_AWARE))
            {
               percent += 15;
            }
         }
         else /* How are you going to see someone get something when you are asleep? */
         {
            percent = 0;
         } 
         if(IS_IMMORTAL(rch) && IS_AWAKE(rch)) /* You aren't sneaking anything by an immortal. */
            percent += 100;
         if(percent > chance)
         {
            set_char_color(AT_ACTION, rch);
            if(IS_OBJ_STAT(container, ITEM_COVERING) )
              ch_printf(rch, "%s gets %s from beneath %s.\n\r", NAME(ch), obj->short_descr, container->short_descr);
            else
              ch_printf(rch, "%s gets %s from %s.\n\r", NAME(ch), obj->short_descr, container->short_descr);
            learn_from_failure(ch, gsn_palm);
         }
         else
            learn_from_success(ch, gsn_palm);
        }
        obj_from_obj( obj );
    }
    else
    {
        act( AT_ACTION, "You palm $p.", ch, obj, container, TO_CHAR );
    /* Not everyone can always catch a failed palm attempt, we'll limit it. */
    for ( rch = ch->in_room->first_person; rch; rch = rch->next_in_room )
    {
       percent = number_percent();
       if ( !can_see( rch, ch ) )
          percent -= 25; /* Kinda hard to detect someone you can't see. */
       if( rch == ch ) /* Damn me for forgetting this again. */
          continue;
       if(IS_AWAKE(rch))
       {
          if(IS_AFFECTED(rch, AFF_SIT_AWARE))
          {
             percent += 15;
          }
       }
       else /* How are you going to see someone get something when you are asleep? */
       {
          percent = 0;
       }
       if(IS_IMMORTAL(rch) && IS_AWAKE(rch)) /* You aren't sneaking anything by an immortal. */
          percent += 100;
       if(percent > chance)
       {
          set_char_color(AT_ACTION, rch);
          ch_printf(rch, "%s gets %s.", NAME(ch), obj->short_descr);          
          learn_from_failure(ch, gsn_palm);
       }
       else
          learn_from_success(ch, gsn_palm);
    }
        obj_from_room( obj );
    }

    /* Clan storeroom checks */
    if ( IS_SET(ch->in_room->room_flags, ROOM_CLANSTOREROOM)
    && (!container || container->carried_by == NULL) )
        for ( clan = first_clan; clan; clan = clan->next )
          if ( clan->storeroom == ch->in_room->vnum )
            save_clan_storeroom(ch, clan);

    if ( obj->item_type != ITEM_CONTAINER )
      check_for_trap( ch, obj, TRAP_GET );
    if ( char_died(ch) )
      return;
    if ( obj->item_type == ITEM_MONEY )
    {
        ch->gold += obj->value[0];
        extract_obj( obj );
    }
    else
    {
        obj = obj_to_char( obj, ch );
    }

    if ( char_died(ch) || obj_extracted(obj) )
      return;
    oprog_get_trigger(ch, obj);
    return;
}

/* Tuck it away, without being seen. Redone put with mask chance. -Arcturus */
void do_tuck( CHAR_DATA *ch, char *argument )
{
    char arg1[MAX_INPUT_LENGTH];
    char arg2[MAX_INPUT_LENGTH];
    OBJ_DATA *container;
    OBJ_DATA *obj;
    OBJ_DATA *obj_next;
    CLAN_DATA *clan;
    sh_int      count;
    int         number;
    bool        save_char = FALSE;
    int percent, chance;
    CHAR_DATA *rch;
    chance = IS_NPC(ch) ? 50
                        : ch->pcdata->learned[gsn_tuck]*.75+ch->mod_dex/2;
    argument = one_argument( argument, arg1 );
    if ( is_number(arg1) )
    {
        number = atoi(arg1);
        if ( number < 1 )
        {
            send_to_char( "That was easy...\n\r", ch );
            return;
        }
        argument = one_argument( argument, arg1 );
    }
    else
        number = 0;
    argument = one_argument( argument, arg2 );
    /* munch optional words */
    if ( (!str_cmp(arg2, "into") || !str_cmp(arg2, "inside") || !str_cmp(arg2, "in"))
    &&   argument[0] != '\0' )
        argument = one_argument( argument, arg2 );

    if ( arg1[0] == '\0' || arg2[0] == '\0' )
    {
        send_to_char( "Tuck what in what?\n\r", ch );
        return;
    }

    if ( ms_find_obj(ch) )
        return;

    if ( !str_cmp( arg2, "all" ) || !str_prefix( "all.", arg2 ) )
    {
        send_to_char( "You can't do that.\n\r", ch );
        return;
    }

    if ( ( container = get_obj_here( ch, arg2 ) ) == NULL )
    {
        act( AT_PLAIN, "I see no $T here.", ch, NULL, arg2, TO_CHAR );
        return;
    }

    if ( !container->carried_by && IS_SET( sysdata.save_flags, SV_PUT ) )
        save_char = TRUE;

    if ( IS_OBJ_STAT(container, ITEM_COVERING) )
    {
        if ( ch->carry_weight + container->weight > can_carry_w( ch ) )
        {
            send_to_char( "It's too heavy for you to lift.\n\r", ch );
            return;
        }
    }
    else
    {
        if ( container->item_type != ITEM_CONTAINER )
        {
            send_to_char( "That's not a container.\n\r", ch );
            return;
        }

        if ( IS_SET(container->value[1], CONT_CLOSED) )
        {
            act( AT_PLAIN, "The $d is closed.", ch, NULL, container->name, TO_CHAR );
            return;
        }
    }

    if ( number <= 1 && str_cmp( arg1, "all" ) && str_prefix( "all.", arg1 ) )
    {
        /* 'put obj container' */
        if ( ( obj = get_obj_carry( ch, arg1 ) ) == NULL )
        {
            send_to_char( "You do not have that item.\n\r", ch );
            return;
        }

        if ( obj == container )
        {
            send_to_char( "You can't fold it into itself.\n\r", ch );
            return;
        }

        if ( !can_drop_obj( ch, obj ) )
        {
            send_to_char( "You can't let go of it.\n\r", ch );
            return;
        }

        if ( (IS_OBJ_STAT(container, ITEM_COVERING)
        &&   (get_obj_weight( obj ) / obj->count)
          > ((get_obj_weight( container ) / container->count)
          -   container->weight)) )
        {
            send_to_char( "It won't fit under there.\n\r", ch );
            return;
        }

        if ( (get_obj_weight( obj ) / obj->count)
           + (get_obj_weight( container ) / container->count)
           >  container->value[0] )
        {
            send_to_char( "It won't fit.\n\r", ch );
            return;
        }

        separate_obj(obj);
        separate_obj(container);
        obj_from_char( obj );
        obj = obj_to_obj( obj, container );
        check_for_trap ( ch, container, TRAP_PUT );
        if ( char_died(ch) )
          return;
        count = obj->count;
        obj->count = 1;
        act( AT_ACTION, IS_OBJ_STAT( container, ITEM_COVERING )
                ? "You tuck $p beneath $P." : "You tuck $p in $P.",
                ch, obj, container, TO_CHAR );
        for ( rch = ch->in_room->first_person; rch; rch = rch->next_in_room )
        {
          percent = number_percent();
          if ( !can_see( rch, ch ) )
             percent -= 25; /* Kinda hard to detect someone you can't see. */
          if( rch == ch ) /* Damn me for forgetting this again. */
             continue;
          if(IS_AWAKE(rch))
          {
             if(IS_AFFECTED(rch, AFF_SIT_AWARE))
             {
                percent += 15;
             }
          }
          else /* How are you going to see someone get something when you are asleep? */
          {
             percent = 0;
          }
          if(IS_IMMORTAL(rch) && IS_AWAKE(rch)) /* You aren't sneaking anything by an immortal. */
             percent += 100;
          if(percent > chance)
          {
             set_char_color(AT_ACTION, rch);
             if(IS_OBJ_STAT( container, ITEM_COVERING ) )
                ch_printf(rch, "%s hides %s beneath %s.\n\r", NAME(ch), obj->short_descr, container->short_descr);
             else
                ch_printf(rch, "%s puts %s in %s.\n\r", NAME(ch), obj->short_descr, container->short_descr); 
             learn_from_failure(ch, gsn_tuck);
          }
          else
             learn_from_success(ch, gsn_tuck);
        }
        obj->count = count;

        if ( save_char )
          save_char_obj(ch);
        /* Clan storeroom check */
        if ( IS_SET(ch->in_room->room_flags, ROOM_CLANSTOREROOM)
        &&   container->carried_by == NULL)
           for ( clan = first_clan; clan; clan = clan->next )
              if ( clan->storeroom == ch->in_room->vnum )
                save_clan_storeroom(ch, clan);
    }
    else
    {
        bool found = FALSE;
        int cnt = 0;
        bool fAll;
        char *chk;

        if ( !str_cmp(arg1, "all") )
            fAll = TRUE;
        else
            fAll = FALSE;
        if ( number > 1 )
            chk = arg1;
        else
            chk = &arg1[4];

        separate_obj(container);
        /* 'put all container' or 'put all.obj container' */
        for ( obj = ch->first_carrying; obj; obj = obj_next )
        {
            obj_next = obj->next_content;

            if ( ( fAll || nifty_is_name( chk, obj->name ) )
            &&   can_see_obj( ch, obj )
            &&   obj->wear_loc == WEAR_NONE
            &&   obj != container
            &&   can_drop_obj( ch, obj )
            &&   get_obj_weight( obj ) + get_obj_weight( container )
                 <= container->value[0] )
            {
                if ( number && (cnt + obj->count) > number )
                    split_obj( obj, number - cnt );
                cnt += obj->count;
                obj_from_char( obj );
                act( AT_ACTION, "You tuck $p in $P.", ch, obj, container, TO_CHAR );
                for ( rch = ch->in_room->first_person; rch; rch = rch->next_in_room )
                {
                   percent = number_percent();
                   if ( !can_see( rch, ch ) )
                      percent -= 25; /* Kinda hard to detect someone you can't see. */
                   if( rch == ch ) /* Damn me for forgetting this again. */
                      continue;
                   if(IS_AWAKE(rch))
                   {
                      if(IS_AFFECTED(rch, AFF_SIT_AWARE))
                      {
                         percent += 15;
                      }
                   }
                   else /* How are you going to see someone get something when you are asleep? */
                   {
                      percent = 0;
                   }
                   if(IS_IMMORTAL(rch) && IS_AWAKE(rch)) /* You aren't sneaking anything by an immortal. */
                       percent += 100;
                   if(percent > chance)
                   {
                      set_char_color(AT_ACTION, rch);
                      ch_printf(rch, "%s puts %s in %s.\n\r", NAME(ch), obj->short_descr, container->short_descr); 
                      learn_from_failure(ch, gsn_tuck);
                   }
                   else
                      learn_from_success(ch, gsn_tuck);
                }
                obj = obj_to_obj( obj, container );
                found = TRUE;

                check_for_trap( ch, container, TRAP_PUT );
                if ( char_died(ch) )
                  return;
                if ( number && cnt >= number )
                  break;
            }
        }

        /*
         * Don't bother to save anything if nothing was dropped   -Thoric
         */
        if ( !found )
        {
            if ( fAll )
              act( AT_PLAIN, "You are not carrying anything.",
                    ch, NULL, NULL, TO_CHAR );
            else
              act( AT_PLAIN, "You are not carrying any $T.",
                    ch, NULL, chk, TO_CHAR );
            return;
        }
        if ( save_char )
            save_char_obj(ch);
        /* Clan storeroom check */
        if ( IS_SET(ch->in_room->room_flags, ROOM_CLANSTOREROOM)
        && container->carried_by == NULL )
          for ( clan = first_clan; clan; clan = clan->next )
             if ( clan->storeroom == ch->in_room->vnum )
                save_clan_storeroom(ch, clan);
    }
    return;
}
