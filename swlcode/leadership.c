/***************************************************************************

Star Wars Life 3.0a
Leadership Skills/Abilities

FOr credits see 'Help Credits' in game.

(c) 2001, 2002; FRPG Ltd.
All Rights Reserved

****************************************************************************/

#include <math.h> 
#include <sys/types.h>
#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include "mud.h"

int ris_save( CHAR_DATA *ch, int chance, int ris );

void do_reinforcements( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    int chance, credits;
    
    if ( IS_NPC( ch ) || !ch->pcdata )
    	return;
    	
    strcpy( arg, argument );    

    
    switch( ch->substate )
    { 
    	default:
    	        if ( ch->backup_wait )
    	        {
    	            send_to_char( "&RYour reinforcements are already on the way.\n\r", ch );
    	            return;
    	        }
    	        
    	        if ( !ch->pcdata->clan )
    	        {
    	            send_to_char( "&RYou need to be a member of an organization before you can call for reinforcements.\n\r", ch );
    	            return;
    	        }    
    	        
    	        if ( ch->gold < ch->skill_level[LEADERSHIP_ABILITY] * 50 )
    	        {
    	            ch_printf( ch, "&RYou dont have enough credits to send for reinforcements.\n\r" );
    	            return;
    	        }    
    	        
    	        chance = (int) (ch->pcdata->learned[gsn_reinforcements]);
                if ( number_percent( ) < chance )
    		{
    		   send_to_char( "&GYou begin making the call for reinforcements.\n\r", ch);
    		   act( AT_PLAIN, "$n begins issuing orders into $s comlink.", ch,
		        NULL, argument , TO_ROOM );
		   add_timer ( ch , TIMER_DO_FUN , 1 , do_reinforcements , 1 );
    		   ch->dest_buf = str_dup(arg);
    		   return;
	        }
	        send_to_char("&RYou call for reinforcements but nobody answers.\n\r",ch);
	        learn_from_failure( ch, gsn_reinforcements );
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
    	        send_to_char("&RYou are interupted before you can finish your call.\n\r", ch);
    	        return;
    }
    
    ch->substate = SUB_NONE;
    
    send_to_char( "&GYour reinforcements are on the way.\n\r", ch);
    credits = ch->skill_level[LEADERSHIP_ABILITY] * 50;
    ch_printf( ch, "It cost you %d credits.\n\r", credits);
    ch->gold -= UMIN( credits , ch->gold );
             
    learn_from_success( ch, gsn_reinforcements );
        gain_exp( ch, ch->skill_level[LEADERSHIP_ABILITY] * 800, LEADERSHIP_ABILITY);
    
    if ( nifty_is_name( "empire" , ch->pcdata->clan->name ) )
       ch->backup_mob = MOB_VNUM_STORMTROOPER;
    else if ( nifty_is_name( "republic" , ch->pcdata->clan->name ) )
       ch->backup_mob = MOB_VNUM_NR_TROOPER;
    else
       ch->backup_mob = MOB_VNUM_MERCINARY;

    ch->backup_wait = number_range(1,2);
    
}

void do_postguard( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    int chance, credits;
    
    if ( IS_NPC( ch ) || !ch->pcdata )
    	return;
    	
    strcpy( arg, argument );    
    
    switch( ch->substate )
    { 
    	default:
    	        if ( ch->backup_wait )
    	        {
    	            send_to_char( "&RYou already have backup coming.\n\r", ch );
    	            return;
    	        }
    	        
    	        if ( !ch->pcdata->clan )
    	        {
    	            send_to_char( "&RYou need to be a member of an organization before you can call for a guard.\n\r", ch );
    	            return;
    	        }    
    	        
    	        if ( ch->gold < ch->skill_level[LEADERSHIP_ABILITY] * 30 )
    	        {
    	            ch_printf( ch, "&RYou dont have enough credits.\n\r", ch );
    	            return;
    	        }    
    	        
    	        chance = (int) (ch->pcdata->learned[gsn_postguard]);
                if ( number_percent( ) < chance )
    		{
    		   send_to_char( "&GYou begin making the call for reinforcements.\n\r", ch);
    		   act( AT_PLAIN, "$n begins issuing orders into $s comlink.", ch,
		        NULL, argument , TO_ROOM );
		   add_timer ( ch , TIMER_DO_FUN , 1 , do_postguard , 1 );
    		   ch->dest_buf = str_dup(arg);
    		   return;
	        }
	        send_to_char("&RYou call for a guard but nobody answers.\n\r",ch);
	        learn_from_failure( ch, gsn_postguard );
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
    	        send_to_char("&RYou are interupted before you can finish your call.\n\r", ch);
    	        return;
    }
    
    ch->substate = SUB_NONE;
    
    send_to_char( "&GYour guard is on the way.\n\r", ch);
    
    credits = ch->skill_level[LEADERSHIP_ABILITY] * 20;
    ch_printf( ch, "It cost you %d credits.\n\r", credits);
    ch->gold -= UMIN( credits , ch->gold );
        gain_exp( ch, ch->skill_level[LEADERSHIP_ABILITY] * 400, LEADERSHIP_ABILITY);

    learn_from_success( ch, gsn_postguard );
    
    ch->backup_mob = MOB_VNUM_BOUNCER;

    ch->backup_wait = 1;

}

void do_add_patrol ( CHAR_DATA *ch , char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    int chance, credits;
    
    if ( IS_NPC( ch ) || !ch->pcdata )
    	return;
    	
    strcpy( arg, argument );    
    
    switch( ch->substate )
    { 
    	default:
    	        if ( ch->backup_wait )
    	        {
    	            send_to_char( "&RYou already have backup patrol coming.\n\r", ch );
    	            return;
    	        }
    	        
    	        if ( !ch->pcdata->clan )
    	        {
    	            send_to_char( "&RYou need to be a member of an organization before you can call for a patrol.\n\r", ch );
    	            return;
    	        }    
    	        
    	        if ( ch->gold < ch->skill_level[LEADERSHIP_ABILITY] * 30 )
    	        {
    	            ch_printf( ch, "&RYou dont have enough credits.\n\r", ch );
    	            return;
    	        }    
    	        
    	        chance = (int) (ch->pcdata->learned[gsn_addpatrol]);
                if ( number_percent( ) < chance )
    		{
    		   send_to_char( "&GYou begin making the call for a patrol.\n\r", ch);
    		   act( AT_PLAIN, "$n begins issuing orders into $s comlink.", ch,
		        NULL, argument , TO_ROOM );
		   add_timer ( ch , TIMER_DO_FUN , 1 , do_add_patrol , 1 );
    		   ch->dest_buf = str_dup(arg);
    		   return;
	        }
	        send_to_char("&RYou call for a patrol but nobody answers.\n\r",ch);
	        learn_from_failure( ch, gsn_addpatrol );
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
    	        send_to_char("&RYou are interupted before you can finish your call.\n\r", ch);
    	        return;
    }
    
    ch->substate = SUB_NONE;
    
    send_to_char( "&GYour patrol is on the way.\n\r", ch);
    
    credits = ch->skill_level[LEADERSHIP_ABILITY] * 20;
    ch_printf( ch, "It cost you %d credits.\n\r", credits);
    ch->gold -= UMIN( credits , ch->gold );

    learn_from_success( ch, gsn_addpatrol );
        gain_exp( ch, ch->skill_level[LEADERSHIP_ABILITY] * 600, LEADERSHIP_ABILITY);
    
    ch->backup_mob = MOB_VNUM_PATROL;

    ch->backup_wait = 1;

}

void do_elite_patrol ( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    int chance, credits;
    
    if ( IS_NPC( ch ) || !ch->pcdata )
    	return;
    	
    strcpy( arg, argument );    
    
    switch( ch->substate )
    { 
    	default:
    	        if ( ch->backup_wait )
    	        {
    	            send_to_char( "&RYou already have an elite patrol coming.\n\r", ch );
    	            return;
    	        }
    	        
    	        if ( !ch->pcdata->clan )
    	        {
    	            send_to_char( "&RYou need to be a member of an organization before you can call for an elite patrol.\n\r", ch );
    	            return;
    	        }    
    	        
    	        if ( ch->gold < ch->skill_level[LEADERSHIP_ABILITY] * 50 )
    	        {
    	            ch_printf( ch, "&RYou dont have enough credits.\n\r", ch );
    	            return;
    	        }    
    	        
    	        chance = (int) (ch->pcdata->learned[gsn_elitepatrol]);
                if ( number_percent( ) < chance )
    		{
    		   send_to_char( "&GYou begin making the call for an elite patrol.\n\r", ch);
    		   act( AT_PLAIN, "$n begins issuing orders into $s comlink.", ch,
		        NULL, argument , TO_ROOM );
		   add_timer ( ch , TIMER_DO_FUN , 1 , do_elite_patrol , 1 );
    		   ch->dest_buf = str_dup(arg);
    		   return;
	        }
	        send_to_char("&RYou call for an elite patrol but nobody answers.\n\r",ch);
	        learn_from_failure( ch, gsn_elitepatrol );
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
    	        send_to_char("&RYou are interupted before you can finish your call.\n\r", ch);
    	        return;
    }
    
    ch->substate = SUB_NONE;
    
    send_to_char( "&GYour elite patrol is on the way.\n\r", ch);
    
    credits = ch->skill_level[LEADERSHIP_ABILITY] * 50;
    ch_printf( ch, "It cost you %d credits.\n\r", credits);
    ch->gold -= UMIN( credits , ch->gold );

    learn_from_success( ch, gsn_elitepatrol );
        gain_exp( ch, ch->skill_level[LEADERSHIP_ABILITY] * 1200, LEADERSHIP_ABILITY);
    
    ch->backup_mob = MOB_VNUM_EPATROL;

    ch->backup_wait = 1;

}

void do_special_forces ( CHAR_DATA *ch , char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    int chance, credits;
    
    if ( IS_NPC( ch ) || !ch->pcdata )
    	return;
    	
    strcpy( arg, argument );    

    
    switch( ch->substate )
    { 
    	default:
    	        if ( ch->backup_wait )
    	        {
    	            send_to_char( "&RYour Special Forces are already on the way.\n\r", ch );
    	            return;
    	        }
    	        
    	        if ( !ch->pcdata->clan )
    	        {
    	            send_to_char( "&RYou need to be a member of an organization before you can call for Special Forces.\n\r", ch );
    	            return;
    	        }    
    	        
    	        if ( ch->gold < ch->skill_level[LEADERSHIP_ABILITY] * 60 )
    	        {
    	            ch_printf( ch, "&RYou dont have enough credits to send for Special forces.\n\r" );
    	            return;
    	        }    
    	        
    	        chance = (int) (ch->pcdata->learned[gsn_specialforces]);
                if ( number_percent( ) < chance )
    		{
    		   send_to_char( "&GYou begin making the call for Special Forces.\n\r", ch);
    		   act( AT_PLAIN, "$n begins issuing orders into $s comlink.", ch,
		        NULL, argument , TO_ROOM );
		   add_timer ( ch , TIMER_DO_FUN , 1 , do_special_forces , 1 );
    		   ch->dest_buf = str_dup(arg);
    		   return;
	        }
	        send_to_char("&RYou call for Special Forces but nobody answers.\n\r",ch);
	        learn_from_failure( ch, gsn_specialforces );
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
    	        send_to_char("&RYou are interupted before you can finish your call.\n\r", ch);
    	        return;
    }
    
    ch->substate = SUB_NONE;
    
    send_to_char( "&GYour reinforcements are on the way.\n\r", ch);
    credits = ch->skill_level[LEADERSHIP_ABILITY] * 60;
    ch_printf( ch, "It cost you %d credits.\n\r", credits);
    ch->gold -= UMIN( credits , ch->gold );
             
    learn_from_success( ch, gsn_specialforces );
        gain_exp( ch, ch->skill_level[LEADERSHIP_ABILITY] * 2000, LEADERSHIP_ABILITY);
    
    ch->backup_mob = MOB_VNUM_SPECIAL;

    ch->backup_wait = number_range(1,2);
    
}

void do_elite_guard ( CHAR_DATA *ch , char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    int chance, credits;
    
    if ( IS_NPC( ch ) || !ch->pcdata )
    	return;
    	
    strcpy( arg, argument );    
    
    switch( ch->substate )
    { 
    	default:
    	        if ( ch->backup_wait )
    	        {
    	            send_to_char( "&RYou already have elite backup coming.\n\r", ch );
    	            return;
    	        }
    	        
    	        if ( !ch->pcdata->clan )
    	        {
    	            send_to_char( "&RYou need to be a member of an organization before you can call for an elite guard.\n\r", ch );
    	            return;
    	        }    
    	        
    	        if ( ch->gold < ch->skill_level[LEADERSHIP_ABILITY] * 50 )
    	        {
    	            ch_printf( ch, "&RYou dont have enough credits.\n\r", ch );
    	            return;
    	        }    
    	        
    	        chance = (int) (ch->pcdata->learned[gsn_eliteguard]);
                if ( number_percent( ) < chance )
    		{
    		   send_to_char( "&GYou begin making the call for elite reinforcements.\n\r", ch);
    		   act( AT_PLAIN, "$n begins issuing orders into $s comlink.", ch,
		        NULL, argument , TO_ROOM );
		   add_timer ( ch , TIMER_DO_FUN , 1 , do_elite_guard , 1 );
    		   ch->dest_buf = str_dup(arg);
    		   return;
	        }
	        send_to_char("&RYou call for an elite guard but nobody answers.\n\r",ch);
	        learn_from_failure( ch, gsn_eliteguard );
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
    	        send_to_char("&RYou are interupted before you can finish your call.\n\r", ch);
    	        return;
    }
    
    ch->substate = SUB_NONE;
    
    send_to_char( "&GYour elite guard is on the way.\n\r", ch);
    
    credits = ch->skill_level[LEADERSHIP_ABILITY] * 50;
    ch_printf( ch, "It cost you %d credits.\n\r", credits);
    ch->gold -= UMIN( credits , ch->gold );

    learn_from_success( ch, gsn_eliteguard );
        gain_exp( ch, ch->skill_level[LEADERSHIP_ABILITY] * 1000, LEADERSHIP_ABILITY);
    
    ch->backup_mob = MOB_VNUM_ELITE;

    ch->backup_wait = 1;

}

void add_reinforcements( CHAR_DATA *ch )
{
     MOB_INDEX_DATA  * pMobIndex;
     OBJ_DATA        * blaster;
     OBJ_INDEX_DATA  * pObjIndex;
     
     if ( ( pMobIndex = get_mob_index( ch->backup_mob ) ) == NULL )
        return;         

     if ( ch->backup_mob == MOB_VNUM_STORMTROOPER ||
          ch->backup_mob == MOB_VNUM_NR_TROOPER   ||
          ch->backup_mob == MOB_VNUM_MERCINARY       )  
     {
        CHAR_DATA * mob[3];
        int         mob_cnt;
        
        send_to_char( "Your reinforcements have arrived.\n\r", ch );
        for ( mob_cnt = 0 ; mob_cnt < 3 ; mob_cnt++ )
        {
            int ability;
            mob[mob_cnt] = create_mobile( pMobIndex );
            char_to_room( mob[mob_cnt], ch->in_room );
            act( AT_IMMORT, "$N has arrived.", ch, NULL, mob[mob_cnt], TO_ROOM );
            mob[mob_cnt]->top_level = ch->skill_level[LEADERSHIP_ABILITY]/3;
            for ( ability = 0 ; ability < MAX_ABILITY ; ability++ )
                     mob[mob_cnt]->skill_level[ability] = mob[mob_cnt]->top_level;
            mob[mob_cnt]->hit = mob[mob_cnt]->top_level*15;
            mob[mob_cnt]->max_hit = mob[mob_cnt]->hit;
            mob[mob_cnt]->armor = 100- mob[mob_cnt]->top_level*2.5;
            mob[mob_cnt]->damroll = mob[mob_cnt]->top_level/5;
            mob[mob_cnt]->hitroll = mob[mob_cnt]->top_level/5;
            if ( ( pObjIndex = get_obj_index( OBJ_VNUM_BLASTECH_E11 ) ) != NULL )
            {
                 blaster = create_object( pObjIndex, mob[mob_cnt]->top_level );
                 obj_to_char( blaster, mob[mob_cnt] );
                 equip_char( mob[mob_cnt], blaster, WEAR_WIELD );                        
            } 
            if ( mob[mob_cnt]->master )
	       stop_follower( mob[mob_cnt] );
	    add_follower( mob[mob_cnt], ch );
            SET_BIT( mob[mob_cnt]->affected_by, AFF_CHARM );
            do_setblaster( mob[mob_cnt] , "full" );
	if (ch->subclass == SUBCLASS_OFFICER)
	{
	    if (mob[mob_cnt]->hit <= 16000)
	        {
		    mob[mob_cnt]->hit *= 2;
		    mob[mob_cnt]->max_hit *= 2;
		}
		else
		{
			mob[mob_cnt]->hit = 32000;
			mob[mob_cnt]->max_hit = 32000;
		}
	    if (mob[mob_cnt]->hit <= 16000)
		{
		    	mob[mob_cnt]->hit *= 2;
		 	mob[mob_cnt]->max_hit *= 2;
		}
	    mob[mob_cnt]->armor = -1300;
	    if(mob[mob_cnt]->damroll <= 500)
		mob[mob_cnt]->damroll = 1000;
	    else
		mob[mob_cnt]->damroll *= 2;
	    if(mob[mob_cnt]->hitroll <= 500)
		mob[mob_cnt]->hitroll = 1000;
	    else
		mob[mob_cnt]->hitroll *= 2;
	}
        }
        return;
     }
     if (ch->backup_mob == MOB_VNUM_SPECIAL )
     {
        CHAR_DATA * mob[3];
        int         mob_cnt;
        
        send_to_char( "Your Special Forces have arrived.\n\r", ch );
        for ( mob_cnt = 0 ; mob_cnt < 3 ; mob_cnt++ )
        {
            int ability;
            mob[mob_cnt] = create_mobile( pMobIndex );
            char_to_room( mob[mob_cnt], ch->in_room );
            act( AT_IMMORT, "$N has arrived.", ch, NULL, mob[mob_cnt], TO_ROOM );
            mob[mob_cnt]->top_level = 105;
            for ( ability = 0 ; ability < MAX_ABILITY ; ability++ )
                     mob[mob_cnt]->skill_level[ability] = mob[mob_cnt]->top_level;
            mob[mob_cnt]->hit = mob[mob_cnt]->top_level*30;
            mob[mob_cnt]->max_hit = mob[mob_cnt]->hit;
            mob[mob_cnt]->armor = 100- mob[mob_cnt]->top_level*4.5;
            mob[mob_cnt]->damroll = mob[mob_cnt]->top_level;
            mob[mob_cnt]->hitroll = mob[mob_cnt]->top_level;
            if ( ( pObjIndex = get_obj_index( OBJ_VNUM_BLASTECH_E11 ) ) != NULL )
            {
                 blaster = create_object( pObjIndex, mob[mob_cnt]->top_level );
                 obj_to_char( blaster, mob[mob_cnt] );
                 equip_char( mob[mob_cnt], blaster, WEAR_WIELD );                        
            } 
            if ( mob[mob_cnt]->master )
	       stop_follower( mob[mob_cnt] );
	    add_follower( mob[mob_cnt], ch );
            SET_BIT( mob[mob_cnt]->affected_by, AFF_CHARM );
            do_setblaster( mob[mob_cnt] , "full" );
	if (ch->subclass == SUBCLASS_OFFICER)
	{

/*
	    mob[mob_cnt]->hit *= 2;
	    mob[mob_cnt]->max_hit *= 2;
	    mob[mob_cnt]->armor = -1300;
	    mob[mob_cnt]->damroll *= 2;
	    mob[mob_cnt]->hitroll *= 2;
*/

	    if (mob[mob_cnt]->hit > 16000)
		{
			mob[mob_cnt]->hit = 32000;
			mob[mob_cnt]->max_hit = 32000;
		}
	    else  
		{
		    	mob[mob_cnt]->hit *= 2;
		 	mob[mob_cnt]->max_hit *= 2;
		}
	    mob[mob_cnt]->armor = -1300;
	    if(mob[mob_cnt]->damroll <= 500)
		mob[mob_cnt]->damroll = 1000;
	    else
		mob[mob_cnt]->damroll *= 2;
	    if(mob[mob_cnt]->hitroll <= 500)
		mob[mob_cnt]->hitroll = 1000;
	    else
		mob[mob_cnt]->hitroll *= 2;



	}
        }
        return;
     }
     if (ch->backup_mob == MOB_VNUM_ELITE )
     {
        CHAR_DATA *mob;
        int ability;
        
        mob = create_mobile( pMobIndex );
        char_to_room( mob, ch->in_room );
        if ( ch->pcdata && ch->pcdata->clan )
        {
          char tmpbuf[MAX_STRING_LENGTH];
        
          STRFREE( mob->name );
          mob->name = STRALLOC( ch->pcdata->clan->name );
          sprintf( tmpbuf , "(%s) %s" , ch->pcdata->clan->name  , mob->long_descr );
          STRFREE( mob->long_descr );
          mob->long_descr = STRALLOC( tmpbuf );
        }
        act( AT_IMMORT, "$N has arrived.", ch, NULL, mob, TO_ROOM );
        send_to_char( "Your elite guard has arrived.\n\r", ch );
        mob->top_level = 105;
        for ( ability = 0 ; ability < MAX_ABILITY ; ability++ )
            mob->skill_level[ability] = mob->top_level;
        mob->hit = mob->top_level*25;
        mob->max_hit = mob->hit;
        mob->armor = 100- mob->top_level*3.5;
        mob->damroll = mob->top_level;
        mob->hitroll = mob->top_level;
        if ( ( pObjIndex = get_obj_index( OBJ_VNUM_BLASTECH_E11 ) ) != NULL )
        {
            blaster = create_object( pObjIndex, mob->top_level );
            obj_to_char( blaster, mob );
            equip_char( mob, blaster, WEAR_WIELD );                        
        }
        
        /* for making this more accurate in the future */
        
        if ( mob->mob_clan )
           STRFREE ( mob->mob_clan );
        if ( ch->pcdata && ch->pcdata->clan )   
           mob->mob_clan = STRALLOC( ch->pcdata->clan->name );
	if (ch->subclass == SUBCLASS_OFFICER)
	{
/*
	    mob->hit *= 2;
	    mob->max_hit *= 2;
	    mob->armor = -1300;
	    mob->damroll *= 2;
	    mob->hitroll *= 2;
*/
	    if (mob->hit <= 16000)
		    mob->hit *= 2;
		else
		{
			mob->hit = 32000;
			mob->max_hit = 32000;
		}
	    if (mob->hit <= 16000)
		{
		    	mob->hit *= 2;
		 	mob->max_hit *= 2;
		}
	    mob->armor = -1300;
	    if(mob->damroll <= 500)
		mob->damroll = 1000;
	    else
		mob->damroll *= 2;
	    if(mob->hitroll <= 500)
		mob->hitroll = 1000;
	    else
		mob->hitroll *= 2;


	}
        return;
     }
     if (ch->backup_mob == MOB_VNUM_EPATROL )
     {
        CHAR_DATA *mob;
        int ability;
        
        mob = create_mobile( pMobIndex );
        char_to_room( mob, ch->in_room );
        if ( ch->pcdata && ch->pcdata->clan )
        {
          char tmpbuf[MAX_STRING_LENGTH];
        
          STRFREE( mob->name );
          mob->name = STRALLOC( ch->pcdata->clan->name );
          sprintf( tmpbuf , "(%s) %s" , ch->pcdata->clan->name  , mob->long_descr );
          STRFREE( mob->long_descr );
          mob->long_descr = STRALLOC( tmpbuf );
        }
        act( AT_IMMORT, "$N has arrived.", ch, NULL, mob, TO_ROOM );
        send_to_char( "Your elite patrol has arrived.\n\r", ch );
        mob->top_level = 105;
        for ( ability = 0 ; ability < MAX_ABILITY ; ability++ )
            mob->skill_level[ability] = mob->top_level;
        mob->hit = mob->top_level*25;
        mob->max_hit = mob->hit;
        mob->armor = 100- mob->top_level*3.5;
        mob->damroll = mob->top_level;
        mob->hitroll = mob->top_level;
        if ( ( pObjIndex = get_obj_index( OBJ_VNUM_BLASTECH_E11 ) ) != NULL )
        {
            blaster = create_object( pObjIndex, mob->top_level );
            obj_to_char( blaster, mob );
            equip_char( mob, blaster, WEAR_WIELD );                        
        }
        
        /* for making this more accurate in the future */
        
        if ( mob->mob_clan )
           STRFREE ( mob->mob_clan );
        if ( ch->pcdata && ch->pcdata->clan )   
           mob->mob_clan = STRALLOC( ch->pcdata->clan->name );
	if (ch->subclass == SUBCLASS_OFFICER)
	{
/*
	    mob->hit *= 2;
	    mob->max_hit *= 2;
	    mob->armor = -1300;
	    mob->damroll *= 2;
	    mob->hitroll *= 2;
*/

	    if (mob->hit <= 16000)
		    mob->hit *= 2;
		else
		{
			mob->hit = 32000;
			mob->max_hit = 32000;
		}
	    if (mob->hit <= 16000)
		{
		    	mob->hit *= 2;
		 	mob->max_hit *= 2;
		}
	    mob->armor = -1300;
	    if(mob->damroll <= 500)
		mob->damroll = 1000;
	    else
		mob->damroll *= 2;
	    if(mob->hitroll <= 500)
		mob->hitroll = 1000;
	    else
		mob->hitroll *= 2;


	}
        return;
     }
     if (ch->backup_mob == MOB_VNUM_PATROL )
     {
        CHAR_DATA *mob;
        int ability;
        
        mob = create_mobile( pMobIndex );
        char_to_room( mob, ch->in_room );
        if ( ch->pcdata && ch->pcdata->clan )
        {
          char tmpbuf[MAX_STRING_LENGTH];
        
          STRFREE( mob->name );
          mob->name = STRALLOC( ch->pcdata->clan->name );
          sprintf( tmpbuf , "(%s) %s" , ch->pcdata->clan->name  , mob->long_descr );
          STRFREE( mob->long_descr );
          mob->long_descr = STRALLOC( tmpbuf );
        }
        act( AT_IMMORT, "$N has arrived.", ch, NULL, mob, TO_ROOM );
        send_to_char( "Your patrol has arrived.\n\r", ch );
        mob->top_level = ch->skill_level[LEADERSHIP_ABILITY];
        for ( ability = 0 ; ability < MAX_ABILITY ; ability++ )
            mob->skill_level[ability] = mob->top_level;
        mob->hit = mob->top_level*15;
        mob->max_hit = mob->hit;
        mob->armor = 100- mob->top_level*2.5;
        mob->damroll = mob->top_level/5;
        mob->hitroll = mob->top_level/5;
        if ( ( pObjIndex = get_obj_index( OBJ_VNUM_BLASTECH_E11 ) ) != NULL )
        {
            blaster = create_object( pObjIndex, mob->top_level );
            obj_to_char( blaster, mob );
            equip_char( mob, blaster, WEAR_WIELD );                        
        }
        
        /* for making this more accurate in the future */
        
        if ( mob->mob_clan )
           STRFREE ( mob->mob_clan );
        if ( ch->pcdata && ch->pcdata->clan )   
           mob->mob_clan = STRALLOC( ch->pcdata->clan->name );
	if (ch->subclass == SUBCLASS_OFFICER)
	{
/*

	    mob->hit *= 2;
	    mob->max_hit *= 2;
	    mob->armor = -1300;
	    mob->damroll *= 2;
	    mob->hitroll *= 2;
*/
	    if (mob->hit <= 16000)
		    mob->hit *= 2;
		else
		{
			mob->hit = 32000;
			mob->max_hit = 32000;
		}
	    if (mob->hit <= 16000)
		{
		    	mob->hit *= 2;
		 	mob->max_hit *= 2;
		}
	    mob->armor = -1300;
	    if(mob->damroll <= 500)
		mob->damroll = 1000;
	    else
		mob->damroll *= 2;
	    if(mob->hitroll <= 500)
		mob->hitroll = 1000;
	    else
		mob->hitroll *= 2;


	}

        return;
     }
     else
     {
        CHAR_DATA *mob;
        int ability;
        
        mob = create_mobile( pMobIndex );
        char_to_room( mob, ch->in_room );
        if ( ch->pcdata && ch->pcdata->clan )
        {
          char tmpbuf[MAX_STRING_LENGTH];
        
          STRFREE( mob->name );
          mob->name = STRALLOC( ch->pcdata->clan->name );
          sprintf( tmpbuf , "(%s) %s" , ch->pcdata->clan->name  , mob->long_descr );
          STRFREE( mob->long_descr );
          mob->long_descr = STRALLOC( tmpbuf );
        }
        act( AT_IMMORT, "$N has arrived.", ch, NULL, mob, TO_ROOM );
        send_to_char( "Your guard has arrived.\n\r", ch );
        mob->top_level = ch->skill_level[LEADERSHIP_ABILITY];
        for ( ability = 0 ; ability < MAX_ABILITY ; ability++ )
            mob->skill_level[ability] = mob->top_level;
        mob->hit = mob->top_level*15;
        mob->max_hit = mob->hit;
        mob->armor = 100- mob->top_level*2.5;
        mob->damroll = mob->top_level/5;
        mob->hitroll = mob->top_level/5;
        if ( ( pObjIndex = get_obj_index( OBJ_VNUM_BLASTECH_E11 ) ) != NULL )
        {
            blaster = create_object( pObjIndex, mob->top_level );
            obj_to_char( blaster, mob );
            equip_char( mob, blaster, WEAR_WIELD );                        
        }
        
        /* for making this more accurate in the future */
        
        if ( mob->mob_clan )
           STRFREE ( mob->mob_clan );
        if ( ch->pcdata && ch->pcdata->clan )   
           mob->mob_clan = STRALLOC( ch->pcdata->clan->name );
	if (ch->subclass == SUBCLASS_OFFICER)
	{
/*
	    mob->hit *= 2;
	    mob->max_hit *= 2;
	    mob->armor = -1300;
	    mob->damroll *= 2;
	    mob->hitroll *= 2;
*/
	    if (mob->hit <= 16000)
		    mob->hit *= 2;
		else
		{
			mob->hit = 32000;
			mob->max_hit = 32000;
		}
	    if (mob->hit <= 16000)
		{
		    	mob->hit *= 2;
		 	mob->max_hit *= 2;
		}
	    mob->armor = -1300;
	    if(mob->damroll <= 500)
		mob->damroll = 1000;
	    else
		mob->damroll *= 2;
	    if(mob->hitroll <= 500)
		mob->hitroll = 1000;
	    else
		mob->hitroll *= 2;


	}
     }                    
}

void do_torture( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;
    int chance, dam;
    bool fail;
    
    if ( !IS_NPC(ch)
    &&  ch->pcdata->learned[gsn_torture] <= 0  )
    {
	send_to_char(
	    "Your mind races as you realize you have no idea how to do that.\n\r", ch );
	return;
    }

    if ( IS_NPC(ch) && IS_AFFECTED( ch, AFF_CHARM ) )
    {
	send_to_char( "You can't do that right now.\n\r", ch );
	return;
    }

    one_argument( argument, arg );

    if ( ch->mount )
    {
	send_to_char( "You can't get close enough while mounted.\n\r", ch );
	return;
    }

    if ( arg[0] == '\0' )
    {
	send_to_char( "Torture whom?\n\r", ch );
	return;
    }

    if ( ( victim = get_char_room( ch, arg ) ) == NULL )
    {
	send_to_char( "They aren't here.\n\r", ch );
	return;
    }

    if ( victim == ch )
    {
	send_to_char( "Are you masacistic or what...\n\r", ch );
	return;
    }
    
    if ( IS_AWAKE(victim) )
    {
	send_to_char( "Try stunning or sleeping them first!!!\n\r", ch );
	return;
    }

    if ( is_safe( ch, victim ) )
      return;

    if ( victim->fighting )
    {
	send_to_char( "You can't torture someone whos in combat.\n\r", ch );
	return;
    }
    
    ch->alignment = ch->alignment -= 100;
    ch->alignment = URANGE( -1000, ch->alignment, 1000 );
    
    WAIT_STATE( ch, skill_table[gsn_torture]->beats );
    
    fail = FALSE;
    chance = ris_save( victim, ch->skill_level[LEADERSHIP_ABILITY], RIS_PARALYSIS );
    if ( chance == 1000 )
      fail = TRUE;
    else
      fail = saves_para_petri( chance, victim );

   if ( !IS_NPC(ch) && !IS_NPC(victim) )
      chance = sysdata.stun_plr_vs_plr;
    else
      chance = sysdata.stun_regular;
    if ( !fail
    && (  IS_NPC(ch)
    || (number_percent( ) + chance) < ch->pcdata->learned[gsn_torture] ) )
    {
	learn_from_success( ch, gsn_torture );
	WAIT_STATE( ch,     2 * PULSE_VIOLENCE );
	WAIT_STATE( victim, PULSE_VIOLENCE );
	act( AT_SKILL, "$N slowly tortures you. The pain is excruciating.", victim, NULL, ch, TO_CHAR );
	act( AT_SKILL, "You torture $N, leaving $M screaming in pain.", ch, NULL, victim, TO_CHAR );
	act( AT_SKILL, "$n tortures $N, leaving $M screaming in agony!", ch, NULL, victim, TO_NOTVICT );
        
        dam = dice( ch->skill_level[LEADERSHIP_ABILITY]/10 , 4 );
        dam = URANGE( 0, victim->max_hit-10, dam ); 
        victim->hit -= dam;
        victim->max_hit -= dam;
        
        ch_printf( victim, "You lose %d permanent hit points." ,dam);
        ch_printf( ch, "They lose %d permanent hit points." , dam);
         
    }
    else
    {
	act( AT_SKILL, "$N tries to cut off your finger!", victim, NULL, ch, TO_CHAR );
	act( AT_SKILL, "You mess up big time.", ch, NULL, victim, TO_CHAR );
	act( AT_SKILL, "$n tries to painfully torture $N.", ch, NULL, victim, TO_NOTVICT );
	WAIT_STATE( ch,     2 * PULSE_VIOLENCE );
        if(!IS_NPC(ch))
        {
           if(IS_SET(ch->pcdata->flags, PCFLAG_BOUND))
              return;
        }

        global_retcode = multi_hit( victim, ch, TYPE_UNDEFINED );   
    }
    return;
    
}

void do_jail ( CHAR_DATA *ch , char *argument )
{
    CHAR_DATA *victim =NULL;
    CLAN_DATA   *clan =NULL;
    ROOM_INDEX_DATA *jail =NULL;
    
    if ( IS_NPC (ch) ) return;
    
    if ( !ch->pcdata || ( clan = ch->pcdata->clan ) == NULL )
    {
	send_to_char( "Only members of organizations can jail their enemies.\n\r", ch );
	return;
    }

    jail = get_room_index( clan->jail );
    if ( !jail && clan->mainclan )
       jail = get_room_index( clan->mainclan->jail );
    
    if ( !jail )
    {
	send_to_char( "Your orginization does not have a suitable prison.\n\r", ch );
	return;
    }

/***********
** Commented out, to make it so ppl can jail from anywhere
************
    if ( jail->area && ch->in_room->area 
    && jail->area != ch->in_room->area &&
    ( !jail->area->planet || jail->area->planet != ch->in_room->area->planet ) )
    {
         send_to_char( "Your orginizations prison is to far away.\n\r", ch );
	 return;
    }
************
** End COmmenting
***********/
        
    if ( ch->mount )
    {
	send_to_char( "You can't do that while mounted.\n\r", ch );
	return;
    }

    if ( argument[0] == '\0' )
    {
	send_to_char( "Jail who?\n\r", ch );
	return;
    }

    if ( ( victim = get_char_room( ch, argument ) ) == NULL )
    {
	send_to_char( "They aren't here.\n\r", ch );
	return;
    }

    if ( victim == ch )
    {
	send_to_char( "That's pointless.\n\r", ch );
	return;
    }

    if ( IS_NPC(victim) )
    {
	send_to_char( "That would be a waste of time.\n\r", ch );
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
    
    if ( ch->position <= POS_SLEEPING )
    {
        send_to_char( "In your dreams or what?\n\r" , ch );
        return;
    }
    if(!IS_SET(victim->pcdata->flags, PCFLAG_BOUND))
    {
       if ( victim->position >= POS_SLEEPING )
       {
           send_to_char( "You will have to stun or bind them first.\n\r" , ch );
           return;
       }
    }
	send_to_char( "You have them escorted off to jail.\n\r", ch );
	act( AT_ACTION, "You wake up in a jail cell.\n\r", ch, NULL, victim, TO_VICT    );
	act( AT_ACTION, "$n has $N escorted away.\n\r",  ch, NULL, victim, TO_NOTVICT );

    char_from_room ( victim );
    char_to_room ( victim , jail );

    act( AT_ACTION, "The door opens briefly as $n is shoved into the room.\n\r",  victim, NULL, NULL, TO_ROOM );

    learn_from_success( ch , gsn_jail );
        gain_exp( ch,2000000, LEADERSHIP_ABILITY);
    
    return;        
}



