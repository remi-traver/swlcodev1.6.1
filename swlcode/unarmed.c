/***************************************************************************

Star Wars Life 3.0a
Unarmed Combat/Martial Arts file

For credits see 'Help Credits' in game.

(c) 2001, 2002; FRPG Ltd.
All rights reserved

****************************************************************************/

#include <sys/types.h>
#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/dir.h>
#include "mud.h"

//from skills.c
void learn_style (CHAR_DATA *ch );

//from fight.c
ch_ret damage (CHAR_DATA *ch, CHAR_DATA *victim, int dam, int dt);
sh_int ris_damage( CHAR_DATA *ch, sh_int dam, int ris );

//ok first a preliminary check to see how many combo attacks will hit.

ch_ret combo_kick (CHAR_DATA *ch, CHAR_DATA *victim)
{
   ch_ret retcode;
   char buf[MAX_STRING_LENGTH];
   int dam;



    /*
     * Can't beat a dead char!
     * Guard against weird room-leavings.
     */
    if ( victim->position == POS_DEAD || ch->in_room != victim->in_room )
	return rVICT_DIED;


    dam = number_range( ch->barenumdie, ch->baresizedie * ch->barenumdie );

    
    dam += GET_DAMROLL(ch);
    
    if ( !IS_NPC(ch) && ch->pcdata->learned[gsn_enhanced_damage] > 0 )
    {
	dam += (int) (dam * ch->pcdata->learned[gsn_enhanced_damage] / 120);
	learn_from_success( ch, gsn_enhanced_damage );
    }
    if ( !IS_NPC(ch) && ch->pcdata->learned[gsn_unarmed] > 0 )
    {
	dam *= ( 1 + ch->pcdata->learned[gsn_unarmed] / 100 );
	learn_from_success( ch, gsn_unarmed );
    }
    dam = ris_damage( victim, dam, RIS_NONMAGIC );
    if ( dam <= 0 )
	dam = 1.5;

    if (ch->fstyle != 0 )
    {
	if ( number_percent() < 4 )
		learn_style(ch);
    }

    //combo damage mod
    dam *= 3.5;

    if ( GET_DAMAGE_RESISTANCE(victim) > 0)
    {
	dam = ( dam * ( 100 - UMIN(GET_DAMAGE_RESISTANCE(victim), 80 ) ) / 100 );
    }

    sprintf( buf, "&YYou kick %s&w\n\r", victim->name ); 
    send_to_char( buf, ch );

    if ( !IS_NPC(victim) )
    {
	sprintf( buf, "&Y%s kicks you!&w\n\r", ch->name ); 
	send_to_char( buf, victim );
    }
    if ( (retcode = damage( ch, victim, dam, TYPE_HIT )) != rNONE )
      return retcode;
    if ( char_died(ch) )
      return rCHAR_DIED;
    if ( char_died(victim) )
      return rVICT_DIED;

    retcode = rNONE;
    if ( dam == 0 )
      return retcode;

    tail_chain( );
    return rNONE;
}

ch_ret combo_jab (CHAR_DATA *ch, CHAR_DATA *victim)
{
   ch_ret retcode;
   char buf[MAX_STRING_LENGTH];
   int dam;



    /*
     * Can't beat a dead char!
     * Guard against weird room-leavings.
     */
    if ( victim->position == POS_DEAD || ch->in_room != victim->in_room )
	return rVICT_DIED;


    dam = number_range( ch->barenumdie, ch->baresizedie * ch->barenumdie );

    
    dam += GET_DAMROLL(ch);
    
    if ( !IS_NPC(ch) && ch->pcdata->learned[gsn_enhanced_damage] > 0 )
    {
	dam += (int) (dam * ch->pcdata->learned[gsn_enhanced_damage] / 120);
	learn_from_success( ch, gsn_enhanced_damage );
    }
    if ( !IS_NPC(ch) && ch->pcdata->learned[gsn_unarmed] > 0 )
    {
	dam *= ( 1 + ch->pcdata->learned[gsn_unarmed] / 100 );
	learn_from_success( ch, gsn_unarmed );
    }
    dam = ris_damage( victim, dam, RIS_NONMAGIC );
    if ( dam <= 0 )
	dam = 1;

    if (ch->fstyle != 0 )
    {
	if ( number_percent() < 4 )
		learn_style(ch);
    }

    //combo damage mod
    dam *= 2.5;


    if ( GET_DAMAGE_RESISTANCE(victim) > 0)
    {
	dam = ( dam * ( 100 - UMIN(GET_DAMAGE_RESISTANCE(victim), 80 ) ) / 100 );
    }

    sprintf( buf, "&YYou jab %s&w\n\r", victim->name ); 
    send_to_char( buf, ch );

    if ( !IS_NPC(victim) )
    {
	sprintf( buf, "&Y%s jabs you!&w\n\r", ch->name ); 
	send_to_char( buf, victim );
    }
    if ( (retcode = damage( ch, victim, dam, TYPE_HIT)) != rNONE )
      return retcode;
    if ( char_died(ch) )
      return rCHAR_DIED;
    if ( char_died(victim) )
      return rVICT_DIED;

    retcode = rNONE;
    if ( dam == 0 )
      return retcode;

    tail_chain( );
    return retcode;
}

ch_ret combo_punch (CHAR_DATA *ch, CHAR_DATA *victim)
{
   ch_ret retcode;
   char buf[MAX_STRING_LENGTH];
   int dam;



    /*
     * Can't beat a dead char!
     * Guard against weird room-leavings.
     */
    if ( victim->position == POS_DEAD || ch->in_room != victim->in_room )
	return rVICT_DIED;


    dam = number_range( ch->barenumdie, ch->baresizedie * ch->barenumdie );

    
    dam += GET_DAMROLL(ch);
    
    if ( !IS_NPC(ch) && ch->pcdata->learned[gsn_enhanced_damage] > 0 )
    {
	dam += (int) (dam * ch->pcdata->learned[gsn_enhanced_damage] / 120);
	learn_from_success( ch, gsn_enhanced_damage );
    }
    if ( !IS_NPC(ch) && ch->pcdata->learned[gsn_unarmed] > 0 )
    {
	dam *= ( 1 + ch->pcdata->learned[gsn_unarmed] / 100 );
	learn_from_success( ch, gsn_unarmed );
    }
    dam = ris_damage( victim, dam, RIS_NONMAGIC );
    if ( dam <= 0 )
	dam = 1;

    if (ch->fstyle != 0 )
    {
	if ( number_percent() < 4 )
		learn_style(ch);
    }

    //combo damage mod
    dam *= 3;


    if ( GET_DAMAGE_RESISTANCE(victim) > 0)
    {
	dam = ( dam * ( 100 - UMIN(GET_DAMAGE_RESISTANCE(victim), 80) ) / 100 );
    }

    sprintf( buf, "&YYou punch %s&w\n\r", victim->name ); 
    send_to_char( buf, ch );

    if ( !IS_NPC(victim) )
    {
	sprintf( buf, "&Y%s punches you!&w\n\r", ch->name ); 
	send_to_char( buf, victim );
    }
    if ( (retcode = damage( ch, victim, dam, TYPE_HIT )) != rNONE )
      return retcode;
    if ( char_died(ch) )
      return rCHAR_DIED;
    if ( char_died(victim) )
      return rVICT_DIED;

    retcode = rNONE;
    if ( dam == 0 )
      return retcode;

    tail_chain( );
    return retcode;
}

ch_ret combo_elbow (CHAR_DATA *ch, CHAR_DATA *victim)
{
   ch_ret retcode;
   char buf[MAX_STRING_LENGTH];
   int dam;

    /*
     * Can't beat a dead char!
     * Guard against weird room-leavings.
     */
    if ( victim->position == POS_DEAD || ch->in_room != victim->in_room )
	return rVICT_DIED;


    dam = number_range( ch->barenumdie, ch->baresizedie * ch->barenumdie );

    
    dam += GET_DAMROLL(ch);
    
    if ( !IS_NPC(ch) && ch->pcdata->learned[gsn_enhanced_damage] > 0 )
    {
	dam += (int) (dam * ch->pcdata->learned[gsn_enhanced_damage] / 120);
	learn_from_success( ch, gsn_enhanced_damage );
    }
    if ( !IS_NPC(ch) && ch->pcdata->learned[gsn_unarmed] > 0 )
    {
	dam *= ( 1 + ch->pcdata->learned[gsn_unarmed] / 100 );
	learn_from_success( ch, gsn_unarmed );
    }
    dam = ris_damage( victim, dam, RIS_NONMAGIC );
    if ( dam <= 0 )
	dam = 1;

    if (ch->fstyle != 0 )
    {
	if ( number_percent() < 4 )
		learn_style(ch);
    }
    //combo damage mod
    dam *= 3.5;


    if ( GET_DAMAGE_RESISTANCE(victim) > 0)
    {
	dam = ( dam * ( 100 - UMIN(GET_DAMAGE_RESISTANCE(victim), 80 ) ) / 100 );
    }

    sprintf( buf, "&YYou elbow %s in the back!&w\n\r", victim->name ); 
    send_to_char( buf, ch );



    if ( !IS_NPC(victim) )
    {
	sprintf( buf, "&Y%s elbows you in the back!&w\n\r", ch->name ); 
	send_to_char( buf, victim );
    }
    if ( (retcode = damage( ch, victim, dam, TYPE_HIT )) != rNONE )
      return retcode;
    if ( char_died(ch) )
      return rCHAR_DIED;
    if ( char_died(victim) )
      return rVICT_DIED;

    retcode = rNONE;
    if ( dam == 0 )
      return retcode;

    tail_chain( );
    return retcode;
}

ch_ret combo_knee (CHAR_DATA *ch, CHAR_DATA *victim)
{
   ch_ret retcode;
   char buf[MAX_STRING_LENGTH];
   int dam;

    /*
     * Can't beat a dead char!
     * Guard against weird room-leavings.
     */
    if ( victim->position == POS_DEAD || ch->in_room != victim->in_room )
	return rVICT_DIED;


    dam = number_range( ch->barenumdie, ch->baresizedie * ch->barenumdie );

    
    dam += GET_DAMROLL(ch);
    
    if ( !IS_NPC(ch) && ch->pcdata->learned[gsn_enhanced_damage] > 0 )
    {
	dam += (int) (dam * ch->pcdata->learned[gsn_enhanced_damage] / 120);
	learn_from_success( ch, gsn_enhanced_damage );
    }
    if ( !IS_NPC(ch) && ch->pcdata->learned[gsn_unarmed] > 0 )
    {
	dam *= ( 1 + ch->pcdata->learned[gsn_unarmed] / 100 );
	learn_from_success( ch, gsn_unarmed );
    }
    dam = ris_damage( victim, dam, RIS_NONMAGIC );
    if ( dam <= 0 )
	dam = 1;

    if (ch->fstyle != 0 )
    {
	if ( number_percent() < 4 )
		learn_style(ch);
    }
    //combo damage mod
    dam *= 6.5;

    if ( GET_DAMAGE_RESISTANCE(victim) > 0)
    {
	dam = ( dam * ( 100 - UMIN(GET_DAMAGE_RESISTANCE(victim), 80 ) ) / 100 );
    }

    sprintf( buf, "&YYou knee %s in the groin!&w\n\r", victim->name ); 
    send_to_char( buf, ch );

    if ( !IS_NPC(victim) )
    {
	sprintf( buf, "&Y%s knees you in the groin!&w\n\r", ch->name ); 
	send_to_char( buf, victim );
    }
    if ( (retcode = damage( ch, victim, dam, TYPE_HIT )) != rNONE )
      return retcode;
    if ( char_died(ch) )
      return rCHAR_DIED;
    if ( char_died(victim) )
      return rVICT_DIED;

    retcode = rNONE;
    if ( dam == 0 )
      return retcode;

    tail_chain( );
    return retcode;
}



ch_ret combo_finish (CHAR_DATA *ch, CHAR_DATA *victim)
{
   ch_ret retcode;
   char buf[MAX_STRING_LENGTH];
   int dam;
    AFFECT_DATA af;



    /*
     * Can't beat a dead char!
     * Guard against weird room-leavings.
     */
    if ( victim->position == POS_DEAD || ch->in_room != victim->in_room )
	return rVICT_DIED;

    sprintf( buf, "&YYou uppercut %s!&w\n\r", victim->name ); 
    send_to_char( buf, ch );

    if ( !IS_NPC(victim) )
    {
	sprintf( buf, "&Y%s gives you a devestating uppercut&w\n\r", ch->name ); 
	send_to_char( buf, victim );
    }

    if ( number_percent() < 8 )
{
    sprintf( buf, "%s goes down under the blow!!&w\n\r", victim->name ); 
    send_to_char( buf, ch );

    if ( !IS_NPC(victim) )
    {
	sprintf( buf, "&Y%s's blow knocks you out!&w\n\r", ch->name ); 
	send_to_char( buf, victim );
    }

		stop_fighting( victim, TRUE );
		if ( !IS_AFFECTED( victim, AFF_PARALYSIS ) )
		{
	  	   af.type      = gsn_stun;
	  	   af.location  = APPLY_AC;
	  	   af.modifier  = 2000;
	  	   af.duration  = 14;
	  	   af.bitvector = AFF_PARALYSIS;
	  	   affect_to_char( victim, &af );
	  	   update_pos( victim );
	  	   if ( IS_NPC(victim) )
	  	   {
	  	       start_hating( victim, ch );
	  	       start_hunting( victim, ch );
	  	       victim->was_stunned = 10;
	  	   }
		}

}

//	dam = GET_DAMCAP(ch);

    if ( (retcode = damage( ch, victim, dam, TYPE_HIT )) != rNONE )
      return retcode;
    if ( char_died(ch) )
      return rCHAR_DIED;
    if ( char_died(victim) )
      return rVICT_DIED;

    retcode = rNONE;
    if ( dam == 0 )
      return retcode;

    tail_chain( );
    return retcode;
}



bool check_combohit (CHAR_DATA *ch, CHAR_DATA *victim)
{
    int victim_ac;
    int diceroll;
    int thac0;
    int thac0_00;
    int thac0_32;
  


    //calculate thac0
    thac0_00 = 20;
    thac0_32 = 10;
    thac0     = interpolate( ch->skill_level[COMBAT_ABILITY] , thac0_00, thac0_32 ) - GET_HITROLL(ch);

    
    victim_ac = (int) (GET_AC(victim) / 150);

	// caps AC at the standard D&d  max of -10 and 10
	if (victim_ac > 10)
		victim_ac = 10;
	if (victim_ac < -10 )
		victim_ac = -10;

    /* Give tired people a penalty to hit in a bonus to enemies AC */
    if ( ch->move <= ( 50 ) && !IS_NPC(ch))
    {
	act( AT_YELLOW, "You are FAR to tired to effectively fight", ch, NULL, victim, TO_CHAR    );                   
        victim_ac -= 4;
    }
    if ( victim->move <= ( 50 ) && !IS_NPC(victim))
    {
	act( AT_YELLOW, "You are FAR to tired to effectively fight", victim, NULL, ch, TO_CHAR    );                   
        victim_ac += 4;
    }

    /* if you can't see what's coming... */
    if ( !can_see( ch, victim ) )
       victim_ac += 4;

    if ( ch->race == RACE_DEFEL )
        victim_ac += 2; 

    if ( !IS_AWAKE ( victim ) )
        victim_ac += 5;
    


	// second AC Cap -- after all modifications
	if (victim_ac > 10)
		victim_ac = 10;
	if (victim_ac < -10 )
		victim_ac = -10;


    diceroll = number_range( 1,20 );

    if ( diceroll == 1
    || ( diceroll < 20 && diceroll < thac0 - victim_ac ) )
    {
	//missed!
	return FALSE;
    }
    return TRUE;
}



ch_ret combo_hit(CHAR_DATA *ch, CHAR_DATA *victim)
{
  int maxhits;
  int hits;
  int loopvariable;
  int hittype;
  int finishertype;
  ch_ret retcode;
  char log_buf[MAX_STRING_LENGTH];

  if (ch->subclass == SUBCLASS_MARTIST)
	maxhits = 12;
  else if (ch->race == RACE_SPACEDRAGON)
	maxhits = 4;
  else
	maxhits = 3;

	hits = 0;

  for ( loopvariable=1; loopvariable <= maxhits; loopvariable++)
  {
	if (!check_combohit(ch, victim))
		break;
	else
		hits++;
  }

  
  if( hits == 0)
   {
	retcode = damage( ch, victim, 0, TYPE_HIT );
	return retcode;
}
  //apply all hits that hit, choosing randomly between the 'standard' 5
  for (loopvariable=hits; loopvariable > 0; loopvariable--)
  {
	hittype = number_percent();
	if ( hittype <= 30 )
	{
            retcode = combo_kick( ch, victim);
            if ( retcode != rNONE || who_fighting( ch ) != victim )
	            return retcode;
	}	
	else if ( hittype <= 60 && hittype >= 31)
	{
            retcode = combo_jab( ch, victim);
            if ( retcode != rNONE || who_fighting( ch ) != victim )
	            return retcode;
	}	
	else if ( hittype <= 75 && hittype >= 61)
	{
            retcode = combo_punch( ch, victim);
            if ( retcode != rNONE || who_fighting( ch ) != victim )
	            return retcode;
	}	
	else if ( hittype <= 90 && hittype >= 76)
	{
            retcode = combo_elbow( ch, victim);
            if ( retcode != rNONE || who_fighting( ch ) != victim )
	            return retcode;
	}	
	else if ( hittype >= 91)
	{
            retcode = combo_elbow( ch, victim);
            if ( retcode != rNONE || who_fighting( ch ) != victim )
	            return retcode;
	}	
	else
	{
		strcpy(log_buf, "BUG: combo_hit (unarmed.c) - Invalid Normal Hittype");
		log_string(log_buf);
		bug(log_buf, 0);
	}
  }


  //if all combohits hit, then apply a bonus 'finishing' hit
  if (hits == maxhits)
  {
      retcode = combo_finish( ch, victim);
      if ( retcode != rNONE || who_fighting( ch ) != victim )
	 return retcode;
  }
//		strcpy(log_buf, "BUG: combo_hit (unarmed.c) - Combo Ends");
//		log_string(log_buf);

  tail_chain( );
  return retcode;

}

