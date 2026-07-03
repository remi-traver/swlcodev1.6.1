/***************************************************************************

Star Wars Life 3.0a
Engineering Skills Module

For credits see 'help credits' in game.

(c) FRPG Ltd; 2000, 2001, 2002
All Rights Reserved

****************************************************************************/

#include <math.h> 
#include <sys/types.h>
#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include "mud.h"

extern int      top_affect;

void do_makeblade( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    char buf[MAX_STRING_LENGTH];
    int level, chance, charge, strengthmin, strengthmax;
    bool checktool, checkdura, checkbatt, checkoven; 
    OBJ_DATA *obj;
    OBJ_INDEX_DATA *pObjIndex;
    int vnum;
    AFFECT_DATA *paf;
    AFFECT_DATA *paf2;
    AFFECT_DATA *paf3;
            
    strcpy( arg , argument );
    if(!IS_NPC(ch))
    {
       if(IS_SET(ch->pcdata->flags, PCFLAG_BOUND))
       {
          send_to_char( "How do you intend to do that when bound?\n\r", ch);
          return;
       }
    }
   
    switch( ch->substate )
    { 
    	default:
    	        
    	        if ( arg[0] == '\0' )
                {
                  send_to_char( "&RUsage: Makeblade <name>\n\r&w", ch);
                  return;   
                }
 
    	        checktool = FALSE;
                checkdura = FALSE;
                checkbatt = FALSE;
                checkoven = FALSE;
        
                if ( !IS_SET( ch->in_room->room_flags, ROOM_FACTORY ) )
                {
                   send_to_char( "&RYou need to be in a factory or workshop to do that.\n\r", ch);
                   return;
                }
                
                for ( obj = ch->last_carrying; obj; obj = obj->prev_content )     
                {
                  if (obj->item_type == ITEM_TOOLKIT)
                    checktool = TRUE;
                  if (obj->item_type == ITEM_DURASTEEL)
          	    checkdura = TRUE;
                  if (obj->item_type == ITEM_BATTERY)
                  checkbatt = TRUE;

                  if (obj->item_type == ITEM_OVEN)
                  checkoven = TRUE;                  
                }
                
                if ( !checktool )
                {
                   send_to_char( "&RYou need toolkit to make a vibro-blade.\n\r", ch);
                   return;
                }
 
                if ( !checkdura )
                {
                   send_to_char( "&RYou need something to make it out of.\n\r", ch);
                   return;
                }

                if ( !checkbatt )
                {
                   send_to_char( "&RYou need a power source for your blade.\n\r", ch);
                   return;
                }
                
                if ( !checkoven )
                {
                   send_to_char( "&RYou need a small furnace to heat the metal.\n\r", ch);
                   return;
                }
 
    	        chance = IS_NPC(ch) ? ch->top_level
	                 : (int) (ch->pcdata->learned[gsn_makeblade]);
                if(ch->subclass == SUBCLASS_WEAPONSMITH)
                    chance += 10;
                if ( number_percent( ) < chance )
    		{
    		   send_to_char( "&GYou begin the long process of crafting a vibroblade.\n\r", ch);
    		   act( AT_PLAIN, "$n takes $s tools and a small oven and begins to work on something.", ch,
		        NULL, argument , TO_ROOM );
		   if(IS_IMMORTAL(ch))
                   add_timer( ch, TIMER_DO_FUN, 1, do_makeblade, 1 );
                   else if (ch->subclass == SUBCLASS_QUICKWORK )
		   add_timer ( ch , TIMER_DO_FUN , 3 , do_makeblade , 1 );
                   else if(ch->subclass == SUBCLASS_WEAPONSMITH)
                   add_timer ( ch, TIMER_DO_FUN,  9, do_makeblade, 1);
		   else
		   add_timer ( ch , TIMER_DO_FUN , 10 , do_makeblade , 1 );
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

    level = IS_NPC(ch) ? ch->skill_level[ENGINEERING_ABILITY] : (int) (ch->pcdata->learned[gsn_makeblade]);
    if(ch->subclass == SUBCLASS_WEAPONSMITH)
    {
       level += 10;
    }
    vnum = 10422;
    
    if ( ( pObjIndex = get_obj_index( vnum ) ) == NULL )
    {
         send_to_char( "&RThe item you are trying to create is missing from the database.\n\rPlease inform the administration of this error.\n\r", ch );
         return;
    }

    checktool = FALSE;
    checkdura = FALSE;
    checkbatt = FALSE;
    checkoven = FALSE;
    
    for ( obj = ch->last_carrying; obj; obj = obj->prev_content )     
    {
       if (obj->item_type == ITEM_TOOLKIT)
          checktool = TRUE;
       if (obj->item_type == ITEM_OVEN)
          checkoven = TRUE;
       if (obj->item_type == ITEM_DURASTEEL && checkdura == FALSE)
       { /* damage determined by durasteel */
          float exdam;
          if(ch->subclass == SUBCLASS_WEAPONSMITH)
             exdam = 1.1;
          else
             exdam = 1.0;
          strengthmin = (int) URANGE((level/10), (obj->value[0] * 3*exdam), (level/5+10));
          strengthmax = (int) URANGE((level/5), (obj->value[1] * 6*exdam), (level/2+10)); 
          if(strengthmin > strengthmax)
             strengthmax = strengthmin;
          checkdura = TRUE;
          separate_obj( obj );
          obj_from_char( obj );
          extract_obj( obj );
       }
       if (obj->item_type == ITEM_BATTERY && checkbatt == FALSE )
       {
          charge = UMAX( 5, obj->value[0] ); 
          separate_obj( obj );
          obj_from_char( obj );
          extract_obj( obj );
          checkbatt = TRUE;
       }
    }                            
    
    chance = IS_NPC(ch) ? ch->top_level
                : (int) (ch->pcdata->learned[gsn_makeblade]) ;
    if(ch->subclass == SUBCLASS_WEAPONSMITH)
        chance += 10;
    if ( number_percent( ) > chance*2  || ( !checktool ) || ( !checkdura ) || ( !checkbatt ) || ( !checkoven ) )
    {
       send_to_char( "&RYou activate your newly created vibroblade.\n\r", ch);
       send_to_char( "&RIt hums softly for a few seconds then begins to shake violently.\n\r", ch);
       send_to_char( "&RIt finally shatters breaking apart into a dozen pieces.\n\r", ch);
       learn_from_failure( ch, gsn_makeblade );
       return;
    }

    obj = create_object( pObjIndex, level );
    
    obj->item_type = ITEM_WEAPON;
    SET_BIT( obj->wear_flags, ITEM_WIELD );
    SET_BIT( obj->wear_flags, ITEM_TAKE );
    obj->level = level;
    obj->weight = 3;
    STRFREE( obj->name );
    strcpy( buf, arg );
    strcat( buf, " vibro-blade blade" );
    obj->name = STRALLOC( stripclr( buf ) );
    strcpy( buf, arg );
    STRFREE( obj->short_descr );
    obj->short_descr = STRALLOC( buf );        
    STRFREE( obj->description );
    strcat( buf, " was left here." );
    obj->description = STRALLOC( buf );
    CREATE( paf, AFFECT_DATA, 1 );
    paf->type               = -1;
    paf->duration           = -1;
    paf->location           = get_atype( "backstab" );
    paf->modifier           = level/3;
    paf->bitvector          = 0;
    paf->next               = NULL;
    LINK( paf, obj->first_affect, obj->last_affect, next, prev );
    ++top_affect;
    CREATE( paf2, AFFECT_DATA, 1 );
    paf2->type               = -1;
    paf2->duration           = -1;
    paf2->location           = get_atype( "hitroll" );
    paf2->modifier           = level/33 + 1;
    paf2->bitvector          = 0;
    paf2->next               = NULL;
    LINK( paf2, obj->first_affect, obj->last_affect, next, prev );
    ++top_affect;
    CREATE( paf3, AFFECT_DATA, 1 );
    paf3->type               = -1;
    paf3->duration           = -1;
    paf3->location           = get_atype( "damroll" );
    paf3->modifier           = level/33;
    paf3->bitvector          = 0;
    paf3->next               = NULL;
    LINK( paf3, obj->first_affect, obj->last_affect, next, prev );
    ++top_affect;
    obj->value[0] = INIT_WEAPON_CONDITION;      
    obj->value[1] = strengthmin;      /* min dmg  */
    obj->value[2] = strengthmax;      /* max dmg */
    obj->value[3] = WEAPON_VIBRO_BLADE;
    obj->value[4] = charge;
    obj->value[5] = charge;
    obj->cost = (obj->value[2]+obj->value[1])*10;                                                                
    obj = obj_to_char( obj, ch );
                                                            
    send_to_char( "&GYou finish your work and hold up your newly created blade.&w\n\r", ch);
    act( AT_PLAIN, "$n finishes crafting a vibro-blade.", ch,
         NULL, argument , TO_ROOM );
    
    {
         long xpgain;
         
         xpgain = UMIN( 30000 ,( exp_level(ch->skill_level[ENGINEERING_ABILITY]+1) - exp_level(ch->skill_level[ENGINEERING_ABILITY]) ) );
         gain_exp(ch, xpgain, ENGINEERING_ABILITY);
         ch_printf( ch , "You gain %d engineering experience.", xpgain );
    }
        
    learn_from_success( ch, gsn_makeblade );
}

void do_makesword( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    char buf[MAX_STRING_LENGTH];
    int level, chance, charge;
    bool checktool, checkdura, checkoven; 
    OBJ_DATA *obj;
    OBJ_INDEX_DATA *pObjIndex;
    int vnum, strengthmin, strengthmax;
    AFFECT_DATA *paf;
    AFFECT_DATA *paf2;
    AFFECT_DATA *paf3;
            
    strcpy( arg , argument );
    if(!IS_NPC(ch))
    {
       if(IS_SET(ch->pcdata->flags, PCFLAG_BOUND))
       {
          send_to_char( "How do you intend to do that when bound?\n\r", ch);
          return;
       }
    }
    
    switch( ch->substate )
    { 
    	default:
    	        
    	        if ( arg[0] == '\0' )
                {
                  send_to_char( "&RUsage: Makesword <name>\n\r&w", ch);
                  return;   
                }
 
       	        checktool = FALSE;
                checkdura = FALSE;
                checkoven = FALSE;
        
                if ( !IS_SET( ch->in_room->room_flags, ROOM_FACTORY ) )
                {
                   send_to_char( "&RYou need to be in a factory or workshop to do that.\n\r", ch);
                   return;
                }
                
                for ( obj = ch->last_carrying; obj; obj = obj->prev_content )     
                {
                  if (obj->item_type == ITEM_TOOLKIT)
                    checktool = TRUE;
                  if (obj->item_type == ITEM_DURASTEEL)
          	    checkdura = TRUE;
                  if (obj->item_type == ITEM_OVEN)
                  checkoven = TRUE;                  
                }
                
                if ( !checktool )
                {
                   send_to_char( "&RYou need toolkit to make a sword.\n\r", ch);
                   return;
                }
 
                if ( !checkdura )
                {
                   send_to_char( "&RYou need something to make it out of.\n\r", ch);
                   return;
                }
               
                if ( !checkoven )
                {
                   send_to_char( "&RYou need a small furnace to heat the metal.\n\r", ch);
                   return;
                }
 
    	        chance = IS_NPC(ch) ? ch->top_level
	                 : (int) (ch->pcdata->learned[gsn_makesword]);
                if(ch->subclass == SUBCLASS_WEAPONSMITH)
                    chance += 10;
                if ( number_percent( ) < chance )
    		{
    		   send_to_char( "&GYou begin the long process of forging a sword.\n\r", ch);
    		   act( AT_PLAIN, "$n takes $s tools and a small oven and begins to work on something.", ch,
		        NULL, argument , TO_ROOM );
		   if(IS_IMMORTAL(ch))
                   add_timer( ch, TIMER_DO_FUN, 1, do_makesword, 1 );
                   else if (ch->subclass == SUBCLASS_QUICKWORK )
		   add_timer ( ch , TIMER_DO_FUN , 5 , do_makesword , 1 );
                   else if(ch->subclass == SUBCLASS_WEAPONSMITH)
		   add_timer ( ch, TIMER_DO_FUN,  18 , do_makesword , 1 );
                   else
		   add_timer ( ch , TIMER_DO_FUN , 20 , do_makesword , 1 );
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

    level = IS_NPC(ch) ? ch->skill_level[ENGINEERING_ABILITY] : (int) (ch->pcdata->learned[gsn_makesword]);
    if(ch->subclass == SUBCLASS_WEAPONSMITH)
    {
      level += 10;
    }

    vnum = 10436;
    
    if ( ( pObjIndex = get_obj_index( vnum ) ) == NULL )
    {
         send_to_char( "&RThe item you are trying to create is missing from the database.\n\rPlease inform the administration of this error.\n\r", ch );
         return;
    }

    checktool = FALSE;
    checkdura = FALSE;
    checkoven = FALSE;
    
    for ( obj = ch->last_carrying; obj; obj = obj->prev_content )     
    {
       if (obj->item_type == ITEM_TOOLKIT)
          checktool = TRUE;
       if (obj->item_type == ITEM_OVEN)
          checkoven = TRUE;
       if (obj->item_type == ITEM_DURASTEEL && checkdura == FALSE)
       {/* Little code segment to make durasteel values effect damage. */
          float exdam;
          if(ch->subclass == SUBCLASS_WEAPONSMITH)
             exdam = 1.1;
          else
             exdam = 1.0;
          strengthmin = (int) URANGE(((level/2)*1.5), (exdam *(obj->value[0] * 10 + 20)*1.5), ((level+20))*1.5);
          strengthmax = (int) URANGE(((level/2+10)*1.5), (exdam *(obj->value[1] * 15 + 30)*1.5), ((level+80)*1.5));
          if(strengthmin > strengthmax)
             strengthmax = strengthmin;
          checkdura = TRUE;
          separate_obj( obj );
          obj_from_char( obj );
          extract_obj( obj );
       }
    }                            
    
    chance = IS_NPC(ch) ? ch->top_level
                : (int) (ch->pcdata->learned[gsn_makesword]) ;
    if(ch->subclass == SUBCLASS_WEAPONSMITH)
       chance += 10;                
    if ( number_percent( ) > chance*2  || ( !checktool ) || ( !checkdura ) || ( !checkoven ) )
    {
       send_to_char( "&RYou Draw your newly forged sword.\n\r", ch);
       send_to_char( "&RAs you hold it to the light you can see inperfections in the edge.\n\r", ch);
       send_to_char( "&RWith an angry shout, you break it over your knee\n\r", ch);
       learn_from_failure( ch, gsn_makesword );
       return;
    }

    obj = create_object( pObjIndex, level );
    
    obj->item_type = ITEM_WEAPON;
    SET_BIT( obj->wear_flags, ITEM_WIELD );
    SET_BIT( obj->wear_flags, ITEM_TAKE );
    obj->level = level;
    obj->weight = 3;
    STRFREE( obj->name );
    strcpy( buf, arg );
    strcat( buf, " sword" );
    obj->name = STRALLOC( stripclr( buf ) );
    strcpy( buf, arg );
    STRFREE( obj->short_descr );
    obj->short_descr = STRALLOC( buf );        
    STRFREE( obj->description );
    strcat( buf, " was left here." );
    obj->description = STRALLOC( buf );
    CREATE( paf, AFFECT_DATA, 1 );
    paf->type               = -1;
    paf->duration           = -1;
    paf->location           = get_atype( "parry" );
    paf->modifier           = level/10;
    paf->bitvector          = 0;
    paf->next               = NULL;
    LINK( paf, obj->first_affect, obj->last_affect, next, prev );
    ++top_affect;
    CREATE( paf2, AFFECT_DATA, 1 );
    paf2->type               = -1;
    paf2->duration           = -1;
    paf2->location           = get_atype( "hitroll" );
    paf2->modifier           = level/10;
    paf2->bitvector          = 0;
    paf2->next               = NULL;
    LINK( paf2, obj->first_affect, obj->last_affect, next, prev );
    ++top_affect;
    CREATE( paf3, AFFECT_DATA, 1 );
    paf3->type               = -1;
    paf3->duration           = -1;
    paf3->location           = get_atype( "damroll" );
    paf3->modifier           = level/20;
    paf3->bitvector          = 0;
    paf3->next               = NULL;
    LINK( paf3, obj->first_affect, obj->last_affect, next, prev );
    ++top_affect;
    obj->value[0] = INIT_WEAPON_CONDITION * 2;      
    obj->value[1] = strengthmin;      /* min dmg  */
    obj->value[2] = strengthmax;      /* max dmg  */
    obj->value[3] = WEAPON_SWORD;
    obj->value[4] = charge;
    obj->value[5] = charge;
    obj->cost = obj->value[2]*7;
                                                                    
    obj = obj_to_char( obj, ch );
                                                            
    send_to_char( "&GYou finish your work and hold up your newly created sword.&w\n\r", ch);
    act( AT_PLAIN, "$n finishes forging a sword.", ch,
         NULL, argument , TO_ROOM );
    
    {
         long xpgain;
         
         xpgain = UMIN( 40000 ,( exp_level(ch->skill_level[ENGINEERING_ABILITY]+1) - exp_level(ch->skill_level[ENGINEERING_ABILITY]) ) );
         gain_exp(ch, xpgain, ENGINEERING_ABILITY);
         ch_printf( ch , "You gain %d engineering experience.", xpgain );
    }
        
    learn_from_success( ch, gsn_makesword );
}

void do_makeblaster( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    char buf[MAX_STRING_LENGTH];
    int level, chance;
    bool checktool, checkdura, checkbatt, checkoven, checkcond, checkcirc, checkammo;
    OBJ_DATA *obj;
    OBJ_INDEX_DATA *pObjIndex;
    int vnum, power, scope, ammo;
    AFFECT_DATA *paf;
    AFFECT_DATA *paf2;
    
    strcpy( arg , argument );
    if(!IS_NPC(ch))
    {
       if(IS_SET(ch->pcdata->flags, PCFLAG_BOUND))
       {
          send_to_char( "How do you intend to do that when bound?\n\r", ch);
          return;
       }
    }
    
    switch( ch->substate )
    { 
    	default:
    	        if ( arg[0] == '\0' )
                {
                  send_to_char( "&RUsage: Makeblaster <name>\n\r&w", ch);
                  return;   
                }

    	        checktool = FALSE;
                checkdura = FALSE;
                checkbatt = FALSE;
                checkoven = FALSE;
                checkcond = FALSE;
                checkcirc = FALSE;

                if ( !IS_SET( ch->in_room->room_flags, ROOM_FACTORY ) )
                {
                   send_to_char( "&RYou need to be in a factory or workshop to do that.\n\r", ch);
                   return;
                }
                
                for ( obj = ch->last_carrying; obj; obj = obj->prev_content )     
                {
                  if (obj->item_type == ITEM_TOOLKIT)
                    checktool = TRUE;
                  if (obj->item_type == ITEM_DURAPLAST)
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
                   send_to_char( "&RYou need toolkit to make a blaster.\n\r", ch);
                   return;
                }
 
                if ( !checkdura )
                {
                   send_to_char( "&RYou need something to make it out of.\n\r", ch);
                   return;
                }

                if ( !checkbatt )
                {
                   send_to_char( "&RYou need a power source for your blaster.\n\r", ch);
                   return;
                }
                
                if ( !checkoven )
                {
                   send_to_char( "&RYou need a small furnace to heat the plastics.\n\r", ch);
                   return;
                }
                
                if ( !checkcirc )
                {
                   send_to_char( "&RYou need a small circuit board to control the firing mechanism.\n\r", ch);
                   return;
                }
                
                if ( !checkcond )
                {
                   send_to_char( "&RYou still need a small superconductor.\n\r", ch);
                   return;
                }
    	        chance = IS_NPC(ch) ? ch->top_level
	                 : (int) (ch->pcdata->learned[gsn_makeblaster]); 
                if(ch->subclass == SUBCLASS_WEAPONSMITH)
                    chance += 10;
                if ( number_percent( ) < chance )
    		{
    		   send_to_char( "&GYou begin the long process of making a blaster.\n\r", ch);
    		   act( AT_PLAIN, "$n takes $s tools and a small oven and begins to work on something.", ch,
		        NULL, argument , TO_ROOM );
		   if(IS_IMMORTAL(ch))
                   add_timer( ch, TIMER_DO_FUN, 1 , do_makeblaster, 1 );
                   else if (ch->subclass == SUBCLASS_QUICKWORK )
		   add_timer ( ch , TIMER_DO_FUN , 3 , do_makeblaster , 1 );
                   else if(ch->subclass == SUBCLASS_WEAPONSMITH)
                   add_timer ( ch,  TIMER_DO_FUN,  9 , do_makeblaster , 1);
		   else
		   add_timer ( ch , TIMER_DO_FUN , 10 , do_makeblaster , 1 );
    		   ch->dest_buf   = str_dup(arg);
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
    
    level = IS_NPC(ch) ? ch->top_level : (int) (ch->pcdata->learned[gsn_makeblaster]);
     
    if(ch->subclass == SUBCLASS_WEAPONSMITH)
    {
       level += 10;
    }
    vnum = 10420;
    
    if ( ( pObjIndex = get_obj_index( vnum ) ) == NULL )
    {
         send_to_char( "&RThe item you are trying to create is missing from the database.\n\rPlease inform the administration of this error.\n\r", ch );
         return;
    }

    checkammo= FALSE;
    checktool = FALSE;
    checkdura = FALSE;
    checkbatt = FALSE;
    checkoven = FALSE;
    checkcond = FALSE;
    checkcirc = FALSE;
    power     = 0;
    scope     = 0;
    ammo = 0;
    
    for ( obj = ch->last_carrying; obj; obj = obj->prev_content )     
    {
       if (obj->item_type == ITEM_TOOLKIT)
          checktool = TRUE;
       if (obj->item_type == ITEM_OVEN)
          checkoven = TRUE;
       if (obj->item_type == ITEM_DURAPLAST && checkdura == FALSE)
       {
          checkdura = TRUE;
          separate_obj( obj );
          obj_from_char( obj );
          extract_obj( obj );
       }
       if (obj->item_type == ITEM_AMMO && checkammo == FALSE)
       {
          ammo = obj->value[0];
          checkammo = TRUE;
          separate_obj( obj );
          obj_from_char( obj );
          extract_obj( obj );
       }
       if (obj->item_type == ITEM_BATTERY && checkbatt == FALSE)
       {
          separate_obj( obj );
          obj_from_char( obj );
          extract_obj( obj );
          checkbatt = TRUE;
       }
       if (obj->item_type == ITEM_LENS && scope == 0)
       {
          separate_obj( obj );
          obj_from_char( obj );
          extract_obj( obj );
          scope++;
       }
       if (obj->item_type == ITEM_SUPERCONDUCTOR && power<2)
       {
          power++;
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
    }                            
    
    chance = IS_NPC(ch) ? ch->top_level
                : (int) (ch->pcdata->learned[gsn_makeblaster]) ;
    if(ch->subclass == SUBCLASS_WEAPONSMITH)
        chance += 10;              
    if ( number_percent( ) > chance*2  || ( !checktool ) || ( !checkdura ) || ( !checkbatt ) || ( !checkoven )  || ( !checkcond ) || ( !checkcirc) )
    {
       send_to_char( "&RYou hold up your new blaster and aim at a leftover piece of plastic.\n\r", ch);
       send_to_char( "&RYou slowly squeeze the trigger hoping for the best...\n\r", ch);
       send_to_char( "&RYour blaster backfires destroying your weapon and burning your hand.\n\r", ch);
       learn_from_failure( ch, gsn_makeblaster );
       return;
    }

    obj = create_object( pObjIndex, level );
    
    obj->item_type = ITEM_WEAPON;
    SET_BIT( obj->wear_flags, ITEM_WIELD );
    SET_BIT( obj->wear_flags, ITEM_TAKE );
    obj->level = level;
    obj->weight = 2+level/10;
    STRFREE( obj->name );
    strcpy( buf , arg );
    strcat( buf , " blaster");
    obj->name = STRALLOC( stripclr( buf ) );
    strcpy( buf, arg );
    STRFREE( obj->short_descr );
    obj->short_descr = STRALLOC( buf );        
    STRFREE( obj->description );
    strcat( buf, " was carelessly misplaced here." );
    obj->description = STRALLOC( buf );
    CREATE( paf, AFFECT_DATA, 1 );
    paf->type               = -1;
    paf->duration           = -1;
    paf->location           = get_atype( "hitroll" );
    paf->modifier           = (level / 20);
    if(scope == TRUE)
      paf->modifier        += 5;
    paf->bitvector          = 0;
    paf->next               = NULL;
    LINK( paf, obj->first_affect, obj->last_affect, next, prev );
    ++top_affect;
    CREATE( paf2, AFFECT_DATA, 1 );
    paf2->type               = -1;
    paf2->duration           = -1;
    paf2->location           = get_atype( "damroll" );
    paf2->modifier = (level/20);
    if(scope == TRUE)
      paf2->modifier        += 5;
    paf2->bitvector          = 0;
    paf2->next               = NULL;
    LINK( paf2, obj->first_affect, obj->last_affect, next, prev );
    ++top_affect;
    obj->value[0] = INIT_WEAPON_CONDITION;       /* condition  */
    obj->value[1] = (int) ((level));      /* min dmg  */
    obj->value[2] = (int) ((level*=4.5));      /* max dmg  */
    obj->value[3] = WEAPON_BLASTER;
    obj->value[4] = ammo;
    obj->value[5] = 2000;
    obj->cost = obj->value[2]*50;
                                                                   
    obj = obj_to_char( obj, ch );
                                                            
    send_to_char( "&GYou finish your work and hold up your newly created blaster.&w\n\r", ch);
    act( AT_PLAIN, "$n finishes making $s new blaster.", ch,
         NULL, argument , TO_ROOM );
    
    {
         long xpgain;
         
         xpgain = UMIN( 50000 ,( exp_level(ch->skill_level[ENGINEERING_ABILITY]+1) - exp_level(ch->skill_level[ENGINEERING_ABILITY]) ) );
         gain_exp(ch, xpgain, ENGINEERING_ABILITY);
         ch_printf( ch , "You gain %d engineering experience.", xpgain );
    }
    learn_from_success( ch, gsn_makeblaster );
}

         

void do_makespice( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    char buf[MAX_STRING_LENGTH];
    int chance;
    OBJ_DATA *obj;
    if(!IS_NPC(ch))
    {
       if(IS_SET(ch->pcdata->flags, PCFLAG_BOUND))
       {
          send_to_char( "How do you intend to do that when bound?\n\r", ch);
          return;
       }
    }
        
    switch( ch->substate )
    { 
    	default:
    	        strcpy( arg, argument );
    	        
    	        if ( arg[0] == '\0' )
                {
                  send_to_char( "&RFrom what?\n\r&w", ch);
                  return;   
                }
    	        
    	        if ( !IS_SET( ch->in_room->room_flags, ROOM_REFINERY ) )
                {
                   send_to_char( "&RYou need to be in a refinery to create drugs from spice.\n\r", ch);
                   return;
                }
                
                if ( ms_find_obj(ch) )
                      return;
                
                if ( ( obj = get_obj_carry( ch, arg ) ) == NULL )
                {
                        send_to_char( "&RYou do not have that item.\n\r&w", ch );
                        return;
                }                                                            
                
                if ( obj->item_type != ITEM_RAWSPICE )
                {
                       send_to_char( "&RYou can't make a drug out of that\n\r&w",ch);
                       return;
                }
                
    	        chance = IS_NPC(ch) ? ch->top_level
	                 : (int) (ch->pcdata->learned[gsn_spice_refining]);
                if ( number_percent( ) < chance )
    		{
    		   send_to_char( "&GYou begin the long process of refining spice into a drug.\n\r", ch);
    		   act( AT_PLAIN, "$n begins working on something.", ch,
		        NULL, argument , TO_ROOM );
                   if(IS_IMMORTAL(ch))
                   add_timer( ch, TIMER_DO_FUN, 1 , do_makespice, 1);
 		   else if (ch->subclass == SUBCLASS_QUICKWORK)
		   add_timer ( ch , TIMER_DO_FUN , 3 , do_makespice , 1 );
		   else
		   add_timer ( ch , TIMER_DO_FUN , 10 , do_makespice , 1 );
    		   ch->dest_buf = str_dup(arg);
    		   return;
	        }
	        send_to_char("&RYou can't figure out what to do with the stuff.\n\r",ch);
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
    	        send_to_char("&RYou are distracted and are unable to finish your work.\n\r&w", ch);
    	        return;
    }
    
    ch->substate = SUB_NONE;
    
    if ( ( obj = get_obj_carry( ch, arg ) ) == NULL )
    {
           send_to_char( "You seem to have lost your spice!\n\r", ch );
           return;
    }                                                            
    if ( obj->item_type != ITEM_RAWSPICE )
    {
          send_to_char( "&RYou get your tools mixed up and can't finish your work.\n\r&w",ch);
          return;
    }
    
    obj->value[1] = URANGE (10, obj->value[1], ( IS_NPC(ch) ? ch->top_level
	                 : (int) (ch->pcdata->learned[gsn_spice_refining]) ) +10);
    strcpy( buf, obj->name );
    STRFREE( obj->name );
    strcat( buf, " drug spice" );
    obj->name = STRALLOC( stripclr( buf ) );
    strcpy( buf, "a drug made from " );
    strcat( buf, obj->short_descr );   
    STRFREE( obj->short_descr );
    obj->short_descr = STRALLOC( buf ); 
    strcat( buf, " was foolishly left lying around here." );
    STRFREE( obj->description );
    obj->description = STRALLOC( buf ); 
    obj->item_type = ITEM_SPICE;
    
    send_to_char( "&GYou finish your work.\n\r", ch);
    act( AT_PLAIN, "$n finishes $s work.", ch,
         NULL, argument , TO_ROOM );
    
    obj->cost += obj->value[1]*10;
    {
         long xpgain;
         
         xpgain = UMIN( 25000 ,( exp_level(ch->skill_level[MEDICAL_ABILITY]+1) - exp_level(ch->skill_level[MEDICAL_ABILITY]) ) );
         gain_exp(ch, xpgain, MEDICAL_ABILITY);
         ch_printf( ch , "You gain %d medical experience.", xpgain );
    }
                 
    learn_from_success( ch, gsn_spice_refining );
    	
}

void do_makegrenade( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    char buf[MAX_STRING_LENGTH];
    int level, chance, strength, weight;
    bool checktool, checkdrink, checkbatt, checkchem, checkcirc;
    OBJ_DATA *obj;
    OBJ_INDEX_DATA *pObjIndex;
    int vnum;
    
    strcpy( arg , argument );
    if(!IS_NPC(ch))
    {
       if(IS_SET(ch->pcdata->flags, PCFLAG_BOUND))
       {
          send_to_char( "How do you intend to do that when bound?\n\r", ch);
          return;
       }
    }
    
    switch( ch->substate )
    { 
    	default:
    	        if ( arg[0] == '\0' )
                {
                  send_to_char( "&RUsage: Makegrenade <name>\n\r&w", ch);
                  return;   
                }

    	        checktool  = FALSE;
                checkdrink = FALSE;
                checkbatt  = FALSE;
                checkchem  = FALSE;
                checkcirc  = FALSE;

                if ( !IS_SET( ch->in_room->room_flags, ROOM_FACTORY ) )
                {
                   send_to_char( "&RYou need to be in a factory or workshop to do that.\n\r", ch);
                   return;
                }
                
                for ( obj = ch->last_carrying; obj; obj = obj->prev_content )     
                {
                  if (obj->item_type == ITEM_TOOLKIT)
                    checktool = TRUE;
                  if (obj->item_type == ITEM_DRINK_CON && obj->value[1] == 0 )
          	    checkdrink = TRUE;
                  if (obj->item_type == ITEM_BATTERY)
                    checkbatt = TRUE;
                  if (obj->item_type == ITEM_CIRCUIT)
                    checkcirc = TRUE;
                  if (obj->item_type == ITEM_CHEMICAL)
                    checkchem = TRUE;                  
                }
                
                if ( !checktool )
                {
                   send_to_char( "&RYou need toolkit to make a grenade.\n\r", ch);
                   return;
                }
 
                if ( !checkdrink )
                {
                   send_to_char( "&RYou will need an empty drink container to mix and hold the chemicals.\n\r", ch);
                   return;
                }

                if ( !checkbatt )
                {
                   send_to_char( "&RYou need a small battery for the timer.\n\r", ch);
                   return;
                }
                
                if ( !checkcirc )
                {
                   send_to_char( "&RYou need a small circuit for the timer.\n\r", ch);
                   return;
                }
                
                if ( !checkchem )
                {
                   send_to_char( "&RSome explosive chemicals would come in handy!\n\r", ch);
                   return;
                }
 
    	        chance = IS_NPC(ch) ? ch->top_level
	                 : (int) (ch->pcdata->learned[gsn_makegrenade]);
                if(ch->subclass == SUBCLASS_WEAPONSMITH)
                    chance += 10;
                if ( number_percent( ) < chance )
    		{
    		   send_to_char( "&GYou begin the long process of making a grenade.\n\r", ch);
    		   act( AT_PLAIN, "$n takes $s tools and a drink container and begins to work on something.", ch,
		        NULL, argument , TO_ROOM );
	           if(IS_IMMORTAL(ch))
                   add_timer(ch, TIMER_DO_FUN, 1, do_makegrenade, 1);
               	   else if (ch->subclass == SUBCLASS_QUICKWORK )
		   add_timer ( ch , TIMER_DO_FUN , 7 , do_makegrenade , 1 );
                   else if(ch->subclass == SUBCLASS_WEAPONSMITH)
                   add_timer ( ch, TIMER_DO_FUN , 22 , do_makegrenade , 1 );
     		   else
		   add_timer ( ch , TIMER_DO_FUN , 25 , do_makegrenade , 1 );
    		   ch->dest_buf   = str_dup(arg);
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
    
    level = IS_NPC(ch) ? ch->skill_level[ENGINEERING_ABILITY] : (int) (ch->pcdata->learned[gsn_makegrenade]);
    if(ch->subclass == SUBCLASS_WEAPONSMITH)
    {
       level += 10;
    }
    vnum = 10425;
    
    if ( ( pObjIndex = get_obj_index( vnum ) ) == NULL )
    {
         send_to_char( "&RThe item you are trying to create is missing from the database.\n\rPlease inform the administration of this error.\n\r", ch );
         return;
    }

    checktool = FALSE;
    checkdrink = FALSE;
    checkbatt = FALSE;
    checkchem = FALSE;
    checkcirc = FALSE;
    
    for ( obj = ch->last_carrying; obj; obj = obj->prev_content )     
    {
       if (obj->item_type == ITEM_TOOLKIT)
          checktool = TRUE;
       if (obj->item_type == ITEM_DRINK_CON && checkdrink == FALSE && obj->value[1] == 0 )
       {
          checkdrink = TRUE;
          separate_obj( obj );
          obj_from_char( obj );
          extract_obj( obj );
       }
       if (obj->item_type == ITEM_BATTERY && checkbatt == FALSE)
       {
          separate_obj( obj );
          obj_from_char( obj );
          extract_obj( obj );
          checkbatt = TRUE;
       }
       if (obj->item_type == ITEM_CHEMICAL)
       {
          float exdam;
          if(ch->subclass == SUBCLASS_WEAPONSMITH)
             exdam = 1.1;
          else
             exdam = 1.0;
          strength = URANGE( 10, obj->value[0]*exdam, level *20 );
          weight = obj->weight;
          separate_obj( obj );
          obj_from_char( obj );
          extract_obj( obj );
          checkchem = TRUE;
       }
       if (obj->item_type == ITEM_CIRCUIT && checkcirc == FALSE)
       {
          separate_obj( obj );
          obj_from_char( obj );
          extract_obj( obj );
          checkcirc = TRUE;
       }
    }                            
    
    chance = IS_NPC(ch) ? ch->top_level
                : (int) (ch->pcdata->learned[gsn_makegrenade]) ;
    if(ch->subclass == SUBCLASS_WEAPONSMITH)
        chance += 10;         
    if ( number_percent( ) > chance*2  || ( !checktool ) || ( !checkdrink ) || ( !checkbatt ) || ( !checkchem ) || ( !checkcirc) )
    {
       send_to_char( "&RJust as you are about to finish your work,\n\ryour newly created grenade explodes in your hands...doh!\n\r", ch);
       learn_from_failure( ch, gsn_makegrenade );
       return;
    }

    obj = create_object( pObjIndex, level );
    
    obj->item_type = ITEM_GRENADE;
    SET_BIT( obj->wear_flags, ITEM_HOLD );
    SET_BIT( obj->wear_flags, ITEM_TAKE );
    obj->level = level;
    obj->weight = weight;
    STRFREE( obj->name );
    strcpy( buf , arg );
    strcat( buf , " grenade");
    obj->name = STRALLOC( stripclr( buf ) );
    strcpy( buf, arg );
    STRFREE( obj->short_descr );
    obj->short_descr = STRALLOC( buf );        
    STRFREE( obj->description );
    strcat( buf, " is slowly ticking away here....." );
    obj->description = STRALLOC( buf );
    obj->value[0] = strength/2;
    obj->value[1] = strength;
    obj->cost = obj->value[1]*5;
                                                                   
    obj = obj_to_char( obj, ch );
                                                            
    send_to_char( "&GYou finish your work and hold up your newly created grenade.&w\n\r", ch);
    act( AT_PLAIN, "$n finishes making $s new grenade.", ch,
         NULL, argument , TO_ROOM );
    
    {
         long xpgain;
         
         xpgain = UMIN( 35000 ,( exp_level(ch->skill_level[ENGINEERING_ABILITY]+1) - exp_level(ch->skill_level[ENGINEERING_ABILITY]) ) );
         gain_exp(ch, xpgain, ENGINEERING_ABILITY);
         ch_printf( ch , "You gain %d engineering experience.", xpgain );
    }
        learn_from_success( ch, gsn_makegrenade );
}

void do_makelandmine( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    char buf[MAX_STRING_LENGTH];
    int level, chance, strength, weight;
    bool checktool, checkdrink, checkbatt, checkchem, checkcirc;
    OBJ_DATA *obj;
    OBJ_INDEX_DATA *pObjIndex;
    int vnum;
    
    strcpy( arg , argument );
    if(!IS_NPC(ch))
    {
       if(IS_SET(ch->pcdata->flags, PCFLAG_BOUND))
       {
          send_to_char( "How do you intend to do that when bound?\n\r", ch);
          return;
       }
    }
    
    switch( ch->substate )
    { 
    	default:
    	        if ( arg[0] == '\0' )
                {
                  send_to_char( "&RUsage: Makelandmine <name>\n\r&w", ch);
                  return;   
                }

    	        checktool  = FALSE;
                checkdrink = FALSE;
                checkbatt  = FALSE;
                checkchem  = FALSE;
                checkcirc  = FALSE;

                if ( !IS_SET( ch->in_room->room_flags, ROOM_FACTORY ) )
                {
                   send_to_char( "&RYou need to be in a factory or workshop to do that.\n\r", ch);
                   return;
                }
                
                for ( obj = ch->last_carrying; obj; obj = obj->prev_content )     
                {
                  if (obj->item_type == ITEM_TOOLKIT)
                    checktool = TRUE;
                  if (obj->item_type == ITEM_DRINK_CON && obj->value[1] == 0 )
          	    checkdrink = TRUE;
                  if (obj->item_type == ITEM_BATTERY)
                    checkbatt = TRUE;
                  if (obj->item_type == ITEM_CIRCUIT)
                    checkcirc = TRUE;
                  if (obj->item_type == ITEM_CHEMICAL)
                    checkchem = TRUE;                  
                }
                
                if ( !checktool )
                {
                   send_to_char( "&RYou need toolkit to make a landmine.\n\r", ch);
                   return;
                }
 
                if ( !checkdrink )
                {
                   send_to_char( "&RYou will need an empty drink container to mix and hold the chemicals.\n\r", ch);
                   return;
                }

                if ( !checkbatt )
                {
                   send_to_char( "&RYou need a small battery for the detonator.\n\r", ch);
                   return;
                }
                
                if ( !checkcirc )
                {
                   send_to_char( "&RYou need a small circuit for the detonator.\n\r", ch);
                   return;
                }
                
                if ( !checkchem )
                {
                   send_to_char( "&RSome explosive chemicals would come in handy!\n\r", ch);
                   return;
                }
 
    	        chance = IS_NPC(ch) ? ch->top_level
	                 : (int) (ch->pcdata->learned[gsn_makelandmine]);
                if(ch->subclass == SUBCLASS_WEAPONSMITH)
                    chance += 10;
                if ( number_percent( ) < chance )
    		{
    		   send_to_char( "&GYou begin the long process of making a landmine.\n\r", ch);
    		   act( AT_PLAIN, "$n takes $s tools and a drink container and begins to work on something.", ch,
		        NULL, argument , TO_ROOM );
		   if(IS_IMMORTAL(ch))
                   add_timer (ch, TIMER_DO_FUN, 1, do_makelandmine, 1);
                   else if (ch->subclass == SUBCLASS_QUICKWORK )
		   add_timer ( ch , TIMER_DO_FUN , 7 , do_makelandmine , 1 );
                   else if(ch->subclass == SUBCLASS_WEAPONSMITH)
                   add_timer ( ch, TIMER_DO_FUN  , 25, do_makelandmine , 1 );
		   else
		   add_timer ( ch , TIMER_DO_FUN , 28 , do_makelandmine , 1 );
    		   ch->dest_buf   = str_dup(arg);
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
    
    level = IS_NPC(ch) ? ch->skill_level[ENGINEERING_ABILITY] : (int) (ch->pcdata->learned[gsn_makelandmine]);   
    if(ch->subclass == SUBCLASS_WEAPONSMITH) 
    { 
      level += 10;
    }
    vnum = 10427;
    
    if ( ( pObjIndex = get_obj_index( vnum ) ) == NULL )
    {
         send_to_char( "&RThe item you are trying to create is missing from the database.\n\rPlease inform the administration of this error.\n\r", ch );
         return;
    }

    checktool = FALSE;
    checkdrink = FALSE;
    checkbatt = FALSE;
    checkchem = FALSE;
    checkcirc = FALSE;
    
    for ( obj = ch->last_carrying; obj; obj = obj->prev_content )     
    {
       if (obj->item_type == ITEM_TOOLKIT)
          checktool = TRUE;
       if (obj->item_type == ITEM_DRINK_CON && checkdrink == FALSE && obj->value[1] == 0 )
       {
          checkdrink = TRUE;
          separate_obj( obj );
          obj_from_char( obj );
          extract_obj( obj );
       }
       if (obj->item_type == ITEM_BATTERY && checkbatt == FALSE)
       {
          separate_obj( obj );
          obj_from_char( obj );
          extract_obj( obj );
          checkbatt = TRUE;
       }
       if (obj->item_type == ITEM_CHEMICAL)
       {
          float exdam;
          if(ch->subclass == SUBCLASS_WEAPONSMITH)
             exdam = 1.1;
          else
             exdam = 1.0;
          strength = URANGE( 10, obj->value[0]*exdam, level * 20 );
          weight = obj->weight;
          separate_obj( obj );
          obj_from_char( obj );
          extract_obj( obj );
          checkchem = TRUE;
       }
       if (obj->item_type == ITEM_CIRCUIT && checkcirc == FALSE)
       {
          separate_obj( obj );
          obj_from_char( obj );
          extract_obj( obj );
          checkcirc = TRUE;
       }
    }                            
    
    chance = IS_NPC(ch) ? ch->top_level
                : (int) (ch->pcdata->learned[gsn_makelandmine]) ;
    if(ch->subclass == SUBCLASS_WEAPONSMITH)
       chance += 10;            
    if ( number_percent( ) > chance*2  || ( !checktool ) || ( !checkdrink ) || ( !checkbatt ) || ( !checkchem ) || ( !checkcirc) )
    {
       send_to_char( "&RJust as you are about to finish your work,\n\ryour newly created landmine explodes in your hands...doh!\n\r", ch);
       learn_from_failure( ch, gsn_makelandmine );
       return;
    }

    obj = create_object( pObjIndex, level );
    
    obj->item_type = ITEM_LANDMINE;
    SET_BIT( obj->wear_flags, ITEM_HOLD );
    SET_BIT( obj->wear_flags, ITEM_TAKE );
    obj->level = level;
    obj->weight = weight;
    STRFREE( obj->name );
    strcpy( buf , arg );
    strcat( buf , " landmine");
    obj->name = STRALLOC( stripclr( buf ) );
    strcpy( buf, arg );
    STRFREE( obj->short_descr );
    obj->short_descr = STRALLOC( buf );        
    STRFREE( obj->description );
    strcat( buf, " was carelessly misplaced here." );
    obj->description = STRALLOC( buf );
    obj->value[0] = strength * 2;
    obj->value[1] = strength;
    obj->cost = obj->value[1]*5;
                                                                   
    obj = obj_to_char( obj, ch );
                                                            
    send_to_char( "&GYou finish your work and hold up your newly created landmine.&w\n\r", ch);
    act( AT_PLAIN, "$n finishes making $s new landmine.", ch,
         NULL, argument , TO_ROOM );
    
    {
         long xpgain;
         
         xpgain = UMIN( 35000 ,( exp_level(ch->skill_level[ENGINEERING_ABILITY]+1) - exp_level(ch->skill_level[ENGINEERING_ABILITY]) ) );
         gain_exp(ch, xpgain, ENGINEERING_ABILITY);
         ch_printf( ch , "You gain %d engineering experience.", xpgain );
    }
        learn_from_success( ch, gsn_makelandmine );
}
void do_makelight( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    char buf[MAX_STRING_LENGTH];
    int level, chance, strength;
    bool checktool, checkbatt, checkchem, checkcirc, checklens;
    OBJ_DATA *obj;
    OBJ_INDEX_DATA *pObjIndex;
    int vnum;
    strcpy( arg , argument );
    if(!IS_NPC(ch))
    {
       if(IS_SET(ch->pcdata->flags, PCFLAG_BOUND))
       {
          send_to_char( "How do you intend to do that when bound?\n\r", ch);
          return;
       }
    }
    
    switch( ch->substate )
    { 
    	default:
    	        if ( arg[0] == '\0' )
                {
                  send_to_char( "&RUsage: Makeflashlight <name>\n\r&w", ch);
                  return;   
                }

    	        checktool  = FALSE;
                checkbatt  = FALSE;
                checkchem  = FALSE;
                checkcirc  = FALSE;
                checklens = FALSE;

                if ( !IS_SET( ch->in_room->room_flags, ROOM_FACTORY ) )
                {
                   send_to_char( "&RYou need to be in a factory or workshop to do that.\n\r", ch);
                   return;
                }
                
                for ( obj = ch->last_carrying; obj; obj = obj->prev_content )     
                {
                  if (obj->item_type == ITEM_TOOLKIT)
                    checktool = TRUE;
                  if (obj->item_type == ITEM_BATTERY)
                    checkbatt = TRUE;
                  if (obj->item_type == ITEM_CIRCUIT)
                    checkcirc = TRUE;
                  if (obj->item_type == ITEM_CHEMICAL)
                    checkchem = TRUE; 
                  if (obj->item_type == ITEM_LENS)
                    checklens = TRUE;                  
                }
                
                if ( !checktool )
                {
                   send_to_char( "&RYou need toolkit to make a light.\n\r", ch);
                   return;
                }
                
                if ( !checklens )
                {
                   send_to_char( "&RYou need a lens to make a light.\n\r", ch);
                   return;
                }
 
                if ( !checkbatt )
                {
                   send_to_char( "&RYou need a battery for the light to work.\n\r", ch);
                   return;
                }
                
                if ( !checkcirc )
                {
                   send_to_char( "&RYou need a small circuit.\n\r", ch);
                   return;
                }
                
                if ( !checkchem )
                {
                   send_to_char( "&RSome chemicals to light would come in handy!\n\r", ch);
                   return;
                }
 
    	        chance = IS_NPC(ch) ? ch->top_level
	                 : (int) (ch->pcdata->learned[gsn_makelight]);
                if ( number_percent( ) < chance )
    		{
    		   send_to_char( "&GYou begin the long process of making a light.\n\r", ch);
    		   act( AT_PLAIN, "$n takes $s tools and begins to work on something.", ch,
		        NULL, argument , TO_ROOM );
                   if(IS_IMMORTAL(ch))
                   add_timer(ch, TIMER_DO_FUN, 1, do_makelight, 1);
		   else if (ch->subclass == SUBCLASS_QUICKWORK )
		   add_timer ( ch , TIMER_DO_FUN , 3 , do_makelight , 1 );
		    else
		   add_timer ( ch , TIMER_DO_FUN , 10 , do_makelight , 1 );
    		   ch->dest_buf   = str_dup(arg);
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
    
    level = IS_NPC(ch) ? ch->skill_level[ENGINEERING_ABILITY] : (int) 
(ch->pcdata->learned[gsn_makelight]);
    vnum = 10428;
    
    if ( ( pObjIndex = get_obj_index( vnum ) ) == NULL )
    {
         send_to_char( "&RThe item you are trying to create is missing from the database.\n\rPlease inform the administration of this error.\n\r", ch );
         return;
    }

    checktool = FALSE;
    checklens = FALSE;
    checkbatt = FALSE;
    checkchem = FALSE;
    checkcirc = FALSE;
    
    for ( obj = ch->last_carrying; obj; obj = obj->prev_content )     
    {
       if (obj->item_type == ITEM_TOOLKIT)
          checktool = TRUE;
       if (obj->item_type == ITEM_BATTERY && checkbatt == FALSE)
       {
          strength = obj->value[0];
          separate_obj( obj );
          obj_from_char( obj );
          extract_obj( obj );
          checkbatt = TRUE;
       }
       if (obj->item_type == ITEM_CHEMICAL)
       {
          separate_obj( obj );
          obj_from_char( obj );
          extract_obj( obj );
          checkchem = TRUE;
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
    }                            
    
    chance = IS_NPC(ch) ? ch->top_level
                : (int) (ch->pcdata->learned[gsn_makelight]) ;
                
    if ( number_percent( ) > chance*2  || ( !checktool ) || ( !checklens ) || ( !checkbatt ) || ( !checkchem ) || ( !checkcirc) )
    {
       send_to_char( "&RJust as you are about to finish your work,\n\ryour newly created light explodes in your hands...doh!\n\r", ch);
       learn_from_failure( ch, gsn_makelight );
       return;
    }

    obj = create_object( pObjIndex, level );
    
    obj->item_type = ITEM_LIGHT;
    SET_BIT( obj->wear_flags, ITEM_TAKE );
    obj->level = level;
    obj->weight = 3;
    STRFREE( obj->name );
    strcpy( buf , arg );
    strcat( buf , " light");
    obj->name = STRALLOC( stripclr( buf ) );
    strcpy( buf, arg );
    STRFREE( obj->short_descr );
    obj->short_descr = STRALLOC( buf );        
    STRFREE( obj->description );
    strcat( buf, " was carelessly misplaced here." );
    obj->description = STRALLOC( buf );
    obj->value[2] = strength;
    obj->cost = obj->value[2];
                                                                   
    obj = obj_to_char( obj, ch );
                                                            
    send_to_char( "&GYou finish your work and hold up your newly created light.&w\n\r", ch);
    act( AT_PLAIN, "$n finishes making $s new light.", ch,
         NULL, argument , TO_ROOM );
    
    {
         long xpgain;
         
         xpgain = UMIN( 5000 ,( exp_level(ch->skill_level[ENGINEERING_ABILITY]+1) - exp_level(ch->skill_level[ENGINEERING_ABILITY]) ) );
         gain_exp(ch, xpgain, ENGINEERING_ABILITY);
         ch_printf( ch , "You gain %d engineering experience.", xpgain );
    }
        learn_from_success( ch, gsn_makelight );
}

void do_makejewelry( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    char arg2[MAX_INPUT_LENGTH];
    char buf[MAX_STRING_LENGTH];
    int level, chance;
    bool checktool, checkoven, checkmetal; 
    OBJ_DATA *obj;
    OBJ_DATA *metal;
    int value, cost;
            
    argument = one_argument( argument, arg );
    strcpy ( arg2, argument);
    if(!IS_NPC(ch))
    {
       if(IS_SET(ch->pcdata->flags, PCFLAG_BOUND))
       {
          send_to_char( "How do you intend to do that when bound?\n\r", ch);
          return;
       }
    }
    
    if ( !str_cmp( arg, "body" )
    || !str_cmp( arg, "head" )
    || !str_cmp( arg, "legs" )
    || !str_cmp( arg, "arms" )
    || !str_cmp( arg, "about" )
    || !str_cmp( arg, "waist" )
    || !str_cmp( arg, "hold" )
    || !str_cmp( arg, "feet" )
    || !str_cmp( arg, "hands" ) )
    {
        send_to_char( "&RYou cannot make jewelry for that body part.\n\r&w", ch);
        send_to_char( "&RTry MAKEARMOR.\n\r&w", ch);
        return;
    }
    if ( !str_cmp( arg, "shield" ) )
    {
        send_to_char( "&RYou cannot make jewelry worn as a shield.\n\r&w", ch);
        send_to_char( "&RTry MAKESHIELD.\n\r&w", ch);
        return;
    }
    if ( !str_cmp( arg, "wield" ) )
    {
        send_to_char( "&RAre you going to fight with your jewelry?\n\r&w", ch);
        send_to_char( "&RTry MAKEBLADE...\n\r&w", ch);
        return;
    }
    
    switch( ch->substate )
    { 
    	default:
    	        
    	        if ( arg2[0] == '\0' )
                {
                  send_to_char( "&RUsage: Makejewelry <wearloc> <name>\n\r&w", ch);
                  return;   
                }
 
    	        checktool = FALSE;
                checkoven = FALSE;
                checkmetal = FALSE;
        
                if ( !IS_SET( ch->in_room->room_flags, ROOM_FACTORY ) )
                {
                   send_to_char( "&RYou need to be in a factory or workshop to do that.\n\r", ch);
                   return;
                }
                
                for ( obj = ch->last_carrying; obj; obj = obj->prev_content )     
                {
                  if (obj->item_type == ITEM_TOOLKIT)
                    checktool = TRUE;
                  if (obj->item_type == ITEM_OVEN)
          	    checkoven = TRUE;
                  if (obj->item_type == ITEM_RARE_METAL)
          	    checkmetal = TRUE;
                }                
                if ( !checktool )
                {
                   send_to_char( "&RYou need a toolkit.\n\r", ch);
                   return;
                }
 
                if ( !checkoven )
                {
                   send_to_char( "&RYou need an oven.\n\r", ch);
                   return;
                }
                
                if ( !checkmetal )
                {
                   send_to_char( "&RYou need some precious metal.\n\r", ch);
                   return;
                }

    	        chance = IS_NPC(ch) ? ch->top_level
	                 : (int) (ch->pcdata->learned[gsn_makejewelry]);
                if(ch->subclass == SUBCLASS_TAILOR)
                    chance += 10;
                if ( number_percent( ) < chance )
    		{
    		   send_to_char( "&GYou begin the long process of creating some jewelry.\n\r", ch);
    		   act( AT_PLAIN, "$n takes $s toolkit and some metal and begins to work.", ch,
		        NULL, argument , TO_ROOM );
		   if(IS_IMMORTAL(ch))
                   add_timer(ch, TIMER_DO_FUN, 1, do_makejewelry, 1);
                   else if (ch->subclass == SUBCLASS_QUICKWORK )
		   add_timer ( ch , TIMER_DO_FUN , 3 , do_makejewelry , 1 );
                   else if(ch->subclass == SUBCLASS_TAILOR)
                   add_timer ( ch, TIMER_DO_FUN , 9  , do_makejewelry , 1 );
		   else
		   add_timer ( ch , TIMER_DO_FUN , 10 , do_makejewelry , 1 );
    		   ch->dest_buf = str_dup(arg);
    		   ch->dest_buf_2 = str_dup(arg2);
    		   return;
	        }
	        send_to_char("&RYou can't figure out what to do.\n\r",ch);
    	   	return;	
    	
    	case 1: 
    		if ( !ch->dest_buf )
    		     return;
    		if ( !ch->dest_buf_2 )
    		     return;
    		strcpy(arg, ch->dest_buf);
    		DISPOSE( ch->dest_buf);
    		strcpy(arg2, ch->dest_buf_2);
    		DISPOSE( ch->dest_buf_2);
    		break;
    		
    	case SUB_TIMER_DO_ABORT:
    		DISPOSE( ch->dest_buf );
    		DISPOSE( ch->dest_buf_2 );
    		ch->substate = SUB_NONE;    		                                   
    	        send_to_char("&RYou are interupted and fail to finish your work.\n\r", ch);
    	        return;
    }
    
    ch->substate = SUB_NONE;
    
    level = IS_NPC(ch) ? ch->top_level : (int) (ch->pcdata->learned[gsn_makejewelry]);
    
    checkmetal = FALSE;
    checkoven = FALSE;
    checktool = FALSE;
    value=0;
    cost=0;
    
    for ( obj = ch->last_carrying; obj; obj = obj->prev_content )     
    {
       if (obj->item_type == ITEM_TOOLKIT)
          checktool = TRUE;
       if (obj->item_type == ITEM_OVEN)
          checkoven = TRUE;
       if (obj->item_type == ITEM_RARE_METAL && checkmetal == FALSE)
       {
          checkmetal = TRUE;
          separate_obj( obj );
          obj_from_char( obj );
          metal = obj;
       }
       if (obj->item_type == ITEM_CRYSTAL)
       {
          cost += obj->cost;
          separate_obj( obj );
          obj_from_char( obj );
          extract_obj( obj );
       }
    }                            
    
    chance = IS_NPC(ch) ? ch->top_level
                : (int) (ch->pcdata->learned[gsn_makejewelry]) ;
    if(ch->subclass == SUBCLASS_TAILOR)
        chance += 10;                
    if ( number_percent( ) > chance*2  || ( !checkoven ) || ( !checktool ) || ( !checkmetal ) )
    {
       send_to_char( "&RYou hold up your newly created jewelry.\n\r", ch);
       send_to_char( "&RIt suddenly dawns upon you that you have created the most useless\n\r", ch);
       send_to_char( "&Rpiece of junk you've ever seen. You quickly hide your mistake...\n\r", ch);
       learn_from_failure( ch, gsn_makejewelry );
       return;
    }

    obj = metal; 

    obj->item_type = ITEM_ARMOR;
    SET_BIT( obj->wear_flags, ITEM_TAKE );
    value = get_wflag( arg );
    if ( value < 0 || value > 31 )
        SET_BIT( obj->wear_flags, ITEM_WEAR_NECK );                    
    else
        SET_BIT( obj->wear_flags, 1 << value );
    obj->level = level;
    STRFREE( obj->name );
    strcpy( buf, arg2 );
    obj->name = STRALLOC( stripclr( buf ) );
    strcpy( buf, arg2 );
    STRFREE( obj->short_descr );
    obj->short_descr = STRALLOC( buf );        
    STRFREE( obj->description );
    strcat( buf, " was dropped here." );
    obj->description = STRALLOC( buf );
    if(ch->subclass == SUBCLASS_TAILOR)
    {
       obj->value[1] *= 1.1;
    }
    obj->value[0] = obj->value[1];      
    obj->cost *= 10;
    obj->cost += cost;
                                                                    
    obj = obj_to_char( obj, ch );
                                                            
    send_to_char( "&GYou finish your work and hold up your newly created jewelry.&w\n\r", ch);
    act( AT_PLAIN, "$n finishes sewing some new jewelry.", ch,
         NULL, argument , TO_ROOM );
    
    {
         long xpgain;
         
         xpgain = UMIN( 40000 ,( exp_level(ch->skill_level[ENGINEERING_ABILITY]+1) - exp_level(ch->skill_level[ENGINEERING_ABILITY]) ) );
         gain_exp(ch, xpgain, ENGINEERING_ABILITY);
         ch_printf( ch , "You gain %d engineering experience.", xpgain );
    }
        learn_from_success( ch, gsn_makejewelry );
     
}

void do_makearmor( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    char arg2[MAX_INPUT_LENGTH];
    char buf[MAX_STRING_LENGTH];
    int level, chance;
    bool checksew, checkfab; 
    OBJ_DATA *obj;
    OBJ_DATA *material;
    int value;
            
    argument = one_argument( argument, arg );
    strcpy ( arg2, argument);
    if(!IS_NPC(ch))
    {
       if(IS_SET(ch->pcdata->flags, PCFLAG_BOUND))
       {
          send_to_char( "How do you intend to do that when bound?\n\r", ch);
          return;
       }
    }
    
    if ( !str_cmp( arg, "eyes" )
    || !str_cmp( arg, "ears" )
    || !str_cmp( arg, "finger" )
    || !str_cmp( arg, "neck" )
    || !str_cmp( arg, "wrist" )
    || !str_cmp( arg, "trinket" ) )
    {
        send_to_char( "&RYou cannot make clothing for that body part.\n\r&w", ch);
        send_to_char( "&RTry MAKEJEWELRY.\n\r&w", ch);
        return;
    }
    if ( !str_cmp( arg, "shield" ) )
    {
        send_to_char( "&RYou cannot make clothing worn as a shield.\n\r&w", ch);
        send_to_char( "&RTry MAKESHIELD.\n\r&w", ch);
        return;
    }
    if ( !str_cmp( arg, "wield" ) )
    {
        send_to_char( "&RAre you going to fight with your clothing?\n\r&w", ch);
        send_to_char( "&RTry MAKEBLADE...\n\r&w", ch);
        return;
    }
    
    switch( ch->substate )
    { 
    	default:
    	        
    	        if ( arg2[0] == '\0' )
                {
                  send_to_char( "&RUsage: Makearmor <wearloc> <name>\n\r&w", ch);
                  return;   
                }
 
    	        checksew = FALSE;
                checkfab = FALSE;
        
                if ( !IS_SET( ch->in_room->room_flags, ROOM_FACTORY ) )
                {
                   send_to_char( "&RYou need to be in a factory or workshop to do that.\n\r", ch);
                   return;
                }
                
                for ( obj = ch->last_carrying; obj; obj = obj->prev_content )     
                {
                  if (obj->item_type == ITEM_FABRIC)
                    checkfab = TRUE;
                  if (obj->item_type == ITEM_THREAD)
          	    checksew = TRUE;
                }
                
                if ( !checkfab )
                {
                   send_to_char( "&RYou need some sort of fabric or material.\n\r", ch);
                   return;
                }
 
                if ( !checksew )
                {
                   send_to_char( "&RYou need a needle and some thread.\n\r", ch);
                   return;
                }
    	        chance = IS_NPC(ch) ? ch->top_level
	                 : (int) (ch->pcdata->learned[gsn_makearmor]);
                if(ch->subclass == SUBCLASS_TAILOR)
                  chance += 10;
                if ( number_percent( ) < chance )
    		{
    		   send_to_char( "&GYou begin the long process of creating some armor.\n\r", ch);
    		   act( AT_PLAIN, "$n takes $s sewing kit and some material and begins to work.", ch,
		        NULL, argument , TO_ROOM );
                   if(IS_IMMORTAL(ch))
                   add_timer( ch, TIMER_DO_FUN,  1, do_makearmor, 1);
		   else if (ch->subclass == SUBCLASS_QUICKWORK )
		   add_timer ( ch , TIMER_DO_FUN , 3 , do_makearmor , 1 );
                   else if(ch->subclass == SUBCLASS_TAILOR)
                   add_timer ( ch , TIMER_DO_FUN , 9,  do_makearmor , 1 ); 
		   else
		   add_timer ( ch , TIMER_DO_FUN , 10 , do_makearmor , 1 );
    		   ch->dest_buf = str_dup(arg);
    		   ch->dest_buf_2 = str_dup(arg2);
    		   return;
	        }
	        send_to_char("&RYou can't figure out what to do.\n\r",ch);
    	   	return;	
    	case 1: 
    		if ( !ch->dest_buf )
    		     return;
    		if ( !ch->dest_buf_2 )
    		     return;
    		strcpy(arg, ch->dest_buf);
    		DISPOSE( ch->dest_buf);
    		strcpy(arg2, ch->dest_buf_2);
    		DISPOSE( ch->dest_buf_2);
    		break;
    		
    	case SUB_TIMER_DO_ABORT:
    		DISPOSE( ch->dest_buf );
    		DISPOSE( ch->dest_buf_2 );
    		ch->substate = SUB_NONE;    		                                   
    	        send_to_char("&RYou are interupted and fail to finish your work.\n\r", ch);
    	        return;
    }
    
    ch->substate = SUB_NONE;
    
    level = IS_NPC(ch) ? ch->top_level : (int) (ch->pcdata->learned[gsn_makearmor]);
    
    checksew = FALSE;
    checkfab = FALSE;
    
    for ( obj = ch->last_carrying; obj; obj = obj->prev_content )     
    {
       if (obj->item_type == ITEM_THREAD)
          checksew = TRUE;
       if (obj->item_type == ITEM_FABRIC && checkfab == FALSE)
       {
          checkfab = TRUE;
          separate_obj( obj );
          obj_from_char( obj );
          material = obj;
       }
    }                              
    chance = IS_NPC(ch) ? ch->top_level
                : (int) (ch->pcdata->learned[gsn_makearmor]) ;
    if(ch->subclass == SUBCLASS_TAILOR)
       chance += 10;                
    if ( number_percent( ) > chance*2  || ( !checkfab ) || ( !checksew ) )
    {
       send_to_char( "&RYou hold up your newly created armor.\n\r", ch);
       send_to_char( "&RIt suddenly dawns upon you that you have created the most useless\n\r", ch);
       send_to_char( "&Rgarment you've ever seen. You quickly hide your mistake...\n\r", ch);
       learn_from_failure( ch, gsn_makearmor );
       return;
    }

    obj = material; 

    obj->item_type = ITEM_ARMOR;
    SET_BIT( obj->wear_flags, ITEM_TAKE );
    value = get_wflag( arg );
    if ( value < 0 || value > 31 )
        SET_BIT( obj->wear_flags, ITEM_WEAR_BODY );                    
    else
        SET_BIT( obj->wear_flags, 1 << value );
    obj->level = level;
    STRFREE( obj->name );
    strcpy( buf, arg2 );
    obj->name = STRALLOC( stripclr( buf ) );
    strcpy( buf, arg2 );
    STRFREE( obj->short_descr );
    obj->short_descr = STRALLOC( buf );        
    STRFREE( obj->description );
    strcat( buf, " was dropped here." );
    obj->description = STRALLOC( buf );
    if (ch->subclass == SUBCLASS_TAILOR)
	obj->value[1] *= 1.1;
    obj->value[0] = obj->value[1];      
    obj->cost *= 10;
                                                                    
    obj = obj_to_char( obj, ch );
                                                            
    send_to_char( "&GYou finish your work and hold up your newly created garment.&w\n\r", ch);
    act( AT_PLAIN, "$n finishes sewing some new armor.", ch,
         NULL, argument , TO_ROOM );
    
    {
         long xpgain;
         
         xpgain = UMIN( 40000 ,( exp_level(ch->skill_level[ENGINEERING_ABILITY]+1) - exp_level(ch->skill_level[ENGINEERING_ABILITY]) ) );
         gain_exp(ch, xpgain, ENGINEERING_ABILITY);
         ch_printf( ch , "You gain %d engineering experience.", xpgain );
    }
        learn_from_success( ch, gsn_makearmor );
}


void do_makecomlink( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    char arg2[MAX_INPUT_LENGTH];
    char buf[MAX_STRING_LENGTH];
    int chance;
    bool checktool, checkgem, checkbatt, checkcirc; 
    OBJ_DATA *obj;
    OBJ_INDEX_DATA *pObjIndex;
    int vnum, value;
    argument = one_argument(argument, arg);         
    strcpy( arg2 , argument );
    if(!IS_NPC(ch))
    {
       if(IS_SET(ch->pcdata->flags, PCFLAG_BOUND))
       {
          send_to_char( "How do you intend to do that when bound?\n\r", ch);
          return;
       }
    }
        if ( !str_cmp( arg, "body" )
    || !str_cmp( arg, "head" )
    || !str_cmp( arg, "legs" )
    || !str_cmp( arg, "arms" )
    || !str_cmp( arg, "about" )
    || !str_cmp( arg, "waist" )
    || !str_cmp( arg, "hold" )
    || !str_cmp( arg, "feet" )
    || !str_cmp( arg, "hands" ) 
    || !str_cmp( arg, "shield" )
    || !str_cmp( arg, "finger" )
    || !str_cmp( arg, "light" )
    || !str_cmp( arg, "wield" ))
    {
        send_to_char( "&RYou cannot make comlinks for that body part.\n\r&w", ch);
        return;
    }

    switch( ch->substate )
    { 
    	default:
    	        
    	        if ( arg2[0] == '\0' )
                {
                  send_to_char( "&RUsage: Makecomlink <wearloc> <name>\n\r&w", ch);
                  return;   
                }
 
    	        checktool = FALSE;
                checkgem  = FALSE;
                checkbatt = FALSE;
                checkcirc = FALSE;
        
                if ( !IS_SET( ch->in_room->room_flags, ROOM_FACTORY ) )
                {
                   send_to_char( "&RYou need to be in a factory or workshop to do that.\n\r", ch);
                   return;
                }
                
                for ( obj = ch->last_carrying; obj; obj = obj->prev_content )     
                {
                  if (obj->item_type == ITEM_TOOLKIT)
                    checktool = TRUE;
                  if (obj->item_type == ITEM_CRYSTAL)
          	    checkgem = TRUE;
                  if (obj->item_type == ITEM_BATTERY)
                  checkbatt = TRUE;
                  if (obj->item_type == ITEM_CIRCUIT)
                  checkcirc = TRUE;                  
                }
                
                if ( !checktool )
                {
                   send_to_char( "&RYou need toolkit to make a comlink.\n\r", ch);
                   return;
                }
 
                if ( !checkgem )
                {
                   send_to_char( "&RYou need a small crystal.\n\r", ch);
                   return;
                }

                if ( !checkbatt )
                {
                   send_to_char( "&RYou need a power source for your comlink.\n\r", ch);
                   return;
                }
                
                if ( !checkcirc )
                {
                   send_to_char( "&RYou need a small circuit.\n\r", ch);
                   return;
                }
 
    	        chance = IS_NPC(ch) ? ch->top_level
	                 : (int) (ch->pcdata->learned[gsn_makecomlink]);
                if ( number_percent( ) < chance )
    		{
    		   send_to_char( "&GYou begin the long process of making a comlink.\n\r", ch);
    		   act( AT_PLAIN, "$n takes $s tools and begins to work on something.", ch,
		        NULL, argument , TO_ROOM );
                   if(IS_IMMORTAL(ch))
                   add_timer( ch, TIMER_DO_FUN, 1, do_makecomlink, 1);
		   else if (ch->subclass == SUBCLASS_QUICKWORK )
		   add_timer ( ch , TIMER_DO_FUN , 3 , do_makecomlink , 1 );
		      else
		   add_timer ( ch , TIMER_DO_FUN , 10 , do_makecomlink , 1 );
    		   ch->dest_buf = str_dup(arg);
                   ch->dest_buf_2 = str_dup(arg2);
    		   return;
	        }
	        send_to_char("&RYou can't figure out how to fit the parts together.\n\r",ch);
    	   	return;	
    	
    	case 1: 
    		if ( !ch->dest_buf )
    		     return;
                if ( !ch->dest_buf_2 )
                     return;
    		strcpy(arg, ch->dest_buf);
    		DISPOSE( ch->dest_buf);
                strcpy(arg2, ch->dest_buf_2);
                DISPOSE( ch->dest_buf_2);
    		break;
    		
    	case SUB_TIMER_DO_ABORT:
    		DISPOSE( ch->dest_buf );
                DISPOSE( ch->dest_buf_2 );
    		ch->substate = SUB_NONE;    		                                   
    	        send_to_char("&RYou are interupted and fail to finish your work.\n\r", ch);
    	        return;
    }
    
    ch->substate = SUB_NONE;

    vnum = 10430;
    
    if ( ( pObjIndex = get_obj_index( vnum ) ) == NULL )
    {
         send_to_char( "&RThe item you are trying to create is missing from the database.\n\rPlease inform the administration of this error.\n\r", ch );
         return;
    }

    checktool = FALSE;
    checkgem  = FALSE;
    checkbatt = FALSE;
    checkcirc = FALSE;
    
    for ( obj = ch->last_carrying; obj; obj = obj->prev_content )     
    {
       if (obj->item_type == ITEM_TOOLKIT)
          checktool = TRUE;
       if (obj->item_type == ITEM_CRYSTAL && checkgem == FALSE)
       {
          checkgem = TRUE;
          separate_obj( obj );
          obj_from_char( obj );
          extract_obj( obj );
       }
       if (obj->item_type == ITEM_CIRCUIT && checkcirc == FALSE)
       {
          checkcirc = TRUE;
          separate_obj( obj );
          obj_from_char( obj );
          extract_obj( obj );
       }
       if (obj->item_type == ITEM_BATTERY && checkbatt == FALSE )
       {
          separate_obj( obj );
          obj_from_char( obj );
          extract_obj( obj );
          checkbatt = TRUE;
       }
    }                            
    
    chance = IS_NPC(ch) ? ch->top_level
                : (int) (ch->pcdata->learned[gsn_makecomlink]) ;
                
    if ( number_percent( ) > chance*2  || ( !checktool ) || ( !checkcirc ) || ( !checkbatt ) || ( !checkgem ) )
    {
       send_to_char( "&RYou hold up your newly created comlink....\n\r", ch);
       send_to_char( "&Rand it falls apart in your hands.\n\r", ch);
       learn_from_failure( ch, gsn_makecomlink );
       return;
    }

    obj = create_object( pObjIndex, ch->top_level );
    
    obj->item_type = ITEM_COMLINK;
    SET_BIT( obj->wear_flags, ITEM_TAKE );
    value = get_wflag( arg );
    if ( value < 0 || value > 31 )
        SET_BIT( obj->wear_flags, ITEM_HOLD );
    else
        SET_BIT( obj->wear_flags, 1 << value );
    obj->weight = 3;
    STRFREE( obj->name );
    strcpy( buf, arg2 );
    strcat( buf, " comlink" );
    obj->name = STRALLOC( stripclr( buf ) );
    strcpy( buf, arg2 );
    STRFREE( obj->short_descr );
    obj->short_descr = STRALLOC( buf );        
    STRFREE( obj->description );
    strcat( buf, " was left here." );
    obj->description = STRALLOC( buf );
    obj->cost = 50;
                                                                    
    obj = obj_to_char( obj, ch );
                                                            
    send_to_char( "&GYou finish your work and hold up your newly created comlink.&w\n\r", ch);
    act( AT_PLAIN, "$n finishes crafting a comlink.", ch,
         NULL, argument , TO_ROOM );
    
    {
         long xpgain;
         
         xpgain = UMIN( 10000 ,( exp_level(ch->skill_level[ENGINEERING_ABILITY]+1) - exp_level(ch->skill_level[ENGINEERING_ABILITY]) ) );
         gain_exp(ch, xpgain, ENGINEERING_ABILITY);
         ch_printf( ch , "You gain %d engineering experience.", xpgain );
    }
        learn_from_success( ch, gsn_makecomlink );
     
}

void do_makeshield( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    char buf[MAX_STRING_LENGTH];
    AFFECT_DATA *paf;
    int chance;
    bool checktool, checkbatt, checkcond, checkcirc, checkgems;
    OBJ_DATA *obj;
    OBJ_INDEX_DATA *pObjIndex;
    int vnum, level, charge, gemtype, cost;
    
    strcpy( arg, argument );    
    if(!IS_NPC(ch))
    {
       if(IS_SET(ch->pcdata->flags, PCFLAG_BOUND))
       {
          send_to_char( "How do you intend to do that when bound?\n\r", ch);
          return;
       }
    }
    
    switch( ch->substate )
    { 
    	default:
    	        if ( arg[0] == '\0' )
                {
                  send_to_char( "&RUsage: Makeshield <name>\n\r&w", ch);
                  return;   
                }

    	        checktool = FALSE;
                checkbatt = FALSE;
                checkcond = FALSE;
                checkcirc = FALSE;
                checkgems = FALSE;

                if ( !IS_SET( ch->in_room->room_flags, ROOM_FACTORY ) )
                {
                   send_to_char( "&RYou need to be in a workshop.\n\r", ch);
                   return;
                }
                
                for ( obj = ch->last_carrying; obj; obj = obj->prev_content )     
                {
                  if (obj->item_type == ITEM_TOOLKIT)
                    checktool = TRUE;
                  if (obj->item_type == ITEM_CRYSTAL)
                    checkgems = TRUE;                    
                  if (obj->item_type == ITEM_BATTERY)
                    checkbatt = TRUE;
                  if (obj->item_type == ITEM_CIRCUIT)
                    checkcirc = TRUE;
                  if (obj->item_type == ITEM_SUPERCONDUCTOR)
                    checkcond = TRUE;                  
                }
                
                if ( !checktool )
                {
                   send_to_char( "&RYou need toolkit to make an energy shield.\n\r", ch);
                   return;
                }
 
                if ( !checkbatt )
                {
                   send_to_char( "&RYou need a power source for your energy shield.\n\r", ch);
                   return;
                }
                
                if ( !checkcirc )
                {
                   send_to_char( "&RYou need a small circuit board.\n\r", ch);
                   return;
                }
                
                if ( !checkcond )
                {
                   send_to_char( "&RYou still need a small superconductor for your energy shield.\n\r", ch);
                   return;
                }
                
                if ( !checkgems )
                {
                   send_to_char( "&RYou need a small crystal.\n\r", ch);
                   return;
                }
                
    	        chance = IS_NPC(ch) ? ch->top_level
	                 : (int) (ch->pcdata->learned[gsn_makeshield]);
                if(ch->subclass == SUBCLASS_TAILOR)
                    chance += 10;
                if ( number_percent( ) < chance )
    		{
    		   send_to_char( "&GYou begin the long process of crafting an energy shield.\n\r", ch);
    		   act( AT_PLAIN, "$n takes $s tools and begins to work on something.", ch,
		        NULL, argument , TO_ROOM );
                   if(IS_IMMORTAL(ch))
                   add_timer( ch, TIMER_DO_FUN, 1, do_makeshield, 1);
		   else if (ch->subclass == SUBCLASS_QUICKWORK )
		   add_timer ( ch , TIMER_DO_FUN , 5 , do_makeshield , 1 );
                   else if(ch->subclass == SUBCLASS_TAILOR)
                   add_timer ( ch, TIMER_DO_FUN,  18 , do_makeshield , 1 );
			else
		   add_timer ( ch , TIMER_DO_FUN , 20 , do_makeshield , 1 );
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
    
    level = IS_NPC(ch) ? ch->skill_level[ENGINEERING_ABILITY] : (int) (ch->pcdata->learned[gsn_makeshield]);
    vnum = 10429;
    
    if ( ( pObjIndex = get_obj_index( vnum ) ) == NULL )
    {
         send_to_char( "&RThe item you are trying to create is missing from the database.\n\rPlease inform the administration of this error.\n\r", ch );
         return;
    }

    checktool = FALSE;
    checkbatt = FALSE;
    checkcond = FALSE;
    checkcirc = FALSE;
    checkgems = FALSE;
    charge = 0;

    for ( obj = ch->last_carrying; obj; obj = obj->prev_content )     
    {
       if (obj->item_type == ITEM_TOOLKIT)
          checktool = TRUE;

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
       if (obj->item_type == ITEM_CRYSTAL && checkgems == FALSE)
       {
          cost = obj->cost;
          gemtype = obj->value[0];
          separate_obj( obj );
          obj_from_char( obj );
          extract_obj( obj );
          checkgems = TRUE;
       }
    }                            
    
    chance = IS_NPC(ch) ? ch->top_level
                : (int) (ch->pcdata->learned[gsn_makeshield]) ;
    if(ch->subclass == SUBCLASS_TAILOR)
       chance += 10;        
    if ( number_percent( ) > chance*2  || ( !checktool ) || ( !checkbatt )
                                       || ( !checkgems ) || ( !checkcond ) || ( !checkcirc) )
    
    {
       send_to_char( "&RYou hold up your new energy shield and press the switch hoping for the best.\n\r", ch);
       send_to_char( "&RInstead of a field of energy being created, smoke starts pouring from the device.\n\r", ch);
       send_to_char( "&RYou drop the hot device and watch as it melts on away on the floor.\n\r", ch);
       learn_from_failure( ch, gsn_makeshield );
       return;
    }

    obj = create_object( pObjIndex, level );
    
    obj->item_type = ITEM_ARMOR;
    SET_BIT( obj->wear_flags, ITEM_WIELD );
    SET_BIT( obj->wear_flags, ITEM_WEAR_SHIELD );
    SET_BIT( obj->extra_flags, ITEM_GLOW );
    SET_BIT( obj->extra_flags, ITEM_HUM );
    obj->level = level;
    obj->weight = 2;
    STRFREE( obj->name );
    strcpy( buf, arg );
    strcat( buf, " energy shield" );
    obj->name = STRALLOC( stripclr( buf ) );
    strcpy( buf, arg );
    STRFREE( obj->short_descr );
    obj->short_descr = STRALLOC( buf );        
    STRFREE( obj->description );
    strcat( buf, " was carelessly misplaced here." );
    obj->description = STRALLOC( buf );
    obj->value[1] = (int) (level/2+gemtype*2);      /* armor */
    if(ch->subclass == SUBCLASS_TAILOR)
    {
       obj->value[1] *= 1.1;
    }
    obj->value[0] = obj->value[1];
    obj->value[4] = charge;
    obj->value[5] = charge;
    CREATE( paf, AFFECT_DATA, 1 );
    paf->type               = -1;
    paf->duration           = -1;
    paf->location           = get_atype( "damageresistance" );
    paf->modifier           = ( gemtype / 4 );
    paf->bitvector          = 0;
    paf->next               = NULL;
    LINK( paf, obj->first_affect, obj->last_affect, next, prev );
    ++top_affect;

    if (obj->value[0] == 0)
       obj->value[0] = 1;

    obj->cost = (int) (obj->value[0] + cost + level);
                                                                    
    obj = obj_to_char( obj, ch );
                                                            
    send_to_char( "&GYou finish your work and hold up your newly created energy shield.&w\n\r", ch);
    act( AT_PLAIN, "$n finishes making $s new energy shield.", ch,
         NULL, argument , TO_ROOM );
    
    {
         long xpgain;
         
         xpgain = UMIN( 35000 ,( exp_level(ch->skill_level[ENGINEERING_ABILITY]+1) - exp_level(ch->skill_level[ENGINEERING_ABILITY]) ) );
         gain_exp(ch, xpgain, ENGINEERING_ABILITY);
         ch_printf( ch , "You gain %d engineering experience.", xpgain );
    }
        learn_from_success( ch, gsn_makeshield );

}

void do_makecontainer( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    char arg2[MAX_INPUT_LENGTH];
    char buf[MAX_STRING_LENGTH];
    int level, chance;
    bool checksew, checkfab; 
    OBJ_DATA *obj;
    OBJ_DATA *material;
    int value;
            
    argument = one_argument( argument, arg );
    strcpy( arg2 , argument );
    if(!IS_NPC(ch))
    {
       if(IS_SET(ch->pcdata->flags, PCFLAG_BOUND))
       {
          send_to_char( "How do you intend to do that when bound?\n\r", ch);
          return;
       }
    }
    
    if ( !str_cmp( arg, "eyes" )
    || !str_cmp( arg, "ears" )
    || !str_cmp( arg, "finger" )
    || !str_cmp( arg, "neck" )
    || !str_cmp( arg, "wrist" ) 
    || !str_cmp( arg, "trinket" ))
    {
        send_to_char( "&RYou cannot make a container for that body part.\n\r&w", ch);
        send_to_char( "&RTry MAKEJEWELRY.\n\r&w", ch);
        return;
    }
    if ( !str_cmp( arg, "feet" ) 
    || !str_cmp( arg, "hands" ) )
    {
        send_to_char( "&RYou cannot make a container for that body part.\n\r&w", ch);
        send_to_char( "&RTry MAKEARMOR.\n\r&w", ch);
        return;
    }
    if ( !str_cmp( arg, "shield" ) )
    {
        send_to_char( "&RYou cannot make a container a shield.\n\r&w", ch);
        send_to_char( "&RTry MAKESHIELD.\n\r&w", ch);
        return;
    }
    if ( !str_cmp( arg, "wield" ) )
    {
        send_to_char( "&RAre you going to fight with a container?\n\r&w", ch);
        send_to_char( "&RTry MAKEBLADE...\n\r&w", ch);
        return;
    }
    
    switch( ch->substate )
    { 
    	default:
    	        
    	        if ( arg2[0] == '\0' )
                {
                  send_to_char( "&RUsage: Makecontainer <wearloc> <name>\n\r&w", ch);
                  return;   
                }
 
    	        checksew = FALSE;
                checkfab = FALSE;
        
                if ( !IS_SET( ch->in_room->room_flags, ROOM_FACTORY ) )
                {
                   send_to_char( "&RYou need to be in a factory or workshop to do that.\n\r", ch);
                   return;
                }
                
                for ( obj = ch->last_carrying; obj; obj = obj->prev_content )     
                {
                  if (obj->item_type == ITEM_FABRIC)
                    checkfab = TRUE;
                  if (obj->item_type == ITEM_THREAD)
          	    checksew = TRUE;
                }
                
                if ( !checkfab )
                {
                   send_to_char( "&RYou need some sort of fabric or material.\n\r", ch);
                   return;
                }
 
                if ( !checksew )
                {
                   send_to_char( "&RYou need a needle and some thread.\n\r", ch);
                   return;
                }

    	        chance = IS_NPC(ch) ? ch->top_level
	                 : (int) (ch->pcdata->learned[gsn_makecontainer]);
                if ( number_percent( ) < chance )
    		{
    		   send_to_char( "&GYou begin the long process of creating a bag.\n\r", ch);
    		   act( AT_PLAIN, "$n takes $s sewing kit and some material and begins to work.", ch,
		        NULL, argument , TO_ROOM );
                   if(IS_IMMORTAL(ch))
                   add_timer(ch, TIMER_DO_FUN, 1, do_makecontainer, 1);
		   else if (ch->subclass == SUBCLASS_QUICKWORK )
		   add_timer ( ch , TIMER_DO_FUN , 2 , do_makecontainer , 1 );
			else
		   add_timer ( ch , TIMER_DO_FUN , 5 , do_makecontainer , 1 );
    		   ch->dest_buf = str_dup(arg);
    		   ch->dest_buf_2 = str_dup(arg2);
    		   return;
	        }
	        send_to_char("&RYou can't figure out what to do.\n\r",ch);
    	   	return;	
    	
    	case 1: 
    		if ( !ch->dest_buf )
    		     return;
    		if ( !ch->dest_buf_2 )
    		     return;
    		strcpy(arg, ch->dest_buf);
    		DISPOSE( ch->dest_buf);
    		strcpy(arg2, ch->dest_buf_2);
    		DISPOSE( ch->dest_buf_2);
    		break;
    		
    	case SUB_TIMER_DO_ABORT:
    		DISPOSE( ch->dest_buf );
    		DISPOSE( ch->dest_buf_2 );
    		ch->substate = SUB_NONE;    		                                   
    	        send_to_char("&RYou are interupted and fail to finish your work.\n\r", ch);
    	        return;
    }
    
    ch->substate = SUB_NONE;
    
    level = IS_NPC(ch) ? ch->skill_level[ENGINEERING_ABILITY] : (int) (ch->pcdata->learned[gsn_makecontainer]);
    
    checksew = FALSE;
    checkfab = FALSE;
    
    for ( obj = ch->last_carrying; obj; obj = obj->prev_content )     
    {
       if (obj->item_type == ITEM_THREAD)
          checksew = TRUE;
       if (obj->item_type == ITEM_FABRIC && checkfab == FALSE)
       {
          checkfab = TRUE;
          separate_obj( obj );
          obj_from_char( obj );
          material = obj;
       }
    }                            
    
    chance = IS_NPC(ch) ? ch->top_level
                : (int) (ch->pcdata->learned[gsn_makecontainer]) ;
                
    if ( number_percent( ) > chance*2  || ( !checkfab ) || ( !checksew ) )
    {
       send_to_char( "&RYou hold up your newly created container.\n\r", ch);
       send_to_char( "&RIt suddenly dawns upon you that you have created the most useless\n\r", ch);
       send_to_char( "&Rcontainer you've ever seen. You quickly hide your mistake...\n\r", ch);
       learn_from_failure( ch, gsn_makecontainer );
       return;
    }

    obj = material; 

    obj->item_type = ITEM_CONTAINER;
    SET_BIT( obj->wear_flags, ITEM_TAKE );
    value = get_wflag( arg );
    if ( value < 0 || value > 31 )
        SET_BIT( obj->wear_flags, ITEM_HOLD );                    
    else
        SET_BIT( obj->wear_flags, 1 << value );
    obj->level = level;
    STRFREE( obj->name );
    strcpy( buf, arg2 );
    obj->name = STRALLOC( stripclr( buf ) );
    strcpy( buf, arg2 );
    STRFREE( obj->short_descr );
    obj->short_descr = STRALLOC( buf );        
    STRFREE( obj->description );
    strcat( buf, " was dropped here." );
    obj->description = STRALLOC( buf );
    obj->value[0] = level * gsn_makecontainer / 2;
    obj->value[1] = 1;
    obj->value[2] = 0;      
    obj->value[3] = 10 + level;      
    obj->cost *= 2;
                                                                    
    obj = obj_to_char( obj, ch );
                                                            
    send_to_char( "&GYou finish your work and hold up your newly created container.&w\n\r", ch);
    act( AT_PLAIN, "$n finishes sewing a new container.", ch,
         NULL, argument , TO_ROOM );
    
    {
         long xpgain;
         
         xpgain = UMIN( 2000 ,( exp_level(ch->skill_level[ENGINEERING_ABILITY]+1) - exp_level(ch->skill_level[ENGINEERING_ABILITY]) ) );
         gain_exp(ch, xpgain, ENGINEERING_ABILITY);
         ch_printf( ch , "You gain %d engineering experience.", xpgain );
    }
        learn_from_success( ch, gsn_makecontainer );
}

void do_makemissile( CHAR_DATA *ch, char *argument )
{
    /* don't think we really need this */
    send_to_char( "&RSorry, this skill isn't finished yet :(\n\r", ch);
}

void do_gemcutting( CHAR_DATA *ch, char *argument )
{
//     send_to_char( "&RSorry, this skill isn't finished yet :(\n\r", ch);
//     return;


   int chance;
   bool checktool, checkoven, checkneedle, checkitem;
   OBJ_DATA *obj;
   checktool = FALSE;
   checkoven = FALSE;
   checkitem = FALSE;
   if(IS_NPC(ch))
   {
      send_to_char("And just what do you think your doing?.\n\r", ch );
      return;
   }
    if(!IS_NPC(ch))
    {
       if(IS_SET(ch->pcdata->flags, PCFLAG_BOUND))
       {
          send_to_char( "How do you intend to do that when bound?\n\r", ch);
          return;
       }
    }

   switch(ch->substate)
   {
   default:
   if (!IS_SET(ch->in_room->room_flags, ROOM_FACTORY))
   {
      send_to_char("&RYou need to be in a factory or workshop to do that.&w\n\r", ch );
      return;
   }
   for ( obj = ch->last_carrying; obj; obj = obj->prev_content )
   {
      if (obj->item_type == ITEM_TOOLKIT)
         checktool = TRUE;
      if (obj->item_type == ITEM_OVEN)
         checkoven = TRUE;
   }
   if (!checktool)
   {
      send_to_char("&RYou need a toolkit to cut the gem.&w\n\r", ch );
      return;
   }
   if (!checkoven)
   {
      send_to_char("&RAn oven is required to temper the cut gem.&w\n\r", ch );
      return;
   }
   if ( ( obj = get_obj_carry( ch, argument ) ) == NULL )
   {
      send_to_char("&RYou do not have that item.&w\n\r", ch );
      return;
   }
   else
   {
   if (obj->item_type != ITEM_CRYSTAL)
   {
      send_to_char("&RTry choosing a gem to cut perhaps?&w\n\r", ch );
      return;
   }
 
   if (obj->value[1] != 0)
   {
      send_to_char("&RThat gem has allready been cut.&w\n\r", ch );
      return;
   }

   }
   chance = (int) (ch->pcdata->learned[gsn_gemcutting]);
   if(ch->subclass == SUBCLASS_TAILOR)
       chance += 10;
   if (number_percent() < chance)
   {
      send_to_char( "&GYou pay 500 credits for materials and begin the long process of cutting a gem.\n\r", ch);
      act( AT_PLAIN, "$n takes $s tools and an oven, and begins to work over a gem.", ch, NULL, argument , TO_ROOM );
      if(IS_IMMORTAL(ch))
      add_timer(ch, TIMER_DO_FUN, 1, do_gemcutting, 1);
      else if (ch->subclass == SUBCLASS_QUICKWORK )
      add_timer ( ch , TIMER_DO_FUN , 8 , do_gemcutting , 1 );
      else if(ch->subclass == SUBCLASS_TAILOR)
      add_timer ( ch, TIMER_DO_FUN, 27  ,  do_gemcutting, 1 );
	else
      add_timer ( ch , TIMER_DO_FUN , 30 , do_gemcutting , 1 );
                   ch->dest_buf   = str_dup(argument);
                   return;
   }
   else
   {
      send_to_char( "&RYou can't figure out where to start...&w\n\r", ch );
      return;
   }
   case 1:
   checktool = FALSE;
   checkoven = FALSE;
   for ( obj = ch->last_carrying; obj; obj = obj->prev_content )
   {
      if(obj->item_type == ITEM_TOOLKIT)
         checktool = TRUE;
      if(obj->item_type == ITEM_OVEN)
         checkoven = TRUE;
   }
   chance = IS_NPC(ch) ? ch->top_level : (int) (ch->pcdata->learned[gsn_gemcutting]);
   chance = UMIN(chance, 65);
   if(ch->subclass == SUBCLASS_TAILOR)
      chance += 10;
   if ( ( obj = get_obj_carry( ch, ch->dest_buf ) ) == NULL )
   {
      send_to_char("&RYou do not have that item.&w\n\r", ch );
      DISPOSE(ch->dest_buf);
      return;
   }
   if (obj->item_type != ITEM_CRYSTAL)
   {
      send_to_char("&RTry choosing a gem to cut perhaps?&w\n\r", ch );
      DISPOSE(ch->dest_buf);
      return;
   }
 
   if (obj->value[1] != 0)
   {
      send_to_char("&RThat gem has already been cut.&w\n\r", ch );
      DISPOSE(ch->dest_buf);
      return;
   }
 
   if ( number_percent( ) > chance  || ( !checktool ) || ( !checkoven )   )
   {
       send_to_char( "&RAs you are about to finish, your chisel slips and renders the gem useless\n\r", ch);
       separate_obj(obj);
       obj->value[0] = 0;
       obj->value[1] = 1;
       learn_from_failure( ch, gsn_gemcutting);
       return;
    }
    send_to_char("You successfully cut the gem as to double it's effectivness\n\r", ch);

    separate_obj(obj);
    obj->value[0] = obj->value[0] * 2;
    obj->value[1] = 1;
    learn_from_success(ch, gsn_gemcutting);
    DISPOSE(ch->dest_buf);
    return;

    case SUB_TIMER_DO_ABORT:
        DISPOSE( ch->dest_buf );
        ch->substate = SUB_NONE;
        send_to_char("&RYou are interupted and fail to finish your work.\n\r", ch);
        return;
    }


}

void do_scrapattack( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *victim;
    sh_int dameq;
    OBJ_DATA *damobj;

    if ( IS_NPC(ch) && IS_AFFECTED( ch, AFF_CHARM ) )
    {
	send_to_char( "Not while charmed!\n\r", ch );
	return;
    }

    if ( ( victim = who_fighting( ch ) ) == NULL )
    {
	send_to_char( "You aren't fighting anyone.\n\r", ch );
	return;
    }

    WAIT_STATE( ch, skill_table[gsn_kick]->beats );
    if ( IS_NPC(ch) || number_percent( ) < ch->pcdata->learned[gsn_scrapattack] )
    {
	send_to_char( "You focus on damaging your foe's equipment...\n\r", ch );
	learn_from_success( ch, gsn_scrapattack );
	global_retcode = damage( ch, victim, number_range( 1, ch->skill_level[ENGINEERING_ABILITY] ), gsn_scrapattack );
   dameq  = number_range(WEAR_LIGHT, WEAR_EYES);
	damobj = get_eq_char(victim, dameq);
	if ( damobj )
	{
	     set_cur_obj(damobj);
	     damage_obj(damobj);
	}
   dameq  = number_range(WEAR_LIGHT, WEAR_EYES);
	damobj = get_eq_char(victim, dameq);
	if ( damobj )
	{
	     set_cur_obj(damobj);
	     damage_obj(damobj);
	}
      dameq  = number_range(WEAR_LIGHT, WEAR_EYES);
	damobj = get_eq_char(victim, dameq);
	if ( damobj )
	{
	     set_cur_obj(damobj);
	     damage_obj(damobj);
	}
    }
    else
    {
	send_to_char( "You are unable to damage your foe's equipment...\n\r", ch );
	learn_from_failure( ch, gsn_scrapattack );
	global_retcode = damage( ch, victim, 0, gsn_kick );
    }
    return;
}

void do_makebludgeon( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    char buf[MAX_STRING_LENGTH];
    int level, chance, strengthmin, strengthmax;
    bool checktool, checkdura, checkfab, checkoven; 
    OBJ_DATA *obj;
    OBJ_INDEX_DATA *pObjIndex;
    int vnum;
    AFFECT_DATA *paf;
    AFFECT_DATA *paf2;
            
    strcpy( arg , argument );
    if(!IS_NPC(ch))
    {
       if(IS_SET(ch->pcdata->flags, PCFLAG_BOUND))
       {
          send_to_char( "How do you intend to do that when bound?\n\r", ch);
          return;
       }
    }
    
    switch( ch->substate )
    { 
    	default:
    	        
    	        if ( arg[0] == '\0' )
                {
                  send_to_char( "&RUsage: Makebludgeon <name>\n\r&w", ch);
                  return;   
                }

                checktool = FALSE;
                checkdura = FALSE;
                checkfab = FALSE;
                checkoven = FALSE;
        
                if ( !IS_SET( ch->in_room->room_flags, ROOM_FACTORY ) )
                {
                   send_to_char( "&RYou need to be in a factory or workshop to do that.\n\r", ch);
                   return;
                }
                
                for ( obj = ch->last_carrying; obj; obj = obj->prev_content )     
                {
                  if (obj->item_type == ITEM_TOOLKIT)
                    checktool = TRUE;
                  if (obj->item_type == ITEM_DURASTEEL)
                    checkdura = TRUE;
                  if (obj->item_type == ITEM_FABRIC)
                    checkfab = TRUE;
                  if (obj->item_type == ITEM_OVEN)
                    checkoven = TRUE;                  
                }
                
                if ( !checktool )
                {
                   send_to_char( "&RYou need toolkit to make a bludgeon weapon.\n\r", ch);
                   return;
                }
 
                if ( !checkdura )
                {
                   send_to_char( "&RA big hunk of metal would be nice...\n\r", ch);
                   return;
                }

                if ( !checkfab )
                {
                   send_to_char( "&RYou need some fabric for the grip!!!\n\r", ch);
                   return;
                }
                
                if ( !checkoven )
                {
                   send_to_char( "&RYou need an oven to heat the metal.\n\r", ch);
                   return;
                }
 
    	        chance = IS_NPC(ch) ? ch->top_level
	                 : (int) (ch->pcdata->learned[gsn_makebludgeon]);
                if(ch->subclass == SUBCLASS_WEAPONSMITH)
                   chance += 10;
                if ( number_percent( ) < chance )
    		{
    		   send_to_char( "&GYou begin the long process of crafting a bludgeon.\n\r", ch);
    		   act( AT_PLAIN, "$n takes $s tools and a small oven and begins to work on something.", ch, NULL, argument , TO_ROOM );
                   if(IS_IMMORTAL(ch))
                   add_timer(ch, TIMER_DO_FUN, 1, do_makebludgeon, 1);
		   else if (ch->subclass == SUBCLASS_QUICKWORK )
		   add_timer ( ch , TIMER_DO_FUN , 3 , do_makebludgeon , 1 );
                   else if (ch->subclass == SUBCLASS_WEAPONSMITH )
                   add_timer ( ch, TIMER_DO_FUN ,  10 , do_makebludgeon , 1 );
		   else
		   add_timer ( ch , TIMER_DO_FUN , 12 , do_makebludgeon , 1 );
    		   ch->dest_buf = str_dup(arg);
    		   return;
	        }
	        send_to_char("&RYou can't figure out how to shove the parts together.\n\r",ch);
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

    level = IS_NPC(ch) ? ch->skill_level[ENGINEERING_ABILITY] : (int) (ch->pcdata->learned[gsn_makebludgeon]);
    vnum = 10415;
    if(ch->subclass == SUBCLASS_WEAPONSMITH)
    {
       level += 10;
    }
    if ( ( pObjIndex = get_obj_index( vnum ) ) == NULL )
    {
         send_to_char( "&RThe item you are trying to create is missing from the database.\n\rPlease inform the administration of this error.\n\r", ch );
         return;
    }

    checktool = FALSE;
    checkdura = FALSE;
    checkfab = FALSE;
    checkoven = FALSE;
    
    for ( obj = ch->last_carrying; obj; obj = obj->prev_content )     
    {
       if (obj->item_type == ITEM_TOOLKIT)
          checktool = TRUE;
       if (obj->item_type == ITEM_OVEN)
          checkoven = TRUE;
       if (obj->item_type == ITEM_DURASTEEL && checkdura == FALSE)
       {
          float exdam;
          if(ch->subclass == SUBCLASS_WEAPONSMITH)
             exdam = 1.1;
          else
             exdam = 1.0;
          strengthmin = (int) URANGE((level/5+20), (exdam*(obj->value[0] * 10 + level/2)), (level/2+100));
          strengthmax = (int) URANGE((level+50), (exdam*(obj->value[1] * 20 + 40)), (level*2+40));
          if(strengthmin > strengthmax)
             strengthmax = strengthmin;
          checkdura = TRUE;
          separate_obj( obj );
          obj_from_char( obj );
          extract_obj( obj );
       }
       if (obj->item_type == ITEM_FABRIC)
       {
          checkfab = TRUE;
          separate_obj( obj );
          obj_from_char( obj );
          extract_obj( obj );
       }
    }                            
    
    chance = IS_NPC(ch) ? ch->top_level
                : (int) (ch->pcdata->learned[gsn_makebludgeon]) ;
   
    if(ch->subclass == SUBCLASS_WEAPONSMITH)
       chance += 10;            
    if ( number_percent( ) > chance*2  || ( !checktool ) || ( !checkdura ) || ( !checkfab ) || ( !checkoven ) )
    {
       send_to_char( "&RYou wave your new bludgeon around some...\n\r", ch);
       send_to_char( "&RThe deformed metal looks really crappy to you,\n\r", ch);
       send_to_char( "&RAnd you decide to hide it before someone see's it \n\r", ch);
       learn_from_failure( ch, gsn_makebludgeon );
       return;
    }

    obj = create_object( pObjIndex, level );
    
    obj->item_type = ITEM_WEAPON;
    SET_BIT( obj->wear_flags, ITEM_WIELD );
    SET_BIT( obj->wear_flags, ITEM_TAKE );
    obj->level = level;
    obj->weight = 8;
    STRFREE( obj->name );
    strcpy( buf, arg );
    strcat( buf, " bludgeon bludge" );
    obj->name = STRALLOC( stripclr( buf ) );
    strcpy( buf, arg );
    STRFREE( obj->short_descr );
    obj->short_descr = STRALLOC( buf );        
    STRFREE( obj->description );
    strcat( buf, " could be used to bash some brains..." );
    obj->description = STRALLOC( buf );
    CREATE( paf, AFFECT_DATA, 1 );
    paf->type               = -1;
    paf->duration           = -1;
    paf->location           = get_atype( "hitroll" );
    paf->modifier           = level/10;
    paf->bitvector          = 0;
    paf->next               = NULL;
    LINK( paf, obj->first_affect, obj->last_affect, next, prev );
    ++top_affect;
    CREATE( paf2, AFFECT_DATA, 1 );
    paf2->type               = -1;
    paf2->duration           = -1;
    paf2->location           = get_atype( "damroll" );
    paf2->modifier           = level/7.5;
    paf2->bitvector          = 0;
    paf2->next               = NULL;
    LINK( paf2, obj->first_affect, obj->last_affect, next, prev );
    ++top_affect;
    obj->value[0] = INIT_WEAPON_CONDITION;      
    obj->value[1] = strengthmin;      /* min dmg  */
    obj->value[2] = strengthmax;      /* max dmg */
    obj->value[3] = WEAPON_BLUDGEON;
    obj->cost = obj->value[2]*10;
                                                                    
    obj = obj_to_char( obj, ch );
                                                            
    send_to_char( "&GYou finish your work and hold up your newly created bludgeon!!!&w\n\r", ch);
    act( AT_PLAIN, "$n finishes crafting a bludgeoning weapon.", ch, NULL, argument , TO_ROOM );
    
    {
         long xpgain;
         
         xpgain = UMIN( 55000 ,( exp_level(ch->skill_level[ENGINEERING_ABILITY]+1) - exp_level(ch->skill_level[ENGINEERING_ABILITY]) ) );
         gain_exp(ch, xpgain, ENGINEERING_ABILITY);
         ch_printf( ch , "You gain %d engineering experience.", xpgain );
    }
        
    learn_from_success( ch, gsn_makebludgeon );
}

void do_repairitem( CHAR_DATA *ch, char *argument)
{
   int chance;
   bool checktool, checkoven, checkneedle, checkitem;
   OBJ_DATA *obj;
   checktool = FALSE;
   checkoven = FALSE;
   checkneedle = FALSE;
   checkitem = FALSE;

   if(IS_NPC(ch))
   {
      send_to_char("And just what do you think your doing?.\n\r", ch );
      return;
   }
    if(!IS_NPC(ch))
    {
       if(IS_SET(ch->pcdata->flags, PCFLAG_BOUND))
       {
          send_to_char( "How do you intend to do that when bound?\n\r", ch);
          return;
       }
    }

   switch(ch->substate)
   {
   default:
   if (!IS_SET(ch->in_room->room_flags, ROOM_FACTORY))
   {
      send_to_char("&RYou need to be in a factory or workshop to do that.&w\n\r", ch );
      return;
   }
   for ( obj = ch->last_carrying; obj; obj = obj->prev_content )
   {
      if (obj->item_type == ITEM_TOOLKIT)
         checktool = TRUE;
      if (obj->item_type == ITEM_OVEN)
         checkoven = TRUE;
      if (obj->item_type == ITEM_THREAD)
         checkneedle = TRUE;
   }
   if (!checktool)
   {
      send_to_char("&RYou need a toolkit to bend items back into shape.&w\n\r", ch );
      return;
   }
   if (!checkoven)
   {
      send_to_char("&RYou might need an oven to remelt the item.&w\n\r", ch );
      return;
   }
   if (!checkneedle)
   {
      send_to_char("&RHow do you expect to put it back together without thread?&w\n\r", ch );
      return;
   }
   if ( ( obj = get_obj_carry( ch, argument ) ) == NULL )
   {
      send_to_char("&RYou do not have that item.&w\n\r", ch );
      return;
   }
   else
   {
      if(!(obj->item_type == ITEM_WEAPON ||
           obj->item_type == ITEM_ARMOR || obj->item_type == ITEM_CARMOR))
      {
         send_to_char("&RYou only know how to repair weapons and armor.&w\n\r", ch );
         return;
      }
   }
   chance = (int) (ch->pcdata->learned[gsn_repairitem]);
   if (number_percent() < chance)
   {
      send_to_char( "&GYou begin the long process repairing an item.\n\r", ch);
      act( AT_PLAIN, "$n takes $s tools, a small oven, and some thread and begins to repair something.", ch, NULL, argument , TO_ROOM );
      if(IS_IMMORTAL(ch))
       add_timer(ch , TIMER_DO_FUN, 1, do_repairitem, 1);
	else   if (ch->subclass == SUBCLASS_QUICKWORK )
      add_timer ( ch , TIMER_DO_FUN , 3 , do_repairitem , 1 );
	else
      add_timer ( ch , TIMER_DO_FUN , 12 , do_repairitem , 1 );
                   ch->dest_buf   = str_dup(argument);
                   return;
   }
   else
   {
      send_to_char( "&RYou can't figure out the problem.&w\n\r", ch );
      return;
   }
   case 1:
   checktool = FALSE;
   checkoven = FALSE;
   checkneedle = FALSE;
   for ( obj = ch->last_carrying; obj; obj = obj->prev_content )
   {
      if(obj->item_type == ITEM_TOOLKIT)
         checktool = TRUE;
      if(obj->item_type == ITEM_OVEN)
         checkoven = TRUE;
      if(obj->item_type == ITEM_THREAD)
         checkneedle = TRUE;
   }
   chance = IS_NPC(ch) ? ch->top_level : (int) (ch->pcdata->learned[gsn_repairitem]);
   chance = UMIN(chance, 95);
   if ( ( obj = get_obj_carry( ch, ch->dest_buf ) ) == NULL )
   {
      send_to_char("&RYou do not have that item.&w\n\r", ch );
      DISPOSE(ch->dest_buf);
      return;
   }
   if ( number_percent( ) > chance*2  || ( !checktool ) || ( !checkoven )  || ( !checkneedle ) )
   {
       send_to_char( "&RYou finish putting the item back together\n\r", ch);
       send_to_char( "&RYou reexamine the schematics you wrote.\n\r", ch);
       send_to_char( "&RAnd find several errors damaging the item.\n\r", ch);
       damage_obj(obj);
       learn_from_failure( ch, gsn_repairitem);
       return;
    }
    send_to_char("You successfully put the item back together.\n\r", ch);
    switch ( obj->item_type )
    {
      default:
        send_to_char( "???\n\r", ch);
        break;
      case ITEM_CARMOR:
      case ITEM_ARMOR:
        obj->value[0] = obj->value[1];
        break;
      case ITEM_WEAPON:
        obj->value[0] = INIT_WEAPON_CONDITION;
        break;
    }
    learn_from_success(ch, gsn_repairitem);
    oprog_repair_trigger(ch, obj);
    DISPOSE(ch->dest_buf);
    return;

    case SUB_TIMER_DO_ABORT:
        DISPOSE( ch->dest_buf );
        ch->substate = SUB_NONE;
        send_to_char("&RYou are interupted and fail to finish your work.\n\r", ch);
        return;
    }
}


void do_makeforcepike( CHAR_DATA *ch, char *argument )
{
   char arg[MAX_INPUT_LENGTH];
   char buf[MAX_STRING_LENGTH];
   int level, chance, charge;
   bool checktool, checkdura, checkbatt, checkoven;
   OBJ_DATA *obj;
   OBJ_INDEX_DATA *pObjIndex;
   int vnum, strengthmin, strengthmax;
   AFFECT_DATA *paf;
   AFFECT_DATA *paf2;
   AFFECT_DATA *paf3;
   
   strcpy( arg , argument );
    if(!IS_NPC(ch))
    {
       if(IS_SET(ch->pcdata->flags, PCFLAG_BOUND))
       {
          send_to_char( "How do you intend to do that when bound?\n\r", ch);
          return;
       }
    }

   switch ( ch->substate )
   {
   default:
      if (arg[0] == '\0' )
      {
         send_to_char("&RUsage: Makeforcepike <name>\n\r&w", ch);
         return;
      }
      checktool = FALSE;
      checkdura = FALSE;
      checkbatt = FALSE;
      checkoven = FALSE;
        
      if ( !IS_SET( ch->in_room->room_flags, ROOM_FACTORY ) )
      {
         send_to_char("&RYou need to be in a factory or workshop to do that.\n\r&w", ch);
         return;
      }
      for (obj = ch->last_carrying; obj; obj = obj->prev_content )
  	    {
        if (obj->item_type == ITEM_TOOLKIT)
           checktool = TRUE;
        if (obj->item_type ==  ITEM_DURASTEEL)
           checkdura = TRUE;
        if (obj->item_type == ITEM_BATTERY)
           checkbatt = TRUE;
        if (obj->item_type == ITEM_OVEN)
           checkoven = TRUE;
      }
      if( !checktool )
      {
         send_to_char( "&RYou need a toolkit to construct a force pike.\n\r", ch);
         return;
      }
      if(!checkdura)
      {
         send_to_char( "&RYou need something to make it out of.\n\r", ch);
         return;
      } 
      if (!checkbatt)
      {
         send_to_char("&RYou need a powersource for the tip.\n\r", ch);
         return;
      }
      if (!checkoven)
      {
         send_to_char("&RYou need a heat source to forge the metal.\n\r", ch);
         return;
      }
      chance = IS_NPC(ch) ? ch->top_level : (int) (ch->pcdata->learned[gsn_makeforcepike]);
      if(ch->subclass == SUBCLASS_WEAPONSMITH)
         chance += 10;
      if ( number_percent( ) < chance )
      {
          send_to_char( "&GYou begin the long process of crafting a force pike.\n\r", ch);
          act( AT_PLAIN, "$n takes $s tools and a small oven and begins to work on something.", ch,
               NULL, argument , TO_ROOM );
          if(IS_IMMORTAL(ch))
          add_timer( ch, TIMER_DO_FUN, 1, do_makeforcepike, 1);
          else if (ch->subclass == SUBCLASS_QUICKWORK )
          add_timer ( ch , TIMER_DO_FUN , 3 , do_makeforcepike , 1 );
          else
          add_timer ( ch , TIMER_DO_FUN , 12 , do_makeforcepike , 1 );
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

    level = IS_NPC(ch) ? ch->skill_level[ENGINEERING_ABILITY] : (int) (ch->pcdata->learned[gsn_makeforcepike]);
    vnum = 10438;
    if(ch->subclass == SUBCLASS_WEAPONSMITH)
    {
      level += 10;
    }
    if ( ( pObjIndex = get_obj_index( vnum ) ) == NULL )
    {
         send_to_char( "&RThe item you are trying to create is missing from the database.\n\rPlease inform the administration of this error.\r\n", ch);
         return;
    }

    checktool = FALSE;
    checkdura = FALSE;
    checkbatt = FALSE;
    checkoven = FALSE;

    for ( obj = ch->last_carrying; obj; obj = obj->prev_content )
    {
       if (obj->item_type == ITEM_TOOLKIT)
          checktool = TRUE;
       if (obj->item_type == ITEM_OVEN)
          checkoven = TRUE;
       if (obj->item_type == ITEM_DURASTEEL && checkdura == FALSE)
       {
          /* durasteel affects damage. */
          float exdam;
          if(ch->subclass == SUBCLASS_WEAPONSMITH)
             exdam = 1.1;
          else
             exdam = 1.0;
          strengthmin = (int) URANGE(((level/3)*2), (2*exdam *(obj->value[0] * 10 + 15)), ((level/2+50)*2));
          strengthmax = (int) URANGE(((level/2)*2), (2*exdam *(obj->value[1] * 15 + 30)), ((level+50)*2));
          if (strengthmin > strengthmax)
             strengthmax = strengthmin;
          checkdura = TRUE;
          separate_obj( obj );
          obj_from_char( obj );
          extract_obj( obj );
       }
       if (obj->item_type == ITEM_BATTERY && checkbatt == FALSE )
       {
          charge = UMAX( 5, obj->value[0] );
          separate_obj( obj );
          obj_from_char( obj );
          extract_obj( obj );
          checkbatt = TRUE;
       }
    }
    chance = IS_NPC(ch) ? ch->top_level : (int) (ch->pcdata->learned[gsn_makeforcepike]) ;
    if(ch->subclass == SUBCLASS_WEAPONSMITH)
       chance += 10;
    if ( number_percent( ) > chance*2  || ( !checktool ) || ( !checkdura ) || ( !checkbatt ) || ( !checkoven ) )
    {
       send_to_char( "&RYou activate your newly created force pike.\n\r", ch);
       send_to_char( "&RIt hums softly for a few seconds then begins to shake violently.\n\r", ch);
       send_to_char( "&RThe shaft assembly comes apart, you quickly dispose of it.\n\r", ch);
       learn_from_failure( ch, gsn_makeforcepike );
       return;
    }

    obj = create_object( pObjIndex, level );

    obj->item_type = ITEM_WEAPON;
    SET_BIT( obj->wear_flags, ITEM_WIELD );
    SET_BIT( obj->wear_flags, ITEM_TAKE );
    obj->level = level;
    obj->weight = 7;
    STRFREE( obj->name );
    strcpy( buf, arg );
    strcat( buf, " force pike forcepike" );
    obj->name = STRALLOC( stripclr( buf ) );
    strcpy( buf, arg );
    STRFREE( obj->short_descr );
    obj->short_descr = STRALLOC( buf );
    STRFREE( obj->description );
    strcat( buf, " was left here." );
    obj->description = STRALLOC( buf );
    CREATE( paf, AFFECT_DATA, 1 );
    paf->type               = -1;
    paf->duration           = -1;
    paf->location           = get_atype( "disarm" );
    paf->modifier           = level/2;
    paf->bitvector          = 0;
    paf->next               = NULL;
    LINK( paf, obj->first_affect, obj->last_affect, next, prev );
    ++top_affect;
    CREATE( paf2, AFFECT_DATA, 1 );
    paf2->type               = -1;
    paf2->duration           = -1;
    paf2->location           = get_atype( "hitroll" );
    paf2->modifier           = level/25 + 1;
    paf2->bitvector          = 0;
    paf2->next               = NULL;
    LINK( paf2, obj->first_affect, obj->last_affect, next, prev );
    ++top_affect;
    CREATE( paf3, AFFECT_DATA, 1 );
    paf3->type               = -1;
    paf3->duration           = -1;
    paf3->location           = get_atype( "damroll" );
    paf3->modifier           = level/25 + 1;
    paf3->bitvector          = 0;
    paf3->next               = NULL;
    LINK( paf3, obj->first_affect, obj->last_affect, next, prev );
    ++top_affect;
    obj->value[0] = INIT_WEAPON_CONDITION;
    obj->value[1] = strengthmin;      /* min dmg  */
    obj->value[2] = strengthmax;      /* max dmg */
    obj->value[3] = WEAPON_FORCE_PIKE;
    obj->value[4] = charge;
    obj->value[5] = charge;
    obj->cost = obj->value[2]*20;

    obj = obj_to_char( obj, ch );

    send_to_char( "&GYou finish your work and hold up your newly created pike.&w\n\r", ch);
    act( AT_PLAIN, "$n finishes crafting a force pike.", ch,
         NULL, argument , TO_ROOM );

    {
         long xpgain;

         xpgain = UMIN( 35000 ,( exp_level(ch->skill_level[ENGINEERING_ABILITY]+1) - exp_level(ch->skill_level[ENGINEERING_ABILITY]) ) );
         gain_exp(ch, xpgain, ENGINEERING_ABILITY);
         ch_printf( ch , "You gain %d engineering experience.", xpgain );
    }

    learn_from_success( ch, gsn_makeforcepike );
    
} 
