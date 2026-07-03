/***************************************************************************

Star Wars Life 3.0a
Diplomatic Skills

For credits see 'help credits' in game

(c) 2001, 2002 FRPG Inc.
All Rights Reserved

****************************************************************************/

#include <math.h> 
#include <sys/types.h>
#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include "mud.h"


void do_smalltalk ( CHAR_DATA *ch , char *argument )
{
    char buf  [MAX_STRING_LENGTH];
    char arg1 [MAX_INPUT_LENGTH];
    CHAR_DATA *victim = NULL;
    PLANET_DATA *planet = NULL;
    CLAN_DATA   *clan = NULL;
    int percent, xp;
    
    if ( IS_NPC(ch) || !ch->pcdata || !ch->pcdata->clan || !ch->in_room->area || !ch->in_room->area->planet )
    {
        send_to_char( "What would be the point of that.\n\r", ch );
        return;
    }
    
    argument = one_argument( argument, arg1 );

    if ( ch->mount )
    {
	send_to_char( "You can't do that while mounted.\n\r", ch );
	return;
    }

    if ( arg1[0] == '\0' )
    {
	send_to_char( "Create smalltalk with whom?\n\r", ch );
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
    

    if ( !IS_NPC(victim) || victim->vip_flags == 0 )
    {
        send_to_char( "Diplomacy would be wasted on them.\n\r" , ch );
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

    WAIT_STATE( ch, skill_table[gsn_smalltalk]->beats );

    if ( percent - ch->skill_level[DIPLOMACY_ABILITY] + victim->top_level > ch->pcdata->learned[gsn_smalltalk]  ) 
    {
	/*
	 * Failure.
	 */
	send_to_char( "You attempt to make smalltalk with them.. but are ignored.\n\r", ch );
	act( AT_ACTION, "$n is really getting on your nerves with all this chatter!\n\r", ch, NULL, victim, TO_VICT    );
	act( AT_ACTION, "$n asks $N about the weather but is ignored.\n\r",  ch, NULL, victim, TO_NOTVICT );

        if ( victim->alignment < -500 && victim->top_level >= ch->top_level+5 )
	{
	  sprintf( buf, "SHUT UP %s!", ch->name );
	  do_yell( victim, buf );
          global_retcode = multi_hit( victim, ch, TYPE_UNDEFINED );
	}
	
	return;
    }
    
    send_to_char( "You strike up a short conversation with them.\n\r", ch );
    act( AT_ACTION, "$n smiles at you and says, 'hello'.\n\r", ch, NULL, victim, TO_VICT    );
    act( AT_ACTION, "$n chats briefly with $N.\n\r",  ch, NULL, victim, TO_NOTVICT );
    
    if ( IS_NPC(ch) || !ch->pcdata || !ch->pcdata->clan || !ch->in_room->area || !ch->in_room->area->planet )
      return;
    
    if ( ( clan = ch->pcdata->clan->mainclan ) == NULL )
       clan = ch->pcdata->clan;
       
    planet = ch->in_room->area->planet;
    
    if ( clan != planet->governed_by )
       return;
       
    planet->pop_support += 0.2;
    send_to_char( "Popular support for your organization increases slightly.\n\r", ch );
    xp = 1000 + victim->top_level * 50, DIPLOMACY_ABILITY;
    xp = UMIN(xp, exp_level(ch->skill_level[DIPLOMACY_ABILITY]+1) - exp_level(ch->skill_level[DIPLOMACY_ABILITY]));         
    gain_exp(ch, xp, DIPLOMACY_ABILITY);
    ch_printf( ch , "You gain %d diplomacy experience.\n\r", xp );
    
    learn_from_success( ch, gsn_smalltalk );
        
    if ( planet->pop_support > 100 )
        planet->pop_support = 100;
}

void do_propeganda ( CHAR_DATA *ch , char *argument )
{
    char buf  [MAX_STRING_LENGTH];
    char arg1 [MAX_INPUT_LENGTH];
    CHAR_DATA *victim;
    PLANET_DATA *planet;
    CLAN_DATA   *clan;
    int percent, xp;
    
   if ( IS_NPC(ch) || !ch->pcdata || !ch->pcdata->clan || !ch->in_room->area || !ch->in_room->area->planet )
   {
       send_to_char( "What would be the point of that.\n\r", ch );
       return;
   }
    
    argument = one_argument( argument, arg1 );

    if ( ch->mount )
    {
	send_to_char( "You can't do that while mounted.\n\r", ch );
	return;
    }

    if ( arg1[0] == '\0' )
    {
	send_to_char( "Spread propeganda to who?\n\r", ch );
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
    

    if ( victim->vip_flags == 0 )
    {
        send_to_char( "Diplomacy would be wasted on them.\n\r" , ch );
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

    if ( ( clan = ch->pcdata->clan->mainclan ) == NULL )
       clan = ch->pcdata->clan;
       
    planet = ch->in_room->area->planet;
        
    sprintf( buf, ", and the evils of %s" , planet->governed_by ? planet->governed_by->name : "their current leaders" );
    ch_printf( ch, "You speak to them about the benifits of the %s%s.\n\r", ch->pcdata->clan->name,
        planet->governed_by == clan ? "" : buf );
    act( AT_ACTION, "$n speaks about his organization.\n\r", ch, NULL, victim, TO_VICT    );
    act( AT_ACTION, "$n tells $N about their organization.\n\r",  ch, NULL, victim, TO_NOTVICT );

    WAIT_STATE( ch, skill_table[gsn_propeganda]->beats );

    if ( percent - get_curr_cha(ch) + victim->top_level > ch->pcdata->learned[gsn_propeganda]  ) 
    {

        if ( planet->governed_by != clan )
	{
	  sprintf( buf, "%s is a traitor!" , ch->name);
	  do_yell( victim, buf );
          global_retcode = multi_hit( victim, ch, TYPE_UNDEFINED );
	}
	
	return;
    }
    
    if ( planet->governed_by == clan )
    { 
	if (ch->subclass == SUBCLASS_SENATOR)
       planet->pop_support += .5 + ch->top_level/50 * 2 ;
	else
       planet->pop_support += .5 + ch->top_level/50;
       send_to_char( "Popular support for your organization increases.\n\r", ch );
    }     
    else
    {
	if (ch->subclass == SUBCLASS_SENATOR)
       planet->pop_support -= .5 + ch->top_level/50 * 2 ;
	else
       planet->pop_support -= .5 + ch->top_level/50;
       send_to_char( "Popular support for the current government decreases.\n\r", ch );
    }

    xp = 2500 + victim->top_level * 100;
    xp = UMIN(xp, exp_level(ch->skill_level[DIPLOMACY_ABILITY]+1) - exp_level(ch->skill_level[DIPLOMACY_ABILITY]));    
    gain_exp(ch, xp, DIPLOMACY_ABILITY);
    ch_printf( ch , "You gain %d diplomacy experience.\n\r", xp );
    
    learn_from_success( ch, gsn_propeganda );
        
    if ( planet->pop_support > 100 )
        planet->pop_support = 100;
    if ( planet->pop_support < -100 )
        planet->pop_support = -100;

}

void do_bribe ( CHAR_DATA *ch , char *argument )
{
    char arg1 [MAX_INPUT_LENGTH];
    CHAR_DATA *victim;
    PLANET_DATA *planet;
    CLAN_DATA   *clan;
    int percent, amount;
    
    if ( IS_NPC(ch) || !ch->pcdata || !ch->pcdata->clan || !ch->in_room->area || !ch->in_room->area->planet )
    {
      send_to_char( "What would be the point of that.\n\r", ch );
      return;
    }
    
    argument = one_argument( argument, arg1 );

    if ( ch->mount )
    {
	send_to_char( "You can't do that while mounted.\n\r", ch );
	return;
    }

    if ( argument[0] == '\0' )
    {
	send_to_char( "Bribe who how much?\n\r", ch );
	return;
    }

    amount = atoi( argument );
    
    if(amount > ch->gold)
    {
       send_to_char("You don't have that much to bribe with.\n\r", ch);
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

    if ( amount <= 0 )
    {
       	send_to_char( "A little bit more money would be a good plan.\n\r", ch );
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

    if ( victim->vip_flags == 0 )
    {
        send_to_char( "Diplomacy would be wasted on them.\n\r" , ch );
        return;
    }
    
    ch->gold -= amount;
/* Old version, players shouldn't get money back by killing 
    victim->gold += amount; 
    Give it to the economy instead. */ 
    boost_economy(ch->in_room->area, amount);

	ch_printf( ch, "You give them a small gift on behalf of %s.\n\r", ch->pcdata->clan->name );
	act( AT_ACTION, "$n offers you a small bribe.\n\r", ch, NULL, victim, TO_VICT    );
	act( AT_ACTION, "$n gives $N some money.\n\r",  ch, NULL, victim, TO_NOTVICT );

    if ( !IS_NPC( victim ) )
	return;
    
    WAIT_STATE( ch, skill_table[gsn_bribe]->beats );

    if ( percent - amount + victim->top_level > ch->pcdata->learned[gsn_bribe]  ) 
    {
       learn_from_failure(ch, gsn_bribe);
       return;
    }       
    if ( ( clan = ch->pcdata->clan->mainclan ) == NULL )
       clan = ch->pcdata->clan;
       
    planet = ch->in_room->area->planet;
        
    if ( clan == planet->governed_by )
    {
      planet->pop_support += URANGE( 0.1 , amount/1000 , 2 );
      send_to_char( "Popular support for your organization increases slightly.\n\r", ch );
      if ( planet->pop_support > 100 )
        planet->pop_support = 100;
    }
    else
    {
      planet->pop_support -= URANGE( 0.1, amount/1000 , 2 );
      send_to_char( "Popular support for the current government decreases slightly.\n\r", ch);
      if ( planet->pop_support < -100)
         planet->pop_support = -100;
    }
    amount = UMIN(amount, exp_level(ch->skill_level[DIPLOMACY_ABILITY]+1) - exp_level(ch->skill_level[DIPLOMACY_ABILITY]));
    gain_exp(ch, amount , DIPLOMACY_ABILITY);
    ch_printf( ch , "You gain %d diplomacy experience.\n\r", amount );
    learn_from_success( ch, gsn_bribe );
    return;
}

void do_seduce ( CHAR_DATA *ch , char *argument )
{
    send_to_char( "This command does not work, please do NOT use it!!!!\n\r", ch);
    return;
}

void do_mass_propeganda ( CHAR_DATA *ch , char *argument )
{
    char buf [MAX_STRING_LENGTH];
    char arg1 [MAX_INPUT_LENGTH];
    CHAR_DATA *victim;
    PLANET_DATA *planet;
    CLAN_DATA   *clan;
    int percent, xp;

   if ( IS_NPC(ch) || !ch->pcdata || !ch->pcdata->clan || !ch->in_room->area || !ch->in_room->area->planet )
   {
      send_to_char( "What would be the point of that?\n\r", ch );
      return;
   }

    argument = one_argument( argument, arg1 );

    if ( ch->mount )
    {
      send_to_char( "You can't do that while mounted.\n\r", ch );
      return;
    }

    if ( arg1[0] == '\0' )
    {
      send_to_char( "Spread mass propeganda to whom?\n\r", ch );
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

    if ( ch->position == POS_FIGHTING )
    {
      send_to_char( "Interesting combat technique.\n\r", ch );
      return;
    }

    if ( victim->position == POS_FIGHTING )
    {
       send_to_char( "They're a little busy right now.\n\r" , ch );
       return;
    }

    if ( victim->vip_flags == 0 )
    {
       send_to_char( "Diplomacy would be wasted on them.\n\r" , ch );
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

    if ( ( clan = ch->pcdata->clan->mainclan ) == NULL )
       clan = ch->pcdata->clan;

    planet = ch->in_room->area->planet;

    sprintf( buf, ", and the evils of %s" , planet->governed_by ? planet->governed_by->name : "their current leaders" );
    ch_printf( ch, "You speak to them about the benifits of the %s%s.\n\r", ch->pcdata->clan->name,
        planet->governed_by == clan ? "" : buf );
    act( AT_ACTION, "$n speaks about his organization.\n\r", ch, NULL, victim, TO_VICT    );
    act( AT_ACTION, "$n tells $N about their organization.\n\r",  ch, NULL, victim, TO_NOTVICT );

    WAIT_STATE( ch, skill_table[gsn_masspropeganda]->beats );

    if ( percent - get_curr_cha(ch) + victim->top_level > ch->pcdata->learned[gsn_masspropeganda]   )
    {

	if ( planet->governed_by != clan )
       {
	sprintf( buf, "%s is a traitor!" , ch->name);
	do_yell( victim, buf );
	  global_retcode = multi_hit( victim, ch, TYPE_UNDEFINED );
       }

       return;
    }

    if ( planet->governed_by == clan )
    {
	if (ch->subclass == SUBCLASS_SENATOR)
      planet->pop_support += 5 + ch->top_level/50 * 2;
	else
      planet->pop_support += 5 + ch->top_level/50;
      send_to_char( "Popular support for your organization increases.\n\r", ch );
    }
    else
    {
	if (ch->subclass == SUBCLASS_SENATOR)
      planet->pop_support -= 5 + ch->top_level/50 * 2;
	else
      planet->pop_support -= 5 - ch->top_level/50;
      send_to_char( "Popular support for the current government decreases.\n\r", ch );
    }

    xp = 5000 + victim->top_level * 200;
    xp = UMIN(xp, exp_level(ch->skill_level[DIPLOMACY_ABILITY]+1) - exp_level(ch->skill_level[DIPLOMACY_ABILITY]));
    gain_exp(ch, xp, DIPLOMACY_ABILITY);    
    ch_printf( ch , "You gain %d diplomacy experience.\n\r", xp );

    learn_from_success( ch, gsn_masspropeganda );

    if ( planet->pop_support > 100 )
	planet->pop_support = 100;
    if ( planet->pop_support < -100 )
	planet->pop_support = -100;
}

void do_negotiate( CHAR_DATA *ch, char *argument ) /* Made by Charles July '02 */
{
    CHAR_DATA *victim;
    AFFECT_DATA *paf;
    SKILLTYPE *skill;
    int credits;

    if ( ( victim = who_fighting( ch ) ) == NULL )
    {
        send_to_char( "&c&CYou aren't fighting anyone.\n\r", ch );
        return;
    }
    if ( IS_AFFECTED(victim, AFF_BERSERK) )
    {
       send_to_char( "&c&CYou cannot persuade them, they are in a berserking rage", ch );
       return;
    }
    if ( !IS_NPC( victim ) )
    {
       credits = victim->skill_level[COMBAT_ABILITY] * 1500;  //1.5k per combat level.//
    }
    if ( IS_NPC( victim ) )
    {
       credits = 10000; //mobs u pay 10k//
    }
    if ( ch->gold < credits )
    {
       send_to_char( "&c&CYou think smiling will make him stop fighting?", ch );
       return;
    }
    for (paf = victim->first_affect; paf; paf = paf->next)
    {
       if ( (skill=get_skilltype(paf->type)) != NULL )
       {
         if ( !str_cmp( skill->name, "rage" ) )
           send_to_char( "&c&CThey are In a rage of fury! They won't bend to your negotiations", ch );
           return;
       }
    }
    WAIT_STATE( ch, skill_table[gsn_negotiate]->beats );
    if ( IS_NPC(ch) || number_percent( ) < ch->pcdata->learned[gsn_negotiate] )
    {
        stop_fighting( victim, TRUE );
        act( AT_LBLUE, "You negotiate with $N, with credits.",  ch, NULL, victim, TO_CHAR    );
        act( AT_LBLUE, "$n negotiates with you, with credits.", ch, NULL, victim, TO_VICT    );
        act( AT_LBLUE, "$n negotiates with $N.",  ch, NULL, victim, TO_NOTVICT );
        ch->gold -= credits;
        victim->gold += credits;
        gain_exp(ch, victim->top_level * 150, DIPLOMACY_ABILITY);
        ch_printf( ch , "You gain %d diplomacy experience.\n\r", victim->top_level*150);   
        learn_from_success( ch, gsn_negotiate );
        stop_hating( victim );
        stop_hunting( victim );
        stop_fearing( victim );
    }
    else
    {
        act( AT_LBLUE, "You failed the negotiations with $N.",  ch, NULL, victim, TO_CHAR    );
        act( AT_LBLUE, "$n negotiate with you but does it poorly don't you think?", ch, NULL, victim, TO_VICT    );
        act( AT_LBLUE, "$n fails the negotiations with $N.",  ch, NULL, victim, TO_NOTVICT );
        learn_from_failure( ch, gsn_negotiate );
    }
    return;
}

void do_gather_intelligence(CHAR_DATA *ch, char *argument)
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
           : (int) (ch->pcdata->learned[gsn_gather_intelligence]);
    WAIT_STATE(ch, PULSE_VIOLENCE);
    if (  number_percent() >= chance )
    {
        ch_printf(ch, "You fail to gather intelligence on %s.\n\r", NAME(victim) );
        learn_from_failure(ch, gsn_gather_intelligence);
        return;
    }

    if ( victim->subclass == SUBCLASS_STEALTH_HUNT || victim->subclass == SUBCLASS_SNEAK )
    {
        ch_printf(ch, "It is impossible gather intelligence on %s.\n\r", NAME(victim));
        learn_from_success(ch, gsn_gather_intelligence);
        return;
    }
    if ( IS_IMMORTAL(victim) && (victim->top_level > ch->top_level ) )
    {
       af.type      = gsn_gather_intelligence;
       af.location  = APPLY_HITROLL;
       af.modifier  = -666;
       af.duration  = 32000;
       af.bitvector = AFF_BLIND;
       affect_to_char( ch, &af );
       set_char_color( AT_MAGIC, victim );
       send_to_char( "You are blinded by your target's immortal aura!\n\r", ch );
       return;
    }
    ch_printf(ch, "&wYour intelligence sources have reported the following information:\n\r");
    ch_printf(ch, "&wTitle: %0.74s&w\n\r", victim->pcdata->title);
    ch_printf(ch, "&wRace: %0.20s Class: %0.20s Subclass: %0.20s\n\r", npc_race[victim->race], 
ability_name[victim->main_ability], subclasses[victim->subclass]);
    if(victim->pcdata->clan)
        ch_printf(ch, "Affiliation: %0.20s\n\r", victim->pcdata->clan_name);
    ch_printf(ch, "Hitpoints: %5.5d/%5.5d Movement: %5.5d/%5.5d\n\r", victim->hit, victim->max_hit, victim->move, 
victim->max_move);
    ch_printf(ch, "Armor: %4.4d Hitroll: %5.5d Damroll: %5.5d Alignment: %4.4d", GET_AC(victim), GET_HITROLL(victim),
        GET_DAMROLL(victim), victim->alignment);
    learn_from_success(ch, gsn_gather_intelligence);
    return;
}
