/***************************************************************************

Star Wars Life 3.0a
Force Skills (NOT SPELLS) module

FOr credits see 'help credits' in game

(c) 2001, 2002 FRPG Ltd.
All Rights Reserved

***************************************************************************/

#include <math.h> 
#include <sys/types.h>
#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include "mud.h"

extern int      top_affect;

void do_makelightsaber( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    char buf[MAX_STRING_LENGTH];
    int chance;
    bool checktool, checkdura, checkbatt, 
         checkoven, checkcond, checkcirc, checklens, checkgems, checkmirr;
    OBJ_DATA *obj;
    OBJ_INDEX_DATA *pObjIndex;
    int vnum, level, gems, charge, gemtype;
    AFFECT_DATA *paf;
    AFFECT_DATA *paf2;
    if(!IS_NPC(ch))
    {
       if(IS_SET(ch->pcdata->flags, PCFLAG_BOUND))
       {
          send_to_char( "How do you intend to do that when bound?\n\r", ch);
          return;
       }
    }

    strcpy( arg, argument );    
    
    switch( ch->substate )
    { 
    	default:
    	        if ( arg[0] == '\0' )
                {
                  send_to_char( "&RUsage: Makelightsaber <name>\n\r&w", ch);
                  return;   
                }

    	        checktool = FALSE;
                checkdura = FALSE;
                checkbatt = FALSE;
                checkoven = FALSE;
                checkcond = FALSE;
                checkcirc = FALSE;
                checklens = FALSE;
                checkgems = FALSE;
                checkmirr = FALSE;

                if (/* !IS_SET( ch->in_room->room_flags, ROOM_SAFE ) ||*/ !IS_SET( ch->in_room->room_flags, ROOM_SILENCE ))
                {
                   send_to_char( "&RYou need to be in a quiet peaceful place to craft a lightsaber.\n\r", ch);
                   return;
                }
                
                for ( obj = ch->last_carrying; obj; obj = obj->prev_content )     
                {
                  if (obj->item_type == ITEM_TOOLKIT)
                    checktool = TRUE;
                  if (obj->item_type == ITEM_LENS)
                    checklens = TRUE;
                  if (obj->item_type == ITEM_CRYSTAL)
                    checkgems = TRUE;                    
                  if (obj->item_type == ITEM_MIRROR)
                    checkmirr = TRUE;
                  if (obj->item_type == ITEM_DURAPLAST || obj->item_type == ITEM_DURASTEEL )
          	    checkdura = TRUE;
                  if (obj->item_type == ITEM_BATTERY)
                    checkbatt = TRUE;
                  if (obj->item_type == ITEM_OVEN)
                    checkoven = TRUE;
                  if (obj->item_type == ITEM_CIRCUIT)
                    checkcirc = TRUE;
                  if (obj->item_type == ITEM_SUPERCONDUCTOR)
                    checkcond = TRUE;                  
                }
                
                if ( !checktool )
                {
                   send_to_char( "&RYou need toolkit to make a lightsaber.\n\r", ch);
                   return;
                }
 
                if ( !checkdura )
                {
                   send_to_char( "&RYou need something to make it out of.\n\r", ch);
                   return;
                }

                if ( !checkbatt )
                {
                   send_to_char( "&RYou need a power source for your lightsaber.\n\r", ch);
                   return;
                }
                
                if ( !checkoven )
                {
                   send_to_char( "&RYou need a small furnace to heat and shape the components.\n\r", ch);
                   return;
                }
                
                if ( !checkcirc )
                {
                   send_to_char( "&RYou need a small circuit board.\n\r", ch);
                   return;
                }
                
                if ( !checkcond )
                {
                   send_to_char( "&RYou still need a small superconductor for your lightsaber.\n\r", ch);
                   return;
                }
                
                if ( !checklens )
                {
                   send_to_char( "&RYou still need a lens to focus the beam.\n\r", ch);
                   return;
                }
                
                if ( !checkgems )
                {
                   send_to_char( "&RLightsabers require 1 to 3 gems to work properly.\n\r", ch);
                   return;
                }
                
                if ( !checkmirr )
                {
                   send_to_char( "&RYou need a high intesity reflective cup to create a lightsaber.\n\r", ch);
                   return;
                }
 
    	        chance = IS_NPC(ch) ? ch->top_level
	                 : (int) (ch->pcdata->learned[gsn_lightsaber_crafting]);
                if ( number_percent( ) < chance )
    		{
    		   send_to_char( "&GYou begin the long process of crafting a lightsaber.\n\r", ch);
    		   act( AT_PLAIN, "$n takes $s tools and a small oven and begins to work on something.", ch,
		        NULL, argument , TO_ROOM );
		   add_timer ( ch , TIMER_DO_FUN , 25 , do_makelightsaber , 1 );
    		   ch->dest_buf = str_dup(arg);
    		   return;
	        }
	        send_to_char("&RYou can't figure out how to fit the parts together.\n\r",ch);
    	   	return;	
    	
    	case 1: 
    		if ( !ch->dest_buf )
    		     return;
    		strcpy(arg, ch->dest_buf);
    		DISPOSE( ch->dest_buf);
    		break;
    		
    	case SUB_TIMER_DO_ABORT:
    		DISPOSE( ch->dest_buf );
    		ch->substate = SUB_NONE;    		                                   
    	        send_to_char("&RYou are interupted and fail to finish your work.\n\r", ch);
    	        return;
    }
    
    ch->substate = SUB_NONE;
    
    level = IS_NPC(ch) ? ch->top_level : (int) (ch->pcdata->learned[gsn_lightsaber_crafting]);
    vnum = 10421;
    
    if ( ( pObjIndex = get_obj_index( vnum ) ) == NULL )
    {
         send_to_char( "&RThe item you are trying to create is missing from the database.\n\rPlease inform the administration of this error.\n\r", ch );
         return;
    }

    checktool = FALSE;
    checkdura = FALSE;
    checkbatt = FALSE;
    checkoven = FALSE;
    checkcond = FALSE;
    checkcirc = FALSE;
    checklens = FALSE;
    checkgems = FALSE;
    checkmirr = FALSE;
    gems = 0;
    charge = 0;
    gemtype =0;

    for ( obj = ch->last_carrying; obj; obj = obj->prev_content )     
    {
       if (obj->item_type == ITEM_TOOLKIT)
          checktool = TRUE;
       if (obj->item_type == ITEM_OVEN)
          checkoven = TRUE;
       if ( (obj->item_type == ITEM_DURAPLAST || obj->item_type == ITEM_DURASTEEL) && checkdura == FALSE)
       {
          checkdura = TRUE;
          separate_obj( obj );
          obj_from_char( obj );
          extract_obj( obj );
       }
       if (obj->item_type == ITEM_DURASTEEL && checkdura == FALSE)
       {
          checkdura = TRUE;
          separate_obj( obj );
          obj_from_char( obj );
          extract_obj( obj );
       }
       if (obj->item_type == ITEM_BATTERY && checkbatt == FALSE)
       {
          charge = UMIN(obj->value[1], 10);
          separate_obj( obj );
          obj_from_char( obj );
          extract_obj( obj );
          checkbatt = TRUE;
       }
       if (obj->item_type == ITEM_SUPERCONDUCTOR && checkcond == FALSE)
       {
          separate_obj( obj );
          obj_from_char( obj );
          extract_obj( obj );
          checkcond = TRUE;
       }
       if (obj->item_type == ITEM_CIRCUIT && checkcirc == FALSE)
       {
          separate_obj( obj );
          obj_from_char( obj );
          extract_obj( obj );
          checkcirc = TRUE;
       }
       if (obj->item_type == ITEM_LENS && checklens == FALSE)
       {
          separate_obj( obj );
          obj_from_char( obj );
          extract_obj( obj );
          checklens = TRUE;
       }
       if (obj->item_type == ITEM_MIRROR && checkmirr == FALSE)
       {
          separate_obj( obj );
          obj_from_char( obj );
          extract_obj( obj );
          checkmirr = TRUE;
       }
       if (obj->item_type == ITEM_CRYSTAL && gems < 3)
       {
          gems++;
          if ( gemtype < obj->value[0] )
             gemtype = obj->value[0];
          separate_obj( obj );
          obj_from_char( obj );
          extract_obj( obj );
          checkgems = TRUE;
       }
    }                            
    
    chance = IS_NPC(ch) ? ch->top_level
                : (int) (ch->pcdata->learned[gsn_lightsaber_crafting]) ;
                
    if ( number_percent( ) > chance*2  || ( !checktool ) || ( !checkdura ) || ( !checkbatt ) || ( !checkoven ) 
                                       || ( !checkmirr ) || ( !checklens ) || ( !checkgems ) || ( !checkcond ) || ( !checkcirc) )
    
    {
       send_to_char( "&RYou hold up your new lightsaber and press the switch hoping for the best.\n\r", ch);
       send_to_char( "&RInstead of a blade of light, smoke starts pouring from the handle.\n\r", ch);
       send_to_char( "&RYou drop the hot handle and watch as it melts on away on the floor.\n\r", ch);
       learn_from_failure( ch, gsn_lightsaber_crafting );
       return;
    }

    obj = create_object( pObjIndex, level );
    
    obj->item_type = ITEM_WEAPON;
    SET_BIT( obj->wear_flags, ITEM_WIELD );
    SET_BIT( obj->wear_flags, ITEM_TAKE );
    obj->level = level;
    obj->weight = 5;
    STRFREE( obj->name );
    strcpy( buf , arg );
    strcat( buf , " lightsaber saber");
    obj->name = STRALLOC( stripclr( buf ) );
    strcpy( buf, arg );
    STRFREE( obj->short_descr );
    obj->short_descr = STRALLOC( buf );        
    STRFREE( obj->description );
    strcat( buf, " was carelessly misplaced here." );
    obj->description = STRALLOC( buf );
    STRFREE( obj->action_desc );
    strcpy( buf, arg );
    strcat( buf, " ignites with a hum and a soft glow." );
    obj->action_desc = STRALLOC( buf );
    CREATE( paf, AFFECT_DATA, 1 );
    paf->type               = -1;
    paf->duration           = -1;
    paf->location           = get_atype( "hitroll" );
    paf->modifier           = URANGE( 0, gems, level/30 );
    paf->bitvector          = 0;
    paf->next               = NULL;
    LINK( paf, obj->first_affect, obj->last_affect, next, prev );
    ++top_affect;
    CREATE( paf2, AFFECT_DATA, 1 );
    paf2->type               = -1;
    paf2->duration           = -1;
    paf2->location           = get_atype( "parry" );
    paf2->modifier           = ( level/3 );
    paf2->bitvector          = 0;
    paf2->next               = NULL;
    LINK( paf2, obj->first_affect, obj->last_affect, next, prev );
    ++top_affect;
    obj->value[0] = INIT_WEAPON_CONDITION;       /* condition  */
    obj->value[1] = (int) (level/10+gemtype*6)+5;      /* min dmg  */
    obj->value[2] = (int) (level/5+gemtype*11)+50;      /* max dmg */
    obj->value[3] = WEAPON_LIGHTSABER;
    obj->value[4] = charge;
    obj->value[5] = charge;
    obj->cost = obj->value[2]*75;
                                                                    
    obj = obj_to_char( obj, ch );
                                                            
    send_to_char( "&GYou finish your work and hold up your newly created lightsaber.&w\n\r", ch);
    act( AT_PLAIN, "$n finishes making $s new lightsaber.", ch,
         NULL, argument , TO_ROOM );
    
    {
         long xpgain;
         
         xpgain = UMIN( obj->cost*50 ,( exp_level(ch->skill_level[FORCE_ABILITY]+1) - exp_level(ch->skill_level[FORCE_ABILITY]) ) );
         gain_exp(ch, xpgain, FORCE_ABILITY);
         ch_printf( ch , "You gain %d force experience.", xpgain );
    }
        learn_from_success( ch, gsn_lightsaber_crafting );
}

