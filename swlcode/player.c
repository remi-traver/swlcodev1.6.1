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
* 		Commands for personal player settings/statictics	   *
****************************************************************************/

#include <sys/types.h>
#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include "mud.h"

/*
 *  Locals
 */
char *tiny_affect_loc_name(int location);

void do_gold(CHAR_DATA * ch, char *argument)
{
   if (IS_NPC(ch))
   {
	return;
   }
   set_char_color( AT_GOLD, ch );
   ch_printf( ch,  "You have %d credits with you.\n\r", ch->gold );
   if ( ch->pcdata->bank != 0 )
   {
     ch_printf( ch,  "Bank: %d.\n\r", ch->pcdata->bank );
     return;
   }
   return;
}

/* QPBuy; another function o' raijeny goodness.  To change prices, change the ZZZ_COST defines in mud.h */

void do_qpbuy( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    char arg2[MAX_INPUT_LENGTH];
    OBJ_DATA *obj;
    OBJ_INDEX_DATA *pObjIndex;
    int vnum;


    argument = one_argument( argument, arg );


    if ( IS_NPC(ch) )
        return;

    if ( !IS_AWAKE(ch) )
    {
        send_to_char( "In your dreams, or what?\n\r", ch );
        return;
    }

    if ( arg[0] == '\0' )
    {
        send_to_char( "Enhance what?\n\r", ch );
        return;
    }

	

    if ( !str_cmp( arg, "hum" ) )
    {
        if( ch->pcdata->quest_curr < HUM_COST )
        {
            send_to_char( "You don't have the questpoints.\n\r",ch );
            return;
        }
        argument = one_argument( argument, arg2 );

        if ( arg2[0] == '\0' )
        {
          send_to_char( "Make what item hum?\n\r", ch );
          return;
        }

        if ( !( obj = get_obj_carry( ch, arg2 ) ) )
        {
           send_to_char( "That item is not in your inventory.\n\r", ch );
           return;
        }

	if ( IS_OBJ_STAT( obj, ITEM_HUM ) )
	{
	    send_to_char( "That item is already humming.\n\r", ch );
	    return;
	}
 
        ch->pcdata->quest_curr -= HUM_COST;
	separate_obj( obj );
        SET_BIT( obj->extra_flags, ITEM_HUM );

        send_to_char( "Your item begins to hum softly.\n\r", ch );
        return;
    }


    if ( !str_cmp( arg, "glow" ) )
    {
        if( ch->pcdata->quest_curr < GLOW_COST )
        {
            send_to_char( "You don't have the questpoints.\n\r",ch );
            return;
        }
        argument = one_argument( argument, arg2 );

        if ( arg2[0] == '\0' )
        {
          send_to_char( "Make what item glow?\n\r", ch );
          return;
        }

        if ( !( obj = get_obj_carry( ch, arg2 ) ) )
        {
           send_to_char( "That item is not in your inventory.\n\r", ch );
           return;
        }

	if ( IS_OBJ_STAT( obj, ITEM_GLOW ) )
	{
	    send_to_char( "That item is already glowing.\n\r", ch );
	    return;
	}
 
        ch->pcdata->quest_curr -= HUM_COST;
	separate_obj( obj );
        SET_BIT( obj->extra_flags, ITEM_GLOW );

        send_to_char( "Your item begins to glow softly.\n\r", ch );
        return;
    }
    if ( !str_cmp( arg, "force" ) ) 
    {


        if( ch->pcdata->quest_curr < FORCE_COST )
        {
            send_to_char( "You don't have the questpoints.\n\r",ch );
            return;
        }

        if (ch->main_ability == HUNTING_ABILITY )
        {
            send_to_char( "Bounty Hunters Can't Buy Force!!!.\n\r",ch );
            return;
        }
        if(ch->race == RACE_DROID)
        {
            send_to_char("You got to be shitting me, you are a machine.\n\r", ch);
            return;
        }
        if (ch->perm_frc >= 15 )
        {
            send_to_char( "You can't improve your force over 75 levels with questpoints.\n\r",ch );
            return;
        }

        ch->pcdata->quest_curr -= FORCE_COST;
        ch->perm_frc += 1;
        if(ch->perm_frc == 1 )
           ch->max_mana = 600; /* to get them started. */
        send_to_char( "You Gain 5 FORCE Levels!!!\n\r", ch );
        return; 
    return;
    }


    if ( !str_cmp( arg, "old" ) ) 
    {
	if( ch->pcdata->quest_curr < OLD_COST )
	{
	    send_to_char( "You don't have the questpoints.\n\r",ch );
	    return;
	}

	ch->pcdata->quest_curr -= OLD_COST;
	ch->pcdata->age += 1;
	send_to_char( "You age a year in an istant!!!\n\r", ch );
	return;
    }

    if ( !str_cmp( arg, "youth" ) ) 
    {
	if( ch->pcdata->quest_curr < YOUTH_COST )
	{
	    send_to_char( "You don't have the questpoints.\n\r",ch );
	    return;
	}

	ch->pcdata->quest_curr -= YOUTH_COST;
	ch->pcdata->age -= 1;
	send_to_char( "You de-age a year in an istant!!!\n\r", ch );
	return;
    }
    if ( !str_cmp( arg, "hp" ) ) 
    {
	if( ch->pcdata->quest_curr < HP_COST )
	{
	    send_to_char( "You don't have the questpoints.\n\r",ch );
	    return;
	}

	ch->pcdata->quest_curr -= HP_COST;
	ch->max_hit += 10;
	ch->hit += 10;
	send_to_char( "You gain 10 hitpoints!\n\r", ch );
	return;
    }

    if ( !str_cmp( arg, "mana" ) )
    {
        if( ch->pcdata->quest_curr < MANA_COST )
        {
            send_to_char( "You don't have the questpoints.\n\r",ch );
            return;
        }
        
        ch->pcdata->quest_curr -= MANA_COST;
        ch->max_mana += 10;
        ch->mana += 10;
        send_to_char( "You gain 10 points of mana!\n\r", ch ); 
	return;
    }

    if ( !str_cmp( arg, "movement" ) )
    {
        if( ch->pcdata->quest_curr < MOVE_COST )
        {
            send_to_char( "You don't have the questpoints.\n\r",ch );
            return;
        }
        
        ch->pcdata->quest_curr -= MOVE_COST;
        ch->max_move += 10;
        ch->move += 10;
        send_to_char( "You gain 10 movement points!\n\r", ch ); 
	return;
    }

    if ( !str_cmp( arg, "str" ) || !str_cmp( arg, "strength" ) )
    {
        if( ch->pcdata->quest_curr < STR_COST )
        {
            send_to_char( "You don't have the questpoints.\n\r",ch );
            return;
        }

        ch->pcdata->quest_curr -= STR_COST;
        ch->perm_str++;
        send_to_char( "You grow stronger!\n\r", ch ); 
	return;
    }
  
    if ( !str_cmp( arg, "int" ) || !str_cmp( arg, "intelligence" ) )
    {
        if( ch->pcdata->quest_curr < INT_COST )
        {
            send_to_char( "You don't have the questpoints.\n\r",ch );
            return;
        }

        
        ch->pcdata->quest_curr -= INT_COST;
        ch->perm_int++;
        send_to_char( "You grow more intelligent!\n\r", ch );               
	return;
    }

    if ( !str_cmp( arg, "wis" ) || !str_cmp( arg, "wisdom" ) )
    {
        if( ch->pcdata->quest_curr < WIS_COST )
        {
            send_to_char( "You don't have the questpoints.\n\r",ch );
            return;
        }
        

        ch->pcdata->quest_curr -= WIS_COST;
        ch->perm_wis++;
        send_to_char( "You grow wiser!\n\r", ch );               
	return;
    }

    if ( !str_cmp( arg, "dex" ) || !str_cmp( arg, "dexterity" ) )
    {
        if( ch->pcdata->quest_curr < DEX_COST )
        {
            send_to_char( "You don't have the questpoints.\n\r",ch );
            return;
        }
        

        ch->pcdata->quest_curr -= DEX_COST;
        ch->perm_dex++;
        send_to_char( "Your dexterity increases!\n\r", ch );               
	return;

    }

    if ( !str_cmp( arg, "con" ) || !str_cmp( arg, "constitution" ) )
    {
        if( ch->pcdata->quest_curr < CON_COST )
        {
            send_to_char( "You don't have the questpoints.\n\r",ch );
            return;
        }
        

        ch->pcdata->quest_curr -= CON_COST;
        ch->perm_con++;
        send_to_char( "Your constitution increases!\n\r", ch );               
	return;
    }

    if ( !str_cmp( arg, "cha" ) || !str_cmp( arg, "charisma" ) )
    {
        if( ch->pcdata->quest_curr < CHA_COST )
        {
            send_to_char( "You don't have the questpoints.\n\r",ch );
            return;
        }
        

        ch->pcdata->quest_curr -= CHA_COST;
        ch->perm_cha++;
        send_to_char( "You grow more charismatic!\n\r", ch );               
	return;
    }
    if ( !str_cmp( arg, "lck" ) || !str_cmp( arg, "luck" ) )
    {
        if( ch->pcdata->quest_curr < LCK_COST )
        {
            send_to_char( "You don't have the questpoints.\n\r",ch );
            return;
        }
        

        ch->pcdata->quest_curr -= LCK_COST;
        ch->perm_lck++;
        send_to_char( "You feel luckier!\n\r", ch );               
	return;
    }


   if ( !str_cmp( arg, "rship" ) )    // spec - rename ship with qp
   {
      SHIP_DATA *ship;

      if (ch->pcdata->quest_curr < RSHIP_COST )  //check for enough qp's
      {  send_to_char("You dont have the questpoints.\n\r", ch );
         return;
      }
     // argument = one_argument( argument, arg2 );

      if (argument[0] == '\0' )  //check for new ship's name
      {
        send_to_char( "What do u want the ship name to be....?\n\r", ch );
        return;
      }

      if ( (ship = ship_from_cockpit(ch->in_room->vnum)) == NULL ) //check if the guy's in the cockpit
      {
          send_to_char("You must be in the cockpit of the ship to rename it!\n\r",ch);
          return;
      }

      if ( !check_pilot( ch, ship) )  //checks for legit ownership/pilot/copilot
      {
          send_to_char("Hey, you can't rename another person's ship!!!\n\r", ch);
      }

      ch->pcdata->quest_curr -= RSHIP_COST;  // subtracts qp
      STRFREE( ship->name );  // frees current ship pointer value, resets to NULL
      smash_tilde(argument); // Removes ~s, just in case ruins save ship.
      argument = stripclr(argument); // Strips Color
      argument[0] = UPPER(argument[0]); // Capitalizes First Letter
      ship->name = STRALLOC( argument ); // renames the ship
      send_to_char( "Your qp is my command.\n\r", ch);  // feedback to spender of qp
      save_ship( ship );  //saves the ship
      return;  //ends the function
   }
   else 
   {
	send_to_char( "That cannot be enhanced. Read HELP QPBUY for details.\n\r", ch);  
	return;
   }
}

void do_newscore(CHAR_DATA *ch, char *argument)
{

    char	buf[MAX_STRING_LENGTH];
    int iLang, drug;
    AFFECT_DATA    *paf;

    if (IS_NPC(ch))
    {
	do_oldscore(ch, argument);
    }
    
    ch_printf(ch, "\n\r&BScore for &C%s.\n\r", ch->pcdata->title);
    ch_printf(ch, "&BRace&z:     &C%3d year old %-10.10s   &BHealth&O: &C%7d - %d\n\r", get_age(ch), capitalize(get_race(ch)), ch->hit, ch->max_hit);
      if ( ch->skill_level[FORCE_ABILITY] > 1    || IS_IMMORTAL(ch) )
        ch_printf(ch, "&BAlign&z:     &C%-24d &BForce&O:  &C%7d - %d\n\r", ch->alignment, ch->mana, ch->max_mana);
    else
        ch_printf(ch, "&BAlign&z:     &C%-24d &BForce&O:     &C????? - ?????\n\r", ch->alignment, ch->mana, ch->max_mana);
        
    ch_printf(ch, "&BArmor&z:     &C%-24d &BMove&O:   &C%7d - %d\n\r", GET_AC(ch), ch->move, ch->max_move);
    ch_printf(ch, "&BHitroll&z:   &C%-24d &BCredits&O:   &C%d\n\r", GET_HITROLL(ch), ch->gold);
    ch_printf(ch, "&BDamroll&z:   &C%-24d &BBank&O:      &C%d\n\r", GET_DAMROLL(ch), ch->pcdata->bank);
    ch_printf(ch, "&BQP&z:        &C%-24d &BWimpy&O:     &C%d\n\r", ch->pcdata->quest_curr, ch->wimpy);
    ch_printf(ch, "&BQPA&z:       &C%-24d &BSubclass&O:  &C%4s\n\r", ch->pcdata->quest_accum, subclasses[ch->subclass])	 ;
    if ( GET_DAMAGE_RESISTANCE(ch) > 0)
    {    
    ch_printf(ch, "&BDm. Rst&O:   &C%-24d\n\r", GET_DAMAGE_RESISTANCE(ch));
    }
    ch_printf(ch, "&b[][][][][][][][][][][][][][][][][][][][][][][][][][][][][][][][][][][}\n\r");
    ch_printf(ch, "&b[] &CStr&z: &c%2d &CDex&z: &c%2d &CCon&z: &c%2d &CInt&z: &c%2d &CWis&z: &c%2d &CCha&z: &c%2d &CLck&z: &c?? &CFrc&z: &c ?? &b[]\n\r",
    			get_curr_str(ch), get_curr_dex(ch), get_curr_con(ch), get_curr_int(ch),
    			get_curr_wis(ch), get_curr_cha(ch));
    ch_printf(ch, "&b[][][][][][][][][][][][][][][][][][][][][][][][][][][][][][][][][][][}\n\r");
    ch_printf(ch, "&z&w/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\\n\r");
     {
       int ability;

       for ( ability = 0 ; ability < MAX_ABILITY ; ability++ )
          if ( ability < FORCE_ABILITY || ch->skill_level[FORCE_ABILITY] > 1 )
            ch_printf( ch, "&B%-15s  &CLevel&z:&c %-3d  &C Max&z:&c %-3d   &CExp&z:&c %-10ld  &C Next&z:&c %-10ld\n\r",
            ability_name[ability], ch->skill_level[ability], max_level(ch, ability), ch->experience[ability],
            exp_level( ch->skill_level[ability]+1 ) );
          else
            ch_printf( ch, "&B%-15s  &CLevel&z:&c %-3d   &CMax&z:&c ???   &CExp&z:&c ???          &CNext&z:&c ???\n\r",
            ability_name[ability], ch->skill_level[ability], ch->experience[ability]);
    } 
    ch_printf(ch, "&z&w/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\\n\r");
    ch_printf(ch, "&BPkills&z: &c%-10.5d &BWeight&O: &c%-6.5d &BMax&O: &c %-15d &BAutoexit &z(&c%c&z)\n\r", ch->pcdata->pkills, ch->carry_weight, can_carry_w(ch), IS_SET(ch->act, 
PLR_AUTOEXIT) ? 
'X' : ' ');
    ch_printf(ch, "&BMkills&z: &c%-10.5d &BItems&O:  &c%-6.5d &BMax&O: &c %-15d &BAutoloot &z(&c%c&z)\n\r", ch->pcdata->mkills, ch->carry_number, can_carry_n(ch), IS_SET(ch->act, 
PLR_AUTOLOOT) ? 
'X' : ' ');
    ch_printf(ch, "&BPager&z:  &z(&c%c&z)  &c%-42d &BAutosac  &z(&c%c&z)\n\r", IS_SET(ch->pcdata->flags, PCFLAG_PAGERON) ? 'X' : ' ', ch->pcdata->pagerlen,              IS_SET(ch->act, 
PLR_AUTOSAC) ? 'X' : ' ');
    
   // send_to_char(buf, ch);
        switch (ch->position)
    {
	case POS_DEAD:
		sprintf(buf, "&CYou are slowly decomposing. ");
		break;
	case POS_MORTAL:
		sprintf(buf, "&CYou are mortally wounded. ");
		break;
	case POS_INCAP:
		sprintf(buf, "&CYou are incapacitated. ");
		break;
	case POS_STUNNED:
		sprintf(buf, "&CYou are stunned. ");
		break;
	case POS_SLEEPING:
		sprintf(buf, "&CYou are sleeping. ");
		break;
	case POS_RESTING:
		sprintf(buf, "&CYou are resting. ");
		break;
	case POS_STANDING:
		sprintf(buf, "&CYou are standing. ");
		break;
	case POS_FIGHTING:
		sprintf(buf, "&CYou are fighting. " );
		break;
	case POS_MOUNTED:
		sprintf(buf, "&CYou are mounted. ");
		break;
        case POS_SITTING:
		sprintf(buf, "&CYou are sitting. ");
		break;
    }
    
    send_to_char( buf, ch );
    
    if (!IS_NPC(ch) && ch->pcdata->condition[COND_DRUNK] > 10)
	send_to_char("&RYou are drunk.\n\r", ch);
    if (!IS_NPC(ch) && ch->pcdata->condition[COND_THIRST] == 0)
	send_to_char("&RYou are in danger of dehydrating.\n\r", ch);
    if (!IS_NPC(ch) && ch->pcdata->condition[COND_FULL] == 0)
	send_to_char("&RYou are starving to death.\n\r", ch);
    if ( ch->position != POS_SLEEPING )
	switch( ch->mental_state / 10 )
	{
	    default:   send_to_char( "&BYou're completely messed up!&w\n\r", ch );	break;
	    case -10:  send_to_char( "&BYou're barely conscious.&w\n\r", ch );	break;
	    case  -9:  send_to_char( "&BYou can barely keep your eyes open.&w\n\r", ch );	break;
	    case  -8:  send_to_char( "&BYou're extremely drowsy.&w\n\r", ch );	break;
	    case  -7:  send_to_char( "&BYou feel very unmotivated.&w\n\r", ch );	break;
	    case  -6:  send_to_char( "&BYou feel sedated.&w\n\r", ch );		break;
	    case  -5:  send_to_char( "&BYou feel sleepy.&w\n\r", ch );		break;
	    case  -4:  send_to_char( "&BYou feel tired.&w\n\r", ch );		break;
	    case  -3:  send_to_char( "&BYou could use a rest.&w\n\r", ch );		break;
	    case  -2:  send_to_char( "&BYou feel a little under the weather, then you curse at your boss Mr. Weather to get the hell off you. &w\n\r", ch );	break;
	    case  -1:  send_to_char( "&BYou feel fine, fine enough to rob a liquor store and blame it on Puff, The Magical Dragon&w\n\r", ch );		break;
	    case   0:  send_to_char( "&BYou feel great.&w\n\r", ch );		break;
	    case   1:  send_to_char( "&BYou feel energetic.&w\n\r", ch );	break;
	    case   2:  send_to_char( "&BYour mind is racing with the monkey in your head, ahahahahaha&w\n\r", ch );	break;
	    case   3:  send_to_char( "&BYou can't think straight, you can only think of Raijen&w\n\r", ch );	break;
	    case   4:  send_to_char( "&BYour mind is going 100 miles an hour, into a brick wall.&w\n\r", ch );	break;
	    case   5:  send_to_char( "&RYou're high as a kite.&w :)\n\r", ch );	break;
	    case   6:  send_to_char( "&BYour mind and body are slipping apart.&w\n\r", ch );	break;
	    case   7:  send_to_char( "&BReality is slipping away.&w\n\r&w", ch );	break;
	    case   8:  send_to_char( "&BYou have no idea what is real, and what is not.&w\n\r", ch );	break;
	    case   9:  send_to_char( "&BYou feel immortal, now go jump in front of a bus.\n\r&w", ch );	break;
	    case  10:  send_to_char( "&BYou are a Supreme Entity, like a Big Freakin Badger!&w\n\r", ch );	break;
	}
    else
    if ( ch->mental_state > 45 )
	send_to_char( "&CYour sleep is filled with strange and vivid dreams.\n\r", ch );
    else
    if ( ch->mental_state >25 )
	send_to_char( "&CYour sleep is uneasy.\n\r", ch );
    else
    if ( ch->mental_state <-35 )
	send_to_char( "&CYou are deep in a much needed sleep.\n\r", ch );
    else
    if ( ch->mental_state <-25 )
	send_to_char( "&CYou are in deep slumber.\n\r", ch );

    send_to_char("&BSPICE &CLevel/Addiction: ", ch );
    for ( drug = 0; drug <= 9; drug++ )
	if ( ch->pcdata->drug_level[drug] > 0 || ch->pcdata->drug_level[drug] > 0 )
	{
	    ch_printf( ch, "&B%s&z(&c%d&z/&C%d&z) ", spice_table[drug],
	                                 ch->pcdata->drug_level[drug],
	                                 ch->pcdata->addiction[drug] );
	}
    send_to_char("\n\r\n\r&BLanguages&z: ", ch );
    for ( iLang = 0; lang_array[iLang] != LANG_UNKNOWN; iLang++ )
	if ( knows_language( ch, lang_array[iLang], ch )
	||  (IS_NPC(ch) && ch->speaks == 0) )
	{
	    if ( lang_array[iLang] & ch->speaking
	    ||  (IS_NPC(ch) && !ch->speaking) )
		set_char_color( AT_RED, ch );
	    send_to_char( lang_names[iLang], ch );
	    send_to_char( " ", ch );
	    set_char_color( AT_LBLUE, ch);
	}
    
   //send_to_char( "\n\r\n\r", ch );
       ch_printf( ch, "\n\r&RWANTED ON&z:&W&C %s\n\r",
             flag_string(ch->pcdata->wanted_flags, planet_flags) );
                            
    if ( ch->pcdata->clan )
    {
      ch_printf(ch, "&BORGANIZATION&z: &C%-35s &BPkills&z&w/&BDeaths&z: &C%3.3d&z&w/&C%3.3d",		ch->pcdata->clan->name, ch->pcdata->clan->pkills, ch->pcdata->clan->pdeaths) ;
      send_to_char( "\n\r\n\r", ch );
    }  
    if ( ch->pcdata->bestowments && ch->pcdata->bestowments[0] != '\0' )
        ch_printf( ch, "&BYou are bestowed with the command&z(&Bs&z)&O:&z&w %s.\n\r",
                ch->pcdata->bestowments );
    if(IS_IMMORTAL(ch))
    {
	ch_printf(ch, "&z&w/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\\n\r");
	ch_printf(ch, "&CIMMORTAL DATA&C:  Wizinvis [%s]  Wizlevel (%d)\n\r",		
        IS_SET(ch->act, PLR_WIZINVIS) ? "X" : " ", ch->pcdata->wizinvis );

	ch_printf(ch, "&W&CBamfin&C:  %s\n\r", (ch->pcdata->bamfin[0] != '\0')
		? ch->pcdata->bamfin : "%s appears in a swirling mist.", ch->name);
	ch_printf(ch, "&W&CBamfout&C: %s\n\r", (ch->pcdata->bamfout[0] != '\0')
		? ch->pcdata->bamfout : "%s leaves in a swirling mist.", ch->name);


	if (ch->pcdata->area)
	{
	    ch_printf(ch, "&CVnums :   Room (%-5.5d - %-5.5d) Object (%-5.5d - %-5.5d)   Mob (%-5.5d - %-5.5d)\n\r",
		ch->pcdata->area->low_r_vnum, ch->pcdata->area->hi_r_vnum,
		ch->pcdata->area->low_o_vnum, ch->pcdata->area->hi_o_vnum,
		ch->pcdata->area->low_m_vnum, ch->pcdata->area->hi_m_vnum);
	    ch_printf(ch, "&CArea Loaded &C[%s]\n\r", (IS_SET(ch->pcdata->area->status, AREA_LOADED)) ? "yes" : "no");
       }
    }   
    if (ch->first_affect)
    {
        int i;
        SKILLTYPE *sktmp;

        i = 0;
        send_to_char( "&z&w/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\\n\r", ch);
        send_to_char("&CAFFECT DATA:                            ", ch);
        for (paf = ch->first_affect; paf; paf = paf->next)
        {
            if ( (sktmp=get_skilltype(paf->type)) == NULL )
                continue;
            if (ch->top_level < 20)
            {
                ch_printf(ch, "[%-34.34s]    ", sktmp->name);
                if (i == 0)
                   i = 2;
                if ((++i % 3) == 0)
                   send_to_char("\n\r", ch);
             }
             else
             {
                if (paf->modifier == 0)
                    ch_printf(ch, "[%-24.24s;%5d rds]    ",
                        sktmp->name,
                        paf->duration);
                else
                if (paf->modifier > 999)
                    ch_printf(ch, "[%-15.15s; %7.7s;%5d rds]    ",
                        sktmp->name,
                        tiny_affect_loc_name(paf->location),
                        paf->duration);
                else
                    ch_printf(ch, "[%-11.11s;%+-3.3d %7.7s;%5d rds]    ",
                        sktmp->name,
                        paf->modifier,
                        tiny_affect_loc_name(paf->location),
                        paf->duration);
                if (i == 0)
                    i = 1;
                if ((++i % 2) == 0)
                    send_to_char("\n\r", ch);
            }
        }
    }
    send_to_char("\n\r", ch);
    return;
}

/*  New score command by Haus.  Modified by Shiro */
void do_score(CHAR_DATA * ch, char *argument)
{
    char            buf[MAX_STRING_LENGTH];
    AFFECT_DATA    *paf;
    int iLang, drug;
    if (IS_NPC(ch))
    {
	do_oldscore(ch, argument);
	return;
    }
    set_char_color(AT_SCORE, ch);

    ch_printf(ch, "\n\rScore for %s.\n\r", ch->pcdata->title);
    set_char_color(AT_SCORE, ch);
    if ( get_trust( ch ) != ch->top_level )
	ch_printf( ch, "You are trusted at level %d.\n\r", get_trust( ch ) );

    send_to_char("----------------------------------------------------------------------------\n\r", ch);

    ch_printf(ch,   "Race: %3d year old %-10.10s                Log In:  %s\r",
	get_age(ch), capitalize(get_race(ch)), ctime(&(ch->logon)) );
	
    ch_printf(ch,   "Hitroll: %-2.2d  Damroll: %-2.2d   Armor: %-4d        Saved:  %s\r",
		GET_HITROLL(ch), GET_DAMROLL(ch), GET_AC(ch),
		ch->save_time ? ctime(&(ch->save_time)) : "no\n" );

    ch_printf(ch,   "Align: %-5d    Wimpy: %-3d                    Time:   %s\r",
		ch->alignment, ch->wimpy  , ctime(&current_time) );

    if ( ch->skill_level[FORCE_ABILITY] > 1    || IS_IMMORTAL(ch) )
      	ch_printf(ch, "Hit Points: %d of %d     Move: %d of %d     Force: %d of %d\n\r",
            ch->hit, ch->max_hit, ch->move, ch->max_move, ch->mana, ch->max_mana );
    else
      	ch_printf(ch, "Hit Points: %d of %d     Move: %d of %d\n\r",
            ch->hit, ch->max_hit, ch->move, ch->max_move);

    ch_printf(ch, "Str: %2d  Dex: %2d  Con: %2d  Int: %2d  Wis: %2d  Cha: %2d  Lck: ??  Frc: ??\n\r",
	get_curr_str(ch), get_curr_dex(ch),get_curr_con(ch),get_curr_int(ch),get_curr_wis(ch),get_curr_cha(ch));

      /* ch_printf(ch, "&YDamcap: &R%d&C\n\r",
            GET_DAMCAP(ch)); */
    if (ch->subclass != SUBCLASS_NONE)
    {
       ch_printf(ch, "Subclass: %s\n\r", subclasses[ch->subclass]);
    }
    
    send_to_char("----------------------------------------------------------------------------\n\r", ch);

    { 
       int ability;
    
       for ( ability = 0 ; ability < MAX_ABILITY ; ability++ )
          if ( ability < FORCE_ABILITY || ch->skill_level[FORCE_ABILITY] > 1 )
            ch_printf( ch, "%-15s   Level: %-3d   Max: %-3d   Exp: %-10ld   Next: %-10ld\n\r",
            ability_name[ability], ch->skill_level[ability], max_level(ch, ability), ch->experience[ability],
            exp_level( ch->skill_level[ability]+1 ) );
          else
            ch_printf( ch, "%-15s   Level: %-3d   Max: ???   Exp: ???          Next: ???\n\r",
            ability_name[ability], ch->skill_level[ability], ch->experience[ability]);
    }

    send_to_char("----------------------------------------------------------------------------\n\r", ch);

    ch_printf( ch, "You have achieved %d glory during your life, and currently have %d.\n\r", ch->pcdata->quest_accum, ch->pcdata->quest_curr );

    if (ch->pcdata->status >= 0 )
        ch_printf( ch, "You have an honour score of %d.\n\r", ch->pcdata->status );
    
    ch_printf(ch, "CREDITS: %-10d   BANK: %-10d    Pkills: %-5.5d   Mkills: %-5.5d\n\r",
	ch->gold, ch->pcdata->bank, ch->pcdata->pkills, ch->pcdata->mkills);
    ch_printf(ch, "Weight: %5.5d (max %7.7d)    Items: %5.5d (max %5.5d)\n\r",
	ch->carry_weight, can_carry_w(ch) , ch->carry_number, can_carry_n(ch));
 
    ch_printf(ch, "Pager: (%c) %3d   AutoExit(%c)  AutoLoot(%c)  Autosac(%c)\n\r", 
	IS_SET(ch->pcdata->flags, PCFLAG_PAGERON) ? 'X' : ' ',
	ch->pcdata->pagerlen, IS_SET(ch->act, PLR_AUTOEXIT) ? 'X' : ' ', 
	IS_SET(ch->act, PLR_AUTOLOOT) ? 'X' : ' ', IS_SET(ch->act, PLR_AUTOSAC) ? 'X' : ' ');
    
    switch (ch->position)
    {
	case POS_DEAD:
		sprintf(buf, "You are slowly decomposing. ");
		break;
	case POS_MORTAL:
		sprintf(buf, "You are mortally wounded. ");
		break;
	case POS_INCAP:
		sprintf(buf, "You are incapacitated. ");
		break;
	case POS_STUNNED:
		sprintf(buf, "You are stunned. ");
		break;
	case POS_SLEEPING:
		sprintf(buf, "You are sleeping. ");
		break;
	case POS_RESTING:
		sprintf(buf, "You are resting. ");
		break;
	case POS_STANDING:
		sprintf(buf, "You are standing. ");
		break;
	case POS_FIGHTING:
		sprintf(buf, "You are fighting. " );
		break;
	case POS_MOUNTED:
		sprintf(buf, "You are mounted. ");
		break;
        case POS_SITTING:
		sprintf(buf, "You are sitting. ");
		break;
    }
    
    send_to_char( buf, ch );

    if (!IS_NPC(ch) && ch->blood < 51 && ch->blood > 25)
	send_to_char("You are bleeding.\n\r", ch);
    if (!IS_NPC(ch) && ch->blood < 26 && ch->blood > 6)
	send_to_char("You are bleeding heavily.\n\r", ch);
    if (!IS_NPC(ch) && ch->blood < 7)
	send_to_char("You are bleeding out on the ground.\n\r", ch);
    if (!IS_NPC(ch) && ch->pcdata->condition[COND_DRUNK] > 10)
	send_to_char("You are drunk.\n\r", ch);
    if (!IS_NPC(ch) && ch->pcdata->condition[COND_THIRST] == 0)
	send_to_char("You are in danger of dehydrating.\n\r", ch);
    if (!IS_NPC(ch) && ch->pcdata->condition[COND_FULL] == 0)
	send_to_char("You are starving to death.\n\r", ch);
    if ( ch->position != POS_SLEEPING )
	switch( ch->mental_state / 10 )
	{
	    default:   send_to_char( "You're completely messed up!\n\r", ch );	break;
	    case -10:  send_to_char( "You're barely conscious.\n\r", ch );	break;
	    case  -9:  send_to_char( "You can barely keep your eyes open.\n\r", ch );	break;
	    case  -8:  send_to_char( "You're extremely drowsy.\n\r", ch );	break;
	    case  -7:  send_to_char( "You feel very unmotivated.\n\r", ch );	break;
	    case  -6:  send_to_char( "You feel sedated.\n\r", ch );		break;
	    case  -5:  send_to_char( "You feel sleepy.\n\r", ch );		break;
	    case  -4:  send_to_char( "You feel tired.\n\r", ch );		break;
	    case  -3:  send_to_char( "You could use a rest.\n\r", ch );		break;
	    case  -2:  send_to_char( "You feel a little under the weather.\n\r", ch );	break;
	    case  -1:  send_to_char( "You feel fine.\n\r", ch );		break;
	    case   0:  send_to_char( "You feel great.\n\r", ch );		break;
	    case   1:  send_to_char( "You feel energetic.\n\r", ch );	break;
	    case   2:  send_to_char( "Your mind is racing.\n\r", ch );	break;
	    case   3:  send_to_char( "You can't think straight.\n\r", ch );	break;
	    case   4:  send_to_char( "Your mind is going 100 miles an hour.\n\r", ch );	break;
	    case   5:  send_to_char( "You're high as a kite.\n\r", ch );	break;
	    case   6:  send_to_char( "Your mind and body are slipping apart.\n\r", ch );	break;
	    case   7:  send_to_char( "Reality is slipping away.\n\r", ch );	break;
	    case   8:  send_to_char( "You have no idea what is real, and what is not.\n\r", ch );	break;
	    case   9:  send_to_char( "You feel immortal.\n\r", ch );	break;
	    case  10:  send_to_char( "You are a Supreme Entity.\n\r", ch );	break;
	}
    else
    if ( ch->mental_state >45 )
	send_to_char( "Your sleep is filled with strange and vivid dreams.\n\r", ch );
    else
    if ( ch->mental_state >25 )
	send_to_char( "Your sleep is uneasy.\n\r", ch );
    else
    if ( ch->mental_state <-35 )
	send_to_char( "You are deep in a much needed sleep.\n\r", ch );
    else
    if ( ch->mental_state <-25 )
	send_to_char( "You are in deep slumber.\n\r", ch );
    send_to_char("SPICE Level/Addiction: ", ch );
    for ( drug = 0; drug <= 9; drug++ )
	if ( ch->pcdata->drug_level[drug] > 0 || ch->pcdata->drug_level[drug] > 0 )
	{
	    ch_printf( ch, "%s(%d/%d) ", spice_table[drug],
	                                 ch->pcdata->drug_level[drug],
	                                 ch->pcdata->addiction[drug] );
	}
    send_to_char("\n\rLanguages: ", ch );
    for ( iLang = 0; lang_array[iLang] != LANG_UNKNOWN; iLang++ )
	if ( knows_language( ch, lang_array[iLang], ch )
	||  (IS_NPC(ch) && ch->speaks == 0) )
	{
	    if ( lang_array[iLang] & ch->speaking
	    ||  (IS_NPC(ch) && !ch->speaking) )
		set_char_color( AT_RED, ch );
	    send_to_char( lang_names[iLang], ch );
	    send_to_char( " ", ch );
	    set_char_color( AT_SCORE, ch );
	}
    
    send_to_char( "\n\r", ch );
    ch_printf( ch, "WANTED ON: %s\n\r",
             flag_string(ch->pcdata->wanted_flags, planet_flags) );
                            
    if ( ch->pcdata->bestowments && ch->pcdata->bestowments[0] != '\0' )
	ch_printf( ch, "You are bestowed with the command(s): %s.\n\r", 
		ch->pcdata->bestowments );

    if ( ch->pcdata->clan )
    {
      send_to_char( "----------------------------------------------------------------------------\n\r", ch);
      ch_printf(ch, "ORGANIZATION: %-35s Pkills/Deaths: %3.3d/%3.3d",
		ch->pcdata->clan->name, ch->pcdata->clan->pkills, ch->pcdata->clan->pdeaths) ;
      send_to_char( "\n\r", ch );
    }  
    if (IS_IMMORTAL(ch))
    {
	send_to_char( "----------------------------------------------------------------------------\n\r", ch);

	ch_printf(ch, "IMMORTAL DATA:  Wizinvis [%s]  Wizlevel (%d)\n\r",
		IS_SET(ch->act, PLR_WIZINVIS) ? "X" : " ", ch->pcdata->wizinvis );

	ch_printf(ch, "Bamfin:  %s\n\r", (ch->pcdata->bamfin[0] != '\0')
		? ch->pcdata->bamfin : "%s appears in a swirling mist.", ch->name);
	ch_printf(ch, "Bamfout: %s\n\r", (ch->pcdata->bamfout[0] != '\0')
		? ch->pcdata->bamfout : "%s leaves in a swirling mist.", ch->name);


	/* Area Loaded info - Scryn 8/11*/
	if (ch->pcdata->area)
	{
	    ch_printf(ch, "Vnums:   Room (%-5.5d - %-5.5d)   Object (%-5.5d - %-5.5d)   Mob (%-5.5d - %-5.5d)\n\r",
		ch->pcdata->area->low_r_vnum, ch->pcdata->area->hi_r_vnum,
		ch->pcdata->area->low_o_vnum, ch->pcdata->area->hi_o_vnum,
		ch->pcdata->area->low_m_vnum, ch->pcdata->area->hi_m_vnum);
	    ch_printf(ch, "Area Loaded [%s]\n\r", (IS_SET (ch->pcdata->area->status, AREA_LOADED)) ? "yes" : "no");
	}
    }
    if (ch->first_affect)
    {
	int i;
	SKILLTYPE *sktmp;

	i = 0;
	send_to_char( "----------------------------------------------------------------------------\n\r", ch);
	send_to_char("AFFECT DATA:                            ", ch);
	for (paf = ch->first_affect; paf; paf = paf->next)
	{
	    if ( (sktmp=get_skilltype(paf->type)) == NULL )
		continue;
	    if (ch->top_level < 20)
	    {
		ch_printf(ch, "[%-34.34s]    ", sktmp->name);
		if (i == 0)
		   i = 2;
		if ((++i % 3) == 0)
		   send_to_char("\n\r", ch);
	     }
	     else
	     {
		if (paf->modifier == 0)
		    ch_printf(ch, "[%-24.24s;%5d rds]    ",
			sktmp->name,
			paf->duration);
		else
		if (paf->modifier > 999)
		    ch_printf(ch, "[%-15.15s; %7.7s;%5d rds]    ",
			sktmp->name,
			tiny_affect_loc_name(paf->location),
			paf->duration);
		else
		    ch_printf(ch, "[%-11.11s;%+-3.3d %7.7s;%5d rds]    ",
			sktmp->name,
			paf->modifier,
			tiny_affect_loc_name(paf->location),
			paf->duration);
		if (i == 0)
		    i = 1;
		if ((++i % 2) == 0)
		    send_to_char("\n\r", ch);
	    }
	}
    }
    send_to_char("\n\r", ch);
    return;
}

/*
 * Return ascii name of an affect location.
 */
char           *
tiny_affect_loc_name(int location)
{
	switch (location) {
	case APPLY_NONE:		return "NIL";
	case APPLY_STR:			return " STR  ";
	case APPLY_DEX:			return " DEX  ";
	case APPLY_INT:			return " INT  ";
	case APPLY_WIS:			return " WIS  ";
	case APPLY_CON:			return " CON  ";
	case APPLY_CHA:			return " CHA  ";
	case APPLY_LCK:			return " LCK  ";
	case APPLY_SEX:			return " SEX  ";
	case APPLY_LEVEL:		return " LVL  ";
	case APPLY_AGE:			return " AGE  ";
	case APPLY_MANA:		return " MANA ";
	case APPLY_HIT:			return " HP   ";
	case APPLY_MOVE:		return " MOVE ";
	case APPLY_GOLD:		return " GOLD ";
	case APPLY_EXP:			return " EXP  ";
	case APPLY_AC:			return " AC   ";
	case APPLY_HITROLL:		return " HITRL";
	case APPLY_DAMROLL:		return " DAMRL";
	case APPLY_DAMCAP:		return "DAMCAP REMOVE ME";
	case APPLY_SAVING_POISON:	return "SV POI";
	case APPLY_SAVING_ROD:		return "SV ROD";
	case APPLY_SAVING_PARA:		return "SV PARA";
	case APPLY_SAVING_BREATH:	return "SV BRTH";
	case APPLY_SAVING_SPELL:	return "SV SPLL";
	case APPLY_HEIGHT:		return "HEIGHT";
	case APPLY_WEIGHT:		return "WEIGHT";
	case APPLY_AFFECT:		return "AFF BY";
	case APPLY_RESISTANT:		return "RESIST";
	case APPLY_IMMUNE:		return "IMMUNE";
	case APPLY_SUSCEPTIBLE:		return "SUSCEPT";
	case APPLY_WEAPONSPELL:		return " WEAPON";
	case APPLY_BACKSTAB:		return "BACKSTB";
	case APPLY_PICK:		return " PICK  ";
	case APPLY_TRACK:		return " TRACK ";
	case APPLY_STEAL:		return " STEAL ";
	case APPLY_SNEAK:		return " SNEAK ";
	case APPLY_HIDE:		return " HIDE  ";
	case APPLY_PALM:		return " PALM  ";
	case APPLY_DETRAP:		return " DETRAP";
	case APPLY_DODGE:		return " DODGE ";
	case APPLY_PEEK:		return " PEEK  ";
	case APPLY_SCAN:		return " SCAN  ";
	case APPLY_GOUGE:		return " GOUGE ";
	case APPLY_SEARCH:		return " SEARCH";
	case APPLY_MOUNT:		return " MOUNT ";
	case APPLY_DISARM:		return " DISARM";
	case APPLY_KICK:		return " KICK  ";
        case APPLY_NEGOTIATE:           return " NEGOTIATE";
	case APPLY_PARRY:		return " PARRY ";
	case APPLY_BASH:		return " BASH  ";
	case APPLY_STUN:		return " STUN  ";
	case APPLY_PUNCH:		return " PUNCH ";
	case APPLY_CLIMB:		return " CLIMB ";
	case APPLY_GRIP:		return " GRIP  ";
	case APPLY_SCRIBE:		return " SCRIBE";
	case APPLY_BREW:		return " BREW  ";
	case APPLY_WEARSPELL:		return " WEAR  ";
	case APPLY_REMOVESPELL:		return " REMOVE";
	case APPLY_EMOTION:		return "EMOTION";
	case APPLY_MENTALSTATE:		return " MENTAL";
	case APPLY_STRIPSN:		return " DISPEL";
	case APPLY_REMOVE:		return " REMOVE";
	case APPLY_DIG:			return " DIG   ";
	case APPLY_FULL:		return " HUNGER";
	case APPLY_THIRST:		return " THIRST";
	case APPLY_DRUNK:		return " DRUNK ";
	case APPLY_BLOOD:		return " BLOOD ";
	case APPLY_DAMAGE_RESISTANCE:	return "DM RES.";
	case APPLY_MAGIC_RESISTANCE:	return "MG RES.";
	case APPLY_ATTACK_SPEED:	return "INITITV";
	case APPLY_NUM_ATTACKS:		return "ATTACKS";

	}

	bug("Affect_location_name: unknown location %d.", location);
	return "UNKNOWN";
}

char *
get_race( CHAR_DATA *ch)
{
    if ( ch->race < MAX_NPC_RACE && ch->race >= 0)
	return ( npc_race[ch->race] );
    return ("Unknown");
}

void do_oldscore( CHAR_DATA *ch, char *argument )
{
    AFFECT_DATA *paf;
    SKILLTYPE   *skill;

    if ( IS_AFFECTED(ch, AFF_POSSESS) )
    {   
       send_to_char("You can't do that in your current state of mind!\n\r", ch);
       return;
    }

    set_char_color( AT_SCORE, ch );
    ch_printf( ch,
	"You are %s%s, level %d, %d years old (%d hours).\n\r",
	ch->name,
	IS_NPC(ch) ? "" : ch->pcdata->title,
	ch->top_level,
	get_age(ch),
	(get_age(ch) - 17) );

    if ( get_trust( ch ) != ch->top_level )
	ch_printf( ch, "You are trusted at level %d.\n\r",
	    get_trust( ch ) );

    if ( IS_SET(ch->act, ACT_MOBINVIS) )
      ch_printf( ch, "You are mobinvis at level %d.\n\r",
            ch->mobinvis);

    
      ch_printf( ch,
	"You have %d/%d hit, %d/%d movement.\n\r",
	ch->hit,  ch->max_hit,
	ch->move, ch->max_move);

    ch_printf( ch,
	"You are carrying %d/%d items with weight %d/%d kg.\n\r",
	ch->carry_number, can_carry_n(ch),
	ch->carry_weight, can_carry_w(ch) );

    ch_printf( ch,
	"Str: %d  Int: %d  Wis: %d  Dex: %d  Con: %d  Cha: %d  Lck: ??  Frc: ??\n\r",
	get_curr_str(ch),
	get_curr_int(ch),
	get_curr_wis(ch),
	get_curr_dex(ch),
	get_curr_con(ch),
	get_curr_cha(ch) );

    ch_printf( ch,
	"You have have %d credits.\n\r" , ch->gold );

    if ( !IS_NPC(ch) )
    ch_printf( ch, "You have achieved %d glory during your life, and currently have %d.\n\r", ch->pcdata->quest_accum, ch->pcdata->quest_curr );

    ch_printf( ch,
	"Autoexit: %s   Autoloot: %s   Autosac: %s   Autocred: %s\n\r",
	(!IS_NPC(ch) && IS_SET(ch->act, PLR_AUTOEXIT)) ? "yes" : "no",
	(!IS_NPC(ch) && IS_SET(ch->act, PLR_AUTOLOOT)) ? "yes" : "no",
	(!IS_NPC(ch) && IS_SET(ch->act, PLR_AUTOSAC) ) ? "yes" : "no",
  	(!IS_NPC(ch) && IS_SET(ch->act, PLR_AUTOGOLD)) ? "yes" : "no" );

    ch_printf( ch, "Wimpy set to %d hit points.\n\r", ch->wimpy );

    if ( !IS_NPC(ch) && ch->pcdata->condition[COND_DRUNK]   > 10 )
	send_to_char( "You are drunk.\n\r",   ch );
    if ( !IS_NPC(ch) && ch->pcdata->condition[COND_THIRST] ==  0 )
	send_to_char( "You are thirsty.\n\r", ch );
    if ( !IS_NPC(ch) && ch->pcdata->condition[COND_FULL]   ==  0 )
	send_to_char( "You are hungry.\n\r",  ch );

    switch( ch->mental_state / 10 )
    {
        default:   send_to_char( "You're completely messed up!\n\r", ch ); break;
        case -10:  send_to_char( "You're barely conscious.\n\r", ch ); break;
        case  -9:  send_to_char( "You can barely keep your eyes open.\n\r", ch ); break;
        case  -8:  send_to_char( "You're extremely drowsy.\n\r", ch ); break;
        case  -7:  send_to_char( "You feel very unmotivated.\n\r", ch ); break;
        case  -6:  send_to_char( "You feel sedated.\n\r", ch ); break;
        case  -5:  send_to_char( "You feel sleepy.\n\r", ch ); break;
        case  -4:  send_to_char( "You feel tired.\n\r", ch ); break;
        case  -3:  send_to_char( "You could use a rest.\n\r", ch ); break;
        case  -2:  send_to_char( "You feel a little under the weather.\n\r", ch ); break;
        case  -1:  send_to_char( "You feel fine.\n\r", ch ); break;
        case   0:  send_to_char( "You feel great.\n\r", ch ); break;
        case   1:  send_to_char( "You feel energetic.\n\r", ch ); break;
        case   2:  send_to_char( "Your mind is racing.\n\r", ch ); break;
        case   3:  send_to_char( "You can't think straight.\n\r", ch ); break;
        case   4:  send_to_char( "Your mind is going 100 miles an hour.\n\r", ch ); break;
        case   5:  send_to_char( "You're high as a kite.\n\r", ch ); break;
        case   6:  send_to_char( "Your mind and body are slipping appart.\n\r", ch ); break;
        case   7:  send_to_char( "Reality is slipping away.\n\r", ch ); break;
        case   8:  send_to_char( "You have no idea what is real, and what is not.\n\r", ch ); break;
        case   9:  send_to_char( "You feel immortal.\n\r", ch ); break;
        case  10:  send_to_char( "You are a Supreme Entity.\n\r", ch ); break;
    }

    switch ( ch->position )
    {
    case POS_DEAD:
	send_to_char( "You are DEAD!!\n\r",		ch );
	break;
    case POS_MORTAL:
	send_to_char( "You are mortally wounded.\n\r",	ch );
	break;
    case POS_INCAP:
	send_to_char( "You are incapacitated.\n\r",	ch );
	break;
    case POS_STUNNED:
	send_to_char( "You are stunned.\n\r",		ch );
	break;
    case POS_SLEEPING:
	send_to_char( "You are sleeping.\n\r",		ch );
	break;
    case POS_RESTING:
	send_to_char( "You are resting.\n\r",		ch );
	break;
    case POS_STANDING:
	send_to_char( "You are standing.\n\r",		ch );
	break;
    case POS_FIGHTING:
	send_to_char( "You are fighting.\n\r",		ch );
	break;
    case POS_MOUNTED:
	send_to_char( "Mounted.\n\r",			ch );
	break;
    case POS_SHOVE:
	send_to_char( "Being shoved.\n\r",		ch );
	break;
    case POS_DRAG:
	send_to_char( "Being dragged.\n\r",		ch );
	break;
    }

    if ( ch->top_level >= 25 )
	ch_printf( ch, "AC: %d.  ", GET_AC(ch) );

    send_to_char( "You are ", ch );
	 if ( GET_AC(ch) >=  101 ) send_to_char( "WORSE than naked!\n\r", ch );
    else if ( GET_AC(ch) >=   80 ) send_to_char( "naked.\n\r",            ch );
    else if ( GET_AC(ch) >=   60 ) send_to_char( "wearing clothes.\n\r",  ch );
    else if ( GET_AC(ch) >=   40 ) send_to_char( "slightly armored.\n\r", ch );
    else if ( GET_AC(ch) >=   20 ) send_to_char( "somewhat armored.\n\r", ch );
    else if ( GET_AC(ch) >=    0 ) send_to_char( "armored.\n\r",          ch );
    else if ( GET_AC(ch) >= - 20 ) send_to_char( "well armored.\n\r",     ch );
    else if ( GET_AC(ch) >= - 40 ) send_to_char( "strongly armored.\n\r", ch );
    else if ( GET_AC(ch) >= - 60 ) send_to_char( "heavily armored.\n\r",  ch );
    else if ( GET_AC(ch) >= - 80 ) send_to_char( "superbly armored.\n\r", ch );
    else if ( GET_AC(ch) >= -100 ) send_to_char( "divinely armored.\n\r", ch );
    else                           send_to_char( "invincible!\n\r",       ch );

    if ( ch->top_level >= 15 )
	ch_printf( ch, "Hitroll: %d  Damroll: %d.\n\r",
	    GET_HITROLL(ch), GET_DAMROLL(ch) );

    if ( ch->top_level >= 10 )
	ch_printf( ch, "Alignment: %d.  ", ch->alignment );

    send_to_char( "You are ", ch );
	 if ( ch->alignment >  900 ) send_to_char( "angelic.\n\r", ch );
    else if ( ch->alignment >  700 ) send_to_char( "saintly.\n\r", ch );
    else if ( ch->alignment >  350 ) send_to_char( "good.\n\r",    ch );
    else if ( ch->alignment >  100 ) send_to_char( "kind.\n\r",    ch );
    else if ( ch->alignment > -100 ) send_to_char( "neutral.\n\r", ch );
    else if ( ch->alignment > -350 ) send_to_char( "mean.\n\r",    ch );
    else if ( ch->alignment > -700 ) send_to_char( "evil.\n\r",    ch );
    else if ( ch->alignment > -900 ) send_to_char( "demonic.\n\r", ch );
    else                             send_to_char( "satanic.\n\r", ch );

    if ( ch->first_affect )
    {
	send_to_char( "You are affected by:\n\r", ch );
	for ( paf = ch->first_affect; paf; paf = paf->next )
	    if ( (skill=get_skilltype(paf->type)) != NULL )
	{
	    ch_printf( ch, "Spell: '%s'", skill->name );

	    if ( ch->top_level >= 20 )
		ch_printf( ch,
		    " modifies %s by %d for %d rounds",
		    affect_loc_name( paf->location ),
		    paf->modifier,
		    paf->duration );

	    send_to_char( ".\n\r", ch );
	}
    }

    if ( !IS_NPC( ch ) && IS_IMMORTAL( ch ) )
    {
	ch_printf( ch, "WizInvis level: %d   WizInvis is %s\n\r",
			ch->pcdata->wizinvis,
			IS_SET( ch->act, PLR_WIZINVIS ) ? "ON" : "OFF" );
	if ( ch->pcdata->r_range_lo && ch->pcdata->r_range_hi )
	  ch_printf( ch, "Room Range: %d - %d\n\r", ch->pcdata->r_range_lo,
					 	   ch->pcdata->r_range_hi	);
	if ( ch->pcdata->o_range_lo && ch->pcdata->o_range_hi )
	  ch_printf( ch, "Obj Range : %d - %d\n\r", ch->pcdata->o_range_lo,
	  					   ch->pcdata->o_range_hi	);
	if ( ch->pcdata->m_range_lo && ch->pcdata->m_range_hi )
	  ch_printf( ch, "Mob Range : %d - %d\n\r", ch->pcdata->m_range_lo,
	  					   ch->pcdata->m_range_hi	);
    }

    return;
}

/*								-Thoric
 * Display your current exp, level, and surrounding level exp requirements
 */
void do_level( CHAR_DATA *ch, char *argument )
{ 
       int ability;
    
       for ( ability = 0 ; ability < MAX_ABILITY ; ability++ )

          if ( ability < FORCE_ABILITY || ch->skill_level[FORCE_ABILITY] > 1 )

            ch_printf( ch, "%-15s   Level: %-3d   Max: %-3d   Exp: %-10ld   Next: %-10ld\n\r",
            ability_name[ability], ch->skill_level[ability], max_level(ch, ability), ch->experience[ability],
            exp_level( ch->skill_level[ability]+1 ) );
          else
            ch_printf( ch, "%-15s   Level: %-3d   Max: ???   Exp: ???          Next: ???\n\r",
            ability_name[ability], ch->skill_level[ability], ch->experience[ability]);
}


void do_affected ( CHAR_DATA *ch, char *argument )
{
    char arg [MAX_INPUT_LENGTH];
    AFFECT_DATA *paf;
    SKILLTYPE *skill;
 
    if ( IS_NPC(ch) )
        return;

    argument = one_argument( argument, arg );

    if ( !str_cmp( arg, "by" ) )
    {
        set_char_color( AT_BLUE, ch );
        send_to_char( "\n\r&Y-&G=&Y-&G=&Y-&G=&Y-&G=&Y-&G=&Y-&G=&Y-&C Affects &Y-&G=&Y-&G=&Y-&G=&Y-&G=&Y-&G=&Y-&G=&Y-\n\r", ch );

	if ( IS_AFFECTED(ch, AFF_BLIND) || IS_AFFECTED(ch, AFF_WEAKEN) || IS_AFFECTED(ch, AFF_POISON) || IS_AFFECTED(ch, AFF_SLEEP) || IS_AFFECTED(ch, AFF_PARALYSIS) || IS_AFFECTED(ch, AFF_POSSESS) || IS_AFFECTED(ch, AFF_CHARM) )
	        send_to_char( "\n\r&Y-&G=&Y-&G=&Y-&G=&Y-&R Negative &Y-&G=&Y-&G=&Y-&G=&Y-\n\r", ch );

	if ( IS_AFFECTED(ch, AFF_BLIND) )
       	  send_to_char( "&RBlindness&Y    -&W You cannot see anything!\n\r", ch );
	if ( IS_AFFECTED(ch, AFF_WEAKEN) )
       	  send_to_char( "&RWeaken&Y       -&W You feel as weak as a kitten!\n\r", ch );
	if ( IS_AFFECTED(ch, AFF_POISON) )
       	  send_to_char( "&RPoison&Y       -&W You have been infected by a poison!\n\r", ch );
	if ( IS_AFFECTED(ch, AFF_SLEEP) )
       	  send_to_char( "&RSleep&Y        -&W You are in an enchanted slumber!\n\r", ch );
	if ( IS_AFFECTED(ch, AFF_PARALYSIS) )
       	  send_to_char( "&RStunned&Y      -&W You are stunned!\n\r", ch );
	if ( IS_AFFECTED(ch, AFF_POSSESS) )
       	  send_to_char( "&RPosessed&Y     -&W You have been posessed by a spirit!\n\r", ch );
	if ( IS_AFFECTED(ch, AFF_CHARM) )
       	  send_to_char( "&RCharmed&Y      -&W You are under someone's glamour!\n\r", ch );
	if ( IS_AFFECTED(ch, AFF_BERSERK) )
	{
          send_to_char( "\n\r&Y-&G=&Y-&G=&Y-&G=&Y-&Y Neutral &Y-&G=&Y-&G=&Y-&G=&Y-\n\r", ch );
       	  send_to_char( "&YBerserk&Y      -&W You are in a battle rage!!!\n\r", ch );
	}

	if ( IS_AFFECTED(ch, AFF_INVISIBLE) || IS_AFFECTED(ch, AFF_HIDE) || IS_AFFECTED(ch, AFF_SNEAK) || IS_AFFECTED(ch, AFF_AQUA_BREATH) || IS_AFFECTED(ch, AFF_REGEN) || IS_AFFECTED(ch, AFF_PASS_DOOR) || IS_AFFECTED(ch, AFF_FLYING) || IS_AFFECTED(ch, AFF_FLOATING)
           || IS_AFFECTED(ch, AFF_SILVERTONGUE) )
        send_to_char( "\n\r&Y-&G=&Y-&G=&Y-&G=&Y-&G Positive &Y-&G=&Y-&G=&Y-&G=&Y-\n\r", ch );

	if ( IS_AFFECTED(ch, AFF_INVISIBLE) )
       	  send_to_char( "&GInvisibility&Y -&W You cannot be seen unaided.\n\r", ch );
	if ( IS_AFFECTED(ch, AFF_HIDE) )
       	  send_to_char( "&GHidden&Y       -&W You are hidden in the shadows.\n\r", ch );
	if ( IS_AFFECTED(ch, AFF_SNEAK) )
       	  send_to_char( "&GSneak&Y        -&W You sneak from room to room.\n\r", ch );
	if ( IS_AFFECTED(ch, AFF_AQUA_BREATH) )
       	  send_to_char( "&GAqua-Breath&Y  -&W You can breathe underwater.\n\r", ch );
	if ( IS_AFFECTED(ch, AFF_REGEN) )
       	  send_to_char( "&GRegenerating&Y -&W Your wounds are healing impossibly quickly.\n\r", ch );
	if ( IS_AFFECTED(ch, AFF_PASS_DOOR) )
       	  send_to_char( "&GEthreal&Y      -&W Your pass through doors like a ghost.\n\r", ch );
	if ( IS_AFFECTED(ch, AFF_FLYING) )
       	  send_to_char( "&GFlying&Y       -&W Your can fly through the sky with ease.\n\r", ch );
	if ( IS_AFFECTED(ch, AFF_FLOATING) )
       	  send_to_char( "&GFloating&Y     -&W Your feet float inches above the ground\n\r", ch );
        if ( IS_AFFECTED(ch ,AFF_SILVERTONGUE ))
          send_to_char( "&GSilvertongue&Y -&W Shopkeepers cut their prices for you.\n\r", ch);


	if ( IS_AFFECTED(ch, AFF_SANCTUARY) || IS_AFFECTED(ch, AFF_PROTECT) || IS_AFFECTED(ch, AFF_DIPIMMUNITY) )
        send_to_char( "\n\r&Y-&G=&Y-&G=&Y-&G=&Y-&W Shields/Defenses &Y-&G=&Y-&G=&Y-&G=&Y-\n\r", ch );

	if ( IS_AFFECTED(ch, AFF_SANCTUARY) )
       	  send_to_char( "&GSanctuary&Y     -&W Take 1/2 damage from all attacks.\n\r", ch );
	if ( IS_AFFECTED(ch, AFF_PROTECT) )
       	  send_to_char( "&GProtection&Y    -&W Removes 1/4 damage from all sources.\n\r", ch );
        if ( IS_AFFECTED(ch, AFF_DIPIMMUNITY ))
          send_to_char( "&GDip_Immunity&Y  -&W You are protected from wanted flags.\n\r", ch);	
	if ( IS_AFFECTED(ch, AFF_INFRARED) || IS_AFFECTED(ch, AFF_SCRYING) || IS_AFFECTED(ch, AFF_TRUESIGHT) || IS_AFFECTED(ch, AFF_DETECT_INVIS) || IS_AFFECTED(ch, AFF_DETECT_HIDDEN) || IS_AFFECTED(ch, AFF_DETECTTRAPS) || IS_AFFECTED(ch, AFF_DETECT_MAGIC) || IS_AFFECTED(ch, AFF_DETECT_EVIL) || IS_AFFECTED(ch, AFF_SIT_AWARE) || (ch->race == RACE_DUINUOGWUIN == GET_AGE(ch) >= 75 ))
        
        send_to_char( "\n\r&Y-&G=&Y-&G=&Y-&G=&Y-&W Detection &Y-&G=&Y-&G=&Y-&G=&Y-\n\r", ch );

	if (ch->race == RACE_DUINUOGWUIN && GET_AGE(ch) >= 75 )
       	  send_to_char( "&WDragonsight&Y    -&WInvisible/Hidden things cannot hide from your vision\n\r", ch );
	if ( IS_AFFECTED(ch, AFF_INFRARED) )
       	  send_to_char( "&WInfrared&Y       -&W You can see heat sources in pitch black\n\r", ch );
	if ( IS_AFFECTED(ch, AFF_SCRYING) )
       	  send_to_char( "&WScrying&Y        -&W You can see into ajacent rooms with crystal clarity\n\r", ch );
	if ( IS_AFFECTED(ch, AFF_TRUESIGHT) )
	{
       	  send_to_char( "&WTruesight&Y      -&W You can see through disguises and illusions.\n\r", ch );
       	  send_to_char( "&W         &Y      -&W Also prevents blindness and lets you see in dark rooms\n\r", ch );
	}
	if ( IS_AFFECTED(ch, AFF_SIT_AWARE) )
       	  send_to_char( "&WSit. Awareness&Y -&W You are aware of things happening around you.\n\r", ch );
	if ( IS_AFFECTED(ch, AFF_DETECT_INVIS) )
       	  send_to_char( "&WDetect Invis.&Y  -&W You can see invisible people/items.\n\r", ch );
	if ( IS_AFFECTED(ch, AFF_DETECT_HIDDEN) )
       	  send_to_char( "&WDetect Hidden&Y  -&W You can see hidden people/items.\n\r", ch );
	if ( IS_AFFECTED(ch, AFF_DETECTTRAPS) )
       	  send_to_char( "&WDetect Traps&Y   -&W You can see unsprung traps.\n\r", ch );
	if ( IS_AFFECTED(ch, AFF_DETECT_MAGIC) )
       	  send_to_char( "&WDetect Magic&Y   -&W You can see magical auras.\n\r", ch );
	if ( IS_AFFECTED(ch, AFF_DETECT_EVIL) )
       	  send_to_char( "&WDetect Evil&Y    -&W You can see evil auras.\n\r", ch );
        if ( IS_AFFECTED(ch, AFF_MASKED_AURA) )
          send_to_char( "&WMasked Aura&Y    -&W You have hidden your auras.\n\r", ch);
        if ( IS_AFFECTED(ch, AFF_TRACING) )
          send_to_char( "&WTrace Comlink&Y  -&W You detect the source of incoming tells.\n\r", ch);
	set_char_color( AT_SCORE, ch );
            send_to_char( "\n\r", ch );
            if ( ch->resistant > 0 )
	    {
		set_char_color ( AT_BLUE, ch );
                send_to_char( "Resistances:  ", ch );
                set_char_color( AT_SCORE, ch );
                ch_printf( ch, "%s\n\r", flag_string(ch->resistant, ris_flags) );
	    }
            if ( ch->immune > 0 )
	    {
                set_char_color( AT_BLUE, ch );
                send_to_char( "Immunities:   ", ch);
                set_char_color( AT_SCORE, ch );
                ch_printf( ch, "%s\n\r", flag_string(ch->immune, ris_flags) );
	    }
            if ( ch->susceptible > 0 )
	    {
                set_char_color( AT_BLUE, ch );
                send_to_char( "Suscepts:     ", ch );
		set_char_color( AT_SCORE, ch );
                ch_printf( ch, "%s\n\r", flag_string(ch->susceptible, ris_flags) );
	    }
	      	ch_printf(ch, "&CDamage Resistance:&Y %d\n\r",
	            GET_DAMAGE_RESISTANCE(ch) );
//	      	ch_printf(ch, "&CMagic Resistance:&Y  %d\n\r",
//	            ch->magic_resistance );
	
	return;      
    }

    if ( !ch->first_affect )
    {
        set_char_color( AT_SCORE, ch );
        send_to_char( "\n\rNo cantrip or skill affects you.\n\r", ch );
    }
    else
    {
	send_to_char( "\n\r", ch );
        for (paf = ch->first_affect; paf; paf = paf->next)
	    if ( (skill=get_skilltype(paf->type)) != NULL )
        {
            set_char_color( AT_BLUE, ch );
            send_to_char( "Affected:  ", ch );
            set_char_color( AT_SCORE, ch );
            if ( ch->top_level >= 20 )
            {
                if (paf->duration < 25 ) set_char_color( AT_WHITE, ch );
                if (paf->duration < 6  ) set_char_color( AT_WHITE + AT_BLINK, ch );
                ch_printf( ch, "(%5d)   ", paf->duration );
	    }
            ch_printf( ch, "%-18s\n\r", skill->name );
        }
    }
    return;
}

void do_inventory( CHAR_DATA *ch, char *argument )
{
    set_char_color( AT_RED, ch );
    send_to_char( "You are carrying:\n\r", ch );
    show_list_to_char( ch->first_carrying, ch, TRUE, TRUE );
    return;
}


void do_equipment( CHAR_DATA *ch, char *argument )
{
    OBJ_DATA *obj;
    int iWear, dam;
    bool found;
    char buf[MAX_STRING_LENGTH];
    
    set_char_color( AT_RED, ch );
    send_to_char( "You are using:\n\r", ch );
    found = FALSE;
    set_char_color( AT_OBJECT, ch );
    for ( iWear = 0; iWear < MAX_WEAR; iWear++ )
    {
	for ( obj = ch->first_carrying; obj; obj = obj->next_content )
	   if ( obj->wear_loc == iWear )
	   {
		send_to_char( where_name[iWear], ch );
		if ( can_see_obj( ch, obj ) )
		{
		    send_to_char( format_obj_to_char( obj, ch, TRUE ), ch );
		    strcpy( buf , "&G" );
		    switch ( obj->item_type )
	            {
	                default:
	                break;

	                case ITEM_ARMOR:
	    		    if ( obj->value[1] == 0 )
	      			obj->value[1] = obj->value[0];
	    		    if ( obj->value[1] == 0 )
	      			obj->value[1] = 1;
	    		    dam = (sh_int) ((obj->value[0] * 10) / obj->value[1]);
			    strcat( buf, "&G");
			    if (dam >= 10) strcat( buf, " (superb) ");
			    else if (dam >=  7) strcat( buf, " (good) ");
 		            else if (dam >=  5) strcat( buf, " (worn) ");
			    else if (dam >=  3) strcat( buf, " (poor) ");
			    else if (dam >=  1) strcat( buf, " (awful) ");
			    else if (dam ==  0) strcat( buf, " (broken) ");
                  	    send_to_char( buf, ch );
	                    break;

	                 case ITEM_WEAPON:
	                   dam = INIT_WEAPON_CONDITION - obj->value[0];
			   strcat( buf, "&G");
	                   if (dam < 2) strcat( buf, " (superb) ");
                           else if (dam < 4) strcat( buf, " (good) ");
                           else if (dam < 7) strcat( buf, " (worn) ");
                           else if (dam < 10) strcat( buf, " (poor) ");
                           else if (dam < 12) strcat( buf, " (awful) ");
                           else if (dam ==  12) strcat( buf, " (broken) ");
                	   send_to_char( buf, ch );
	                   if (obj->value[3] == WEAPON_BLASTER )
	                   {
		            if (obj->blaster_setting == BLASTER_FULL)
	    		      ch_printf( ch, "FULL");
	  	            else if (obj->blaster_setting == BLASTER_HIGH)
	    		      ch_printf( ch, "HIGH");
	  	            else if (obj->blaster_setting == BLASTER_NORMAL)
	    		      ch_printf( ch, "NORMAL");
	  	            else if (obj->blaster_setting == BLASTER_HALF)
	    		      ch_printf( ch, "HALF");
	  	            else if (obj->blaster_setting == BLASTER_LOW)
	    		      ch_printf( ch, "LOW");
	  	            else if (obj->blaster_setting == BLASTER_STUN)
	    		      ch_printf( ch, "STUN");
	  	            ch_printf( ch, " %d", obj->value[4] );
	                   }
	                   else if (     ( obj->value[3] == WEAPON_LIGHTSABER || 
		           obj->value[3] == WEAPON_VIBRO_BLADE
                           || obj->value[3] == WEAPON_SLUGTHROWER  
		           || obj->value[3] == WEAPON_FORCE_PIKE 
		           || obj->value[3] == WEAPON_BOWCASTER 
                           || obj->value[3] == WEAPON_VIBRO_AXE
                           || obj->value[3] == WEAPON_FLAMETHROWER
                           || obj->value[3] == WEAPON_VIBRO_SWORD ) )
	                   {
		             ch_printf( ch, "%d", obj->value[4] );
	                   }        
	                   break;
                    }   
		    send_to_char( "\n\r", ch );
		}
		else
		    send_to_char( "&gsomething.\n\r", ch );
		found = TRUE;
	   }
    }

    if ( !found )
	send_to_char( "Nothing.\n\r", ch );
    return;
}



void set_title( CHAR_DATA *ch, char *title )
{
    char buf[MAX_STRING_LENGTH];

    if ( IS_NPC(ch) )
    {
	bug( "Set_title: NPC.", 0 );
	return;
    }

    if ( isalpha(title[0]) || isdigit(title[0]) )
    {
	buf[0] = ' ';
	strcpy( buf+1, title );
    }
    else
	strcpy( buf, title );

    STRFREE( ch->pcdata->title );
    ch->pcdata->title = STRALLOC( buf );
    return;
}



void do_title( CHAR_DATA *ch, char *argument )
{
    char foo[MAX_STRING_LENGTH];
    if ( IS_NPC(ch) )
	return;

    if ( IS_SET( ch->pcdata->flags, PCFLAG_NOTITLE ))
    {
        send_to_char( "You try but the Force resists you.\n\r", ch );
        return;
    }
 

    if ( argument[0] == '\0' )
    {
	send_to_char( "Change your title to what?\n\r", ch );
	return;
    }
    strcpy( foo, argument );
    if ((get_trust(ch) <= LEVEL_IMMORTAL) && (!nifty_is_name(NAME(ch), stripclr( foo ) ) ) )
     {
       send_to_char("You must include your name/disguise name somewhere in your title!", ch);
       return;
     }
 
    smash_tilde( argument );
    set_title( ch, argument );
    send_to_char( "Ok.\n\r", ch );
}


void do_homepage( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_STRING_LENGTH];

    if ( IS_NPC(ch) )
	return;

    if ( argument[0] == '\0' )
    {
	if ( !ch->pcdata->homepage )
	  ch->pcdata->homepage = str_dup( "" );
	ch_printf( ch, "Your homepage is: %s\n\r",
		show_tilde( ch->pcdata->homepage ) );
	return;
    }

    if ( !str_cmp( argument, "clear" ) )
    {
	if ( ch->pcdata->homepage )
	  DISPOSE(ch->pcdata->homepage);
	ch->pcdata->homepage = str_dup("");
	send_to_char( "Homepage cleared.\n\r", ch );
	return;
    }

    if ( strstr( argument, "://" ) )
	strcpy( buf, argument );
    else
	sprintf( buf, "http://%s", argument );
    if ( strlen(buf) > 70 )
	buf[70] = '\0';

    hide_tilde( buf );
    if ( ch->pcdata->homepage )
      DISPOSE(ch->pcdata->homepage);
    ch->pcdata->homepage = str_dup(buf);
    send_to_char( "Homepage set.\n\r", ch );
}



/*
 * Set your personal description				-Thoric
 */
void do_description( CHAR_DATA *ch, char *argument )
{
    if ( IS_NPC( ch ) )
    {
	send_to_char( "Monsters are too dumb to do that!\n\r", ch );
	return;	  
    }

    if ( !ch->desc )
    {
	bug( "do_description: no descriptor", 0 );
	return;
    }

    switch( ch->substate )
    {
	default:
	   bug( "do_description: illegal substate", 0 );
	   return;

	case SUB_RESTRICTED:
	   send_to_char( "You cannot use this command from within another command.\n\r", ch );
	   return;

	case SUB_NONE:
	   ch->substate = SUB_PERSONAL_DESC;
	   ch->dest_buf = ch;
	   start_editing( ch, ch->description );
	   return;

	case SUB_PERSONAL_DESC:
	   STRFREE( ch->description );
	   ch->description = copy_buffer( ch );
	   stop_editing( ch );
	   return;	
    }
}

/* Ripped off do_description for whois bio's -- Scryn*/
void do_bio( CHAR_DATA *ch, char *argument )
{
    if ( IS_NPC( ch ) )
    {
	send_to_char( "Mobs can't set bio's!\n\r", ch );
	return;	  
    }

    if ( !ch->desc )
    {
	bug( "do_bio: no descriptor", 0 );
	return;
    }

    switch( ch->substate )
    {
	default:
	   bug( "do_bio: illegal substate", 0 );
	   return;
	  	   
	case SUB_RESTRICTED:
	   send_to_char( "You cannot use this command from within another command.\n\r", ch );
	   return;

	case SUB_NONE:
	   ch->substate = SUB_PERSONAL_BIO;
	   ch->dest_buf = ch;
	   start_editing( ch, ch->pcdata->bio );
	   return;

	case SUB_PERSONAL_BIO:
	   STRFREE( ch->pcdata->bio );
	   ch->pcdata->bio = copy_buffer( ch );
	   stop_editing( ch );
	   return;	
    }
}

void do_style (CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    sh_int maxstyle;

  smash_tilde( argument );
    maxstyle = ch->skill_level[COMBAT_ABILITY];
    if (ch->subclass == SUBCLASS_MARTIST)
	maxstyle += 50;
    if (ch->main_ability == COMBAT_ABILITY)
	maxstyle += 25;
    if ( IS_IMMORTAL(ch) )
	maxstyle = 666;
  one_argument( argument, arg );

  if ( !str_cmp(arg, "evasive") )
  {
    ch->fstyle = STYLE_EVASIVE;
    send_to_char( "You now are in a evasive fighting style.\n\r", ch );
    return;
  }

  if ( !str_cmp(arg, "agressive") )
  {
    ch->fstyle = STYLE_AGRESSIVE;
    send_to_char( "You now are in a agressive fighting style.\n\r", ch );
    return;
  }

  if ( !str_cmp(arg, "standard") )
  {
    ch->fstyle = STYLE_STANDARD;
    send_to_char( "You now are in a standard fighting style.\n\r", ch );
    return;
  }

  if ( !str_cmp(arg, "offensive") )
  {
    ch->fstyle = STYLE_OFFENSIVE;
    send_to_char( "You now are in a offensive fighting style.\n\r", ch );
    return;
  }

  if ( !str_cmp(arg, "defensive") )
  {
    ch->fstyle = STYLE_DEFENSIVE;
    send_to_char( "You now are in a defensive fighting style.\n\r", ch );
    return;
  }

  ch_printf( ch, "\n\r&RFightstyles\n\r&Y-=-=-=-=-=-\n\r&GMax Ability = &Y%d\n\r\n\r&CCurrent Scores&G\n\rEvasive - &R%d\n\r&GDefensive - &R%d&G\n\rOffensive - &R%d\n\r&GAgressive - &r%d&w\n\r", maxstyle, ch->evasive_skill, ch->defensive_skill, ch->offensive_skill, ch->agressive_skill );

  return;
}

void do_report( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_INPUT_LENGTH];

    if ( IS_AFFECTED(ch, AFF_POSSESS) )
    {   
       send_to_char("You can't do that in your current state of mind!\n\r", ch);
       return;
    }

    
      ch_printf( ch,
	"You report: %d/%d hp %d/%d mv.\n\r",
	ch->hit,  ch->max_hit,
	ch->move, ch->max_move   );

    
      sprintf( buf, "$n reports: %d/%d hp %d/%d.",
	ch->hit,  ch->max_hit,
	ch->move, ch->max_move   );

    act( AT_REPORT, buf, ch, NULL, NULL, TO_ROOM );

    return;
}

void do_prompt( CHAR_DATA *ch, char *argument )
{
  char arg[MAX_INPUT_LENGTH];
  
  if ( IS_NPC(ch) )
  {
    send_to_char( "NPC's can't change their prompt..\n\r", ch );
    return;
  }
  smash_tilde( argument );
  one_argument( argument, arg );
  if ( !*arg )
  {
//    send_to_char( "Set prompt to what? (try help prompt)\n\r", ch );
      if (!ch->pcdata->prompt || !str_cmp(ch->pcdata->prompt, ""))
        send_to_char( "You have no custom prompt.  See 'help prompt' for help on creating one.\n\r", ch);
      else
        ch_printf( ch,  "Set prompt to what?  Current prompt is\n\r%s\n\rSee 'Help Prompt' for help on creating your prompt", ch->pcdata->prompt );
    return;
  }
  if (ch->pcdata->prompt)
    STRFREE(ch->pcdata->prompt);

  if ( strlen(argument) > 128 )
    argument[128] = '\0';

  /* Can add a list of pre-set prompts here if wanted.. perhaps
     'prompt 1' brings up a different, pre-set prompt */
  if ( !str_cmp(arg, "default") )
    ch->pcdata->prompt = STRALLOC("");
  else
    ch->pcdata->prompt = STRALLOC(argument);
  send_to_char( "Ok.\n\r", ch );
  return;
}

void do_subclass( CHAR_DATA *ch, char *argument )
{

    if ( IS_NPC(ch) )
	return;

    if ( !IS_IMMORTAL(ch) && ch->subclass != SUBCLASS_NONE )
    {
	send_to_char( "You've already got a subclass.\n\r", ch );
	return;
    }

    if ( !str_cmp( argument, "sniper" ) )
    {
	if ( ch->skill_level[COMBAT_ABILITY] >= 100 )
	{
		send_to_char("Congratulations, you are now a sniper.\n\r", ch);
		ch->subclass = SUBCLASS_SNIPER;
		return;
	}
	else
	{
		send_to_char("You don't have enough levels in the requested subclass' class.\n\rPlease attain level 100 in the appropriate ability, and try again.\n\r",ch);
		return;
	}
    }

    if ( !str_cmp( argument, "tank" ) )
    {
	if ( ch->skill_level[COMBAT_ABILITY] >= 100 )
	{
		send_to_char("Congratulations, you are now a tank.\n\r", ch);
		ch->subclass = SUBCLASS_TANK;
		ch->max_hit += 250;
		ch->hit += 250;
		return;
	}
	else
	{
		send_to_char("You don't have enough levels in the requested subclass' class.\n\rPlease attain level 100 in the appropriate ability, and try again.\n\r",ch);
		return;
	}
    }

    if ( !str_cmp( argument, "swordsman" ) )
    {
	if ( ch->skill_level[COMBAT_ABILITY] >= 100)
	{
		send_to_char("Congratulations, you are now a master swordsman.\n\r", ch);
		ch->subclass = SUBCLASS_BLADEMASTER;
		return;
	}
	else
	{
		send_to_char("You don't have enough levels in the requested subclass' class.\n\rPlease attain level 100 in the appropriate ability, and try again.\n\r",ch);
		return;
	}
    }


    if ( !str_cmp( argument, "martialartist" ) || !str_cmp(argument, "martial artist")
      || !str_cmp(argument, "martial_artist") )
    {
	if ( ch->skill_level[COMBAT_ABILITY] >= 100 )
	{
		send_to_char("Congratulations, you are now a martial artist.\n\r", ch);
		ch->subclass = SUBCLASS_MARTIST;
		return;
	}
	else
	{
		send_to_char("You don't have enough levels in the requested subclass' class.\n\rPlease attain level 100 in the appropriate ability, and try again.\n\r",ch);
		return;
	}
    }

    if ( !str_cmp( argument, "fighter pilot" ) || !str_cmp(argument, "fighter_pilot") 
      || !str_cmp(argument, "fighterpilot"))
    {
	if ( ch->skill_level[PILOTING_ABILITY] >= 100 )
	{
		send_to_char("Congratulations, you are now a fighter pilot.\n\r", ch);
		ch->subclass = SUBCLASS_WFOCUS;
		return;
	}
	else
	{
		send_to_char("You don't have enough levels in the requested subclass' class.\n\rPlease attain level 100 in the appropriate ability, and try again.\n\r",ch);
		return;
	}
    }

    if ( !str_cmp( argument, "weaponsmith" ) )
    {
	if ( ch->skill_level[ENGINEERING_ABILITY] >= 100 )
	{
		send_to_char("Congratulations, you are now an elite weapons craftsman.\n\r", ch);
                ch->subclass = SUBCLASS_ADVBLAST;
		return;
	}
	else
	{
		send_to_char("You don't have enough levels in the requested subclass' class.\n\rPlease attain level 100 in the appropriate ability, and try again.\n\r",ch);
		return;
	}
    }

    if ( !str_cmp( argument, "tailor" ) )
    {
	if ( ch->skill_level[ENGINEERING_ABILITY] >= 100 )
	{
		send_to_char("Congratulations, you are now an elite tailor.\n\r", ch);
                ch->subclass = SUBCLASS_TAILOR;
		return;
	}
	else
	{
		send_to_char("You don't have enough levels in the requested subclass' class.\n\rPlease attain level 100 in the appropriate ability, and try again.\n\r",ch);
		return;
	}
    }

    if ( !str_cmp( argument, "quickworker" ) || !str_cmp(argument, "quick worker") || !str_cmp(argument, "quick_worker") )
    {
	if ( ch->skill_level[ENGINEERING_ABILITY] >= 100 )
	{
		send_to_char("Congratulations, you are now a quick worker.\n\r", ch);
		ch->subclass = SUBCLASS_QUICKWORK;
		return;
	}
	else
	{
		send_to_char("You don't have enough levels in the requested subclass' class.\n\rPlease attain level 100 in the appropriate ability, and try again.\n\r",ch);
		return;
	}
    }

    if ( !str_cmp( argument, "thief" ) )
    {
	if ( ch->skill_level[SMUGGLING_ABILITY] >= 100 )
	{
		send_to_char("Congratulations, you are now a pilfer\n\r", ch);
		ch->subclass = SUBCLASS_SNEAK;
		return;
	}
	else
	{
		send_to_char("You don't have enough levels in the requested subclass' class.\n\rPlease attain level 100 in the appropriate ability, and try again.\n\r",ch);
		return;
	}
    }

    if ( !str_cmp( argument, "assassin" ) )
    {
	if ( ch->skill_level[HUNTING_ABILITY] >= 100 )
	{
		send_to_char("Congratulations, you are now a professional assassin.\n\r", ch);
		ch->subclass = SUBCLASS_STEALTH_HUNT;
		return;
	}
	else
	{
		send_to_char("You don't have enough levels in the requested subclass' class.\n\rPlease attain level 100 in the appropriate ability, and try again.\n\r",ch);
		return;
	}
    }

    if ( !str_cmp( argument, "officer" ) )
    {
	if ( ch->skill_level[LEADERSHIP_ABILITY] >= 100 )
	{
		send_to_char("Congratulations, you are now an officer.\n\r", ch);
		ch->subclass = SUBCLASS_OFFICER;
		return;
	}
	else
	{
		send_to_char("You don't have enough levels in the requested subclass' class.\n\rPlease attain level 100 in the appropriate ability, and try again.\n\r",ch);
		return;
	}
    }

    if ( !str_cmp( argument, "senator" ) )
    {
	if ( ch->skill_level[DIPLOMACY_ABILITY] >= 100 )
	{
		send_to_char("Congratulations, you are now a senator.\n\r", ch);
		ch->subclass = SUBCLASS_SENATOR;
		return;
	}
	else
	{
		send_to_char("You don't have enough levels in the requested subclass' class.\n\rPlease attain level 100 in the appropriate ability, and try again.\n\r",ch);
		return;
	}
    }
    if ( !str_cmp(argument, "jack of all trades") || !str_cmp(argument, "jack_of_all_trades")
     ||  !str_cmp(argument, "jackofalltrades") )
    {
       if(ch->top_level >= 99)
       {
          send_to_char("Congradulations, you now do a little bit of everything.\n\r", ch);
          ch->subclass = SUBCLASS_JACKOFTRADES;
          return;
       }
       else
       {
          send_to_char("You need at least level 100, in anything to be a jack of all trades.\n\r", ch);
          return;
       }
    }
    if ( !str_cmp(argument, "medic") )
    {
        if( ch->skill_level[MEDICAL_ABILITY] >= 100 )
        {
           send_to_char("Congradulations, you are now an effective medic.\n\r", ch);
           ch->subclass = SUBCLASS_MEDIC;
           return;
        }
        else
        {
          send_to_char("You need at least level 100 in medical abilities.\n\r", ch);
          return;
        }
    }

    send_to_char("&WPerhaps you should choose a valid subclass?&w\n\r", ch);
    ch_printf(ch, "General Subclasses are as follows.\n\r");
    ch_printf(ch, "   %s\n\r", subclasses[SUBCLASS_JACKOFTRADES]);
    ch_printf(ch, "Combat Subclasses are as follows.\n\r");
    ch_printf(ch, "   %s, %s, %s, %s\n\r", subclasses[SUBCLASS_SNIPER], subclasses[SUBCLASS_TANK],
             subclasses[SUBCLASS_MARTIST], subclasses[SUBCLASS_BLADEMASTER]);
    ch_printf(ch, "Piloting Subclasses are as follows.\n\r");
    ch_printf(ch, "   %s\n\r", subclasses[SUBCLASS_WFOCUS]);
    ch_printf(ch, "Engineering Subclasses are as follows.\n\r");
    ch_printf(ch, "   %s, %s, %s\n\r", subclasses[SUBCLASS_ADVBLAST], subclasses[SUBCLASS_ADVARMOR],
             subclasses[SUBCLASS_QUICKWORK]);
    ch_printf(ch, "Bounty Hunting Subclasses are as follows.\n\r");
    ch_printf(ch, "   %s\n\r", subclasses[SUBCLASS_STEALTH_HUNT]);
    ch_printf(ch, "Smuggling Subclasses are as follows.\n\r");
    ch_printf(ch, "   %s\n\r", subclasses[SUBCLASS_SNEAK]);
    ch_printf(ch, "Leadership Subclasses are as follows.\n\r");
    ch_printf(ch, "   %s\n\r", subclasses[SUBCLASS_OFFICER]);
    ch_printf(ch, "Diplomacy Subclasses are as follows.\n\r");
    ch_printf(ch, "   %s\n\r", subclasses[SUBCLASS_SENATOR]);
    ch_printf(ch, "Medical Subclasses are as follows.\n\r");
    ch_printf(ch, "   %s\n\r", subclasses[SUBCLASS_MEDIC]);
    return;
}





