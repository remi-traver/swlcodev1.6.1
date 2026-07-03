/***************************************************************************
*                           STAR WARS REALITY 1.0                          *
*--------------------------------------------------------------------------*
* Star Wars Reality Code Additions and changes from the Smaug Code         *
* copyright (c) 1997 by Sean Cooper                                        *
* -------------------------------------------------------------------------*
* Starwars and Starwars Names copyright(c) Lucas Film Ltd.                 *
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
*		            Bounty Hunter Module    			   *   
*                    (  and area capturing as well  )                      * 
****************************************************************************/

#include <sys/types.h>
#include <ctype.h>
#include <stdio.h>
#include <string.h>
/* #include <stdlib.h> */
#include <time.h>
#include "mud.h"


BOUNTY_DATA * first_bounty;
BOUNTY_DATA * last_bounty;
BOUNTY_DATA * first_disintigration;
BOUNTY_DATA * last_disintigration;


// from honour.c
void gain_honour (CHAR_DATA *ch, int hincrease);

//from clans.c
void remroster (CHAR_DATA *ch);

void   disintigration args ( ( CHAR_DATA *ch , CHAR_DATA *victim , long amount ) );
void nodisintigration args ( ( CHAR_DATA *ch , CHAR_DATA *victim , long amount ) );
int xp_compute( CHAR_DATA *ch , CHAR_DATA *victim);

void save_disintigrations()
{    
    BOUNTY_DATA *tbounty;
    FILE *fpout;
    char filename[256];
    
    sprintf( filename, "%s%s", SYSTEM_DIR, DISINTIGRATION_LIST );
    fpout = fopen( filename, "w" );
    if ( !fpout )
    {
         bug( "FATAL: cannot open disintigration.lst for writing!\n\r", 0 );
         return;
    }
    for ( tbounty = first_disintigration; tbounty; tbounty = tbounty->next )
    {
        fprintf( fpout, "%s\n", tbounty->target );
        fprintf( fpout, "%ld\n", tbounty->amount );
    }
    fprintf( fpout, "$\n" );
    fclose( fpout );    
}
                                                                    

bool is_disintigration( CHAR_DATA *victim )
{
    BOUNTY_DATA *bounty;
    
    for ( bounty = first_disintigration; bounty; bounty = bounty->next )
    if ( !str_cmp( victim->name , bounty->target ) )
             return TRUE;
    return FALSE;                           
}

BOUNTY_DATA *get_disintigration( char *target )
{
    BOUNTY_DATA *bounty;
    
    for ( bounty = first_disintigration; bounty; bounty = bounty->next )
       if ( !str_cmp( target, bounty->target ) )
         return bounty;
    return NULL;
}

void load_bounties( )
{
    FILE *fpList;
    char *target;
    char bountylist[256];
    BOUNTY_DATA *bounty;
    long int  amount;
     
    first_disintigration = NULL;
    last_disintigration	= NULL;

    log_string( "Loading disintigrations..." );

    sprintf( bountylist, "%s%s", SYSTEM_DIR, DISINTIGRATION_LIST );
    fclose( fpReserve );
    if ( ( fpList = fopen( bountylist, "r" ) ) == NULL )
    {
	perror( bountylist );
	exit( 1 );
    }

    for ( ; ; )
    {
        target = feof( fpList ) ? "$" : fread_word( fpList );
        if ( target[0] == '$' )
        break;                                  
	CREATE( bounty, BOUNTY_DATA, 1 );
        LINK( bounty, first_disintigration, last_disintigration, next, prev );
	bounty->target = STRALLOC(target);
	amount = fread_number( fpList );
	bounty->amount = amount;
    }
    fclose( fpList );
    log_string(" Done bounties " );
    fpReserve = fopen( NULL_FILE, "r" );

    return;
}

void do_bounties( CHAR_DATA *ch, char *argument )
{
    BOUNTY_DATA *bounty;
    int count = 0;
    
    set_char_color( AT_WHITE, ch );
    send_to_char( "\n\rBounty                      Amount\n\r", ch );
    for ( bounty = first_disintigration; bounty; bounty = bounty->next )
    {
        set_char_color( AT_RED, ch );
        ch_printf( ch, "%-26s %-14ld\n\r", bounty->target, bounty->amount );
        count++;
    }

    if ( !count )
    {
        set_char_color( AT_GREY, ch );
        send_to_char( "There are no bounties set at this time.\n\r", ch );
	return;
    }
}

void disintigration ( CHAR_DATA *ch , CHAR_DATA *victim , long amount )
{
    BOUNTY_DATA *bounty;
    bool found;
    char buf[MAX_STRING_LENGTH];

    found = FALSE;
    
    for ( bounty = first_disintigration; bounty; bounty = bounty->next )
    {
    	if ( !str_cmp( bounty->target , victim->name ))
    	{
    		found = TRUE;
    		break;
    	}
    }        
    
    if (! found)                                            
    {
        CREATE( bounty, BOUNTY_DATA, 1 );
        LINK( bounty, first_disintigration, last_disintigration, next, prev );
    
        bounty->target      = STRALLOC( victim->name );
        bounty->amount      = 0;
    }


/*added by shriak to prevent adding a negative bounty*/
if ((bounty->amount + amount) <= 0)
{
	sprintf(buf,"\r&RHaving that much bounty on them would\r");
	sprintf(buf,"&Rbe unnatural!\r\r");
	return;
}

/*end added by shriak*/
        

    bounty->amount      = bounty->amount + amount;
    save_disintigrations();

    sprintf( buf, "%ld credits has been added to the bounty on %s.", amount , victim->name );
    echo_to_all ( AT_RED , buf, 0 );                    
    
}

void do_addbounty( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_STRING_LENGTH];
    long int amount;
    CHAR_DATA *victim;
             
    if ( !argument || argument[0] == '\0' )
    {
         do_bounties( ch , argument );
         return;
    }
    
    argument = one_argument(argument, arg);
    
    if (argument[0] == '\0' )
    {
    	send_to_char( "Usage: Addbounty <target> <amount>\n\r", ch );
    	return;
    }

    if ( ch->pcdata && ch->pcdata->clan && !str_cmp(ch->pcdata->clan->name, "Hunters-Guild"))
    {
    	send_to_char( "Your job is to collect bounties not post them.", ch );
    	return;
    }
   
/*
	commented out by shriak to make bounty's mud wide
    
    if ( !ch->in_room || ch->in_room->vnum != 6604 )
    {
    	send_to_char( "You will have to go to the Hunters Guild to add a new bounty.", ch );
    	return;
    }

*/
    
    if (argument[0] == '\0' )
        amount = 0;
    else
    	amount = atoi (argument); 

     if ( amount < 250000 && !IS_IMMORTAL(ch))
     {
    	send_to_char( "Don't waste bounty hunter's time.... 250,000 credits MINIMUM.\n\r", ch );
    	return;
    }
    
    if ( !(victim = get_char_world( ch, arg )) )
    {
        send_to_char( "They don't appear to be here .. wait til they log in.\n\r", ch );
        return;
    }

    if ( IS_NPC(victim) )
    {
    	send_to_char( "You can only set bounties on other players .. not mobs!\n\r", ch );
	return;
    }
    if ( victim->pcdata && victim->pcdata->clan && !str_cmp(victim->pcdata->clan->name, "Hunters-Guild"))
    {
        send_to_char( "Posting that bounty is a waste of time, hunters don't hunt their own.\n\r", ch);
        return;
    } 
    if (amount <= 0 && !IS_IMMORTAL(ch))
    {
        send_to_char( "Why are you bounting people? Dumbass...\n\r", ch );
        return;
    }
    
    if (ch->gold < amount && !IS_IMMORTAL(ch))
    {
    	send_to_char( "You don't have that many credits!\n\r", ch );
    	return;
    }
    
    ch->gold = ch->gold - amount;
    
    disintigration( ch, victim, amount);
}

void remove_disintigration( BOUNTY_DATA *bounty )
{
	UNLINK( bounty, first_disintigration, last_disintigration, next, prev );
	STRFREE( bounty->target );
	DISPOSE( bounty );
	
	save_disintigrations();
}

void claim_disintigration( CHAR_DATA *ch , CHAR_DATA *victim )
{
	BOUNTY_DATA *bounty;
	int hgain;
	long int     exp;
	char buf[MAX_STRING_LENGTH];

        if ( IS_NPC(victim) )
            return;

	bounty = get_disintigration( victim->name );
            
        if ( ch == victim )
        {
            if ( bounty != NULL )
               remove_disintigration(bounty);
            return;
	}

 //
 // Comnmented Out, Raijen, 9-20-01
 // Un-Commented out, Shiro 10/12/03
 // 
/*
        if (bounty && 
        (!ch->pcdata || !ch->pcdata->clan || str_cmp(ch->pcdata->clan->name, "Hunters-Guild") ) ) 
	{
	   remove_disintigration(bounty);                	
	   bounty = NULL;
	}
*/	
// **********
// End Comment Out
// **********

            log_string( "Honour Check" );
             if ( !ch->pcdata->status)
		ch->pcdata->status = 0;
             if (!victim->pcdata->status)
		victim->pcdata->status = 1;
	     if  ( ch->pcdata->status <= victim->pcdata->status + 5)
{
	if (!victim->pcdata->status) victim->pcdata->status = 1;
	hgain =   UMAX( 1, victim->pcdata->status);
	
            log_string( "Honour Gain" );
		gain_honour (ch, hgain);
}

	if (bounty == NULL)
	{
	    if ( IS_SET(victim->act , PLR_KILLER ) && !IS_NPC(ch) )
	    {
	       exp = URANGE(1, xp_compute(ch, victim) , ( exp_level(ch->skill_level[HUNTING_ABILITY]+1) - exp_level(ch->skill_level[HUNTING_ABILITY]) ));	
	       gain_exp( ch , exp , HUNTING_ABILITY );
	       set_char_color( AT_BLOOD, ch );
	       ch_printf( ch, "You receive %ld hunting experience for executing a wanted killer.\n\r", exp );
	     }
	     else if ( !IS_NPC(ch) ) 
	     {
	        SET_BIT(ch->act, PLR_KILLER );
	        ch_printf( ch, "You are now wanted for the murder of %s.\n\r", victim->name );
	     }
 
	     
	     sprintf( buf, "%s is Dead!", victim->name );
             echo_to_all ( AT_RED , buf, 0 );
	     return;
	     
	}

		
	ch->gold += bounty->amount;	
	
        exp = URANGE(1, bounty->amount + xp_compute(ch, victim) , ( exp_level(ch->skill_level[HUNTING_ABILITY]+1) - exp_level(ch->skill_level[HUNTING_ABILITY]) ));	
	gain_exp( ch , exp , HUNTING_ABILITY );
        	
	set_char_color( AT_BLOOD, ch );
	ch_printf( ch, "You receive %ld experience and %ld credits,\n\r from the bounty on %s\n\r", exp, bounty->amount, bounty->target );
	
	sprintf( buf, "%s has claimed the disintigration bounty on %s!",ch->name, victim->name );
	echo_to_all ( AT_RED , buf, 0 );
	sprintf( buf, "%s is Dead!", victim->name );
	echo_to_all ( AT_RED , buf, 0 ); 
	
	if ( !IS_SET(victim->act , PLR_KILLER ) )
	       SET_BIT(ch->act, PLR_KILLER );
	remove_disintigration(bounty);                	
}

void do_rembounty(CHAR_DATA *ch, char *argument)
{
  BOUNTY_DATA *bounty;
  char buf[MAX_STRING_LENGTH];
    
  if(IS_NPC(ch))
  { 
    ch_printf(ch, "Huh?\n\r");
    return;
  }     
        
  if(!argument || argument[0] == '\0')
  {     
    ch_printf(ch, "Syntax: rembounty <person>\n\r");
    return;
  }     

  if((bounty = get_disintigration(argument)) == NULL)
  {            
    ch_printf(ch, "No bounties on that person.\n\r");
    return;
  }     

  sprintf( buf, "%s has removed the bounty on %s.", ch->name, bounty->target );
  echo_to_all ( AT_RED , buf, 0 );
  
  remove_disintigration(bounty);
  
  return;
}

/* New Research Target, By Arcturus */
void do_researchtarget(CHAR_DATA *ch, char *argument)
{
    AFFECT_DATA af;
    CHAR_DATA *victim;
    int level, chance;
    if( argument[0] == '\0')
    {
       send_to_char("You need to choose a target.\n\r", ch);
       return;
    }
    if ( ( victim = get_char_world( ch, argument ) ) == NULL
    ||   victim == ch    
    ||   !victim->in_room
    ||  (IS_NPC(victim)) )
    {
        send_to_char( "Your target cannot be found.\n\r", ch );
        return;
    }
    if(IS_NPC(victim))
    {
        send_to_char("This skill doesn't work on NPCs.\n\r", ch);
        return;
    }
    chance = IS_NPC(ch) ? ch->top_level
           : (int) (ch->pcdata->learned[gsn_researchtarget]);
    WAIT_STATE(ch, PULSE_VIOLENCE);
    if (  number_percent() >= chance )
    {
        ch_printf(ch, "You fail to dig up dirt on %s.\n\r", NAME(victim) );
        learn_from_failure(ch, gsn_researchtarget);
        return;
    }

    if ( victim->subclass == SUBCLASS_STEALTH_HUNT || victim->subclass == SUBCLASS_SNEAK )
    {
        ch_printf(ch, "It is impossible to dig up dirt on %s.\n\r", NAME(victim));
        learn_from_success(ch, gsn_researchtarget);
        return;
    }


    if ( IS_IMMORTAL(victim) && (victim->top_level > ch->top_level ) )
    {
       af.type      = gsn_researchtarget;
       af.location  = APPLY_HITROLL;
       af.modifier  = -666;
       af.duration  = 32000;
       af.bitvector = AFF_BLIND;
       affect_to_char( ch, &af );
       set_char_color( AT_MAGIC, victim );
       send_to_char( "You are blinded by your target's immortal aura!\n\r", ch );
       return;
    }
    ch_printf(ch, "&wYour research has concluded the following information:\n\r");
    ch_printf(ch, "&wTitle: %0.74s&w\n\r", victim->pcdata->title);
    ch_printf(ch, "&wRace: %0.20s Class: %0.20s Subclass: %0.20s\n\r", npc_race[victim->race], ability_name[victim->main_ability], subclasses[victim->subclass]);
    if(victim->pcdata->clan)
        ch_printf(ch, "Affiliation: %0.20s\n\r", victim->pcdata->clan_name);
    ch_printf(ch, "Hitpoints: %5.5d/%5.5d Movement: %5.5d/%5.5d\n\r", victim->hit, victim->max_hit, victim->move, victim->max_move);
    ch_printf(ch, "Armor: %4.4d Hitroll: %5.5d Damroll: %5.5d Alignment: %4.4d", GET_AC(victim), GET_HITROLL(victim), 
        GET_DAMROLL(victim), victim->alignment);
    learn_from_success(ch, gsn_researchtarget);
    return;
}
