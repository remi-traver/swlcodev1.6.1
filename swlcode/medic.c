/* Medic.c developed by Arcturus for medical type skills. */

#include <sys/types.h>
#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include "mud.h"

/* copied over to be grouped and slightly modified. - Arcturus */
void do_aid( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;
    int percent;
    int xp;
    if ( IS_NPC(ch) && IS_AFFECTED( ch, AFF_CHARM ) )
    {
        send_to_char( "You can't concentrate enough for that.\n\r", ch );
        return;
    }

    one_argument( argument, arg );
    if ( arg[0] == '\0' )
    {
        send_to_char( "Aid whom?\n\r", ch );
        return;
    }

    if ( ( victim = get_char_room( ch, arg ) ) == NULL )
    {
        send_to_char( "They aren't here.\n\r", ch );
        return;
    }

    if ( ch->mount )
    {
        send_to_char( "You can't do that while mounted.\n\r", ch );
        return;
    }

    if ( victim == ch )
    {
        send_to_char( "Aid yourself?\n\r", ch );
        return;
    }
    if ( victim->position > POS_STUNNED )
    {
        act( AT_PLAIN, "$N doesn't need your help.", ch, NULL, victim,
             TO_CHAR);
        return;
    }

    if ( victim->hit <= -400 )
    {
        act( AT_PLAIN, "$N's condition is beyond your aiding ability.", ch,
             NULL, victim, TO_CHAR);
        return;
    }

    ch->alignment = ch->alignment + 20;
    ch->alignment = URANGE( -1000, ch->alignment, 1000 );

    percent = number_percent( ) - (get_curr_lck(ch) - 13) - (ch->subclass == SUBCLASS_MEDIC ? 10 : 0);
    WAIT_STATE( ch, skill_table[gsn_aid]->beats );
    if ( !IS_NPC(ch) && percent > ch->pcdata->learned[gsn_aid] )
    {
        send_to_char( "You fail.\n\r", ch );
        learn_from_failure( ch, gsn_aid );
        return;
    }

    ch->alignment = ch->alignment + 20;
    ch->alignment = URANGE( -1000, ch->alignment, 1000 );

    act( AT_SKILL, "You aid $N!",  ch, NULL, victim, TO_CHAR    );
    act( AT_SKILL, "$n aids $N!",  ch, NULL, victim, TO_NOTVICT );
    xp = victim->top_level * 10 + 250;
    xp = UMIN(xp, exp_level(ch->skill_level[MEDICAL_ABILITY]+1) - exp_level(ch->skill_level[MEDICAL_ABILITY]));
    gain_exp(ch, xp, MEDICAL_ABILITY);
    ch_printf( ch , "You gain %d medical experience.\n\r", xp );
    learn_from_success( ch, gsn_aid );
    WAIT_STATE( ch, PULSE_PER_SECOND ); 
    if ( victim->hit < 1 )
    {
      victim->hit = 1;
        victim->blood = victim->blood + 5;
        victim->blood = URANGE( 0, victim->blood , 100 );
    }

    update_pos( victim );
    act( AT_SKILL, "$n aids you!", ch, NULL, victim, TO_VICT    );
    return;
}

/* Copied over to be grouped, and slightly modified- Arcturus */
void do_first_aid( CHAR_DATA *ch, char *argument )
{
   OBJ_DATA   *medpac;
   CHAR_DATA  *victim;
   int         heal;
   char        buf[MAX_STRING_LENGTH];
   int         healm;
   int         percent, xp;
   if ( ch->position == POS_FIGHTING )
   {
         send_to_char( "You can't do that while fighting!\n\r",ch );
         return;
   }

   medpac = get_eq_char( ch, WEAR_HOLD );
   if ( !medpac || medpac->item_type != ITEM_MEDPAC )
   {
         send_to_char( "You need to be holding a medpac.\n\r",ch );
         return;
   }

   if ( medpac->value[0] <= 0 )
   {
         send_to_char( "Your medpac seems to be empty.\n\r",ch );
         return;
   }

   if ( argument[0] == '\0' )
      victim = ch;
   else
      victim = get_char_room( ch, argument );
   if ( !victim )
   {
       ch_printf( ch, "I don't see any %s here...\n\r" , argument );
       return;
   }

   heal = medpac->value[1] * 10;
   heal *= ch->subclass == SUBCLASS_MEDIC ? 1.1 : 1;
   percent = IS_NPC(ch) ? ch->top_level : ch->pcdata->learned[gsn_first_aid];
   percent += ch->subclass == SUBCLASS_MEDIC ? 10 : 0;
   if( number_percent() > percent)
   {
       send_to_char("You can't seem to find the necessary bandages.\n\r", ch);
       WAIT_STATE(ch, PULSE_PER_SECOND);
       learn_from_failure(ch, gsn_first_aid);
       return;
   }
   if ( victim == ch )
   {
       ch_printf( ch, "You tend to your wounds.\n\r");
       sprintf( buf , "$n uses %s to help heal $s wounds." , medpac->short_descr );
       act( AT_ACTION, buf, ch, NULL, victim, TO_ROOM );
   }
   else
   {
       sprintf( buf , "You tend to $N's wounds." );
       act( AT_ACTION, buf, ch, NULL, victim, TO_CHAR );
       sprintf( buf , "$n uses %s to help heal $N's wounds." , medpac->short_descr );
       act( AT_ACTION, buf, ch, NULL, victim, TO_NOTVICT );
       sprintf( buf , "$n uses %s to help heal your wounds." , medpac->short_descr );
       act( AT_ACTION, buf, ch, NULL, victim, TO_VICT );
   }

   --medpac->value[0];
   victim->hit += URANGE ( 0, heal , victim->max_hit - victim->hit );
   healm = heal * 2;
   victim->move += URANGE ( 0, healm , victim->max_move - victim->move );
   WAIT_STATE(ch, PULSE_VIOLENCE); /* so they don't heal in 1 round. */
   xp = victim->top_level * 50 + 1500;
   xp = UMIN(xp, exp_level(ch->skill_level[MEDICAL_ABILITY]+1) - exp_level(ch->skill_level[MEDICAL_ABILITY]));
   gain_exp(ch, victim->top_level * 20, MEDICAL_ABILITY);
   ch_printf( ch , "You gain %d medical experience.\n\r", victim->top_level*20 );
   learn_from_success( ch , gsn_first_aid );
}

/* Arcturus' Makemedpac */
void do_makemedpac( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    char buf[MAX_STRING_LENGTH];
    int level, chance;
    bool checktool, checkdura, checkchem, checkoven, checkneedle, checkfab, checkdrink;
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
                  send_to_char( "&RUsage: Makemedpac <name>\n\r&w", ch);
                  return;
                }

                checktool = FALSE;
                checkdura = FALSE;
                checkneedle = FALSE;
                checkoven = FALSE;
                checkfab  = FALSE;
                checkchem = FALSE;
                checkdrink = FALSE;
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
                  if (obj->item_type == ITEM_THREAD)
                    checkneedle = TRUE;
                  if (obj->item_type == ITEM_CHEMICAL)
                    checkchem = TRUE;
                  if (obj->item_type == ITEM_OVEN)
                    checkoven = TRUE;
                  if ( obj->item_type == ITEM_FABRIC)
                    checkfab = TRUE;
                  if ( obj->item_type == ITEM_DRINK_CON && obj->value[1] == 0)
                    checkdrink = TRUE;
                }

                if ( !checktool )
                {
                   send_to_char( "&RYou need toolkit to make a medpac\n\r", ch);
                   return;
                }

                if ( !checkdura )
                {
                   send_to_char( "&RYou need some durasteel to make the shell.\n\r", ch);
                   return;
                }
                if (!checkfab )
                {
                   send_to_char( "&RYou need fabric to make the bandages out of.\n\r", ch);
                   return;
                }
                if ( !checkneedle )
                {
                   send_to_char( "&RYou need a needle to sew your bandages.\n\r", ch);
                   return;
                }
                if( !checkchem )
                {
                   send_to_char( "&RYou need chemicals to prepare for the treatments.\n\r", ch);
                   return;
                }
                if ( !checkdrink )
                {
                   send_to_char( "&RYou need empty vials to save the chemicals in.\n\r", ch);
                   return;
                }
                if ( !checkoven )
                {
                   send_to_char( "&RYou need an oven to prepare the chemicals\n\r", ch);
                   return;
                }
                chance = IS_NPC(ch) ? ch->top_level
                         : (int) (ch->pcdata->learned[gsn_makemedpac]);
                if ( number_percent( ) < chance )
                {
                   send_to_char( "&GYou begin the long process of preparing a medpac.\n\r", ch);
                   act( AT_PLAIN, "$n begins preparing something.", ch,
                        NULL, argument , TO_ROOM );
                   if(IS_IMMORTAL(ch))
                   add_timer( ch, TIMER_DO_FUN, 1, do_makemedpac, 1 );
                   else if (ch->subclass == SUBCLASS_MEDIC )
                   add_timer ( ch , TIMER_DO_FUN , 9 , do_makemedpac , 1 );
                   else
                   add_timer ( ch , TIMER_DO_FUN , 10 , do_makemedpac , 1 );
                   ch->dest_buf = str_dup(arg);
                   return;
                }
                send_to_char("&RYou can't figure out how to prepare the medpac.\n\r",ch);
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

    level = IS_NPC(ch) ? ch->skill_level[MEDICAL_ABILITY] 
          : (int) (ch->pcdata->learned[gsn_makemedpac]);
    vnum = 10439;

    if ( ( pObjIndex = get_obj_index( vnum ) ) == NULL )
    {
         send_to_char( "&RThe item you are trying to create is missing from the database.\n\rPlease inform the administration.\n\r", ch);
         return;
    }


    checktool = FALSE;
    checkdura = FALSE;
    checkneedle = FALSE;
    checkoven = FALSE;
    checkfab  = FALSE;
    checkchem = FALSE;
    checkdrink = FALSE;
    for ( obj = ch->last_carrying; obj; obj = obj->prev_content )
    {
       if (obj->item_type == ITEM_TOOLKIT)
          checktool = TRUE;
       if (obj->item_type == ITEM_OVEN)
          checkoven = TRUE;
       if (obj->item_type == ITEM_DURASTEEL && checkdura == FALSE)
       { 
          checkdura = TRUE;
          separate_obj( obj );
          obj_from_char( obj );
          extract_obj( obj );
       }
       if (obj->item_type == ITEM_THREAD)
         checkneedle = TRUE;
       if (obj->item_type == ITEM_CHEMICAL && checkchem == FALSE)
       {
          checkchem = TRUE;
          separate_obj( obj );
          obj_from_char( obj );
          extract_obj( obj );
       }
       if (obj->item_type == ITEM_OVEN)
         checkoven = TRUE;
       if ( obj->item_type == ITEM_FABRIC && checkfab == FALSE)
       {
         checkfab = TRUE;
         separate_obj( obj );
         obj_from_char( obj );
         extract_obj( obj );
      } 
      if ( obj->item_type == ITEM_DRINK_CON && obj->value[1] == 0 && checkdrink == FALSE)
      {
         checkdrink = TRUE;
         separate_obj( obj );
         obj_from_char( obj );
         extract_obj( obj );
      }
    }

    chance = IS_NPC(ch) ? ch->top_level
                : (int) (ch->pcdata->learned[gsn_makemedpac]) ;
    level = chance;
    if(ch->subclass == SUBCLASS_MEDIC)
    {
       level += 10;
       chance += 10;
    }
    if ( number_percent( ) > chance*2  || ( !checktool ) || ( !checkdura ) || ( !checkneedle ) 
       || ( !checkoven )  || ( !checkfab ) || (!checkchem) || ( !checkdrink ) )
    {
       send_to_char( "&RYou inspect your new medpac.\n\r", ch);
       send_to_char( "&RIt is lacking proper medicines, and bandages.\n\r", ch);
       send_to_char( "&RIt is unsatisfactory, so you trash it.\n\r", ch);
       learn_from_failure( ch, gsn_makemedpac );
       return;
    }

    obj = create_object( pObjIndex, level );

    obj->item_type = ITEM_MEDPAC;
    SET_BIT( obj->wear_flags, ITEM_HOLD );
    SET_BIT( obj->wear_flags, ITEM_TAKE );
    obj->level = level;
    obj->weight = 3;
    STRFREE( obj->name );
    strcpy( buf, arg );
    strcat( buf, " medpac medkit" );
    obj->name = STRALLOC( stripclr( buf ) );
    strcpy( buf, arg );
    STRFREE( obj->short_descr );
    obj->short_descr = STRALLOC( buf );
    STRFREE( obj->description );
    strcat( buf, " was left here." );
    obj->description = STRALLOC( buf );
    obj->value[0] = level/5;
    obj->value[1] = level/5;
    obj->cost = obj->value[0] * 150;
    obj = obj_to_char( obj, ch );

    send_to_char( "&GYou finish your work and hold up your newly created medpac.&w\n\r", ch);
    act( AT_PLAIN, "$n finishes crafting a medpac.", ch,
         NULL, argument , TO_ROOM );
    {
         long xpgain;
         xpgain = UMIN( 10000 , exp_level(ch->skill_level[MEDICAL_ABILITY]+1) - exp_level(ch->skill_level[MEDICAL_ABILITY]));
         gain_exp(ch, xpgain, MEDICAL_ABILITY);
         ch_printf( ch , "You gain %d medical experience.", xpgain );
    }

    learn_from_success( ch, gsn_makemedpac );
}

/* Arcturus' makeinoculator */
void do_makeinoculator( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    char buf[MAX_STRING_LENGTH];
    int level, chance;
    bool checktool, checkdura, checkoven, checkneedle, checkdrink;
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
                  send_to_char( "&RUsage: Makeinoculator <name>\n\r&w", ch);
                  return;
                }

                checktool = FALSE;
                checkdura = FALSE;
                checkneedle = FALSE;
                checkoven = FALSE;
                checkdrink = FALSE;
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
                  if (obj->item_type == ITEM_THREAD)
                    checkneedle = TRUE;
                  if (obj->item_type == ITEM_OVEN)
                    checkoven = TRUE;
                  if ( obj->item_type == ITEM_DRINK_CON && obj->value[1] == 0)
                    checkdrink = TRUE;
                }

                if ( !checktool )
                {
                   send_to_char( "&RYou need toolkit to make an inoculator.\n\r", ch);
                   return;
                }
                if ( !checkdura )
                {
                   send_to_char( "&RYou need some durasteel to make the casing.\n\r", ch);
                   return;
                }
                if ( !checkneedle )
                {
                   send_to_char( "&RYou need a needle for the tip.\n\r", ch);
                   return;
                }
                if ( !checkdrink )
                {
                   send_to_char( "&RYou need empty vials for the catridge casing\n\r", ch);
                   return;
                }
                if ( !checkoven )
                {
                   send_to_char( "&RYou need an oven to make the casing.\n\r", ch);
                   return;
                }
                chance = IS_NPC(ch) ? ch->top_level
                         : (int) (ch->pcdata->learned[gsn_makeinoculator]);
                if ( number_percent( ) < chance )
                {
                   send_to_char( "&GYou begin the long process of crafting an inoculator.\n\r", ch);
                   act( AT_PLAIN, "$n begins preparing something.", ch,
                        NULL, argument , TO_ROOM );
                   if(IS_IMMORTAL(ch))
                   add_timer( ch, TIMER_DO_FUN, 1, do_makeinoculator, 1 );
                   else if (ch->subclass == SUBCLASS_MEDIC )
                   add_timer ( ch , TIMER_DO_FUN , 9 , do_makeinoculator , 1 );
                   else
                   add_timer ( ch , TIMER_DO_FUN , 10 , do_makeinoculator , 1 );
                   ch->dest_buf = str_dup(arg);
                   return;
                }
                send_to_char("&RYou cannot figure out how to put the inoculator together.\n\r", ch);
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

    level = IS_NPC(ch) ? ch->skill_level[MEDICAL_ABILITY]
          : (int) (ch->pcdata->learned[gsn_makeinoculator]);
    vnum = 10440;

    if ( ( pObjIndex = get_obj_index( vnum ) ) == NULL )
    {
         send_to_char( "&RThe item you are trying to create is missing from the database.\n\rPlease inform the administration.", ch);
         bug("NULL OBJ, Make Inoculator\n\r");
         return;
    }
    checktool = FALSE;
    checkdura = FALSE;
    checkneedle = FALSE;
    checkoven = FALSE;
    checkdrink = FALSE;
    for ( obj = ch->last_carrying; obj; obj = obj->prev_content )
    {
       if (obj->item_type == ITEM_TOOLKIT)
          checktool = TRUE;
       if (obj->item_type == ITEM_OVEN)
          checkoven = TRUE;
       if (obj->item_type == ITEM_DURASTEEL && checkdura == FALSE)
       {
          checkdura = TRUE;
          separate_obj( obj );
          obj_from_char( obj );
          extract_obj( obj );
       }
       if (obj->item_type == ITEM_THREAD && checkneedle == FALSE)
       {
         checkneedle = TRUE;
         separate_obj(obj);
         obj_from_char(obj);
         extract_obj(obj);
       }
       if (obj->item_type == ITEM_OVEN)
         checkoven = TRUE;
      if ( obj->item_type == ITEM_DRINK_CON && obj->value[1] == 0 && checkdrink == FALSE)
      {
         checkdrink = TRUE;
         separate_obj( obj );
         obj_from_char( obj );
         extract_obj( obj );
      }
    }

    chance = IS_NPC(ch) ? ch->top_level
                : (int) (ch->pcdata->learned[gsn_makeinoculator]) ;
    level = chance;
    if(ch->subclass == SUBCLASS_MEDIC)
    {
       level += 10;
       chance += 10;
    }
    if ( number_percent( ) > chance*2  || ( !checktool ) || ( !checkdura ) || ( !checkneedle )
       || ( !checkoven )  || ( !checkdrink ) )
    {
       send_to_char( "&RYou inspect your new inoculator.\n\r", ch);
       send_to_char( "&RThe casing is faulty and the chamber is cracked..\n\r", ch);
       send_to_char( "&RIt is unsatisfactory, so you trash it.\n\r", ch);
       learn_from_failure( ch, gsn_makeinoculator);
       return;
    }

    obj = create_object( pObjIndex, level );

    obj->item_type = ITEM_INOCULATOR;
    SET_BIT( obj->wear_flags, ITEM_HOLD );
    SET_BIT( obj->wear_flags, ITEM_TAKE );
    obj->level = level;
    obj->weight = 2;
    STRFREE( obj->name );
    strcpy( buf, arg );
    strcat( buf, " inoculator" );
    obj->name = STRALLOC( stripclr( buf ) );
    strcpy( buf, arg );
    STRFREE( obj->short_descr );
    obj->short_descr = STRALLOC( buf );
    STRFREE( obj->description );
    strcat( buf, " was left here." );
    obj->description = STRALLOC( buf );
    obj->value[0] = level/5;
    obj->cost = obj->value[0] * 150;
    obj = obj_to_char( obj, ch );

    send_to_char( "&GYou finish your work and hold up your newly created inoculator.&w\n\r", ch);
    act( AT_PLAIN, "$n finishes crafting an inoculator.", ch,
         NULL, argument , TO_ROOM );
    {
         long xpgain;
         xpgain = UMIN( 30000, exp_level(ch->skill_level[MEDICAL_ABILITY]+1)-exp_level(ch->skill_level[MEDICAL_ABILITY]));
         gain_exp(ch, xpgain, MEDICAL_ABILITY);
         ch_printf( ch , "You gain %d medical experience.", xpgain );
    }

    learn_from_success( ch, gsn_makeinoculator);
}

/* Arcturus' Inoculations */
void do_inoculate( CHAR_DATA *ch, char *argument )
{
   OBJ_DATA   *inoculator;
   CHAR_DATA  *victim;
   char        buf[MAX_STRING_LENGTH];
   int         percent, xp;
   ch_ret      retcode;

   if ( ch->position == POS_FIGHTING )
   {
         send_to_char( "You can't do that while fighting!\n\r",ch );
         return;
   }

   inoculator = get_eq_char( ch, WEAR_HOLD );
   if ( !inoculator || inoculator->item_type != ITEM_INOCULATOR )
   {
         send_to_char( "You need to be holding an inoculator.\n\r",ch );
         return;
   }

   if ( inoculator->value[1] == 0 && inoculator->value[3] == 0)
   {
         send_to_char( "Your inoculator cartridge seems to be empty.\n\r",ch );
         return;
   }

   if ( argument[0] == '\0' )
      victim = ch;
   else
      victim = get_char_room( ch, argument );
   if ( !victim )
   {
       ch_printf( ch, "I don't see any %s here...\n\r" , argument );
       return;
   }

   percent = IS_NPC(ch) ? ch->top_level : ch->pcdata->learned[gsn_inoculate];
   percent += ch->subclass == SUBCLASS_MEDIC ? 10 : 0;
   if( number_percent() > percent)
   {
       send_to_char("You fail to find a proper vein.\n\r", ch);
       WAIT_STATE(ch, PULSE_PER_SECOND);
       learn_from_failure(ch, gsn_inoculate);
       return;
   }
   if ( victim == ch )
   {
       ch_printf( ch, "You shoot yourself in the arm.\n\r");
       sprintf( buf , "$n uses %s to inoculate $n arm." , inoculator->short_descr );
       act( AT_ACTION, buf, ch, NULL, victim, TO_ROOM );
   }
   else
   {
       sprintf( buf , "You shoot $N's arm up." );
       act( AT_ACTION, buf, ch, NULL, victim, TO_CHAR );
       sprintf( buf , "$n uses %s to inoculate $N." , inoculator->short_descr );
       act( AT_ACTION, buf, ch, NULL, victim, TO_NOTVICT );
       sprintf( buf , "$n uses %s to inoculate your arm.." , inoculator->short_descr );
       act( AT_ACTION, buf, ch, NULL, victim, TO_VICT );
   }

   --inoculator->value[0];
   retcode = rNONE;
   switch(inoculator->value[1])
   {
   default: break;
   case MEDICINE_HP: victim->hit += inoculator->value[2]; 
      ch_printf(victim, "That feels much better, you feel healed %d points of health.\n\r", inoculator->value[2]); 
      if(victim->hit > victim->max_hit)
         victim->hit = victim->max_hit;
      break;
   case MEDICINE_MV: victim->move += inoculator->value[2];
      ch_printf(victim, "That feels much better, you feel rejuvenated %d points of movement.\n\r", inoculator[2]); 
      if(victim->move > victim->max_move)
         victim->move = victim->max_move;
      break;
   case MEDICINE_POISON: retcode = obj_cast_spell(skill_lookup("cure poison"), inoculator->value[2], ch, victim, NULL); break;
   case MEDICINE_BLINDNESS: retcode = obj_cast_spell(skill_lookup("cure blindness"), inoculator->value[2], ch, victim, NULL); break;
   case MEDICINE_CBLIND: retcode = obj_cast_spell( skill_lookup("blindness"), inoculator->value[2], ch, victim, NULL); break;
   case MEDICINE_CPOISON: retcode = obj_cast_spell( skill_lookup("poison"), inoculator->value[2], ch, victim, NULL); break;
   case MEDICINE_SLEEP: retcode = obj_cast_spell( skill_lookup("sleep"), inoculator->value[1], ch, victim, NULL); break;
   }
   if(retcode == rNONE)
   {
    switch(inoculator->value[3])
    {
    default: break;
    case MEDICINE_HP: victim->hit += inoculator->value[4];
       ch_printf(victim, "That feels much better, you feel healed %d points of health.\n\r", inoculator->value[4]); 
       if(victim->hit > victim->max_hit)
          victim->hit = victim->max_hit;
       break;
    case MEDICINE_MV: victim->move += inoculator->value[4]; 
       ch_printf(victim, "That feels much better, you feel rejuvenated %d points of movement.\n\r", inoculator->value[4]);
       if(victim->move > victim->max_move)
          victim->move = victim->max_move;
       break;
    case MEDICINE_POISON: retcode = obj_cast_spell(skill_lookup("cure poison"), inoculator->value[4], ch, victim, NULL); break;
    case MEDICINE_BLINDNESS: retcode = obj_cast_spell(skill_lookup("cure blindness"), inoculator->value[4], ch, victim, NULL);break;
    case MEDICINE_CBLIND: retcode = obj_cast_spell( skill_lookup("blindness"), inoculator->value[4], ch, victim, NULL); break;
    case MEDICINE_CPOISON: retcode = obj_cast_spell( skill_lookup("poison"), inoculator->value[4], ch, victim, NULL); break;
    case MEDICINE_SLEEP: retcode = obj_cast_spell( skill_lookup("sleep"), inoculator->value[3], ch, victim, NULL); break;
    }
   }
   WAIT_STATE(ch, PULSE_VIOLENCE); /* so they don't heal in 1 round, and hurt.*/
   for(percent = 1; percent < 5; percent++)
      inoculator->value[percent] = 0;
   if(inoculator->value[0] < 1)
      send_to_char("Your inoculator has become useless.\n\r", ch);
   xp = victim->top_level * 50 + 3500;
   xp = UMIN(xp, exp_level(ch->skill_level[MEDICAL_ABILITY]+1) - exp_level(ch->skill_level[MEDICAL_ABILITY]));
   gain_exp(ch, victim->top_level * 25, MEDICAL_ABILITY);
   ch_printf( ch , "You gain %d medical experience.\n\r", victim->top_level*20 );
   learn_from_success( ch , gsn_inoculate);
}

/* Loading the Inoculator- Arcturus */
void do_loadinoculator( CHAR_DATA *ch, char *argument )
{
    OBJ_DATA *inoculator;
    OBJ_DATA *obj;
    bool checkammo = FALSE;
    obj = NULL;
    inoculator = get_eq_char(ch, WEAR_HOLD);
    if(!inoculator || inoculator->item_type != ITEM_INOCULATOR )
    {
       send_to_char("You aren't holding an inoculator to load up.\n\r", ch);
       return;
    }
    if(inoculator->value[0] < 1)
    {
       send_to_char("That inoculator is useless.\n\r", ch);
       return;
    }
   if(argument[0] == '\0')
   {
       send_to_char("&RUsage: Loadinoculator <obj>&w\n\r", ch);
       return;
   }
   if ( ( obj = get_obj_carry( ch, argument ) ) == NULL )
   {
       send_to_char("&RYou do not have that item.&w\n\r", ch );
       return;
   }
   if(obj->item_type == ITEM_SHOT)
   {
      checkammo = TRUE;
      separate_obj( obj );
      inoculator->value[1] = obj->value[0];
      inoculator->value[2] = obj->value[1];
      inoculator->value[3] = obj->value[2];
      inoculator->value[4] = obj->value[3];
      obj_from_char( obj );
      extract_obj( obj );
   }
   if(checkammo)
   {
      send_to_char("You insert the shot successfully, and it clicks into place.\n\r", ch);
   }
   else
   {
      send_to_char("That is not a suitable shot to load the inoculator with.\n\r", ch);
   }
   return;
}

void do_makeshot( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    char buf[MAX_STRING_LENGTH];
    int level, chance;
    bool checktool, checkdrink, checkmed, checkoven;
    OBJ_DATA *obj, *old;
    OBJ_INDEX_DATA *pObjIndex;
    int vnum;
    int values[4];
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
                  send_to_char( "&RUsage: Makeshot <obj>\n\r&w", ch);
                  return;
                }

                checktool = FALSE;
                checkmed = FALSE;
                checkoven = FALSE;
                checkdrink = FALSE;
                if ( !IS_SET( ch->in_room->room_flags, ROOM_FACTORY ) )
                {
                   send_to_char( "&RYou need to be in a factory or workshop to do that.\n\r", ch);
                   return;
                }

                if ( ( obj = get_obj_carry( ch, argument ) ) == NULL )
                {
                   send_to_char("&RYou do not have that item.&w\n\r", ch );
                   return;
                }
                if(obj->item_type == ITEM_MEDICINE)
                   checkmed = TRUE;
                else
                {
                   send_to_char("That isn't a medicine.\n\r", ch);
                   return;
                }
                for ( obj = ch->last_carrying; obj; obj = obj->prev_content )
                {
                  if (obj->item_type == ITEM_TOOLKIT)
                    checktool = TRUE;
                  if (obj->item_type == ITEM_OVEN)
                    checkoven = TRUE;
                  if ( obj->item_type == ITEM_DRINK_CON && obj->value[1] == 0)
                    checkdrink = TRUE;
                }

                if ( !checktool )
                {
                   send_to_char( "&RYou need toolkit to prepare a shot.\n\r", ch);
                   return;
                }
                if ( !checkdrink )
                {
                   send_to_char( "&RYou need empty vials for the catridge casing\n\r", ch);
                   return;
                }
                if ( !checkoven )
                {
                   send_to_char( "&RYou need an oven to make the casing.\n\r", ch);
                   return;
                }
                chance = IS_NPC(ch) ? ch->top_level
                         : (int) (ch->pcdata->learned[gsn_makeshot]);
                if ( number_percent( ) < chance )
                {
                   send_to_char( "&GYou begin the long process of preparing a shot.\n\r", ch);
                   act( AT_PLAIN, "$n begins preparing something.", ch,
                        NULL, argument , TO_ROOM );
                   if(IS_IMMORTAL(ch))
                   add_timer( ch, TIMER_DO_FUN, 1, do_makeshot, 1 );
                   else if (ch->subclass == SUBCLASS_MEDIC )
                   add_timer ( ch , TIMER_DO_FUN , 9 , do_makeshot , 1 );
                   else
                   add_timer ( ch , TIMER_DO_FUN , 10 , do_makeshot , 1 );
                   ch->dest_buf = str_dup(arg);
                   return;
                }
                send_to_char("&RYou cannot figure out how to handle the medicine.\n\r", ch);
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

    level = IS_NPC(ch) ? ch->skill_level[MEDICAL_ABILITY]
          : (int) (ch->pcdata->learned[gsn_makeshot]);
    vnum = 10441;

    if ( ( pObjIndex = get_obj_index( vnum ) ) == NULL )
    {
         send_to_char( "&RThe item you are trying to create is missing from the database.\n\rPlease inform the administration.\n\r", ch);
         bug("NULL OBJ, Makeshot\n\r");
         return;
    }
    checktool = FALSE;
    checkmed = FALSE;
    checkoven = FALSE;
    checkdrink = FALSE;
    if ( ( obj = get_obj_carry( ch, arg ) ) == NULL )
    {
       send_to_char("&RYou do not have that item.&w\n\r", ch );
       return;
    }
    if(obj->item_type != ITEM_MEDICINE)
    {
       send_to_char("That isn't a medicine. BUG OR LACK OF OBJ.\n\r", ch);
       return;
    }
    old = obj;
    checkmed = TRUE;
    values[0] = obj->value[0];
    values[1] = obj->value[1];
    values[2] = obj->value[2];
    values[3] = obj->value[3];
    checkmed = TRUE;

    for ( obj = ch->last_carrying; obj; obj = obj->prev_content )
    {
       if (obj->item_type == ITEM_TOOLKIT)
           checktool = TRUE;
       if (obj->item_type == ITEM_OVEN)
           checkoven = TRUE;
       if ( obj->item_type == ITEM_MEDICINE && checkmed == FALSE )
       {
           values[0] = obj->value[0]; 
           values[1] = obj->value[1];
           values[2] = obj->value[2];
           values[3] = obj->value[3];
           separate_obj( obj );
           obj_from_char( obj );
           extract_obj( obj );
           checkmed = TRUE;
      }
      if ( obj->item_type == ITEM_DRINK_CON && obj->value[1] == 0 && checkdrink == FALSE)
      {
           checkdrink = TRUE;
           separate_obj( obj );
           obj_from_char( obj );
           extract_obj( obj );
      }
    }

    chance = IS_NPC(ch) ? ch->top_level
                : (int) (ch->pcdata->learned[gsn_makeshot]) ;
    level = chance;
    if(ch->subclass == SUBCLASS_MEDIC)
    {
       level += 10;
       chance += 10;
    }
    if ( number_percent( ) > chance*2  || ( !checktool ) || (!checkmed)
       || ( !checkoven )  || ( !checkdrink ) )
    {
       send_to_char( "&RYou inspect your shot casing.\n\r", ch);
       send_to_char( "&RThere appears to be a leak in the casing.\n\r", ch);
       send_to_char( "&RIt is unsatisfactory, so you trash it.\n\r", ch);
       separate_obj(old);
       obj_from_char(old);
       extract_obj(old);
       learn_from_failure( ch, gsn_makeshot);
       return;
    }

    obj = create_object( pObjIndex, level );

    obj->item_type = ITEM_SHOT;
    SET_BIT( obj->wear_flags, ITEM_HOLD );
    SET_BIT( obj->wear_flags, ITEM_TAKE );
    obj->level = level;
    obj->weight = 1;
    STRFREE( obj->name );
    strcpy( buf, "A shot made from a " );
    strcat( buf, old->short_descr );
    obj->name = STRALLOC( stripclr( buf ) );
    STRFREE( obj->short_descr );
    obj->short_descr = STRALLOC( buf );
    STRFREE( obj->description );
    strcat( buf, " was left here." );
    obj->description = STRALLOC( buf );
    separate_obj(old);
    obj_from_char(old);
    extract_obj(old);
    obj->value[0] = values[0];
    if(values[0] == MEDICINE_HP || values[0] == MEDICINE_MV)
       obj->value[1] = values[1] * level;
    else
       obj->value[1] = values[1] + level;
    obj->value[2] = values[2];
    if(values[2] == MEDICINE_HP || values[2] == MEDICINE_MV)
       obj->value[3] = values[3] * level;
    else
       obj->value[3] = values[3] + level;
    obj->cost = level + obj->value[1] + obj->value[3];
    obj = obj_to_char( obj, ch );

    send_to_char( "&GYou finish your work and hold up your newly prepared shot.&w\n\r", ch);
    act( AT_PLAIN, "$n finishes preparing a shot.", ch,
         NULL, argument , TO_ROOM );
    {
         long xpgain;
         xpgain = UMIN( 35000, exp_level(ch->skill_level[MEDICAL_ABILITY]+1)-exp_level(ch->skill_level[MEDICAL_ABILITY]));
         gain_exp(ch, xpgain, MEDICAL_ABILITY);
         ch_printf( ch , "You gain %d medical experience.", xpgain );
    }
    learn_from_success( ch, gsn_makeshot);
}

/* Concentrate Medicinal Strength-Arcturus */
void do_concentrate( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    char buf[MAX_STRING_LENGTH];
    int level, chance;
    bool checktool, checkoven;
    OBJ_DATA *obj;
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
                  send_to_char( "&RUsage: concentrate <obj>\n\r&w", ch);
                  return;
                }

                checktool = FALSE;
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
                  if (obj->item_type == ITEM_OVEN)
                    checkoven = TRUE;
                }

                if ( !checktool )
                {
                   send_to_char( "&RYou need toolkit to extract its strength.\n\r", ch);
                   return;
                }
                if ( !checkoven )
                {
                   send_to_char( "&RYou need an oven to extract its strength.\n\r", ch);
                   return;
                }
                if ( ( obj = get_obj_carry( ch, argument ) ) == NULL )
                {
                   send_to_char("&RYou do not have that item.&w\n\r", ch );
                   return;
                }
                else if(obj->item_type != ITEM_MEDICINE )
                {
                   send_to_char("&RThat isn't a medicine.&w\n\r", ch);
                   return;
                }
                if(obj->value[5] == 1)
                {
                   send_to_char("&RYou have already concentrated that medicine.\n\r", ch);
                   return;
                }
                chance = IS_NPC(ch) ? ch->top_level
                         : (int) (ch->pcdata->learned[gsn_concentrate]);
                if ( number_percent( ) < chance )
                {
                   send_to_char( "&GYou begin the long process extracting the medicines strength.\n\r", ch);
                   act( AT_PLAIN, "$n begins preparing something.", ch,
                        NULL, argument , TO_ROOM );
                   if(IS_IMMORTAL(ch))
                   add_timer( ch, TIMER_DO_FUN, 1, do_concentrate, 1 );
                   else if (ch->subclass == SUBCLASS_MEDIC )
                   add_timer ( ch , TIMER_DO_FUN , 9 , do_concentrate , 1 );
                   else
                   add_timer ( ch , TIMER_DO_FUN , 10 , do_concentrate , 1 );
                   ch->dest_buf = str_dup(arg);
                   return;
                }
                send_to_char("&RYou cannot figure out how to handle the medicine.\n\r", ch);
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

    level = IS_NPC(ch) ? ch->skill_level[MEDICAL_ABILITY]
          : (int) (ch->pcdata->learned[gsn_concentrate]);
    checktool = FALSE;
    checkoven = FALSE;
    for ( obj = ch->last_carrying; obj; obj = obj->prev_content )
    {
       if (obj->item_type == ITEM_TOOLKIT)
          checktool = TRUE;
       if (obj->item_type == ITEM_OVEN)
          checkoven = TRUE;
    }
   if ( ( obj = get_obj_carry( ch, arg ) ) == NULL )
   {
      send_to_char("&RYou do not have that item.&w\n\r", ch );
      return;
   }
    chance = IS_NPC(ch) ? ch->top_level
                : (int) (ch->pcdata->learned[gsn_concentrate]) ;
    level = chance;
    if(ch->subclass == SUBCLASS_MEDIC)
    {
       level += 10;
       chance += 10;
    }
    if ( number_percent( ) > chance*2  || ( !checktool )
       || ( !checkoven )  )
    {
       send_to_char( "&RYou inspect your medicine concentration.\n\r", ch);
       send_to_char( "&RThere was a fatal mistake, and the medicine is useless.\n\r", ch);
       send_to_char( "&RIt is unsatisfactory, so you trash it.\n\r", ch);
       learn_from_failure( ch, gsn_concentrate);
       separate_obj(obj); 
       obj_from_char(obj);
       extract_obj(obj);
       return;
    }
    separate_obj( obj );
    obj->value[1] *= 2;
    obj->value[3] *= 2;
    obj->value[5] = 1;
    obj->cost = level + obj->value[1] + obj->value[3] + obj->cost;   
    send_to_char( "&GYou finish concetrating the medicine.&w\n\r", ch);
    act( AT_PLAIN, "$n finishes concentrating the medicine.", ch,
         NULL, argument , TO_ROOM );
    {
         long xpgain;
         xpgain = UMIN( 50000, exp_level(ch->skill_level[MEDICAL_ABILITY]+1)-exp_level(ch->skill_level[MEDICAL_ABILITY]));
         gain_exp(ch, xpgain, MEDICAL_ABILITY);
         ch_printf( ch , "You gain %d medical experience.", xpgain );
    }
    learn_from_success( ch, gsn_concentrate);
}


/* Arcturus' Brew */
void do_brew( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    char buf[MAX_STRING_LENGTH];
    int level, chance;
    bool checktool, checkdrink, checkmed, checkoven;
    OBJ_DATA *obj, *old;
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
                  send_to_char( "&RUsage: Brew <obj>\n\r&w", ch);
                  return;
                }

                checktool = FALSE;
                checkmed = FALSE;
                checkoven = FALSE;
                checkdrink = FALSE;
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
                  if ( obj->item_type == ITEM_DRINK_CON && obj->value[1] == 0)
                    checkdrink = TRUE;
                }

                if ( !checktool )
                {
                   send_to_char( "&RYou need toolkit to prepare an elixir\n\r", ch);
                   return;
                }
                if ( ( obj = get_obj_carry( ch, argument ) ) == NULL )
                {
                   send_to_char("&RYou do not have that item.&w\n\r", ch );
                   return;
                }
                if(obj->item_type == ITEM_MEDICINE)
                   checkmed = TRUE;
                else
                {
                   send_to_char("That isn't a medicine.\n\r", ch);
                   return;
                }
                if ( !checkdrink )
                {
                   send_to_char( "&RYou need empty vials to store the elixir.\n\r", ch);
                   return;
                }
                if ( !checkoven )
                {
                   send_to_char( "&RYou need an oven to prepare the elixir.\n\r", ch);
                   return;
                }
                chance = IS_NPC(ch) ? ch->top_level
                         : (int) (ch->pcdata->learned[gsn_brew]);
                if ( number_percent( ) < chance )
                {
                   send_to_char( "&GYou begin the long process of preparing an elixir.\n\r", ch);
                   act( AT_PLAIN, "$n begins preparing an elixir", ch,
                        NULL, argument , TO_ROOM );
                   if(IS_IMMORTAL(ch))
                   add_timer( ch, TIMER_DO_FUN, 1, do_brew, 1 );
                   else if (ch->subclass == SUBCLASS_MEDIC )
                   add_timer ( ch , TIMER_DO_FUN , 9 , do_brew , 1 );
                   else
                   add_timer ( ch , TIMER_DO_FUN , 10 , do_brew , 1 );
                   ch->dest_buf = str_dup(arg);
                   return;
                }
                send_to_char("&RYou cannot figure out how to handle the medicine.\n\r", ch);
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

    level = IS_NPC(ch) ? ch->skill_level[MEDICAL_ABILITY]
          : (int) (ch->pcdata->learned[gsn_brew]);
    vnum = 10443;

    if ( ( pObjIndex = get_obj_index( vnum ) ) == NULL )
    {
         send_to_char( "&RThe item you are trying to create is missing from the database.\n\rPlease inform the administration.\n\r", ch);
         bug("NULL OBJ, Brew!\n\r");
         return;
    }
    checktool = FALSE;
    checkoven = FALSE;
    checkdrink = FALSE;
    checkmed = FALSE;
    if ( ( obj = get_obj_carry( ch, arg ) ) == NULL )
    {
       send_to_char("&RYou do not have that item.&w\n\r", ch );
       return;
    }
    if(obj->item_type != ITEM_MEDICINE)
    {
       send_to_char("That isn't a medicine. BUG OR LACK OF OBJ.\n\r", ch);
       return;
    }
    old = obj;
    checkmed = TRUE;
    for ( obj = ch->last_carrying; obj; obj = obj->prev_content )
    {
       if (obj->item_type == ITEM_TOOLKIT)
           checktool = TRUE;
       if (obj->item_type == ITEM_OVEN)
           checkoven = TRUE;
      if ( obj->item_type == ITEM_DRINK_CON && obj->value[1] == 0 && checkdrink == FALSE)
      {
           checkdrink = TRUE;
           separate_obj( obj );
           obj_from_char( obj );
           extract_obj( obj );
      }
    }

    chance = IS_NPC(ch) ? ch->top_level
                : (int) (ch->pcdata->learned[gsn_brew]) ;
    level = chance;
    if(ch->subclass == SUBCLASS_MEDIC)
    {
       level += 10;
       chance += 10;
    }
    if ( number_percent( ) > chance*2  || ( !checktool ) || (!checkmed)
       || ( !checkoven )  || ( !checkdrink ) )
    {
       send_to_char( "&RYou inspect your elixir\n\r", ch);
       send_to_char( "&RThe elixir has too many impurities.\n\r", ch);
       send_to_char( "&RIt is unsatisfactory, so you trash it.\n\r", ch);
       learn_from_failure( ch, gsn_brew);
       separate_obj(old);
       obj_from_char(old);
       extract_obj(old);
       return;
    }

    obj = create_object( pObjIndex, level );

    obj->item_type = ITEM_ELIXIR;
    SET_BIT( obj->wear_flags, ITEM_HOLD );
    SET_BIT( obj->wear_flags, ITEM_TAKE );
    obj->level = level;
    obj->weight = 1;
    STRFREE( obj->name );
    strcpy( buf, "An Elixir made from a " );
    strcat( buf, old->short_descr );
    obj->name = STRALLOC( stripclr( buf ) );
    STRFREE( obj->short_descr );
    obj->short_descr = STRALLOC( buf );
    STRFREE( obj->description );
    strcat( buf, " was left here." );
    obj->description = STRALLOC( buf );
    obj->value[0] = old->value[0];
    if(level <= 40)
       level = 41;
    if(old->value[0] == MEDICINE_HP || old->value[0] == MEDICINE_MV)
       obj->value[1] = old->value[1] * (level - 40);
    else
       obj->value[1] = old->value[1] + (level - 40);
    obj->value[2] = old->value[2];
    if(old->value[2] == MEDICINE_HP || old->value[2] == MEDICINE_MV)
       obj->value[3] = old->value[3] * (level - 40);
    else
       obj->value[3] = old->value[3] + level - 40;
    obj->cost = level + obj->value[1] + obj->value[3] + 2000;
    obj = obj_to_char( obj, ch );
    separate_obj(old);
    obj_from_char(old);
    extract_obj(old);   
    send_to_char( "&GYou finish your work and hold up your newly prepared elixir.&w\n\r", ch);
    act( AT_PLAIN, "$n finishes preparing an elixir.", ch,
         NULL, argument , TO_ROOM );
    {
         long xpgain;
         xpgain = UMIN( 45000, exp_level(ch->skill_level[MEDICAL_ABILITY]+1)-exp_level(ch->skill_level[MEDICAL_ABILITY]));
         gain_exp(ch, xpgain, MEDICAL_ABILITY);
         ch_printf( ch , "You gain %d medical experience.", xpgain );
    }
    learn_from_success( ch, gsn_brew);
}

/* Mix, By Arcturus */
void do_mix( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH], arg2[MAX_INPUT_LENGTH];
    char buf[MAX_STRING_LENGTH];
    int level, chance;
    bool checktool, checkdrink, checkmed, checkoven;
    OBJ_DATA *obj, *med, *med2;
    OBJ_INDEX_DATA *pObjIndex;
    int vnum;
    argument = one_argument(argument, arg);
    strcpy(arg2, argument);
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

                if ( arg2[0] == '\0' )
                {
                  send_to_char( "&RUsage: Mix <obj> <obj>\n\r&w", ch);
                  return;
                }

                checktool = FALSE;
                checkmed = FALSE;
                checkoven = FALSE;
                checkdrink = FALSE;
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
                  if ( obj->item_type == ITEM_DRINK_CON && obj->value[1] == 0)
                    checkdrink = TRUE;
                }

                if ( !checktool )
                {
                   send_to_char( "&RYou need toolkit to mix medicines.\n\r", ch);
                   return;
                }
                if ( ( med = get_obj_carry( ch, arg) ) == NULL )
                {
                   send_to_char("&RYou do not have that item.&w\n\r", ch );
                   return;
                }
                if(med->item_type == ITEM_MEDICINE)
                   checkmed = TRUE;
                else
                {
                   send_to_char("That isn't a medicine.\n\r", ch);
                   return;
                }
                if( med->value[5] != 0 )
                {
                   send_to_char("You can't mix concentrated medicines.\n\r", ch);
                   return;
                }
                if ( ( med2 = get_obj_carry( ch, arg2) ) == NULL )
                {
                   send_to_char("&RYou do not have that item.&w\n\r", ch );
                   return;
                }
                if(med2->item_type == ITEM_MEDICINE)
                   checkmed = TRUE;
                else
                {
                   send_to_char("That isn't a medicine.\n\r", ch);
                   return;
                }
                if(med2->value[5] != 0)
                {
                   send_to_char("You can't mix concentrated medicines.\n\r", ch);
                   return;
                }
                if(med == med2)
                {
                   send_to_char("You can't mix the same object with itself.\n\r", ch);
                   return;
                }
                /* See if they are mixed already. */
                if(med->value[0] && med->value[2])
                {
                   send_to_char("The first medicine has already been mixed.\n\r", ch);
                   return;
                }
                if(med2->value[0] && med2->value[2])
                {
                   send_to_char("The second medicine has already been mixed.\n\r", ch);
                   return;
                }
                /* Make Sure that the medicines match up ok. */
                if(med->value[0] == 0)
                {
                    med->value[0] = med->value[2];
                    med->value[1] = med->value[3];
                }
                if(med2->value[0] == 0)
                {
                    med2->value[0] = med2->value[2];
                    med2->value[1] = med2->value[3];
                }
                /* Check to make sure it isn't two of the same type. */
                if( med->value[0] == med2->value[0] )
                {
                   send_to_char("You can't mix two medicines of the same type.\n\r", ch);
                   return;
                }                
                if ( !checkdrink )
                {
                   send_to_char( "&RYou need some empty vials to mix the medicines.\n\r", ch);
                   return;
                }
                if ( !checkoven )
                {
                   send_to_char( "&RYou need an oven to mix the medicines.\n\r", ch);
                   return;
                }
                chance = IS_NPC(ch) ? ch->top_level
                         : (int) (ch->pcdata->learned[gsn_mix]);
                if ( number_percent( ) < chance )
                {
                   send_to_char( "&GYou begin the long process of mixing medicines.\n\r", ch);
                   act( AT_PLAIN, "$n begins mixing something.", ch,
                        NULL, argument , TO_ROOM );
                   if(IS_IMMORTAL(ch))
                   add_timer( ch, TIMER_DO_FUN, 1, do_mix, 1 );
                   else if (ch->subclass == SUBCLASS_MEDIC )
                   add_timer ( ch , TIMER_DO_FUN , 9 , do_mix , 1 );
                   else
                   add_timer ( ch , TIMER_DO_FUN , 10 , do_mix , 1 );
                   ch->dest_buf = str_dup(arg); 
                   ch->dest_buf_2 = str_dup(arg2);
                   return;
                }
                send_to_char("&RYou cannot figure out how to handle the medicine.\n\r", ch);
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
                ch->substate = SUB_NONE;
                send_to_char("&RYou are interupted and fail to finish your work.\n\r", ch);
                return;
    }
    ch->substate = SUB_NONE;

    level = IS_NPC(ch) ? ch->skill_level[MEDICAL_ABILITY]
          : (int) (ch->pcdata->learned[gsn_mix]);
    vnum = 10442;

    if ( ( pObjIndex = get_obj_index( vnum ) ) == NULL )
    {
         send_to_char( "&RThe item you are trying to create is missing from the database.\n\rPlease inform the administration.\n\r", ch);
         bug("NULL OBJ, MIX!\n\r");
         return;
    }
    checktool = FALSE;
    checkoven = FALSE;
    checkdrink = FALSE;
    checkmed = FALSE;
    if ( ( obj = get_obj_carry( ch, arg ) ) == NULL )
    {
       send_to_char("&RYou do not have that item.&w\n\r", ch );
       return;
    }
    if(obj->item_type != ITEM_MEDICINE)
    {
       send_to_char("That isn't a medicine. BUG OR LACK OF OBJ.\n\r", ch);
       return;
    }
    med = obj;
    checkmed = TRUE;
    if ( ( obj = get_obj_carry( ch, arg2 ) ) == NULL )
    {
       send_to_char("&RYou do not have that item.&w\n\r", ch );
       return;
    }
    if(obj->item_type != ITEM_MEDICINE)
    {
       send_to_char("That isn't a medicine. BUG OR LACK OF OBJ.\n\r", ch);
       return;
    }
    med2 = obj;
    checkmed = TRUE;
    for ( obj = ch->last_carrying; obj; obj = obj->prev_content )
    {
       if (obj->item_type == ITEM_TOOLKIT)
           checktool = TRUE;
       if (obj->item_type == ITEM_OVEN)
           checkoven = TRUE;
      if ( obj->item_type == ITEM_DRINK_CON && obj->value[1] == 0 && checkdrink == FALSE)
      {
           checkdrink = TRUE;
           separate_obj( obj );
           obj_from_char( obj );
           extract_obj( obj );
      }
    }

    chance = IS_NPC(ch) ? ch->top_level
                : (int) (ch->pcdata->learned[gsn_mix]);
    level = chance;
    if(ch->subclass == SUBCLASS_MEDIC)
    {
       level += 10;
       chance += 10;
    }
    if ( number_percent( ) > chance*2  || ( !checktool ) || (!checkmed)
       || ( !checkoven )  || ( !checkdrink ) )
    {
       send_to_char( "&RYou test a small quantity of your medicine.\n\r", ch);
       send_to_char( "&RThere appears to be unexpected side effects.\n\r", ch);
       send_to_char( "&RIt is unsatisfactory, so you trash it.\n\r", ch);
       learn_from_failure( ch, gsn_mix);
       separate_obj(med);
       obj_from_char(med);
       extract_obj(med);
       separate_obj(med2);
       obj_from_char(med2); 
       extract_obj(med2);
       return;
    }

    obj = create_object( pObjIndex, level );

    obj->item_type = ITEM_MEDICINE;
    SET_BIT( obj->wear_flags, ITEM_HOLD );
    SET_BIT( obj->wear_flags, ITEM_TAKE );
    obj->level = level;
    obj->weight = 1;
    STRFREE( obj->name );
    strcpy( buf, "mixture of " );
    strcat( buf, med->short_descr );
    strcat( buf, " and ");
    strcat( buf, med2->short_descr );
    obj->name = STRALLOC( stripclr( buf ) );
    STRFREE( obj->short_descr );
    obj->short_descr = STRALLOC( buf );
    STRFREE( obj->description );
    strcat( buf, " was left here." );
    obj->description = STRALLOC( buf );
    /* Mix Medicinal Values */
    obj->value[0] = med->value[0];
    obj->value[1] = med->value[1];
    obj->value[2] = med2->value[0];
    obj->value[3] = med2->value[1];
    obj->cost = level + obj->value[1] + obj->value[3] + 2000;
    obj = obj_to_char( obj, ch );
    /* Extract Old Objs */
    separate_obj(med);
    obj_from_char(med);
    extract_obj(med);
    separate_obj(med2);
    obj_from_char(med2);
    extract_obj(med2);
    send_to_char( "&GYou finish your work and hold up your newly prepared mixture.&w\n\r", ch);
    act( AT_PLAIN, "$n finishes preparing a mixture.", ch,
         NULL, argument , TO_ROOM );
    {
         long xpgain;
         xpgain = UMIN( 40000, exp_level(ch->skill_level[MEDICAL_ABILITY]+1)-exp_level(ch->skill_level[MEDICAL_ABILITY]));
         gain_exp(ch, xpgain, MEDICAL_ABILITY);
         ch_printf( ch , "You gain %d medical experience.", xpgain );
    }
    learn_from_success( ch, gsn_mix);
}

void do_loremedicine(CHAR_DATA *ch, char *argument)
{
    OBJ_DATA *obj;
    int mixed = 0;
    int chance;
    if ( argument[0] == '\0' )
    {
      send_to_char( "What medical object would you like to lore?\n\r", ch );
      return;
    }

    if ( ( obj = get_obj_carry( ch, argument ) ) != NULL )
    {
        if(obj->item_type != ITEM_MEDICINE && obj->item_type != ITEM_SHOT
        && obj->item_type != ITEM_INOCULATOR && obj->item_type != ITEM_ELIXIR)
        {
           send_to_char("Thats not a medical object.\n\r", ch); 
           return;
        }
        chance = IS_NPC(ch) ? ch->top_level
                : (int) (ch->pcdata->learned[gsn_loremedicine]);
        if(ch->subclass == SUBCLASS_MEDIC)
           chance += 10;
        if(number_percent( ) > chance )
        {
           send_to_char("You can't seem to identify it at all.\n\r", ch);
           learn_from_failure(ch, gsn_loremedicine);
           return;
        }
        set_char_color( AT_LBLUE, ch );
        ch_printf( ch,
        "Object '%s' is a %s\n\rIts weight is %d, value is %d.\n\r",
        obj->name,
        aoran( item_type_name( obj ) ),
        obj->weight,
        obj->cost
        );
        switch(obj->item_type)
        {
        case ITEM_MEDICINE: 
        case ITEM_SHOT: 
        case ITEM_ELIXIR: 
           switch(obj->value[0])
           {
               default: break;
               case MEDICINE_HP: mixed++;
                  ch_printf(ch, "It has a healing reagent of strength %d\n\r", obj->value[1]); break;
               case MEDICINE_MV: mixed++;
                  ch_printf(ch, "It has a rejuvenation reagent of strength %d\n\r", obj->value[1]); break;
               case MEDICINE_POISON: mixed++;
                  ch_printf(ch, "It has a poison cure reagent of strength %d\n\r", obj->value[1]); break;
               case MEDICINE_BLINDNESS: mixed++;
                  ch_printf(ch, "It has a blindness cure reagent of strength %d\n\r", obj->value[1]); break;
               case MEDICINE_CBLIND:
                  ch_printf(ch, "It causes blindness at a strength of %d\n\r", obj->value[1] ) ; break;
               case MEDICINE_CPOISON:
                  ch_printf(ch, "It causes poison at a strength of %d\n\r", obj->value[1] ); break;
               case MEDICINE_SLEEP:
                  ch_printf(ch, "It causes sleepiness at a strength of %d\n\r", obj->value[1] ); break;
           } 
           switch(obj->value[2])
           {
               default: break;
               case MEDICINE_HP: mixed++;
                  ch_printf(ch, "It has a healing reagent of strength %d\n\r", obj->value[3]); break;
               case MEDICINE_MV: mixed++;
                  ch_printf(ch, "It has a rejuvenation reagent of strength %d\n\r", obj->value[3]); break;
               case MEDICINE_POISON: mixed++;
                  ch_printf(ch, "It has a poison cure reagent of strength %d\n\r", obj->value[3]); break;
               case MEDICINE_BLINDNESS: mixed++;
                  ch_printf(ch, "It has a blindness cure reagent of strength %d\n\r", obj->value[3]); break;
               case MEDICINE_CBLIND:
                  ch_printf(ch, "It causes blindness at a strength of %d\n\r", obj->value[3] ) ; break;
               case MEDICINE_CPOISON:
                  ch_printf(ch, "It causes poison at a strength of %d\n\r", obj->value[3] ); break;
               case MEDICINE_SLEEP:
                  ch_printf(ch, "It causes sleepiness at a strength of %d\n\r", obj->value[3] ); break;               
           }
           if(mixed == 2)
           {
               send_to_char("It is a mixture of two medicines.\n\r", ch);
           }
           if(obj->value[5] != 0)
           {
               send_to_char("It is concentrated.\n\r", ch);
           }
           else
           {
               send_to_char("It is not concentrated.\n\r", ch);
           } 
           learn_from_success(ch, gsn_loremedicine);
           break;
        case ITEM_INOCULATOR: 
           if(obj->value[0] == 0)
           {
              send_to_char("This inoculator is worthless.\n\r", ch);
              learn_from_success(ch, gsn_loremedicine);
              return;
           }
           else
           {
              ch_printf(ch, "This inoculator has %d uses left on it.\n\r", obj->value[0]);
           }
           switch(obj->value[1])
           {
               default: break;
               case MEDICINE_HP: mixed++;
                  ch_printf(ch, "It has a healing reagent of strength %d\n\r", obj->value[2]); break;
               case MEDICINE_MV: mixed++;
                  ch_printf(ch, "It has a rejuvenation reagent of strength %d\n\r", obj->value[2]); break;
               case MEDICINE_POISON: mixed++;
                  ch_printf(ch, "It has a poison cure reagent of strength %d\n\r", obj->value[2]); break;
               case MEDICINE_BLINDNESS: mixed++;
                  ch_printf(ch, "It has a blindness cure reagent of strength %d\n\r", obj->value[2]); break;
               case MEDICINE_CBLIND:
                  ch_printf(ch, "It causes blindness at a strength of %d\n\r", obj->value[2] ) ; break;
               case MEDICINE_CPOISON:
                  ch_printf(ch, "It causes poison at a strength of %d\n\r", obj->value[2] ); break;
               case MEDICINE_SLEEP:
                  ch_printf(ch, "It causes sleepiness at a strength of %d\n\r", obj->value[2] ); break;
           } 
           switch(obj->value[3])
           {
               default: break;
               case MEDICINE_HP: mixed++;
                  ch_printf(ch, "It has a healing reagent of strength %d\n\r", obj->value[4]); break;
               case MEDICINE_MV: mixed++;
                  ch_printf(ch, "It has a rejuvenation reagent of strength %d\n\r", obj->value[4]); break;
               case MEDICINE_POISON: mixed++;
                  ch_printf(ch, "It has a poison cure reagent of strength %d\n\r", obj->value[4]); break;
               case MEDICINE_BLINDNESS: mixed++;
                  ch_printf(ch, "It has a blindness cure reagent of strength %d\n\r", obj->value[4]); break;
               case MEDICINE_CBLIND:
                  ch_printf(ch, "It causes blindness at a strength of %d\n\r", obj->value[4] ) ; break;
               case MEDICINE_CPOISON:
                  ch_printf(ch, "It causes poison at a strength of %d\n\r", obj->value[4] ); break;
               case MEDICINE_SLEEP:
                  ch_printf(ch, "It causes sleepiness at a strength of %d\n\r", obj->value[4] ); break;

           }
           if(mixed == 2)
              send_to_char("It has a combination of two medicines.\n\r", ch);
           learn_from_success(ch, gsn_loremedicine);
           break;
        }
        return;        
    }
    send_to_char("You do not possess that item.\n\r", ch);
    return;
}
