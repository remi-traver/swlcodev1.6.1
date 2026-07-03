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
*			      Regular update module			   *
****************************************************************************/

#include <sys/types.h>
#include <sys/time.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include "mud.h"

/* from swskills.c */
void    add_reinforcements  args( ( CHAR_DATA *ch ) );

/*
 * Local functions.
 */
int	hit_gain	args( ( CHAR_DATA *ch ) );
int	mana_gain	args( ( CHAR_DATA *ch ) );
int	move_gain	args( ( CHAR_DATA *ch ) );
void    gain_addiction  args( ( CHAR_DATA *ch ) );
void	mobile_update	args( ( void ) );
void	time_update	args( ( void ) );	/* FB */
void	weather_update	args( ( void ) );
void	update_taxes	args( ( void ) );
void	char_update	args( ( void ) );
void	obj_update	args( ( void ) );
void	aggr_update	args( ( void ) );
void	room_act_update	args( ( void ) );
void	obj_act_update	args( ( void ) );
void	char_check	args( ( void ) );
void    drunk_randoms	args( ( CHAR_DATA *ch ) );
void    halucinations	args( ( CHAR_DATA *ch ) );
void	subtract_times	args( ( struct timeval *etime,
				struct timeval *stime ) );

/* weather functions - FB */
void	adjust_vectors		args( ( WEATHER_DATA *weather) );
void	get_weather_echo	args( ( WEATHER_DATA *weather) );
void	get_time_echo		args( ( WEATHER_DATA *weather) );

/*
 * Global Variables
 */

CHAR_DATA *	gch_prev;
OBJ_DATA *	gobj_prev;

CHAR_DATA *	timechar;

char * corpse_descs[] =
   { 
     "The corpse of %s will soon be gone.", 
     "The corpse of %s lies here.",
     "The corpse of %s lies here.",
     "The corpse of %s lies here.",
     "The corpse of %s lies here."
   };

char * d_corpse_descs[] =
   { 
     "The shattered remains %s will soon be gone.", 
     "The shattered remains %s are here.",
     "The shattered remains %s are here.",
     "The shattered remains %s are here.",
     "The shattered remains %s are here."
   };

extern int      top_exit;

/*
 * Advancement stuff.
 */
int max_level( CHAR_DATA *ch, int ability)
{
    int level = 0;
    
    if ( IS_NPC(ch) ) 
      return 100;
      
    if ( IS_IMMORTAL(ch) )
      return 200;
    
    if ( ability == COMBAT_ABILITY )
    {
       if ( ch->main_ability == COMBAT_ABILITY ) level = 100;
       if ( ch->main_ability == HUNTING_ABILITY ) level = 40;
       if ( ch->main_ability == MEDICAL_ABILITY ) level += 30;
       level += ch->perm_con + ch->perm_dex + ch->perm_str;
    }
    
    if ( ability == PILOTING_ABILITY )
    {
       if ( ch->main_ability == ability ) level = 100;
       if ( ch->main_ability == ENGINEERING_ABILITY ) level = 15;
       if ( ch->main_ability == HUNTING_ABILITY ) level = 15;
       if ( ch->main_ability == SMUGGLING_ABILITY ) level = 50;
       level += ch->perm_dex*2;
    }          
    
    if ( ability == ENGINEERING_ABILITY )
    {
       if ( ch->main_ability == ability ) level = 100;
       if ( ch->main_ability == PILOTING_ABILITY ) level = 20;
       level += ch->perm_int * 2;
    }

    if ( ability == HUNTING_ABILITY ) 
    {
       if ( ch->main_ability == ability ) level = 105;
    }

    if ( ability == SMUGGLING_ABILITY )
    {
       if ( ch->main_ability == ability ) level = 100;
       if ( ch->main_ability == PILOTING_ABILITY ) level = 20;
       if ( ch->main_ability == ENGINEERING_ABILITY ) level = 20;
       level += ch->perm_lck*2 + ch->perm_dex;
    }
    
    if ( ability == LEADERSHIP_ABILITY )
    {
       if ( ch->main_ability == ability )    level = 100;
       if ( ch->main_ability == COMBAT_ABILITY ) level = 15;
       if ( ch->main_ability == HUNTING_ABILITY ) level -= 15;
       if ( ch->main_ability == DIPLOMACY_ABILITY ) level = 50;
       level += ch->perm_wis + ch->perm_cha;
    }
    if( ability == DIPLOMACY_ABILITY )
    {
       if(ch->main_ability == ability ) level = 100;
       if(ch->main_ability == LEADERSHIP_ABILITY) level = 50;
       if(ch->main_ability == COMBAT_ABILITY ) level = 15;
       if(ch->main_ability == HUNTING_ABILITY ) level -= 15;
       level += ch->perm_cha + ch->perm_int;
    }
    if( ability == MEDICAL_ABILITY )
    {
       if(ch->main_ability == ability ) level = 100;
       if(ch->main_ability == COMBAT_ABILITY) level = 15;
       if(ch->main_ability == HUNTING_ABILITY) level -= 25;
       level += ch->perm_int + ch->perm_dex;
    }

    if(ch->subclass == SUBCLASS_JACKOFTRADES && ability != FORCE_ABILITY)
      level += 10; 
    level = URANGE( 1, level, 150 );

    if ( ability == FORCE_ABILITY )
    {

       level = 0;
       level += ch->perm_frc *5;
       if ( ch->race == RACE_DROID )
       {
          if(ch->perm_frc >= 1)
             ch->perm_frc = 0;
          level = 0;
       }  
    }          
    return level;
}

/*************
** Commented Out to allow for Gains at levels.
** Raijen
**************
void advance_level( CHAR_DATA *ch , int ability)
{

    if ( ch->top_level < ch->skill_level[ability] && ch->top_level < 100 )
    {
          ch->top_level = URANGE( 1 , ch->skill_level[ability] , 100 );
    }

    if ( !IS_NPC(ch) )
    {
	REMOVE_BIT( ch->act, PLR_BOUGHT_PET );
        do_save( ch, "" );

    }

    return;
}   
*/

void advance_level( CHAR_DATA *ch , int ability)
{
     /***** Redone by Raijen, 9-17-01 *****/
     /* Modified by Arcturus sometime in 9-03 */
     char buf[MAX_STRING_LENGTH];     
     int add_hp;
     int add_mp;
     int add_mv;
     int add_credits;

     //Calculate HP/Mana/Move gain, Based on Stats.

     add_hp	= 10; 
     add_hp +=  con_app[ch->perm_con].hitp; //Add con modifier from const.c's CON table.
     add_mp     = 10;
     add_mp +=  ( ch->perm_int /4 + ch->perm_wis /4); //No specific add mana in the table, so divide the stat by 4
     add_mv     = 10;
     add_mv += ( ch->perm_dex /8 ) + (ch->perm_con /8); //take half of the mana gain, and apply same w/ dex

     // Credits is a bit more complex.  Takes the base, adds 1+charmbonus (from CHA table) for engies,
     // 1+ the INVERSE of the luck modifier/100 for smuggies to get the credit bonus. 

     add_credits = 100 * ch->skill_level[ability];
     if (add_hp <= 0)
        add_hp = 1;
     if (add_mp <= 0)
        add_mp = 1;
     if (add_mv <= 0)
        add_mv = 1;
     if (add_credits <= 0)
        add_credits = 1;

        add_hp = URANGE(1, add_hp, 20);
        add_mv = URANGE(1, add_mv, 20);

 	if (ability == PILOTING_ABILITY)
	{
            add_hp /= 4;
            add_mv /= 2;
	}

 	if (ability == ENGINEERING_ABILITY || ability == MEDICAL_ABILITY)
	{
		// Apply bonus credits based on INT Learn ability.  Also giv4e em double, because they eat materials whiel they level
           add_hp /= 4;
           add_mv /= 4;
	     add_credits = add_credits * (1 + ( ( 
             int_app[get_curr_cha(ch)].learn *2 ) / 100 ) );

            ch->gold += add_credits;
 		sprintf( buf,"You get &R%d&B/&R%d&G&W Credits!\n\r",
 			add_credits,	ch->gold);

	       set_char_color( AT_WHITE, ch );
	       send_to_char( buf, ch );
	}

 	if (ability == HUNTING_ABILITY)
	{
            add_hp /=2;
            add_mv /=2;
	}

 	if (ability == SMUGGLING_ABILITY)
	{
            add_hp /=3;
            add_mv /=3;
            add_credits = add_credits * (1 + ( ( lck_app[get_curr_lck(ch)].luck * (-1) ) / 100 ) ); 
            ch->gold += add_credits;
 		sprintf( buf,"You get &R%d&B/&R%d&G&W Credits!\n\r",
 			add_credits,	ch->gold);

	       set_char_color( AT_WHITE, ch );
	       send_to_char( buf, ch );
	}

	if (ability == LEADERSHIP_ABILITY || ability == DIPLOMACY_ABILITY)
	{
		// Apply bonus credits based on CHA Charm ability.

            add_credits = add_credits * (1 + ( cha_app[get_curr_cha( ch)].charm / 100 ) ); //add 1*charm 
            add_hp /=4;
            add_mv /=4;
            ch->gold += add_credits;
 		sprintf( buf,"You get &R%d&B/&R%d&G&W Credits!\n\r",
 			add_credits,	ch->gold);

	       set_char_color( AT_WHITE, ch );
	       send_to_char( buf, ch );
	}
    
 	if (ability == FORCE_ABILITY)
	{
              add_hp /= 5;
              add_mv /= 5;
	 	ch->max_mana 	+= add_mp;
 		sprintf( buf,"Your gain is: &R%d&B/&R%d&G&W ForcePoints.\n\r",
 			(add_mp),	ch->max_mana);
	       set_char_color( AT_WHITE, ch );
	       send_to_char( buf, ch );
	}/* Just in case its less than 1 after penalties.*/
               add_hp = UMAX(1, add_hp);
               add_mv = UMAX(1, add_mv);          
               ch->max_hit     += add_hp;
               ch->max_move    += add_mv;
               sprintf( buf,"Your gain is: &R%d&B/&R%d&G&W Hit Points. And &R%d&B/&R%d&G&W Move Points\n\r",
               add_hp, ch->max_hit, add_mv, ch->max_move);

               set_char_color( AT_WHITE, ch );
               send_to_char( buf, ch );

	if ( ch->top_level < ch->skill_level[ability] && ch->top_level < 100 )
         {
           ch->top_level = URANGE( 1 , ch->skill_level[ability] , 100 );
         }
     
      if ( !IS_NPC(ch) )
 	do_save(ch, "");
      return;
}

void deadvance_level( CHAR_DATA *ch, int ability)
{
     char buf[MAX_STRING_LENGTH];
     int add_hp;
     int add_mp;
     int add_mv;
      //Calculate HP/Mana/Move loss, Based on Stats.

     add_hp     = 10;
     add_hp +=  con_app[ch->perm_con].hitp; //Add con modifier from const.c's CON table.
     add_mp     = 10;
     add_mp +=  ( ch->perm_int /4 + ch->perm_wis /4); //No specific add mana in the table, so divide the stat by 4
     add_mv     = 10;
     add_mv += ( ch->perm_dex /8 ) + (ch->perm_con /8); //take half of the mana gain, and apply same w/ dex

     if (add_hp <= 0)
        add_hp = 1;
     if (add_mp <= 0)
        add_mp = 1;
     if (add_mv <= 0)
        add_mv = 1;

        add_hp = URANGE(1, add_hp, 20);
        add_mv = URANGE(1, add_mv, 20);

        if (ability == PILOTING_ABILITY)
        {
            add_hp /= 4;
            add_mv /= 2;
        }
        if (ability == ENGINEERING_ABILITY || ability == MEDICAL_ABILITY)
        {
           add_hp /= 4;
           add_mv /= 4;
                set_char_color( AT_WHITE, ch );
               send_to_char( buf, ch );
        }

        if (ability == HUNTING_ABILITY)
        {
            add_hp /=2;
            add_mv /=2;
        }

        if (ability == SMUGGLING_ABILITY)
        {
            add_hp /=3;
            add_mv /=3;
               set_char_color( AT_WHITE, ch );
               send_to_char( buf, ch );
        }

        if (ability == LEADERSHIP_ABILITY || ability == DIPLOMACY_ABILITY)
        {
                // Apply bonus credits based on CHA Charm ability.

            add_hp /=4;
            add_mv /=4;
               set_char_color( AT_WHITE, ch );
               send_to_char( buf, ch );
        }

        if (ability == FORCE_ABILITY)
        {
              add_hp /= 5;
              add_mv /= 5;
                ch->max_mana    -= add_mp;
                sprintf( buf,"Your loss is: &R%d&B/&R%d&G&W ForcePoints.\n\r",
                        (add_mp),       ch->max_mana);
               set_char_color( AT_WHITE, ch );
               send_to_char( buf, ch );
        }/* Just in case its less than 1 after penalties.*/
               add_hp = UMAX(1, add_hp);
               add_mv = UMAX(1, add_mv);
               ch->max_hit     -= add_hp;
               ch->max_move    -= add_mv;
               sprintf( buf,"Your loss is: &R%d&B/&R%d&G&W Hit Points, and &R%d&B/&R%d&G&W Move Points\n\r",
               add_hp, ch->max_hit, add_mv, ch->max_move);

               set_char_color( AT_WHITE, ch );
               send_to_char( buf, ch );
               ch->skill_level[ability]--;
        if ( ch->top_level < ch->skill_level[ability] && ch->top_level < 100 )
         {
           ch->top_level = URANGE( 1 , ch->skill_level[ability] + 1, 100 );
         }
      if ( !IS_NPC(ch) )
        do_save(ch, "");
      return;
}

void gain_exp( CHAR_DATA *ch, int gain , int ability )
{
    
    if ( IS_NPC(ch) )
	return;

    if ( ch->forsaken != 0 )
	gain = gain * 0.1 ;
    if ( ch->blessed != 0 )
	gain = gain * 2.5;

    ch->experience[ability] = UMAX( 0, ch->experience[ability] + gain );

    if (NOT_AUTHED(ch) && ch->experience[ability] >= exp_level(ch->skill_level[ability]+1))
    {
	send_to_char("You can not ascend to a higher level until you are authorized.\n\r", ch);
	ch->experience[ability] = (exp_level( ch->skill_level[ability]+1 ) - 1);
	return;
    }

    
    while ( ch->experience[ability] >= exp_level( ch->skill_level[ability]+1))
    {
        if ( ch->skill_level[ability] >= max_level(ch , ability) )
        {
          ch->experience[ability] = (exp_level( ch->skill_level[ability]+1 ) - 1);
	  return;
        }
	advance_level( ch , ability);
	set_char_color( AT_WHITE + AT_BLINK, ch );
	ch_printf( ch, "You have now obtained %s level %d!\n\r", ability_name[ability], ++ch->skill_level[ability] );
	set_char_color( AT_WHITE, ch );
    }

    return;
}


/*
 * Regeneration stuff.
 */
int hit_gain( CHAR_DATA *ch )
{
    int gain;

    if ( IS_NPC(ch) )
    {
	gain = ch->top_level;
    }
    else
    {
	gain = UMIN( 5, ch->top_level );

	switch ( ch->position )
	{
	case POS_DEAD:	   return 0;
	case POS_MORTAL:   return -25;
	case POS_INCAP:    return -20;
	case POS_STUNNED:  gain += get_curr_con(ch) * 4;
	case POS_SLEEPING: gain += get_curr_con(ch) * 3;	break;
	case POS_RESTING:  gain += get_curr_con(ch) * 2; 	break;
        default: gain += get_curr_con(ch); break;
	}
        //hp regen rates by race
        if ( ch->race == RACE_DUINUOGWUIN )
           gain += 15 + (GET_AGE(ch)/2);
        if ( ch->race == RACE_TRANDOSHAN )
           gain += 25;

	if ( ch->pcdata->condition[COND_FULL]   == 0 )
	    gain /= 2;
	if ( ch->pcdata->condition[COND_THIRST] == 0 )
	    gain /= 2;
    }

    if ( IS_AFFECTED(ch, AFF_POISON) )
	gain /= 4;
    if ( IS_AFFECTED(ch, AFF_REGEN) )
        gain *= 4;
 
   if ( ch->blood < 50 && !IS_NPC(ch))
   {
       send_to_char( "&YYou feel lightheaded from loss of blood.&w\n\r",ch);
       gain /= 2;
   }

    if ( ch->blood < 25 && !IS_NPC(ch))
    {
       send_to_char( "&RThe blood flows from your wounds, and pools on the ground.&w\n\r",ch);
	 make_blood( ch );
       gain /= 4;
    }

    if ( ch->blood < 7 && !IS_NPC(ch))
    {
       send_to_char( "&RThe steady flow of blood prevents any healing from taking place.&w\n\r",ch);
       gain = 0;
    }

    ch->blood = ch->blood + 1;
    ch->blood = URANGE( 0, ch->blood, 100 );
    if ( ch->forsaken != 0 )
	gain /= 2;
    if ( ch->blessed != 0 )
	gain *= 2;
    return UMIN(gain, ch->max_hit - ch->hit);
}



int mana_gain( CHAR_DATA *ch )
{
    int gain;

    if ( IS_NPC(ch) )
    {
	gain = ch->top_level;
    }
    else
    {
        if ( ch->skill_level[FORCE_ABILITY] <= 1 )
          return (0 - ch->mana); 
    
	gain = UMIN( 5, ch->skill_level[FORCE_ABILITY] / 2 );

	if ( ch->position < POS_SLEEPING )
	  return 0;
	switch ( ch->position )
	{
	case POS_SLEEPING: gain += get_curr_int(ch) * 3;	break;
	case POS_RESTING:  gain += get_curr_int(ch) * 2;	break;
        default:           gain += get_curr_int(ch);            break;
	}
        //mana regen
        if ( ch->race == RACE_DUINUOGWUIN )
          gain += 20 + GET_AGE(ch)/2;
        if ( ch->race == RACE_ZABRAK )
           gain += 15;

	if ( ch->pcdata->condition[COND_FULL]   == 0 )
	    gain /= 2;
	if ( ch->pcdata->condition[COND_THIRST] == 0 )
	    gain /= 2;
    }
    if ( IS_AFFECTED( ch, AFF_POISON ) )
	gain /= 4;
    if (ch->forsaken != 0)
	gain /= 2;
    if (ch->blessed != 0)
	gain *= 2;

    return UMIN(gain, ch->max_mana - ch->mana);
}



int move_gain( CHAR_DATA *ch )
{
    int gain;

    if ( IS_NPC(ch) )
    {
	gain = ch->top_level;
    }
    else
    {
	gain = UMAX( 15, 2 * ch->top_level );

	switch ( ch->position )
	{
	case POS_DEAD:	   return 0;
	case POS_MORTAL:   return -1;
	case POS_INCAP:    return -1;
	case POS_STUNNED:  gain += get_curr_dex(ch) * 4;	break;
	case POS_SLEEPING: gain += get_curr_dex(ch) * 3;	break;
	case POS_RESTING:  gain += get_curr_dex(ch) * 2;	break;
        default:           gain += get_curr_dex(ch);		break;
	}
        //move regen
        if ( ch->race == RACE_DUINUOGWUIN )
           gain += 20 + GET_AGE(ch)/2;
        if ( ch->race == RACE_NOGHRI )
           gain += 25;
        
	if ( ch->pcdata->condition[COND_FULL]   == 0 )
	    gain /= 2;
	if ( ch->pcdata->condition[COND_THIRST] == 0 )
	    gain /= 2;
    }

    if ( IS_AFFECTED(ch, AFF_POISON) )
	gain /= 4;
    if ( IS_AFFECTED(ch, AFF_REGEN) )
        gain *= 4;
    if ( ch->blood < 50 && !IS_NPC(ch))
       gain /= 2;
    if ( ch->blood < 25 && !IS_NPC(ch))
       gain /= 4;
    if ( ch->blood < 7 && !IS_NPC(ch))
       gain = 0;
    if (ch->blessed != 0)
	gain *= 2;
    if (ch->forsaken != 0)
	gain /= 2;
   
    return UMIN(gain, ch->max_move - ch->move);
}

void gain_addiction( CHAR_DATA *ch )
{
    short drug;
    ch_ret retcode;
    AFFECT_DATA af;
    
    for ( drug=0 ; drug <= 9 ; drug ++ )
    {
       
       if ( ch->pcdata->addiction[drug] < ch->pcdata->drug_level[drug] )
          ch->pcdata->addiction[drug]++;
  
       if ( ch->pcdata->addiction[drug] > ch->pcdata->drug_level[drug]+150 )
       {
          switch (ch->pcdata->addiction[drug])
          {
              default:
              case SPICE_GLITTERSTIM:
                  if ( !IS_AFFECTED( ch, AFF_BLIND ) )
	          {
	             	af.type      = gsn_blindness;
	      		af.location  = APPLY_AC;
	      		af.modifier  = 10;
	      		af.duration  = ch->pcdata->addiction[drug];
	      		af.bitvector = AFF_BLIND;
	      		affect_to_char( ch, &af );
	   	  }    
              case SPICE_CARSANUM:
                  if ( !IS_AFFECTED( ch, AFF_WEAKEN ) )
	          {
	             	af.type      = -1;
	      		af.location  = APPLY_DAMROLL;
	      		af.modifier  = -10;
	      		af.duration  = ch->pcdata->addiction[drug];
	      		af.bitvector = AFF_WEAKEN;
	      		affect_to_char( ch, &af );
	   	  }
              case SPICE_RYLL:
                  if ( !IS_AFFECTED( ch, AFF_WEAKEN ) )
	          {
	             	af.type      = -1;
	      		af.location  = APPLY_DEX;
	      		af.modifier  = -5;
	      		af.duration  = ch->pcdata->addiction[drug];
	      		af.bitvector = AFF_WEAKEN;
	      		affect_to_char( ch, &af );
	   	  }
              case SPICE_ANDRIS:
                  if ( !IS_AFFECTED( ch, AFF_WEAKEN ) )
	          {
	             	af.type      = -1;
	      		af.location  = APPLY_CON;
	      		af.modifier  = -5;
	      		af.duration  = ch->pcdata->addiction[drug];
	      		af.bitvector = AFF_WEAKEN;
	      		affect_to_char( ch, &af );
	   	  }
              case SPICE_WEED:
                  if ( !IS_AFFECTED( ch, AFF_WEAKEN ) )
	          {
	             	af.type      = -1;
	      		af.location  = APPLY_HIT;
	      		af.modifier  = -5;
	      		af.duration  = ch->pcdata->addiction[drug];
	      		af.bitvector = AFF_WEAKEN;
	      		affect_to_char( ch, &af );
	   	  }
              case SPICE_OPIUM:
                  if ( !IS_AFFECTED( ch, AFF_WEAKEN ) )
	          {
	             	af.type      = -1;
	      		af.location  = APPLY_HIT;
	      		af.modifier  = -5;
	      		af.duration  = ch->pcdata->addiction[drug];
	      		af.bitvector = AFF_WEAKEN;
	      		affect_to_char( ch, &af );
	   	  }
              case SPICE_SPEED:
                  if ( !IS_AFFECTED( ch, AFF_WEAKEN ) )
	          {
	             	af.type      = -1;
	      		af.location  = APPLY_HIT;
	      		af.modifier  = -5;
	      		af.duration  = ch->pcdata->addiction[drug];
	      		af.bitvector = AFF_WEAKEN;
	      		affect_to_char( ch, &af );
	   	  }
              case SPICE_SHROOMS:
                  if ( !IS_AFFECTED( ch, AFF_WEAKEN ) )
	          {
	             	af.type      = -1;
	      		af.location  = APPLY_HIT;
	      		af.modifier  = -5;
	      		af.duration  = ch->pcdata->addiction[drug];
	      		af.bitvector = AFF_WEAKEN;
	      		affect_to_char( ch, &af );
	   	  }
              case SPICE_STEROIDS:
                  if ( !IS_AFFECTED( ch, AFF_WEAKEN ) )
	          {
	             	af.type      = -1;
	      		af.location  = APPLY_HIT;
	      		af.modifier  = -5;
	      		af.duration  = ch->pcdata->addiction[drug];
	      		af.bitvector = AFF_WEAKEN;
	      		affect_to_char( ch, &af );
	   	  }

		case SPICE_EXTACY:
                  if ( !IS_AFFECTED( ch, AFF_WEAKEN ) )
	          {
	             	af.type      = -1;
	      		af.location  = APPLY_HIT;
	      		af.modifier  = -5;
	      		af.duration  = ch->pcdata->addiction[drug];
	      		af.bitvector = AFF_WEAKEN;
	      		affect_to_char( ch, &af );
			ch_printf(ch, "You moan softly as everything you touch makes you horny!\n");
	   	  }
			
          }
       }
  
       if ( ch->pcdata->addiction[drug] > ch->pcdata->drug_level[drug]+200 )
       {      
           ch_printf ( ch, "You feel like you are going to die. You NEED %s\n\r.",   spice_table[drug] );
           worsen_mental_state( ch, 25 );
           retcode = damage(ch, ch, 500, TYPE_UNDEFINED);
       }
       else if ( ch->pcdata->addiction[drug] > ch->pcdata->drug_level[drug]+100 )
       {      
           ch_printf ( ch, "You need some %s.\n\r",   spice_table[drug] );
           worsen_mental_state( ch, 2 );
           retcode = damage(ch, ch, 50, TYPE_UNDEFINED);
       }
       else if ( ch->pcdata->addiction[drug] > ch->pcdata->drug_level[drug]+50 )
       {      
           ch_printf ( ch, "You really crave some %s.\n\r",   spice_table[drug] );
           worsen_mental_state( ch, 1 );
           retcode = damage(ch, ch, 5, TYPE_UNDEFINED);
       }
       else if ( ch->pcdata->addiction[drug] > ch->pcdata->drug_level[drug]+25 )
       {      
           ch_printf ( ch, "Some more %s would feel quite nice.\n\r",   spice_table[drug] );
       }
       else if ( ch->pcdata->addiction[drug] < ch->pcdata->drug_level[drug]-50 )
       {       
           act( AT_POISON, "$n bends over and vomits.\n\r", ch, NULL, NULL, TO_ROOM );
	   act( AT_POISON, "You vomit.\n\r", ch, NULL, NULL, TO_CHAR );
           ch->pcdata->drug_level[drug] -=10;
       }
       
       if ( ch->pcdata->drug_level[drug] > 1 )
          ch->pcdata->drug_level[drug] -=2;
       else if ( ch->pcdata->drug_level[drug] > 0 )
          ch->pcdata->drug_level[drug] -=1;
       else if ( ch->pcdata->addiction[drug] > 0 && ch->pcdata->drug_level[drug] <= 0 )
          ch->pcdata->addiction[drug]--;
    }
    
}

void gain_condition( CHAR_DATA *ch, int iCond, int value )
{
    int condition;
    ch_ret retcode;

    if ( value == 0 || IS_NPC(ch) || get_trust(ch) >= LEVEL_IMMORTAL || NOT_AUTHED(ch))
	return;

    condition	        	    = ch->pcdata->condition[iCond];
    if(ch->pcdata->condition[iCond] > 48)
       return;
    ch->pcdata->condition[iCond]    = URANGE( 0, condition + value, 48 );
    if(ch->race == RACE_DROID)
    {
       if(iCond == COND_DRUNK)
       {
          ch->pcdata->condition[iCond] = 0;
       }
       else
       {
          ch->pcdata->condition[iCond] = 48;
       }
       return;
    }
    if ( ch->pcdata->condition[iCond] == 0 )
    {
	switch ( iCond )
	{
	case COND_FULL:
          if ( ch->top_level <= LEVEL_AVATAR )
          {
            set_char_color( AT_HUNGRY, ch );
	    send_to_char( "You are STARVING!\n\r",  ch );
            act( AT_HUNGRY, "$n is starved half to death!", ch, NULL, NULL, TO_ROOM);
	    worsen_mental_state( ch, 1 );
	    retcode = damage(ch, ch, 5, TYPE_UNDEFINED);
          }
          break;

	case COND_THIRST:
          if ( ch->top_level <= LEVEL_AVATAR )
          {
            set_char_color( AT_THIRSTY, ch );
	    send_to_char( "You are DYING of THIRST!\n\r", ch );
            act( AT_THIRSTY, "$n is dying of thirst!", ch, NULL, NULL, TO_ROOM);
	    worsen_mental_state( ch, 2 );
	    retcode = damage(ch, ch, 5, TYPE_UNDEFINED);
          }
          break;

	case COND_DRUNK:
	    if ( condition != 0 ) {
                set_char_color( AT_SOBER, ch );
		send_to_char( "You are sober.\n\r", ch );
	    }
	    retcode = rNONE;
	    break;
	default:
	    bug( "Gain_condition: invalid condition type %d", iCond );
	    retcode = rNONE;
	    break;
	}
    }

    if ( retcode != rNONE )
      return;

    if ( ch->pcdata->condition[iCond] == 1 )
    {
	switch ( iCond )
	{
	case COND_FULL:
          if ( ch->top_level <= LEVEL_AVATAR )
          {
            set_char_color( AT_HUNGRY, ch );
	    send_to_char( "You are really hungry.\n\r",  ch );
            act( AT_HUNGRY, "You can hear $n's stomach growling.", ch, NULL, NULL, TO_ROOM);
	    if ( number_bits(1) == 0 )
		worsen_mental_state( ch, 1 );
          } 
	  break;

	case COND_THIRST:
          if ( ch->top_level <= LEVEL_AVATAR  )
          {
            set_char_color( AT_THIRSTY, ch );
	    send_to_char( "You are really thirsty.\n\r", ch );
	    worsen_mental_state( ch, 1 );
	    act( AT_THIRSTY, "$n looks a little parched.", ch, NULL, NULL, TO_ROOM);
          } 
	  break;

	case COND_DRUNK:
	    if ( condition != 0 ) {
                set_char_color( AT_SOBER, ch );
		send_to_char( "You are feeling a little less light headed.\n\r", ch );
            }
	    break;
	}
    }


    if ( ch->pcdata->condition[iCond] == 2 )
    {
	switch ( iCond )
	{
	case COND_FULL:
          if ( ch->top_level <= LEVEL_AVATAR )
          {
            set_char_color( AT_HUNGRY, ch );
	    send_to_char( "You are hungry.\n\r",  ch );
          } 
	  break;

	case COND_THIRST:
          if ( ch->top_level <= LEVEL_AVATAR )
          {
            set_char_color( AT_THIRSTY, ch );
	    send_to_char( "You are thirsty.\n\r", ch );
          } 
	  break;

	}
    }

    if ( ch->pcdata->condition[iCond] == 3 )
    {
	switch ( iCond )
	{
	case COND_FULL:
          if ( ch->top_level <= LEVEL_AVATAR )
          {
            set_char_color( AT_HUNGRY, ch );
	    send_to_char( "You are a mite peckish.\n\r",  ch );
          } 
	  break;

	case COND_THIRST:
          if ( ch->top_level <= LEVEL_AVATAR )
          {
            set_char_color( AT_THIRSTY, ch );
	    send_to_char( "You could use a sip of something refreshing.\n\r", ch );
          } 
	  break;

	}
    }
    return;
}



/*
 * Mob autonomous action.
 * This function takes 25% to 35% of ALL Mud cpu time.
 */
void mobile_update( void )
{
    char buf[MAX_STRING_LENGTH];
    CHAR_DATA *ch;
    EXIT_DATA *pexit;
    int door;
    ch_ret     retcode;

    retcode = rNONE;

    /* Examine all mobs. */
    for ( ch = last_char; ch; ch = gch_prev )
    {
	set_cur_char( ch );
	if ( ch == first_char && ch->prev )
	{
	    bug( "mobile_update: first_char->prev != NULL... fixed", 0 );
	    ch->prev = NULL;
	}
	  
	gch_prev = ch->prev;
	
	if ( gch_prev && gch_prev->next != ch )
	{
	    sprintf( buf, "FATAL: Mobile_update: %s->prev->next doesn't point to ch.",
		ch->name );
	    bug( buf, 0 );	    
	    bug( "Short-cutting here", 0 );
	    gch_prev = NULL;
	    ch->prev = NULL;
	    do_shout( ch, "Thoric says, 'Prepare for the worst!'" );
	}

	if ( !IS_NPC(ch) )
	{
	    drunk_randoms(ch);
	    halucinations(ch);
	    continue;
	}

	if ( !ch->in_room
	||   IS_AFFECTED(ch, AFF_CHARM)
	||   IS_AFFECTED(ch, AFF_PARALYSIS) )
	    continue;

/* Clean up 'animated corpses' that are not charmed' - Scryn */

        if ( ch->pIndexData->vnum == 5 && !IS_AFFECTED(ch, AFF_CHARM) )
	{
	  if(ch->in_room->first_person)
	    act(AT_MAGIC, "$n returns to the dust from whence $e came.", ch, NULL, NULL, TO_ROOM);
          
   	  if(IS_NPC(ch)) /* Guard against purging switched? */
	    extract_char(ch, TRUE);
	  continue;
	}

	if ( !IS_SET( ch->act, ACT_RUNNING )
	&&   !IS_SET( ch->act, ACT_SENTINEL )
	&&   !ch->fighting && ch->hunting )
	{
	  if (  ch->top_level < 20 )
	   WAIT_STATE( ch, 6 * PULSE_PER_SECOND );
	  else	if (  ch->top_level < 40 )
	   WAIT_STATE( ch, 5 * PULSE_PER_SECOND );
	  else if (  ch->top_level < 60 )
	   WAIT_STATE( ch, 4 * PULSE_PER_SECOND );
	  else	if (  ch->top_level < 80 )
	   WAIT_STATE( ch, 3 * PULSE_PER_SECOND );
	  else	if (  ch->top_level < 100 )
	   WAIT_STATE( ch, 2 * PULSE_PER_SECOND );
	  else
	   WAIT_STATE( ch, 1 * PULSE_PER_SECOND );
	  hunt_victim( ch );
	  continue;
	}  
        else if ( !ch->fighting && !ch->hunting 
        && !IS_SET( ch->act, ACT_RUNNING)
        && ch->was_sentinel && ch->position >= POS_STANDING )
	{
	   act( AT_ACTION, "$n leaves.", ch, NULL, NULL, TO_ROOM );
	   char_from_room( ch );
	   char_to_room( ch , ch->was_sentinel );
	   act( AT_ACTION, "$n arrives.", ch, NULL, NULL, TO_ROOM );
	   SET_BIT( ch->act , ACT_SENTINEL );            
	   ch->was_sentinel = NULL;
	}
	
	/* Examine call for special procedure */
	if ( !IS_SET( ch->act, ACT_RUNNING )
	&&    ch->spec_fun )
	{
	    if ( (*ch->spec_fun) ( ch ) )
		continue;
	    if ( char_died(ch) )
		continue;
	}
        
        if ( !IS_SET( ch->act, ACT_RUNNING )
	&&    ch->spec_2 )
	{
	    if ( (*ch->spec_2) ( ch ) )
		continue;
	    if ( char_died(ch) )
		continue;
	}

	/* Check for mudprogram script on mob */
	if ( IS_SET( ch->pIndexData->progtypes, SCRIPT_PROG ) )
	{
	    mprog_script_trigger( ch );
	    continue;
	}

	if ( ch != cur_char )
	{
	    bug( "Mobile_update: ch != cur_char after spec_fun", 0 );
	    continue;
	}

	/* That's all for sleeping / busy monster */
	if ( ch->position != POS_STANDING )
	    continue;
        
        
	if ( IS_SET(ch->act, ACT_MOUNTED ) )
	{
	    if ( IS_SET(ch->act, ACT_AGGRESSIVE) )
		do_emote( ch, "snarls and growls." );
	    continue;
	}

	if ( IS_SET(ch->in_room->room_flags, ROOM_SAFE )
	&&   IS_SET(ch->act, ACT_AGGRESSIVE) )
	    do_emote( ch, "glares around and snarls." );


	/* MOBprogram random trigger */
	if ( ch->in_room->area->nplayer > 0 )
	{
	    mprog_random_trigger( ch );
	    if ( char_died(ch) )
		continue;
	    if ( ch->position < POS_STANDING )
	        continue;
	}

        /* MOBprogram hour trigger: do something for an hour */
        mprog_hour_trigger(ch);

	if ( char_died(ch) )
	  continue;

	rprog_hour_trigger(ch);
	if ( char_died(ch) )
	  continue;

	if ( ch->position < POS_STANDING )
	  continue;

	/* Scavenge */
	if ( IS_SET(ch->act, ACT_SCAVENGER)
	&&   ch->in_room->first_content
	&&   number_bits( 2 ) == 0 )
	{
	    OBJ_DATA *obj;
	    OBJ_DATA *obj_best;
	    int max;

	    max         = 1;
	    obj_best    = NULL;
	    for ( obj = ch->in_room->first_content; obj; obj = obj->next_content )
	    {
		if ( CAN_WEAR(obj, ITEM_TAKE) && obj->cost > max 
		&& !IS_OBJ_STAT( obj, ITEM_BURRIED ) )
		{
		    obj_best    = obj;
		    max         = obj->cost;
		}
	    }

	    if ( obj_best )
	    {
		obj_from_room( obj_best );
		obj_to_char( obj_best, ch );
		act( AT_ACTION, "$n gets $p.", ch, obj_best, NULL, TO_ROOM );
	    }
	}

	/* Wander */
	if ( !IS_SET(ch->act, ACT_RUNNING)
	&&   !IS_SET(ch->act, ACT_SENTINEL)
	&&   !IS_SET(ch->act, ACT_PROTOTYPE)
	&& ( door = number_bits( 5 ) ) <= 9
	&& ( pexit = get_exit(ch->in_room, door) ) != NULL
	&&   pexit->to_room
	&&   !IS_SET(pexit->exit_info, EX_CLOSED)
	&&   !IS_SET(pexit->to_room->room_flags, ROOM_NO_MOB)
	&& ( !IS_SET(ch->act, ACT_STAY_AREA)
	||   pexit->to_room->area == ch->in_room->area ) )
	{
	    retcode = move_char( ch, pexit, 0 );
						/* If ch changes position due
						to it's or someother mob's
						movement via MOBProgs,
						continue - Kahn */
	    if ( char_died(ch) )
	      continue;
	    if ( retcode != rNONE || IS_SET(ch->act, ACT_SENTINEL)
	    ||    ch->position < POS_STANDING )
	        continue;
	}

	/* Flee */
	if ( ch->hit < ch->max_hit / 2
	&& ( door = number_bits( 4 ) ) <= 9
	&& ( pexit = get_exit(ch->in_room,door) ) != NULL
	&&   pexit->to_room
	&&   !IS_SET(pexit->exit_info, EX_CLOSED)
	&&   !IS_SET(pexit->to_room->room_flags, ROOM_NO_MOB) )
	{
	    CHAR_DATA *rch;
	    bool found;

	    found = FALSE;
	    for ( rch  = ch->in_room->first_person;
		  rch;
		  rch  = rch->next_in_room )
	    {
		if ( is_fearing(ch, rch) )
		{
		    switch( number_bits(2) )
		    {
			case 0:
			  sprintf( buf, "Get away from me, %s!", rch->name );
			  break;
			case 1:
			  sprintf( buf, "Leave me be, %s!", rch->name );
			  break;
			case 2:
			  sprintf( buf, "%s is trying to kill me!  Help!", rch->name );
			  break;
			case 3:
			  sprintf( buf, "Someone save me from %s!", rch->name );
			  break;
		    }
		    do_yell( ch, buf );
		    found = TRUE;
		    break;
		}
	    }
	    if ( found )
		retcode = move_char( ch, pexit, 0 );
	}
    }

    return;
}

void update_taxes( void )
{
     PLANET_DATA *planet;
     CLAN_DATA *clan;
     int i;

    for ( planet = first_planet; planet; planet = planet->next )
    {
        for( i = 1; i < CARGO_MAX; i++)
        {
           planet->resource[i] += planet->produces[i];
           planet->resource[i] -= planet->consumes[i];
           if(planet->resource[i] < 0)
              planet->resource[i] = 0;
        }
      
        save_planet(planet);
        clan = planet->governed_by;
        if ( clan )
        {
            int sCount = 0;
            CLAN_DATA * subclan = NULL;
            
            if ( clan->first_subclan )
            {
               for ( subclan = clan->first_subclan ; subclan ; subclan = subclan->next_subclan )
   		  sCount++;
   	       
   	       for ( subclan = clan->first_subclan ; subclan ; subclan = subclan->next_subclan )
   	       {
   	       	  subclan->funds += get_taxes(planet)/50/sCount;
                  save_clan (subclan);
               }
               
               clan->funds += get_taxes(planet)/50;
               save_clan (clan);
            }  
            else
            {   
               clan->funds += get_taxes(planet)/25;
               save_clan( clan );
            }
            save_planet( planet );
        }
    }
    
}


/*
 * function updates weather for each area
 * Last Modified: July 31, 1997
 * Fireblade
 */
void weather_update()
{
	AREA_DATA *pArea;
	DESCRIPTOR_DATA *d;
	int limit;
	
	limit = 3 * weath_unit;
	
	for(pArea = first_area; pArea;
		pArea = (pArea == last_area) ? first_build : pArea->next)
	{
		/* Apply vectors to fields */
		pArea->weather->temp +=
			pArea->weather->temp_vector;
		pArea->weather->precip +=
			pArea->weather->precip_vector;
		pArea->weather->wind +=
			pArea->weather->wind_vector;
		
		/* Make sure they are within the proper range */
		pArea->weather->temp = URANGE(-limit,
			pArea->weather->temp, limit);
		pArea->weather->precip = URANGE(-limit,
			pArea->weather->precip, limit);
		pArea->weather->wind = URANGE(-limit,
			pArea->weather->wind, limit);
		
		/* get an appropriate echo for the area */
		get_weather_echo(pArea->weather);
	}

	for(pArea = first_area; pArea;
		pArea = (pArea == last_area) ? first_build : pArea->next)
	{
		adjust_vectors(pArea->weather);
	}
	
	/* display the echo strings to the appropriate players */
	for(d = first_descriptor; d; d = d->next)
	{
		WEATHER_DATA *weath;
		
		if(d->connected == CON_PLAYING &&
			IS_OUTSIDE(d->character) &&
			!NO_WEATHER_SECT(d->character->in_room->sector_type) &&
			IS_AWAKE(d->character))
		{
			weath = d->character->in_room->area->weather;
			if(!weath->echo)
				continue;
			set_char_color(weath->echo_color, d->character);
			ch_printf(d->character, weath->echo);
		}
	}
	
	return;
}
/*
 * get weather echo messages according to area weather...
 * stores echo message in weath_data.... must be called before
 * the vectors are adjusted
 * Last Modified: August 10, 1997
 * Fireblade
 */
void get_weather_echo(WEATHER_DATA *weath)
{
	int n;
	int temp, precip, wind;
	int dT, dP, dW;
	int tindex, pindex, windex;
	
	/* set echo to be nothing */
	weath->echo = NULL;
	weath->echo_color = AT_GREY;
	
	/* get the random number */
	n = number_bits(2);
	
	/* variables for convenience */
	temp = weath->temp;
	precip = weath->precip;
	wind = weath->wind;

	dT = weath->temp_vector;
	dP = weath->precip_vector;
	dW = weath->wind_vector;
	
	tindex = (temp + 3*weath_unit - 1)/weath_unit;
	pindex = (precip + 3*weath_unit - 1)/weath_unit;
	windex = (wind + 3*weath_unit - 1)/weath_unit;
	
	/* get the echo string... mainly based on precip */
	switch(pindex)
	{
		case 0:
			if(precip - dP > -2*weath_unit)
			{
				char *echo_strings[4] =
				{
					"The clouds disappear.\n\r",
					"The clouds disappear.\n\r",
					"The sky begins to break through "
						"the clouds.\n\r",
					"The clouds are slowly "
						"evaporating.\n\r"
				};
				
				weath->echo = echo_strings[n];
				weath->echo_color = AT_WHITE;
			}
			break;

		case 1:
			if(precip - dP <= -2*weath_unit)
			{
				char *echo_strings[4] =
				{
					"The sky is getting cloudy.\n\r",
					"The sky is getting cloudy.\n\r",
					"Light clouds cast a haze over "
						"the sky.\n\r",
					"Billows of clouds spread through "
						"the sky.\n\r"
				};
				weath->echo = echo_strings[n];
				weath->echo_color = AT_GREY;
			}
			break;
			
		case 2:
			if(precip - dP > 0)
			{
				if(tindex > 1)
				{
					char *echo_strings[4] =
					{
						"The rain stops.\n\r",
						"The rain stops.\n\r",
						"The rainstorm tapers "
							"off.\n\r",
						"The rain's intensity "
							"breaks.\n\r"
					};
					weath->echo = echo_strings[n];
					weath->echo_color = AT_CYAN;
				}
				else
				{
					char *echo_strings[4] =
					{
						"The snow stops.\n\r",
						"The snow stops.\n\r",
						"The snow showers taper "
							"off.\n\r",
						"The snow flakes disappear "
							"from the sky.\n\r"
					};
					weath->echo = echo_strings[n];
					weath->echo_color = AT_WHITE;
				}
			}
			break;
			
		case 3:
			if(precip - dP <= 0)
			{
				if(tindex > 1)
				{
					char *echo_strings[4] =
					{
						"It starts to rain.\n\r",
						"It starts to rain.\n\r",
						"A droplet of rain falls "
							"upon you.\n\r",
						"The rain begins to "
							"patter.\n\r"
					};
					weath->echo = echo_strings[n];
					weath->echo_color = AT_CYAN;
				}
				else
				{
					char *echo_strings[4] =
					{
						"It starts to snow.\n\r",
						"It starts to snow.\n\r",
						"Crystal flakes begin to "
							"fall from the "
							"sky.\n\r",
						"Snow flakes drift down "
							"from the clouds.\n\r"
					};
					weath->echo = echo_strings[n];
					weath->echo_color = AT_WHITE;
				}
			}
			else if(tindex < 2 && temp - dT > -weath_unit)
			{
				char *echo_strings[4] =
				{
					"The temperature drops and the rain "
						"becomes a light snow.\n\r",
					"The temperature drops and the rain "
						"becomes a light snow.\n\r",
					"Flurries form as the rain freezes.\n\r",
					"Large snow flakes begin to fall "
						"with the rain.\n\r"
				};
				weath->echo = echo_strings[n];
				weath->echo_color = AT_WHITE;
			}
			else if(tindex > 1 && temp - dT <= -weath_unit)
			{
				char *echo_strings[4] =
				{
					"The snow flurries are gradually "
						"replaced by pockets of rain.\n\r",
					"The snow flurries are gradually "
						"replaced by pockets of rain.\n\r",
					"The falling snow turns to a cold drizzle.\n\r",
					"The snow turns to rain as the air warms.\n\r"
				};
				weath->echo = echo_strings[n];
				weath->echo_color = AT_CYAN;
			} 
			break;
		
		case 4:
			if(precip - dP > 2*weath_unit)
			{
				if(tindex > 1)
				{
					char *echo_strings[4] =
					{
						"The lightning has stopped.\n\r",
						"The lightning has stopped.\n\r",
						"The sky settles, and the "
							"thunder surrenders.\n\r",
						"The lightning bursts fade as "
							"the storm weakens.\n\r"
					};
					weath->echo = echo_strings[n];
					weath->echo_color = AT_GREY;
				}
			}
			else if(tindex < 2 && temp - dT > -weath_unit)
			{
				char *echo_strings[4] =
				{
					"The cold rain turns to snow.\n\r",
					"The cold rain turns to snow.\n\r",
					"Snow flakes begin to fall "
						"amidst the rain.\n\r",
					"The driving rain begins to freeze.\n\r"
				};
				weath->echo = echo_strings[n];
				weath->echo_color = AT_WHITE;
			}
			else if(tindex > 1 && temp - dT <= -weath_unit)
			{
				char *echo_strings[4] =
				{
					"The snow becomes a freezing rain.\n\r",
					"The snow becomes a freezing rain.\n\r",
					"A cold rain beats down on you "
						"as the snow begins to melt.\n\r",
					"The snow is slowly replaced by a heavy "
						"rain.\n\r"
				};
				weath->echo = echo_strings[n];
				weath->echo_color = AT_CYAN;
			}
			break;
			
		case 5:
			if(precip - dP <= 2*weath_unit)
			{
				if(tindex > 1)
				{
					char *echo_strings[4] =
					{
						"Lightning flashes in the "
							"sky.\n\r",
						"Lightning flashes in the "
							"sky.\n\r",
						"A flash of lightning splits "
							"the sky.\n\r",
						"The sky flashes, and the "
							"ground trembles with "
							"thunder.\n\r"
					};
					weath->echo = echo_strings[n];
					weath->echo_color = AT_YELLOW;
				}
			}
			else if(tindex > 1 && temp - dT <= -weath_unit)
			{
				char *echo_strings[4] =
				{
					"The sky rumbles with thunder as "
						"the snow changes to rain.\n\r",
					"The sky rumbles with thunder as "
						"the snow changes to rain.\n\r",
					"The falling turns to freezing rain "
						"amidst flashes of "
						"lightning.\n\r",
					"The falling snow begins to melt as "
						"thunder crashes overhead.\n\r"
				};
				weath->echo = echo_strings[n];
				weath->echo_color = AT_WHITE;
			}
			else if(tindex < 2 && temp - dT > -weath_unit)
			{
				char *echo_strings[4] =
				{
					"The lightning stops as the rainstorm "
						"becomes a blinding "
						"blizzard.\n\r",
					"The lightning stops as the rainstorm "
						"becomes a blinding "
						"blizzard.\n\r",
					"The thunder dies off as the "
						"pounding rain turns to "
						"heavy snow.\n\r",
					"The cold rain turns to snow and "
						"the lightning stops.\n\r"
				};
				weath->echo = echo_strings[n];
				weath->echo_color = AT_CYAN;
			}
			break;
			
		default:
			bug("echo_weather: invalid precip index");
			weath->precip = 0;
			break;
	}

	return;
}

/*
 * get echo messages according to time changes...
 * some echoes depend upon the weather so an echo must be
 * found for each area
 * Last Modified: August 10, 1997
 * Fireblade
 */
void get_time_echo(WEATHER_DATA *weath)
{
	int n;
	int pindex;
	
	n = number_bits(2);
	pindex = (weath->precip + 3*weath_unit - 1)/weath_unit;
	weath->echo = NULL;
	weath->echo_color = AT_GREY;
	
	switch(time_info.hour)
	{
		case 5:
		{
			char *echo_strings[4] =
			{
				"The day has begun.\n\r",
				"The day has begun.\n\r",
				"The sky slowly begins to glow.\n\r",
				"The sun slowly embarks upon a new day.\n\r"
			};
			time_info.sunlight = SUN_RISE;
			weath->echo = echo_strings[n];
			weath->echo_color = AT_YELLOW;
			break;
		}
		case 6:
		{
			char *echo_strings[4] =
			{
				"The sun rises in the east.\n\r",
				"The sun rises in the east.\n\r",
				"The hazy sun rises over the horizon.\n\r",
				"Day breaks as the sun lifts into the sky.\n\r"
			};
			time_info.sunlight = SUN_LIGHT;
			weath->echo = echo_strings[n];
			weath->echo_color = AT_ORANGE;
			break;
		}
		case 12:
		{
			if(pindex > 0)
			{
				weath->echo = "It's noon.\n\r";
			}
			else
			{
				char *echo_strings[2] =
				{
					"The intensity of the sun "
						"heralds the noon hour.\n\r",
					"The sun's bright rays beat down "
						"upon your shoulders.\n\r"
				};
				weath->echo = echo_strings[n%2];
			}
			time_info.sunlight = SUN_LIGHT;
			weath->echo_color = AT_WHITE;
			break;
		}
		case 19:
		{
			char *echo_strings[4] =
			{
				"The sun slowly disappears in the west.\n\r",
				"The reddish sun sets past the horizon.\n\r",
				"The sky turns a reddish orange as the sun "
					"ends its journey.\n\r",
				"The sun's radiance dims as it sinks in the "
					"sky.\n\r"
			};
			time_info.sunlight = SUN_SET;
			weath->echo = echo_strings[n];
			weath->echo_color = AT_RED;
			break;
		}
		case 20:
		{
			if(pindex > 0)
			{
				char *echo_strings[2] =
				{
					"The night begins.\n\r",
					"Twilight descends around you.\n\r"
				};
				weath->echo = echo_strings[n%2];
			}
			else
			{
				char *echo_strings[2] =
				{
					"The moon's gentle glow diffuses "
						"through the night sky.\n\r",
					"The night sky gleams with "
						"glittering starlight.\n\r"
				};
				weath->echo = echo_strings[n%2];
			}
			time_info.sunlight = SUN_DARK;
			weath->echo_color = AT_DBLUE;
			break;
		}
	}
	
	return;
}

/*
 * Update all chars, including mobs.
 * This function is performance sensitive.
 */
void char_update( void )
{   
    CHAR_DATA *ch;
    CHAR_DATA *ch_save;
    sh_int save_count = 0;

    ch_save	= NULL;
    for ( ch = last_char; ch; ch = gch_prev )
    {
	if ( ch == first_char && ch->prev )
	{
	    bug( "char_update: first_char->prev != NULL... fixed", 0 );
	    ch->prev = NULL;
	}
	gch_prev = ch->prev;
	set_cur_char( ch );
	if ( gch_prev && gch_prev->next != ch )
	{
	    bug( "char_update: ch->prev->next != ch", 0 );
	    return;
	}

        /*
	 *  Do a room_prog rand check right off the bat
	 *   if ch disappears (rprog might wax npc's), continue
	 */
	if(!IS_NPC(ch))
	    rprog_random_trigger( ch );

	if( char_died(ch) )
	    continue;

	if(IS_NPC(ch))
	    mprog_time_trigger(ch);   

	if( char_died(ch) )
	    continue;

	rprog_time_trigger(ch);

	if( char_died(ch) )
	    continue;

	/*
	 * See if player should be auto-saved.
	 */
	if ( !IS_NPC(ch)
	&&    !NOT_AUTHED(ch)
	&&    current_time - ch->save_time > (sysdata.save_frequency*60) )
	    ch_save	= ch;
	else
	    ch_save	= NULL;

       if (!IS_NPC(ch) && !IS_IMMORTAL(ch))
{
       if ( ch->pcdata->clan && ch->lastpaid <= (ch->played + (int) (current_time - ch->logon) - 3600 ))
       {
        if (ch->pcdata->clan->funds < (ch->pcdata->clan->wage * ch->pcdata->clanlevel))
	{
	  send_to_char( "&RYour Clan Cannot Afford To Pay You this Pay Period!\n\r", ch );
	}
        else
	{
          ch_printf(ch, "&GYour clan pays your wages of %d Credits!!!&w\n\r",
            ch->pcdata->clan->wage * ch->pcdata->clanlevel);
          ch->pcdata->clan->funds -= (ch->pcdata->clan->wage * ch->pcdata->clanlevel);
          ch->gold += (ch->pcdata->clan->wage * ch->pcdata->clanlevel);
	}
        ch->lastpaid = ch->played + (int) (current_time - ch->logon);

       }
}
	if ( ch->position >= POS_STUNNED )
	{
	    if ( ch->hit  < ch->max_hit )
		ch->hit  += hit_gain(ch);

	    if ( ch->mana < ch->max_mana || ch->skill_level[FORCE_ABILITY] == 1 )
		ch->mana += mana_gain(ch);

	    if ( ch->move < ch->max_move )
		ch->move += move_gain(ch);
	}

	if ( ch->position == POS_STUNNED )
	    update_pos( ch );
        
        if ( ch->pcdata )
              gain_addiction( ch );
            
        
	if ( !IS_NPC(ch) && ch->top_level < LEVEL_IMMORTAL )
	{
	    OBJ_DATA *obj;

	    if ( ( obj = get_eq_char( ch, WEAR_LIGHT ) ) != NULL
	    &&   obj->item_type == ITEM_LIGHT
	    &&   obj->value[2] > 0 )
	    {
		if ( --obj->value[2] == 0 && ch->in_room )
		{
		    ch->in_room->light -= obj->count;
		    act( AT_ACTION, "$p goes out.", ch, obj, NULL, TO_ROOM );
		    act( AT_ACTION, "$p goes out.", ch, obj, NULL, TO_CHAR );
		    if ( obj->serial == cur_obj )
		      global_objcode = rOBJ_EXPIRED;
		    extract_obj( obj );
		}
	    }

	    if ( ch->pcdata->condition[COND_DRUNK] > 8 )
		worsen_mental_state( ch, ch->pcdata->condition[COND_DRUNK]/8 );
	    if ( ch->pcdata->condition[COND_FULL] > 1 )
	    {
		switch( ch->position )
		{
		    case POS_SLEEPING:  better_mental_state( ch, 4 );	break;
		    case POS_RESTING:   better_mental_state( ch, 3 );	break;
		    case POS_SITTING:
		    case POS_MOUNTED:   better_mental_state( ch, 2 );	break;
		    case POS_STANDING:  better_mental_state( ch, 1 );	break;
		    case POS_FIGHTING:
			if ( number_bits(2) == 0 )
			    better_mental_state( ch, 1 );
			break;
		}
	    }
	    if ( ch->pcdata->condition[COND_THIRST] > 1 )
	    {
		switch( ch->position )
		{
		    case POS_SLEEPING:  better_mental_state( ch, 5 );	break;
		    case POS_RESTING:   better_mental_state( ch, 3 );	break;
		    case POS_SITTING:
		    case POS_MOUNTED:   better_mental_state( ch, 2 );	break;
		    case POS_STANDING:  better_mental_state( ch, 1 );	break;
		    case POS_FIGHTING:
			if ( number_bits(2) == 0 )
			    better_mental_state( ch, 1 );
			break;
		}
	    }
	    gain_condition( ch, COND_DRUNK,  -1 );
	    gain_condition( ch, COND_FULL,   -1 );
	    if ( ch->in_room )
	      switch( ch->in_room->sector_type )
	      {
		default:
		    gain_condition( ch, COND_THIRST, -1 );  break;
		case SECT_DESERT:
		    gain_condition( ch, COND_THIRST, -2 );  break;
		case SECT_UNDERWATER:
		case SECT_OCEANFLOOR:
		    if ( number_bits(1) == 0 )
			gain_condition( ch, COND_THIRST, -1 );  break;
	    }

	}

	if ( !char_died(ch) )
	{
	    /*
	     * Careful with the damages here,
	     *   MUST NOT refer to ch after damage taken,
	     *   as it may be lethal damage (on NPC).
	     */
	    if ( IS_AFFECTED(ch, AFF_POISON) )
	    {
		act( AT_POISON, "$n shivers and suffers.", ch, NULL, NULL, TO_ROOM );
		act( AT_POISON, "You shiver and suffer.", ch, NULL, NULL, TO_CHAR );
		ch->mental_state = URANGE( 20, ch->mental_state
				 + 4 , 100 );
		damage( ch, ch, 6, gsn_poison );
	    }
	    else
	    if ( ch->position == POS_INCAP )
		damage( ch, ch, 1, TYPE_UNDEFINED );
	    else
	    if ( ch->position == POS_MORTAL )
		damage( ch, ch, 4, TYPE_UNDEFINED );
	    if ( char_died(ch) )
		continue;
	    if ( ch->mental_state >= 30 )
		switch( (ch->mental_state+5) / 10 )
		{
		    case  3:
		    	send_to_char( "You feel feverish.\n\r", ch );
			act( AT_ACTION, "$n looks kind of out of it.", ch, NULL, NULL, TO_ROOM );
		    	break;
		    case  4:
		    	send_to_char( "You do not feel well at all.\n\r", ch );
			act( AT_ACTION, "$n doesn't look too good.", ch, NULL, NULL, TO_ROOM );
		    	break;
		    case  5:
		    	send_to_char( "You need help!\n\r", ch );
			act( AT_ACTION, "$n looks like $e could use your help.", ch, NULL, NULL, TO_ROOM );
		    	break;
		    case  6:
		    	send_to_char( "Seekest thou a cleric.\n\r", ch );
			act( AT_ACTION, "Someone should fetch a healer for $n.", ch, NULL, NULL, TO_ROOM );
		    	break;
		    case  7:
		    	send_to_char( "You feel reality slipping away...\n\r", ch );
			act( AT_ACTION, "$n doesn't appear to be aware of what's going on.", ch, NULL, NULL, TO_ROOM );
		    	break;
		    case  8:
		    	send_to_char( "You begin to understand... everything.\n\r", ch );
			act( AT_ACTION, "$n starts ranting like a madman!", ch, NULL, NULL, TO_ROOM );
		    	break;
		    case  9:
		    	send_to_char( "You are ONE with the universe.\n\r", ch );
			act( AT_ACTION, "$n is ranting on about 'the answer', 'ONE' and other mumbo-jumbo...", ch, NULL, NULL, TO_ROOM );
		    	break;
		    case 10:
		    	send_to_char( "You feel the end is near.\n\r", ch );
			act( AT_ACTION, "$n is muttering and ranting in tongues...", ch, NULL, NULL, TO_ROOM );
		    	break;
		}
	    if ( ch->mental_state <= -30 )
		switch( (abs(ch->mental_state)+5) / 10 )
		{
		    case  10:
			if ( ch->position > POS_SLEEPING )
			{
			   if ( (ch->position == POS_STANDING
			   ||    ch->position < POS_FIGHTING)
			   &&    number_percent()+10 < abs(ch->mental_state) )
				do_sleep( ch, "" );
			   else
				send_to_char( "You're barely conscious.\n\r", ch );
			}
			break;
		    case   9:
			if ( ch->position > POS_SLEEPING )
			{
			   if ( (ch->position == POS_STANDING
			   ||    ch->position < POS_FIGHTING)
			   &&   (number_percent()+20) < abs(ch->mental_state) )
				do_sleep( ch, "" );
			   else
				send_to_char( "You can barely keep your eyes open.\n\r", ch );
			}
			break;
		    case   8:
			if ( ch->position > POS_SLEEPING )
			{
			   if ( ch->position < POS_SITTING
			   &&  (number_percent()+30) < abs(ch->mental_state) )
				do_sleep( ch, "" );
			   else
				send_to_char( "You're extremely drowsy.\n\r", ch );
			}
			break;
		    case   7:
			if ( ch->position > POS_RESTING )
			   send_to_char( "You feel very unmotivated.\n\r", ch );
			break;
		    case   6:
			if ( ch->position > POS_RESTING )
			   send_to_char( "You feel sedated.\n\r", ch );
			break;
		    case   5:
			if ( ch->position > POS_RESTING )
			   send_to_char( "You feel sleepy.\n\r", ch );
			break;
		    case   4:
			if ( ch->position > POS_RESTING )
			   send_to_char( "You feel tired.\n\r", ch );
			break;
		    case   3:
			if ( ch->position > POS_RESTING )
			   send_to_char( "You could use a rest.\n\r", ch );
			break;
		}
	    
	    if ( ch->backup_wait > 0 )
	    {
	       --ch->backup_wait;
	       if ( ch->backup_wait == 0 )
	          add_reinforcements( ch );
	    }
	    
	    if ( !IS_NPC (ch) )
	    {
	     if ( ++ch->timer > 15 && !ch->desc )
             {
                if ( ch->in_room )
		     char_from_room( ch );
		char_to_room( ch , get_room_index( ROOM_PLUOGUS_QUIT ) );      
                ch->position = POS_RESTING;
                ch->hit = UMAX ( 1 , ch->hit ); 
                save_char_obj( ch );
        	do_quit( ch, "" );
             }
	     else
	     if ( ch == ch_save && IS_SET( sysdata.save_flags, SV_AUTO )
	     &&   ++save_count < 10 )	/* save max of 10 per tick */
		save_char_obj( ch );
	    }
	}
     
    }

    return;
}



/*
 * Update all objs.
 * This function is performance sensitive.
 */
void obj_update( void )
{   
    OBJ_DATA *obj;
    sh_int AT_TEMP;
    	        
    for ( obj = last_object; obj; obj = gobj_prev )
    {
	CHAR_DATA *rch;
	char *message;

	if ( obj == first_object && obj->prev )
	{
	    bug( "obj_update: first_object->prev != NULL... fixed", 0 );
	    obj->prev = NULL;
	}
	gobj_prev = obj->prev;
	if ( gobj_prev && gobj_prev->next != obj )
	{
	    bug( "obj_update: obj->prev->next != obj", 0 );
	    return;
	}
	set_cur_obj( obj );
	if ( obj->carried_by )
	  oprog_random_trigger( obj ); 
	else
	if( obj->in_room && obj->in_room->area->nplayer > 0 )
	  oprog_random_trigger( obj ); 

        if( obj_extracted(obj) )
	  continue;

        if ( obj->item_type == ITEM_WEAPON && obj->carried_by  &&
             ( obj->wear_loc == WEAR_WIELD || obj->wear_loc == WEAR_DUAL_WIELD ) &&
              obj->value[3] != WEAPON_BLASTER && obj->value[4] > 0 &&  obj->value[3] != WEAPON_SLUGTHROWER &&
              obj->value[3] != WEAPON_BOWCASTER &&  obj->value[3] != WEAPON_FORCE_PIKE 
            && obj->value[3] != WEAPON_FLAMETHROWER)
        {
           obj->value[4]--;
           if ( obj->value[4] <= 0 )
           {
              if ( obj->value[3] == WEAPON_LIGHTSABER )
              {
                 act( AT_PLAIN, "$p fizzles and dies." , obj->carried_by, obj, NULL, TO_CHAR );
                 act( AT_PLAIN, "$n's lightsaber fizzles and dies." , obj->carried_by, NULL, NULL, TO_ROOM );
              }
              else if ( obj->value[3] == WEAPON_VIBRO_BLADE 
                     || obj->value[3] == WEAPON_VIBRO_AXE 
                     || obj->value[3] == WEAPON_VIBRO_SWORD)
              {
                 act( AT_PLAIN, "$p stops vibrating." , obj->carried_by, obj, NULL, TO_CHAR );
              }
           }
        }                  

	if ( obj->item_type == ITEM_PIPE )
	{
	    if ( IS_SET( obj->value[3], PIPE_LIT ) )
	    {
		if ( --obj->value[1] <= 0 )
		{
		  obj->value[1] = 0;
		  REMOVE_BIT( obj->value[3], PIPE_LIT );
		}
		else
		if ( IS_SET( obj->value[3], PIPE_HOT ) )
		  REMOVE_BIT( obj->value[3], PIPE_HOT );
		else
		{
		  if ( IS_SET( obj->value[3], PIPE_GOINGOUT ) )
		  {
		    REMOVE_BIT( obj->value[3], PIPE_LIT );
		    REMOVE_BIT( obj->value[3], PIPE_GOINGOUT );
		  }
		  else
		    SET_BIT( obj->value[3], PIPE_GOINGOUT );
		}
		if ( !IS_SET( obj->value[3], PIPE_LIT ) )
		  SET_BIT( obj->value[3], PIPE_FULLOFASH );
	    }
	    else
	      REMOVE_BIT( obj->value[3], PIPE_HOT );
	}


/* Corpse decay (npc corpses decay at 8 times the rate of pc corpses) - Narn */

        if ( obj->item_type == ITEM_CORPSE_PC || obj->item_type == ITEM_CORPSE_NPC 
        || obj->item_type == ITEM_DROID_CORPSE )
        {
          sh_int timerfrac = UMAX(1, obj->timer - 1);
          if ( obj->item_type == ITEM_CORPSE_PC )
	    timerfrac = (int)(obj->timer / 8 + 1);

	  if ( obj->timer > 0 && obj->value[2] > timerfrac )
	  {
            char buf[MAX_STRING_LENGTH];
            char name[MAX_STRING_LENGTH];
            char *bufptr;
            bufptr = one_argument( obj->short_descr, name ); 
            bufptr = one_argument( bufptr, name ); 
            bufptr = one_argument( bufptr, name ); 

	    separate_obj(obj);
            obj->value[2] = timerfrac;
            if ( obj->item_type == ITEM_DROID_CORPSE )
              sprintf( buf, d_corpse_descs[ UMIN( timerfrac - 1, 4 ) ], 
                          bufptr ); 
            else 
              sprintf( buf, corpse_descs[ UMIN( timerfrac - 1, 4 ) ], 
                          capitalize( bufptr ) ); 

            STRFREE( obj->description );
            obj->description = STRALLOC( buf ); 
          }  
        }
       
	/* don't let inventory decay */
	if ( IS_OBJ_STAT(obj, ITEM_INVENTORY) )
	  continue;

        if ( obj->timer > 0 && obj->timer < 5 && (obj->item_type == ITEM_ARMOR || obj->item_type == ITEM_CARMOR) )
        {
          if ( obj->carried_by )
	  { 
	    act( AT_TEMP, "$p is almost dead." , obj->carried_by, obj, NULL, TO_CHAR );
	  }
        }

	if ( ( obj->timer <= 0 || --obj->timer > 0 ) )
	  continue;
	
	
	/* if we get this far, object's timer has expired. */
 
         AT_TEMP = AT_PLAIN;
	 switch ( obj->item_type )
	 {
	 default:
	   message = "$p has depleted itself.";
           AT_TEMP = AT_PLAIN;
	   break;
	 
	 case ITEM_GRENADE:
	   explode( obj );
	   return;
	   break;
	                   
	 case ITEM_PORTAL:
	   message = "$p winks out of existence.";
           remove_portal(obj);
	   obj->item_type = ITEM_TRASH;		/* so extract_obj	 */
           AT_TEMP = AT_MAGIC;			/* doesn't remove_portal */
	   break;
	 case ITEM_FOUNTAIN:
	   message = "$p dries up.";
           AT_TEMP = AT_BLUE;
	   break;
	 case ITEM_CORPSE_NPC:
	   message = "$p decays into dust and blows away.";
           AT_TEMP = AT_OBJECT;
	   break;
	 case ITEM_DROID_CORPSE:
	   message = "$p rusts away into oblivion.";
           AT_TEMP = AT_OBJECT;
	   break;
	 case ITEM_CORPSE_PC:
	   message = "$p decays into dust and is blown away...";
           AT_TEMP = AT_MAGIC;
	   break;
	 case ITEM_FOOD:
	   message = "$p is devoured by a swarm of maggots.";
           AT_TEMP = AT_HUNGRY;
	   break;
         case ITEM_BLOOD:
           message = "$p slowly seeps into the ground.";
           AT_TEMP = AT_BLOOD;
           break;
         case ITEM_BLOODSTAIN:
           message = "$p dries up into flakes and blows away.";
           AT_TEMP = AT_BLOOD;
	   break;
         case ITEM_SCRAPS:
           message = "$p crumbles and decays into nothing.";
           AT_TEMP = AT_OBJECT;
	   break;
	 case ITEM_FIRE:
	   if (obj->in_room)
	     --obj->in_room->light;
	   message = "$p burns out.";
	   AT_TEMP = AT_FIRE;
	 }
        
	  if ( obj->carried_by )
	  { 
	    act( AT_TEMP, message, obj->carried_by, obj, NULL, TO_CHAR );
	  }
	  else if ( obj->in_room
	  &&      ( rch = obj->in_room->first_person ) != NULL
	  &&	!IS_OBJ_STAT( obj, ITEM_BURRIED ) )
	  {
	    act( AT_TEMP, message, rch, obj, NULL, TO_ROOM );
            act( AT_TEMP, message, rch, obj, NULL, TO_CHAR );
	  }
 
	if ( obj->serial == cur_obj )
	  global_objcode = rOBJ_EXPIRED;
	extract_obj( obj );
    }
    return;
}


/*
 * Function to check important stuff happening to a player
 * This function should take about 5% of mud cpu time
 */
void char_check( void )
{
    CHAR_DATA *ch, *ch_next;
    EXIT_DATA *pexit;
    static int cnt = 0;
    int door, retcode;

    cnt = (cnt+1) % 2;

    for ( ch = first_char; ch; ch = ch_next )
    {
	set_cur_char(ch);
	ch_next = ch->next;
	will_fall(ch, 0);

	if ( char_died( ch ) )
	  continue;

	if ( IS_NPC( ch ) )
	{
	    if ( cnt != 0 )
		continue;

	    /* running mobs	-Thoric */
	    if ( IS_SET(ch->act, ACT_RUNNING) )
	    {
		if ( !IS_SET( ch->act, ACT_SENTINEL )
		&&   !ch->fighting && ch->hunting )
		{
		    WAIT_STATE( ch, 2 * PULSE_VIOLENCE );
		    hunt_victim( ch );
		    continue;
		}

		if ( ch->spec_fun )
		{
		    if ( (*ch->spec_fun) ( ch ) )
			continue;
		    if ( char_died(ch) )
			continue;
		}
                if ( ch->spec_2 )
		{
		    if ( (*ch->spec_2) ( ch ) )
			continue;
		    if ( char_died(ch) )
			continue;
		}

		if ( !IS_SET(ch->act, ACT_SENTINEL)
		&&   !IS_SET(ch->act, ACT_PROTOTYPE)
		&& ( door = number_bits( 4 ) ) <= 9
		&& ( pexit = get_exit(ch->in_room, door) ) != NULL
		&&   pexit->to_room
		&&   !IS_SET(pexit->exit_info, EX_CLOSED)
		&&   !IS_SET(pexit->to_room->room_flags, ROOM_NO_MOB)
		&& ( !IS_SET(ch->act, ACT_STAY_AREA)
		||   pexit->to_room->area == ch->in_room->area ) )
		{
		    retcode = move_char( ch, pexit, 0 );
		    if ( char_died(ch) )
			continue;
		    if ( retcode != rNONE || IS_SET(ch->act, ACT_SENTINEL)
		    ||    ch->position < POS_STANDING )
			continue;
		}
	    }
	    continue;
	}
	else
	{
	    if ( ch->mount
	    &&   ch->in_room != ch->mount->in_room )
	    {
		REMOVE_BIT( ch->mount->act, ACT_MOUNTED );
		ch->mount = NULL;
		ch->position = POS_STANDING;
		send_to_char( "No longer upon your mount, you fall to the ground...\n\rOUCH!\n\r", ch );
	    }

	    if ( ( ch->in_room && ch->in_room->sector_type == SECT_UNDERWATER )
	    || ( ch->in_room && ch->in_room->sector_type == SECT_OCEANFLOOR ) )
	    {
		if ( !IS_AFFECTED( ch, AFF_AQUA_BREATH ) )
		{
		    if ( get_trust(ch) < LEVEL_IMMORTAL )
		    {
			int dam;

        		  	
			dam = number_range( ch->max_hit / 50 , ch->max_hit / 30 );
			dam = UMAX( 1, dam );
			if(  ch->hit <= 0 )
			    dam = UMIN( 10, dam );
			if ( number_bits(3) == 0 )
			  send_to_char( "You cough and choke as you try to breathe water!\n\r", ch );
			damage( ch, ch, dam, TYPE_UNDEFINED );
		    }
		}
	    }
	
	    if ( char_died( ch ) )
		continue; 

	    if ( ch->in_room
	    && (( ch->in_room->sector_type == SECT_WATER_NOSWIM )
	    ||  ( ch->in_room->sector_type == SECT_WATER_SWIM ) ) )
	    {
		if ( !IS_AFFECTED( ch, AFF_FLYING )
		&& !IS_AFFECTED( ch, AFF_FLOATING ) 
		&& !IS_AFFECTED( ch, AFF_AQUA_BREATH )
		&& !ch->mount )
		{
			if ( get_trust(ch) < LEVEL_IMMORTAL )
			{
			    int dam;

			    if ( ch->move > 0 )
				    ch->move--;
			    else
			    {
				dam = number_range( ch->max_hit / 50, ch->max_hit / 30 );
				dam = UMAX( 1, dam );
				if(  ch->hit <= 0 )
			           dam = UMIN( 10, dam );
				if ( number_bits(3) == 0 )
				   send_to_char( "Struggling with exhaustion, you choke on a mouthful of water.\n\r", ch );
				damage( ch, ch, dam, TYPE_UNDEFINED );
			    }
          	      }
		}
	    }

	}
    }
}


/*
 * Aggress.
 *
 * for each descriptor
 *     for each mob in room
 *         aggress on some random PC
 *
 * This function should take 5% to 10% of ALL mud cpu time.
 * Unfortunately, checking on each PC move is too tricky,
 *   because we don't the mob to just attack the first PC
 *   who leads the party into the room.
 *
 */
void aggr_update( void )
{
    DESCRIPTOR_DATA *d, *dnext;
    CHAR_DATA *wch;
    CHAR_DATA *ch;
    CHAR_DATA *ch_next;
    CHAR_DATA *victim;
    struct act_prog_data *apdtmp;

#ifdef UNDEFD
  /*
   *  GRUNT!  To do
   *
   */
        if ( IS_NPC( wch ) && wch->mpactnum > 0
	    && wch->in_room->area->nplayer > 0 )
        {
            MPROG_ACT_LIST * tmp_act, *tmp2_act;
	    for ( tmp_act = wch->mpact; tmp_act;
		 tmp_act = tmp_act->next )
	    {
                 oprog_wordlist_check( tmp_act->buf,wch, tmp_act->ch,
				      tmp_act->obj, tmp_act->vo, ACT_PROG );
                 DISPOSE( tmp_act->buf );
            }
	    for ( tmp_act = wch->mpact; tmp_act; tmp_act = tmp2_act )
	    {
                 tmp2_act = tmp_act->next;
                 DISPOSE( tmp_act );
            }
            wch->mpactnum = 0;
            wch->mpact    = NULL;
        }
#endif

    /* check mobprog act queue */
    while ( (apdtmp = mob_act_list) != NULL )
    {
	wch = mob_act_list->vo;
	if ( !char_died(wch) && wch->mpactnum > 0 )
	{
	    MPROG_ACT_LIST * tmp_act;

	    while ( (tmp_act = wch->mpact) != NULL )
	    {
		if ( tmp_act->obj && obj_extracted(tmp_act->obj) )
		  tmp_act->obj = NULL;
		if ( tmp_act->ch && !char_died(tmp_act->ch) )
		  mprog_wordlist_check( tmp_act->buf, wch, tmp_act->ch,
					tmp_act->obj, tmp_act->vo, ACT_PROG );
		wch->mpact = tmp_act->next;
		DISPOSE(tmp_act->buf);
		DISPOSE(tmp_act);
	    }
	    wch->mpactnum = 0;
	    wch->mpact    = NULL;
        }
	mob_act_list = apdtmp->next;
	DISPOSE( apdtmp );
    }


    /*
     * Just check descriptors here for victims to aggressive mobs
     * We can check for linkdead victims to mobile_update	-Thoric
     */
    for ( d = first_descriptor; d; d = dnext )
    {
	dnext = d->next;
	if ( d->connected != CON_PLAYING || (wch=d->character) == NULL )
	   continue;

	if ( char_died(wch)
	||   IS_NPC(wch)
	||   wch->top_level >= LEVEL_IMMORTAL
	||  !wch->in_room )
	    continue;

	for ( ch = wch->in_room->first_person; ch; ch = ch_next )
	{
	    int count;

	    ch_next	= ch->next_in_room;

	    if ( !IS_NPC(ch)
	    ||   ch->fighting
	    ||   IS_AFFECTED(ch, AFF_CHARM)
	    ||   !IS_AWAKE(ch)
	    ||   ( IS_SET(ch->act, ACT_WIMPY) )
	    ||   !can_see( ch, wch ) )
		continue;

	    if ( is_hating( ch, wch ) )
	    {
		found_prey( ch, wch );
		continue;
	    }

	    if ( !IS_SET(ch->act, ACT_AGGRESSIVE)
	    ||    IS_SET(ch->act, ACT_MOUNTED)
	    ||    IS_SET(ch->in_room->room_flags, ROOM_SAFE ))
		continue;

	    victim = wch;

	    if ( !victim )
	    {
		bug( "Aggr_update: null victim.", count );
		continue;
	    }

            if ( get_timer(victim, TIMER_RECENTFIGHT) > 0 )
                continue;

	    if ( IS_NPC(ch) && IS_SET(ch->attacks, ATCK_BACKSTAB ) )
	    {
		OBJ_DATA *obj;

		if ( !ch->mount
    		&& (obj = get_eq_char( ch, WEAR_WIELD )) != NULL
    		&& obj->value[3] == 11
		&& !victim->fighting
		&& victim->hit >= victim->max_hit )
		{
		    WAIT_STATE( ch, skill_table[gsn_backstab]->beats );
		    if ( !IS_AWAKE(victim)
		    ||   number_percent( )+5 < ch->top_level )
		    {
			global_retcode = multi_hit( ch, victim, gsn_backstab );
			continue;
		    }
		    else
		    {
			global_retcode = damage( ch, victim, 0, gsn_backstab );
			continue;
		    }
		}
	    }
	    global_retcode = multi_hit( ch, victim, TYPE_UNDEFINED );
	}
    }

    return;
}

/* From interp.c */
bool check_social  args( ( CHAR_DATA *ch, char *command, char *argument ) );

/*
 * drunk randoms	- Tricops
 * (Made part of mobile_update	-Thoric)
 */
void drunk_randoms( CHAR_DATA *ch )
{
    CHAR_DATA *rvch = NULL;
    CHAR_DATA *vch;
    sh_int drunk;
    sh_int position;

    if ( IS_NPC( ch ) || ch->pcdata->condition[COND_DRUNK] <= 0 )
	return;

    if ( number_percent() < 30 )
	return;

    drunk = ch->pcdata->condition[COND_DRUNK];
    position = ch->position;
    ch->position = POS_STANDING;

    if ( number_percent() < (2*drunk / 20) )
	check_social( ch, "burp", "" );
    else
    if ( number_percent() < (2*drunk / 20) )
	check_social( ch, "hiccup", "" );
    else
    if ( number_percent() < (2*drunk / 20) )
	check_social( ch, "drool", "" );
    else
    if ( number_percent() < (2*drunk / 20) )
	check_social( ch, "fart", "" );
    else
    if ( drunk > (10+(get_curr_con(ch)/5))
    &&   number_percent() < ( 2 * drunk / 18 ) )
    {
	for ( vch = ch->in_room->first_person; vch; vch = vch->next_in_room )
	    if ( number_percent() < 10 )
		rvch = vch;
	check_social( ch, "puke", (rvch ? rvch->name : "") );
    }

    ch->position = position;
    return;
}

void halucinations( CHAR_DATA *ch )
{
    if ( ch->mental_state >= 30 && number_bits(5 - (ch->mental_state >= 50) - (ch->mental_state >= 75)) == 0 )
    {
	char *t;

	switch( number_range( 1, UMIN(20, (ch->mental_state+5) / 5)) )
	{
	    default:
	    case  1: t = "You feel very restless... you can't sit still, as Omikron mocks you from afar....\n\r";		break;
	    case  2: t = "You see shiro pull out a shovel screaming DIE FUCKING FAERIES and bash them in the head!\n\r";				break;
	    case  3: t = "Your skin is crawling with the thought that Raijen coded on SWL.\n\r";	break;
	    case  4: t = "You see Arcturus dressed as a Tusken Raider waving around a Gaffi Stick in the sand!\n\r";	break;
	    case  5: t = "Those damn little faeries keep laughing at you!\n\r";		break;
	    case  6: t = "You can hear your mother's brother's roommate's ex-girlfriend's dog crying...\n\r";			break;
	    case  7: t = "Have you been here before, or not?  You're not sure... because the voices in your head beg to differ.\n\r";	break;
	    case  8: t = "Painful childhood memories flash through your mind.\n\r";	break;
	    case  9: t = "Soul calls your name, and slays you in cold blood.\n\r";	break;
	    case 10: t = "Your head is pulsating with the knowledge of Arcturus.\n\r";	break;
	    case 11: t = "You feel the urge to blow up a planet!\n\r";			break;
	    case 12: t = "You're not quite sure what is real anymore, is it that deathstar talking or the crazy vibro-sword wielding murderer?\n\r";		break;
	    case 13: t = "You see Arcturus setting an example by stabbing Kybus...\n\r";            break;
	    case 14: t = "They're coming to take you away. Ha Ha they're coming to take you away ho-ho he-he ha-ha to the funny farm! where life is beautiful all the time!\n\r";	break;
	    case 15: t = "You begin to feel all weak at the site of a nude fat man running in slow motion.\n\r";	break;
	    case 16: t = "You're light as air... the heavens are yours for the taking.\n\r";	break;
	    case 17: t = "Your whole life flashes by... and your future...\n\r";	break;
	    case 18: t = "You are everywhere and everything... you know all and are all!\n\r";	break;
	    case 19: t = "You feel immortal enough to step into oncoming traffic!\n\r";					break;
	    case 20: t = "Ahh... the power of a Supreme Entity... what to do...with this 20 credits you stole.\n\r";	break;
	}
	send_to_char( t, ch );
    }
    return;
}

void tele_update( void )
{
    TELEPORT_DATA *tele, *tele_next;

    if ( !first_teleport )
      return;
    
    for ( tele = first_teleport; tele; tele = tele_next )
    {
	tele_next = tele->next;
	if ( --tele->timer <= 0 )
	{
	    if ( tele->room->first_person )
	    {
		  teleport( tele->room->first_person, tele->room->tele_vnum,
			TELE_TRANSALL );
	    }
	    UNLINK( tele, first_teleport, last_teleport, next, prev );
	    DISPOSE( tele );
	}
    }
}

#if FALSE
/* 
 * Write all outstanding authorization requests to Log channel - Gorog
 */ 
void auth_update( void ) 
{ 
  CHAR_DATA *victim; 
  DESCRIPTOR_DATA *d; 
  char log_buf [MAX_INPUT_LENGTH];
  bool first_time = TRUE;         /* so titles are only done once */

  for ( d = first_descriptor; d; d = d->next ) 
      {
      victim = d->character;
      if ( victim && IS_WAITING_FOR_AUTH(victim) )
         {
         if ( first_time )
            {
            first_time = FALSE;
            strcpy (log_buf, "Pending authorizations:" ); 
            to_channel( log_buf, CHANNEL_MONITOR, "Monitor", 1);
            }
         sprintf( log_buf, " %s@%s new %s", victim->name,
            victim->desc->host, race_table[victim->race].race_name);
         to_channel( log_buf, CHANNEL_MONITOR, "Monitor", 1);
         }
      }
} 
#endif

void auth_update( void ) 
{ 
    CHAR_DATA *victim; 
    DESCRIPTOR_DATA *d; 
    char buf [MAX_INPUT_LENGTH], log_buf [MAX_INPUT_LENGTH];
    bool found_hit = FALSE;         /* was at least one found? */

    strcpy (log_buf, "Pending authorizations:\n\r" );
    for ( d = first_descriptor; d; d = d->next ) 
    {
	if ( (victim = d->character) && IS_WAITING_FOR_AUTH(victim) )
	{
	    found_hit = TRUE;
	    sprintf( buf, " %s@%s new %s\n\r", victim->name,
		victim->desc->host, race_table[victim->race].race_name);
	    strcat (log_buf, buf);
	}
    }
    if (found_hit)
    {
	log_string( log_buf ); 
	to_channel( log_buf, CHANNEL_MONITOR, "Monitor", 1);
    }
} 

/*
 * Handle all kinds of updates.
 * Called once per pulse from game loop.
 * Random times to defeat tick-timing clients and players.
 */
void update_handler( void )
{
    static  int     pulse_taxes;
    static  int     pulse_area;
    static  int     pulse_mobile;
    static  int     pulse_violence;
    static  int     pulse_point;
    static  int	    pulse_second;
    static  int     pulse_space;
    static  int     pulse_recharge;
    static  int     pulse_ship;
    struct timeval stime;
    struct timeval etime;

    if ( timechar )
    {
      set_char_color(AT_PLAIN, timechar);
      send_to_char( "Starting update timer.\n\r", timechar );
      gettimeofday(&stime, NULL);
    }
    
    if ( --pulse_area     <= 0 )
    {
	pulse_area	= number_range( PULSE_AREA / 2, 3 * PULSE_AREA / 2 );
	area_update	( );
    }
    
    if ( --pulse_taxes     <= 0 )
    {
	pulse_taxes	= PULSE_TAXES ;
	update_taxes	( );
    }

    if ( --pulse_mobile   <= 0 )
    {
	pulse_mobile	= PULSE_MOBILE;
	mobile_update  ( );
    }
    
    if ( --pulse_space   <= 0 )
    {
       pulse_space    = PULSE_SPACE;
       update_space  ( );
       update_bus ( );
       update_traffic ( );
    }

    if ( --pulse_recharge <= 0 )
    {
         pulse_recharge = PULSE_SPACE/3;
         recharge_ships ( );
    }

    if ( --pulse_ship   <= 0 )
    {
       pulse_ship  = PULSE_SPACE/10;
       move_ships  ( );
    }                        
    
    if ( --pulse_violence <= 0 )
    {
	pulse_violence	= PULSE_VIOLENCE;
	violence_update	( );
    }

    if ( --pulse_point    <= 0 )
    {
	pulse_point     = number_range( PULSE_TICK * 0.75, PULSE_TICK * 1.25 );

        auth_update     ( );			/* Gorog */
        time_update     ( );
	weather_update	( );
	char_update	( );
	obj_update	( );
	clear_vrooms	( );			/* remove virtual rooms */
    }

    if ( --pulse_second   <= 0 )
    {
	pulse_second	= PULSE_PER_SECOND;
	char_check( );
 	/*reboot_check( "" ); Disabled to check if its lagging a lot - Scryn*/
 	/* Much faster version enabled by Altrag..
 	   although I dunno how it could lag too much, it was just a bunch
 	   of comparisons.. */
 	reboot_check(0);
    }

    if ( auction->item && --auction->pulse <= 0 )
    {                                                  
	auction->pulse = PULSE_AUCTION;                     
	auction_update( );
    }

    tele_update( );
    aggr_update( );
    obj_act_update ( );
    room_act_update( );
    clean_obj_queue();		/* dispose of extracted objects */
    clean_char_queue();		/* dispose of dead mobs/quitting chars */
    if ( timechar )
    {
      gettimeofday(&etime, NULL);
      set_char_color(AT_PLAIN, timechar);
      send_to_char( "Update timing complete.\n\r", timechar );
      subtract_times(&etime, &stime);
      ch_printf( timechar, "Timing took %d.%06d seconds.\n\r",
          etime.tv_sec, etime.tv_usec );
      timechar = NULL;
    }
    tail_chain( );
    return;
}


void remove_portal( OBJ_DATA *portal )
{
    ROOM_INDEX_DATA *fromRoom, *toRoom;
    CHAR_DATA *ch;
    EXIT_DATA *pexit;
    bool found;

    if ( !portal )
    {
	bug( "remove_portal: portal is NULL", 0 );
	return;
    }

    fromRoom = portal->in_room;
    found = FALSE;
    if ( !fromRoom )
    {
	bug( "remove_portal: portal->in_room is NULL", 0 );
	return;
    }

    for ( pexit = fromRoom->first_exit; pexit; pexit = pexit->next )
	if ( IS_SET( pexit->exit_info, EX_PORTAL ) )
	{
	    found = TRUE;
	    break;
	}

    if ( !found )
    {
	bug( "remove_portal: portal not found in room %d!", fromRoom->vnum );
	return;
    }

    if ( pexit->vdir != DIR_PORTAL )
	bug( "remove_portal: exit in dir %d != DIR_PORTAL", pexit->vdir );

    if ( ( toRoom = pexit->to_room ) == NULL )
      bug( "remove_portal: toRoom is NULL", 0 );
 
    extract_exit( fromRoom, pexit );
    /* rendunancy */
    /* send a message to fromRoom */
    /* ch = fromRoom->first_person; */
    /* if(ch!=NULL) */
    /* act( AT_PLAIN, "A magical portal below winks from existence.", ch, NULL, NULL, TO_ROOM ); */

    /* send a message to toRoom */
    if ( toRoom && (ch = toRoom->first_person) != NULL )
      act( AT_PLAIN, "A magical portal above winks from existence.", ch, NULL, NULL, TO_ROOM );

    /* remove the portal obj: looks better to let update_obj do this */
    /* extract_obj(portal);  */

    return;
}

void reboot_check( time_t reset )
{
  static char *tmsg[] =
  { "SYSTEM: Reboot in 10 seconds.",
    "SYSTEM: Reboot in 30 seconds.",
    "SYSTEM: Reboot in 1 minute.",
    "SYSTEM: Reboot in 2 minutes.",
    "SYSTEM: Reboot in 3 minutes.",
    "SYSTEM: Reboot in 4 minutes.",
    "SYSTEM: Reboot in 5 minutes.",
    "SYSTEM: Reboot in 10 minutes.",
  };
  static const int times[] = { 10, 30, 60, 120, 180, 240, 300, 600 };
  static const int timesize =
      UMIN(sizeof(times)/sizeof(*times), sizeof(tmsg)/sizeof(*tmsg));
  char buf[MAX_STRING_LENGTH];
  static int trun;
  static bool init;
  
  if ( !init || reset >= current_time )
  {
    for ( trun = timesize-1; trun >= 0; trun-- )
      if ( reset >= current_time+times[trun] )
        break;
    init = TRUE;
    return;
  }
  
  if ( (current_time % 1800) == 0 )
  {
    sprintf(buf, "%.24s: %d players", ctime(&current_time), num_descriptors);
    append_to_file(USAGE_FILE, buf);
  }
  
  if ( new_boot_time_t - boot_time < 60*60*18 &&
      !set_boot_time->manual )
    return;
  
  if ( new_boot_time_t <= current_time )
  {
    CHAR_DATA *vch;
    extern bool mud_down;
    
    if ( auction->item )
    {
      sprintf(buf, "Sale of %s&C has been stopped by mud.",
          auction->item->short_descr);
      talk_auction(buf);
      obj_to_char(auction->item, auction->seller);
      auction->item = NULL;
      if ( auction->buyer && auction->buyer != auction->seller )
      {
        auction->buyer->gold += auction->bet;
        send_to_char("Your money has been returned.\n\r", auction->buyer);
      }
    }
    echo_to_all(AT_YELLOW, "The world begins to bend to Arcturus' will, and fades into nothingness.\n\r", ECHOTAR_ALL);
    for ( vch = first_char; vch; vch = vch->next )
      if ( !IS_NPC(vch) )
        save_char_obj(vch);
    mud_down = TRUE;
    return;
  }
  
  if ( trun != -1 && new_boot_time_t - current_time <= times[trun] )
  {
    echo_to_all(AT_YELLOW, tmsg[trun], ECHOTAR_ALL);
    if ( trun <= 5 )
      sysdata.DENY_NEW_PLAYERS = TRUE;
    --trun;
    return;
  }
  return;
}
  
#if 0
void reboot_check( char *arg )
{
    char buf[MAX_STRING_LENGTH];
    extern bool mud_down;
    /*struct tm *timestruct;
    int timecheck;*/
    CHAR_DATA *vch;

    /*Bools to show which pre-boot echoes we've done. */
    static bool thirty  = FALSE;
    static bool fifteen = FALSE;
    static bool ten     = FALSE;
    static bool five    = FALSE;
    static bool four    = FALSE;
    static bool three   = FALSE;
    static bool two     = FALSE;
    static bool one     = FALSE;

    /* This function can be called by do_setboot when the reboot time
       is being manually set to reset all the bools. */
    if ( !str_cmp( arg, "reset" ) )
    {
      thirty  = FALSE;
      fifteen = FALSE;
      ten     = FALSE;
      five    = FALSE;
      four    = FALSE;
      three   = FALSE;
      two     = FALSE;
      one     = FALSE;
      return;
    }

    /* If the mud has been up less than 18 hours and the boot time 
       wasn't set manually, forget it. */ 
/* Usage monitor */

if ((current_time % 1800) == 0)
{
  sprintf(buf, "%s: %d players", ctime(&current_time), num_descriptors);  
  append_to_file(USAGE_FILE, buf);
}

/* Change by Scryn - if mud has not been up 18 hours at boot time - still 
 * allow for warnings even if not up 18 hours 
 */
    if ( new_boot_time_t - boot_time < 60*60*18 
         && set_boot_time->manual == 0 )
    {
      return;
    }
/*
    timestruct = localtime( &current_time);

    if ( timestruct->tm_hour == set_boot_time->hour        
         && timestruct->tm_min  == set_boot_time->min )*/
    if ( new_boot_time_t <= current_time )
    {
       /* Return auction item to seller */
       if (auction->item != NULL)
       {
        sprintf (buf,"Sale of %s&C has been stopped by mud.",
                 auction->item->short_descr);
        talk_auction (buf);
        obj_to_char (auction->item, auction->seller);
        auction->item = NULL;
        if (auction->buyer != NULL && auction->seller != auction->buyer) /* return money to the buyer */
        {
            auction->buyer->gold += auction->bet;
            send_to_char ("Your money has been returned.\n\r",auction->buyer);
        }
       }      

       sprintf( buf, "You are forced from these realms by a strong magical presence" ); 
       echo_to_all( AT_YELLOW, buf, ECHOTAR_ALL );
       sprintf( buf, "as life here is reconstructed." );
       echo_to_all( AT_YELLOW, buf, ECHOTAR_ALL );

       /* Save all characters before booting. */
       for ( vch = first_char; vch; vch = vch->next )
       {
         if ( !IS_NPC( vch ) )
           save_char_obj( vch );
       }
       mud_down = TRUE;
    }

  /* How many minutes to the scheduled boot? */
/*  timecheck = ( set_boot_time->hour * 60 + set_boot_time->min )
              - ( timestruct->tm_hour * 60 + timestruct->tm_min );

  if ( timecheck > 30  || timecheck < 0 ) return;

  if ( timecheck <= 1 ) */
  if ( new_boot_time_t - current_time <= 60 )
  {
    if ( one == FALSE )
    {
	sprintf( buf, "You feel the ground shake as the end comes near!" );
	echo_to_all( AT_YELLOW, buf, ECHOTAR_ALL );
	one = TRUE;
	sysdata.DENY_NEW_PLAYERS = TRUE;
    }
    return;   
  }

/*  if ( timecheck == 2 )*/
  if ( new_boot_time_t - current_time <= 120 )
  {
    if ( two == FALSE )
    {
	sprintf( buf, "Lightning crackles in the sky above!" );
	echo_to_all( AT_YELLOW, buf, ECHOTAR_ALL );
	two = TRUE;
	sysdata.DENY_NEW_PLAYERS = TRUE;
    }
    return;   
  }

/*  if ( timecheck == 3 )*/ 
  if (new_boot_time_t - current_time <= 180 )
  {
    if ( three == FALSE )
    {
	sprintf( buf, "Crashes of thunder sound across the land!" );
	echo_to_all( AT_YELLOW, buf, ECHOTAR_ALL );
	three = TRUE;
	sysdata.DENY_NEW_PLAYERS = TRUE;
    }
    return;   
  }

/*  if ( timecheck == 4 )*/
  if( new_boot_time_t - current_time <= 240 )
  {
    if ( four == FALSE )
    {
	sprintf( buf, "The sky has suddenly turned midnight black." );
	echo_to_all( AT_YELLOW, buf, ECHOTAR_ALL );
	four = TRUE;
	sysdata.DENY_NEW_PLAYERS = TRUE;
    }
    return;   
  }

/*  if ( timecheck == 5 )*/
  if( new_boot_time_t - current_time <= 300 )
  {
    if ( five == FALSE )
    {
	sprintf( buf, "You notice the life forms around you slowly dwindling away." );
	echo_to_all( AT_YELLOW, buf, ECHOTAR_ALL );
	five = TRUE;
	sysdata.DENY_NEW_PLAYERS = TRUE;
    }
    return;   
  }

/*  if ( timecheck == 10 )*/
  if( new_boot_time_t - current_time <= 600 )
  {
    if ( ten == FALSE )
    {
	sprintf( buf, "The seas across the realm have turned frigid." );
	echo_to_all( AT_YELLOW, buf, ECHOTAR_ALL );
	ten = TRUE;
    }
    return;   
  }

/*  if ( timecheck == 15 )*/
  if( new_boot_time_t - current_time <= 900 )
  {
    if ( fifteen == FALSE )
    {
	sprintf( buf, "The aura of magic which once surrounded the realms seems slightly unstable." );
	echo_to_all( AT_YELLOW, buf, ECHOTAR_ALL );
	fifteen = TRUE;
    }
    return;   
  }

/*  if ( timecheck == 30 )*/
  if( new_boot_time_t - current_time <= 1800 )
  { 
    if ( thirty == FALSE )
    {
	sprintf( buf, "You sense a change in the magical forces surrounding you." );
	echo_to_all( AT_YELLOW, buf, ECHOTAR_ALL );
	thirty = TRUE;
    }
    return;   
  }

  return;
}
#endif

/* the auction update*/

void auction_update (void)
{
    int tax, pay;
    char buf[MAX_STRING_LENGTH];

    switch (++auction->going) /* increase the going state */
    {
	case 1 : /* going once */
	case 2 : /* going twice */
	    if (auction->bet > auction->starting)
		sprintf (buf, "%s&C: going %s for %d.", auction->item->short_descr,
			((auction->going == 1) ? "once" : "twice"), auction->bet);
	    else
		sprintf (buf, "%s&C: going %s (bid not received yet).", auction->item->short_descr,
			((auction->going == 1) ? "once" : "twice"));

	    talk_auction (buf);
	    break;

	case 3 : /* SOLD! */
	    if (!auction->buyer && auction->bet)
	    {
		bug( "Auction code reached SOLD, with NULL buyer, but %d gold bid", auction->bet );
		auction->bet = 0;
	    }
	    if (auction->bet > 0 && auction->buyer != auction->seller)
	    {
		sprintf (buf, "%s&C sold to %s for %d.",
			auction->item->short_descr,
			IS_NPC(auction->buyer) ? auction->buyer->short_descr : auction->buyer->name,
			auction->bet);
		talk_auction(buf);

		act(AT_ACTION, "The auctioneer materializes before you, and hands you $p.",
			auction->buyer, auction->item, NULL, TO_CHAR);
		act(AT_ACTION, "The auctioneer materializes before $n, and hands $m $p.",
			auction->buyer, auction->item, NULL, TO_ROOM);

		if ( (auction->buyer->carry_weight 
		+     get_obj_weight( auction->item ))
		>     can_carry_w( auction->buyer ) )
		{
		    act( AT_PLAIN, "$p is too heavy for you to carry with your current inventory.", auction->buyer, auction->item, NULL, TO_CHAR );
    		    act( AT_PLAIN, "$n is carrying too much to also carry $p, and $e drops it.", auction->buyer, auction->item, NULL, TO_ROOM );
		    obj_to_room( auction->item, auction->buyer->in_room );
		}
		else
		    obj_to_char( auction->item, auction->buyer );   
	        pay = (int)auction->bet * 0.9;
		tax = (int)auction->bet * 0.1;
                tax = UMAX(10, tax); /* just to make sure it always charges at least 10 credits. */  
		boost_economy( auction->seller->in_room->area, tax );
                auction->seller->gold += pay; /* give him the money, tax 10 % */
		sprintf(buf, "The auctioneer pays you %d credits, charging an auction fee of %d credits.\n\r", pay, tax);
		send_to_char(buf, auction->seller);
                auction->item = NULL; /* reset item */
		if ( IS_SET( sysdata.save_flags, SV_AUCTION ) )
		{
		    save_char_obj( auction->buyer );
		    save_char_obj( auction->seller );
		}
            }
            else /* not sold */
            {
                sprintf (buf, "No bids received for %s&C - object has been removed from auction\n\r.",auction->item->short_descr);
                talk_auction(buf);
                act (AT_ACTION, "The auctioneer appears before you to return $p to you.",
                      auction->seller,auction->item,NULL,TO_CHAR);
                act (AT_ACTION, "The auctioneer appears before $n to return $p to $m.",
                      auction->seller,auction->item,NULL,TO_ROOM);
		if ( (auction->seller->carry_weight
		+     get_obj_weight( auction->item ))
		>     can_carry_w( auction->seller ) )
		{
		    act( AT_PLAIN, "You drop $p as it is just too much to carry"
			" with everything else you're carrying.", auction->seller,
			auction->item, NULL, TO_CHAR );
		    act( AT_PLAIN, "$n drops $p as it is too much extra weight"
			" for $m with everything else.", auction->seller,
			auction->item, NULL, TO_ROOM );
		    obj_to_room( auction->item, auction->seller->in_room );
		}
		else
		    obj_to_char (auction->item,auction->seller);
		tax = (int)auction->item->cost * 0.05;
                tax = UMAX(10, tax); /* At least ten credit tax. */
		boost_economy( auction->seller->in_room->area, tax );
		sprintf(buf, "The auctioneer charges you an auction fee of %d credits.\n\r", tax );
		send_to_char(buf, auction->seller);
		if ((auction->seller->gold - tax) < 0)
		  auction->seller->gold = 0;
		else
		  auction->seller->gold -= tax;
		if ( IS_SET( sysdata.save_flags, SV_AUCTION ) )
		    save_char_obj( auction->seller );
	    } /* else */
	    auction->item = NULL; /* clear auction */
    } /* switch */
} /* func */

void subtract_times(struct timeval *etime, struct timeval *stime)
{
  etime->tv_sec -= stime->tv_sec;
  etime->tv_usec -= stime->tv_usec;
  while ( etime->tv_usec < 0 )
  {
    etime->tv_usec += 1000000;
    etime->tv_sec--;
  }
  return;
}

/*
 * Function to update weather vectors according to climate
 * settings, random effects, and neighboring areas.
 * Last modified: July 18, 1997
 * - Fireblade
 */
void adjust_vectors(WEATHER_DATA *weather)
{
	NEIGHBOR_DATA *neigh;
	double dT, dP, dW;

	if(!weather)
	{
		bug("adjust_vectors: NULL weather data.", 0);
		return;
	}

	dT = 0;
	dP = 0;
	dW = 0;

	/* Add in random effects */
	dT += number_range(-rand_factor, rand_factor);
	dP += number_range(-rand_factor, rand_factor);
	dW += number_range(-rand_factor, rand_factor);
	
	/* Add in climate effects*/
	dT += climate_factor *
		(((weather->climate_temp - 2)*weath_unit) -
		(weather->temp))/weath_unit;
	dP += climate_factor *
		(((weather->climate_precip - 2)*weath_unit) -
		(weather->precip))/weath_unit;
	dW += climate_factor *
		(((weather->climate_wind - 2)*weath_unit) -
		(weather->wind))/weath_unit;
	
		
	/* Add in effects from neighboring areas */
	for(neigh = weather->first_neighbor; neigh;
			neigh = neigh->next)
	{
		/* see if we have the area cache'd already */
		if(!neigh->address)
		{
			/* try and find address for area */
			neigh->address = get_area(neigh->name);
			
			/* if couldn't find area ditch the neigh */
			if(!neigh->address)
			{
				NEIGHBOR_DATA *temp;
				bug("adjust_weather: "
					"invalid area name.", 0);
				temp = neigh->prev;
				UNLINK(neigh,
				       weather->first_neighbor,
				       weather->last_neighbor,
				       next,
				       prev);
				STRFREE(neigh->name);
				DISPOSE(neigh);
				neigh = temp;
				continue;
			}
		}
		
		dT +=(neigh->address->weather->temp -
		      weather->temp) / neigh_factor;
		dP +=(neigh->address->weather->precip -
		      weather->precip) / neigh_factor;
		dW +=(neigh->address->weather->wind -
		      weather->wind) / neigh_factor;
	}
	
	/* now apply the effects to the vectors */
	weather->temp_vector += (int)dT;
	weather->precip_vector += (int)dP;
	weather->wind_vector += (int)dW;

	/* Make sure they are within the right range */
	weather->temp_vector = URANGE(-max_vector,
		weather->temp_vector, max_vector);
	weather->precip_vector = URANGE(-max_vector,
		weather->precip_vector, max_vector);
	weather->wind_vector = URANGE(-max_vector,
		weather->wind_vector, max_vector);
	
	return;
}


/*
 * update the time
 */
void time_update()
{
	AREA_DATA *pArea;
	DESCRIPTOR_DATA *d;
	WEATHER_DATA *weath;
	
	switch(++time_info.hour)
	{
		case 5:
		case 6:
		case 12:
		case 19:
		case 20:
			for(pArea = first_area; pArea;
				pArea = (pArea == last_area) ? first_build : pArea->next)
			{
				get_time_echo(pArea->weather);
			}
			
			for(d = first_descriptor; d; d = d->next)
			{
				if(d->connected == CON_PLAYING &&
					IS_OUTSIDE(d->character) &&
					IS_AWAKE(d->character))
				{
					weath = d->character->in_room->area->weather;
					if(!weath->echo)
						continue;
					set_char_color(weath->echo_color,
						d->character);
					ch_printf(d->character, weath->echo);
				}
			}
			break;
		case 24:
			time_info.hour = 0;
			time_info.day++;
			break;
	}
	
	if(time_info.day >= 35)
	{
		time_info.day = 0;
		time_info.month++;
	}
	
	if(time_info.month >= 10)
	{
		time_info.month = 0;
		time_info.year++;
	}
	
	return;
}

