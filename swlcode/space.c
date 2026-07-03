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
*		                Space Module    			   *   
****************************************************************************/
 
#include <math.h> 
#include <sys/types.h>
#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include "mud.h"

extern char * const cargo_names[CARGO_MAX]; 

SHIP_DATA * first_ship;
SHIP_DATA * last_ship;

MISSILE_DATA * first_missile;
MISSILE_DATA * last_missile;

SPACE_DATA * first_starsystem;
SPACE_DATA * last_starsystem;
void fread_modules	args ( ( SHIP_DATA *ship, FILE *fp ) );
void update_ship_modules	args ( ( SHIP_DATA *ship ) );
bool module_type_install	args ( ( OBJ_DATA *obj, SHIP_DATA *ship));
bool check_pilot 	        args ( (CHAR_DATA *ch, SHIP_DATA *ship));
bool evaded                     args ( (SHIP_DATA *ship, SHIP_DATA *target));
CHAR_DATA *pilot_from_ship      args ( (SHIP_DATA *ship));

int bus_pos =0;
int bus_planet =0;
int bus2_planet = 4;
int turbocar_stop =0;
int corus_shuttle =0;
 
/* For docking - Parnic */
#define IS_DOCKED(ship)         ((ship)->docked_ship)

#define MAX_STATION    10
#define MAX_BUS_STOP   14

#define STOP_PLANET     202
#define STOP_SHIPYARD   32015

#define SENATEPAD       10196
#define OUTERPAD        10195

/* Not sure about this function, says to install in ship simulators -Arcturus */
char *ship_bit_name( int vector )
{
    static char buf[512];

    buf[0] = '\0';
	if ( vector & SHIPFLAG_SIMULATOR    ) strcat( buf, " simulator" );
    return ( buf[0] != '\0' ) ? buf+1 : "none";
}

/* For setship purposes */
char *  const   ship_flags [] =
{
"cloaked", "overdrive", "afterburner", "sablaser", "sabions", 
"sabengine", "sabturret1",  "sabturret2", "sablauncher", 
"shieldrlaser",  "shieldrengine", "enginerlaser", "enginershield", 
"laserrengine", "laserrshield",  "duallaser", "trilaser", 
"quadlaser", "dualion", "triion",  "quadion", "simulator",  
"dualmissile", "dualtorpedo", "dualrocket", "sabrlauncher", 
"sabtlauncher", "r28", "r29", "r30", "r31","r32"
};

/* For setship purposes-Arcturus */
int get_shipflag( char *flag )
{
    int x;

    for ( x = 0; x < 32; x++ )
      if ( !str_cmp( flag, ship_flags[x] ) )
        return x;
    return -1;
}

/* Remove Extra Bits Like Linked Lasers on launch, endsimulator, blow up, etc. Not Sabotage, Or Simulator Though*/
void rem_extshipflags(SHIP_DATA *ship)
{
                if(IS_SET(ship->flags, SHIPFLAG_CLOAKED))
                   REMOVE_BIT(ship->flags, SHIPFLAG_CLOAKED);
                if(IS_SET(ship->flags, SHIPFLAG_OVERDRIVENODE))
                   REMOVE_BIT(ship->flags, SHIPFLAG_OVERDRIVENODE);
                if(IS_SET(ship->flags, SHIPFLAG_AFTERBURNER))
                   REMOVE_BIT(ship->flags, SHIPFLAG_AFTERBURNER);
                if(IS_SET(ship->flags, SHIPFLAG_SHIELDRLASER))
                   REMOVE_BIT(ship->flags, SHIPFLAG_SHIELDRLASER);
                if(IS_SET(ship->flags, SHIPFLAG_SHIELDRENGINE))
                   REMOVE_BIT(ship->flags, SHIPFLAG_SHIELDRENGINE);
                if(IS_SET(ship->flags, SHIPFLAG_ENGINERSHIELD))
                   REMOVE_BIT(ship->flags, SHIPFLAG_ENGINERSHIELD);
                if(IS_SET(ship->flags, SHIPFLAG_ENGINERLASER))
                   REMOVE_BIT(ship->flags, SHIPFLAG_ENGINERLASER);
                if(IS_SET(ship->flags, SHIPFLAG_LASERRENGINE))
                   REMOVE_BIT(ship->flags, SHIPFLAG_LASERRENGINE);
                if(IS_SET(ship->flags, SHIPFLAG_LASERRSHIELD))
                   REMOVE_BIT(ship->flags, SHIPFLAG_LASERRSHIELD);
                if(IS_SET(ship->flags, SHIPFLAG_DUALLASER))
                   REMOVE_BIT(ship->flags, SHIPFLAG_DUALLASER);
                if(IS_SET(ship->flags, SHIPFLAG_TRILASER))
                   REMOVE_BIT(ship->flags, SHIPFLAG_TRILASER);
                if(IS_SET(ship->flags, SHIPFLAG_QUADLASER))
                   REMOVE_BIT(ship->flags, SHIPFLAG_QUADLASER);
                if(IS_SET(ship->flags, SHIPFLAG_DUALION))
                   REMOVE_BIT(ship->flags, SHIPFLAG_DUALION);
                if(IS_SET(ship->flags, SHIPFLAG_TRIION))
                   REMOVE_BIT(ship->flags, SHIPFLAG_TRIION);
                if(IS_SET(ship->flags, SHIPFLAG_QUADION))
                   REMOVE_BIT(ship->flags, SHIPFLAG_QUADION);
                if(IS_SET(ship->flags, SHIPFLAG_DUALMISSILE))
                   REMOVE_BIT(ship->flags, SHIPFLAG_DUALMISSILE);
                if(IS_SET(ship->flags, SHIPFLAG_DUALTORPEDO))
                   REMOVE_BIT(ship->flags, SHIPFLAG_DUALTORPEDO);
                if(IS_SET(ship->flags, SHIPFLAG_DUALROCKET))
                   REMOVE_BIT(ship->flags, SHIPFLAG_DUALROCKET);
}


int     const   station_vnum [MAX_STATION] =
{
    215 , 216 , 217 , 218 , 219 , 220 , 221 ,222 , 223 , 224
};

char *  const   station_name [MAX_STATION] =
{
   "Menari Spaceport" , "Skydome Botanical Gardens" , "Grand Towers" , 
   "Grandis Mon Theater" , "Palace Station" , "Great Galactic Museum" , 
   "College Station" , "Holographic Zoo of Extinct Animals" , 
   "Dometown Station " , "Monument Plaza"  
};

int     const   bus_vnum [MAX_BUS_STOP] =
{
    201 ,  21100 , 2102 , 18201 , 29001 , 26500 , 28038 , 31872 , 1001 , 15600 , 
    28613 , 3060 , 23100 , 28247 /* , 32297 */
};

char *  const   bus_stop [MAX_BUS_STOP+1] =
{
  "Coruscant", "Mon Calamari", "Cloning Facilities", 
  "Ajina", "Adari", "Zumonduland", "Gamorr", "Tatooine" , "Honoghr" ,  
  "Ryloth", "Kashyyyk", "Endor", "Bespin", "Byss",  "Coruscant"  /* last should always be same as first */  
};

/* local routines */
void	fread_ship	args( ( SHIP_DATA *ship, FILE *fp ) );
bool	load_ship_file	args( ( char *shipfile ) );
void	write_ship_list	args( ( void ) );
void    fread_starsystem      args( ( SPACE_DATA *starsystem, FILE *fp ) );
bool    load_starsystem  args( ( char *starsystemfile ) );
void    write_starsystem_list args( ( void ) );
void    resetship args( ( SHIP_DATA *ship ) );
void    landship args( ( SHIP_DATA *ship, char *arg ) );
void    launchship args( ( SHIP_DATA *ship ) );
bool    land_bus args( ( SHIP_DATA *ship, int destination ) );
void    launch_bus args( ( SHIP_DATA *ship ) );
void    echo_to_room_dnr args( ( int ecolor , ROOM_INDEX_DATA *room ,  char *argument ) );
ch_ret drive_ship( CHAR_DATA *ch, SHIP_DATA *ship, EXIT_DATA  *exit , int fall );
bool    autofly(SHIP_DATA *ship);
bool is_facing( SHIP_DATA *ship , SHIP_DATA *target );
void sound_to_ship( SHIP_DATA *ship , char *argument );
void    dockship                        args    ( ( SHIP_DATA *ship ) );
float speedbonus(SHIP_DATA *ship);
/* from comm.c */
bool    write_to_descriptor     args( ( int desc, char *txt, int length ) );

ROOM_INDEX_DATA *generate_exit( ROOM_INDEX_DATA *in_room, EXIT_DATA **pexit );

void echo_to_room_dnr ( int ecolor , ROOM_INDEX_DATA *room ,  char *argument ) 
{
    CHAR_DATA *vic;
    
    if ( room == NULL )
    	return;
    	
    for ( vic = room->first_person; vic; vic = vic->next_in_room )
    {
	set_char_color( ecolor, vic );
	send_to_char( argument, vic );
    }
}


bool  land_bus( SHIP_DATA *ship, int destination )
{
    char buf[MAX_STRING_LENGTH];
    
    if ( !ship_to_room( ship , destination ) )
    {
       return FALSE;
    }     
    echo_to_ship( AT_YELLOW , ship , "You feel a slight thud as the ship sets down on the ground."); 
    ship->location = destination;
    ship->lastdoc = ship->location;
    ship->shipstate = SHIP_DOCKED;
    if (ship->starsystem)
        ship_from_starsystem( ship, ship->starsystem );  
    sprintf( buf, "%s lands on the platform.", ship->name );
    echo_to_room( AT_YELLOW , get_room_index(ship->location) , buf );
    sprintf( buf , "The hatch on %s opens." , ship->name);  
    echo_to_room( AT_YELLOW , get_room_index(ship->location) , buf );
    echo_to_room( AT_YELLOW , get_room_index(ship->entrance) , "The hatch opens." );	
    ship->hatchopen = TRUE;
    sound_to_room( get_room_index(ship->entrance) , "!!SOUND(door)" );
    sound_to_room( get_room_index(ship->location) , "!!SOUND(door)" );
    return TRUE;
}

void    launch_bus( SHIP_DATA *ship )
{
      char buf[MAX_STRING_LENGTH];
      
      sound_to_room( get_room_index(ship->entrance) , "!!SOUND(door)" );
       sound_to_room( get_room_index(ship->location) , "!!SOUND(door)" );
      sprintf( buf , "The hatch on %s closes and it begins to launch." , ship->name);  
      echo_to_room( AT_YELLOW , get_room_index(ship->location) , buf );
      echo_to_room( AT_YELLOW , get_room_index(ship->entrance) , "The hatch slides shut." );	
      ship->hatchopen = FALSE;
      extract_ship( ship );
      echo_to_ship( AT_YELLOW , ship , "The ship begins to launch."); 
      ship->location = 0;
      ship->shipstate = SHIP_READY;
}

void update_traffic( )
{
    SHIP_DATA *shuttle, *senate;
    SHIP_DATA *turbocar;
    char       buf[MAX_STRING_LENGTH];

    shuttle = ship_from_cockpit( ROOM_CORUSCANT_SHUTTLE );
    senate = ship_from_cockpit( ROOM_SENATE_SHUTTLE );
    if ( senate != NULL && shuttle != NULL )
    {
        switch (corus_shuttle)
        {
             default:
                corus_shuttle++;
                break;
             
             case 0: 
                land_bus( shuttle , STOP_PLANET );
                land_bus( senate , SENATEPAD );
                corus_shuttle++;
                echo_to_ship( AT_CYAN , shuttle , "Welcome to Menari Spaceport." ); 
                echo_to_ship( AT_CYAN , senate , "Welcome to The Senate Halls." ); 
                break;
                
             case 4:
                launch_bus( shuttle );
                launch_bus( senate );
                corus_shuttle++;
                break;
                
             case 5:
                land_bus( shuttle , STOP_SHIPYARD );
                land_bus( senate , OUTERPAD );
                echo_to_ship( AT_CYAN , shuttle , "Welcome to Coruscant Shipyard." );
                echo_to_ship( AT_CYAN , senate , "Welcome to The Outer System Landing Area." );
                corus_shuttle++;
                break;
                
             case 9:
                launch_bus( shuttle );
                launch_bus( senate );
                corus_shuttle++;
                break;
                        
        }
        
        if ( corus_shuttle >= 10 )
              corus_shuttle = 0;
    }
    
    turbocar = ship_from_cockpit( ROOM_CORUSCANT_TURBOCAR );
    if ( turbocar != NULL )
    {
      	sprintf( buf , "The turbocar doors close and it speeds out of the station.");  
      	echo_to_room( AT_YELLOW , get_room_index(turbocar->location) , buf );
      	extract_ship( turbocar );
      	turbocar->location = 0;
       	ship_to_room( turbocar , station_vnum[turbocar_stop] );
       	echo_to_ship( AT_YELLOW , turbocar , "The turbocar makes a quick journey to the next station."); 
       	turbocar->location = station_vnum[turbocar_stop];
       	turbocar->lastdoc = turbocar->location;
       	turbocar->shipstate = SHIP_DOCKED;
       	if (turbocar->starsystem)
          ship_from_starsystem( turbocar, turbocar->starsystem );  
    	sprintf( buf, "A turbocar pulls into the platform and the doors slide open.");
    	echo_to_room( AT_YELLOW , get_room_index(turbocar->location) , buf );
    	sprintf( buf, "Welcome to %s." , station_name[turbocar_stop] );
    	echo_to_ship( AT_CYAN , turbocar , buf );
        turbocar->hatchopen = TRUE;
        
        turbocar_stop++;
        if ( turbocar_stop >= MAX_STATION )
           turbocar_stop = 0;
    }
       
}

void update_bus( )
{
    SHIP_DATA *ship;
    SHIP_DATA *ship2;
    SHIP_DATA *target;
    int        destination;
    char       buf[MAX_STRING_LENGTH];
    
    ship = ship_from_cockpit( ROOM_SHUTTLE_BUS );
    ship2 = ship_from_cockpit( ROOM_SHUTTLE_BUS_2 );
    
    if ( ship == NULL && ship2 == NULL )
    	return;
    
    switch (bus_pos)
    {  

       case 0:
            target = ship_from_hanger( bus_vnum[bus_planet] );
            if ( target != NULL && !target->starsystem )
            {
               sprintf( buf,  "An electronic voice says, 'Cannot land at %s ... it seems to have dissapeared.'", bus_stop[bus_planet] );
               echo_to_ship( AT_CYAN , ship , buf );    
               bus_pos = 5;
            }   
            
            target = ship_from_hanger( bus_vnum[bus2_planet] );
            if ( target != NULL && !target->starsystem )
            {
               sprintf( buf,  "An electronic voice says, 'Cannot land at %s ... it seems to have dissapeared.'", bus_stop[bus_planet] );
               echo_to_ship( AT_CYAN , ship2 , buf );    
               bus_pos = 5;
            }   

            bus_pos++;
            break;
       
       case 6:
            launch_bus ( ship );
            launch_bus ( ship2 );
            bus_pos++;
            break; 
       
       case 7:
            echo_to_ship( AT_YELLOW , ship , "The ship lurches slightly as it makes the jump to lightspeed.");
            echo_to_ship( AT_YELLOW , ship2 , "The ship lurches slightly as it makes the jump to lightspeed.");
            bus_pos++;
            break; 
       
       case 9:
            
            echo_to_ship( AT_YELLOW , ship , "The ship lurches slightly as it comes out of hyperspace..");
            echo_to_ship( AT_YELLOW , ship2 , "The ship lurches slightly as it comes out of hyperspace..");
            bus_pos++;
            break; 
       
       case 1:
            destination = bus_vnum[bus_planet];
            if ( !land_bus( ship, destination ) )
            {
               sprintf( buf, "An electronic voice says, 'Oh My, %s seems to have dissapeared.'" , bus_stop[bus_planet] );
               echo_to_ship( AT_CYAN , ship , buf ); 
               echo_to_ship( AT_CYAN , ship , "An electronic voice says, 'I do hope it wasn't a superlaser. Landing aborted.'"); 
            }
            else 
            {
               sprintf( buf,  "An electronic voice says, 'Welcome to %s'" , bus_stop[bus_planet] );
               echo_to_ship( AT_CYAN , ship , buf); 
               echo_to_ship( AT_CYAN , ship , "It continues, 'Please exit through the main ramp. Enjoy your stay.'");             
            }
            destination = bus_vnum[bus2_planet];
            if ( !land_bus( ship2, destination ) )
            {
               sprintf( buf, "An electronic voice says, 'Oh My, %s seems to have dissapeared.'" , bus_stop[bus_planet] );
               echo_to_ship( AT_CYAN , ship2 , buf ); 
               echo_to_ship( AT_CYAN , ship2 , "An electronic voice says, 'I do hope it wasn't a superlaser. Landing aborted.'"); 
            }
            else 
            {
               sprintf( buf,  "An electronic voice says, 'Welcome to %s'" , bus_stop[bus2_planet] );
               echo_to_ship( AT_CYAN , ship2 , buf); 
               echo_to_ship( AT_CYAN , ship2 , "It continues, 'Please exit through the main ramp. Enjoy your stay.'");             
            }
            bus_pos++;
            break;
               
       case 5:
            sprintf( buf, "It continues, 'Next stop, %s'" , bus_stop[bus_planet+1] );
            echo_to_ship( AT_CYAN , ship , "An electronic voice says, 'Preparing for launch.'");
            echo_to_ship( AT_CYAN , ship , buf);
            sprintf( buf, "It continues, 'Next stop, %s'" , bus_stop[bus2_planet+1] );
            echo_to_ship( AT_CYAN , ship2 , "An electronic voice says, 'Preparing for launch.'");
            echo_to_ship( AT_CYAN , ship2 , buf);
            bus_pos++;
            break;
                    
       default:
            bus_pos++;
            break;
    }
    
    if ( bus_pos >= 10 )
    {
       bus_pos = 0;
       bus_planet++;
       bus2_planet++;
    }
    
    if ( bus_planet >= MAX_BUS_STOP )
       bus_planet = 0;
    if ( bus2_planet >= MAX_BUS_STOP )
       bus2_planet = 0;
    
}

void move_ships( )
{
   SHIP_DATA *ship;
   MISSILE_DATA *missile;
   MISSILE_DATA *m_next;
   SHIP_DATA *target;
   float dx, dy, dz, change;
   char buf[MAX_STRING_LENGTH];
   char logbuf[MAX_STRING_LENGTH];
   CHAR_DATA *ch;
   bool ch_found = FALSE;
   
   for ( missile = first_missile; missile; missile = m_next )
   {
           m_next = missile->next;
           
              ship = missile->fired_from;
              target = missile->target;
              
              if ( target->starsystem && target->starsystem == missile->starsystem )
              {
                if ( missile->mx < target->vx ) 
                  missile->mx += UMIN( missile->speed/5 , target->vx - missile->mx );
                else if ( missile->mx > target->vx ) 
                  missile->mx -= UMIN( missile->speed/5 , missile->mx - target->vx );  
                if ( missile->my < target->vy ) 
                  missile->my += UMIN( missile->speed/5 , target->vy - missile->my );
                else if ( missile->my > target->vy ) 
                  missile->my -= UMIN( missile->speed/5 , missile->my - target->vy );  
                if ( missile->mz < target->vz ) 
                  missile->mz += UMIN( missile->speed/5 , target->vz - missile->mz );
                else if ( missile->mz > target->vz ) 
                  missile->mz -= UMIN( missile->speed/5 , missile->mz - target->vz );  
              
                if ( abs(missile->mx) - abs(target->vx) <= 20 && abs(missile->mx) - abs(target->vx) >= -20
                && abs(missile->my) - abs(target->vy) <= 20 && abs(missile->my) - abs(target->vy) >= -20
                && abs(missile->mz) - abs(target->vz) <= 20 && abs(missile->mz) - abs(target->vz) >= -20 )
                {  
                   if ( target->chaff_released <= 0)
                   { 
                    if(!evaded(ship, target))
                    {
                    echo_to_room( AT_YELLOW , get_room_index(ship->gunseat), "Your missile hits its target dead on!" );
                    echo_to_cockpit( AT_BLOOD, target, "The ship is hit by a missile.");
                    echo_to_ship( AT_RED , target , "A loud explosion shakes thee ship violently!" );
                    sprintf( buf, "You see a small explosion as %s is hit by a missile" , target->name );
                    echo_to_system( AT_ORANGE , target , buf , ship );
                    for ( ch = first_char; ch; ch = ch->next )
                      if ( !IS_NPC( ch ) && nifty_is_name( missile->fired_by, ch->name ) )
                      {   
                        ch_found = TRUE; 
                        damage_ship_ch( target , 20+missile->missiletype*20 , 
                           30+missile->missiletype*30 , ch, missile->fired_from );
                      }
                    if ( !ch_found )
                       damage_ship( target , 20+missile->missiletype*20 , 
                           30+missile->missiletype*30, missile->fired_from );   
                    extract_missile( missile );
                   }
                   else
                   {
                    echo_to_room( AT_YELLOW , get_room_index(ship->gunseat), "Your missile overshoots its target and explodes!");
                    echo_to_cockpit( AT_YELLOW, target, "You evade a missile.");
                    extract_missile( missile );
                   }
                 }
                   else
                   {
                    echo_to_room( AT_YELLOW , get_room_index(ship->gunseat), "Your missile explodes harmlessly in a cloud of chaff!" );
                    echo_to_cockpit( AT_YELLOW, target, "A missile explodes in your chaff.");
                    extract_missile( missile );
                   }
                   continue;
                }
                else
                {
                   missile->age++;
                   if (missile->age >= 50)
                   {
                      extract_missile( missile );
                      continue;
                   }
                }
              }
              else
              { 
                extract_missile( missile );
                continue;
              }

   }
   
   for ( ship = first_ship; ship; ship = ship->next )
   {
   
     if ( !ship->starsystem )
        continue;
          
     if ( ship->currspeed > 0 )
        {
          
          change = sqrt( ship->hx*ship->hx + ship->hy*ship->hy + ship->hz*ship->hz ); 
         
           if (change > 0)
           {
             dx = ship->hx/change;     
             dy = ship->hy/change;     
             dz = ship->hz/change;
             ship->vx += (dx * ship->currspeed/5);
             ship->vy += (dy * ship->currspeed/5);
             ship->vz += (dz * ship->currspeed/5);
           }
           
        } 
    /* SABOTAGE BIT BY ARCTURUS */
    if(IS_SET(ship->flags,  SHIPFLAG_SABOTAGEDENGINE))
    {
       echo_to_cockpit( AT_BLUE , ship , "There is a small explosion and then your ship stops moving. It must have been sabotage!\n\r");
       echo_to_cockpit( AT_BLOOD + AT_BLINK , ship , "Ships Drive DAMAGED!");
       ship->shipstate = SHIP_DISABLED;
       ship->currspeed = 0;
       REMOVE_BIT(ship->flags, SHIPFLAG_SABOTAGEDENGINE);
       return;
    }
        if ( autofly(ship) )
           continue;
        
/*           
          if ( ship->class != SHIP_PLATFORM && !autofly(ship) )
          {
            if ( ship->starsystem->star1 && strcmp(ship->starsystem->star1,"") )
            {
              if (ship->vx >= ship->starsystem->s1x + 1 || ship->vx <= ship->starsystem->s1x - 1 )
                ship->vx -= URANGE(-3,(ship->starsystem->gravitys1)/(ship->vx - ship->starsystem->s1x)/2,3);
              if (ship->vy >= ship->starsystem->s1y + 1 || ship->vy <= ship->starsystem->s1y - 1 )
                ship->vy -= URANGE(-3,(ship->starsystem->gravitys1)/(ship->vy - ship->starsystem->s1y)/2,3);
              if (ship->vz >= ship->starsystem->s1z + 1 || ship->vz <= ship->starsystem->s1z - 1 )
                ship->vz -= URANGE(-3,(ship->starsystem->gravitys1)/(ship->vz - ship->starsystem->s1z)/2,3);
            }
          
            if ( ship->starsystem->star2 && strcmp(ship->starsystem->star2,"") )
            {
              if (ship->vx >= ship->starsystem->s2x + 1 || ship->vx <= ship->starsystem->s2x - 1 )
                ship->vx -= URANGE(-3,(ship->starsystem->gravitys2)/(ship->vx - ship->starsystem->s2x)/2,3);
              if (ship->vy >= ship->starsystem->s2y + 1 || ship->vy <= ship->starsystem->s2y - 1 )
                ship->vy -= URANGE(-3,(ship->starsystem->gravitys2)/(ship->vy - ship->starsystem->s2y)/2,3);
              if (ship->vz >= ship->starsystem->s2z + 1 || ship->vz <= ship->starsystem->s2z - 1 )
                ship->vz -= URANGE(-3,(ship->starsystem->gravitys2)/(ship->vz - ship->starsystem->s2z)/2,3);
            }
          
            if ( ship->starsystem->planet1 && strcmp(ship->starsystem->planet1,"") )
            {
              if (ship->vx >= ship->starsystem->p1x + 1 || ship->vx <= ship->starsystem->p1x - 1 )
                ship->vx -= URANGE(-3,(ship->starsystem->gravityp1)/(ship->vx - ship->starsystem->p1x)/2,3);
              if (ship->vy >= ship->starsystem->p1y + 1 || ship->vy <= ship->starsystem->p1y - 1 )
                ship->vy -= URANGE(-3,(ship->starsystem->gravityp1)/(ship->vy - ship->starsystem->p1y)/2,3);
              if (ship->vz >= ship->starsystem->p1z + 1 || ship->vz <= ship->starsystem->p1z - 1 )
                ship->vz -= URANGE(-3,(ship->starsystem->gravityp1)/(ship->vz - ship->starsystem->p1z)/2,3);
            }
          
            if ( ship->starsystem->planet2 && strcmp(ship->starsystem->planet2,"") )
            {
              if (ship->vx >= ship->starsystem->p2x + 1 || ship->vx <= ship->starsystem->p2x - 1 )
                 ship->vx -= URANGE(-3,(ship->starsystem->gravityp2)/(ship->vx - ship->starsystem->p2x)/2,3);
              if (ship->vy >= ship->starsystem->p2y + 1 || ship->vy <= ship->starsystem->p2y - 1 )
                ship->vy -= URANGE(-3,(ship->starsystem->gravityp2)/(ship->vy - ship->starsystem->p2y)/2,3);
              if (ship->vz >= ship->starsystem->p2z + 1 || ship->vz <= ship->starsystem->p2z - 1 )
                ship->vz -= URANGE(-3,(ship->starsystem->gravityp2)/(ship->vz - ship->starsystem->p2z)/2,3);
            }
          
            if ( ship->starsystem->planet3 && strcmp(ship->starsystem->planet3,"") )
            {
              if (ship->vx >= ship->starsystem->p3x + 1 || ship->vx <= ship->starsystem->p3x - 1 )
                ship->vx -= URANGE(-3,(ship->starsystem->gravityp3)/(ship->vx - ship->starsystem->p3x)/2,3);
              if (ship->vy >= ship->starsystem->p3y + 1 || ship->vy <= ship->starsystem->p3y - 1 )
                ship->vy -= URANGE(-3,(ship->starsystem->gravityp3)/(ship->vy - ship->starsystem->p3y)/2,3);
              if (ship->vz >= ship->starsystem->p3z + 1 || ship->vz <= ship->starsystem->p3z - 1 )
                ship->vz -= URANGE(-3,(ship->starsystem->gravityp3)/(ship->vz - ship->starsystem->p3z)/2,3);
            }
          }

*/          
/*
          for ( target = ship->starsystem->first_ship; target; target = target->next_in_starsystem)
          { 
                if ( target != ship &&
                    abs(ship->vx - target->vx) < 1 &&
                    abs(ship->vy - target->vy) < 1 &&
                    abs(ship->vz - target->vz) < 1 )
                {
                    ship->collision = target->maxhull;
                    target->collision = ship->maxhull;
                }                   
          }
*/
          if ( ship->starsystem->star1 && strcmp(ship->starsystem->star1,"") && 
                     abs(ship->vx - ship->starsystem->s1x) < 10 && 
                     abs(ship->vy - ship->starsystem->s1y) < 10 &&
                     abs(ship->vz - ship->starsystem->s1z) < 10 )
                {
                    echo_to_cockpit( AT_BLOOD+AT_BLINK, ship, "You fly directly into the sun.");
                    sprintf( buf , "%s flys directly into %s!", ship->name, ship->starsystem->star1); 
                    echo_to_system( AT_ORANGE , ship , buf , NULL );
                    sprintf(logbuf, "%s, the ship, was destroyed by a sun!", ship->name );
                    log_string( logbuf );
                    destroy_ship(ship, NULL);
                    continue;
                }              
          if ( ship->starsystem->star2 && strcmp(ship->starsystem->star2,"") && 
               abs(ship->vx - ship->starsystem->s2x) < 10 && 
                     abs(ship->vy - ship->starsystem->s2y) < 10 &&
                     abs(ship->vz - ship->starsystem->s2z) < 10 )
                {
                    echo_to_cockpit( AT_BLOOD+AT_BLINK, ship, "You fly directly into the sun.");
                    sprintf( buf , "%s flys directly into %s!", ship->name, ship->starsystem->star2); 
                    echo_to_system( AT_ORANGE , ship , buf , NULL );
                    sprintf(logbuf, "%s, the ship, was destroyed by a sun!", ship->name );
                    log_string( logbuf );
                    destroy_ship(ship , NULL);
                    continue;
                }            

        if ( ship->currspeed > 0 )
        {       
          if ( ship->starsystem->planet1 && strcmp(ship->starsystem->planet1,"") && 
               abs(ship->vx - ship->starsystem->p1x) < 10 && 
                     abs(ship->vy - ship->starsystem->p1y) < 10 &&
                     abs(ship->vz - ship->starsystem->p1z) < 10 )
                {
                    sprintf( buf , "You begin orbitting %s.", ship->starsystem->planet1); 
                    echo_to_cockpit( AT_YELLOW, ship, buf);
                    sprintf( buf , "%s begins orbiting %s.", ship->name, ship->starsystem->planet1); 
                    echo_to_system( AT_ORANGE , ship , buf , NULL );
                    ship->currspeed = 0;
                    continue;
                }            
          if ( ship->starsystem->planet2 && strcmp(ship->starsystem->planet2,"") && 
               abs(ship->vx - ship->starsystem->p2x) < 10 && 
                     abs(ship->vy - ship->starsystem->p2y) < 10 &&
                     abs(ship->vz - ship->starsystem->p2z) < 10 )
                {
                    sprintf( buf , "You begin orbitting %s.", ship->starsystem->planet2); 
                    echo_to_cockpit( AT_YELLOW, ship, buf);
                    sprintf( buf , "%s begins orbiting %s.", ship->name, ship->starsystem->planet2); 
                    echo_to_system( AT_ORANGE , ship , buf , NULL );
                    ship->currspeed = 0;
                    continue;
                }            
          if ( ship->starsystem->planet3 && strcmp(ship->starsystem->planet3,"") && 
               abs(ship->vx - ship->starsystem->p3x) < 10 && 
                     abs(ship->vy - ship->starsystem->p3y) < 10 &&
                     abs(ship->vz - ship->starsystem->p3z) < 10 )
                {
                    sprintf( buf , "You begin orbitting %s.", ship->starsystem->planet2); 
                    echo_to_cockpit( AT_YELLOW, ship, buf);
                    sprintf( buf , "%s begins orbiting %s.", ship->name, ship->starsystem->planet2); 
                    echo_to_system( AT_ORANGE , ship , buf , NULL );
                    ship->currspeed = 0;
                    continue;
                }            
        }
   }

   for ( ship = first_ship; ship; ship = ship->next )
       if (ship->collision) 
       {
           echo_to_cockpit( AT_WHITE+AT_BLINK , ship,  "You have collided with another ship!" );
           echo_to_ship( AT_RED , ship , "A loud explosion shakes the ship violently!" );   
           damage_ship( ship , ship->collision , ship->collision, ship );
           ship->collision = 0;
       }
}   

void do_refuel( CHAR_DATA *ch, char *argument )
{
    SHIP_DATA *ship;
    if ( ch->gold <= 499 )
    {
      send_to_char( "&RInsufficient funds to refuel ship&w\n\r", ch );
	return;
    }
    if ( ( ship = ship_from_cockpit( ch->in_room->vnum)) )
    {
    if ( ship->target0 || ship->target1 || ship->target2 )
     {
    	 send_to_char("&RNot while the ship is enganged with an enemy!\n\r",ch);
    	 return;
     }
     if ( ship->energy < ship->maxenergy)
     {	
      ship->energy += (( ship->class + 1) * 75 );
 
      send_to_char( "&gYou refuel your ship a little.\n\r", ch );
      send_to_char( "&gFunny of how you got he fuel from that tank behind the storage room. :)\n\r", ch );
      ch->gold -= 500;
}
else
{
return;
}
}
}
void recharge_ships( )
{
   SHIP_DATA *ship;
   char buf[MAX_STRING_LENGTH];
   int bonuscharge; /*For redirection */
   int change; /* For Fuel Consumption, in Overcharging */
   bonuscharge = 0;
   change = 0;
   for ( ship = first_ship; ship; ship = ship->next )
   {                
      /* Old recharge code      
        if (ship->statet0 > 0)
        {
           ship->energy -= ship->statet0;
           ship->statet0 = 0;
        }
     */
        if (ship->statet1 > 0)
        {
           ship->energy -= ship->statet1;
           ship->statet1 = 0;
        }
        if (ship->statet2 > 0)
        {
           ship->energy -= ship->statet2;
           ship->statet2 = 0;
        }
     /* if (ship->ionstate > 0)
        {
           ship->energy -= ship->statet2;
           ship->ionstate = 0;
        }
     */
      /* Replacement for overcharging. */
      bonuscharge = 0;
      change = 0;
      if(IS_SET(ship->flags, SHIPFLAG_SHIELDRLASER))
         bonuscharge += ship->lasers / 2;
      if(IS_SET(ship->flags, SHIPFLAG_ENGINERLASER))
         bonuscharge += ship->lasers /2;
      if(IS_SET(ship->flags, SHIPFLAG_LASERRSHIELD))
         bonuscharge -= ship->lasers /2;
      if(IS_SET(ship->flags, SHIPFLAG_LASERRENGINE))
         bonuscharge -= ship->lasers /2;
     if(!(ship->statet0 == LASER_DAMAGED))
     { 
        change = ship->statet0;
        ship->statet0 -= (ship->lasers + bonuscharge);
        if(abs(ship->statet0) >= (ship->lasers+bonuscharge))
        {
           if(ship->statet0 < 0)/* Could be. */
             ship->statet0 = (ship->lasers +bonuscharge) * -1;
           else /* Shouldn't be. */
             ship->statet0 = (ship->lasers +bonuscharge);
        }
        if(ship->statet0 <= LASER_DAMAGED)/*Shouldn't happen but could with large craft. */
           ship->statet0 = LASER_DAMAGED + 1; /*Just Over the Mark */
        ship->energy = ship->energy - (abs(ship->statet0) + change);
     }
      bonuscharge = 0;
      change = 0;
      if(IS_SET(ship->flags, SHIPFLAG_SHIELDRLASER))
         bonuscharge += ship->ions / 2;
      if(IS_SET(ship->flags, SHIPFLAG_ENGINERLASER))
         bonuscharge += ship->ions /2;
      if(IS_SET(ship->flags, SHIPFLAG_LASERRSHIELD))
         bonuscharge -= ship->ions /2;
      if(IS_SET(ship->flags, SHIPFLAG_LASERRENGINE))
         bonuscharge -= ship->ions /2;
     if(!(ship->ionstate == LASER_DAMAGED))
     {
        change = ship->ionstate;
        ship->ionstate -= (ship->ions + bonuscharge);
        if(abs(ship->ionstate) >= (ship->ions+bonuscharge))
        {
           if(ship->ionstate < 0)/*  Probably. */
             ship->ionstate = (ship->ions +bonuscharge) * -1;
           else /* Shouldn't be. */
             ship->ionstate = (ship->ions +bonuscharge);
        }
        if(ship->ionstate <= LASER_DAMAGED) /* Shouldn't happen, but could with larger craft */
           ship->ionstate = LASER_DAMAGED + 1; /* Just over the mark */
        ship->energy = ship->energy - (abs(ship->ionstate) + change);
      }

/* Stock Reload Code 
        if (ship->missilestate == MISSILE_RELOAD_2)
        {
           ship->missilestate = MISSILE_READY;
           if ( ship->missiles > 0 )
               echo_to_room( AT_YELLOW, get_room_index(ship->gunseat), 
               "Missile launcher reloaded.");
        }
        
        if (ship->missilestate == MISSILE_RELOAD )
        {
           ship->missilestate = MISSILE_RELOAD_2;
        }
        
        if (ship->missilestate == MISSILE_FIRED )
           ship->missilestate = MISSILE_RELOAD;
 */
 /* Arcturus New Missile Reload Code */
if(ship->missilestate > MISSILE_READY)
{
   ship->missilestate = MISSILE_READY;/* For Reloading */
      if ( ship->missiles > 0 )
          echo_to_room( AT_YELLOW, get_room_index(ship->gunseat), "Missile launcher reloaded.");
}
if(ship->torpedostate > MISSILE_READY)
{
   ship->torpedostate = MISSILE_READY; /* For Reloading */
      if(ship->torpedos > 0)
          echo_to_room( AT_YELLOW, get_room_index(ship->gunseat), "Torpedo launcher reloaded.");
}
if(ship->rocketstate > MISSILE_READY)
{
   ship->rocketstate = MISSILE_READY; /* For Reloading */
      if ( ship->rockets > 0 )
          echo_to_room( AT_YELLOW, get_room_index(ship->gunseat), "Rocket launcher reloaded.");
}
       if ( autofly(ship) )
       {
          if ( ship->starsystem )
          {
             if (ship->target0 && ship->statet0 != LASER_DAMAGED )
             {
                 int chance = 50;
                 SHIP_DATA * target = ship->target0;
                 int shots;
                 
                for ( shots=0 ; shots <= ship->lasers ; shots++ ) 
                {   
                  if (ship->shipstate != SHIP_HYPERSPACE && ship->energy > 25 
                  && ship->target0->starsystem == ship->starsystem
                  && abs(target->vx - ship->vx) <= 1000 
                  && abs(target->vy - ship->vy) <= 1000
                  && abs(target->vz - ship->vz) <= 1000 
                  && ship->statet0 < ship->lasers 
                  && !IS_SET(target->flags, SHIPFLAG_CLOAKED) )
                  {
                    if ( ship->class > 1 || is_facing ( ship , target ) )
                    {
                       chance = 50;
                       chance += (ship->manuever - target->manuever) / 10;
                       chance += (ship->currspeed - target->currspeed) / 10;
                       chance -= ( abs(target->vx - ship->vx)/120 );
                       chance -= ( abs(target->vy - ship->vy)/120 );
                       chance -= ( abs(target->vz - ship->vz)/120 );
                       chance = URANGE(10, chance, 90);
             
                     if(evaded(ship, target))
                     {
                     sprintf( buf , "Lasers that fire from %s at you are evaded." , ship->name);
                     echo_to_cockpit( AT_ORANGE , target , buf );
                     sprintf( buf, "Laserfire from %s barely misses %s." , ship->name , target->name );
                     echo_to_system( AT_ORANGE , ship , buf , target );
             	     }
                     else if ( number_percent( ) > chance )
             		{	  
           		    sprintf( buf , "%s fires at you but misses." , ship->name);  
             		    echo_to_cockpit( AT_ORANGE , target , buf );
      	                    sprintf( buf, "Laserfire from %s barely misses %s." , ship->name , target->name );
                            echo_to_system( AT_ORANGE , target , buf , NULL );	
             		} 
             		else
             		{
             		    sprintf( buf, "Laserfire from %s hits %s." , ship->name, target->name );
             		    echo_to_system( AT_ORANGE , target , buf , NULL );
                            sprintf( buf , "You are hit by lasers from %s!" , ship->name);  
                            echo_to_cockpit( AT_BLOOD , target , buf );           
                            echo_to_ship( AT_RED , target , "A small explosion vibrates through the ship." );           
                            damage_ship( target , ship->laserdamage/2 , ship->laserdamage, ship );
                        } 
                        ship->statet0++;
                    }
                  }
                }
             }
          }
       }       
   }
}
                     



void update_space( )
{
   SHIP_DATA *ship;
   SHIP_DATA *target;
   char buf[MAX_STRING_LENGTH];
   int too_close, target_too_close;
   int recharge;
 
   for ( ship = first_ship; ship; ship = ship->next )
   {    
        if (ship->starsystem)
        {
          if ( ship->energy > 0 )
          {
             ship->energy += ( 5 + ship->class*5 );
             if(IS_SET(ship->flags, SHIPFLAG_AFTERBURNER))
             {
                ship->energy -= (5 + ship->class*5);
             }
             if(IS_SET(ship->flags, SHIPFLAG_OVERDRIVENODE))
             {
                ship->energy -= (5 + ship->class*5)*2;
             }
             if(IS_SET(ship->flags, SHIPFLAG_CLOAKED) )
                ship->energy -= (5 + ship->class*5)*3;
          }
          else 
             destroy_ship(ship , NULL);
        }
        if(IS_DOCKED(ship))
        {
           if(!get_ship_here(ship->docked_ship->name, ship->starsystem))
              ship->shipstate2 = SHIP_READY;   
        }
        if ( ship->chaff_released > 0 )
           ship->chaff_released--;
                
        if (ship->shipstate == SHIP_HYPERSPACE)
        {
            ship->hyperdistance -= ship->hyperspeed*2;
            if (ship->hyperdistance <= 0)
            {
            	ship_to_starsystem (ship, ship->currjump);
            	
            	if (ship->starsystem == NULL)
            	{
            	    echo_to_cockpit( AT_RED, ship, "Ship lost in Hyperspace. Make new calculations.");
            	}
            	else
            	{
            	    echo_to_room( AT_YELLOW, get_room_index(ship->pilotseat), "Hyperjump complete.");
            	    echo_to_ship( AT_YELLOW, ship, "The ship lurches slightly as it comes out of hyperspace.");
            	    sprintf( buf ,"%s enters the starsystem at %.0f %.0f %.0f" , ship->name, ship->vx, ship->vy, ship->vz );
            	    echo_to_system( AT_YELLOW, ship, buf , NULL );
            	    ship->shipstate = SHIP_READY;
            	    STRFREE( ship->home );
            	    ship->home = STRALLOC( ship->starsystem->name );
            	    if ( str_cmp("Public",ship->owner) )
                        save_ship(ship);
                    
            	}
            }
            else
            {
               sprintf( buf ,"%d" , ship->hyperdistance );
               echo_to_room_dnr( AT_YELLOW , get_room_index(ship->pilotseat), "Remaining jump distance: " );
               echo_to_room( AT_WHITE , get_room_index(ship->pilotseat),  buf );
               
            }
        }
        
        /* following was originaly to fix ships that lost their pilot 
           in the middle of a manuever and are stuck in a busy state 
           but now used for timed maneuvers such as turning */
    
    	if (ship->shipstate == SHIP_BUSY_3)
           {
              echo_to_room( AT_YELLOW, get_room_index(ship->pilotseat), "Manuever complete.");  
              ship->shipstate = SHIP_READY;
           }
        if (ship->shipstate == SHIP_BUSY_2)
           ship->shipstate = SHIP_BUSY_3;
        if (ship->shipstate == SHIP_BUSY)
           ship->shipstate = SHIP_BUSY_2;

        if (ship->shipstate2 == SHIP_DOCK_2)
           dockship( ship );
        if (ship->shipstate2 == SHIP_DOCK)
           ship->shipstate2 = SHIP_DOCK_2;
        
        if (ship->shipstate == SHIP_LAND_2)
           landship( ship , ship->dest );
        if (ship->shipstate == SHIP_LAND)
           ship->shipstate = SHIP_LAND_2;
        
        if (ship->shipstate == SHIP_LAUNCH_2)
           launchship( ship );
        if (ship->shipstate == SHIP_LAUNCH)
           ship->shipstate = SHIP_LAUNCH_2;
        

        if (ship->sdestnum == 0)
        {
           ship->sdestnum++;
           echo_to_ship(AT_DANGER, ship, "The ship's self-destruct timer reads 3...");
        }
        else if (ship->sdestnum == 1)
        {
           ship->sdestnum++;
           echo_to_ship(AT_DANGER, ship, "The ship's self-destruct timer reads 2...");
        }
        else if (ship->sdestnum == 2)
        {
           ship->sdestnum++;
           echo_to_ship(AT_DANGER, ship, "The ship's self-destruct timer reads 1...");
        }
        else if (ship->sdestnum == 3)
        {
           echo_to_ship(AT_DANGER, ship, "The ship's self-destruct timer reaches 0...");
           ship->sdestnum = -1;
           destroy_ship( ship, NULL );
           continue;
        }
        
        ship->shield = UMAX( 0 , ship->shield-1-ship->class);
                
        if (ship->autorecharge && ship->maxshield > ship->shield && ship->energy > 100)
        {
           recharge  = UMIN( ship->maxshield-ship->shield, 10 + ship->class*10 );           
           recharge  = UMIN( recharge , ship->energy/2 -100 );
           recharge  = UMAX( 1, recharge );
           if(IS_SET(ship->flags, SHIPFLAG_SHIELDRLASER) || IS_SET(ship->flags, SHIPFLAG_SHIELDRENGINE))
           {
              recharge = (int) recharge * .75;
              recharge = UMAX(1, recharge);
           }
           if(IS_SET(ship->flags, SHIPFLAG_LASERRSHIELD))
           {
              recharge = (int) recharge * 1.25;
              recharge = UMAX(1, recharge);
           }
           if(IS_SET(ship->flags, SHIPFLAG_ENGINERSHIELD))
           {
              recharge = (int) recharge * 1.25;
              recharge = UMAX(1, recharge);
           }
           recharge = UMIN(recharge, ship->maxshield); 
           ship->shield += recharge;
           ship->energy -= recharge;        
        }
        
        if (ship->shield > 0)
        {
          if (ship->energy < 200)
          {
          	ship->shield = 0;
          	echo_to_cockpit( AT_RED, ship,"The ships shields fizzle and die.");
                ship->autorecharge = FALSE;
          }
        }        
        
        if ( ship->starsystem && ship->currspeed > 0 )
        {
               sprintf( buf, "%d",
                          ship->currspeed );
               echo_to_room_dnr( AT_BLUE , get_room_index(ship->pilotseat),  "Speed: " );
               echo_to_room_dnr( AT_LBLUE , get_room_index(ship->pilotseat),  buf );
               sprintf( buf, "%.0f %.0f %.0f",
                           ship->vx , ship->vy, ship->vz );
               echo_to_room_dnr( AT_BLUE , get_room_index(ship->pilotseat),  "  Coords: " );
               echo_to_room( AT_LBLUE , get_room_index(ship->pilotseat),  buf );
            if ( ship->pilotseat != ship->coseat )
            {
               sprintf( buf, "%d",
                          ship->currspeed );
               echo_to_room_dnr( AT_BLUE , get_room_index(ship->coseat),  "Speed: " );
               echo_to_room_dnr( AT_LBLUE , get_room_index(ship->coseat),  buf );
               sprintf( buf, "%.0f %.0f %.0f",
                           ship->vx , ship->vy, ship->vz );
               echo_to_room_dnr( AT_BLUE , get_room_index(ship->coseat),  "  Coords: " );
               echo_to_room( AT_LBLUE , get_room_index(ship->coseat),  buf );
            }
        } 

        if ( ship->starsystem )
        {
          too_close = ship->currspeed + 50;
          for ( target = ship->starsystem->first_ship; target; target = target->next_in_starsystem)
          { 
            target_too_close = too_close+target->currspeed;
            if ( target != ship &&
                 abs(ship->vx - target->vx) < target_too_close &&
                 abs(ship->vy - target->vy) < target_too_close &&
                 abs(ship->vz - target->vz) < target_too_close )
            {
                sprintf( buf, "Proximity alert: %s  %.0f %.0f %.0f"
                            , target->name, target->vx, target->vy, target->vz);
                echo_to_room( AT_RED , get_room_index(ship->pilotseat),  buf );    
            }                
          }    
          too_close = ship->currspeed + 100;
          if ( ship->starsystem->star1 &&  strcmp(ship->starsystem->star1,"") && 
               abs(ship->vx - ship->starsystem->s1x) < too_close && 
               abs(ship->vy - ship->starsystem->s1y) < too_close &&
               abs(ship->vz - ship->starsystem->s1z) < too_close )
          {
                sprintf( buf, "Proximity alert: %s  %d %d %d", ship->starsystem->star1,
                         ship->starsystem->s1x, ship->starsystem->s1y, ship->starsystem->s1z);
                echo_to_room( AT_RED , get_room_index(ship->pilotseat),  buf );
          }               
          if ( ship->starsystem->star2 && strcmp(ship->starsystem->star2,"") && 
               abs(ship->vx - ship->starsystem->s2x) < too_close && 
               abs(ship->vy - ship->starsystem->s2y) < too_close &&
               abs(ship->vz - ship->starsystem->s2z) < too_close )
          {
                sprintf( buf, "Proximity alert: %s  %d %d %d", ship->starsystem->star2,
                         ship->starsystem->s2x, ship->starsystem->s2y, ship->starsystem->s2z);
                echo_to_room( AT_RED , get_room_index(ship->pilotseat),  buf );
          }               
          if ( ship->starsystem->planet1 && strcmp(ship->starsystem->planet1,"") && 
               abs(ship->vx - ship->starsystem->p1x) < too_close && 
               abs(ship->vy - ship->starsystem->p1y) < too_close &&
               abs(ship->vz - ship->starsystem->p1z) < too_close )
          {
                sprintf( buf, "Proximity alert: %s  %d %d %d", ship->starsystem->planet1,
                         ship->starsystem->p1x, ship->starsystem->p1y, ship->starsystem->p1z);
                echo_to_room( AT_RED , get_room_index(ship->pilotseat),  buf );
          }               
          if ( ship->starsystem->planet2 && strcmp(ship->starsystem->planet2,"") && 
               abs(ship->vx - ship->starsystem->p2x) < too_close && 
               abs(ship->vy - ship->starsystem->p2y) < too_close &&
               abs(ship->vz - ship->starsystem->p2z) < too_close )
          {
                sprintf( buf, "Proximity alert: %s  %d %d %d", ship->starsystem->planet2,
                         ship->starsystem->p2x, ship->starsystem->p2y, ship->starsystem->p2z);
                echo_to_room( AT_RED , get_room_index(ship->pilotseat),  buf );
          }               
          if ( ship->starsystem->planet3 && strcmp(ship->starsystem->planet3,"") && 
               abs(ship->vx - ship->starsystem->p3x) < too_close && 
               abs(ship->vy - ship->starsystem->p3y) < too_close &&
               abs(ship->vz - ship->starsystem->p3z) < too_close )
          {
                sprintf( buf, "Proximity alert: %s  %d %d %d", ship->starsystem->planet3,
                         ship->starsystem->p3x, ship->starsystem->p3y, ship->starsystem->p3z);
                echo_to_room( AT_RED , get_room_index(ship->pilotseat),  buf );
          }               
        }
       

        if (ship->target0)
        {               
               sprintf( buf, "%s   %.0f %.0f %.0f", ship->target0->name,
                          ship->target0->vx , ship->target0->vy, ship->target0->vz );    
               echo_to_room_dnr( AT_BLUE , get_room_index(ship->gunseat), "Target: " );
               echo_to_room( AT_LBLUE , get_room_index(ship->gunseat),  buf );
               if (ship->starsystem != ship->target0->starsystem)
               		ship->target0 = NULL;
         }
         
        if (ship->target1)
        {
               sprintf( buf, "%s   %.0f %.0f %.0f", ship->target1->name,
                          ship->target1->vx , ship->target1->vy, ship->target1->vz );
               echo_to_room_dnr( AT_BLUE , get_room_index(ship->turret1), "Target: " );
               echo_to_room( AT_LBLUE , get_room_index(ship->turret1),  buf );
               if (ship->starsystem != ship->target1->starsystem)
               		ship->target1 = NULL;
         }
        
        if (ship->target2)
        {
               sprintf( buf, "%s   %.0f %.0f %.0f", ship->target2->name,
                          ship->target2->vx , ship->target2->vy, ship->target2->vz );
               echo_to_room_dnr( AT_BLUE , get_room_index(ship->turret2), "Target: " );
               echo_to_room( AT_LBLUE , get_room_index(ship->turret2),  buf );
               if (ship->starsystem != ship->target2->starsystem)
               		ship->target2 = NULL;
         }
         
   	if (ship->energy < 100 && ship->starsystem )
   	{
   	    echo_to_cockpit( AT_RED , ship,  "Warning: Ship fuel low." );
        }
                    
        ship->energy = URANGE( 0 , ship->energy, ship->maxenergy );
   } 

   for ( ship = first_ship; ship; ship = ship->next )
   {
       
       if (ship->autotrack && ship->target0 && ship->class <= MOBILE_SUIT )    
       {
           target = ship->target0;
           too_close = ship->currspeed + 10; 
           target_too_close = too_close+target->currspeed;
           if ( target != ship && ship->shipstate == SHIP_READY && 
                 abs(ship->vx - target->vx) < target_too_close &&
                 abs(ship->vy - target->vy) < target_too_close &&
                 abs(ship->vz - target->vz) < target_too_close )
           {
              ship->hx = 0-(ship->target0->vx - ship->vx);
              ship->hy = 0-(ship->target0->vy - ship->vy);
              ship->hz = 0-(ship->target0->vz - ship->vz);
              ship->energy -= ship->currspeed/10;
              echo_to_room( AT_RED , get_room_index(ship->pilotseat), "Autotrack: Evading to avoid collision!\n\r" );  
    	      if ( ship->class == FIGHTER_SHIP || ( ship->class == MIDSIZE_SHIP && ship->manuever > 50 ) )
        	ship->shipstate = SHIP_BUSY_3;
              else if ( ship->class == MIDSIZE_SHIP || ( ship->class > MIDSIZE_SHIP && ship->manuever > 50 ) )
        	ship->shipstate = SHIP_BUSY_2;
    	      else
        	ship->shipstate = SHIP_BUSY;     
           }
           else if  ( !is_facing(ship, ship->target0) )
           {
              ship->hx = ship->target0->vx - ship->vx;
              ship->hy = ship->target0->vy - ship->vy;
              ship->hz = ship->target0->vz - ship->vz;
              ship->energy -= ship->currspeed/10;
              echo_to_room( AT_BLUE , get_room_index(ship->pilotseat), "Autotracking target ... setting new course.\n\r" );      
    	      if ( ship->class == FIGHTER_SHIP || ( ship->class == MIDSIZE_SHIP && ship->manuever > 50 ) )
        	ship->shipstate = SHIP_BUSY_3;
              else if ( ship->class == MIDSIZE_SHIP || ( ship->class > MIDSIZE_SHIP && ship->manuever > 50 ) )
        	ship->shipstate = SHIP_BUSY_2;
    	      else
        	ship->shipstate = SHIP_BUSY;     
           }
       }

       if ( autofly(ship) )
       {
          if ( ship->starsystem )
          {
             if (ship->target0)
             {
                 int chance = 50;
             
                 /* auto assist ships */
                  
                 for ( target = ship->starsystem->first_ship; target; target = target->next_in_starsystem)
                 {
                    if ( autofly(target) )
                       if ( !str_cmp ( target->owner , ship->owner ) && target != ship )
                         if ( target->target0 == NULL && ship->target0 != target )
                         {  
                           target->target0 = ship->target0;
                           sprintf( buf , "You are being targetted by %s." , target->name);  
                           echo_to_cockpit( AT_BLOOD , target->target0 , buf );
                           break;
                        }   
                 }
                   
                 target = ship->target0;
                 ship->autotrack = TRUE;
                 if( ship->class != SHIP_PLATFORM )
                      ship->currspeed = ship->realspeed;
                 if ( ship->energy >200  )
                    ship->autorecharge=TRUE;
                    
                 
                 if (ship->shipstate != SHIP_HYPERSPACE && ship->energy > 25 
                 && (((int)(ship->missilestate/3)) < ship->mlaunchers)  && ship->target0->starsystem == ship->starsystem
                 && abs(target->vx - ship->vx) <= 1200 
                 && abs(target->vy - ship->vy) <= 1200
                 && abs(target->vz - ship->vz) <= 1200
                 && ship->missiles > 0 )
                 {
                   if ( ship->class > 1 || is_facing( ship , target ) )
                   {
                       chance += (ship->manuever - target->manuever) / 10;
                       chance += (ship->currspeed - target->currspeed) / 10;
                       chance -= ( abs(target->vx - ship->vx)/250 );
                       chance -= ( abs(target->vy - ship->vy)/250 );
                       chance -= ( abs(target->vz - ship->vz)/250 );
                       chance = URANGE( 10 , chance , 90 );

             		if ( number_percent( ) > chance )
             		{
             		} 
                        else
                        {       
                          new_missile( ship , target , NULL , CONCUSSION_MISSILE );
                          ship->missiles-- ;
             		        sprintf( buf , "Incoming missile from %s." , ship->name);  
             		        echo_to_cockpit( AT_BLOOD , target , buf );
             		        sprintf( buf, "%s fires a missile towards %s." , ship->name, target->name );
             	          echo_to_system( AT_ORANGE , target , buf , NULL );
                          ship->missilestate += 3;
                        }
                   }
                 }
                 if(ship->type == MOB_SHIP || ship->class == SHIP_PLATFORM)
                 {
                    if( ship->missilestate ==  MISSILE_DAMAGED )
                        ship->missilestate =  MISSILE_READY;
                    if( ship->statet0 ==  LASER_DAMAGED )
                        ship->statet0 =  LASER_READY;
                    if( ship->shipstate ==  SHIP_DISABLED )
                        ship->shipstate =  SHIP_READY;
                    if( ship->torpedostate == MISSILE_DAMAGED )
                        ship->torpedostate = MISSILE_READY;
                    if( ship->rocketstate == MISSILE_DAMAGED )
                        ship->rocketstate = MISSILE_READY;
                }             
             }
             else
             {
                 ship->currspeed = 0;
                 
                 if ( !str_cmp( ship->owner , "The Empire" ) )
                   for ( target = first_ship; target; target = target->next )
                     if ( ship->starsystem == target->starsystem )
                        if ( !str_cmp( target->owner , "New Republic" ) )       
                        {  
                          ship->target0 = target; 
                          sprintf( buf , "You are being targetted by %s." , ship->name);  
                          echo_to_cockpit( AT_BLOOD , target , buf );
                          break;
                        }
                if ( !str_cmp( ship->owner , "New Republic" ) )
                   for ( target = first_ship; target; target = target->next )
                     if ( ship->starsystem == target->starsystem )
                        if ( !str_cmp( target->owner , "The Empire" ) )
                        {  
                          sprintf( buf , "You are being targetted by %s." , ship->name);  
                          echo_to_cockpit( AT_BLOOD , target , buf );
                          ship->target0 = target;
                          break;
                        }
               
               if ( !str_cmp( ship->owner , "Pirates" ) )
                   for ( target = first_ship; target; target = target->next )
                     if ( ship->starsystem == target->starsystem )
                     {  
                          sprintf( buf , "You are being targetted by %s." , ship->name);  
                          echo_to_cockpit( AT_BLOOD , target , buf );
                          ship->target0 = target;
                          break;
                     }         
                        
             }
          }   
          else
          {
               if ( number_range(1, 25) == 25 )
               {
          	  ship_to_starsystem(ship, starsystem_from_name(ship->home) );  
          	  ship->vx = number_range( -5000 , 5000 );
          	  ship->vy = number_range( -5000 , 5000 );
          	  ship->vz = number_range( -5000 , 5000 );
                  ship->hx = 1;
                  ship->hy = 1;
                  ship->hz = 1;
               }
          }   
       }
       
       if ( ( ship->class >= FRIGATE_SHIP && ship->class <= SHIP_PLATFORM )  
       && ship->target0 == NULL )
       {
          if( ship->missiles < ship->maxmissiles )
             ship->missiles++;
          if( ship->torpedos < ship->maxtorpedos )
             ship->torpedos++;
          if( ship->rockets < ship->maxrockets )
             ship->rockets++;
       }
   }

}



void write_starsystem_list( )
{
    SPACE_DATA *tstarsystem;
    FILE *fpout;
    char filename[256];
    
    sprintf( filename, "%s%s", SPACE_DIR, SPACE_LIST );
    fpout = fopen( filename, "w" );
    if ( !fpout )
    {
         bug( "FATAL: cannot open starsystem.lst for writing!\n\r", 0 );
         return;
    }
    for ( tstarsystem = first_starsystem; tstarsystem; tstarsystem = tstarsystem->next )
    fprintf( fpout, "%s\n", tstarsystem->filename );
    fprintf( fpout, "$\n" );
    fclose( fpout );
}
                                                                    

/*
 * Get pointer to space structure from starsystem name.
 */
SPACE_DATA *starsystem_from_name( char *name )
{
    SPACE_DATA *starsystem;
    
    for ( starsystem = first_starsystem; starsystem; starsystem = starsystem->next )
       if ( !str_cmp( name, starsystem->name ) )
         return starsystem;
    
    for ( starsystem = first_starsystem; starsystem; starsystem = starsystem->next )
       if ( !str_prefix( name, starsystem->name ) )
         return starsystem;
    
    return NULL;
}

/*
 * Get pointer to space structure from the dock vnun.
 */
SPACE_DATA *starsystem_from_vnum( int vnum )
{
    SPACE_DATA *starsystem;
    SHIP_DATA *ship;
    
    for ( starsystem = first_starsystem; starsystem; starsystem = starsystem->next )
       if ( vnum == starsystem->doc1a || vnum == starsystem->doc2a    || vnum == starsystem->doc3a ||
            vnum == starsystem->doc1b || vnum == starsystem->doc2b    || vnum == starsystem->doc3b ||   
            vnum == starsystem->doc1c || vnum == starsystem->doc2c    || vnum == starsystem->doc3c )
         return starsystem;

    for ( ship = first_ship; ship; ship = ship->next )
       if ( vnum == ship->hanger )
            return ship->starsystem;
                                
    return NULL;
}


/*
 * Save a starsystem's data to its data file
 */
void save_starsystem( SPACE_DATA *starsystem )
{
    FILE *fp;
    char filename[256];
    char buf[MAX_STRING_LENGTH];

    if ( !starsystem )
    {
	bug( "save_starsystem: null starsystem pointer!", 0 );
	return;
    }
        
    if ( !starsystem->filename || starsystem->filename[0] == '\0' )
    {
	sprintf( buf, "save_starsystem: %s has no filename", starsystem->name );
	bug( buf, 0 );
	return;
    }
 
    sprintf( filename, "%s%s", SPACE_DIR, starsystem->filename );
    
    fclose( fpReserve );
    if ( ( fp = fopen( filename, "w" ) ) == NULL )
    {
    	bug( "save_starsystem: fopen", 0 );
    	perror( filename );
    }
    else
    {
	fprintf( fp, "#SPACE\n" );
	fprintf( fp, "Name         %s~\n",	starsystem->name	);
	fprintf( fp, "Filename     %s~\n",	starsystem->filename	);
	fprintf( fp, "Planet1      %s~\n",	starsystem->planet1	);
	fprintf( fp, "Planet2      %s~\n",	starsystem->planet2	);
	fprintf( fp, "Planet3      %s~\n",	starsystem->planet3	);
	fprintf( fp, "Star1        %s~\n",	starsystem->star1	);
	fprintf( fp, "Star2        %s~\n",	starsystem->star2	);
	fprintf( fp, "Location1a      %s~\n",	starsystem->location1a	);
	fprintf( fp, "Location1b      %s~\n",	starsystem->location1b	);
	fprintf( fp, "Location1c      %s~\n",	starsystem->location1c	);
	fprintf( fp, "Location2a       %s~\n",	starsystem->location2a	);
	fprintf( fp, "Location2b      %s~\n",	starsystem->location2b	);
	fprintf( fp, "Location2c      %s~\n",	starsystem->location2c	);
	fprintf( fp, "Location3a      %s~\n",	starsystem->location3a	);
	fprintf( fp, "Location3b      %s~\n",	starsystem->location3b	);
	fprintf( fp, "Location3c      %s~\n",	starsystem->location3c	);
	fprintf( fp, "Doc1a          %d\n",	starsystem->doc1a	);
	fprintf( fp, "Doc2a          %d\n",      starsystem->doc2a       );
	fprintf( fp, "Doc3a          %d\n",      starsystem->doc3a       );        	
	fprintf( fp, "Doc1b          %d\n", 	 starsystem->doc1b	);
	fprintf( fp, "Doc2b          %d\n",      starsystem->doc2b       );
	fprintf( fp, "Doc3b          %d\n",      starsystem->doc3b       );        	
	fprintf( fp, "Doc1c          %d\n",	 starsystem->doc1c	);
	fprintf( fp, "Doc2c          %d\n",      starsystem->doc2c       );
	fprintf( fp, "Doc3c          %d\n",      starsystem->doc3c       );        	
	fprintf( fp, "P1x          %d\n",       starsystem->p1x         );
	fprintf( fp, "P1y          %d\n",       starsystem->p1y         );
	fprintf( fp, "P1z          %d\n",       starsystem->p1z         );
	fprintf( fp, "P2x          %d\n",       starsystem->p2x         );
	fprintf( fp, "P2y          %d\n",       starsystem->p2y         );
	fprintf( fp, "P2z          %d\n",       starsystem->p2z         );
	fprintf( fp, "P3x          %d\n",       starsystem->p3x         );
	fprintf( fp, "P3y          %d\n",       starsystem->p3y         );
	fprintf( fp, "P3z          %d\n",       starsystem->p3z         );
	fprintf( fp, "S1x          %d\n",       starsystem->s1x         );
	fprintf( fp, "S1y          %d\n",       starsystem->s1y         );
	fprintf( fp, "S1z          %d\n",       starsystem->s1z         );
	fprintf( fp, "S2x          %d\n",       starsystem->s2x         );
	fprintf( fp, "S2y          %d\n",       starsystem->s2y         );
	fprintf( fp, "S2z          %d\n",       starsystem->s2z         );
	fprintf( fp, "Gravitys1     %d\n",       starsystem->gravitys1    );
	fprintf( fp, "Gravitys2     %d\n",       starsystem->gravitys2    );
	fprintf( fp, "Gravityp1     %d\n",       starsystem->gravityp1    );
	fprintf( fp, "Gravityp2     %d\n",       starsystem->gravityp2    );
	fprintf( fp, "Gravityp3     %d\n",       starsystem->gravityp3    );
	fprintf( fp, "Xpos          %d\n",       starsystem->xpos    );
	fprintf( fp, "Ypos          %d\n",       starsystem->ypos    );	
	fprintf( fp, "End\n\n"						);
	fprintf( fp, "#END\n"						);
    }
    fclose( fp );
    fpReserve = fopen( NULL_FILE, "r" );
    return;
}


/*
 * Read in actual starsystem data.
 */

#if defined(KEY)
#undef KEY
#endif

#define KEY( literal, field, value ) {					\
				if ( !str_cmp( word, literal ) )	\
				{					\
				    field  = value;			\
				    fMatch = TRUE;			\
				    break;				\
				} }

void fread_starsystem( SPACE_DATA *starsystem, FILE *fp )
{
    char buf[MAX_STRING_LENGTH];
    char *word;
    bool fMatch;

 
    for ( ; ; )
    {
	word   = feof( fp ) ? "End" : fread_word( fp );
	fMatch = FALSE;

	switch ( UPPER(word[0]) )
	{
	case '*':
	    fMatch = TRUE;
	    fread_to_eol( fp );
	    break;

        case 'D':
             KEY( "Doc1a",  starsystem->doc1a,          fread_number( fp ) );
             KEY( "Doc2a",  starsystem->doc2a,          fread_number( fp ) );
             KEY( "Doc3a",  starsystem->doc3a,          fread_number( fp ) ); 
             KEY( "Doc1b",  starsystem->doc1b,          fread_number( fp ) );
             KEY( "Doc2b",  starsystem->doc2b,          fread_number( fp ) );
             KEY( "Doc3b",  starsystem->doc3b,          fread_number( fp ) ); 
             KEY( "Doc1c",  starsystem->doc1c,          fread_number( fp ) );
             KEY( "Doc2c",  starsystem->doc2c,          fread_number( fp ) );
             KEY( "Doc3c",  starsystem->doc3c,          fread_number( fp ) ); 
             break;
                                

	case 'E':
	    if ( !str_cmp( word, "End" ) )
	    {
		if (!starsystem->name)
		  starsystem->name		= STRALLOC( "" );
		if (!starsystem->location1a)
		  starsystem->location1a            = STRALLOC( "" );  
		if (!starsystem->location2a)
		  starsystem->location2a            = STRALLOC( "" );
		if (!starsystem->location3a)
		  starsystem->location3a            = STRALLOC( "" );
		if (!starsystem->location1b)
		  starsystem->location1b            = STRALLOC( "" );  
		if (!starsystem->location2b)
		  starsystem->location2b            = STRALLOC( "" );
		if (!starsystem->location3b)
		  starsystem->location3b            = STRALLOC( "" );
		if (!starsystem->location1c)
		  starsystem->location1c            = STRALLOC( "" );  
		if (!starsystem->location2c)
		  starsystem->location2c            = STRALLOC( "" );
		if (!starsystem->location3c)
		  starsystem->location3c            = STRALLOC( "" );
		if (!starsystem->planet1)
		  starsystem->planet1            = STRALLOC( "" );  
		if (!starsystem->planet2)
		  starsystem->planet2            = STRALLOC( "" );
		if (!starsystem->planet3)
		  starsystem->planet3            = STRALLOC( "" );
		if (!starsystem->star1)
		  starsystem->star1            = STRALLOC( "" );  
		if (!starsystem->star2)
		  starsystem->star2            = STRALLOC( "" );
		return;
	    }
	    break;
	    
	case 'F':
	    KEY( "Filename",	starsystem->filename,		fread_string_nohash( fp ) );
	    break;
        
        case 'G':
            KEY( "Gravitys1",  starsystem->gravitys1,     fread_number( fp ) ); 
            KEY( "Gravitys2",  starsystem->gravitys2,     fread_number( fp ) ); 
            KEY( "Gravityp1",  starsystem->gravityp1,     fread_number( fp ) ); 
            KEY( "Gravityp2",  starsystem->gravityp2,     fread_number( fp ) ); 
            KEY( "Gravityp3",  starsystem->gravityp3,     fread_number( fp ) ); 
            break;  
        
        case 'L':
	    KEY( "Location1a",	starsystem->location1a,	fread_string( fp ) );
	    KEY( "Location2a",	starsystem->location2a,	fread_string( fp ) );
	    KEY( "Location3a",	starsystem->location3a,	fread_string( fp ) );
	    KEY( "Location1b",	starsystem->location1b,	fread_string( fp ) );
	    KEY( "Location2b",	starsystem->location2b,	fread_string( fp ) );
	    KEY( "Location3b",	starsystem->location3b,	fread_string( fp ) );
	    KEY( "Location1c",	starsystem->location1c,	fread_string( fp ) );
	    KEY( "Location2c",	starsystem->location2c,	fread_string( fp ) );
	    KEY( "Location3c",	starsystem->location3c,	fread_string( fp ) );
	    break;
  
	case 'N':
	    KEY( "Name",	starsystem->name,		fread_string( fp ) );
	    break;
        
        case 'P':
             KEY( "Planet1",	starsystem->planet1,	fread_string( fp ) );
	     KEY( "Planet2",	starsystem->planet2,	fread_string( fp ) );
	     KEY( "Planet3",	starsystem->planet3,	fread_string( fp ) );
	     KEY( "P1x",  starsystem->p1x,          fread_number( fp ) ); 
             KEY( "P1y",  starsystem->p1y,          fread_number( fp ) ); 
             KEY( "P1z",  starsystem->p1z,          fread_number( fp ) ); 
             KEY( "P2x",  starsystem->p2x,          fread_number( fp ) ); 
             KEY( "P2y",  starsystem->p2y,          fread_number( fp ) );
             KEY( "P2z",  starsystem->p2z,          fread_number( fp ) );
             KEY( "P3x",  starsystem->p3x,          fread_number( fp ) );
             KEY( "P3y",  starsystem->p3y,          fread_number( fp ) );
             KEY( "P3z",  starsystem->p3z,          fread_number( fp ) );
             break;
       	
       	case 'S':
       	     KEY( "Star1",	starsystem->star1,	fread_string( fp ) );
	     KEY( "Star2",	starsystem->star2,	fread_string( fp ) );
	     KEY( "S1x",  starsystem->s1x,          fread_number( fp ) ); 
             KEY( "S1y",  starsystem->s1y,          fread_number( fp ) ); 
             KEY( "S1z",  starsystem->s1z,          fread_number( fp ) ); 
             KEY( "S2x",  starsystem->s2x,          fread_number( fp ) ); 
             KEY( "S2y",  starsystem->s2y,          fread_number( fp ) );
             KEY( "S2z",  starsystem->s2z,          fread_number( fp ) );
            
        case 'X':
            KEY( "Xpos",  starsystem->xpos,     fread_number( fp ) ); 
        
        case 'Y':
            KEY( "Ypos",  starsystem->ypos,     fread_number( fp ) ); 
                 
       	}
	
	if ( !fMatch )
	{
	    sprintf( buf, "Fread_starsystem: no match: %s", word );
	    bug( buf, 0 );
	}
    }
}

/*
 * Load a starsystem file
 */

bool load_starsystem( char *starsystemfile )
{
    char filename[256];
    SPACE_DATA *starsystem;
    FILE *fp;
    bool found;

    CREATE( starsystem, SPACE_DATA, 1 );

    found = FALSE;
    sprintf( filename, "%s%s", SPACE_DIR, starsystemfile );

    if ( ( fp = fopen( filename, "r" ) ) != NULL )
    {

	found = TRUE;
        LINK( starsystem, first_starsystem, last_starsystem, next, prev );
	for ( ; ; )
	{
	    char letter;
	    char *word;

	    letter = fread_letter( fp );
	    if ( letter == '*' )
	    {
		fread_to_eol( fp );
		continue;
	    }

	    if ( letter != '#' )
	    {
		bug( "Load_starsystem_file: # not found.", 0 );
		break;
	    }

	    word = fread_word( fp );
	    if ( !str_cmp( word, "SPACE"	) )
	    {
	    	fread_starsystem( starsystem, fp );
	    	break;
	    }
	    else
	    if ( !str_cmp( word, "END"	) )
	        break;
	    else
	    {
		char buf[MAX_STRING_LENGTH];

		sprintf( buf, "Load_starsystem_file: bad section: %s.", word );
		bug( buf, 0 );
		break;
	    }
	}
	fclose( fp );
    }

    if ( !(found) )
      DISPOSE( starsystem );

    return found;
}

/*
 * Load in all the starsystem files.
 */
void load_space( )
{
    FILE *fpList;
    char *filename;
    char starsystemlist[256];
    char buf[MAX_STRING_LENGTH];
    
    
    first_starsystem	= NULL;
    last_starsystem	= NULL;

    log_string( "Loading space..." );

    sprintf( starsystemlist, "%s%s", SPACE_DIR, SPACE_LIST );
    fclose( fpReserve );
    if ( ( fpList = fopen( starsystemlist, "r" ) ) == NULL )
    {
	perror( starsystemlist );
	exit( 1 );
    }

    for ( ; ; )
    {
	filename = feof( fpList ) ? "$" : fread_word( fpList );
	if ( filename[0] == '$' )
	  break;
	  
       
	if ( !load_starsystem( filename ) )
	{
	  sprintf( buf, "Cannot load starsystem file: %s", filename );
	  bug( buf, 0 );
	}
    }
    fclose( fpList );
    log_string(" Done starsystems " );
    fpReserve = fopen( NULL_FILE, "r" );
    return;
}

void do_setstarsystem( CHAR_DATA *ch, char *argument )
{
    char arg1[MAX_INPUT_LENGTH];
    char arg2[MAX_INPUT_LENGTH];
    SPACE_DATA *starsystem;

    if ( IS_NPC( ch ) )
    {
	send_to_char( "Huh?\n\r", ch );
	return;
    }

    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );

    if ( arg2[0] == '\0' || arg1[0] == '\0' )
    {
	send_to_char( "Usage: setstarsystem <starsystem> <field> <values>\n\r", ch );
	send_to_char( "\n\rField being one of:\n\r", ch );
	send_to_char( "name filename xpos ypos,\n\r", ch );
	send_to_char( "star1 s1x s1y s1z gravitys1\n\r", ch );
	send_to_char( "star2 s2x s2y s2z gravitys2\n\r", ch );
	send_to_char( "planet1 p1x p1y p1z gravityp1\n\r", ch );
	send_to_char( "planet2 p2x p2y p2z gravityp2\n\r", ch );
	send_to_char( "planet3 p3x p3y p3z gravityp3\n\r", ch );
	send_to_char( "location1a location1b location1c doc1a doc1b doc1c\n\r", ch );
	send_to_char( "location2a location2b location2c doc2a doc2b doc2c\n\r", ch );
	send_to_char( "location3a location3b location3c doc3a doc3b doc3c\n\r", ch );
	send_to_char( "", ch );
	return;
    }

    starsystem = starsystem_from_name( arg1 );
    if ( !starsystem )
    {
	send_to_char( "No such starsystem.\n\r", ch );
	return;
    }


    if ( !str_cmp( arg2, "doc1a" ) )
    {
	starsystem->doc1a = atoi( argument );
	send_to_char( "Done.\n\r", ch );
	save_starsystem( starsystem );
	return;
    }
    if ( !str_cmp( arg2, "doc1b" ) )
    {
	starsystem->doc1b = atoi( argument );
	send_to_char( "Done.\n\r", ch );
	save_starsystem( starsystem );
	return;
    }
    if ( !str_cmp( arg2, "doc1c" ) )
    {
	starsystem->doc1c = atoi( argument );
	send_to_char( "Done.\n\r", ch );
	save_starsystem( starsystem );
	return;
    }

    if ( !str_cmp( arg2, "doc2a" ) )
    {
	starsystem->doc2a = atoi( argument );
	send_to_char( "Done.\n\r", ch );
	save_starsystem( starsystem );
	return;
    }
    if ( !str_cmp( arg2, "doc2b" ) )
    {
	starsystem->doc2b = atoi( argument );
	send_to_char( "Done.\n\r", ch );
	save_starsystem( starsystem );
	return;
    }
    if ( !str_cmp( arg2, "doc2c" ) )
    {
	starsystem->doc2c = atoi( argument );
	send_to_char( "Done.\n\r", ch );
	save_starsystem( starsystem );
	return;
    }

    if ( !str_cmp( arg2, "doc3a" ) )
    {
	starsystem->doc3a = atoi( argument );
	send_to_char( "Done.\n\r", ch );
	save_starsystem( starsystem );
	return;
    }
    if ( !str_cmp( arg2, "doc3b" ) )
    {
	starsystem->doc3b = atoi( argument );
	send_to_char( "Done.\n\r", ch );
	save_starsystem( starsystem );
	return;
    }
    if ( !str_cmp( arg2, "doc3c" ) )
    {
	starsystem->doc3c = atoi( argument );
	send_to_char( "Done.\n\r", ch );
	save_starsystem( starsystem );
	return;
    }
    
    if ( !str_cmp( arg2, "s1x" ) )
    {
	starsystem->s1x = atoi( argument );
	send_to_char( "Done.\n\r", ch );
	save_starsystem( starsystem );
	return;
    }
    if ( !str_cmp( arg2, "s1y" ) )
    {
	starsystem->s1y = atoi( argument );
	send_to_char( "Done.\n\r", ch );
	save_starsystem( starsystem );
	return;
    }
    if ( !str_cmp( arg2, "s1z" ) )
    {
	starsystem->s1z = atoi( argument );
	send_to_char( "Done.\n\r", ch );
	save_starsystem( starsystem );
	return;
    }

    if ( !str_cmp( arg2, "s2x" ) )
    {
	starsystem->s2x = atoi( argument );
	send_to_char( "Done.\n\r", ch );
	save_starsystem( starsystem );
	return;
    }
    if ( !str_cmp( arg2, "s2y" ) )
    {
	starsystem->s2y = atoi( argument );
	send_to_char( "Done.\n\r", ch );
	save_starsystem( starsystem );
	return;
    }
    if ( !str_cmp( arg2, "s2z" ) )
    {
	starsystem->s2z = atoi( argument );
	send_to_char( "Done.\n\r", ch );
	save_starsystem( starsystem );
	return;
    }

    if ( !str_cmp( arg2, "p1x" ) )
    {
	starsystem->p1x = atoi( argument );
	send_to_char( "Done.\n\r", ch );
	save_starsystem( starsystem );
	return;
    }
    if ( !str_cmp( arg2, "p1y" ) )
    {
	starsystem->p1y = atoi( argument );
	send_to_char( "Done.\n\r", ch );
	save_starsystem( starsystem );
	return;
    }
    if ( !str_cmp( arg2, "p1z" ) )
    {
	starsystem->p1z = atoi( argument );
	send_to_char( "Done.\n\r", ch );
	save_starsystem( starsystem );
	return;
    }

    if ( !str_cmp( arg2, "p2x" ) )
    {
	starsystem->p2x = atoi( argument );
	send_to_char( "Done.\n\r", ch );
	save_starsystem( starsystem );
	return;
    }
    if ( !str_cmp( arg2, "p2y" ) )
    {
	starsystem->p2y = atoi( argument );
	send_to_char( "Done.\n\r", ch );
	save_starsystem( starsystem );
	return;
    }
    if ( !str_cmp( arg2, "p2z" ) )
    {
	starsystem->p2z = atoi( argument );
	send_to_char( "Done.\n\r", ch );
	save_starsystem( starsystem );
	return;
    }

    if ( !str_cmp( arg2, "p3x" ) )
    {
	starsystem->p3x = atoi( argument );
	send_to_char( "Done.\n\r", ch );
	save_starsystem( starsystem );
	return;
    }
    if ( !str_cmp( arg2, "p3y" ) )
    {
	starsystem->p3y = atoi( argument );
	send_to_char( "Done.\n\r", ch );
	save_starsystem( starsystem );
	return;
    }
    if ( !str_cmp( arg2, "p3z" ) )
    {
	starsystem->p3z = atoi( argument );
	send_to_char( "Done.\n\r", ch );
	save_starsystem( starsystem );
	return;
    }

    if ( !str_cmp( arg2, "xpos" ) )
    {
	starsystem->xpos = atoi( argument );
	send_to_char( "Done.\n\r", ch );
	save_starsystem( starsystem );
	return;
    }

    if ( !str_cmp( arg2, "ypos" ) )
    {
	starsystem->ypos = atoi( argument );
	send_to_char( "Done.\n\r", ch );
	save_starsystem( starsystem );
	return;
    }
    
    if ( !str_cmp( arg2, "gravitys1" ) )
    {
	starsystem->gravitys1 = atoi( argument );
	send_to_char( "Done.\n\r", ch );
	save_starsystem( starsystem );
	return;
    }
    
    if ( !str_cmp( arg2, "gravitys2" ) )
    {
	starsystem->gravitys2 = atoi( argument );
	send_to_char( "Done.\n\r", ch );
	save_starsystem( starsystem );
	return;
    }
    
    if ( !str_cmp( arg2, "gravityp1" ) )
    {
	starsystem->gravityp1 = atoi( argument );
	send_to_char( "Done.\n\r", ch );
	save_starsystem( starsystem );
	return;
    }
    
    if ( !str_cmp( arg2, "gravityp2" ) )
    {
	starsystem->gravityp2 = atoi( argument );
	send_to_char( "Done.\n\r", ch );
	save_starsystem( starsystem );
	return;
    }
    
    if ( !str_cmp( arg2, "gravityp3" ) )
    {
	starsystem->gravityp3 = atoi( argument );
	send_to_char( "Done.\n\r", ch );
	save_starsystem( starsystem );
	return;
    }

    if ( !str_cmp( arg2, "name" ) )
    {
	STRFREE( starsystem->name );
	starsystem->name = STRALLOC( argument );
	send_to_char( "Done.\n\r", ch );
	save_starsystem( starsystem );
	return;
    }
    
    if ( !str_cmp( arg2, "star1" ) )
    {
	STRFREE( starsystem->star1 );
	starsystem->star1 = STRALLOC( argument );
	send_to_char( "Done.\n\r", ch );
	save_starsystem( starsystem );
	return;
    }
    
    if ( !str_cmp( arg2, "star2" ) )
    {
	STRFREE( starsystem->star2 );
	starsystem->star2 = STRALLOC( argument );
	send_to_char( "Done.\n\r", ch );
	save_starsystem( starsystem );
	return;
    }
    
    if ( !str_cmp( arg2, "planet1" ) )
    {
	STRFREE( starsystem->planet1 );
	starsystem->planet1 = STRALLOC( argument );
	send_to_char( "Done.\n\r", ch );
	save_starsystem( starsystem );
	return;
    }
    
    if ( !str_cmp( arg2, "planet2" ) )
    {
	STRFREE( starsystem->planet2 );
	starsystem->planet2 = STRALLOC( argument );
	send_to_char( "Done.\n\r", ch );
	save_starsystem( starsystem );
	return;
    }
    
    if ( !str_cmp( arg2, "planet3" ) )
    {
	STRFREE( starsystem->planet3 );
	starsystem->planet3 = STRALLOC( argument );
	send_to_char( "Done.\n\r", ch );
	save_starsystem( starsystem );
	return;
    }
    
    if ( !str_cmp( arg2, "location1a" ) )
    {
	STRFREE( starsystem->location1a );
	starsystem->location1a = STRALLOC( argument );
	send_to_char( "Done.\n\r", ch );
	save_starsystem( starsystem );
	return;
    }
    if ( !str_cmp( arg2, "location1b" ) )
    {
	STRFREE( starsystem->location1b );
	starsystem->location1b = STRALLOC( argument );
	send_to_char( "Done.\n\r", ch );
	save_starsystem( starsystem );
	return;
    }
    if ( !str_cmp( arg2, "location1c" ) )
    {
	STRFREE( starsystem->location1c );
	starsystem->location1c = STRALLOC( argument );
	send_to_char( "Done.\n\r", ch );
	save_starsystem( starsystem );
	return;
    }
    
    if ( !str_cmp( arg2, "location2a" ) )
    {
	STRFREE( starsystem->location2a  );
	starsystem->location2a = STRALLOC( argument );
	send_to_char( "Done.\n\r", ch );
	save_starsystem( starsystem );
	return;
    }
    if ( !str_cmp( arg2, "location2b" ) )
    {
	STRFREE( starsystem->location2a );
	starsystem->location2b = STRALLOC( argument );
	send_to_char( "Done.\n\r", ch );
	save_starsystem( starsystem );
	return;
    }
    if ( !str_cmp( arg2, "location2c" ) )
    {
	STRFREE( starsystem->location2c );
	starsystem->location2c = STRALLOC( argument );
	send_to_char( "Done.\n\r", ch );
	save_starsystem( starsystem );
	return;
    }
    
    if ( !str_cmp( arg2, "location3a" ) )
    {
	STRFREE( starsystem->location3a );
	starsystem->location3a = STRALLOC( argument );
	send_to_char( "Done.\n\r", ch );
	save_starsystem( starsystem );
	return;
    }
    if ( !str_cmp( arg2, "location3b" ) )
    {
	STRFREE( starsystem->location3b );
	starsystem->location3b = STRALLOC( argument );
	send_to_char( "Done.\n\r", ch );
	save_starsystem( starsystem );
	return;
    }
    if ( !str_cmp( arg2, "location3c" ) )
    {
	STRFREE( starsystem->location3c );
	starsystem->location3c = STRALLOC( argument );
	send_to_char( "Done.\n\r", ch );
	save_starsystem( starsystem );
	return;
    }


    do_setstarsystem( ch, "" );
    return;
}

void showstarsystem( CHAR_DATA *ch , SPACE_DATA *starsystem )
{   
    ch_printf( ch, "Starsystem:%s     Filename: %s    Xpos: %d   Ypos: %d\n\r",
    			starsystem->name,
    			starsystem->filename,
    			starsystem->xpos, starsystem->ypos);
    ch_printf( ch, "Star1: %s   Gravity: %d   Coordinates: %d %d %d\n\r",
    			starsystem->star1, starsystem->gravitys1,
    			starsystem->s1x , starsystem->s1y, starsystem->s1z);
    ch_printf( ch, "Star2: %s   Gravity: %d   Coordinates: %d %d %d\n\r",
    			starsystem->star2, starsystem->gravitys2,
    			starsystem->s2x , starsystem->s2y, starsystem->s2z);
    ch_printf( ch, "Planet1: %s   Gravity: %d   Coordinates: %d %d %d\n\r",
    			starsystem->planet1, starsystem->gravityp1, 
    			starsystem->p1x , starsystem->p1y, starsystem->p1z);
    ch_printf( ch, "     Doc1a: %5d (%s)\n\r",
    			starsystem->doc1a, starsystem->location1a);
    ch_printf( ch, "     Doc1b: %5d (%s)\n\r",
    			starsystem->doc1b, starsystem->location1b);
    ch_printf( ch, "     Doc1c: %5d (%s)\n\r",
    			starsystem->doc1c, starsystem->location1c);
    ch_printf( ch, "Planet2: %s   Gravity: %d   Coordinates: %d %d %d\n\r",
    			starsystem->planet2, starsystem->gravityp2, 
    			starsystem->p2x , starsystem->p2y, starsystem->p2z);
    ch_printf( ch, "     Doc2a: %5d (%s)\n\r",
    			starsystem->doc2a, starsystem->location2a);
    ch_printf( ch, "     Doc2b: %5d (%s)\n\r",
    			starsystem->doc2b, starsystem->location2b);
    ch_printf( ch, "     Doc2c: %5d (%s)\n\r",
    			starsystem->doc2c, starsystem->location2c);
    ch_printf( ch, "Planet3: %s   Gravity: %d   Coordinates: %d %d %d\n\r",
    			starsystem->planet3, starsystem->gravityp3, 
    			starsystem->p3x , starsystem->p3y, starsystem->p3z);
    ch_printf( ch, "     Doc3a: %5d (%s)\n\r",
    			starsystem->doc3a, starsystem->location3a);
    ch_printf( ch, "     Doc3b: %5d (%s)\n\r",
    			starsystem->doc3b, starsystem->location3b);
    ch_printf( ch, "     Doc3c: %5d (%s)\n\r",
    			starsystem->doc3c, starsystem->location3c);
    return;
}

void do_showstarsystem( CHAR_DATA *ch, char *argument )
{
   SPACE_DATA *starsystem;

   starsystem = starsystem_from_name( argument );
   
   if ( starsystem == NULL )
      send_to_char("&RNo such starsystem.\n\r",ch);
   else
      showstarsystem(ch , starsystem);
   
}

void do_makestarsystem( CHAR_DATA *ch, char *argument )
{   
    char arg[MAX_INPUT_LENGTH];
    char filename[256];
    SPACE_DATA *starsystem;

    if ( !argument || argument[0] == '\0' )
    {
	send_to_char( "Usage: makestarsystem <starsystem name>\n\r", ch );
	return;
    }


    CREATE( starsystem, SPACE_DATA, 1 );
    LINK( starsystem, first_starsystem, last_starsystem, next, prev );

    starsystem->name		= STRALLOC( argument );
    
		  starsystem->location1a            = STRALLOC( "" );  
		  starsystem->location2a            = STRALLOC( "" );
		  starsystem->location3a            = STRALLOC( "" );
		  starsystem->location1b            = STRALLOC( "" );  
		  starsystem->location2b            = STRALLOC( "" );
		  starsystem->location3b            = STRALLOC( "" );
		  starsystem->location1c            = STRALLOC( "" );  
		  starsystem->location2c            = STRALLOC( "" );
		  starsystem->location3c            = STRALLOC( "" );
		  starsystem->planet1            = STRALLOC( "" );  
		  starsystem->planet2            = STRALLOC( "" );
		  starsystem->planet3            = STRALLOC( "" );
		  starsystem->star1            = STRALLOC( "" );  
		  starsystem->star2            = STRALLOC( "" );
    
    argument = one_argument( argument, arg );
    sprintf( filename, "%s.system" , strlower(arg) );
    starsystem->filename = str_dup( filename );
    save_starsystem( starsystem );
    write_starsystem_list();
}

void do_starsystems( CHAR_DATA *ch, char *argument )
{
    SPACE_DATA *starsystem;
    int count = 0;

    for ( starsystem = first_starsystem; starsystem; starsystem = starsystem->next )
    {
        set_char_color( AT_NOTE, ch );
        ch_printf( ch, "%s\n\r", starsystem->name );
        count++;
    }

    if ( !count )
    {
        send_to_char( "There are no starsystems currently formed.\n\r", ch );
	return;
    }
}
   
void echo_to_ship( int color , SHIP_DATA *ship , char *argument )
{
     int room;
      
     for ( room = ship->firstroom ; room <= ship->lastroom ;room++ )
     {
         echo_to_room( color , get_room_index(room) , argument );
     }  
     
}

void sound_to_ship( SHIP_DATA *ship , char *argument )
{
     int roomnum;
     ROOM_INDEX_DATA *room;
     CHAR_DATA *vic;
      
     for ( roomnum = ship->firstroom ; roomnum <= ship->lastroom ;roomnum++ )
     {
        room = get_room_index( roomnum );
        if ( room == NULL ) continue;
        
        for ( vic = room->first_person; vic; vic = vic->next_in_room )
        {
	   if ( !IS_NPC(vic) && IS_SET( vic->act, PLR_SOUND ) )
	     send_to_char( argument, vic );
        }
     }  
     
}

void echo_to_cockpit( int color , SHIP_DATA *ship , char *argument )
{
     int room;
      
     for ( room = ship->firstroom ; room <= ship->lastroom ;room++ )
     {
         if ( room == ship->cockpit || room == ship->navseat
         || room == ship->pilotseat || room == ship->coseat
         || room == ship->gunseat || room == ship->engineroom
         || room == ship->turret1 || room == ship->turret2 )
               echo_to_room( color , get_room_index(room) , argument );
     }  
     
}

void echo_to_system( int color , SHIP_DATA *ship , char *argument , SHIP_DATA *ignore )
{
     SHIP_DATA *target;
     
     if (!ship->starsystem)
        return;
      
     for ( target = ship->starsystem->first_ship; target; target = target->next_in_starsystem )
     {
       if (target != ship && target != ignore )  
         echo_to_cockpit( color , target , argument );
     }  
     
}

bool is_facing( SHIP_DATA *ship , SHIP_DATA *target )
{
    	float dy, dx, dz, hx, hy, hz;
        float cosofa;
     	     	
     	hx = ship->hx;
     	hy = ship->hy;
     	hz = ship->hz;
     	     				
     	     					dx = target->vx - ship->vx;
     	     						dy = target->vy - ship->vy;
     	     							dz = target->vz - ship->vz;
     	     							
     	    cosofa = ( hx*dx + hy*dy + hz*dz ) 
     	     		/ ( sqrt(hx*hx+hy*hy+hz*hz) + sqrt(dx*dx+dy*dy+dz*dz) );
     	     								               
     	           	if ( cosofa > 0.75 )
     	                  	             return TRUE;
     	     								               	             
     	     								 return FALSE;
     	}
     	     								               	             	

long int get_ship_value( SHIP_DATA *ship )
{
     long int price;
          
     if (ship->class == FIGHTER_SHIP)
        price = 5000;
     else if (ship->class == MIDSIZE_SHIP)
     	price = 50000; 
     else if (ship->class == FRIGATE_SHIP)
      price = 100000; 
     else if (ship->class == CAPITAL_SHIP) 
        price = 500000;
     else if (ship->class == MOBILE_SUIT)
      price = 500000; 
     else if (ship->class == SUPERCAPITAL_SHIP)
      price = 1000000; 
     else 
        price = 2000;
        
     if ( ship->class <= CAPITAL_SHIP ) 
       price += ( ship->manuever*250*(1+ship->class) );
     
     price += ( ship->tractorbeam * 500 );
     price += ( ship->realspeed * 100 );
     price += ( ship->astro_array * 50 );
     price += ( 25 * ship->maxhull );
     price += ( ship->armor * 5 );
     price += ( ship->maxarmor * 70 );
     price += ( 50 * ship->maxenergy );
     price += ( 500 * ship->maxchaff );
               
     if (ship->maxenergy > 5000 )
          price += ( (ship->maxenergy-5000)*20 ) ;
     
     if (ship->maxenergy > 10000 )
          price += ( (ship->maxenergy-10000)*50 );
     
     if (ship->maxhull > 1000)
        price += ( (ship->maxhull-1000)*10 );
     
     if (ship->maxhull > 10000)
        price += ( (ship->maxhull-10000)*20 );
        
     if (ship->maxshield > 200)
          price += ( (ship->maxshield-200)*50 );
     
     if (ship->maxshield > 1000)
          price += ( (ship->maxshield-1000)*100 );
     
     if (ship->realspeed > 100 )
        price += ( (ship->realspeed-100)*500 ) ;
        
     if (ship->lasers > 5 )
        price += ( (ship->lasers-5)*1000 );

     if (ship->ions)
        price += ( ( (ship->ions) * 2500 ) + 1000 );
      
     if (ship->maxshield)
     	price += ( 1000 + 50 * ship->maxshield);
     
     if (ship->lasers)
     	price += ( 500 + 750 * ship->lasers );
    
     if (ship->maxmissiles)
     	price += ( 1000 + 100 * ship->maxmissiles );
     if (ship->maxrockets)
     	price += ( 2000 + 200 * ship->maxmissiles );
     if (ship->maxtorpedos)
     	price += ( 1500 + 150 * ship->maxmissiles );
     
     if (ship->missiles )
     	price += ( 250 * ship->missiles );
     else if (ship->torpedos )
     	price += ( 500 * ship->torpedos );
     else if (ship->rockets )
        price += ( 1000 * ship->rockets );
         
     if (ship->turret1)
        price += 10000;
        
     if (ship->turret2)
        price += 10000;
     
     if (ship->overdrive == 1)
        price += (1000 * ship->realspeed );

     if (ship->interdict == 1)
        price += ( 2500000 );

     if (ship->hyperspeed)
        price += ( 1000 + ship->hyperspeed * 25 );
     
     if (ship->hanger)
        price += ( ship->class == MIDSIZE_SHIP ? 100000 : 500000 );
 
     price *= 1.5;
     
     return price;
     
}

void write_ship_list( )
{
    SHIP_DATA *tship;
    FILE *fpout;
    char filename[256];
    
    sprintf( filename, "%s%s", SHIP_DIR, SHIP_LIST );
    fpout = fopen( filename, "w" );
    if ( !fpout )
    {
         bug( "FATAL: cannot open ship.lst for writing!\n\r", 0 );
         return;
    }
    for ( tship = first_ship; tship; tship = tship->next )
    fprintf( fpout, "%s\n", tship->filename );
    fprintf( fpout, "$\n" );
    fclose( fpout );
}
                                                                    
SHIP_DATA * ship_in_room( ROOM_INDEX_DATA *room, char *name )
{
    SHIP_DATA *ship;

    if ( !room )
     return NULL;
     
    for ( ship = room->first_ship ; ship ; ship = ship->next_in_room )
     if ( !str_cmp( name, ship->name ) )
         return ship;
    
    for ( ship = room->first_ship ; ship ; ship = ship->next_in_room )
     if ( nifty_is_name_prefix( name, ship->name ) )
         return ship;
    
    return NULL;    
}

/*
 * Get pointer to ship structure from ship name.
 */
SHIP_DATA *get_ship( char *name )
{
    SHIP_DATA *ship;
    
    for ( ship = first_ship; ship; ship = ship->next )
       if ( !str_cmp( name, ship->name ) )
         return ship;
    
    for ( ship = first_ship; ship; ship = ship->next )
	if ( nifty_is_name_prefix( name, ship->name ) )
         return ship;
    
    return NULL;
}

/*
 * Checks if ships in a starsystem and returns poiner if it is.
 */
SHIP_DATA *get_ship_here( char *name , SPACE_DATA *starsystem)
{
    SHIP_DATA *ship;
    
    if ( starsystem == NULL )
         return NULL;
    
    for ( ship = starsystem->first_ship ; ship; ship = ship->next_in_starsystem )
       if ( !str_cmp( name, ship->name ) )
         return ship;
    
    for ( ship = starsystem->first_ship; ship; ship = ship->next_in_starsystem )
       if ( nifty_is_name_prefix( name, ship->name ) )
         return ship;
    
    return NULL;
}


/*
 * Get pointer to ship structure from ship name.
 */
SHIP_DATA *ship_from_pilot( char *name )
{
    SHIP_DATA *ship;
    
    for ( ship = first_ship; ship; ship = ship->next )
       if ( !str_cmp( name, ship->pilot ) )
         return ship;
       if ( !str_cmp( name, ship->copilot ) )
         return ship;
       if ( !str_cmp( name, ship->owner ) )
         return ship;  
    return NULL;
}


/*
 * Get pointer to ship structure from cockpit, turret, or entrance ramp vnum.
 */
 
SHIP_DATA *ship_from_cockpit( int vnum )
{
    SHIP_DATA *ship;
    
    for ( ship = first_ship; ship; ship = ship->next )
       if ( vnum == ship->cockpit || vnum == ship->turret1 || vnum == ship->turret2
       || vnum == ship->pilotseat || vnum == ship->coseat || vnum == ship->navseat
        || vnum == ship->gunseat  || vnum == ship->engineroom )
         return ship;
    return NULL;
}

SHIP_DATA *ship_from_pilotseat( int vnum )
{
    SHIP_DATA *ship;
    
    for ( ship = first_ship; ship; ship = ship->next )
       if ( vnum == ship->pilotseat )
         return ship;
    return NULL;
}

SHIP_DATA *ship_from_coseat( int vnum )
{
    SHIP_DATA *ship;
    
    for ( ship = first_ship; ship; ship = ship->next )
       if ( vnum == ship->coseat )
         return ship;
    return NULL;
}

SHIP_DATA *ship_from_navseat( int vnum )
{
    SHIP_DATA *ship;
    
    for ( ship = first_ship; ship; ship = ship->next )
       if ( vnum == ship->navseat )
         return ship;
    return NULL;
}

SHIP_DATA *ship_from_gunseat( int vnum )
{
    SHIP_DATA *ship;
    
    for ( ship = first_ship; ship; ship = ship->next )
       if ( vnum == ship->gunseat )
         return ship;
    return NULL;
}

SHIP_DATA *ship_from_engine( int vnum )
{
    SHIP_DATA *ship;
    
    for ( ship = first_ship; ship; ship = ship->next )
    {   
      if (ship->engineroom)   
      {  
        if ( vnum == ship->engineroom )
          return ship;
      }
      else
      { 
        if ( vnum == ship->cockpit )
          return ship;
      }
    }
    
    return NULL;
}



SHIP_DATA *ship_from_turret( int vnum )
{
    SHIP_DATA *ship;
    
    for ( ship = first_ship; ship; ship = ship->next )
       if ( vnum == ship->gunseat || vnum == ship->turret1 || vnum == ship->turret2 )
         return ship;
    return NULL;
}

SHIP_DATA *ship_from_entrance( int vnum )
{
    SHIP_DATA *ship;
    
    for ( ship = first_ship; ship; ship = ship->next )
       if ( vnum == ship->entrance )
         return ship;
    return NULL;
}

SHIP_DATA *ship_from_hanger( int vnum )
{
    SHIP_DATA *ship;
    
    for ( ship = first_ship; ship; ship = ship->next )
       if ( vnum == ship->hanger )
         return ship;
    return NULL;
}

void do_quickreset( CHAR_DATA *ch )
{
SHIP_DATA *ship;


for ( ship = first_ship; ship; ship = ship->next )
{
 ship->description = "";
 ship->owner = "";
 ship->pilot = "";
 ship->copilot = "";
}

save_ship(ship);
}

void save_ship( SHIP_DATA *ship )
{
    FILE *fp;
    char filename[256];
    char buf[MAX_STRING_LENGTH];
    int module;

    if ( !ship )
    {
	bug( "save_ship: null ship pointer!", 0 );
	return;
    }
    if(ship->maxmodules < 15) /* Minimum Maxmodules raised */
       ship->maxmodules = 15; 
    if ( !ship->filename || ship->filename[0] == '\0' )
    {
	sprintf( buf, "save_ship: %s has no filename", ship->name );
	bug( buf, 0 );
	return;
    }
     
//added to convert bay to an int for saving;
    if (ship->bayopen == TRUE)
	ship->baystat = 0;
    else
	ship->baystat = 1;

    sprintf( filename, "%s%s", SHIP_DIR, ship->filename );
    
    fclose( fpReserve );
    if ( ( fp = fopen( filename, "w" ) ) == NULL )
    {
    	bug( "save_ship: fopen", 0 );
    	perror( filename );
    }
    else
    {
	fprintf( fp, "#SHIP\n" );
	fprintf( fp, "Name         %s~\n",	ship->name		);
	fprintf( fp, "Filename     %s~\n",	ship->filename		);
        fprintf( fp, "Description  %s~\n",	ship->description	);
	fprintf( fp, "Owner        %s~\n",	ship->owner		);
	fprintf( fp, "Pilot        %s~\n",      ship->pilot             );
	fprintf( fp, "Copilot      %s~\n",      ship->copilot           );
	fprintf( fp, "Class        %d\n",	ship->class		);
	fprintf( fp, "Tractorbeam  %d\n",	ship->tractorbeam	);
	fprintf( fp, "Shipyard     %d\n",	ship->shipyard		);
	fprintf( fp, "Hanger       %d\n",	ship->hanger    	);
	fprintf( fp, "Turret1      %d\n",	ship->turret1		);
	fprintf( fp, "Turret2      %d\n",	ship->turret2		);
	fprintf( fp, "Statet0      %d\n",	ship->statet0		);
	fprintf( fp, "Statet1      %d\n",	ship->statet1		);
	fprintf( fp, "Statet2      %d\n",	ship->statet2		);
	fprintf( fp, "IonState     %d\n",	ship->ionstate		);
	fprintf( fp, "Lasers       %d\n",	ship->lasers    	);
	fprintf( fp, "Ions         %d\n",	ship->ions    	);
	fprintf( fp, "Laserdamage  %d\n",	ship->laserdamage       );
        fprintf( fp, "Armor        %d\n",   ship->armor       );
        fprintf( fp, "Maxarmor     %d\n",   ship->maxarmor       );
        fprintf( fp, "Interdict    %d\n",   ship->interdict       );
        fprintf( fp, "Autocannon   %d\n",   ship->autocannon   );
        fprintf( fp, "Autodamage   %d\n",   ship->autodamage   );
        fprintf( fp, "Autoammo     %d\n",   ship->autoammomax   );
        fprintf( fp, "BayStat      %d\n",   ship->baystat   );
	fprintf( fp, "Missiles     %d\n",	ship->missiles		);
	fprintf( fp, "Maxmissiles  %d\n",	ship->maxmissiles	);
	fprintf( fp, "Rockets      %d\n",	ship->rockets		);
	fprintf( fp, "Maxrockets   %d\n",	ship->maxrockets	);
	fprintf( fp, "Torpedos     %d\n",	ship->torpedos		);
	fprintf( fp, "Maxtorpedos  %d\n",	ship->maxtorpedos	);
	fprintf( fp, "Lastdoc      %d\n",	ship->lastdoc		);
	fprintf( fp, "Firstroom    %d\n",	ship->firstroom		);
	fprintf( fp, "Lastroom     %d\n",	ship->lastroom		);
	fprintf( fp, "Shield       %d\n",	ship->shield		);
	fprintf( fp, "Maxshield    %d\n",	ship->maxshield		);
	fprintf( fp, "Hull         %d\n",	ship->hull		);
	fprintf( fp, "Maxhull      %d\n",	ship->maxhull		);
	fprintf( fp, "Maxenergy    %d\n",	ship->maxenergy		);
	fprintf( fp, "Hyperspeed   %d\n",	ship->hyperspeed	);
	fprintf( fp, "Comm         %d\n",	ship->comm		);
	fprintf( fp, "Chaff        %d\n",	ship->chaff		);
	fprintf( fp, "Maxchaff     %d\n",	ship->maxchaff		);
	fprintf( fp, "Sensor       %d\n",	ship->sensor		);
	fprintf( fp, "Astro_array  %d\n",	ship->astro_array	);
	fprintf( fp, "Realspeed    %d\n",	ship->realspeed		);
	fprintf( fp, "Type         %d\n",	ship->type		);
	fprintf( fp, "Cockpit      %d\n",	ship->cockpit		);
	fprintf( fp, "Coseat       %d\n",	ship->coseat		);
	fprintf( fp, "Pilotseat    %d\n",	ship->pilotseat		);
	fprintf( fp, "Gunseat      %d\n",	ship->gunseat		);
	fprintf( fp, "Navseat      %d\n",	ship->navseat		);
	fprintf( fp, "Engineroom   %d\n",       ship->engineroom        );
	fprintf( fp, "Entrance     %d\n",       ship->entrance          );
	fprintf( fp, "Shipstate    %d\n",	ship->shipstate		);
	fprintf( fp, "Missilestate %d\n",	ship->missilestate	);
	fprintf( fp, "Energy       %d\n",	ship->energy		);
	fprintf( fp, "Manuever     %d\n",       ship->manuever          );
	fprintf( fp, "Home         %s~\n",      ship->home              );
	fprintf( fp, "MaxModules %d\n", ship->maxmodules		);
	fprintf( fp, "MaxCargo     %d\n",       ship->maxcargo          );
        fprintf( fp, "Flags        %d\n",       ship->flags             );
        fprintf( fp, "Mlaunchers   %d\n",       ship->mlaunchers        );
        fprintf( fp, "Tlaunchers   %d\n",       ship->tlaunchers        );
        fprintf( fp, "Rlaunchers   %d\n",       ship->rlaunchers        );
        fprintf( fp, "Cloak        %d\n",       ship->cloak             );
        if (ship->cargo > 0)
	{
	   fprintf( fp, "Cargo     %d\n",       ship->cargo          );
	   fprintf( fp, "CargoType %d\n",       ship->cargotype          );
	}	
	if ( ship->modules > 0 )
	{
		for ( module = 1; module <= ship->modules; module++ )
		{
			fprintf( fp, "Module         %d\n", ship->module_vnum[module] );	
		}
/*                bug("Modules input into ship file.", 0); */
	}
	fprintf( fp, "End\n"						);
    }
    fclose( fp );
    fpReserve = fopen( NULL_FILE, "r" );
    return;
}


/*
 * Read in actual ship data.
 */

#if defined(KEY)
#undef KEY
#endif

#define KEY( literal, field, value )					\
				if ( !str_cmp( word, literal ) )	\
				{					\
				    field  = value;			\
				    fMatch = TRUE;			\
				    break;				\
				}

void fread_ship( SHIP_DATA *ship, FILE *fp )
{
    char buf[MAX_STRING_LENGTH];
    char *word;
    bool fMatch;
    int dummy_number;
    int modules = 1;
    ship->modules = 0;

    for ( ; ; )
    {
	word   = feof( fp ) ? "End" : fread_word( fp );
	fMatch = FALSE;
        
	switch ( UPPER(word[0]) )
	{
	case '*':
	    fMatch = TRUE;
	    fread_to_eol( fp );
	    break;
        
        case 'A':
             KEY( "Astro_array",      ship->astro_array,       fread_number( fp ) );
             KEY( "Autocannon",      ship->autocannon,       fread_number( fp ) );
             KEY( "Autoammo",      ship->autoammomax,       fread_number( fp ) );
             KEY( "Armor",      ship->armor,       fread_number( fp ) );
             KEY( "Autodamage",      ship->autodamage,       fread_number( fp ) );
             break;

	case 'B':
             KEY( "BayStat",     ship->baystat,          fread_number( fp ) );
		
        
        case 'C':
             KEY( "Cockpit",     ship->cockpit,          fread_number( fp ) );
             KEY( "Cargo",       ship->cargo,      fread_number( fp ) );
             KEY( "CargoType",   ship->cargotype,  fread_number( fp ) );
	     KEY( "Coseat",     ship->coseat,          fread_number( fp ) );
             KEY( "Class",       ship->class,            fread_number( fp ) );
             KEY( "Copilot",     ship->copilot,          fread_string( fp ) );
             KEY( "Comm",        ship->comm,      fread_number( fp ) );
             KEY( "Chaff",       ship->chaff,      fread_number( fp ) );
             KEY( "Cloak",       ship->cloak,      fread_number( fp ) );
             break;
                                

	case 'D':
	    KEY( "Description",	ship->description,	fread_string( fp ) );
	    break;

	case 'E':
	    KEY( "Engineroom",    ship->engineroom,      fread_number( fp ) );
	    KEY( "Entrance",	ship->entrance,	        fread_number( fp ) );
	    KEY( "Energy",      ship->energy,        fread_number( fp ) );
	    if ( !str_cmp( word, "End" ) )
	    {
		if (!ship->home)
		  ship->home		= STRALLOC( "" );
		if (!ship->name)
		  ship->name		= STRALLOC( "" );
		if (!ship->owner)
		  ship->owner		= STRALLOC( "" );
		if (!ship->description)
		  ship->description 	= STRALLOC( "" );
		if (!ship->copilot)
		  ship->copilot 	= STRALLOC( "" );
		if (!ship->pilot)
		  ship->pilot   	= STRALLOC( "" );  
                  ship->shipstate2 = SHIP_READY;
		if (ship->shipstate != SHIP_DISABLED)
		  ship->shipstate = SHIP_DOCKED;
		if (ship->statet0 != LASER_DAMAGED)
		  ship->statet0 = LASER_READY;
		if (ship->statet1 != LASER_DAMAGED)
		  ship->statet1 = LASER_READY;
		if (ship->statet2 != LASER_DAMAGED)
		  ship->statet2 = LASER_READY;
		if (ship->missilestate != MISSILE_DAMAGED)
		  ship->missilestate = MISSILE_READY;
                if (ship->torpedostate != MISSILE_DAMAGED)
                  ship->torpedostate = MISSILE_READY;
                if( ship->rocketstate != MISSILE_DAMAGED)
                  ship->rocketstate = MISSILE_READY;
                if( ship->maxrockets > 0 && !(ship->rlaunchers > 0))
                  ship->rlaunchers = 1;
                if( ship->maxmissiles > 0 && !(ship->mlaunchers > 0))
                  ship->mlaunchers = 1;
                if( ship->maxtorpedos > 0 && !(ship->tlaunchers > 0))
                  ship->tlaunchers = 1;
	        if ( !ship->ionstate )
		  ship->ionstate = LASER_READY;
		if (ship->ionstate != LASER_DAMAGED)
		  ship->ionstate = LASER_READY;
                if ( str_cmp(ship->owner, "Public") && ship->shipyard != ROOM_LIMBO_SHIPYARD)
		    ship->shipyard = ROOM_STARSHIP_GRAVEYARD;
		if (ship->lastdoc <= 0) 
		  ship->lastdoc = ship->shipyard;
		if (!ship->baystat || ship->baystat<1 )
			ship->bayopen = TRUE;
		else
			ship->bayopen = FALSE;
			
		if (!ship->autopilot)
		   ship->autopilot   = FALSE;
                /* Safety Purposes. */
	        if( ship->cloak != 1 )
                   ship->cloak = 0;
		ship->hatchopen = FALSE;
		if (ship->navseat <= 0) 
		  ship->navseat = ship->cockpit;
		if (ship->gunseat <= 0) 
		  ship->gunseat = ship->cockpit;
		if (ship->coseat <= 0) 
		  ship->coseat = ship->cockpit;
		if (ship->pilotseat <= 0) 
		  ship->pilotseat = ship->cockpit;

		ship->sdestnum = -1;
	    if (!ship->armor)
	      ship->armor = 0;
                if (ship->cargotype != CARGO_NONE && ship->cargo < 1)
                   ship->cargotype = CARGO_NONE;
	    if (!ship->maxarmor)
	      ship->maxarmor = 0;
            if (!ship->interdict)
              ship->interdict     = 0;  
            if (!ship->ions)
              ship->ions     = 0;  
		if (!ship->autocannon)
		  ship->autocannon = 0;
		if (!ship->autoammomax)
		  ship->autoammomax = 0;
		if (!ship->autodamage)
		  ship->autodamage = 0;
		
		ship->autoammo = ship->autoammomax;

		if (ship->missiletype == 1)
		{
		  ship->torpedos = ship->missiles;    /* for back compatability */
		  ship->missiles = 0;
		}
		ship->starsystem = NULL;
		ship->energy = ship->maxenergy;
		ship->hull = ship->maxhull;
		ship->in_room=NULL;
                ship->next_in_room=NULL;
                ship->prev_in_room=NULL;
		if (ship->maxmodules == 0) 
		ship->maxmodules = 15;
                if(ship->maxmodules < 15)
                ship->maxmodules = 15;/*max modules changed to 15 minimum. */
                /* Remove Temporary Ship Flags IF set, just about all, except simulators. */
                if(IS_SET(ship->flags, SHIPFLAG_CLOAKED))
                   REMOVE_BIT(ship->flags, SHIPFLAG_CLOAKED);
                if(IS_SET(ship->flags, SHIPFLAG_OVERDRIVENODE))
                   REMOVE_BIT(ship->flags, SHIPFLAG_OVERDRIVENODE);
                if(IS_SET(ship->flags, SHIPFLAG_SABOTAGEDLASERS))
                   REMOVE_BIT(ship->flags, SHIPFLAG_SABOTAGEDLASERS);
                if(IS_SET(ship->flags, SHIPFLAG_SABOTAGEDENGINE))
                   REMOVE_BIT(ship->flags, SHIPFLAG_SABOTAGEDENGINE);
                if(IS_SET(ship->flags, SHIPFLAG_SABOTAGEDTURRET1))
                   REMOVE_BIT(ship->flags, SHIPFLAG_SABOTAGEDTURRET1);
                if(IS_SET(ship->flags, SHIPFLAG_SABOTAGEDTURRET2))
                   REMOVE_BIT(ship->flags, SHIPFLAG_SABOTAGEDTURRET2);
                if(IS_SET(ship->flags, SHIPFLAG_SABOTAGEDIONS))
                   REMOVE_BIT(ship->flags, SHIPFLAG_SABOTAGEDIONS);
                if(IS_SET(ship->flags, SHIPFLAG_SABOTAGEDLAUNCHERS))
                   REMOVE_BIT(ship->flags, SHIPFLAG_SABOTAGEDLAUNCHERS);
                if(IS_SET(ship->flags, SHIPFLAG_AFTERBURNER))
                   REMOVE_BIT(ship->flags, SHIPFLAG_AFTERBURNER);
                if(IS_SET(ship->flags, SHIPFLAG_SHIELDRLASER))
                   REMOVE_BIT(ship->flags, SHIPFLAG_SHIELDRLASER);
                if(IS_SET(ship->flags, SHIPFLAG_SHIELDRENGINE))
                   REMOVE_BIT(ship->flags, SHIPFLAG_SHIELDRENGINE);
                if(IS_SET(ship->flags, SHIPFLAG_ENGINERSHIELD))
                   REMOVE_BIT(ship->flags, SHIPFLAG_ENGINERSHIELD);
                if(IS_SET(ship->flags, SHIPFLAG_ENGINERLASER))
                   REMOVE_BIT(ship->flags, SHIPFLAG_ENGINERLASER);
                if(IS_SET(ship->flags, SHIPFLAG_LASERRENGINE))
                   REMOVE_BIT(ship->flags, SHIPFLAG_LASERRENGINE);
                if(IS_SET(ship->flags, SHIPFLAG_LASERRSHIELD))
                   REMOVE_BIT(ship->flags, SHIPFLAG_LASERRSHIELD);
                if(IS_SET(ship->flags, SHIPFLAG_DUALLASER))
                   REMOVE_BIT(ship->flags, SHIPFLAG_DUALLASER);
                if(IS_SET(ship->flags, SHIPFLAG_TRILASER))
                   REMOVE_BIT(ship->flags, SHIPFLAG_TRILASER);
                if(IS_SET(ship->flags, SHIPFLAG_QUADLASER))
                   REMOVE_BIT(ship->flags, SHIPFLAG_QUADLASER);
                if(IS_SET(ship->flags, SHIPFLAG_DUALION))
                   REMOVE_BIT(ship->flags, SHIPFLAG_DUALION);
                if(IS_SET(ship->flags, SHIPFLAG_TRIION))
                   REMOVE_BIT(ship->flags, SHIPFLAG_TRIION);
                if(IS_SET(ship->flags, SHIPFLAG_QUADION))
                   REMOVE_BIT(ship->flags, SHIPFLAG_QUADION);
                if(IS_SET(ship->flags, SHIPFLAG_DUALMISSILE))
                   REMOVE_BIT(ship->flags, SHIPFLAG_DUALMISSILE);
                if(IS_SET(ship->flags, SHIPFLAG_DUALTORPEDO))
                   REMOVE_BIT(ship->flags, SHIPFLAG_DUALTORPEDO);
                if(IS_SET(ship->flags, SHIPFLAG_DUALROCKET))
                   REMOVE_BIT(ship->flags, SHIPFLAG_DUALROCKET);
                if(IS_SET(ship->flags, SHIPFLAG_SABOTAGEDTLAUNCHERS))
                   REMOVE_BIT(ship->flags, SHIPFLAG_SABOTAGEDTLAUNCHERS);
                if(IS_SET(ship->flags, SHIPFLAG_SABOTAGEDRLAUNCHERS))
                   REMOVE_BIT(ship->flags, SHIPFLAG_SABOTAGEDRLAUNCHERS);
        	return;
	    }
	    break;
	    
	case 'F':
	    KEY( "Filename",	ship->filename,		fread_string_nohash( fp ) );
            KEY( "Firstroom",   ship->firstroom,        fread_number( fp ) );
            KEY( "Flags",       ship->flags,            fread_number( fp ) );
            break;
        
        case 'G':    
            KEY( "Gunseat",     ship->gunseat,          fread_number( fp ) );
            break;
        
        case 'H':
            KEY( "Home" , ship->home, fread_string( fp ) );
            KEY( "Hyperspeed",   ship->hyperspeed,      fread_number( fp ) );
            KEY( "Hull",      ship->hull,        fread_number( fp ) );
            KEY( "Hanger",  ship->hanger,      fread_number( fp ) );
            break;

        case 'I':
            KEY( "Ions" , ship->ions, fread_number( fp ) );
            KEY( "IonState" , ship->ionstate, fread_number( fp ) );
            KEY( "Interdict" , ship->interdict, fread_number( fp ) );
            break;
        
        case 'L':
            KEY( "Laserdamage",   ship->laserdamage,   ( fread_number( fp ) ) );
            KEY( "Lasers",   ship->lasers,      fread_number( fp ) );
            KEY( "Lastdoc",    ship->lastdoc,       fread_number( fp ) );
            KEY( "Lastroom",   ship->lastroom,        fread_number( fp ) );
            break;

        case 'M':
             KEY( "MaxCargo",    ship->maxcargo,   fread_number( fp ) );
            KEY( "Manuever",   ship->manuever,      fread_number( fp ) );
            KEY( "Maxmissiles",   ship->maxmissiles,      fread_number( fp ) );
            KEY( "Maxtorpedos",   ship->maxtorpedos,      fread_number( fp ) );
            KEY( "Maxrockets",   ship->maxrockets,      fread_number( fp ) );
            KEY( "Maxarmor",   ship->maxarmor,      fread_number( fp ) );
            KEY( "Missiles",   ship->missiles,      fread_number( fp ) );
            KEY( "Missiletype",   ship->missiletype,      fread_number( fp ) );
            KEY( "Maxshield",      ship->maxshield,        fread_number( fp ) );
            KEY( "Maxenergy",      ship->maxenergy,        fread_number( fp ) );
            KEY( "Missilestate",   ship->missilestate,        fread_number( fp ) );
            KEY( "Maxhull",      ship->maxhull,        fread_number( fp ) );
            KEY( "Maxchaff",       ship->maxchaff,      fread_number( fp ) );
	    KEY( "MaxModules",		ship->maxmodules,	fread_number( fp ) );
            KEY( "Mlaunchers",     ship->mlaunchers,    fread_number( fp ) );
            if(!str_cmp(word, "Module"))
            {
               ship->module_vnum[modules] = fread_number( fp );
               ship->modules += 1;
               modules++;
               update_ship_modules(ship);
/*               bug("Ship module read, and updated.", 0); */
               fMatch = 1;
            }                       
             break;

	case 'N':
	    KEY( "Name",	ship->name,		fread_string( fp ) );
	    KEY( "Navseat",     ship->navseat,          fread_number( fp ) );
            break;
  
        case 'O':
            KEY( "Owner",            ship->owner,            fread_string( fp ) );
            KEY( "Objectnum",        dummy_number,        fread_number( fp ) );
            break;
        
        case 'P':
            KEY( "Pilot",            ship->pilot,            fread_string( fp ) ); 
            KEY( "Pilotseat",     ship->pilotseat,          fread_number( fp ) );
            break;
        
        case 'R':
            KEY( "Rlaunchers",  ship->rlaunchers,      fread_number( fp ) );
            KEY( "Realspeed",   ship->realspeed,       fread_number( fp ) );
            KEY( "Rockets",     ship->rockets,         fread_number( fp ) );
            break;
       
        case 'S':
            KEY( "Shipyard",    ship->shipyard,      fread_number( fp ) );
            KEY( "Sensor",      ship->sensor,       fread_number( fp ) );
            KEY( "Shield",      ship->shield,        fread_number( fp ) );
            KEY( "Shipstate",   ship->shipstate,        fread_number( fp ) );
            KEY( "Statet0",   ship->statet0,        fread_number( fp ) );
            KEY( "Statet1",   ship->statet1,        fread_number( fp ) );
            KEY( "Statet2",   ship->statet2,        fread_number( fp ) ); 


	case 'T':
            KEY( "Tlaunchers", ship->tlaunchers, fread_number( fp ) );
	    KEY( "Type",	ship->type,	fread_number( fp ) );
	    KEY( "Tractorbeam", ship->tractorbeam,      fread_number( fp ) );
	    KEY( "Turret1",	ship->turret1,	fread_number( fp ) );
	    KEY( "Turret2",	ship->turret2,	fread_number( fp ) );
	    KEY( "Torpedos",	ship->torpedos,	fread_number( fp ) );
	    break;
	}

	if ( !fMatch )
	{
	    sprintf( buf, "Fread_ship: no match: %s", word );
	    bug( buf, 0 );
	}
    }
}

/*
 * Load a ship file
 */

bool load_ship_file( char *shipfile )
{
    char filename[256];
    SHIP_DATA *ship;
    FILE *fp;
    bool found;
    ROOM_INDEX_DATA *pRoomIndex;
    CLAN_DATA *clan;
        
    CREATE( ship, SHIP_DATA, 1 );

    found = FALSE;
    sprintf( filename, "%s%s", SHIP_DIR, shipfile );

    if ( ( fp = fopen( filename, "r" ) ) != NULL )
    {

	found = TRUE;
	for ( ; ; )
	{
	    char letter;
	    char *word;

	    letter = fread_letter( fp );
	    if ( letter == '*' )
	    {
		fread_to_eol( fp );
		continue;
	    }

	    if ( letter != '#' )
	    {
		bug( "Load_ship_file: # not found.", 0 );
		break;
	    }

	    word = fread_word( fp );
	    if ( !str_cmp( word, "SHIP"	) )
	    {
	    	fread_ship( ship, fp );
	    	break;
	    }
	    else if ( !str_cmp( word, "MODS" ) )	
		{		
			/*log_string( "Read Modules" );*/
			fread_modules( ship, fp );
			/*log_string( "Use Modules" );*/
			update_ship_modules( ship );
		}
	    else if ( !str_cmp( word, "END"	) )
	        break;
	    else
	    {
		char buf[MAX_STRING_LENGTH];

		sprintf( buf, "Load_ship_file: bad section: %s.", word );
		bug( buf, 0 );
		break;
	    }
	}
	fclose( fp );
    }
    if ( !(found) )
      DISPOSE( ship );
    else
    {      
       LINK( ship, first_ship, last_ship, next, prev );
       if ( !str_cmp("Public",ship->owner) || ship->type == MOB_SHIP )
       {
       
         if ( ship->class != SHIP_PLATFORM && ship->type != MOB_SHIP && 
ship->class != CAPITAL_SHIP && ship->class != SUPERCAPITAL_SHIP && 
ship->class != FRIGATE_SHIP )
         {
           extract_ship( ship );
           ship_to_room( ship , ship->shipyard ); 
     
           ship->location = ship->shipyard;
           ship->lastdoc = ship->shipyard; 
           ship->shipstate = SHIP_DOCKED;
           }

     ship->currspeed=0;
     ship->energy=ship->maxenergy;
     ship->chaff=ship->maxchaff;
     ship->hull=ship->maxhull;
     ship->shield=0;
     
     ship->ionstate = LASER_READY; 
     ship->statet1 = LASER_READY; 
     ship->statet2 = LASER_READY; 
     ship->statet0 = LASER_READY; 
     ship->missilestate = LASER_READY;
       
     ship->currjump=NULL;
     ship->target0=NULL;
     ship->target1=NULL;
     ship->target2=NULL;
     
     ship->hatchopen = FALSE;
     ship->bayopen = TRUE;
     
     ship->missiles = ship->maxmissiles;
     ship->torpedos = ship->maxtorpedos;
     ship->rockets = ship->maxrockets;
     ship->autorecharge = FALSE;
     ship->autotrack = FALSE;
     ship->autospeed = FALSE;
          

       }

       else if ( ship->cockpit == ROOM_SHUTTLE_BUS || 
                 ship->cockpit == ROOM_SHUTTLE_BUS_2 || 
                 ship->cockpit == ROOM_SENATE_SHUTTLE || 
                 ship->cockpit == ROOM_CORUSCANT_TURBOCAR ||
                 ship->cockpit == ROOM_CORUSCANT_SHUTTLE   )
       {}
       else if ( ( pRoomIndex = get_room_index( ship->lastdoc ) ) != NULL 
            && ship->class != CAPITAL_SHIP && ship->class != SHIP_PLATFORM 
&& ship->class != SUPERCAPITAL_SHIP && ship->class != FRIGATE_SHIP ) 
       {
              LINK( ship, pRoomIndex->first_ship, pRoomIndex->last_ship, next_in_room, prev_in_room );
              ship->in_room = pRoomIndex;
              ship->location = ship->lastdoc;                                                                           
       }
           

       if ( ship->class == SHIP_PLATFORM || ship->type == MOB_SHIP || 
ship->class == CAPITAL_SHIP || ship->class == SUPERCAPITAL_SHIP || 
ship->class == FRIGATE_SHIP )
       {
          ship_to_starsystem(ship, starsystem_from_name(ship->home) );  
          ship->vx = number_range( -5000 , 5000 );
          ship->vy = number_range( -5000 , 5000 );
          ship->vz = number_range( -5000 , 5000 );
          ship->hx = 1;
          ship->hy = 1;
          ship->hz = 1;
          ship->shipstate = SHIP_READY;
          ship->autopilot = TRUE;
          ship->autorecharge = TRUE;
          ship->shield = ship->maxshield;
       }

         if ( ship->type != MOB_SHIP && (clan = get_clan( ship->owner )) != NULL )
         {
          if ( ship->class <= SHIP_PLATFORM )
             clan->spacecraft++;
          else
             clan->vehicles++;
         }  
         
    }
    
    return found;
}

/*
 * Load in all the ship files.
 */
void load_ships( )
{
    FILE *fpList;
    char *filename;
    char shiplist[256];
    char buf[MAX_STRING_LENGTH];
    
    
    first_ship	= NULL;
    last_ship	= NULL;
    first_missile = NULL;
    last_missile = NULL;
    
    log_string( "Loading ships..." );

    sprintf( shiplist, "%s%s", SHIP_DIR, SHIP_LIST );
    fclose( fpReserve );
    if ( ( fpList = fopen( shiplist, "r" ) ) == NULL )
    {
	perror( shiplist );
	exit( 1 );
    }

    for ( ; ; )
    {
    
	filename = feof( fpList ) ? "$" : fread_word( fpList );

	if ( filename[0] == '$' )
	  break;
	         
	if ( !load_ship_file( filename ) )
	{
	  sprintf( buf, "Cannot load ship file: %s", filename );
	  bug( buf, 0 );
	}

    }
    fclose( fpList );
    log_string(" Done ships " );
    fpReserve = fopen( NULL_FILE, "r" );
    return;
}

void resetship( SHIP_DATA *ship )
{
     ship->shipstate = SHIP_READY;
     
     if ( ship->class != SHIP_PLATFORM && ship->type != MOB_SHIP)
     {
           extract_ship( ship );
           ship_to_room( ship , ship->shipyard );      
           ship->location = ship->shipyard;
           ship->lastdoc = ship->shipyard; 
           ship->shipstate = SHIP_DOCKED;
           ship->shipstate2 = SHIP_READY;
     }
     rem_extshipflags(ship);
     if (ship->starsystem)
        ship_from_starsystem( ship, ship->starsystem );  
     ship->currspeed=0;
     ship->energy=ship->maxenergy;
     ship->chaff=ship->maxchaff;
     ship->hull=ship->maxhull;
     ship->shield=0;
     
     ship->ionstate = LASER_READY; 
     ship->statet1 = LASER_READY; 
     ship->statet2 = LASER_READY; 
     ship->statet0 = LASER_READY; 
     ship->missilestate = MISSILE_READY;
     ship->torpedostate = MISSILE_READY;
     ship->rocketstate  = MISSILE_READY;
     ship->currjump=NULL;
     ship->target0=NULL;
     ship->target1=NULL;
     ship->target2=NULL;
     
     ship->hatchopen = FALSE;
     ship->bayopen = TRUE;
     
     ship->missiles = ship->maxmissiles;
     ship->torpedos = ship->maxtorpedos;
     ship->rockets = ship->maxrockets;
     ship->autorecharge = FALSE;
     ship->autotrack = FALSE;
     ship->autospeed = FALSE;

     ship->sdestnum = -1;
     
     if ( str_cmp("Public",ship->owner) && ship->type != MOB_SHIP && !IS_SET(ship->flags, SHIPFLAG_SIMULATOR))
     {
        CLAN_DATA *clan;
        
        if ( ship->type != MOB_SHIP && (clan = get_clan( ship->owner )) != NULL )
	{
          if ( ship->class <= SHIP_PLATFORM || ship->class == MOBILE_SUIT)
             clan->spacecraft--;
          else 
             clan->vehicles--;
       	}

        STRFREE( ship->owner );
        ship->owner = STRALLOC( "" );
        STRFREE( ship->pilot );
        ship->pilot = STRALLOC( "" );
        STRFREE( ship->copilot );
        ship->copilot = STRALLOC( "" );
     }
     
     save_ship(ship);               
}

void do_resetship( CHAR_DATA *ch, char *argument )
{    
     SHIP_DATA *ship;
     
     ship = get_ship( argument );
     if (ship == NULL)
     {
        send_to_char("&RNo such ship!",ch);
        return;
     } 
     
     resetship( ship ); 
     
     if ( ship->type == MOB_SHIP )
     {
          ship_to_starsystem(ship, starsystem_from_name(ship->home) );  
          ship->vx = number_range( -5000 , 5000 );
          ship->vy = number_range( -5000 , 5000 );
          ship->vz = number_range( -5000 , 5000 );
          ship->shipstate = SHIP_READY;
          ship->autopilot = TRUE;
          ship->autorecharge = TRUE;
          ship->shield = ship->maxshield;
     }

     if ( ship->type != MOB_SHIP && ship->class >= FRIGATE_SHIP && ship->class < MOBILE_SUIT )
     {
          ship_to_starsystem(ship, starsystem_from_name(ship->home) );  
          ship->vx = number_range( -5000 , 5000 );
          ship->vy = number_range( -5000 , 5000 );
          ship->vz = number_range( -5000 , 5000 );
          ship->shipstate = SHIP_READY;
          ship->autopilot = TRUE;
          ship->autorecharge = TRUE;
          ship->shield = ship->maxshield;
     }
}         

void do_setship( CHAR_DATA *ch, char *argument )
{
    char arg1[MAX_INPUT_LENGTH];
    char arg2[MAX_INPUT_LENGTH];
    char arg3[MAX_INPUT_LENGTH];
    SHIP_DATA *ship;
    int  tempnum;
    ROOM_INDEX_DATA *roomindex;
    
    if ( IS_NPC( ch ) )
    {
	send_to_char( "Huh?\n\r", ch );
	return;
    }

    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );

    if ( arg1[0] == '\0' || arg2[0] == '\0' || arg1[0] == '\0' )
    {
	send_to_char( "Usage: setship <ship> <field> <values>\n\r", ch );
	send_to_char( "\n\rField being one of:\n\r", ch );
	send_to_char( "filename name owner copilot pilot description home\n\r", ch );
	send_to_char( "cockpit entrance turret1 turret2 hangar\n\r", ch );
	send_to_char( "engineroom firstroom lastroom shipyard\n\r", ch );
	send_to_char( "manuever speed hyperspeed tractorbeam\n\r", ch );
	send_to_char( "lasers missiles shield hull energy chaff\n\r", ch );
	send_to_char( "comm sensor astroarray class torpedos\n\r", ch );
	send_to_char( "pilotseat coseat gunseat navseat rockets dock\n\r", ch );
        send_to_char( "maxmodules laserdamage interdict ions\n\r", ch );
        send_to_char( "autocannon, autodamage, autoammo, armor, flags\n\r", ch );
	send_to_char( "maxcargo mlaunchers tlaunchers rlaunchers\n\r",ch );
        send_to_char( "cloak\n\r", ch);
	return;
    }

    ship = get_ship( arg1 );
    if ( !ship )
    {
	send_to_char( "No such ship.\n\r", ch );
	return;
    }

    if ( ship->class == MOBILE_SUIT && get_trust( ch ) <= (MAX_LEVEL -1) )
    {
	send_to_char( "At current, Only Head Imms can modify Coralskippers.\n\r", ch );
	return;
    }
    if ( !str_cmp( arg2, "maxcargo" ) )
    {
        ship->maxcargo = URANGE(0, atoi(argument), 500);
        send_to_char( "Done.\n\r", ch );
        save_ship(ship);
        return;
    }
    
    if ( !str_cmp( arg2, "maxmodules" ) )
	{
		if (atoi(argument) < ship->modules)
		{
			send_to_char("Please remove some modules first.\n\r", ch);
			return; 
		}
                ship->maxmodules = URANGE(10, atoi(argument), MAX_MODULES);	
		return;
	}
    if ( !str_cmp( arg2, "owner" ) )
    {
         CLAN_DATA *clan;
         if ( ship->type != MOB_SHIP && (clan = get_clan( ship->owner )) != NULL )
         {
          if ( ship->class <= SHIP_PLATFORM )
             clan->spacecraft--;
          else
             clan->vehicles--;
         }
	STRFREE( ship->owner );
	ship->owner = STRALLOC( argument );
	send_to_char( "Done.\n\r", ch );
	save_ship( ship );
	if ( ship->type != MOB_SHIP && (clan = get_clan( ship->owner )) != NULL )
         {
          if ( ship->class <= SHIP_PLATFORM )
             clan->spacecraft++;
          else
             clan->vehicles++;
         }
	return;
    }
    
    if ( !str_cmp( arg2, "home" ) )
    {
	STRFREE( ship->home );
	ship->home = STRALLOC( argument );
	send_to_char( "Done.\n\r", ch );
	save_ship( ship );
	return;
    }
    
    if ( !str_cmp( arg2, "pilot" ) )
    {
	STRFREE( ship->pilot );
	ship->pilot = STRALLOC( argument );
	send_to_char( "Done.\n\r", ch );
	save_ship( ship );
	return;
    }

    if ( !str_cmp( arg2, "copilot" ) )
    {
	STRFREE( ship->copilot );
	ship->copilot = STRALLOC( argument );
	send_to_char( "Done.\n\r", ch );
	save_ship( ship );
	return;
    }
   
    if ( !str_cmp( arg2, "firstroom" ) )
    {   
        tempnum = atoi(argument); 
    	roomindex = get_room_index(tempnum);
    	if (roomindex == NULL)
    	{
    	   send_to_char("That room doesn't exist.\n\r",ch);
    	   return;
    	} 
	ship->firstroom = tempnum;
        ship->lastroom = tempnum;
        ship->cockpit = tempnum;
        ship->coseat = tempnum;
        ship->pilotseat = tempnum;
        ship->gunseat = tempnum;
        ship->navseat = tempnum;
        ship->entrance = tempnum;
        ship->turret1 = 0;
        ship->turret2 = 0;
        ship->hanger = 0;
	ship->dock = 0;
	send_to_char( "You will now need to set the other rooms in the ship.\n\r", ch );
	save_ship( ship );
	return;
    }

    if ( !str_cmp( arg2, "lastroom" ) )
    {   
        tempnum = atoi(argument); 
    	roomindex = get_room_index(tempnum);
    	if (roomindex == NULL)
    	{
    	   send_to_char("That room doesn't exist.\n\r",ch);
    	   return;
    	} 
    	if ( tempnum < ship->firstroom )
    	{
    	   send_to_char("The last room on a ship must be greater than or equal to the first room.\n\r",ch);
           return;
    	}
    	if ( ship->class == FIGHTER_SHIP && (tempnum - ship->firstroom) > 5 )
    	{
    	   send_to_char("Starfighters may have up to 5 rooms only.\n\r",ch);
    	   return;
    	}  
	if ( ship->class == MIDSIZE_SHIP && (tempnum - ship->firstroom) > 25 && (tempnum - ship->firstroom) < 3 )
    	{
    	   send_to_char("Midships may have up to 25 rooms only And a minimum of 3.\n\r",ch);
    	   return;
    	}  
	if ( ship->class == FRIGATE_SHIP && (tempnum - ship->firstroom) > 60 && (tempnum - ship->firstroom) < 15)
    	{
    	   send_to_char("Frigates must have between 15 and 60 rooms only.\n\r",ch);
    	   return;
    	}  
	if ( ship->class == CAPITAL_SHIP && (tempnum - ship->firstroom) > 100 && (tempnum - ship->firstroom) < 50)
    	{
    	   send_to_char("Capital Ships must have between 50 and 100 rooms only.\n\r",ch);
    	   return;
    	}  
	if ( ship->class == SUPERCAPITAL_SHIP && (tempnum - ship->firstroom) > 250 && (tempnum - ship->firstroom) < 75)
    	{
    	   send_to_char("Super-Capital Ships must have between 75 and 250 rooms only.\n\r",ch);
    	   return;
    	}  
	if ( ship->class == SHIP_PLATFORM && (tempnum - ship->firstroom) < 15)
    	{
    	   send_to_char("Platforms must have more than 15 Rooms.\n\r",ch);
    	   return;
    	}  
	if ( ship->class == MOBILE_SUIT && tempnum != ship->firstroom)
    	{
    	   send_to_char("Coralskippers may only have a single Room.\n\r",ch);
    	   return;
    	}  
	if ( ship->class == CLOUD_CAR && (tempnum - ship->firstroom) > 3)
    	{
    	   send_to_char("Super-Capital Ships must have between 1 and 3 rooms only.\n\r",ch);
    	   return;
    	}  
	if ( ship->class == LAND_SPEEDER && tempnum != ship->firstroom)
    	{
    	   send_to_char("Speeders may only have a Single Room.\n\r",ch);
    	   return;
    	}  
	if ( ship->class == WALKER && (tempnum - ship->firstroom) > 2 && (tempnum - ship->firstroom) < 15)
    	{
    	   send_to_char("Walkers must have between 2 and 15 rooms only.\n\r",ch);
    	   return;
    	}  
	ship->lastroom = tempnum;
	send_to_char( "Done.\n\r", ch );
	save_ship( ship );
	return;
    }

    if ( !str_cmp( arg2, "cockpit" ) )
    {   
        tempnum = atoi(argument); 
    	roomindex = get_room_index(tempnum);
    	if (roomindex == NULL)
    	{
    	   send_to_char("That room doesn't exist.\n\r",ch);
    	   return;
    	} 
    	if ( tempnum < ship->firstroom || tempnum > ship->lastroom )
    	{
    	   send_to_char("That room number is not in that ship .. \n\rIt must be between Firstroom and Lastroom.\n\r",ch);
           return;
    	}
	if ( tempnum == ship->turret1 || tempnum == ship->turret2 || tempnum == ship->hanger )
    	{
    	   send_to_char("That room is already being used by another part of the ship\n\r",ch);
           return;
    	}
	ship->cockpit = tempnum;
	send_to_char( "Done.\n\r", ch );
	save_ship( ship );
	return;
    }
    
    if ( !str_cmp( arg2, "pilotseat" ) )
    {   
        tempnum = atoi(argument); 
    	roomindex = get_room_index(tempnum);
    	if (roomindex == NULL)
    	{
    	   send_to_char("That room doesn't exist.\n\r",ch);
    	   return;
    	} 
    	if ( tempnum < ship->firstroom || tempnum > ship->lastroom )
    	{
    	   send_to_char("That room number is not in that ship .. \n\rIt must be between Firstroom and Lastroom.\n\r",ch);
           return;
    	}
	if ( tempnum == ship->turret1 || tempnum == ship->turret2 || tempnum == ship->hanger )
    	{
    	   send_to_char("That room is already being used by another part of the ship\n\r",ch);
           return;
    	}
	ship->pilotseat = tempnum;
	send_to_char( "Done.\n\r", ch );
	save_ship( ship );
	return;
    }
    if ( !str_cmp( arg2, "coseat" ) )
    {   
        tempnum = atoi(argument); 
    	roomindex = get_room_index(tempnum);
    	if (roomindex == NULL)
    	{
    	   send_to_char("That room doesn't exist.\n\r",ch);
    	   return;
    	} 
    	if ( tempnum < ship->firstroom || tempnum > ship->lastroom )
    	{
    	   send_to_char("That room number is not in that ship .. \n\rIt must be between Firstroom and Lastroom.\n\r",ch);
           return;
    	}
	if ( tempnum == ship->turret1 || tempnum == ship->turret2 || tempnum == ship->hanger )
    	{
    	   send_to_char("That room is already being used by another part of the ship\n\r",ch);
           return;
    	}
	ship->coseat = tempnum;
	send_to_char( "Done.\n\r", ch );
	save_ship( ship );
	return;
    }
    if ( !str_cmp( arg2, "navseat" ) )
    {   
        tempnum = atoi(argument); 
    	roomindex = get_room_index(tempnum);
    	if (roomindex == NULL)
    	{
    	   send_to_char("That room doesn't exist.\n\r",ch);
    	   return;
    	} 
    	if ( tempnum < ship->firstroom || tempnum > ship->lastroom )
    	{
    	   send_to_char("That room number is not in that ship .. \n\rIt must be between Firstroom and Lastroom.\n\r",ch);
           return;
    	}
	if ( tempnum == ship->turret1 || tempnum == ship->turret2 || tempnum == ship->hanger )
    	{
    	   send_to_char("That room is already being used by another part of the ship\n\r",ch);
           return;
    	}
	ship->navseat = tempnum;
	send_to_char( "Done.\n\r", ch );
	save_ship( ship );
	return;
    }
    if ( !str_cmp( arg2, "gunseat" ) )
    {   
        tempnum = atoi(argument); 
    	roomindex = get_room_index(tempnum);
    	if (roomindex == NULL)
    	{
    	   send_to_char("That room doesn't exist.\n\r",ch);
    	   return;
    	} 
    	if ( tempnum < ship->firstroom || tempnum > ship->lastroom )
    	{
    	   send_to_char("That room number is not in that ship .. \n\rIt must be between Firstroom and Lastroom.\n\r",ch);
           return;
    	}
	if ( tempnum == ship->turret1 || tempnum == ship->turret2 || tempnum == ship->hanger )
    	{
    	   send_to_char("That room is already being used by another part of the ship\n\r",ch);
           return;
    	}
	ship->gunseat = tempnum;
	send_to_char( "Done.\n\r", ch );
	save_ship( ship );
	return;
    }
    
    if ( !str_cmp( arg2, "entrance" ) )
    {   
        tempnum = atoi(argument); 
    	roomindex = get_room_index(tempnum);
    	if (roomindex == NULL)
    	{
    	   send_to_char("That room doesn't exist.\n\r",ch);
    	   return;
    	} 
    	if ( tempnum < ship->firstroom || tempnum > ship->lastroom )
    	{
    	   send_to_char("That room number is not in that ship .. \n\rIt must be between Firstroom and Lastroom.\n\r",ch);
           return;
    	}
	ship->entrance = tempnum;
	send_to_char( "Done.\n\r", ch );
	save_ship( ship );
	return;
    }

    if ( !str_cmp( arg2, "turret1" ) )
    {   
        tempnum = atoi(argument); 
    	roomindex = get_room_index(tempnum);
    	if (roomindex == NULL)
    	{
    	   send_to_char("That room doesn't exist.\n\r",ch);
    	   return;
    	} 
	if ( tempnum < ship->firstroom || tempnum > ship->lastroom )
    	{
    	   send_to_char("That room number is not in that ship .. \n\rIt must be between Firstroom and Lastroom.\n\r",ch);
           return;
    	}
	if ( ship->class >= MOBILE_SUIT )
    	{
    	   send_to_char("This Ship can't have extra laser turrets.\n\r",ch);
    	   return;
    	}  
    	if ( ship->class == FIGHTER_SHIP )
    	{
    	   send_to_char("Starfighters can't have extra laser turrets.\n\r",ch);
    	   return;
    	}  
	if ( tempnum == ship->cockpit || tempnum == ship->entrance ||
    	     tempnum == ship->turret2 || tempnum == ship->hanger || tempnum == ship->engineroom )
    	{
    	   send_to_char("That room is already being used by another part of the ship\n\r",ch);
           return;
    	}
	ship->turret1 = tempnum;
	send_to_char( "Done.\n\r", ch );
	save_ship( ship );
	return;
    }

    if ( !str_cmp( arg2, "turret2" ) )
    {   
        tempnum = atoi(argument); 
    	roomindex = get_room_index(tempnum);
    	if (roomindex == NULL)
    	{
    	   send_to_char("That room doesn't exist.\n\r",ch);
    	   return;
    	} 
	if ( tempnum < ship->firstroom || tempnum > ship->lastroom )
    	{
    	   send_to_char("That room number is not in that ship .. \n\rIt must be between Firstroom and Lastroom.\n\r",ch);
           return;
    	}
	if ( ship->class == FIGHTER_SHIP )
    	{
    	   send_to_char("Starfighters can't have extra laser turrets.\n\r",ch);
    	   return;
    	}  
	if ( ship->class >= MOBILE_SUIT )
    	{
    	   send_to_char("This Ship can't have extra laser turrets.\n\r",ch);
    	   return;
    	}  
	if ( tempnum == ship->cockpit || tempnum == ship->entrance ||
    	     tempnum == ship->turret1 || tempnum == ship->hanger || tempnum == ship->engineroom )
    	{
    	   send_to_char("That room is already being used by another part of the ship\n\r",ch);
           return;
    	}
	ship->turret2 = tempnum;
	send_to_char( "Done.\n\r", ch );
	save_ship( ship );
	return;
    }
    if ( !str_cmp( arg2, "dock" ) )
    {
	tempnum = atoi(argument);
	roomindex = get_room_index(tempnum);
	if (roomindex == NULL)
	{
	  send_to_char( "That room doesn't exits.\n\r", ch );
	  return;
	}
	if ( tempnum < ship->firstroom || tempnum > ship->lastroom )
	{
	  send_to_char( "That room number is not in that ship..\n\rIt must be between Firstroom and Lastroom.\n\r", ch );
	  return;
	}
	ship->dock = tempnum;
	send_to_char( "Done.\n\r", ch );
	save_ship( ship );
	return;
}
    if ( !str_cmp( arg2, "hangar" ) )
    {   
        tempnum = atoi(argument); 
    	roomindex = get_room_index(tempnum);
    	if (roomindex == NULL)
    	{
    	   send_to_char("That room doesn't exist.\n\r",ch);
    	   return;
    	} 
	if ( tempnum < ship->firstroom || tempnum > ship->lastroom )
    	{
    	   send_to_char("That room number is not in that ship .. \n\rIt must be between Firstroom and Lastroom.\n\r",ch);
           return;
    	}
    	if ( tempnum == ship->cockpit || tempnum == ship->turret1 || tempnum == ship->turret2 || tempnum == ship->engineroom )
    	{
    	   send_to_char("That room is already being used by another part of the ship\n\r",ch);
           return;
    	}
	if ( ship->class <= MIDSIZE_SHIP )
	{
	   send_to_char("Starfighters are to small to have hangars for other ships!\n\r",ch);
	   return;
	}
	if ( ship->class >= MOBILE_SUIT )
    	{
    	   send_to_char("This Ship can't have hangars.\n\r",ch);
    	   return;
    	}  
	ship->hanger = tempnum;
	send_to_char( "Done.\n\r", ch );
	save_ship( ship );
	return;
    }

   if ( !str_cmp( arg2, "engineroom" ) )
    {   
        tempnum = atoi(argument); 
    	roomindex = get_room_index(tempnum);
    	if (roomindex == NULL)
    	{
    	   send_to_char("That room doesn't exist.\n\r",ch);
    	   return;
    	} 
	if ( tempnum < ship->firstroom || tempnum > ship->lastroom )
    	{
    	   send_to_char("That room number is not in that ship .. \n\rIt must be between Firstroom and Lastroom.\n\r",ch);
           return;
    	}
    	if ( tempnum == ship->cockpit || tempnum == ship->entrance ||
    	     tempnum == ship->turret1 || tempnum == ship->turret2 || tempnum == ship->hanger )
    	{
    	   send_to_char("That room is already being used by another part of the ship\n\r",ch);
           return;
    	}
	ship->engineroom = tempnum;
	send_to_char( "Done.\n\r", ch );
	save_ship( ship );
	return;
    }

    if ( !str_cmp( arg2, "shipyard" ) )
    {   
        tempnum = atoi(argument); 
    	roomindex = get_room_index(tempnum);
    	if (roomindex == NULL)
    	{
    	   send_to_char("That room doesn't exist.",ch);
    	   return;
    	} 
	ship->shipyard = tempnum;
	send_to_char( "Done.\n\r", ch );
	save_ship( ship );
	return;
    }

    if ( !str_cmp( arg2, "type" ) )
    {
	if ( !str_cmp( argument, "republic" ) )
	  ship->type = SHIP_REPUBLIC;
	else if ( !str_cmp( argument, "imperial" ) )
	  ship->type = SHIP_IMPERIAL;
/*	else if ( !str_cmp( argument, "vong" ) )
	  ship->type = SHIP_VONG; No Vong Craft */
	else if ( !str_cmp( argument, "civilian" ) )
	  ship->type = SHIP_CIVILIAN;
	else if ( !str_cmp( argument, "mob" ) )
	  ship->type = MOB_SHIP;
	else
	{
	   send_to_char( "Ship type must be either: republic, imperial, civilian or mob.\n\r", ch );
	   return;
	}
	send_to_char( "Done.\n\r", ch );
	save_ship( ship );
	return;
    }

    if ( !str_cmp( arg2, "name" ) )
    {
	STRFREE( ship->name );
	ship->name = STRALLOC( argument );
	send_to_char( "Done.\n\r", ch );
	save_ship( ship );
	return;
    }

    if ( !str_cmp( arg2, "filename" ) )
    {
	DISPOSE( ship->filename );
	ship->filename = str_dup( argument );
	send_to_char( "Done.\n\r", ch );
	save_ship( ship );
	write_ship_list( );
	return;
    }
 
    if ( !str_cmp( arg2, "desc" ) )
    {
	STRFREE( ship->description );
	ship->description = STRALLOC( argument );
	send_to_char( "Done.\n\r", ch );
	save_ship( ship );
	return;
    }

    if ( !str_cmp( arg2, "manuever" ) )
    { 
       if ( ship->class == FIGHTER_SHIP )
       {  
	ship->manuever = URANGE( 0, atoi(argument) , 200 );
	send_to_char( "Done.\n\r", ch );
	save_ship( ship );
	return;
       }
       if ( ship->class == MIDSIZE_SHIP )
       {
	ship->manuever = URANGE( 0, atoi(argument) , 150 );
	send_to_char( "Done.\n\r", ch );
	save_ship( ship );
	return;
       }
       if ( ship->class == FRIGATE_SHIP )
       {
	ship->manuever = URANGE( 0, atoi(argument) , 75 );
	send_to_char( "Done.\n\r", ch );
	save_ship( ship );
	return;
       }
       if ( ship->class == CAPITAL_SHIP )
       {
        ship->manuever = URANGE( 0, atoi(argument) , 50 );
  	send_to_char( "Done.\n\r", ch );
	save_ship( ship );
	return;
       }
       if ( ship->class == SUPERCAPITAL_SHIP )
       {
        ship->manuever = URANGE( 0, atoi(argument) , 15 );
  	send_to_char( "Done.\n\r", ch );
	save_ship( ship );
	return;
       }
       if ( ship->class == SHIP_PLATFORM )
       {
	ship->manuever = URANGE( 0, atoi(argument) , 5 );
	send_to_char( "Done.\n\r", ch );
	save_ship( ship );
	return;
       }
       if ( ship->class == MOBILE_SUIT )
       {
	ship->manuever = URANGE( 0, atoi(argument) , 250 );
	send_to_char( "Done.\n\r", ch );
	save_ship( ship );
	return;
       }
       if ( ship->class == CLOUD_CAR )
       {
	ship->manuever = URANGE( 0, atoi(argument) , 75 );
	send_to_char( "Done.\n\r", ch );
	save_ship( ship );
	return;
       }
       if ( ship->class == LAND_SPEEDER )
       {
	ship->manuever = URANGE( 0, atoi(argument) , 50 );
	send_to_char( "Done.\n\r", ch );
	save_ship( ship );
	return;
       }
       if ( ship->class == WALKER )
       {
	ship->manuever = URANGE( 0, atoi(argument) , 5 );
	send_to_char( "Done.\n\r", ch );
	save_ship( ship );
	return;
       }
    }

    if ( !str_cmp( arg2, "laserdamage" ) )
    {
       if ( ship->class == FIGHTER_SHIP )
       {   
	ship->laserdamage = URANGE( 0, atoi(argument) , 5 );
	send_to_char( "Done.\n\r", ch );
	save_ship( ship );
	return;
       }
       if ( ship->class == MIDSIZE_SHIP )
       {
	ship->laserdamage = URANGE( 0, atoi(argument) , 10 );
	send_to_char( "Done.\n\r", ch );
	save_ship( ship );
	return;
       }
       if ( ship->class == FRIGATE_SHIP )
       {
	ship->laserdamage = URANGE( 0, atoi(argument) , 15 );
	send_to_char( "Done.\n\r", ch );
	save_ship( ship );
	return;
       }
       if ( ship->class == CAPITAL_SHIP )
       {
	ship->laserdamage = URANGE( 0, atoi(argument) , 20 );
	send_to_char( "Done.\n\r", ch );
	save_ship( ship );
	return;
       }
       if ( ship->class == SUPERCAPITAL_SHIP )
       {
	ship->laserdamage = URANGE( 0, atoi(argument) , 25 );
	send_to_char( "Done.\n\r", ch );
	save_ship( ship );
	return;
       }
       if ( ship->class == SHIP_PLATFORM )
       {
   	ship->laserdamage = URANGE( 0, atoi(argument) , 50 );
	send_to_char( "Done.\n\r", ch );
	save_ship( ship );
	return;
       }
       if ( ship->class == MOBILE_SUIT )
       {
	ship->laserdamage = URANGE( 0, atoi(argument) , 255 );
	send_to_char( "Done.\n\r", ch );
	save_ship( ship );
	return;
       }
       if ( ship->class == CLOUD_CAR )
       {
	ship->laserdamage = URANGE( 0, atoi(argument) , 7 );
	send_to_char( "Done.\n\r", ch );
	save_ship( ship );
	return;
       }
       if ( ship->class == LAND_SPEEDER )
       {
	ship->laserdamage = URANGE( 0, atoi(argument) , 2 );
	send_to_char( "Done.\n\r", ch );
	save_ship( ship );
	return;
       }
       if ( ship->class == WALKER )
       {
	ship->laserdamage = URANGE( 0, atoi(argument) , 10 );
	send_to_char( "Done.\n\r", ch );
	save_ship( ship );
	return;
       }

    }


    if ( !str_cmp( arg2, "lasers" ) )
    {
       if ( ship->class == FIGHTER_SHIP )
       {   
	ship->lasers = URANGE( 0, atoi(argument) , 4 );
	send_to_char( "Done.\n\r", ch );
	save_ship( ship );
	return;
       }
       if ( ship->class == MIDSIZE_SHIP )
       {
	ship->lasers = URANGE( 0, atoi(argument) , 6 );
	send_to_char( "Done.\n\r", ch );
	save_ship( ship );
	return;
       }
       if ( ship->class == FRIGATE_SHIP )
       {
	ship->lasers = URANGE( 0, atoi(argument) , 10 );
	send_to_char( "Done.\n\r", ch );
	save_ship( ship );
	return;
       }
       if ( ship->class == CAPITAL_SHIP )
       {
	ship->lasers = URANGE( 0, atoi(argument) , 15 );
	send_to_char( "Done.\n\r", ch );
	save_ship( ship );
	return;
       }
       if ( ship->class == SUPERCAPITAL_SHIP )
       {
	ship->lasers = URANGE( 0, atoi(argument) , 25 );
	send_to_char( "Done.\n\r", ch );
	save_ship( ship );
	return;
       }
       if ( ship->class == SHIP_PLATFORM )
       {
   	ship->lasers = URANGE( 0, atoi(argument) , 50 );
	send_to_char( "Done.\n\r", ch );
	save_ship( ship );
	return;
       }
       if ( ship->class == MOBILE_SUIT )
       {
	ship->lasers = URANGE( 0, atoi(argument) , 255 );
	send_to_char( "Done.\n\r", ch );
	save_ship( ship );
	return;
       }
       if ( ship->class == CLOUD_CAR )
       {
	ship->lasers = URANGE( 0, atoi(argument) , 2 );
	send_to_char( "Done.\n\r", ch );
	save_ship( ship );
	return;
       }
       if ( ship->class == LAND_SPEEDER )
       {
	ship->lasers = URANGE( 0, atoi(argument) , 1 );
	send_to_char( "Done.\n\r", ch );
	save_ship( ship );
	return;
       }
       if ( ship->class == WALKER )
       {
	ship->lasers = URANGE( 0, atoi(argument) , 6 );
	send_to_char( "Done.\n\r", ch );
	save_ship( ship );
	return;
       }
}

    if ( !str_cmp( arg2, "ions" ) )
    {
       if ( ship->class == FIGHTER_SHIP )
       {   
	ship->ions = URANGE( 0, atoi(argument) , 2 );
	send_to_char( "Done.\n\r", ch );
	save_ship( ship );
	return;
       }
       if ( ship->class == MIDSIZE_SHIP )
       {
	ship->ions = URANGE( 0, atoi(argument) , 3 );
	send_to_char( "Done.\n\r", ch );
	save_ship( ship );
	return;
       }
       if ( ship->class == FRIGATE_SHIP )
       {
	ship->ions = URANGE( 0, atoi(argument) , 5 );
	send_to_char( "Done.\n\r", ch );
	save_ship( ship );
	return;
       }
       if ( ship->class == CAPITAL_SHIP )
       {
	ship->ions = URANGE( 0, atoi(argument) , 10 );
	send_to_char( "Done.\n\r", ch );
	save_ship( ship );
	return;
       }
       if ( ship->class == SUPERCAPITAL_SHIP )
       {
	ship->ions = URANGE( 0, atoi(argument) , 15 );
	send_to_char( "Done.\n\r", ch );
	save_ship( ship );
	return;
       }
       if ( ship->class == SHIP_PLATFORM )
       {
   	ship->ions = URANGE( 0, atoi(argument) , 25 );
	send_to_char( "Done.\n\r", ch );
	save_ship( ship );
	return;
       }
       if ( ship->class == MOBILE_SUIT )
       {
	ship->ions = URANGE( 0, atoi(argument) , 255 );
	send_to_char( "Done.\n\r", ch );
	save_ship( ship );
	return;
       }
       if ( ship->class == CLOUD_CAR )
       {
	ship->ions = URANGE( 0, atoi(argument) , 1);
	send_to_char( "Done.\n\r", ch );
	save_ship( ship );
	return;
       }
       if ( ship->class == LAND_SPEEDER )
       {
	send_to_char( "Speeders can't have Ions!\n\r", ch );
	return;
       }
       if ( ship->class == WALKER )
       {
	ship->ions = URANGE( 0, atoi(argument) , 3 );
	send_to_char( "Done.\n\r", ch );
	save_ship( ship );
	return;
       }
}
    
    if ( !str_cmp( arg2, "class" ) )
    {   
	ship->class = URANGE( 0, atoi(argument) , 9 );
	if ( get_trust( ch ) != MAX_LEVEL && ship->class == MOBILE_SUIT )
	{
         send_to_char( "Sorry, Only Top Imms can set ships to Coralskippers.\n\r", ch );
         send_to_char( "Resetting to Starfighter.\n\r", ch );
	   ship->class = URANGE( 0, 0, 0);
	}	
	send_to_char( "Done.\n\r", ch );
	save_ship( ship );
	return;
    }

    if ( !str_cmp( arg2, "armor" ) )
    {   
       if ( ship->class == FIGHTER_SHIP )
       {   
	ship->maxarmor = URANGE( 0, atoi(argument) , 5 );
	ship->armor = URANGE( 0, atoi(argument) , 5 );
	send_to_char( "Done.\n\r", ch );
	save_ship( ship );
	return;
       }
       if ( ship->class == MIDSIZE_SHIP )
       {
	ship->maxarmor = URANGE( 0, atoi(argument) , 10 );
	ship->armor = URANGE( 0, atoi(argument) , 10 );
	send_to_char( "Done.\n\r", ch );
	save_ship( ship );
	return;
       }
       if ( ship->class == FRIGATE_SHIP )
       {
	ship->maxarmor = URANGE( 0, atoi(argument) , 25 );
	ship->armor = URANGE( 0, atoi(argument) , 25 );
	send_to_char( "Done.\n\r", ch );
	save_ship( ship );
	return;
       }
       if ( ship->class == CAPITAL_SHIP )
       {
	ship->maxarmor = URANGE( 0, atoi(argument) , 50 );
	ship->armor = URANGE( 0, atoi(argument) , 50 );
	send_to_char( "Done.\n\r", ch );
	save_ship( ship );
	return;
       }
       if ( ship->class == SUPERCAPITAL_SHIP )
       {
	ship->maxarmor = URANGE( 0, atoi(argument) , 75 );
	ship->armor = URANGE( 0, atoi(argument) , 75 );
	send_to_char( "Done.\n\r", ch );
	save_ship( ship );
	return;
       }
       if ( ship->class == SHIP_PLATFORM )
       {
	ship->maxarmor = URANGE( 0, atoi(argument) , 150 );
	ship->armor = URANGE( 0, atoi(argument) , 150 );
	send_to_char( "Done.\n\r", ch );
	save_ship( ship );
	return;
       }
       if ( ship->class == MOBILE_SUIT )
       {
	ship->maxmissiles = URANGE( 0, atoi(argument) , 255 );
	ship->missiles = URANGE( 0, atoi(argument) , 255 );
	send_to_char( "Done.\n\r", ch );
	save_ship( ship );
	return;
       }
       if ( ship->class == CLOUD_CAR )
       {
	ship->maxmissiles = URANGE( 0, atoi(argument) , 6 );
	ship->missiles = URANGE( 0, atoi(argument) , 6 );
	send_to_char( "Done.\n\r", ch );
	save_ship( ship );
	return;
       }
       if ( ship->class == LAND_SPEEDER )
       {
	ship->maxmissiles = URANGE( 0, atoi(argument) , 1 );
	ship->missiles = URANGE( 0, atoi(argument) , 1 );
	send_to_char( "Done.\n\r", ch );
	save_ship( ship );
	return;
       }
       if ( ship->class == WALKER )
       {
	send_to_char( "Walkers Can't Carry Missiles.\n\r", ch );
	return;
       }
    }


    if ( !str_cmp( arg2, "missiles" ) )
    {   
       if ( ship->class == FIGHTER_SHIP )
       {   
	ship->maxmissiles = URANGE( 0, atoi(argument) , 12 );
	ship->missiles = URANGE( 0, atoi(argument) , 12 );
	send_to_char( "Done.\n\r", ch );
	save_ship( ship );
	return;
       }
       if ( ship->class == MIDSIZE_SHIP )
       {
	ship->maxmissiles = URANGE( 0, atoi(argument) , 18 );
	ship->missiles = URANGE( 0, atoi(argument) , 18 );
	send_to_char( "Done.\n\r", ch );
	save_ship( ship );
	return;
       }
       if ( ship->class == FRIGATE_SHIP )
       {
	ship->maxmissiles = URANGE( 0, atoi(argument) , 32 );
	ship->missiles = URANGE( 0, atoi(argument) , 32 );
	send_to_char( "Done.\n\r", ch );
	save_ship( ship );
	return;
       }
       if ( ship->class == CAPITAL_SHIP )
       {
	ship->maxmissiles = URANGE( 0, atoi(argument) , 64 );
	ship->missiles = URANGE( 0, atoi(argument) , 64 );
	send_to_char( "Done.\n\r", ch );
	save_ship( ship );
	return;
       }
       if ( ship->class == SUPERCAPITAL_SHIP )
       {
	ship->maxmissiles = URANGE( 0, atoi(argument) , 128 );
	ship->missiles = URANGE( 0, atoi(argument) , 128 );
	send_to_char( "Done.\n\r", ch );
	save_ship( ship );
	return;
       }
       if ( ship->class == SHIP_PLATFORM )
       {
	ship->maxmissiles = URANGE( 0, atoi(argument) , 200 );
	ship->missiles = URANGE( 0, atoi(argument) , 200 );
	send_to_char( "Done.\n\r", ch );
	save_ship( ship );
	return;
       }
       if ( ship->class == MOBILE_SUIT )
       {
	ship->maxmissiles = URANGE( 0, atoi(argument) , 255 );
	ship->missiles = URANGE( 0, atoi(argument) , 255 );
	send_to_char( "Done.\n\r", ch );
	save_ship( ship );
	return;
       }
       if ( ship->class == CLOUD_CAR )
       {
	ship->maxmissiles = URANGE( 0, atoi(argument) , 6 );
	ship->missiles = URANGE( 0, atoi(argument) , 6 );
	send_to_char( "Done.\n\r", ch );
	save_ship( ship );
	return;
       }
       if ( ship->class == LAND_SPEEDER )
       {
	ship->maxmissiles = URANGE( 0, atoi(argument) , 1 );
	ship->missiles = URANGE( 0, atoi(argument) , 1 );
	send_to_char( "Done.\n\r", ch );
	save_ship( ship );
	return;
       }
       if ( ship->class == WALKER )
       {
	send_to_char( "Walkers Can't Carry Missiles.\n\r", ch );
	return;
       }
    }

    if ( !str_cmp( arg2, "torpedos" ) )
    {   
       if ( ship->class == FIGHTER_SHIP )
       {   
	ship->maxtorpedos = URANGE( 0, atoi(argument) , 6 );
	ship->torpedos = URANGE( 0, atoi(argument) , 6 );
	send_to_char( "Done.\n\r", ch );
	save_ship( ship );
	return;
       }
       if ( ship->class == MIDSIZE_SHIP )
       {
	ship->maxtorpedos = URANGE( 0, atoi(argument) , 12 );
	ship->torpedos = URANGE( 0, atoi(argument) , 12 );
	send_to_char( "Done.\n\r", ch );
	save_ship( ship );
	return;
       }
       if ( ship->class == FRIGATE_SHIP )
       {
	ship->maxtorpedos = URANGE( 0, atoi(argument) , 24 );
	ship->torpedos = URANGE( 0, atoi(argument) , 24 );
	send_to_char( "Done.\n\r", ch );
	save_ship( ship );
	return;
       }
       if ( ship->class == CAPITAL_SHIP )
       {
	ship->maxtorpedos = URANGE( 0, atoi(argument) , 48 );
	ship->torpedos = URANGE( 0, atoi(argument) , 48 );
	send_to_char( "Done.\n\r", ch );
	save_ship( ship );
	return;
       }
       if ( ship->class == SUPERCAPITAL_SHIP )
       {
	ship->maxtorpedos = URANGE( 0, atoi(argument) , 96 );
	ship->torpedos = URANGE( 0, atoi(argument) , 96 );
	send_to_char( "Done.\n\r", ch );
	save_ship( ship );
	return;
       }
       if ( ship->class == SHIP_PLATFORM )
       {
	ship->maxtorpedos = URANGE( 0, atoi(argument) , 200 );
	ship->torpedos = URANGE( 0, atoi(argument) , 200 );
	send_to_char( "Done.\n\r", ch );
	save_ship( ship );
	return;
       }
       if ( ship->class == MOBILE_SUIT )
       {
	ship->maxtorpedos = URANGE( 0, atoi(argument) , 255 );
	ship->torpedos = URANGE( 0, atoi(argument) , 255 );
	send_to_char( "Done.\n\r", ch );
	save_ship( ship );
	return;
       }
       if ( ship->class == CLOUD_CAR )
       {
	send_to_char( "Cloud Cars Can't Carry Torpedos.\n\r", ch );
	return;
       }
       if ( ship->class == LAND_SPEEDER )
       {
	send_to_char( "Speeders Can't Carry Torpedos.\n\r", ch );
	save_ship( ship );
	return;
       }
       if ( ship->class == WALKER )
       {
	send_to_char( "Walkers Can't Carry Torpedos.\n\r", ch );
	return;
       }
    }
    
    if ( !str_cmp( arg2, "rockets" ) )
    {   
       if ( ship->class == FIGHTER_SHIP )
       {   
	ship->maxrockets = URANGE( 0, atoi(argument) , 1 );
	ship->rockets = URANGE( 0, atoi(argument) , 1 );
	send_to_char( "Done.\n\r", ch );
	save_ship( ship );
	return;
       }
       if ( ship->class == MIDSIZE_SHIP )
       {
	ship->maxrockets = URANGE( 0, atoi(argument) , 3 );
	ship->rockets = URANGE( 0, atoi(argument) , 3 );
	send_to_char( "Done.\n\r", ch );
	save_ship( ship );
	return;
       }
       if ( ship->class == FRIGATE_SHIP )
       {
	ship->maxrockets = URANGE( 0, atoi(argument) , 9 );
	ship->rockets = URANGE( 0, atoi(argument) , 9 );
	send_to_char( "Done.\n\r", ch );
	save_ship( ship );
	return;
       }
       if ( ship->class == CAPITAL_SHIP )
       {
	ship->maxrockets = URANGE( 0, atoi(argument) , 27 );
	ship->rockets = URANGE( 0, atoi(argument) , 27 );
	send_to_char( "Done.\n\r", ch );
	save_ship( ship );
	return;
       }
       if ( ship->class == SUPERCAPITAL_SHIP )
       {
	ship->maxrockets = URANGE( 0, atoi(argument) , 81 );
	ship->rockets = URANGE( 0, atoi(argument) , 81 );
	send_to_char( "Done.\n\r", ch );
	save_ship( ship );
	return;
       }
       if ( ship->class == SHIP_PLATFORM )
       {
	ship->maxrockets = URANGE( 0, atoi(argument) , 100 );
	ship->rockets = URANGE( 0, atoi(argument) , 100 );
	send_to_char( "Done.\n\r", ch );
	save_ship( ship );
	return;
       }
       if ( ship->class == MOBILE_SUIT )
       {
	ship->maxrockets = URANGE( 0, atoi(argument) , 255 );
	ship->rockets = URANGE( 0, atoi(argument) , 255 );
	send_to_char( "Done.\n\r", ch );
	save_ship( ship );
	return;
       }
       if ( ship->class == CLOUD_CAR )
       {
	send_to_char( "Cloud Cars Can't Deploy Rockets.\n\r", ch );
       }
       if ( ship->class == LAND_SPEEDER )
       {
	send_to_char( "Speeder's Cant possibly hold a rocket.\n\r", ch );
       }
       if ( ship->class == WALKER )
       {
	send_to_char( "Ain't No way a Rocket's Fitting into a Walker.\n\r", ch );
       }
    }

    if ( !str_cmp( arg2, "speed" ) )
    { 
       if ( ship->class == FIGHTER_SHIP )
       {   
	ship->realspeed = URANGE( 0, atoi(argument) , 250 );
	send_to_char( "Done.\n\r", ch );
	save_ship( ship );
	return;
       }
       if ( ship->class == MIDSIZE_SHIP )
       {
	ship->realspeed = URANGE( 0, atoi(argument) , 150 );
	send_to_char( "Done.\n\r", ch );
	save_ship( ship );
	return;
       }
       if ( ship->class == FRIGATE_SHIP )
       {
	ship->realspeed = URANGE( 0, atoi(argument) , 75 );
	send_to_char( "Done.\n\r", ch );
	save_ship( ship );
	return;
       }
       if ( ship->class == CAPITAL_SHIP )
       {
	ship->realspeed = URANGE( 0, atoi(argument) , 25 );
	send_to_char( "Done.\n\r", ch );
	save_ship( ship );
	return;
       }
       if ( ship->class == SUPERCAPITAL_SHIP )
       {
	ship->realspeed = URANGE( 0, atoi(argument) , 15 );
	send_to_char( "Done.\n\r", ch );
	save_ship( ship );
	return;
       }
       if ( ship->class == SHIP_PLATFORM )
       {
	ship->realspeed = URANGE( 0, atoi(argument) , 5 );
	send_to_char( "Done.\n\r", ch );
	save_ship( ship );
	return;
       }
       if ( ship->class == MOBILE_SUIT )
       {
	ship->realspeed = URANGE( 0, atoi(argument) , 255 );
	send_to_char( "Done.\n\r", ch );
	save_ship( ship );
	return;
       }
       if ( ship->class == CLOUD_CAR )
       {
	ship->realspeed = URANGE( 0, atoi(argument) , 75 );
	send_to_char( "Done.\n\r", ch );
	save_ship( ship );
	return;
       }
       if ( ship->class == LAND_SPEEDER )
       {
	ship->realspeed = URANGE( 0, atoi(argument) , 50 );
	send_to_char( "Done.\n\r", ch );
	save_ship( ship );
	return;
       }
       if ( ship->class == WALKER )
       {
	ship->realspeed = URANGE( 0, atoi(argument) , 5 );
	send_to_char( "Done.\n\r", ch );
	save_ship( ship );
	return;
       }
}
 
    if ( !str_cmp( arg2, "tractorbeam" ) )
    {   
       if ( ship->class == FIGHTER_SHIP )
       {   
	send_to_char( "It is impossible to put a TB on a fighter.\n\r", ch );
	return;
       }
       if ( ship->class == MIDSIZE_SHIP )
       {
	ship->tractorbeam = URANGE( 0, atoi(argument) , 5 );
	send_to_char( "Done.\n\r", ch );
	save_ship( ship );
	return;
       }
       if ( ship->class == FRIGATE_SHIP )
       {
	ship->tractorbeam = URANGE( 0, atoi(argument) , 25 );
	send_to_char( "Done.\n\r", ch );
	save_ship( ship );
	return;
       }
       if ( ship->class == CAPITAL_SHIP )
       {
	ship->tractorbeam = URANGE( 0, atoi(argument) , 100 );
	send_to_char( "Done.\n\r", ch );
	save_ship( ship );
	return;
       }
       if ( ship->class == SUPERCAPITAL_SHIP )
       {
	ship->tractorbeam = URANGE( 0, atoi(argument) , 150 );
	send_to_char( "Done.\n\r", ch );
	save_ship( ship );
	return;
       }
       if ( ship->class == SHIP_PLATFORM )
       {
   	ship->tractorbeam = URANGE( 0, atoi(argument) , 200 );
	send_to_char( "Done.\n\r", ch );
	save_ship( ship );
	return;
       }
       if ( ship->class == MOBILE_SUIT )
       {
	send_to_char( "Impossible.\n\r", ch );
	return;
       }
       if ( ship->class == CLOUD_CAR )
       {
	send_to_char( "Impossible.\n\r", ch );
	return;
       }
       if ( ship->class == LAND_SPEEDER )
       {
	send_to_char( "Impossible.\n\r", ch );
	return;
       }
       if ( ship->class == WALKER )
       {
	send_to_char( "Impossible.\n\r", ch );
	return;
       }
    }
 
    if ( !str_cmp( arg2, "hyperspeed" ) )
    {   
      if ( ship->class == FIGHTER_SHIP )
       {   
	ship->hyperspeed = URANGE( 0, atoi(argument) , 100 );
 	send_to_char( "Done.\n\r", ch );
	save_ship( ship );
	return;
       }
       if ( ship->class == MIDSIZE_SHIP )
       {
	ship->hyperspeed = URANGE( 0, atoi(argument) , 125 );
 	send_to_char( "Done.\n\r", ch );
	save_ship( ship );
	return;
       }
       if ( ship->class == FRIGATE_SHIP )
       {
	ship->hyperspeed = URANGE( 0, atoi(argument) , 75 );
 	send_to_char( "Done.\n\r", ch );
	save_ship( ship );
	return;
       }
       if ( ship->class == CAPITAL_SHIP )
       {
	ship->hyperspeed = URANGE( 0, atoi(argument) , 50 );
 	send_to_char( "Done.\n\r", ch );
	save_ship( ship );
	return;
       }
       if ( ship->class == SUPERCAPITAL_SHIP )
       {
	ship->hyperspeed = URANGE( 0, atoi(argument) , 50 );
 	send_to_char( "Done.\n\r", ch );
	save_ship( ship );
	return;
       }
       if ( ship->class == SHIP_PLATFORM )
       {
 	ship->hyperspeed = URANGE( 0, atoi(argument) , 5 );
 	send_to_char( "Done.\n\r", ch );
	save_ship( ship );
	return;
       }
       if ( ship->class == MOBILE_SUIT )
       {
 	ship->hyperspeed = URANGE( 0, atoi(argument) , 255 );
	send_to_char( "Done.\n\r", ch );
	save_ship( ship );
	return;
       }
       if ( ship->class == CLOUD_CAR )
       {
	send_to_char( "Not On Ground Vehicles.\n\r", ch );
	return;
       }
       if ( ship->class == LAND_SPEEDER )
       {
	send_to_char( "Not On Ground Vehicles.\n\r", ch );
	return;
       }
       if ( ship->class == WALKER )
       {
	send_to_char( "Not On Ground Vehicles.\n\r", ch );
	return;
       }
}
 
    if ( !str_cmp( arg2, "shield" ) || !str_cmp( arg2, "shields" ) )
    {   
       if ( ship->class == FIGHTER_SHIP )
       {   
	ship->maxshield = URANGE( 0, atoi(argument) , 500 );
	send_to_char( "Done.\n\r", ch );
	save_ship( ship );
	return;
       }
       if ( ship->class == MIDSIZE_SHIP )
       {
	ship->maxshield = URANGE( 0, atoi(argument) , 1000 );
	send_to_char( "Done.\n\r", ch );
	save_ship( ship );
	return;
       }
       if ( ship->class == FRIGATE_SHIP )
       {
	ship->maxshield = URANGE( 0, atoi(argument) , 2500 );
	send_to_char( "Done.\n\r", ch );
	save_ship( ship );
	return;
       }
       if ( ship->class == CAPITAL_SHIP )
       {
	ship->maxshield = URANGE( 0, atoi(argument) , 5000 );
	send_to_char( "Done.\n\r", ch );
	save_ship( ship );
	return;
       }
       if ( ship->class == SUPERCAPITAL_SHIP )
       {
	ship->maxshield = URANGE( 0, atoi(argument) , 7500 );
	send_to_char( "Done.\n\r", ch );
	save_ship( ship );
	return;
       }
       if ( ship->class == SHIP_PLATFORM )
       {
	ship->maxshield = URANGE( 0, atoi(argument) , 10000 );
	send_to_char( "Done.\n\r", ch );
	save_ship( ship );
	return;
       }
       if ( ship->class == MOBILE_SUIT )
       {
	ship->maxshield = URANGE( 0, atoi(argument) , 32000 );
	send_to_char( "Done.\n\r", ch );
	save_ship( ship );
	return;
       }
       if ( ship->class == CLOUD_CAR )
       {
	ship->maxshield = URANGE( 0, atoi(argument) , 50 );
	send_to_char( "Done.\n\r", ch );
	save_ship( ship );
	return;
       }
       if ( ship->class == LAND_SPEEDER )
       {
	send_to_char( "Not on Speeders.\n\r", ch );
	return;
       }
       if ( ship->class == WALKER )
       {
	ship->maxshield = URANGE( 0, atoi(argument) , 150 );
	send_to_char( "Done.\n\r", ch );
	save_ship( ship );
	return;
       }
    }
 
    if ( !str_cmp( arg2, "hull" ) )
    {   
       if ( ship->class == FIGHTER_SHIP )
       {   
	ship->hull = URANGE( 1, atoi(argument) , 1000 );
	ship->maxhull = URANGE( 1, atoi(argument) , 1000 );
	send_to_char( "Done.\n\r", ch );
	save_ship( ship );
	return;
       }
       if ( ship->class == MIDSIZE_SHIP )
       {
	ship->hull = URANGE( 1, atoi(argument) , 5000 );
	ship->maxhull = URANGE( 1, atoi(argument) , 5000 );
	send_to_char( "Done.\n\r", ch );
	save_ship( ship );
	return;
       }
       if ( ship->class == FRIGATE_SHIP )
       {
	ship->hull = URANGE( 1, atoi(argument) , 10000 );
	ship->maxhull = URANGE( 1, atoi(argument) , 10000 );
	send_to_char( "Done.\n\r", ch );
	save_ship( ship );
	return;
       }
       if ( ship->class == CAPITAL_SHIP )
       {
	ship->hull = URANGE( 1, atoi(argument) , 17500 );
	ship->maxhull = URANGE( 1, atoi(argument) , 17500 );
	send_to_char( "Done.\n\r", ch );
	save_ship( ship );
	return;
       }
       if ( ship->class == SUPERCAPITAL_SHIP )
       {
	ship->hull = URANGE( 1, atoi(argument) , 25000 );
	ship->maxhull = URANGE( 1, atoi(argument) , 25000 );
	send_to_char( "Done.\n\r", ch );
	save_ship( ship );
	return;
       }
       if ( ship->class == SHIP_PLATFORM )
       {
	ship->hull = URANGE( 1, atoi(argument) , 30000 );
	ship->maxhull = URANGE( 1, atoi(argument) , 30000 );
	send_to_char( "Done.\n\r", ch );
	save_ship( ship );
	return;
       }
       if ( ship->class == MOBILE_SUIT )
       {
	ship->hull = URANGE( 1, atoi(argument) , 32000 );
	ship->maxhull = URANGE( 1, atoi(argument) , 32000 );
	send_to_char( "Done.\n\r", ch );
	save_ship( ship );
	return;
       }
       if ( ship->class == CLOUD_CAR )
       {
	ship->hull = URANGE( 1, atoi(argument) , 250 );
	ship->maxhull = URANGE( 1, atoi(argument) , 250 );
	send_to_char( "Done.\n\r", ch );
	save_ship( ship );
	return;
       }
       if ( ship->class == LAND_SPEEDER )
       {
	ship->hull = URANGE( 1, atoi(argument) , 400 );
	ship->maxhull = URANGE( 1, atoi(argument) , 400 );
	send_to_char( "Done.\n\r", ch );
	save_ship( ship );
	return;
       }
       if ( ship->class == WALKER )
       {
	ship->hull = URANGE( 1, atoi(argument) , 5000 );
	ship->maxhull = URANGE( 1, atoi(argument) , 5000 );
	send_to_char( "Done.\n\r", ch );
	save_ship( ship );
	return;
       }
    }
 
    if ( !str_cmp( arg2, "energy" ) )
    {   
       if ( ship->class == FIGHTER_SHIP )
       {   
	ship->energy = URANGE( 1, atoi(argument) , 5000 );
	ship->maxenergy = URANGE( 1, atoi(argument) , 5000 );
	send_to_char( "Done.\n\r", ch );
	save_ship( ship );
	return;
       }
       if ( ship->class == MIDSIZE_SHIP )
       {
	ship->energy = URANGE( 1, atoi(argument) , 10000 );
	ship->maxenergy = URANGE( 1, atoi(argument) , 10000 );
	send_to_char( "Done.\n\r", ch );
	save_ship( ship );
	return;
       }
       if ( ship->class == FRIGATE_SHIP )
       {
	ship->energy = URANGE( 1, atoi(argument) , 25000 );
	ship->maxenergy = URANGE( 1, atoi(argument) , 25000 );
	send_to_char( "Done.\n\r", ch );
	save_ship( ship );
	return;
       }
       if ( ship->class == CAPITAL_SHIP )
       {
	ship->energy = URANGE( 1, atoi(argument) , 30000 );
	ship->maxenergy = URANGE( 1, atoi(argument) , 30000 );
	send_to_char( "Done.\n\r", ch );
	save_ship( ship );
	return;
       }
       if ( ship->class == SUPERCAPITAL_SHIP )
       {
	ship->energy = URANGE( 1, atoi(argument) , 32000 );
	ship->maxenergy = URANGE( 1, atoi(argument) , 32000 );
	send_to_char( "Done.\n\r", ch );
	save_ship( ship );
	return;
       }
       if ( ship->class == SHIP_PLATFORM )
       {
	ship->energy = URANGE( 1, atoi(argument) , 32000 );
	ship->maxenergy = URANGE( 1, atoi(argument) , 32000 );
	send_to_char( "Done.\n\r", ch );
	save_ship( ship );
	return;
       }
       if ( ship->class == MOBILE_SUIT )
       {
	ship->energy = URANGE( 1, atoi(argument) , 32000 );
	ship->maxenergy = URANGE( 1, atoi(argument) , 32000 );
	send_to_char( "Done.\n\r", ch );
	save_ship( ship );
	return;
       }
       if ( ship->class == CLOUD_CAR )
       {
	ship->energy = URANGE( 1, atoi(argument) , 5000 );
	ship->maxenergy = URANGE( 1, atoi(argument) , 5000 );
	send_to_char( "Done.\n\r", ch );
	save_ship( ship );
	return;
       }
       if ( ship->class == LAND_SPEEDER )
       {
	ship->energy = URANGE( 1, atoi(argument) , 1500 );
	ship->maxenergy = URANGE( 1, atoi(argument) , 1500 );
	send_to_char( "Done.\n\r", ch );
	save_ship( ship );
	return;
       }
       if ( ship->class == WALKER )
       {
	ship->energy = URANGE( 1, atoi(argument) , 7500 );
	ship->maxenergy = URANGE( 1, atoi(argument) , 7500 );
	send_to_char( "Done.\n\r", ch );
	save_ship( ship );
	return;
       }
    }
 
    if ( !str_cmp( arg2, "sensor" ) )
    {   
       if ( ship->class == FIGHTER_SHIP )
       {   
	ship->sensor = URANGE( 0, atoi(argument) , 50 );
	send_to_char( "Done.\n\r", ch );
	save_ship( ship );
	return;
       }
       if ( ship->class == MIDSIZE_SHIP )
       {
	ship->sensor = URANGE( 0, atoi(argument) , 100 );
	send_to_char( "Done.\n\r", ch );
	save_ship( ship );
	return;
       }
       if ( ship->class == FRIGATE_SHIP )
       {
	ship->sensor = URANGE( 0, atoi(argument) , 150 );
	send_to_char( "Done.\n\r", ch );
	save_ship( ship );
	return;
       }
       if ( ship->class == CAPITAL_SHIP )
       {
	ship->sensor = URANGE( 0, atoi(argument) , 175 );
	send_to_char( "Done.\n\r", ch );
	save_ship( ship );
	return;
       }
       if ( ship->class == SUPERCAPITAL_SHIP )
       {
	ship->sensor = URANGE( 0, atoi(argument) , 200 );
	send_to_char( "Done.\n\r", ch );
	save_ship( ship );
	return;
       }
       if ( ship->class == SHIP_PLATFORM )
       {
	ship->sensor = URANGE( 0, atoi(argument) , 255 );
	send_to_char( "Done.\n\r", ch );
	save_ship( ship );
	return;
       }
       if ( ship->class == MOBILE_SUIT )
       {
	ship->sensor = URANGE( 0, atoi(argument) , 255 );
	send_to_char( "Done.\n\r", ch );
	save_ship( ship );
	return;
       }
       if ( ship->class == CLOUD_CAR )
       {
	ship->sensor = URANGE( 0, atoi(argument) , 25 );
	send_to_char( "Done.\n\r", ch );
	save_ship( ship );
	return;
       }
       if ( ship->class == LAND_SPEEDER )
       {
	ship->sensor = URANGE( 0, atoi(argument) , 5 );
	send_to_char( "Done.\n\r", ch );
	save_ship( ship );
	return;
       }
       if ( ship->class == WALKER )
       {
	ship->sensor = URANGE( 0, atoi(argument) , 15 );
	send_to_char( "Done.\n\r", ch );
	save_ship( ship );
	return;
       }
    }
  
    if ( !str_cmp( arg2, "astroarray" ) )
    {   
//	ship->astro_array = URANGE( 0, atoi(argument) , 255 );
	send_to_char( "This Variable Not Used Yet.\n\r", ch );
	return;
    }
    
    if ( !str_cmp( arg2, "comm" ) )
    {   
//	ship->comm = URANGE( 0, atoi(argument) , 255 );
	send_to_char( "This Variable Not Used Yet.\n\r", ch );
	return;
    }
    
    if ( !str_cmp( arg2, "chaff" ) )
    {   
       if ( ship->class == FIGHTER_SHIP )
       {   
	ship->chaff = URANGE( 0, atoi(argument) , 5 );
	ship->maxchaff = URANGE( 0, atoi(argument) , 5 );
		send_to_char( "Done.\n\r", ch );
	save_ship( ship );
	return;
       }
       if ( ship->class == MIDSIZE_SHIP )
       {
	ship->chaff = URANGE( 0, atoi(argument) , 15 );
	ship->maxchaff = URANGE( 0, atoi(argument) , 15 );
	send_to_char( "Done.\n\r", ch );
	save_ship( ship );
	return;
       }
       if ( ship->class == FRIGATE_SHIP )
       {
	ship->chaff = URANGE( 0, atoi(argument) , 25 );
	ship->maxchaff = URANGE( 0, atoi(argument) , 25 );
	send_to_char( "Done.\n\r", ch );
	save_ship( ship );
	return;
       }
       if ( ship->class == CAPITAL_SHIP )
       {
	ship->chaff = URANGE( 0, atoi(argument) , 50 );
	ship->maxchaff = URANGE( 0, atoi(argument) , 50 );
	send_to_char( "Done.\n\r", ch );
	save_ship( ship );
	return;
       }
       if ( ship->class == SUPERCAPITAL_SHIP )
       {
	ship->chaff = URANGE( 0, atoi(argument) , 50 );
	ship->maxchaff = URANGE( 0, atoi(argument) , 50 );
	send_to_char( "Done.\n\r", ch );
	save_ship( ship );
	return;
       }
       if ( ship->class == SHIP_PLATFORM )
       {
	ship->chaff = URANGE( 0, atoi(argument) , 75 );
	ship->maxchaff = URANGE( 0, atoi(argument) , 75 );
	send_to_char( "Done.\n\r", ch );
	save_ship( ship );
	return;
       }
       if ( ship->class == MOBILE_SUIT )
       {
	ship->chaff = URANGE( 0, atoi(argument) , 255 );
	ship->maxchaff = URANGE( 0, atoi(argument) , 255 );
	send_to_char( "Done.\n\r", ch );
	save_ship( ship );
	return;
       }
       if ( ship->class == CLOUD_CAR )
       {
	ship->chaff = URANGE( 0, atoi(argument) , 5 );
	ship->maxchaff = URANGE( 0, atoi(argument) , 5 );
	send_to_char( "Done.\n\r", ch );
	save_ship( ship );
	return;
       }
       if ( ship->class == LAND_SPEEDER )
       {
	send_to_char( "Not on a Speeder.\n\r", ch );
	return;
       }
       if ( ship->class == WALKER )
       {
	send_to_char( "Not on a Walker.\n\r", ch );
	return;
       }
}
/* Installed to setship flags, primarily for simulators. -Arcturus */    
 	if ( !str_cmp( arg2, "flags" ) )
	{
            if ( !argument || argument[0] == '\0' )
            {
            send_to_char( "Usage: setship <ship> flags <flag> [flag]...\n\r", ch );
            send_to_char( "cloaked overdrive afterburner sablaser sabions sabengine sabturret1 sabturret2\n\r", ch);
            send_to_char( "sablauncher shieldrlaser shieldrengine enginerlaser enginershield laserrengine\n\r", ch);
            send_to_char( "laserrshield duallaser trilaser quadlaser dualion triion quadion simulator\n\r", ch );
            send_to_char( "dualmissile dualtorpedo dualrocket sabrlauncher sabtlauncher\n\r", ch);
            return;
            }
        while ( argument[0] != '\0' )
        {
            argument = one_argument( argument, arg3 );
            tempnum = get_shipflag( arg3 );

            if ( tempnum < 0 || tempnum > 31 )
            {
                ch_printf( ch, "Unknown flag: %s\n\r", arg3 );
                return;
            }
            TOGGLE_BIT( ship->flags, 1 << tempnum );
        }
        save_ship( ship );
        return;
	}
    if( !str_cmp( arg2, "cloak" ))
    {
       if(ship->cloak == 0)
       {
         ship->cloak = 1;
         send_to_char("Cloaking unit added.\n\r", ch);
         return;
       }
       else
       {
         ship->cloak = 0;
         send_to_char("Cloaking unit removed.\n\r", ch);
         return;
       }
    }
    if(!str_cmp(arg2, "mlaunchers") )
    {
        ship->mlaunchers =  atoi(argument);
        send_to_char( "Done.\n\r", ch );
        save_ship( ship );
        return;

    }
    if(!str_cmp(arg2, "tlaunchers"))
    {
        ship->tlaunchers =  atoi(argument);
        send_to_char( "Done.\n\r", ch );
        save_ship( ship );
        return;
    }
    if(!str_cmp(arg2, "rlaunchers"))
    {
        ship->rlaunchers =  atoi(argument);
        send_to_char( "Done.\n\r", ch );
        save_ship( ship );
        return;
    }
    if ( !str_cmp( arg2, "autocannon" ) )
    {
      if ( get_trust( ch ) < (MAX_LEVEL -1))
      {
        send_to_char( "At current, Only Head Imms can modify autocannon settings.\n\r", ch );
        return;
      }
      if ( ship->autocannon == 0 )
      {
        ship->autocannon = 1;
        send_to_char( "Autocannon Added!\n\r", ch );
        save_ship( ship );
        return;
      }
      if ( ship->autocannon == 1 )
      {
        ship->autocannon = 0;
        send_to_char( "Autocannon Removed!\n\r", ch );
        save_ship( ship );
        return;
      }
    }

    if ( !str_cmp( arg2, "autodamage" ) )
    {
      if ( get_trust( ch ) < (MAX_LEVEL -1))
      {
        send_to_char( "At current, Only Head Imms can modify autocannon settings.\n\r", ch );
        return;
      }
	ship->autodamage = URANGE( 0, atoi(argument) , 255 );
      send_to_char( "Done.\n\r", ch );
      save_ship( ship );
    }

    if ( !str_cmp( arg2, "autoammo" ) )
    {
      if ( get_trust( ch ) < (MAX_LEVEL -1))
      {
        send_to_char( "At current, Only Head Imms can modify autocannon settings.\n\r", ch );
        return;
      }
	ship->autoammomax = URANGE( 0, atoi(argument) , 255 );
	ship->autoammo = URANGE( 0, atoi(argument) , 255 );
      send_to_char( "Done.\n\r", ch );
      save_ship( ship );
    }

    if ( !str_cmp( arg2, "interdict" ) )
    {
      if ( get_trust( ch ) < (MAX_LEVEL -1))
      {
        send_to_char( "At current, Only Head Imms can modify Interdictor Fields\n\r", ch );
        return;
      }
      if ( ship->interdict == 0 )
      {
        ship->interdict = 1;
        send_to_char( "Interdictor Generators Added!\n\r", ch );
        save_ship( ship );
        return;
      }
      if ( ship->interdict == 1 )
      {
        ship->interdict = 0;
        send_to_char( "Interdictor Generators Removed!\n\r", ch );
        save_ship( ship );
        return;
      }
    }


    do_setship( ch, "" );
    return;
}

void do_showship( CHAR_DATA *ch, char *argument )
{   
    SHIP_DATA *ship;

    if ( IS_NPC( ch ) )
    {
	send_to_char( "Huh?\n\r", ch );
	return;
    }

    if ( argument[0] == '\0' )
    {
	send_to_char( "Usage: showship <ship>\n\r", ch );
	return;
    }

    ship = get_ship( argument );


    if ( !ship )
    {
	send_to_char( "No such ship.\n\r", ch );
	return;
    }
    set_char_color( AT_YELLOW, ch );
    ch_printf( ch, "%s %s : %s\n\rFilename: %s\n\r",
			ship->type == SHIP_REPUBLIC ? "New Republic" :
		       (ship->type == SHIP_IMPERIAL ? "Imperial" : 
		       (ship->type == SHIP_VONG ? "Yuuzhan Vong" : 
		       (ship->type == SHIP_CIVILIAN ? "Civilian" : "Mob" ) ) ),
		        ship->class == FIGHTER_SHIP ? "Starfighter" :
		       (ship->class == MIDSIZE_SHIP ? "Midship" : 
		       (ship->class == CAPITAL_SHIP ? "Capital Ship" : 
		       (ship->class == SHIP_PLATFORM ? "Platform" : 
		       (ship->class == CLOUD_CAR ? "Cloudcar" : 
		       (ship->class == FRIGATE_SHIP ? "Frigate" : 
		       (ship->class == LAND_SPEEDER ? "Speeder" : 
		       (ship->class == SUPERCAPITAL_SHIP ? "SuperCapital" 
: 
		       (ship->class == MOBILE_SUIT ? "Coralskipper" : 
		       (ship->class == WALKER ? "Walker" : "Unknown" ) ) ) ) ) ) ) ) ), 
    			ship->name,
    			ship->filename);
    ch_printf( ch, "Home: %s   Description: %s\n\rOwner: %s   Pilot: %s   Copilot: %s\n\r",
    			ship->home,  ship->description,
    			ship->owner, ship->pilot,  ship->copilot );
    ch_printf( ch, "Firstroom: %d   Lastroom: %d",
    			ship->firstroom,
    			ship->lastroom);
    ch_printf( ch, "Cockpit: %d   Entrance: %d   Hangar: %d   Dock: %d  Engineroom: %d\n\r",
    			ship->cockpit,
    			ship->entrance,
    			ship->hanger,
			ship->dock,
    			ship->engineroom);
    ch_printf( ch, "Pilotseat: %d   Coseat: %d   Navseat: %d  Gunseat: %d\n\r",
    			ship->pilotseat,
    			ship->coseat,
    			ship->navseat,
    			ship->gunseat);
    ch_printf( ch, "Location: %d   Lastdoc: %d   Shipyard: %d\n\r",
    			ship->location,
    			ship->lastdoc,
    			ship->shipyard);
    ch_printf( ch, "Tractor Beam: %d   Comm: %d   Sensor: %d   Astro Array: %d\n\r",
    			ship->tractorbeam,
    			ship->comm,
    			ship->sensor,
    			ship->astro_array);
    ch_printf( ch, "Lasers: %d  Ions: %d  Power: %d  Laser Condition: %s Ion Condition: %s\n\r",
    			ship->lasers,
    			ship->ions,
    			ship->laserdamage,
                  ship->statet0 == LASER_DAMAGED ? "Damaged" : "Good",
                  ship->ionstate == LASER_DAMAGED ? "Damaged" : "Good");		
    ch_printf( ch, "Turret One: %d  Condition: %s\n\r",
    			ship->turret1,
    			ship->statet1 == LASER_DAMAGED ? "Damaged" : "Good");		
    ch_printf( ch, "Turret Two: %d  Condition: %s\n\r",
    			ship->turret2,
    			ship->statet2 == LASER_DAMAGED ? "Damaged" : "Good");		
    ch_printf( ch, "Missiles: %d/%d  Torpedos: %d/%d  Rockets: %d/%d\n\r",
       			ship->missiles,
    			ship->maxmissiles,
    			ship->torpedos,
    			ship->maxtorpedos,
    			ship->rockets,
    			ship->maxrockets);
    ch_printf(ch, "Mlaunchers: %d MCondition: %s Tlaunchers: %d TCondition: %s Rlaunchers: %d RCondition: %s\n\r",
    			ship->mlaunchers, ship->missilestate == MISSILE_DAMAGED ? "Damaged" : "Good",
                        ship->tlaunchers, ship->torpedostate == MISSILE_DAMAGED ? "Damaged" : "Good",
                        ship->rlaunchers, ship->rocketstate  == MISSILE_DAMAGED ? "Damaged" : "Good");		
    if (ship->overdrive == 1)
    {
      send_to_char( "SLAM Overdrive Node Installed!!!\n\r", ch);
    }
    if ( ship->autocannon == 1)
    {
    ch_printf( ch, "Autocannon Installed - %d/%d ammo, %d intensity\n\r",
			ship->autoammomax,
			ship->autoammo,
			ship->autodamage );
    }
    ch_printf( ch, "Hull: %d/%d  Ship Condition: %s\n\r",
                        ship->hull,
    		        ship->maxhull,	
    			ship->shipstate == SHIP_DISABLED ? "Disabled" : "Running");
    		
    ch_printf( ch, "Shields: %d/%d   Energy(fuel): %d/%d   Chaff: %d/%d\n\r",
                        ship->shield,
    		        ship->maxshield,
    		        ship->energy,
    		        ship->maxenergy,
    		        ship->chaff,
    		        ship->maxchaff);
    ch_printf( ch, "Modules: %d/%d\n\r", ship->modules, ship->maxmodules);
    ch_printf( ch, "Cargo: %d/%d, Cargo Type: %s \n\r",
                        ship->cargo,
                        ship->maxcargo,
                        cargo_names[ship->cargotype]);
    ch_printf( ch, "Current Coordinates: %.0f %.0f %.0f\n\r",
                        ship->vx, ship->vy, ship->vz );
    ch_printf( ch, "Current Heading: %.0f %.0f %.0f\n\r",
                        ship->hx, ship->hy, ship->hz );
    ch_printf( ch, "Speed: %d/%d   Hyperspeed: %d  Manueverability: %d\n\r",
                        ship->currspeed, ship->realspeed, ship->hyperspeed , ship->manuever );                    
    if ( ship->interdict == 1)
    {
      send_to_char( "Interdiction System Installed!!!\n\r", ch );
    }
    if (IS_SET(ship->flags, SHIPFLAG_SIMULATOR) )
      send_to_char("This craft is a simulator.\n\r", ch);
    if( ship->cloak == 1 )
      send_to_char("This craft has a cloaking device.\n\r", ch);
    return;
}

void do_makeship( CHAR_DATA *ch, char *argument )
{
    SHIP_DATA *ship;
    char arg[MAX_INPUT_LENGTH];
    
    argument = one_argument( argument, arg );
    
    if ( !argument || argument[0] == '\0' )
    {
	send_to_char( "Usage: makeship <filename> <ship name>\n\r", ch );
	return;
    }

    CREATE( ship, SHIP_DATA, 1 );
    LINK( ship, first_ship, last_ship, next, prev );

    ship->name		= STRALLOC( argument );
    ship->description	= STRALLOC( "" );
    ship->owner 	= STRALLOC( "" );
    ship->copilot       = STRALLOC( "" );
    ship->pilot         = STRALLOC( "" );
    ship->home          = STRALLOC( "" );
    ship->type          = SHIP_CIVILIAN;
    ship->maxcargo=0;
    ship->cargo=0;
    ship->cargotype=0;
    ship->starsystem = NULL;
    ship->energy = ship->maxenergy;
    ship->hull = ship->maxhull;
    ship->in_room=NULL;
    ship->next_in_room=NULL;
    ship->prev_in_room=NULL;
    ship->currjump=NULL;
    ship->target0=NULL;
    ship->target1=NULL;
    ship->target2=NULL;
    ship->maxmodules=10;
    ship->modules=0;
    ship->sdestnum=-1;
    ship->filename = str_dup( arg );
    save_ship( ship );
    write_ship_list( );
	
}

void do_copyship( CHAR_DATA *ch, char *argument )
{
    SHIP_DATA *ship;
    SHIP_DATA *old;
    char arg[MAX_INPUT_LENGTH];
    char arg2[MAX_INPUT_LENGTH];
    
    argument = one_argument( argument, arg );
    argument = one_argument( argument, arg2 );
    
    if ( !argument || argument[0] == '\0' )
    {
	send_to_char( "Usage: copyship <oldshipname> <filename> <newshipname>\n\r", ch );
	return;
    }

    old = get_ship ( arg );
    
    if (!old)
    {
	send_to_char( "Thats not a ship!\n\r", ch );
	return;
    }

    CREATE( ship, SHIP_DATA, 1 );
    LINK( ship, first_ship, last_ship, next, prev );

    ship->name		= STRALLOC( argument );
    ship->description	= STRALLOC( "" );
    ship->owner 	= STRALLOC( "" );
    ship->copilot       = STRALLOC( "" );
    ship->pilot         = STRALLOC( "" );
    ship->home          = STRALLOC( "" );
    ship->type          = old->type;
    ship->class         = old->class;
    ship->lasers        = old->lasers  ;
    ship->maxmissiles   = old->maxmissiles  ;
    ship->maxrockets        = old->maxrockets  ;
    ship->maxtorpedos        = old->maxtorpedos  ;
    ship->maxshield        = old->maxshield  ;
    ship->maxhull        = old->maxhull  ;
    ship->maxenergy        = old->maxenergy  ;
    ship->hyperspeed        = old->hyperspeed  ;
    ship->maxchaff        = old->maxchaff  ;
    ship->realspeed        = old->realspeed  ;
    ship->manuever        = old->manuever  ;
    ship->in_room=NULL;
    ship->next_in_room=NULL;
    ship->prev_in_room=NULL;
    ship->currjump=NULL;
    ship->target0=NULL;
    ship->target1=NULL;
    ship->target2=NULL;

    ship->filename         = str_dup(arg2);
    save_ship( ship );
    write_ship_list();
}

void do_ships( CHAR_DATA *ch, char *argument )
{
    SHIP_DATA *ship;
    int count;
    
    if ( !IS_NPC(ch) )
    {
      count = 0;
      send_to_char( "&YThe following ships are owned by you or by your organization:\n\r", ch );
      send_to_char( "\n\r&WShip                               Owner\n\r",ch);
      for ( ship = first_ship; ship; ship = ship->next )
      {   
        if ( str_cmp(ship->owner, ch->name) )
        {
           if ( !ch->pcdata || !ch->pcdata->clan || str_cmp(ship->owner,ch->pcdata->clan->name) || ship->class > CLOUD_CAR )
               continue;
        }
         
        if (ship->type == MOB_SHIP)
           continue;
        else if (ship->type == SHIP_REPUBLIC)
           set_char_color( AT_BLOOD, ch );
        else if (ship->type == SHIP_VONG)
           set_char_color( AT_CYAN, ch );
        else if (ship->type == SHIP_IMPERIAL)
           set_char_color( AT_DGREEN, ch );
        else
          set_char_color( AT_BLUE, ch );
        
        if  ( ship->in_room )       
          ch_printf( ch, "%s (%s) - %s\n\r", ship->name, ship->owner, ship->in_room->name );
        else 
          ch_printf( ch, "%s (%s)\n\r", ship->name, ship->owner );
        
        count++;
      }

      if ( !count )
      {
        send_to_char( "There are no ships owned by you.\n\r", ch );
      }
    
    }

    
    count =0;
    send_to_char( "&Y\n\rThe following ships are docked here:\n\r", ch );
    
    send_to_char( "\n\r&WShip                               Owner          Cost/Rent\n\r", ch );
    for ( ship = first_ship; ship; ship = ship->next )
    {   
        if ( ship->location != ch->in_room->vnum || ship->class > 
CLOUD_CAR )
               continue;

        if (ship->type == MOB_SHIP)
           continue;
        else if (ship->type == SHIP_REPUBLIC)
           set_char_color( AT_BLOOD, ch );
        else if (ship->type == SHIP_VONG)
           set_char_color( AT_CYAN, ch );
        else if (ship->type == SHIP_IMPERIAL)
           set_char_color( AT_DGREEN, ch );
        else
          set_char_color( AT_BLUE, ch );
        
        ch_printf( ch, "%-35s %-15s", ship->name, ship->owner );
        if (ship->type == MOB_SHIP || ship->class == SHIP_PLATFORM )
        {
          ch_printf( ch, "\n\r");
          continue;
        }
        if ( !str_cmp(ship->owner, "Public") )
        { 
          ch_printf( ch, "%ld to rent.\n\r", 500+250*ship->class ); 
        }
        else if ( str_cmp(ship->owner, "") )
          ch_printf( ch, "%s", "\n\r" );
        else
           ch_printf( ch, "%ld to buy.\n\r", get_ship_value(ship) ); 
        
        count++;
    }

    if ( !count )
    {
        send_to_char( "There are no ships docked here.\n\r", ch );
    }
}

void do_speeders( CHAR_DATA *ch, char *argument )
{
    SHIP_DATA *ship;
    int count;

    if ( !IS_NPC(ch) )
    {
      count = 0;
      send_to_char( "&YThe following are owned by you or by your organization:\n\r", ch );
      send_to_char( "\n\r&WVehicle                            Owner\n\r",ch);
      for ( ship = first_ship; ship; ship = ship->next )
      {   
        if ( str_cmp(ship->owner, ch->name) )
        {
           if ( !ch->pcdata || !ch->pcdata->clan || str_cmp(ship->owner,ch->pcdata->clan->name) || ship->class <= SHIP_PLATFORM )
               continue;
        }
        if ( ship->location != ch->in_room->vnum || ship->class <= SHIP_PLATFORM)
               continue;
               
        if (ship->type == MOB_SHIP)
           continue;
        else if (ship->type == SHIP_REPUBLIC)
           set_char_color( AT_BLOOD, ch );
        else if (ship->type == SHIP_VONG)
           set_char_color( AT_CYAN, ch );
        else if (ship->type == SHIP_IMPERIAL)
           set_char_color( AT_DGREEN, ch );
        else
          set_char_color( AT_BLUE, ch );
                
        ch_printf( ch, "%-35s %-15s\n\r", ship->name, ship->owner );
        
        count++;
      }

      if ( !count )
      {
        send_to_char( "There are no land or air vehicles owned by you.\n\r", ch );
      }
    
    }

    
    count =0;
    send_to_char( "&Y\n\rThe following vehicles are parked here:\n\r", ch );
    
    send_to_char( "\n\r&WVehicle                            Owner          Cost/Rent\n\r", ch );
    for ( ship = first_ship; ship; ship = ship->next )
    {   
        if ( ship->location != ch->in_room->vnum || ship->class <= SHIP_PLATFORM)
               continue;
               
        if (ship->type == MOB_SHIP)
           continue;
        else if (ship->type == SHIP_REPUBLIC)
           set_char_color( AT_BLOOD, ch );
        else if (ship->type == SHIP_VONG)
           set_char_color( AT_CYAN, ch );
        else if (ship->type == SHIP_IMPERIAL)
           set_char_color( AT_DGREEN, ch );
        else
          set_char_color( AT_BLUE, ch );
        

        ch_printf( ch, "%-35s %-15s", ship->name, ship->owner );
        
        if ( !str_cmp(ship->owner, "Public") )
        { 
          ch_printf( ch, "%ld to rent.\n\r", 500+250*ship->class ); 
        }
        else if ( str_cmp(ship->owner, "") )
          ch_printf( ch, "%s", "\n\r" );
        else
           ch_printf( ch, "%ld to buy.\n\r", get_ship_value(ship) ); 
        
        count++;
    }

    if ( !count )
    {
        send_to_char( "There are no sea air or land vehicles here.\n\r", ch );
    }
}

void do_allspeeders( CHAR_DATA *ch, char *argument )
{
    SHIP_DATA *ship;
    int count = 0;

      count = 0;
      send_to_char( "&Y\n\rThe following sea/land/air vehicles are currently formed:\n\r", ch );
    
      send_to_char( "\n\r&WVehicle                            Owner\n\r", ch );
      for ( ship = first_ship; ship; ship = ship->next )
      {   
        if ( ship->class <= CLOUD_CAR ) 
           continue; 
      
        if (ship->type == MOB_SHIP)
           continue;
        else if (ship->type == SHIP_REPUBLIC)
           set_char_color( AT_BLOOD, ch );
        else if (ship->type == SHIP_VONG)
           set_char_color( AT_CYAN, ch );
        else if (ship->type == SHIP_IMPERIAL)
           set_char_color( AT_DGREEN, ch );
        else
          set_char_color( AT_BLUE, ch );
        
        
        ch_printf( ch, "%-35s %-15s ", ship->name, ship->owner );

        if ( !str_cmp(ship->owner, "Public") )
        { 
          ch_printf( ch, "%ld to rent.\n\r", 500+250*ship->class ); 
        }
        else if ( str_cmp(ship->owner, "") )
          ch_printf( ch, "%s", "\n\r" );
        else
           ch_printf( ch, "%ld to buy.\n\r", get_ship_value(ship) ); 
        
        count++;
      }
    
      if ( !count )
      {
        send_to_char( "There are none currently formed.\n\r", ch );
	return;
      }
    
}

void do_allships( CHAR_DATA *ch, char *argument )
{
    SHIP_DATA *ship;
    int count = 0;

      count = 0;
      send_to_char( "&Y\n\rThe following ships are currently formed:\n\r", ch );
    
      send_to_char( "\n\r&WShip                               Owner\n\r", ch );
      
      if ( IS_IMMORTAL( ch ) )
        for ( ship = first_ship; ship; ship = ship->next )
           if (ship->type == MOB_SHIP)
              ch_printf( ch, "&w%-35s %-15s\n\r", ship->name, ship->owner );
            
      for ( ship = first_ship; ship; ship = ship->next )
      {   
        if ( ship->class > MOBILE_SUIT ) 
           continue; 
      
        if (ship->type == MOB_SHIP)
           continue;
        else if (ship->type == SHIP_REPUBLIC)
           set_char_color( AT_BLOOD, ch );
        else if (ship->type == SHIP_VONG)
           set_char_color( AT_CYAN, ch );
        else if (ship->type == SHIP_IMPERIAL)
           set_char_color( AT_DGREEN, ch );
        else
          set_char_color( AT_BLUE, ch );
        
        ch_printf( ch, "%-35s %-15s ", ship->name, ship->owner );
        if (ship->type == MOB_SHIP || ship->class == SHIP_PLATFORM )
        {
          ch_printf( ch, "\n\r");
          continue;
        }
        if ( !str_cmp(ship->owner, "Public") )
        { 
          ch_printf( ch, "%ld to rent.\n\r", 500+250*ship->class ); 
        }
        else if ( str_cmp(ship->owner, "") )
          ch_printf( ch, "%s", "\n\r" );
        else
           ch_printf( ch, "%ld to buy.\n\r", get_ship_value(ship) ); 
        
        count++;
      }
    
      if ( count==0 )
      {
        send_to_char( "There are no ships currently formed.\n\r", ch );
	return;
      }
    

}

void do_freeships( CHAR_DATA *ch, char *argument )
{
    SHIP_DATA *ship;
    int count = 0;

      count = 0;
      send_to_char( "&Y\n\rThe following ships are currently availiable for purchace:\n\r", ch );
    
      send_to_char( "\n\r&WShip                                                  Class          Price\n\r", ch );
      
      for ( ship = first_ship; ship; ship = ship->next )
      {   
        if ( ship->class >= MOBILE_SUIT ) 
           continue; 
	if (ship->location == 5)
	   continue;      
	if (ship->location == 45)
	   continue;      
	if (ship->location == 133)
	   continue;      

        if (ship->type == MOB_SHIP)
           continue;
        else if (ship->type == SHIP_REPUBLIC)
           set_char_color( AT_BLOOD, ch );
        else if (ship->type == SHIP_VONG)
           set_char_color( AT_CYAN, ch );
        else if (ship->type == SHIP_IMPERIAL)
           set_char_color( AT_DGREEN, ch );
        else
          set_char_color( AT_BLUE, ch );
        
        if ( !str_cmp(ship->owner, "") )
           ch_printf( ch, "%-53s %-15s %ld\n\r", ship->name,
		        ship->class == FIGHTER_SHIP ? "(Starfighter)" :
		       (ship->class == MIDSIZE_SHIP ? "(Midtarget)" : 
		       (ship->class == CAPITAL_SHIP ? "(Capital Ship)" :
		       (ship->class == SHIP_PLATFORM ? "(Platform)" : 
		       (ship->class == CLOUD_CAR ? "(Cloudcar)" : 
		       (ship->class == FRIGATE_SHIP ? "(Frigate)" : 
		       (ship->class == LAND_SPEEDER ? "(Speeder)" : 
		       (ship->class == SUPERCAPITAL_SHIP ? "(SuperCapital)" : 
		       (ship->class == MOBILE_SUIT ? "(Coralskipper)" : 
		       (ship->class == WALKER ? "(Walker)" : "(Unknown)" ))))))))), 
		       get_ship_value(ship));
        count++;
      }
    
      if ( count==0 )
      {
        send_to_char( "There are no ships currently free.\n\r", ch );
	return;
      }
    
}

void do_availships( CHAR_DATA *ch, char *argument )
{
    SHIP_DATA *ship;
    int count = 0;

      count = 0;
      send_to_char( "&Y\n\rThe following ships are currently for sale:\n\r", ch );
    
      send_to_char( "\n\r&WShip                               Price\n\r", ch );
      
      for ( ship = first_ship; ship; ship = ship->next )
      {   
        if ( ship->class > MOBILE_SUIT )
           continue; 
      
        if (ship->type == MOB_SHIP)
           continue;
        else if (ship->type == SHIP_REPUBLIC)
           set_char_color( AT_BLOOD, ch );
        else if (ship->type == SHIP_VONG)
           set_char_color( AT_CYAN, ch );
        else if (ship->type == SHIP_IMPERIAL)
           set_char_color( AT_DGREEN, ch );
        else
          set_char_color( AT_BLUE, ch );

        if (str_cmp(ship->owner, "") || (!str_cmp(ship->owner, "Public")))
        {
          ch_printf( ch, "%-35s", ship->name );
        }
        if (ship->type == MOB_SHIP || ship->class == SHIP_PLATFORM )
        {
          ch_printf( ch, "\n\r");
          continue;
        }
        if ( !str_cmp(ship->owner, "Public") )
        { 
          ch_printf( ch, "%ld to rent.\n\r", get_ship_value(ship)/100 ); 
        }
        else if ( str_cmp(ship->owner, "") )
          ch_printf( ch, "%s", "\n\r" );
        else
           ch_printf( ch, "%ld to buy.\n\r", get_ship_value(ship) ); 
        
        count++;
      }
    
      if ( !count )
      {
        send_to_char( "There are no ships currently for sale.\n\r", ch );
	return;
      }
    
}




void ship_to_starsystem( SHIP_DATA *ship , SPACE_DATA *starsystem )
{
     if ( starsystem == NULL )
        return;
     
     if ( ship == NULL )
        return;
     
     if ( starsystem->first_ship == NULL )
        starsystem->first_ship = ship;
     
     if ( starsystem->last_ship )
     {
         starsystem->last_ship->next_in_starsystem = ship;
         ship->prev_in_starsystem = starsystem->last_ship;
     }
     
     starsystem->last_ship = ship;
     
     ship->starsystem = starsystem;
        
}

void new_missile( SHIP_DATA *ship , SHIP_DATA *target , CHAR_DATA *ch , int missiletype )
{
     SPACE_DATA *starsystem;
     MISSILE_DATA *missile;

     if ( ship  == NULL )
        return;

     if ( target  == NULL )
        return;

     if ( ( starsystem = ship->starsystem ) == NULL )
        return;
          
     CREATE( missile, MISSILE_DATA, 1 );
     LINK( missile, first_missile, last_missile, next, prev );
     
     missile->target = target; 
     missile->fired_from = ship;
     if ( ch )
     {
        missile->fired_by = STRALLOC( ch->name );
        if ( ch->subclass == SUBCLASS_WFOCUS )
           missiletype -= 1;
     }
     else 
        missile->fired_by = STRALLOC( "" );
     missile->missiletype = missiletype;
     missile->age =0;
     if ( missile->missiletype == HEAVY_BOMB )
       missile->speed = 100;
     else if ( missile->missiletype == PROTON_TORPEDO ) 
       missile->speed = 400;
     else if ( missile->missiletype == CONCUSSION_MISSILE ) 
       missile->speed = 500;
     else 
       missile->speed = 150;
     if(ch)
     {
        if ( ch->subclass == SUBCLASS_WFOCUS )
           missile->missiletype += 1;
     }

     missile->mx = ship->vx;
     missile->my = ship->vy;
     missile->mz = ship->vz;
            
     if ( starsystem->first_missile == NULL )
        starsystem->first_missile = missile;
     
     if ( starsystem->last_missile )
     {
         starsystem->last_missile->next_in_starsystem = missile;
         missile->prev_in_starsystem = starsystem->last_missile;
     }
     
     starsystem->last_missile = missile;
     
     missile->starsystem = starsystem;
        
}

void ship_from_starsystem( SHIP_DATA *ship , SPACE_DATA *starsystem )
{

     if ( starsystem == NULL )
        return;
     
     if ( ship == NULL )
        return;
     
     if ( starsystem->last_ship == ship )
        starsystem->last_ship = ship->prev_in_starsystem;
        
     if ( starsystem->first_ship == ship )
        starsystem->first_ship = ship->next_in_starsystem;
        
     if ( ship->prev_in_starsystem )
        ship->prev_in_starsystem->next_in_starsystem = ship->next_in_starsystem;
     
     if ( ship->next_in_starsystem)
        ship->next_in_starsystem->prev_in_starsystem = ship->prev_in_starsystem;
        
     ship->starsystem = NULL;
     ship->next_in_starsystem = NULL;
     ship->prev_in_starsystem = NULL;   

}

void extract_missile( MISSILE_DATA *missile )
{
    SPACE_DATA *starsystem;

     if ( missile == NULL )
        return;

     if ( ( starsystem = missile->starsystem ) != NULL )
     {
     
      if ( starsystem->last_missile == missile )
        starsystem->last_missile = missile->prev_in_starsystem;
        
      if ( starsystem->first_missile == missile )
        starsystem->first_missile = missile->next_in_starsystem;
        
      if ( missile->prev_in_starsystem )
        missile->prev_in_starsystem->next_in_starsystem = missile->next_in_starsystem;
     
      if ( missile->next_in_starsystem)
        missile->next_in_starsystem->prev_in_starsystem = missile->prev_in_starsystem;
        
      missile->starsystem = NULL;
      missile->next_in_starsystem = NULL;
      missile->prev_in_starsystem = NULL;   

     }
     
     UNLINK( missile, first_missile, last_missile, next, prev );
     
     missile->target = NULL; 
     missile->fired_from = NULL;
     if (  missile->fired_by )
        STRFREE( missile->fired_by );
     
     DISPOSE( missile );
          
}

bool is_rental( CHAR_DATA *ch , SHIP_DATA *ship )
{
   if ( !str_cmp("Public",ship->owner) )
          return TRUE;
             
   return FALSE; 
}

bool check_pilot( CHAR_DATA *ch , SHIP_DATA *ship )
{
   if ( !str_cmp(ch->name,ship->owner) || !str_cmp(ch->name,ship->pilot) 
   || !str_cmp(ch->name,ship->copilot) || !str_cmp("Public",ship->owner) )
      return TRUE;
   
   if ( !IS_NPC(ch) && ch->pcdata && ch->pcdata->clan )
   {        
      if ( !str_cmp(ch->pcdata->clan->name,ship->owner) ) 
      {
        if ( !str_cmp(ch->pcdata->clan->leader,ch->name) )
          return TRUE;
        if ( !str_cmp(ch->pcdata->clan->number1,ch->name) )
          return TRUE;
        if ( !str_cmp(ch->pcdata->clan->number2,ch->name) )
          return TRUE;
        if ( ch->pcdata->bestowments && is_name( "pilot", ch->pcdata->bestowments) )
          return TRUE;
      }
   }
    
   return FALSE;
}

bool extract_ship( SHIP_DATA *ship )
{   
    ROOM_INDEX_DATA *room;
    
    if ( ( room = ship->in_room ) != NULL )
    {               
        UNLINK( ship, room->first_ship, room->last_ship, next_in_room, prev_in_room );
        ship->in_room = NULL;
    }
    return TRUE;
}
void iondamage_ship_ch( SHIP_DATA *ship , int min , int max , CHAR_DATA *ch, SHIP_DATA *attacker )
{   
    int damage , shield_dmg;
    long xp;
    char buf[MAX_STRING_LENGTH];
 
    damage = number_range( min , max );
    damage = (int) damage * 1.3;
    shield_dmg = 0;
     
    xp = ( exp_level( ch->skill_level[PILOTING_ABILITY]+1) - exp_level( ch->skill_level[PILOTING_ABILITY]) ) / 25 ;
    xp = UMIN( get_ship_value( ship ) /100 , xp ) ;
    gain_exp( ch , xp , PILOTING_ABILITY );
      
    if ( ship->shield > 0 )
    {   
        shield_dmg = UMIN( ship->shield , damage );
    	damage -= shield_dmg;
        sprintf(buf, "Damage Report: sustained %d damage to shields.\n\r", shield_dmg);
        echo_to_cockpit( AT_YELLOW, ship , buf);
        sprintf(buf, "Sensors report blast inflicted %d damage to shields of %s.\n\r", shield_dmg, ship->name);
        echo_to_ship(AT_YELLOW, attacker, buf);
    	ship->shield -= shield_dmg;
    	if ( ship->shield == 0 )
  	echo_to_cockpit( AT_BLOOD , ship , "Shields down..." );    	  
    }
    if(ship->shield > 0)
        return; /* No Disabling of ships with shields still up. */
    if ( damage > 0 )
    {
        if ( number_range(1, 50) <= attacker->laserdamage*2 && ship->shipstate != SHIP_DISABLED )
        {
           echo_to_cockpit( AT_BLUE , ship , "Blue ion energy washes over the room!" );   
           echo_to_cockpit( AT_BLOOD + AT_BLINK , ship , "Ships Drive DAMAGED!" );   
           ship->shipstate = SHIP_DISABLED;
           ship->currspeed = 0;
           sprintf(buf, "Sensors report blast disabled %s's drive!", ship->name);
           echo_to_ship(AT_YELLOW, attacker, buf); 
           sprintf(buf, "Blasts from %s disable %s drive!", attacker->name, ship->name);
           echo_to_system(AT_ORANGE, ship, buf, NULL);
           echo_to_cockpit( AT_YELLOW , ship , "The ship begins to slow down.");
           sprintf( buf, "%s begins to slow down." , ship->name );
           echo_to_system( AT_ORANGE , ship , buf , NULL );
        }
        
        if ( number_range(1, 50) <= attacker->laserdamage*2 && ship->missilestate != MISSILE_DAMAGED && ship->mlaunchers > 0 )
        {
           echo_to_room( AT_BLUE , get_room_index(ship->gunseat) , "Blue ion energy washes over the room!" );   
           echo_to_room( AT_BLOOD + AT_BLINK , get_room_index(ship->gunseat) , "Ships Missile Launcher DAMAGED!" );   
        	   ship->missilestate = MISSILE_DAMAGED;
            sprintf(buf, "Sensors report blast disabled %s's missile launcher!", ship->name);
           echo_to_ship(AT_YELLOW, attacker, buf);  
        }
        if( number_range(1, 50) <= attacker->laserdamage*2 && ship->torpedostate != MISSILE_DAMAGED && ship->tlaunchers > 0)
        {
           echo_to_room( AT_BLUE , get_room_index(ship->gunseat) , "Blue ion energy washes over the room!" );
           echo_to_room( AT_BLOOD + AT_BLINK , get_room_index(ship->gunseat) , "Ships Torpedo Launcher DAMAGED!" );
                   ship->torpedostate = MISSILE_DAMAGED;
            sprintf(buf, "Sensors report blast disabled %s's torpedo launcher!", ship->name);
            echo_to_ship(AT_YELLOW, attacker, buf);
        }
        if( number_range(1, 50) <= attacker->laserdamage*2 && ship->rocketstate != MISSILE_DAMAGED && ship->rlaunchers > 0)
        {
           echo_to_room( AT_BLUE , get_room_index(ship->gunseat) , "Blue ion energy washes over the room!" );
           echo_to_room( AT_BLOOD + AT_BLINK , get_room_index(ship->gunseat) , "Ships Rocket Launcher DAMAGED!" );
                   ship->rocketstate = MISSILE_DAMAGED;
           sprintf(buf, "Sensors report blast disabled %s's rocket launcher!", ship->name);
           echo_to_ship(AT_YELLOW, attacker, buf);
        }

        if ( number_range(1, 50) <= attacker->laserdamage*2 && ship->statet0 != LASER_DAMAGED )
        {
           echo_to_room( AT_BLUE , get_room_index(ship->gunseat) , "Blue ion energy washes over the room!" );   
           echo_to_room( AT_BLOOD + AT_BLINK , get_room_index(ship->gunseat) , "Lasers DAMAGED!" );   
           ship->statet0 = LASER_DAMAGED;
           sprintf(buf, "Sensors report blast disabled %s's lasers!", ship->name);
           echo_to_ship(AT_YELLOW, attacker, buf);
        }
        if( number_range(1, 50) <= attacker->laserdamage*2 && ship->ionstate != LASER_DAMAGED && ship->ions > 0)
        {
           echo_to_room( AT_BLUE , get_room_index(ship->gunseat) , "Blue ion energy washes over the room!" );
           echo_to_room( AT_BLOOD + AT_BLINK , get_room_index(ship->gunseat) , "Ships Ion Cannons Damaged!" );
           ship->ionstate = LASER_DAMAGED;
           sprintf(buf, "Sensors report blast disabled %s's ion cannons!", ship->name);
           echo_to_ship(AT_YELLOW, attacker, buf);
        }    
        if ( number_range(1, 50) <= attacker->laserdamage*2 && ship->statet1 != LASER_DAMAGED && ship->turret1 )
        {
           echo_to_room( AT_BLUE , get_room_index(ship->turret1) , "Blue ion energy washes over the room!" );   
           echo_to_room( AT_BLOOD + AT_BLINK , get_room_index(ship->turret1) , "Turret DAMAGED!" );   
           ship->statet1 = LASER_DAMAGED;
           sprintf(buf, "Sensors report blast disabled %s's turret 1!", ship->name);
           echo_to_ship(AT_YELLOW, attacker, buf); 
        }
    
        if ( number_range(1, 50) <= attacker->laserdamage*2 && ship->statet2 != LASER_DAMAGED && ship->turret2 )
        {
           echo_to_room( AT_BLUE , get_room_index(ship->turret2) , "Blue ion energy washes over the room!" );   
           echo_to_room( AT_BLOOD + AT_BLINK , get_room_index(ship->turret2) , "Turret DAMAGED!" );   
           ship->statet2 = LASER_DAMAGED;
           sprintf(buf, "Sensors report blast disabled %s's turret 2!", ship->name);
           echo_to_ship(AT_YELLOW, attacker, buf);
        }
	}
	else
	{
        if ( number_range(1, 100) <= attacker->laserdamage*2 && ship->shipstate != SHIP_DISABLED )
        {
           echo_to_cockpit( AT_BLUE , ship , "Blue ion energy washes over the room!" );   
           echo_to_cockpit( AT_BLOOD + AT_BLINK , ship , "Ships Drive DAMAGED!" );   
           ship->shipstate = SHIP_DISABLED;
           ship->currspeed = 0;
           echo_to_cockpit( AT_YELLOW , ship , "The ship begins to slow down.");
           sprintf(buf, "Sensors report blast disabled %s's drive!", ship->name);
           echo_to_ship(AT_YELLOW, attacker, buf);
           sprintf(buf, "Blasts from %s disable %s drive!", attacker->name, ship->name);
           echo_to_system(AT_ORANGE, ship, buf, NULL);
           sprintf( buf, "%s begins to slow down." , ship->name );
           echo_to_system( AT_ORANGE , ship , buf , NULL );
        }
        
        if ( number_range(1, 100) <= attacker->laserdamage*2 && ship->missilestate != MISSILE_DAMAGED && ship->mlaunchers > 0 )
        {
           echo_to_room( AT_BLUE , get_room_index(ship->gunseat) , "Blue ion energy washes over the room!" );   
           echo_to_room( AT_BLOOD + AT_BLINK , get_room_index(ship->gunseat) , "Ships Missile Launcher DAMAGED!" );   
           ship->missilestate = MISSILE_DAMAGED;
           sprintf(buf, "Sensors report blast disabled %s's missile launcher!", ship->name);
           echo_to_ship(AT_YELLOW, attacker, buf);
        }
        if( number_range(1, 100) <= attacker->laserdamage*2 && ship->torpedostate != MISSILE_DAMAGED && ship->tlaunchers > 0)
        {
           echo_to_room( AT_BLUE , get_room_index(ship->gunseat) , "Blue ion energy washes over the room!" );
           echo_to_room( AT_BLOOD + AT_BLINK , get_room_index(ship->gunseat) , "Ships Torpedo Launcher DAMAGED!" );
                   ship->torpedostate = MISSILE_DAMAGED;
            sprintf(buf, "Sensors report blast disabled %s's torpedo launcher!", ship->name);
            echo_to_ship(AT_YELLOW, attacker, buf);
        }
        if( number_range(1, 100) <= attacker->laserdamage*2 && ship->rocketstate != MISSILE_DAMAGED && ship->rlaunchers > 0)
        {
           echo_to_room( AT_BLUE , get_room_index(ship->gunseat) , "Blue ion energy washes over the room!" );
           echo_to_room( AT_BLOOD + AT_BLINK , get_room_index(ship->gunseat) , "Ships Rocket Launcher DAMAGED!" );
                   ship->rocketstate = MISSILE_DAMAGED;
            sprintf(buf, "Sensors report blast disabled %s's rocket launcher!", ship->name);
            echo_to_ship(AT_YELLOW, attacker, buf);
        }
        if ( number_range(1, 100) <= attacker->laserdamage*2  && ship->statet0 != LASER_DAMAGED )
        {
           echo_to_room( AT_BLUE , get_room_index(ship->gunseat) , "Blue ion energy washes over the room!" );   
           echo_to_room( AT_BLOOD + AT_BLINK , get_room_index(ship->gunseat) , "Lasers DAMAGED!" );   
           ship->statet0 = LASER_DAMAGED;
            sprintf(buf, "Sensors report blast disabled %s's lasers!", ship->name);
            echo_to_ship(AT_YELLOW, attacker, buf);
        }
        if( number_range(1, 100) <= attacker->laserdamage*2 && ship->ionstate != LASER_DAMAGED && ship->ions > 0)
        {
           echo_to_room( AT_BLUE , get_room_index(ship->gunseat) , "Blue ion energy washes over the room!" );
           echo_to_room( AT_BLOOD + AT_BLINK , get_room_index(ship->gunseat) , "Ships Ion Cannons Damaged!" );
           ship->ionstate = LASER_DAMAGED;
           sprintf(buf, "Sensors report blast disabled %s's ion cannons!", ship->name);
           echo_to_ship(AT_YELLOW, attacker, buf);
        }
    
        if ( number_range(1, 100) <= attacker->laserdamage*2 && ship->statet1 != LASER_DAMAGED && ship->turret1 )
        {
           echo_to_room( AT_BLUE , get_room_index(ship->turret1) , "Blue ion energy washes over the room!" );   
           echo_to_room( AT_BLOOD + AT_BLINK , get_room_index(ship->turret1) , "Turret DAMAGED!" );   
           ship->statet1 = LASER_DAMAGED;
            sprintf(buf, "Sensors report blast disabled %s's turret 1!", ship->name);
            echo_to_ship(AT_YELLOW, attacker, buf);

        }
    
        if ( number_range(1, 100) <= attacker->laserdamage*2 && ship->statet2 != LASER_DAMAGED && ship->turret2 )
        {
           echo_to_room( AT_BLUE , get_room_index(ship->turret2) , "Blue ion energy washes over the room!" );   
           echo_to_room( AT_BLOOD + AT_BLINK , get_room_index(ship->turret2) , "Turret DAMAGED!" );   
           ship->statet2 = LASER_DAMAGED;
            sprintf(buf, "Sensors report blast disabled %s's turret 2!", ship->name);
            echo_to_ship(AT_YELLOW, attacker, buf);
        }
    }
}

void iondamage_ship( SHIP_DATA *ship , int min , int max, SHIP_DATA *attacker)
{   
    int damage , shield_dmg;
    char buf[MAX_STRING_LENGTH];
    shield_dmg = 0;
    damage = number_range( min , max );
    damage = (int) damage * 1.3;
    
    if ( ship->shield > 0 )
    {   
        shield_dmg = UMIN( ship->shield , damage );
    	damage -= shield_dmg;
    	ship->shield -= shield_dmg;
        sprintf(buf, "Damage Report: sustained %d damage to shields.\n\r", shield_dmg);
        echo_to_cockpit( AT_YELLOW, ship , buf);
        sprintf(buf, "Sensors report blast inflicted %d damage to shields to %s.\n\r", shield_dmg, ship->name);
        echo_to_ship(AT_YELLOW, attacker, buf);
    	if ( ship->shield == 0 )
    	  echo_to_cockpit( AT_BLOOD , ship , "Shields down..." );    	  
    }
    if( ship->shield > 0)
        return; /* No disable through shields -Arcturus */
    if ( damage > 0 )
    {
        if ( number_range(1, 50) <= attacker->laserdamage*2 && ship->shipstate != SHIP_DISABLED )
        {
           echo_to_cockpit( AT_BLUE , ship , "Blue ion energy washes over the room!" );   
           echo_to_cockpit( AT_BLOOD + AT_BLINK , ship , "Ships Drive DAMAGED!" );   
           ship->shipstate = SHIP_DISABLED;
           ship->currspeed = 0;
           echo_to_cockpit( AT_YELLOW , ship , "The ship begins to slow down.");
           sprintf(buf, "Sensors report blast disabled %s's drive!", ship->name);
           echo_to_ship(AT_YELLOW, attacker, buf);
           sprintf(buf, "Blasts from %s disable %s drive!", attacker->name, ship->name);
           echo_to_system(AT_ORANGE, ship, buf, NULL);
           sprintf( buf, "%s begins to slow down." , ship->name );
           echo_to_system( AT_ORANGE , ship , buf , NULL );
        }
        
        if ( number_range(1, 50) <= attacker->laserdamage*2 && ship->missilestate != MISSILE_DAMAGED && ship->mlaunchers > 0 )
        {
           echo_to_room( AT_BLUE , get_room_index(ship->gunseat) , "Blue ion energy washes over the room!" );
           echo_to_room( AT_BLOOD + AT_BLINK , get_room_index(ship->gunseat) , "Ships Missile Launcher DAMAGED!" );
                   ship->missilestate = MISSILE_DAMAGED;
           sprintf(buf, "Sensors report blast disabled %s's missile launcher!", ship->name);
           echo_to_ship(AT_YELLOW, attacker, buf);
        }
        if( number_range(1, 50) <= attacker->laserdamage*2 && ship->torpedostate != MISSILE_DAMAGED && ship->tlaunchers > 0)
        {
           echo_to_room( AT_BLUE , get_room_index(ship->gunseat) , "Blue ion energy washes over the room!" );
           echo_to_room( AT_BLOOD + AT_BLINK , get_room_index(ship->gunseat) , "Ships Torpedo Launcher DAMAGED!" );
                   ship->torpedostate = MISSILE_DAMAGED;
            sprintf(buf, "Sensors report blast disabled %s's torpedo launcher!", ship->name);
            echo_to_ship(AT_YELLOW, attacker, buf);
        }
        if( number_range(1, 50) <= attacker->laserdamage*2 && ship->rocketstate != MISSILE_DAMAGED && ship->rlaunchers > 0)
        {
           echo_to_room( AT_BLUE , get_room_index(ship->gunseat) , "Blue ion energy washes over the room!" );
           echo_to_room( AT_BLOOD + AT_BLINK , get_room_index(ship->gunseat) , "Ships Rocket Launcher DAMAGED!" );
                   ship->rocketstate = MISSILE_DAMAGED;
           sprintf(buf, "Sensors report blast disabled %s's rocket launcher!", ship->name);
           echo_to_ship(AT_YELLOW, attacker, buf);
        }        
        if ( number_range(1, 50) <= attacker->laserdamage*2 && ship->statet0 != LASER_DAMAGED )
        {
           echo_to_room( AT_BLUE , get_room_index(ship->gunseat) , "Blue ion energy washes over the room!" );   
           echo_to_room( AT_BLOOD + AT_BLINK , get_room_index(ship->gunseat) , "Lasers DAMAGED!" );   
           ship->statet0 = LASER_DAMAGED;
           sprintf(buf, "Sensors report blast disabled %s's lasers!", ship->name);
           echo_to_ship(AT_YELLOW, attacker, buf);
        }
        if( number_range(1, 50) <= attacker->laserdamage*2 && ship->ionstate != LASER_DAMAGED && ship->ions > 0)
        {
           echo_to_room( AT_BLUE , get_room_index(ship->gunseat) , "Blue ion energy washes over the room!" );
           echo_to_room( AT_BLOOD + AT_BLINK , get_room_index(ship->gunseat) , "Ships Ion Cannons Damaged!" );
           ship->ionstate = LASER_DAMAGED;
           sprintf(buf, "Sensors report blast disabled %s's ion cannons!", ship->name);
           echo_to_ship(AT_YELLOW, attacker, buf);
        }    
        if ( number_range(1, 50) <= attacker->laserdamage*2 && ship->statet1 != LASER_DAMAGED && ship->turret1 )
        {
           echo_to_room( AT_BLUE , get_room_index(ship->turret1) , "Blue ion energy washes over the room!" );   
           echo_to_room( AT_BLOOD + AT_BLINK , get_room_index(ship->turret1) , "Turret DAMAGED!" );   
           ship->statet1 = LASER_DAMAGED;
           sprintf(buf, "Sensors report blast disabled %s's turret 1!", ship->name);
           echo_to_ship(AT_YELLOW, attacker, buf);
        }
    
        if ( number_range(1, 50) <= attacker->laserdamage*2 && ship->statet2 != LASER_DAMAGED && ship->turret2 )
        {
           echo_to_room( AT_BLUE , get_room_index(ship->turret2) , "Blue ion energy washes over the room!" );   
           echo_to_room( AT_BLOOD + AT_BLINK , get_room_index(ship->turret2) , "Turret DAMAGED!" );   
           ship->statet2 = LASER_DAMAGED;
           sprintf(buf, "Sensors report blast disabled %s's turret 2!", ship->name);
           echo_to_ship(AT_YELLOW, attacker, buf);
        }
	}
	else
	{
        if ( number_range(1, 100) <= attacker->laserdamage*2 && ship->shipstate != SHIP_DISABLED )
        {
           echo_to_cockpit( AT_BLUE , ship , "Blue ion energy washes over the room!" );   
           echo_to_cockpit( AT_BLOOD + AT_BLINK , ship , "Ships Drive DAMAGED!" );   
           ship->shipstate = SHIP_DISABLED;
           ship->realspeed = 0; 
           echo_to_cockpit( AT_YELLOW , ship , "The ship begins to slow down.");
           sprintf(buf, "Sensors report blast disabled %s's drive!", ship->name);
           echo_to_ship(AT_YELLOW, attacker, buf);
           sprintf(buf, "Blasts from %s disable %s drive!", attacker->name, ship->name);
           echo_to_system(AT_ORANGE, ship, buf, NULL);
           sprintf( buf, "%s begins to slow down." , ship->name );
           echo_to_system( AT_ORANGE , ship , buf , NULL );
       }
        
        if ( number_range(1, 100) <= attacker->laserdamage*2 && ship->missilestate != MISSILE_DAMAGED && ship->mlaunchers > 0 )
        {
           echo_to_room( AT_BLUE , get_room_index(ship->gunseat) , "Blue ion energy washes over the room!" );   
           echo_to_room( AT_BLOOD + AT_BLINK , get_room_index(ship->gunseat) , "Ships Missile Launcher DAMAGED!" );   
           ship->missilestate = MISSILE_DAMAGED;
           sprintf(buf, "Sensors report blast disabled %s's missile launcher!", ship->name);
           echo_to_ship(AT_YELLOW, attacker, buf);
        }
        if( number_range(1, 100) <= attacker->laserdamage*2 && ship->torpedostate != MISSILE_DAMAGED && ship->tlaunchers > 0)
        {
           echo_to_room( AT_BLUE , get_room_index(ship->gunseat) , "Blue ion energy washes over the room!" );
           echo_to_room( AT_BLOOD + AT_BLINK , get_room_index(ship->gunseat) , "Ships Torpedo Launcher DAMAGED!" );
                   ship->torpedostate = MISSILE_DAMAGED;
            sprintf(buf, "Sensors report blast disabled %s's torpedo launcher!", ship->name);
            echo_to_ship(AT_YELLOW, attacker, buf);
        }
        if( number_range(1, 100) <= attacker->laserdamage*2 && ship->rocketstate != MISSILE_DAMAGED && ship->rlaunchers > 0)
        {
           echo_to_room( AT_BLUE , get_room_index(ship->gunseat) , "Blue ion energy washes over the room!" );
           echo_to_room( AT_BLOOD + AT_BLINK , get_room_index(ship->gunseat) , "Ships Rocket Launcher DAMAGED!" );
                   ship->rocketstate = MISSILE_DAMAGED;
           sprintf(buf, "Sensors report blast disabled %s's rocket launcher!", ship->name);
           echo_to_ship(AT_YELLOW, attacker, buf);
        }
        if ( number_range(1, 100) <= attacker->laserdamage*2 && ship->statet0 != LASER_DAMAGED )
        {
           echo_to_room( AT_BLUE , get_room_index(ship->gunseat) , "Blue ion energy washes over the room!" );   
           echo_to_room( AT_BLOOD + AT_BLINK , get_room_index(ship->gunseat) , "Lasers DAMAGED!" );   
           ship->statet0 = LASER_DAMAGED;
        }
        if( number_range(1, 100) <= attacker->laserdamage*2 && ship->ionstate != LASER_DAMAGED && ship->ions > 0)
        {
           echo_to_room( AT_BLUE , get_room_index(ship->gunseat) , "Blue ion energy washes over the room!" );
           echo_to_room( AT_BLOOD + AT_BLINK , get_room_index(ship->gunseat) , "Ships Ion Cannons Damaged!" );
           ship->ionstate = LASER_DAMAGED;
           sprintf(buf, "Sensors report blast disabled %s's ion cannons!", ship->name);
           echo_to_ship(AT_YELLOW, attacker, buf);
        }
        if ( number_range(1, 100) <= attacker->laserdamage*2 && ship->statet1 != LASER_DAMAGED && ship->turret1 )
        {
           echo_to_room( AT_BLUE , get_room_index(ship->turret1) , "Blue ion energy washes over the room!" );   
           echo_to_room( AT_BLOOD + AT_BLINK , get_room_index(ship->turret1) , "Turret DAMAGED!" );   
           ship->statet1 = LASER_DAMAGED;
        }
    
        if ( number_range(1, 100) <= attacker->laserdamage*2 && ship->statet2 != LASER_DAMAGED && ship->turret2 )
        {
           echo_to_room( AT_BLUE , get_room_index(ship->turret2) , "Blue ion energy washes over the room!" );   
           echo_to_room( AT_BLOOD + AT_BLINK , get_room_index(ship->turret2) , "Turret DAMAGED!" );   
           ship->statet2 = LASER_DAMAGED;
        }
    }
}

void damage_ship_ch( SHIP_DATA *ship , int min , int max , CHAR_DATA *ch, SHIP_DATA *attacker )
{   
    int damage , shield_dmg;
    long xp;
    char buf[MAX_STRING_LENGTH];
    shield_dmg = 0; 
    damage = number_range( min , max );
    
    xp = ( exp_level( ch->skill_level[PILOTING_ABILITY]+1) - exp_level( ch->skill_level[PILOTING_ABILITY]) ) / 25 ;
    xp = UMIN( get_ship_value( ship ) /100 , xp ) ;
    gain_exp( ch , xp , PILOTING_ABILITY );

        
    if ( ship->shield > 0 )
    {   
        shield_dmg = UMIN( ship->shield , damage );
    	damage -= shield_dmg;
    	ship->shield -= shield_dmg;
        sprintf(buf, "Damage Report: sustained %d damage to shields.\n\r", shield_dmg);
        echo_to_cockpit( AT_YELLOW, ship , buf);
        sprintf(buf, "Sensors report blast inflicted %d damage to shields to %s.\n\r", shield_dmg, ship->name);
        echo_to_ship(AT_YELLOW, attacker, buf);    
	if ( ship->shield == 0 )
    	  echo_to_cockpit( AT_BLOOD , ship , "Shields down..." );    	  
    }
    damage = damage - ship->armor;
    if (damage < 1 )
        damage = 0;
    if ( damage > 0 )
    {/* Arcturus' Damage bit, a bit more random, and perhaps a little more often. */
        if ( number_range(1, 100) <= number_range(1, damage/2+1) && ship->shipstate != SHIP_DISABLED )
        {
           echo_to_cockpit( AT_BLOOD + AT_BLINK , ship , "Ships Drive DAMAGED!" );   
           ship->shipstate = SHIP_DISABLED;
           ship->currspeed = 0;
           echo_to_cockpit( AT_YELLOW , ship , "The ship begins to slow down.");
           sprintf(buf, "Sensors report blast disabled %s's drive!", ship->name);
           echo_to_ship(AT_YELLOW, attacker, buf);
           sprintf(buf, "Blasts from %s disable %s drive!", attacker->name, ship->name);
           echo_to_system(AT_ORANGE, ship, buf, NULL);   
           sprintf( buf, "%s begins to slow down." , ship->name );
           echo_to_system( AT_ORANGE , ship , buf , NULL );
        }
        
        if ( number_range(1, 100) <= number_range(1, damage/2+1)&& ship->missilestate != MISSILE_DAMAGED && ship->mlaunchers > 0 )
        {
           echo_to_room( AT_BLUE , get_room_index(ship->gunseat) , "Blue ion energy washes over the room!" );
           echo_to_room( AT_BLOOD + AT_BLINK , get_room_index(ship->gunseat) , "Ships Missile Launcher DAMAGED!" );
                   ship->missilestate = MISSILE_DAMAGED;
           sprintf(buf, "Sensors report blast disabled %s's missile launcher!", ship->name);
           echo_to_ship(AT_YELLOW, attacker, buf);
        }        
        if( number_range(1, 100) <= number_range(1, damage/2+1) && ship->torpedostate != MISSILE_DAMAGED && ship->tlaunchers > 0)
        {
           echo_to_room( AT_BLUE , get_room_index(ship->gunseat) , "Blue ion energy washes over the room!" );
           echo_to_room( AT_BLOOD + AT_BLINK , get_room_index(ship->gunseat) , "Ships Torpedo Launcher DAMAGED!" );
                   ship->torpedostate = MISSILE_DAMAGED;
            sprintf(buf, "Sensors report blast disabled %s's torpedo launcher!", ship->name);
            echo_to_ship(AT_YELLOW, attacker, buf);
        }
        if( number_range(1, 100) <= number_range(1, damage/2+1) && ship->rocketstate != MISSILE_DAMAGED && ship->rlaunchers > 0)
        {
           echo_to_room( AT_BLUE , get_room_index(ship->gunseat) , "Blue ion energy washes over the room!" );
           echo_to_room( AT_BLOOD + AT_BLINK , get_room_index(ship->gunseat) , "Ships Rocket Launcher DAMAGED!" );
                   ship->rocketstate = MISSILE_DAMAGED;
           sprintf(buf, "Sensors report blast disabled %s's rocket launcher!", ship->name);
           echo_to_ship(AT_YELLOW, attacker, buf);
        }
        if ( number_range(1, 100) <= number_range(1, damage/2+1) && ship->statet0 != LASER_DAMAGED )
        {
           echo_to_room( AT_BLOOD + AT_BLINK , get_room_index(ship->gunseat) , "Lasers DAMAGED!" );   
           ship->statet0 = LASER_DAMAGED;
           sprintf(buf, "Sensors report blast disabled %s's lasers!", ship->name);
           echo_to_ship(AT_YELLOW, attacker, buf);
        }
        if( number_range(1, 100) <= number_range(1, damage/2+1) && ship->ionstate != LASER_DAMAGED && ship->ions > 0)
        {
           echo_to_room( AT_BLUE , get_room_index(ship->gunseat) , "Blue ion energy washes over the room!" );
           echo_to_room( AT_BLOOD + AT_BLINK , get_room_index(ship->gunseat) , "Ships Ion Cannons Damaged!" );
           ship->ionstate = LASER_DAMAGED;
           sprintf(buf, "Sensors report blast disabled %s's ion cannons!", ship->name);
           echo_to_ship(AT_YELLOW, attacker, buf);
        }
        if ( number_range(1, 100) <= number_range(1, damage/2+1) && ship->statet1 != LASER_DAMAGED && ship->turret1 )
        {
           echo_to_room( AT_BLOOD + AT_BLINK , get_room_index(ship->turret1) , "Turret DAMAGED!" );   
           ship->statet1 = LASER_DAMAGED;
           sprintf(buf, "Sensors report blast disabled %s's turret 1!", ship->name);
           echo_to_ship(AT_YELLOW, attacker, buf);
        }
    
        if ( number_range(1, 100) <= number_range(1, damage/2+1) && ship->statet2 != LASER_DAMAGED && ship->turret2 )
        {
           echo_to_room( AT_BLOOD + AT_BLINK , get_room_index(ship->turret2) , "Turret DAMAGED!" );   
           ship->statet2 = LASER_DAMAGED;
           sprintf(buf, "Sensors report blast disabled %s's turret 2!", ship->name);
           echo_to_ship(AT_YELLOW, attacker, buf);
        }

    }

    if (damage >= 1)
    {
        sprintf(buf, "Damage Report: sustained %d damage to hull integrity.\n\r", damage);
        echo_to_cockpit( AT_YELLOW, ship , buf);
        sprintf(buf, "Sensors report blast inflicted %d damage to hull integrity of %s.\n\r", damage, ship->name);
        echo_to_ship(AT_YELLOW, attacker, buf);
    }
    ship->hull -= damage;
    
    if ( ship->hull <= 0 )
    {
       destroy_ship( ship , NULL );
       
       xp =  ( exp_level( ch->skill_level[PILOTING_ABILITY]+1) - exp_level( ch->skill_level[PILOTING_ABILITY]) );
       xp = UMIN( get_ship_value( ship ) , xp );
       gain_exp( ch , xp , PILOTING_ABILITY);
       ch_printf( ch, "&WYou gain %ld piloting experience!\n\r", xp );
       return;
    }
    
    if ( ship->hull <= ship->maxhull/20 )
       echo_to_cockpit( AT_BLOOD+ AT_BLINK , ship , "WARNING! Ship hull severely damaged!" );    	  

}

void damage_ship( SHIP_DATA *ship , int min , int max, SHIP_DATA *attacker )
{   
    int damage , shield_dmg;
    char buf[MAX_STRING_LENGTH];     
    damage = number_range( min , max );
    
    if ( ship->shield > 0 )
    {   
        shield_dmg = UMIN( ship->shield , damage );
    	damage -= shield_dmg;
    	ship->shield -= shield_dmg;
        sprintf(buf, "Damage Report: sustained %d damage to shields.\n\r", shield_dmg);
        echo_to_cockpit( AT_YELLOW, ship , buf);
        sprintf(buf, "Sensors report blast inflicted %d damage to shields to %s.\n\r", shield_dmg, ship->name);
        echo_to_ship(AT_YELLOW, attacker, buf);
    	if ( ship->shield == 0 )
    	  echo_to_cockpit( AT_BLOOD , ship , "Shields down..." );    	  
    }
    damage = damage - ship->armor;
    if (damage < 1 )
        damage = 0;    
    if ( damage > 0 )
    {

        if ( number_range(1, 100) <= number_range(1, damage/2+1) && ship->shipstate != SHIP_DISABLED )
        {
           echo_to_cockpit( AT_BLOOD + AT_BLINK , ship , "Ships Drive DAMAGED!" );   
           ship->shipstate = SHIP_DISABLED;
           ship->currspeed = 0;
           echo_to_cockpit( AT_YELLOW , ship , "The ship begins to slow down.");
           sprintf(buf, "Sensors report blast disabled %s's drive!", ship->name);
           echo_to_ship(AT_YELLOW, attacker, buf);
           sprintf(buf, "Blasts from %s disable %s drive!", attacker->name, ship->name);
           echo_to_system(AT_ORANGE, ship, buf, NULL);
           sprintf( buf, "%s begins to slow down." , ship->name );
           echo_to_system( AT_ORANGE , ship , buf , NULL );
        }
        
        if ( number_range(1, 100) <= number_range(1, damage/2+1)&& ship->missilestate != MISSILE_DAMAGED && ship->mlaunchers > 0 )
        {
           echo_to_room( AT_BLUE , get_room_index(ship->gunseat) , "Blue ion energy washes over the room!" );
           echo_to_room( AT_BLOOD + AT_BLINK , get_room_index(ship->gunseat) , "Ships Missile Launcher DAMAGED!" );
                   ship->missilestate = MISSILE_DAMAGED;
           sprintf(buf, "Sensors report blast disabled %s's missile launcher!", ship->name);
           echo_to_ship(AT_YELLOW, attacker, buf);
        }        
        if( number_range(1, 100) <= number_range(1, damage/2+1) && ship->torpedostate != MISSILE_DAMAGED && ship->tlaunchers > 0)
        {
           echo_to_room( AT_BLUE , get_room_index(ship->gunseat) , "Blue ion energy washes over the room!" );
           echo_to_room( AT_BLOOD + AT_BLINK , get_room_index(ship->gunseat) , "Ships Torpedo Launcher DAMAGED!" );
                   ship->torpedostate = MISSILE_DAMAGED;
            sprintf(buf, "Sensors report blast disabled %s's torpedo launcher!", ship->name);
            echo_to_ship(AT_YELLOW, attacker, buf);
        }
        if( number_range(1, 100) <= number_range(1, damage/2+1) && ship->rocketstate != MISSILE_DAMAGED && ship->rlaunchers > 0)
        {
           echo_to_room( AT_BLUE , get_room_index(ship->gunseat) , "Blue ion energy washes over the room!" );
           echo_to_room( AT_BLOOD + AT_BLINK , get_room_index(ship->gunseat) , "Ships Rocket Launcher DAMAGED!" );
                   ship->rocketstate = MISSILE_DAMAGED;
           sprintf(buf, "Sensors report blast disabled %s's rocket launcher!", ship->name);
           echo_to_ship(AT_YELLOW, attacker, buf);
        }
        if ( number_range(1, 100) <= number_range(1, damage/2+1) && ship->statet0 != LASER_DAMAGED )
        {
           echo_to_room( AT_BLOOD + AT_BLINK , get_room_index(ship->gunseat) , "Lasers DAMAGED!" );
           ship->statet0 = LASER_DAMAGED;
           sprintf(buf, "Sensors report blast disabled %s's lasers!", ship->name);
           echo_to_ship(AT_YELLOW, attacker, buf);
        }
        if( number_range(1, 100) <= number_range(1, damage/2+1) && ship->ionstate != LASER_DAMAGED && ship->ions > 0)
        {
           echo_to_room( AT_BLUE , get_room_index(ship->gunseat) , "Blue ion energy washes over the room!" );
           echo_to_room( AT_BLOOD + AT_BLINK , get_room_index(ship->gunseat) , "Ships Ion Cannons Damaged!" );
           ship->ionstate = LASER_DAMAGED;
           sprintf(buf, "Sensors report blast disabled %s's ion cannons!", ship->name);
           echo_to_ship(AT_YELLOW, attacker, buf);
        }

        if ( number_range(1, 100) <= number_range(1, damage/2+1) && ship->statet1 != LASER_DAMAGED && ship->turret1 )
        {
           echo_to_room( AT_BLOOD + AT_BLINK , get_room_index(ship->turret1) , "Turret DAMAGED!" );   
           ship->statet1 = LASER_DAMAGED;
           sprintf(buf, "Sensors report blast disabled %s's turret 1!", ship->name);
           echo_to_ship(AT_YELLOW, attacker, buf);
        }
        if ( number_range(1, 100) <= number_range(1, damage/2+1) && ship->statet2 != LASER_DAMAGED && ship->turret2 )
        {
           echo_to_room( AT_BLOOD + AT_BLINK , get_room_index(ship->turret2) , "Turret DAMAGED!" );   
           ship->statet2 = LASER_DAMAGED;
           sprintf(buf, "Sensors report blast disabled %s's turret 2!", ship->name);
           echo_to_ship(AT_YELLOW, attacker, buf);
        }
    
    }
    
    if(damage >= 1)
    {
        sprintf(buf, "Damage Report: sustained %d damage to hull integrity.\n\r", damage);
        echo_to_cockpit( AT_YELLOW, ship , buf);
        sprintf(buf, "Sensors report blast inflicted %d damage to hull integrity of %s\n\r", damage, ship->name);
        echo_to_ship(AT_YELLOW, attacker, buf);
    }
    ship->hull -= damage;
    
    if ( ship->hull <= 0 )
    {
       destroy_ship( ship , NULL );
       return;
    }
    
    if ( ship->hull <= ship->maxhull/2 )
       echo_to_cockpit( AT_BLOOD+ AT_BLINK , ship , "WARNING! Ship hull severely damaged!" );    	  
     
}

void destroy_ship( SHIP_DATA *ship , CHAR_DATA *ch )
{   
    char buf[MAX_STRING_LENGTH];
    char logbuf[MAX_STRING_LENGTH];
    int  roomnum;
    ROOM_INDEX_DATA *room;
    OBJ_DATA *robj;
    CHAR_DATA *rch;
    
    sprintf( buf , "%s explodes in a blinding flash of light!", ship->name );  
    echo_to_system( AT_WHITE + AT_BLINK , ship , buf , NULL );
    sprintf( logbuf, "%s blew up", ship->name );
    log_string( logbuf );    
    echo_to_ship( AT_WHITE + AT_BLINK , ship , "A blinding flash of light burns your eyes...");
    echo_to_ship( AT_WHITE , ship , "But before you have a chance to scream...\n\rYou are ripped apart as your spacecraft explodes...");
        
    for ( roomnum = ship->firstroom ; roomnum <= ship->lastroom ; roomnum++ )
    {
        room = get_room_index(roomnum);
        
        if (room != NULL)
        {
         rch = room->first_person;   
         while ( rch )
         { /* Simulator Code written by Ackbar, added by Arcturus */
           if( IS_SET( ship->flags, SHIPFLAG_SIMULATOR ) )
            {
                 resetship(ship);
	         ship->shipyard = ship->lastdoc;
                 ship->location = ship->lastdoc;
	  	 ship->shipstate = SHIP_READY;
                 ship->shipstate2 = SHIP_READY;
	         extract_ship( ship );
        	 ship_to_room( ship , ship->shipyard ); 
          	 ship->location = ship->shipyard;
          	 ship->lastdoc = ship->shipyard; 
          	 ship->shipstate = SHIP_DOCKED;    
          	 if (ship->starsystem)
        	   ship_from_starsystem( ship, ship->starsystem );  
                 save_ship(ship);               
          	 send_to_char("The lights dim and the hatch opens.\n\r", rch);	 
           	 return;
            }
            if ( IS_IMMORTAL(rch) )
            {
                 char_from_room(rch);
                 char_to_room( rch, get_room_index(wherehome(rch)) );
            }
            else
            {
                     sprintf( buf, "%s is Dead! {Ship incident}", rch->name );
                     echo_to_all( AT_RED, buf, 0);
                     raw_kill( rch , rch );
            }
            rch = room->first_person;     
         }
        
         for ( robj = room->first_content ; robj ; robj = robj->next_content )
         {
           separate_obj( robj );
           extract_obj( robj );
         }
        }

    }
    
    resetship(ship);

}

bool ship_to_room(SHIP_DATA *ship , int vnum )
{
    ROOM_INDEX_DATA *shipto;
    
    if ( (shipto=get_room_index(vnum)) == NULL )
            return FALSE;
    LINK( ship, shipto->first_ship, shipto->last_ship, next_in_room, prev_in_room );
    ship->in_room = shipto; 
    return TRUE;
}


/* New do_board with docking support below - Parnic
void do_board( CHAR_DATA *ch, char *argument )
{
   ROOM_INDEX_DATA *fromroom;
   ROOM_INDEX_DATA *toroom;
   SHIP_DATA *ship;
   
   if ( !argument || argument[0] == '\0')
   {
       send_to_char( "Board what?\n\r", ch );
       return;
   }
   
   if ( ( ship = ship_in_room( ch->in_room , argument ) ) == NULL )
   {
            act( AT_PLAIN, "I see no $T here.", ch, NULL, argument, TO_CHAR );
           return;
   }

   if ( IS_SET( ch->act, ACT_MOUNTED ) )
   {
          return;
   } 

   fromroom = ch->in_room;

        if ( ( toroom = get_room_index( ship->entrance ) ) != NULL )
   	{
   	   if ( ! ship->hatchopen )
   	   {
   	      send_to_char( "&RThe hatch is closed!\n\r", ch);
   	      return;
   	   }
   	
           if ( toroom->tunnel > 0 )
           {
	        CHAR_DATA *ctmp;
	        int count = 0;
	        
	       for ( ctmp = toroom->first_person; ctmp; ctmp = ctmp->next_in_room )
	       if ( ++count >= toroom->tunnel )
	       {
                  send_to_char( "There is no room for you in there.\n\r", ch );
		  return;
	       }
           }
            if ( ship->shipstate == SHIP_LAUNCH || ship->shipstate == SHIP_LAUNCH_2 )
            {
                 send_to_char("&rThat ship has already started launching!\n\r",ch);
                 return;
            }
            
            act( AT_PLAIN, "$n enters $T.", ch,
		NULL, ship->name , TO_ROOM );
	    act( AT_PLAIN, "You enter $T.", ch,
		NULL, ship->name , TO_CHAR );
   	    char_from_room( ch );
   	    char_to_room( ch , toroom );
   	    act( AT_PLAIN, "$n enters the ship.", ch,
		NULL, argument , TO_ROOM );
            do_look( ch , "auto" );

        }                                                                  
        else
          send_to_char("That ship has no entrance!\n\r", ch);
}
*/

/* New board...originally modified by Soulbender */
void do_board( CHAR_DATA *ch, char *argument )
{
   ROOM_INDEX_DATA *fromroom;
   ROOM_INDEX_DATA *toroom;
   SHIP_DATA *ship;
   SHIP_DATA *in_ship;
   
   if (!argument || argument[0] == '\0')
   {
       send_to_char( "Board what?\n\r", ch );
       return;
   }
   
   if (str_cmp( argument, "dock" ))
   {
     if ( ( ship = ship_in_room( ch->in_room , argument ) ) == NULL )
     {
              act( AT_PLAIN, "I see no $T here.", ch, NULL, argument,TO_CHAR );
             return;
     }
   }
   else
   {
       fromroom = ch->in_room;
       
       if ( ( in_ship = ship_from_entrance( fromroom->vnum ) ) == NULL )
       {
          send_to_char( "&RYou need to be at the entrance.\n\r", ch);
          return;
       }
      
       if (!(in_ship->shipstate2 == SHIP_DOCK_3))
       {
          if (in_ship->shipstate2 == SHIP_DOCK_2)
            send_to_char( "&RWait until docking sequence is complete.",ch);
          else
            send_to_char( "&RThis ship isn't currently docked.", ch);
          return;
       }
       if(IS_SET(in_ship->flags, SHIPFLAG_SIMULATOR))
       {
          send_to_char("Your simulator doesn't lead into there.\n\r", ch);
          return;
       }
       ship = in_ship->docked_ship;
   }
   
   if ( IS_SET( ch->act, ACT_MOUNTED ) )
   {
          act( AT_PLAIN, "You can't go in there riding THAT.", ch, NULL, argument, TO_CHAR );  
          return;
   } 

   fromroom = ch->in_room;

        if ( ( toroom = get_room_index( ship->entrance ) ) != NULL )
   	{
   	   if ( (!ship->hatchopen) && !IS_DOCKED(ship) )
   	   {
   	      send_to_char( "&RThe hatch is closed!\n\r", ch);
   	      return;
   	   }
   	
           if ( toroom->tunnel > 0 )
           {
	        CHAR_DATA *ctmp;
	        int count = 0;
	        
	       for ( ctmp = toroom->first_person; ctmp; ctmp = ctmp->next_in_room )
	       if ( ++count >= toroom->tunnel )
	       {
                  send_to_char( "There is no room for you in there.\n\r",ch );
		  return;
	       }
           }
            if ( ship->shipstate == SHIP_LAUNCH || ship->shipstate ==SHIP_LAUNCH_2 )
            {
                 send_to_char("&rThat ship has already started launching!\n\r",ch);
                 return;
            }
            
    rprog_leave_trigger( ch );
    if( char_died(ch) )
      return;
            act( AT_PLAIN, "$n enters $T.", ch,
		NULL, ship->name , TO_ROOM );
	    act( AT_PLAIN, "You enter $T.", ch,
		NULL, ship->name , TO_CHAR );
   	    char_from_room( ch );
   	    char_to_room( ch , toroom );
   	    act( AT_PLAIN, "$n enters the ship.", ch,
		NULL, argument , TO_ROOM );
            do_look( ch , "auto" );
    mprog_entry_trigger( ch );
    if ( char_died(ch) )
      return;
      
    rprog_enter_trigger( ch );
    if ( char_died(ch) )
       return;
      
    mprog_greet_trigger( ch ); 
    if ( char_died(ch) )
       return;
        
    oprog_greet_trigger( ch );
    if ( char_died(ch) )
       return;
        }                                                                  
        else
          send_to_char("That ship has no entrance!\n\r", ch);
}

bool rent_ship( CHAR_DATA *ch , SHIP_DATA *ship )
{   

    long price;

    if ( IS_NPC ( ch ) )
       return FALSE;
       price = 500 + 250*ship->class;  
       if ( ch->gold < price )
       {
         ch_printf(ch, "&RYou can't afford to rent this ship, it costs %ld credits.\n\r", price);
         return FALSE;
       }
    
       ch->gold -= price;
       ch_printf(ch, "&GYou pay %ld credits to rent the ship.\n\r" , price );   
       return TRUE;
     
}

void do_leaveship( CHAR_DATA *ch, char *argument )
{
    ROOM_INDEX_DATA *fromroom;
    ROOM_INDEX_DATA *toroom;
    SHIP_DATA *ship;
    
    fromroom = ch->in_room;
    
    if  ( (ship = ship_from_entrance(fromroom->vnum)) == NULL )
    {
        send_to_char( "I see no exit here.\n\r" , ch );
        return;
    }   
    
    if  ( ship->class == SHIP_PLATFORM )
    {
        return;
    }   
    
    if ( ship->lastdoc != ship->location )
    {
        send_to_char("&rMaybe you should wait until the ship lands.\n\r",ch);
        return;
    }
    
    if ( ship->shipstate != SHIP_DOCKED && ship->shipstate != SHIP_DISABLED )
    {
        send_to_char("&rPlease wait till the ship is properly docked.\n\r",ch);
        return;
    }
    
    if ( ! ship->hatchopen )
    {
    	send_to_char("&RYou need to open the hatch first" , ch );
    	return;
    }
    
    if ( ( toroom = get_room_index( ship->location ) ) != NULL )
    {
            act( AT_PLAIN, "$n exits the ship.", ch,
		NULL, argument , TO_ROOM );
	    act( AT_PLAIN, "You exit the ship.", ch,
		NULL, argument , TO_CHAR );
   	    char_from_room( ch );
   	    char_to_room( ch , toroom );
   	    act( AT_PLAIN, "$n steps out of a ship.", ch,
		NULL, argument , TO_ROOM );
            do_look( ch , "auto" );
     }       
}

void do_launch ( CHAR_DATA *ch, char *argument)
{

    SHIP_DATA * ship;
    SHIP_DATA * onship;
    int chance; 
    long price = 0;
    char buf[MAX_STRING_LENGTH];
 
    if(!ch) return;           
    	        if ( (ship = ship_from_cockpit(ch->in_room->vnum)) == NULL )  
    	        {
    	            send_to_char("&RYou must be in the cockpit of a ship to do that!\n\r",ch);
    	            return;
    	        }
    	        
        if ( (( onship = ship_from_hanger( ship->location ) )  != NULL ) 
           && !(IS_SET(ship->flags, SHIPFLAG_SIMULATOR) ? 1 
                                                        : onship->bayopen) )
        {
                send_to_char("&RThe hangar is closed!\n\r",ch);
                return;
        }



    	        if ( ship->class > MOBILE_SUIT )
    	        {
                send_to_char("&RGet in a ship first!\n\r",ch);
    	            return;
    	        }
    	        
    	        if ( (ship = ship_from_pilotseat(ch->in_room->vnum)) == NULL )  
    	        {
    	            send_to_char("&RYou don't seem to be in the pilot seat!\n\r",ch);
    	            return;
    	        }
    	        
    	        if ( autofly(ship) )
    	        {
    	            send_to_char("&RThe ship is set on autopilot, you'll have to turn it off first.\n\r",ch);
    	            return;
    	        }
    	        
                if  ( ship->class == SHIP_PLATFORM )
                {
                   send_to_char( "You can't do that here.\n\r" , ch );
                   return;
                }   
    
    	        if ( !check_pilot( ch , ship ) )
    	        {
    	            send_to_char("&RHey, thats not your ship! Try renting a public one.\n\r",ch);
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
                
    	        if ( ship->class == FIGHTER_SHIP )
                    chance = IS_NPC(ch) ? ch->top_level
	                 : (int)  (ch->pcdata->learned[gsn_starfighters]) ;
    	        if ( ship->class == MOBILE_SUIT )
                    chance = IS_NPC(ch) ? ch->top_level
	                 : (int)  (ch->pcdata->learned[gsn_starfighters]) ;
                if ( ship->class == MIDSIZE_SHIP )
                    chance = IS_NPC(ch) ? ch->top_level
	                 : (int)  (ch->pcdata->learned[gsn_midships]) ;
                if ( ship->class == CAPITAL_SHIP )
                    chance = IS_NPC(ch) ? ch->top_level
	                 : (int) (ch->pcdata->learned[gsn_capitalships]);
                if ( ship->class == SUPERCAPITAL_SHIP )
                    chance = IS_NPC(ch) ? ch->top_level
	                 : (int) (ch->pcdata->learned[gsn_supercapitalships]);
                if ( ship->class == FRIGATE_SHIP )
                    chance = IS_NPC(ch) ? ch->top_level
	                 : (int) (ch->pcdata->learned[gsn_frigates]);
                if ( ship->class == SHIP_PLATFORM )
                    chance = IS_NPC(ch) ? ch->top_level
	                 : (int) (ch->pcdata->learned[gsn_capitalships]);
                if ( number_percent( ) < chance )
    		{  
   		   if ( is_rental(ch,ship) )
    		     if( !rent_ship(ch,ship) )
    		        return;
    		    if ( !is_rental(ch,ship) )
                    {    
    		     if ( ship->class == FIGHTER_SHIP )
                       price=20;
                     if ( ship->class == MIDSIZE_SHIP )
                       price=50;
                     if ( ship->class == MOBILE_SUIT )
                       price=50;
                     if ( ship->class == FRIGATE_SHIP )
                       price=250;
                     if ( ship->class == SHIP_PLATFORM )
                       price=750;
                     if ( ship->class == SUPERCAPITAL_SHIP )
                       price=5000;
                     if ( ship->class == CAPITAL_SHIP )
                       price=500;
                      
                     price += ( ship->maxhull-ship->hull );
                     if (ship->missiles )
     	                 price += ( 50 * (ship->maxmissiles-ship->missiles) );
                     else if (ship->torpedos )
     	                 price += ( 75 * (ship->maxtorpedos-ship->torpedos) );
                     else if (ship->rockets )
                         price += ( 150 * (ship->maxrockets-ship->rockets) );
                     else if (ship->autocannon )
                         price += ( 500 * (ship->autoammomax-ship->autoammo) );                 
                     if (ship->shipstate == SHIP_DISABLED )
                            price += 200;
                     if ( ship->missilestate == MISSILE_DAMAGED )
                            price += 100;
                     if ( ship->torpedostate == MISSILE_DAMAGED )
                            price += 200;
                     if ( ship->rocketstate == MISSILE_DAMAGED ) 
                            price += 300;
                     if ( ship->ionstate == LASER_DAMAGED )
                            price += 100;
                     if ( ship->statet0 == LASER_DAMAGED )
                            price += 50;
                     if ( ship->statet1 == LASER_DAMAGED )
                            price += 50;
                     if ( ship->statet2 == LASER_DAMAGED )
                            price += 50;
                    }                
                
    	          if ( ch->pcdata && ch->pcdata->clan && !str_cmp(ch->pcdata->clan->name,ship->owner) ) 
                  {
                   if ( ch->pcdata->clan->funds < price )
                   {
                       ch_printf(ch, "&R%s doesn't have enough funds to prepare this ship for launch.\n\r", ch->pcdata->clan->name );
                       return;
                   }
    
                   ch->pcdata->clan->funds -= price;
                   ch_printf(ch, "&GIt costs %s %ld credits to ready this ship for launch.\n\r", ch->pcdata->clan->name, price );   
                  }
                  else if ( str_cmp( ship->owner , "Public" ) )
                  {
                   if ( ch->gold < price )
                   {
                       ch_printf(ch, "&RYou don't have enough funds to prepare this ship for launch.\n\r");
                       return;
                   }
    
                   ch->gold -= price;
                   ch_printf(ch, "&GYou pay %ld credits to ready the ship for launch.\n\r", price );   
                   rem_extshipflags(ship);                
                  }
                  ship->autoammo = ship->autoammomax;
                  ship->energy = ship->maxenergy;
                  ship->chaff = ship->maxchaff;
                  ship->missiles = ship->maxmissiles;
       		  ship->torpedos = ship->maxtorpedos;
       		  ship->rockets = ship->maxrockets;
       		  ship->shield = 0;
       		  ship->autorecharge = FALSE;
       		  ship->autotrack = FALSE;
       		  ship->autospeed = FALSE;
       		  ship->interdictactive = FALSE;
       		  ship->hull = ship->maxhull;
       
       		  ship->missilestate = MISSILE_READY;
       		  ship->statet0 = LASER_READY;
       		  ship->statet1 = LASER_READY;
       		  ship->statet2 = LASER_READY;
	          ship->ionstate = LASER_READY;
                  ship->torpedostate = MISSILE_READY;
                  ship->rocketstate = MISSILE_READY;
       		  ship->shipstate = SHIP_DOCKED;
                  ship->shipstate2 = SHIP_READY;
                if (ship->energy == 0)
			  {
				  send_to_char("&RThis ship has no fuel, try installing a fuel module.\n\r",ch);
				  return;
			  }	

                if (ship->laserdamage == 0)
			  {
				  ship->laserdamage = 1;
			  }	

    		   if (ship->hatchopen)
    		   {
    		     ship->hatchopen = FALSE;
    		     sprintf( buf , "The hatch on %s closes." , ship->name);  
       	             echo_to_room( AT_YELLOW , get_room_index(ship->location) , buf );
       	             echo_to_room( AT_YELLOW , get_room_index(ship->entrance) , "The hatch slides shut." );
       	             sound_to_room( get_room_index(ship->entrance) , "!!SOUND(door)" );
      		     sound_to_room( get_room_index(ship->location) , "!!SOUND(door)" );
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
                   if ( ship->class == SUPERCAPITAL_SHIP )
                      learn_from_success( ch, gsn_supercapitalships );
                   if ( ship->class == FRIGATE_SHIP )
                      learn_from_success( ch, gsn_frigates );
                   if ( ship->class == SHIP_PLATFORM )
                      learn_from_success( ch, gsn_capitalships );
                   if ( ship->class == MOBILE_SUIT )
                      learn_from_success( ch, gsn_starfighters );
                   sound_to_ship(ship , "!!SOUND(xwing)" );  
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

void launchship( SHIP_DATA *ship )
{   
    char buf[MAX_STRING_LENGTH];
    SHIP_DATA *target;
    int plusminus;
    SPACE_DATA *simul;
    for ( simul = first_starsystem; simul; simul = simul->next ) 
    {
    	if(!strcmp(simul->name,"Simulator\0"))
    	    break;
    }
    if(IS_SET(ship->flags, SHIPFLAG_SIMULATOR))
    {
    	ship_to_starsystem( ship, simul);
    }
    else
    {
        ship_to_starsystem( ship, starsystem_from_vnum( ship->location ) );
    } 
    
    if ( ship->starsystem == NULL && !IS_SET(ship->flags, SHIPFLAG_SIMULATOR ))
    {
       echo_to_room( AT_YELLOW , get_room_index(ship->pilotseat) , "Launch path blocked .. Launch aborted.");
       echo_to_ship( AT_YELLOW , ship , "The ship slowly sets back back down on the landing pad.");
       sprintf( buf ,  "%s slowly sets back down." ,ship->name );
       echo_to_room( AT_YELLOW , get_room_index(ship->location) , buf ); 
       ship->shipstate = SHIP_DOCKED;
       return;
    }    
    
    if (ship->class == MIDSIZE_SHIP)
    {
       sound_to_room( get_room_index(ship->location) , "!!SOUND(falcon)" );
       sound_to_ship(ship , "!!SOUND(falcon)" );
    }
    else if (ship->type == SHIP_IMPERIAL )
    {
       sound_to_ship(ship , "!!SOUND(tie)" );  
       sound_to_room( get_room_index(ship->location) , "!!SOUND(tie)" );
    }
    else
    {
       sound_to_ship(ship , "!!SOUND(xwing)" );  
       sound_to_room( get_room_index(ship->location) , "!!SOUND(xwing)" );
    }
    
    extract_ship(ship);    

    ship->lastdoc = ship->location;
    ship->location = 0;
    
    if (ship->shipstate != SHIP_DISABLED)
       ship->shipstate = SHIP_READY;
    
    plusminus = number_range ( -1 , 2 );
    if (plusminus > 0 )
        ship->hx = 1;
    else
        ship->hx = -1;
    
    plusminus = number_range ( -1 , 2 );
    if (plusminus > 0 )
        ship->hy = 1;
    else
        ship->hy = -1;
        
    plusminus = number_range ( -1 , 2 );
    if (plusminus > 0 )
        ship->hz = 1;
    else
        ship->hz = -1;
    if ( IS_SET( ship->flags, SHIPFLAG_SIMULATOR ) )
    {
    	ship->vx = 1000 + number_range(1, 5000);
    	ship->vx = 1000 + number_range(1, 5000);
    	ship->vz = 1000 + number_range(1, 5000);
    }
    
    if (ship->lastdoc == ship->starsystem->doc1a ||
        ship->lastdoc == ship->starsystem->doc1b ||
        ship->lastdoc == ship->starsystem->doc1c )
    {
       ship->vx = ship->starsystem->p1x;
       ship->vy = ship->starsystem->p1y;
       ship->vz = ship->starsystem->p1z;
    }
    else if (ship->lastdoc == ship->starsystem->doc2a ||
             ship->lastdoc == ship->starsystem->doc2b ||
             ship->lastdoc == ship->starsystem->doc2c )
    {
       ship->vx = ship->starsystem->p2x;
       ship->vy = ship->starsystem->p2y;
       ship->vz = ship->starsystem->p2z;
    }
    else if (ship->lastdoc == ship->starsystem->doc3a ||
             ship->lastdoc == ship->starsystem->doc3b ||
             ship->lastdoc == ship->starsystem->doc3c )
    {
       ship->vx = ship->starsystem->p3x;
       ship->vy = ship->starsystem->p3y;
       ship->vz = ship->starsystem->p3z;
    }
    else 
    {
       for ( target = ship->starsystem->first_ship; target; target = target->next_in_starsystem )
       {       
          if (ship->lastdoc == target->hanger)
          {
             ship->vx = target->vx;
             ship->vy = target->vy;
             ship->vz = target->vz;
          }   
       }
    }
    
    ship->energy -= (100+100*ship->class);
         
    ship->vx += (ship->hx*ship->currspeed*2);
    ship->vy += (ship->hy*ship->currspeed*2);
    ship->vz += (ship->hz*ship->currspeed*2);
    
    echo_to_room( AT_GREEN , get_room_index(ship->location) , "Launch complete.\n\r");	
    echo_to_ship( AT_YELLOW , ship , "The ship leaves the platform far behind as it flies into space." );
    sprintf( buf ,"%s enters the starsystem at %.0f %.0f %.0f" , ship->name, ship->vx, ship->vy, ship->vz );
    echo_to_system( AT_YELLOW, ship, buf , NULL ); 
    sprintf( buf, "%s lifts off into space.", ship->name );
    echo_to_room( AT_YELLOW , get_room_index(ship->lastdoc) , buf );
                 
}

void do_land( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    int chance;
    SHIP_DATA *ship;
    SHIP_DATA *target;
    int vx, vy ,vz;
    
    strcpy( arg, argument );
    
    	        if ( (ship = ship_from_cockpit(ch->in_room->vnum)) == NULL )
    	        {
    	            send_to_char("&RYou must be in the cockpit of a ship to do that!\n\r",ch);
    	            return;
    	        }
    	        
    	        if ( ship->class > MOBILE_SUIT )
    	        {
    	            send_to_char("&RThis isn't a spacecraft!\n\r",ch);
    	            return;
    	        }
    	        
    	        if ( (ship = ship_from_pilotseat(ch->in_room->vnum)) == NULL )
    	        {
    	            send_to_char("&RYou need to be in the pilot seat!\n\r",ch);
    	            return;
    	        }
    	        
    	        if ( autofly(ship) )
    	        {
    	            send_to_char("&RYou'll have to turn off the ships autopilot first.\n\r",ch);
    	            return;
    	        }
    	        
                if  ( ship->class == SHIP_PLATFORM )
                {
                   send_to_char( "&RYou can't land platforms\n\r" , ch );
                   return;
                }   
    
    	        if (ship->class >= FRIGATE_SHIP && ship->class < MOBILE_SUIT)
    	        {
    	            send_to_char("&RCapital ships are to big to land. You'll have to take a shuttle.\n\r",ch);
		    return;
    	        }
    	        if (ship->shipstate == SHIP_DISABLED)
    	        {
    	            send_to_char("&RThe ships drive is disabled. Unable to land.\n\r",ch);
    	            return;
    	        }
                if( IS_SET(ship->flags, SHIPFLAG_CLOAKED))
                {
                    send_to_char("They won't acknowledge a cloaked craft, decloak first.\n\r", ch);
                    return;
                }
    	        if (ship->shipstate == SHIP_DOCKED)
    	        {
    	            send_to_char("&RThe ship is already docked!\n\r",ch);
    	            return;
    	        }

                if ((ship->shipstate2 == SHIP_DOCK) || (ship->shipstate2 == SHIP_DOCK_2))
                {
                    send_to_char("&RWait until after docking procedures are complete.\n\r", ch);
                    return;
                }
                if (ship->shipstate2 == SHIP_DOCK_3)
                {
                    send_to_char("&RDetach from the docked ship first.\r\n", ch);
                    return;
                }
    	                
               if (ship->shipstate == SHIP_HYPERSPACE)
               {
                  send_to_char("&RYou can only do that in realspace!\n\r",ch);
                  return;   
               }
               if(IS_SET(ship->flags, SHIPFLAG_AFTERBURNER))
                {
                    send_to_char("&RYou need to turn off your afterburners.\n\r", ch);
                    return;
                }
               if(IS_SET(ship->flags, SHIPFLAG_OVERDRIVENODE))
               {
                   send_to_char("&RNot while your overdrive node is active.\n\r", ch);
                   return;
               }
    	        if (ship->shipstate != SHIP_READY)
    	        {
    	            send_to_char("&RPlease wait until the ship has finished its current maneuver.\n\r",ch);
    	            return;
    	        }
    	        if ( ship->starsystem == NULL )
    	        {
    	            send_to_char("&RThere's nowhere to land around here!",ch);
    	            return;
    	        }
    	        
    	        if ( ship->energy < (25 + 25*ship->class) )
    	        {
    	           send_to_char("&RTheres not enough fuel!\n\r",ch);
    	           return;
    	        }
    	        
    	        if ( argument[0] == '\0' )
    	        {  
    	           set_char_color(  AT_CYAN, ch );
    	           ch_printf(ch, "%s" , "Land where?\n\r\n\rChoices: ");   	           
    	           
    	        if ( ship->starsystem->doc1a ) 
    	             ch_printf(ch, "%s (%s)  %d %d %d\n\r         " , 
    	                        ship->starsystem->location1a,
    	                        ship->starsystem->planet1,
    	                        ship->starsystem->p1x,
    	                        ship->starsystem->p1y,
    	                        ship->starsystem->p1z ); 
    	        if ( ship->starsystem->doc1b ) 
    	             ch_printf(ch, "%s (%s)  %d %d %d\n\r         " , 
    	                        ship->starsystem->location1b,
    	                        ship->starsystem->planet1,
    	                        ship->starsystem->p1x,
    	                        ship->starsystem->p1y,
    	                        ship->starsystem->p1z ); 
    	        if ( ship->starsystem->doc1c ) 
    	             ch_printf(ch, "%s (%s)  %d %d %d\n\r         " , 
    	                        ship->starsystem->location1c,
    	                        ship->starsystem->planet1,
    	                        ship->starsystem->p1x,
    	                        ship->starsystem->p1y,
    	                        ship->starsystem->p1z ); 
    	        if ( ship->starsystem->doc2a ) 
    	             ch_printf(ch, "%s (%s)  %d %d %d\n\r         " , 
    	                        ship->starsystem->location2a,
    	                        ship->starsystem->planet2,
    	                        ship->starsystem->p2x,
    	                        ship->starsystem->p2y,
    	                        ship->starsystem->p2z ); 
    	        if ( ship->starsystem->doc2b ) 
    	             ch_printf(ch, "%s (%s)  %d %d %d\n\r         " , 
    	                        ship->starsystem->location2b,
    	                        ship->starsystem->planet2,
    	                        ship->starsystem->p2x,
    	                        ship->starsystem->p2y,
    	                        ship->starsystem->p2z ); 
    	        if ( ship->starsystem->doc2c ) 
    	             ch_printf(ch, "%s (%s)  %d %d %d\n\r         " , 
    	                        ship->starsystem->location2c,
    	                        ship->starsystem->planet2,
    	                        ship->starsystem->p2x,
    	                        ship->starsystem->p2y,
    	                        ship->starsystem->p2z ); 
    	        if ( ship->starsystem->doc3a ) 
    	             ch_printf(ch, "%s (%s)  %d %d %d\n\r         " , 
    	                        ship->starsystem->location3a,
    	                        ship->starsystem->planet3,
    	                        ship->starsystem->p3x,
    	                        ship->starsystem->p3y,
    	                        ship->starsystem->p3z ); 
    	        if ( ship->starsystem->doc3b ) 
    	             ch_printf(ch, "%s (%s)  %d %d %d\n\r         " , 
    	                        ship->starsystem->location3b,
    	                        ship->starsystem->planet3,
    	                        ship->starsystem->p3x,
    	                        ship->starsystem->p3y,
    	                        ship->starsystem->p3z ); 
    	        if ( ship->starsystem->doc3c ) 
    	             ch_printf(ch, "%s (%s)  %d %d %d\n\r         " , 
    	                        ship->starsystem->location3c,
    	                        ship->starsystem->planet3,
    	                        ship->starsystem->p3x,
    	                        ship->starsystem->p3y,
    	                        ship->starsystem->p3z ); 
    	           for ( target = ship->starsystem->first_ship; target; target = target->next_in_starsystem )
                   {       
                        if ( target->hanger > 0 && target != ship) 
                           ch_printf(ch, "%s    %.0f %.0f %.0f\n\r         ", 
                           	target->name,
                           	target->vx,
                           	target->vy,
                           	target->vz);
                   }  
                   ch_printf(ch, "\n\rYour Coordinates: %.0f %.0f %.0f\n\r" , 
                             ship->vx , ship->vy, ship->vz);   
                   return;
    	        }
    	        
    	        if ( str_prefix(argument,ship->starsystem->location1a) &&
    	             str_prefix(argument,ship->starsystem->location2a) &&
    	             str_prefix(argument,ship->starsystem->location3a) &&
    	             str_prefix(argument,ship->starsystem->location1b) &&
    	             str_prefix(argument,ship->starsystem->location2b) &&
    	             str_prefix(argument,ship->starsystem->location3b) &&
    	             str_prefix(argument,ship->starsystem->location1c) &&
    	             str_prefix(argument,ship->starsystem->location2c) &&
    	             str_prefix(argument,ship->starsystem->location3c)
    	             )
    	        { 
    	            target = get_ship_here( argument , ship->starsystem );
    	            if ( target == NULL || IS_SET(target->flags, SHIPFLAG_CLOAKED) )
    	            {
    	                send_to_char("&RI don't see that here. Type land by itself for a list\n\r",ch);
    	                return;
    	            } 
    	            if ( target == ship )
    	            {
    	                send_to_char("&RYou can't land your ship inside itself!\n\r",ch);
    	                return;
    	            } 
    	            if ( ! target->hanger )
    	            {
    	                send_to_char("&RThat ship has no hangar for you to land in!\n\r",ch);
    	                return;
    	            }
    	            if ( ship->class == MIDSIZE_SHIP && target->class == MIDSIZE_SHIP )
    	            {
    	                send_to_char("&RThat ship is not big enough for your ship to land in!\n\r",ch);
    	                return;
    	            }
    	            if ( ! target->bayopen )
    	            {
    	                send_to_char("&RTheir hangar is closed. You'll have to ask them to open it for you\n\r",ch);
    	                return;
    	            }
    	            if (  (target->vx > ship->vx + 200) || (target->vx < ship->vx - 200) ||
    	                  (target->vy > ship->vy + 200) || (target->vy < ship->vy - 200) ||
    	                  (target->vz > ship->vz + 200) || (target->vz < ship->vz - 200) )
    	            {
    	                send_to_char("&R That ship is too far away! You'll have to fly a little closer.\n\r",ch);
    	                return;
    	            }       
    	        }
                else
                {
                    if ( !str_prefix(argument,ship->starsystem->location3a) || 
                         !str_prefix(argument,ship->starsystem->location3b) ||
                         !str_prefix(argument,ship->starsystem->location3c) )
                    {
                       vx = ship->starsystem->p3x;
                       vy = ship->starsystem->p3y;
                       vz = ship->starsystem->p3z;
                    }
    		    if ( !str_prefix(argument,ship->starsystem->location2a) || 
                         !str_prefix(argument,ship->starsystem->location2b) ||
                         !str_prefix(argument,ship->starsystem->location2c) )
                    {
                       vx = ship->starsystem->p2x;
                       vy = ship->starsystem->p2y;
                       vz = ship->starsystem->p2z;
                    }
    		    if ( !str_prefix(argument,ship->starsystem->location1a) || 
                         !str_prefix(argument,ship->starsystem->location1b) ||
                         !str_prefix(argument,ship->starsystem->location1c) )
                    {
                       vx = ship->starsystem->p1x;
                       vy = ship->starsystem->p1y;
                       vz = ship->starsystem->p1z;
                    }
    		    if (  (vx > ship->vx + 200) || (vx < ship->vx - 200) ||
    	                  (vy > ship->vy + 200) || (vy < ship->vy - 200) ||
    	                  (vz > ship->vz + 200) || (vz < ship->vz - 200) )
    	            {
    	                send_to_char("&R That platform is too far away! You'll have to fly a little closer.\n\r",ch);
    	                return;
    	            }                    
                }
                
                if ( ship->class == FIGHTER_SHIP ) 
                    chance = IS_NPC(ch) ? ch->top_level
	                 : (int)  (ch->pcdata->learned[gsn_starfighters]) ;
                if ( ship->class == MOBILE_SUIT ) 
                    chance = IS_NPC(ch) ? ch->top_level
	                 : (int)  (ch->pcdata->learned[gsn_starfighters]) ;
                if ( ship->class == MIDSIZE_SHIP )
                    chance = IS_NPC(ch) ? ch->top_level
	                 : (int)  (ch->pcdata->learned[gsn_midships]) ;
                if ( number_percent( ) < chance )
    		{
    		   set_char_color( AT_GREEN, ch );
    		   send_to_char( "Landing sequence initiated.\n\r", ch);
    		   act( AT_PLAIN, "$n begins the landing sequence.", ch,
		        NULL, argument , TO_ROOM );
		   echo_to_ship( AT_YELLOW , ship , "The ship slowly begins its landing aproach.");
    		   ship->dest = STRALLOC(arg);
    		   ship->shipstate = SHIP_LAND;
                   ship->shipstate2 = SHIP_READY;
    		   ship->currspeed = 0;
	           if ( ship->class == MOBILE_SUIT )
                      learn_from_success( ch, gsn_starfighters );
	           if ( ship->class == FIGHTER_SHIP )
                      learn_from_success( ch, gsn_starfighters );
                   if ( ship->class == MIDSIZE_SHIP )
                      learn_from_success( ch, gsn_midships );
                   if ( starsystem_from_vnum(ship->lastdoc) != ship->starsystem )
                   {   
                      int xp =  (exp_level( ch->skill_level[PILOTING_ABILITY]+1) - exp_level( ch->skill_level[PILOTING_ABILITY])) ;
                      xp = UMIN( get_ship_value( ship ) , xp );
                      gain_exp( ch , xp , PILOTING_ABILITY );
                      ch_printf( ch, "&WYou gain %ld points of flight experience!\n\r", UMIN( get_ship_value( ship ) , xp ) );
//                      ch_printf( ch, "&WYou would have gained %ld points of flight experience (but Garboth kept abusing the land bug)!\n\r", UMIN( get_ship_value( ship ) , xp ) );
                   }
                   return;
	        }
	        send_to_char("You fail to work the controls properly.\n\r",ch);
	        if ( ship->class == FIGHTER_SHIP )
                    learn_from_failure( ch, gsn_starfighters );
	        if ( ship->class == MOBILE_SUIT )
                    learn_from_failure( ch, gsn_starfighters );
	        if ( ship->class == MIDSIZE_SHIP )
    	            learn_from_failure( ch, gsn_midships );
    	   	return;	
}

void landship( SHIP_DATA *ship, char *arg )
{    
    SHIP_DATA *target;
    char buf[MAX_STRING_LENGTH];
    int destination;
    
    if ( !str_prefix(arg,ship->starsystem->location3a) )
       destination = ship->starsystem->doc3a;
    if ( !str_prefix(arg,ship->starsystem->location3b) )
       destination = ship->starsystem->doc3b;
    if ( !str_prefix(arg,ship->starsystem->location3c) )
       destination = ship->starsystem->doc3c;
    if ( !str_prefix(arg,ship->starsystem->location2a) )
       destination = ship->starsystem->doc2a;
    if ( !str_prefix(arg,ship->starsystem->location2b) )
       destination = ship->starsystem->doc2b;
    if ( !str_prefix(arg,ship->starsystem->location2c) )
       destination = ship->starsystem->doc2c;
    if ( !str_prefix(arg,ship->starsystem->location1a) )
       destination = ship->starsystem->doc1a;
    if ( !str_prefix(arg,ship->starsystem->location1b) )
       destination = ship->starsystem->doc1b;
    if ( !str_prefix(arg,ship->starsystem->location1c) )
       destination = ship->starsystem->doc1c;
    
    target = get_ship_here( arg , ship->starsystem );
    if ( target != ship && target != NULL && target->bayopen 
            && ( ship->class != MIDSIZE_SHIP || target->class != MIDSIZE_SHIP ) )
    destination = target->hanger;
     
    if ( !ship_to_room( ship , destination ) )
    {
       echo_to_room( AT_YELLOW , get_room_index(ship->pilotseat), "Could not complete aproach. Landing aborted.");
       echo_to_ship( AT_YELLOW , ship , "The ship pulls back up out of its landing sequence.");
       if (ship->shipstate != SHIP_DISABLED)
           ship->shipstate = SHIP_READY;
       return;
    }      
         
    echo_to_room( AT_YELLOW , get_room_index(ship->pilotseat), "Landing sequence complete.");
    echo_to_ship( AT_YELLOW , ship , "You feel a slight thud as the ship sets down on the ground."); 
    sprintf( buf ,"%s disapears from your scanner." , ship->name  );
    echo_to_system( AT_YELLOW, ship, buf , NULL );
    
    ship->location = destination;
    ship->lastdoc = ship->location;
    if (ship->shipstate != SHIP_DISABLED)
       ship->shipstate = SHIP_DOCKED;
    ship_from_starsystem(ship, ship->starsystem);
    
    sprintf( buf, "%s lands on the platform.", ship->name );
    echo_to_room( AT_YELLOW , get_room_index(ship->location) , buf );
    
    ship->energy = ship->energy - 25 - 25*ship->class;
    
    if ( !str_cmp("Public",ship->owner) )
    {
       ship->energy = ship->maxenergy;
       ship->chaff = ship->maxchaff;
       ship->missiles = ship->maxmissiles;
       ship->torpedos = ship->maxtorpedos;
       ship->rockets = ship->maxrockets;
       ship->shield = 0;
       ship->autorecharge = FALSE;
       ship->autotrack = FALSE;
       ship->autospeed = FALSE;
       ship->hull = ship->maxhull;
       
       ship->missilestate = MISSILE_READY;
       ship->statet0 = LASER_READY;
       ship->statet1 = LASER_READY;
       ship->ionstate = LASER_READY;
       ship->statet2 = LASER_READY;
       ship->shipstate = SHIP_DOCKED;
       
       echo_to_cockpit( AT_YELLOW , ship , "Repairing and refueling ship..." );
    }
    
       save_ship(ship);   
}

void do_accelerate( CHAR_DATA *ch, char *argument )
{
    int chance;
    int change;
    SHIP_DATA *ship;
    char buf[MAX_STRING_LENGTH];
    float fuelnegative = 1.0; /* for burn and overdrive, default is 1 */

    	        if (  (ship = ship_from_cockpit(ch->in_room->vnum))  == NULL )
    	        {
    	            send_to_char("&RYou must be in the cockpit of a ship to do that!\n\r",ch);
    	            return;
    	        }
                
                if ( ship->class > MOBILE_SUIT )
    	        {
    	            send_to_char("&RThis isn't a spacecraft!\n\r",ch);
    	            return;
    	        }
    	        
                if (  (ship = ship_from_pilotseat(ch->in_room->vnum))  == NULL )
    	        {
    	            send_to_char("&RThe controls must be at the pilots chair...\n\r",ch);
    	            return;
    	        }
                
                if ( autofly(ship) )
    	        {
    	            send_to_char("&RYou'll have to turn off the ships autopilot first.\n\r",ch);
    	            return;
    	        }
    	        
                if  ( ship->class == SHIP_PLATFORM )
                {
                   send_to_char( "&RPlatforms can't move!\n\r" , ch );
                   return;
                }   

                if (ship->shipstate == SHIP_HYPERSPACE)
                {
                  send_to_char("&RYou can only do that in realspace!\n\r",ch);
                  return;   
                }
                if (ship->shipstate == SHIP_DISABLED)
    	        {
    	            send_to_char("&RThe ships drive is disabled. Unable to accelerate.\n\r",ch);
    	            return;
    	        }
    	        if (ship->shipstate == SHIP_DOCKED)
    	        {
    	            send_to_char("&RYou can't do that until after you've launched!\n\r",ch);
    	            return;
    	        }

                if ((ship->shipstate2 == SHIP_DOCK) || (ship->shipstate2 == SHIP_DOCK_2))
                {
                    send_to_char("&RNot while docking procedures are going on.\n\r", ch);
                    return;
                }
                
                if (ship->shipstate2 == SHIP_DOCK_3)
                {
                    send_to_char("&RDetach from the docked ship first.\n\r",ch);
                    return;
                }

                if(IS_SET(ship->flags, SHIPFLAG_OVERDRIVENODE))
                {
                    fuelnegative = 2;
                }
                else if(IS_SET(ship->flags, SHIPFLAG_AFTERBURNER))
                {
                    fuelnegative = 1.5;
                }

                if(IS_SET(ship->flags,  SHIPFLAG_SABOTAGEDENGINE))
                {
                   echo_to_cockpit( AT_BLUE , ship , "There is a small explosion and then your ship stops moving. It must have been sabotage!\n\r");
                   echo_to_cockpit( AT_BLOOD + AT_BLINK , ship , "Ships Drive DAMAGED!");
                   ship->shipstate = SHIP_DISABLED;
                   ship->currspeed = 0;
                   send_to_char( "&GDecelerating\n\r", ch);
                   echo_to_cockpit( AT_YELLOW , ship , "The ship begins to slow down.");
                   sprintf( buf, "%s begins to slow down." , ship->name );
                   echo_to_system( AT_ORANGE , ship , buf , NULL );
                   REMOVE_BIT(ship->flags, SHIPFLAG_SABOTAGEDENGINE);
                   return;
                }    
	        if ( ship->energy < (int) abs((atoi(argument)-abs(ship->currspeed)*fuelnegative)/10) )
    	        {
    	           send_to_char("&RTheres not enough fuel!\n\r",ch);
    	           return;
    	        }
                                
    	        if ( ship->class == FIGHTER_SHIP )
                    chance = IS_NPC(ch) ? ch->top_level
	                 : (int)  (ch->pcdata->learned[gsn_starfighters]) ;
                if ( ship->class == MOBILE_SUIT )
                    chance = IS_NPC(ch) ? ch->top_level
	                 : (int)  (ch->pcdata->learned[gsn_starfighters]) ;
                if ( ship->class == MIDSIZE_SHIP )
                    chance = IS_NPC(ch) ? ch->top_level
	                 : (int)  (ch->pcdata->learned[gsn_midships]) ;
                if ( ship->class == FRIGATE_SHIP )
                    chance = IS_NPC(ch) ? ch->top_level
	                 : (int)  (ch->pcdata->learned[gsn_frigates]) ;
                if ( ship->class == CAPITAL_SHIP )
                    chance = IS_NPC(ch) ? ch->top_level
	                 : (int) (ch->pcdata->learned[gsn_capitalships]);
                if ( ship->class == SUPERCAPITAL_SHIP )
                    chance = IS_NPC(ch) ? ch->top_level
	                 : (int)  (ch->pcdata->learned[gsn_supercapitalships]) ;
                if ( number_percent( ) >= chance )
    		{
	           send_to_char("&RYou fail to work the controls properly.\n\r",ch);
	           if ( ship->class == FIGHTER_SHIP )
                      learn_from_failure( ch, gsn_starfighters );
	           if ( ship->class == MOBILE_SUIT )
                      learn_from_failure( ch, gsn_starfighters );
	           if ( ship->class == FRIGATE_SHIP )
                      learn_from_failure( ch, gsn_frigates );
	           if ( ship->class == SUPERCAPITAL_SHIP )
                      learn_from_failure( ch, gsn_supercapitalships );
                   if ( ship->class == MIDSIZE_SHIP )
    	              learn_from_failure( ch, gsn_midships );
                   if ( ship->class == CAPITAL_SHIP )
                      learn_from_failure( ch, gsn_capitalships );
    	   	   return;	
                }
                
    change = atoi(argument);
                      
    act( AT_PLAIN, "$n manipulates the ships controls.", ch,
    NULL, argument , TO_ROOM );
    
    if ( change > ship->currspeed )
    {
       send_to_char( "&GAccelerating\n\r", ch);
       echo_to_cockpit( AT_YELLOW , ship , "The ship begins to accelerate.");
       if(!IS_SET(ship->flags, SHIPFLAG_CLOAKED))
       {
          sprintf( buf, "%s begins to speed up." , ship->name );
          echo_to_system( AT_ORANGE , ship , buf , NULL );
       }
    }
    
    if ( change < ship->currspeed )
    {
       send_to_char( "&GDecelerating\n\r", ch);
       echo_to_cockpit( AT_YELLOW , ship , "The ship begins to slow down.");
       if(!IS_SET(ship->flags, SHIPFLAG_CLOAKED))
       {
          sprintf( buf, "%s begins to slow down." , ship->name );
          echo_to_system( AT_ORANGE , ship , buf , NULL );
       }
    }
    		     
    ship->energy -= (int) abs((change-abs(ship->currspeed)*fuelnegative)/10);
    
    ship->currspeed = URANGE( 0 , change , (int) ship->realspeed * speedbonus(ship));         
         
    if ( ship->class == FIGHTER_SHIP )
        learn_from_success( ch, gsn_starfighters );
    if ( ship->class == MOBILE_SUIT )
        learn_from_success( ch, gsn_starfighters );
    if ( ship->class == MIDSIZE_SHIP )
        learn_from_success( ch, gsn_midships );
    if ( ship->class == FRIGATE_SHIP )
        learn_from_success( ch, gsn_frigates );
    if ( ship->class == CAPITAL_SHIP )
        learn_from_success( ch, gsn_capitalships );
    if ( ship->class == SUPERCAPITAL_SHIP )
        learn_from_success( ch, gsn_supercapitalships );
    	
}

void do_trajectory( CHAR_DATA *ch, char *argument )
{
    char  buf[MAX_STRING_LENGTH];
    char  arg2[MAX_INPUT_LENGTH];
    char  arg3[MAX_INPUT_LENGTH];
    int chance;
    float vx,vy,vz;
    SHIP_DATA *ship;
    
  
    	        if (  (ship = ship_from_cockpit(ch->in_room->vnum))  == NULL )
    	        {
    	            send_to_char("&RYou must be in the cockpit of a ship to do that!\n\r",ch);
    	            return;
    	        }
                
                if ( ship->class > MOBILE_SUIT )
    	        {
    	            send_to_char("&RThis isn't a spacecraft!\n\r",ch);
    	            return;
    	        }
    	        
                if (  (ship = ship_from_pilotseat(ch->in_room->vnum))  == NULL )
    	        {
    	            send_to_char("&RYour not in the pilots seat.\n\r",ch);
    	            return;
    	        }
                
                if ( autofly(ship))
    	        {
    	            send_to_char("&RYou'll have to turn off the ships autopilot first.\n\r",ch);
    	            return;
    	        }
    	        
                if (ship->shipstate == SHIP_DISABLED)
    	        {
    	            send_to_char("&RThe ships drive is disabled. Unable to manuever.\n\r",ch);
    	            return;
    	        }
                if  ( ship->class == SHIP_PLATFORM )
                {
                   send_to_char( "&RPlatforms can't turn!\n\r" , ch );
                   return;
                }   

    	        if (ship->shipstate == SHIP_HYPERSPACE)
                {
                  send_to_char("&RYou can only do that in realspace!\n\r",ch);
                  return;   
                }
    	        if (ship->shipstate == SHIP_DOCKED)
    	        {
    	            send_to_char("&RYou can't do that until after you've launched!\n\r",ch);
    	            return;
    	        }
    	        if (ship->shipstate != SHIP_READY)
    	        {
    	            send_to_char("&RPlease wait until the ship has finished its current maneuver.\n\r",ch);
    	            return;
    	        }

                if(IS_SET(ship->flags,  SHIPFLAG_SABOTAGEDENGINE))
                {
                    echo_to_cockpit( AT_BLUE , ship , "There is a small explosion and then your ship stops moving. It must have been sabotage!\n\r");
                    echo_to_cockpit( AT_BLOOD + AT_BLINK , ship , "Ships Drive DAMAGED!");
                    ship->shipstate = SHIP_DISABLED;
                    ship->currspeed = 0;
                    REMOVE_BIT(ship->flags, SHIPFLAG_SABOTAGEDENGINE);
                }
    	        if ( ship->energy < (ship->currspeed/10) )
    	        {
    	           send_to_char("&RTheres not enough fuel!\n\r",ch);
    	           return;
    	        }
    	        
                if ( ship->class == FIGHTER_SHIP )
                    chance = IS_NPC(ch) ? ch->top_level
	                 : (int)  (ch->pcdata->learned[gsn_starfighters]) ;
                if ( ship->class == MOBILE_SUIT )
                    chance = IS_NPC(ch) ? ch->top_level
	                 : (int)  (ch->pcdata->learned[gsn_starfighters]) ;
                if ( ship->class == MIDSIZE_SHIP )
                    chance = IS_NPC(ch) ? ch->top_level
	                 : (int)  (ch->pcdata->learned[gsn_midships]) ;
                if ( ship->class == FRIGATE_SHIP )
                    chance = IS_NPC(ch) ? ch->top_level
	                 : (int)  (ch->pcdata->learned[gsn_frigates]) ;
                if ( ship->class == CAPITAL_SHIP )
                    chance = IS_NPC(ch) ? ch->top_level
	                 : (int) (ch->pcdata->learned[gsn_capitalships]);
                if ( ship->class == SUPERCAPITAL_SHIP )
                    chance = IS_NPC(ch) ? ch->top_level
	                 : (int) (ch->pcdata->learned[gsn_supercapitalships]);
                if ( number_percent( ) > chance )
    		{ 
	        send_to_char("&RYou fail to work the controls properly.\n\r",ch);
	        if ( ship->class == FIGHTER_SHIP )
                    learn_from_failure( ch, gsn_starfighters );
	        if ( ship->class == MOBILE_SUIT )
                    learn_from_failure( ch, gsn_starfighters );
                if ( ship->class == MIDSIZE_SHIP )
    	            learn_from_failure( ch, gsn_midships );
                if ( ship->class == FRIGATE_SHIP )
    	            learn_from_failure( ch, gsn_frigates );
                if ( ship->class == CAPITAL_SHIP )
                    learn_from_failure( ch, gsn_capitalships );
                if ( ship->class == SUPERCAPITAL_SHIP )
                    learn_from_failure( ch, gsn_supercapitalships );
    	   	return;	
    	        }
    	
    argument = one_argument( argument, arg2 );
    argument = one_argument( argument, arg3 );
            
    vx = atof( arg2 );
    vy = atof( arg3 );
    vz = atof( argument );
            
    if ( vx == ship->vx && vy == ship->vy && vz == ship->vz )
    {
       ch_printf( ch , "The ship is already at %.0f %.0f %.0f !" ,vx,vy,vz);
    }
                
    ship->hx = vx - ship->vx;
    ship->hy = vy - ship->vy;
    ship->hz = vz - ship->vz;
    
    ship->energy -= (ship->currspeed/10);
       
    ch_printf( ch ,"&GNew course set, aproaching %.0f %.0f %.0f.\n\r" , vx,vy,vz );            
    act( AT_PLAIN, "$n manipulates the ships controls.", ch, NULL, argument , TO_ROOM );
                         
    echo_to_cockpit( AT_YELLOW ,ship, "The ship begins to turn.\n\r" );                        
    if(!IS_SET(ship->flags, SHIPFLAG_CLOAKED))
    {
       sprintf( buf, "%s turns altering its present course." , ship->name );
       echo_to_system( AT_ORANGE , ship , buf , NULL );
    }                                                        
    if ( ship->class == FIGHTER_SHIP || ship->class == MOBILE_SUIT || ( ship->class == MIDSIZE_SHIP && ship->manuever > 50 ) )
        ship->shipstate = SHIP_BUSY_3;
    else if ( ship->class == MIDSIZE_SHIP || ( ship->class == CAPITAL_SHIP && ship->manuever > 50 ) )
        ship->shipstate = SHIP_BUSY_2;
    else
        ship->shipstate = SHIP_BUSY;     
   
    if ( ship->class == FIGHTER_SHIP )
        learn_from_success( ch, gsn_starfighters );
    if ( ship->class == MOBILE_SUIT )
        learn_from_success( ch, gsn_starfighters );
    if ( ship->class == MIDSIZE_SHIP )
        learn_from_success( ch, gsn_midships );
    if ( ship->class == FRIGATE_SHIP )
        learn_from_success( ch, gsn_frigates );
    if ( ship->class == CAPITAL_SHIP )
        learn_from_success( ch, gsn_capitalships );
    if ( ship->class == SUPERCAPITAL_SHIP )
        learn_from_success( ch, gsn_supercapitalships );
    	
}


void do_buyship(CHAR_DATA *ch, char *argument )
{
    long         price;
    SHIP_DATA   *ship;

   if ( IS_NPC(ch) || !ch->pcdata )
   {
   	send_to_char( "&ROnly players can do that!\n\r" ,ch );
   	return;
   }

   ship = ship_in_room( ch->in_room , argument );
   if ( !ship )
   {
      ship = ship_from_cockpit( ch->in_room->vnum );
      
      if ( !ship )
      {            
           act( AT_PLAIN, "I see no $T here.", ch, NULL, argument, TO_CHAR );
           return;
      }
   }
   
   if ( str_cmp( ship->owner , "" )  || ship->type == MOB_SHIP )
   {
   	send_to_char( "&RThat ship isn't for sale!" ,ch );
   	return;
   }


   if ( ship->type == SHIP_IMPERIAL )
   {
        if ( !ch->pcdata->clan || str_cmp( ch->pcdata->clan->name , "The Empire" ) )
        {
   	 if ( !ch->pcdata->clan || !ch->pcdata->clan->mainclan || str_cmp( ch->pcdata->clan->mainclan->name , "The Empire" ) )
   	 {
           send_to_char( "&RThat ship may only be purchaced by members of The Empire!\n\r" ,ch );
   	   return;
   	 }
   	}
   }
   else if ( ship->type == SHIP_REPUBLIC )
   {
        if ( !ch->pcdata->clan || str_cmp( ch->pcdata->clan->name , "New Republic" ) )
        {
         if ( !ch->pcdata->clan ||  !ch->pcdata->clan->mainclan || str_cmp( ch->pcdata->clan->mainclan->name , "New Republic" ) ) 
   	 {
   	  send_to_char( "&RThat ship may only be purchaced by members of the Republic!\n\r" ,ch );
   	  return;
         }
   	}
   }

   else if ( ship->type == SHIP_VONG )
   {
        if ( !ch->pcdata->clan || str_cmp( ch->pcdata->clan->name , "Yuuzhan Vong" ) )
        {
         if ( !ch->pcdata->clan ||  !ch->pcdata->clan->mainclan || str_cmp( ch->pcdata->clan->mainclan->name , "Yuuzhan Vong" ) ) 
   	 {
   	  send_to_char( "&RThat ship may only be purchaced by members of the Vong!\n\r" ,ch );
   	  return;
         }
   	}
   }

/************

Commented out in favour of allowing clanmembers to purchase all ships, not just clan ones
-Raijen, 9/14/01

   else 
   {
       if ( ch->pcdata->clan && 
       ( !str_cmp( ch->pcdata->clan->name , "Rebellion" ) || 
       ( ch->pcdata->clan->mainclan && !str_cmp(ch->pcdata->clan->mainclan->name ,"Rebellion") )
       ) )
       {
   	  send_to_char( "&RAs a member of the Rebellion you may only purchase Rebel!\n\r" ,ch );
   	  return;       
       }        
       if ( ch->pcdata->clan && 
       ( !str_cmp( ch->pcdata->clan->name , "The-Empire" ) || 
       ( ch->pcdata->clan->mainclan && !str_cmp(ch->pcdata->clan->mainclan->name ,"The-Empire") )
       ) )
       {
   	  send_to_char( "&RAs a member of The Empire you may only purchase Imperial Ships!\n\r" ,ch );
   	  return;       
       }        
   }

**************/
      
   price = get_ship_value( ship );
    
    if ( ch->gold < price )
    {
       ch_printf(ch, "&RThis ship costs %ld. You don't have enough credits!\n\r" , price );
       return;
    }
    
    ch->gold -= price;
    ch_printf(ch, "&GYou pay %ld credits to purchace the ship.\n\r" , price );   

    act( AT_PLAIN, "$n walks over to a terminal and makes a credit transaction.",ch,
       NULL, argument , TO_ROOM );
    
    	STRFREE( ship->owner );
	ship->owner = STRALLOC( ch->name );
	save_ship( ship );
                 
}

void do_clanbuyship(CHAR_DATA *ch, char *argument )
{
    long         price;
    SHIP_DATA   *ship;
    CLAN_DATA   *clan;
    CLAN_DATA   *mainclan;
       
   if ( IS_NPC(ch) || !ch->pcdata )
   {
   	send_to_char( "&ROnly players can do that!\n\r" ,ch );
   	return;
   }
   if ( !ch->pcdata->clan )
   {
   	send_to_char( "&RYou aren't a member of any organizations!\n\r" ,ch );
   	return;
   }
   
   clan = ch->pcdata->clan;
   mainclan = ch->pcdata->clan->mainclan ? ch->pcdata->clan->mainclan : clan;
   
   if ( ( ch->pcdata->bestowments
    &&    is_name("clanbuyship", ch->pcdata->bestowments))
    ||   !str_cmp( ch->name, clan->leader  ))
	;
   else
   {
   	send_to_char( "&RYour organization hasn't seen fit to bestow you with that ability.\n\r" ,ch );
   	return;
   }
	
   ship = ship_in_room( ch->in_room , argument );
   if ( !ship )
   {
      ship = ship_from_cockpit( ch->in_room->vnum );
      
      if ( !ship )
      {            
           act( AT_PLAIN, "I see no $T here.", ch, NULL, argument, TO_CHAR );
           return;
      }
   }

   if ( str_cmp( ship->owner , "" )  || ship->type == MOB_SHIP )
   {
   	send_to_char( "&RThat ship isn't for sale!\n\r" ,ch );
   	return;
   }
   
   if ( str_cmp( mainclan->name , "The Empire" )  && ship->type == SHIP_IMPERIAL ) 
   {
   	   send_to_char( "&RThat ship may only be purchaced by the Empire!\n\r" ,ch );
   	   return;
   }
   
   if ( str_cmp( mainclan->name , "Yuuzhan Vong" )  && ship->type == SHIP_VONG )
   {
   	send_to_char( "&RThat ship may only be purchaced by the Yuuzhan Vong!\n\r" ,ch );
   	return;
   }

  if ( str_cmp( mainclan->name , "New Republic" )  && ship->type == SHIP_REPUBLIC )
   {
   	send_to_char( "&RThat ship may only be purchaced by The New Republic!\n\r" ,ch );
   	return;
   }
   
/***********

   if ( !str_cmp( mainclan->name , "The-Empire" )  && ship->type != SHIP_IMPERIAL )
   {
   	send_to_char( "&RDue to contractual agreements that ship may not be purchaced by the empire!\n\r" ,ch );
   	return;
   }
   
   if ( !str_cmp( mainclan->name , "Rebellion" )  && ship->type != SHIP_REPUBLIC )
   {
   	send_to_char( "&RBecause of contractual agreements, the NR can only purchase NR ships!\n\r" ,ch );
   	return;
   }
*******************/
   
   price = get_ship_value( ship );
    
    if ( ch->pcdata->clan->funds < price )
    {
       ch_printf(ch, "&RThis ship costs %ld. You don't have enough credits!\n\r" , price );
       return;
    }
    
    clan->funds -= price;
    ch_printf(ch, "&G%s pays %ld credits to purchace the ship.\n\r", clan->name , price );   

    act( AT_PLAIN, "$n walks over to a terminal and makes a credit transaction.",ch,
       NULL, argument , TO_ROOM );
    
    	STRFREE( ship->owner );
	ship->owner = STRALLOC( clan->name );
	save_ship( ship );
               
   if ( ship->class <= SHIP_PLATFORM )
             clan->spacecraft++;
   else
             clan->vehicles++;
}

void do_sellship(CHAR_DATA *ch, char *argument )
{
    long         price;
    SHIP_DATA   *ship;

   ship = ship_in_room( ch->in_room , argument );
   if ( !ship )
   {
            act( AT_PLAIN, "I see no $T here.", ch, NULL, argument, TO_CHAR );
           return;
   }

   if ( str_cmp( ship->owner , ch->name ) )
   {
   	send_to_char( "&RThat isn't your ship!" ,ch );
   	return;
   }
   
   price = get_ship_value( ship );
    
    ch->gold += ( price - price/10 );
    ch_printf(ch, "&GYou receive %ld credits from selling your ship.\n\r" , price - price/10 );   

    act( AT_PLAIN, "$n walks over to a terminal and makes a credit transaction.",ch,
       NULL, argument , TO_ROOM );
 
	STRFREE( ship->owner );
        STRFREE( ship->pilot );
        STRFREE( ship->copilot );
	ship->owner = STRALLOC( "" );
        ship->pilot = STRALLOC( "" );
        ship->copilot = STRALLOC( "" );
	save_ship( ship );

}

/* Clansellship by Arcturus */
void do_clansellship(CHAR_DATA *ch, char *argument )
{
    long         price;
    SHIP_DATA   *ship;
    CLAN_DATA   *clan;
    CLAN_DATA   *mainclan;

   ship = ship_in_room( ch->in_room , argument );
   if ( !ship )
   {
            act( AT_PLAIN, "I see no $T here.", ch, NULL, argument, TO_CHAR );
           return;
   }

   if ( !ch->pcdata->clan )
   {
        send_to_char( "&RYou aren't a member of any organizations!\n\r" ,ch );
        return;
   }
   clan = ch->pcdata->clan;
   mainclan = ch->pcdata->clan->mainclan ? ch->pcdata->clan->mainclan : clan;

   if ( str_cmp( ship->owner , clan->name ) )
   {
        send_to_char( "&RThat isn't your ship!" ,ch );
        return;
   }

   if ( ( ch->pcdata->bestowments
    &&    is_name("clansellship", ch->pcdata->bestowments))
    ||   !str_cmp( ch->name, clan->leader  ))
        ;
   else
   {
        send_to_char( "&RYour organization hasn't seen fit to bestow you with that ability.\n\r" ,ch );
        return;
   }
   price = get_ship_value( ship );

   clan->funds += price; 
   ch_printf(ch, "&GYour clan receives %ld credits from selling your ship.\n\r" , price - price/10 );

    act( AT_PLAIN, "$n walks over to a terminal and makes a credit transaction.",ch,
       NULL, argument , TO_ROOM );

        STRFREE( ship->owner );
        STRFREE( ship->pilot );
        STRFREE( ship->copilot );
        ship->owner = STRALLOC( "" );
        ship->pilot = STRALLOC( "" );
        ship->copilot = STRALLOC( "" );
        save_ship( ship );
   if ( ship->class <= SHIP_PLATFORM )
             clan->spacecraft--;
   else
             clan->vehicles--;

}

void do_info(CHAR_DATA *ch, char *argument )
{
    SHIP_DATA *ship;
    SHIP_DATA *target;
   
    if (  (ship = ship_from_cockpit(ch->in_room->vnum))  == NULL )
    {
            if ( argument[0] == '\0' )
            {
               act( AT_PLAIN, "Which ship do you want info on?.", ch, NULL, NULL, TO_CHAR );
               return;
            }
    
            ship = ship_in_room( ch->in_room , argument );
            if ( !ship )
            {
               act( AT_PLAIN, "I see no $T here.", ch, NULL, argument, TO_CHAR );
               return;
            }

            target = ship;
    }
    else if (argument[0] == '\0')
       target = ship;
    else
       target = get_ship_here( argument , ship->starsystem );
    	
    if ( target == NULL || (IS_SET(target->flags, SHIPFLAG_CLOAKED) && ship != target))
    {
         send_to_char("&RI don't see that here.\n\rTry the radar, or type info by itself for info on this ship.\n\r",ch);
         return;
    }          
    
    if ( abs(target->vx - ship->vx) > 100+ship->sensor*40 ||
         abs(target->vy - ship->vy) > 100+ship->sensor*40 ||
         abs(target->vz - ship->vz) > 100+ship->sensor*40 )  
    {
         send_to_char("&RThat ship is to far away to scan.\n\r",ch);
         return;
    }
    
    ch_printf( ch, "&Y%s %s : %s\n\r&B",
			target->type == SHIP_REPUBLIC ? "Republic" :
			(ship->type == SHIP_VONG ? "Yuuzhan Vong" : 
		       (target->type == SHIP_IMPERIAL ? "Imperial" : "Civilian" )),
		        target->class == FIGHTER_SHIP ? "Starfighter" :
		       (target->class == MIDSIZE_SHIP ? "Midtarget" : 
		       (target->class == CAPITAL_SHIP ? "Capital Ship" :
		       (target->class == SHIP_PLATFORM ? "Platform" : 
		       (target->class == CLOUD_CAR ? "Cloudcar" : 
		       (target->class == FRIGATE_SHIP ? "Frigate" : 
		       (target->class == LAND_SPEEDER ? "Speeder" : 
		       (target->class == SUPERCAPITAL_SHIP ? "SuperCapital" : 
		       (target->class == MOBILE_SUIT ? "Coralskipper" : 
		       (target->class == WALKER ? "Walker" : "Unknown" ) )  ) ) ) ) ) ) ), 
    			target->name,
    			target->filename);
    ch_printf( ch, "Description: %s\n\rOwner: %s   Pilot: %s   Copilot: %s\n\r",
    			target->description,
    			target->owner, target->pilot,  target->copilot );
    ch_printf( ch, "Laser cannons: %d ",
    			target->lasers);				
    ch_printf( ch, "Ion Cannons: %d ",
    			target->ions);				
    ch_printf( ch, "LaserPower: %d\n\r",
    			target->laserdamage);				
    ch_printf( ch, "Max Missiles: %d  Missile Launchers: %d\n\r",
       			target->maxmissiles, target->mlaunchers);		
    ch_printf( ch, "Max Torpedos: %d  Torpedo Launchers: %d\n\r",
                        target->maxtorpedos, target->tlaunchers);
    ch_printf( ch, "Max Rockets:  %d   Rocket Launchers: %d \n\r",
                        target->maxrockets,  target->rlaunchers);
   // ch_printf( ch, "Max Chaff: %d\n\r",
//       			target->maxchaff);		
    if (target->overdrive == 1)
    {
      send_to_char( "&YSLAM Overdrive System Installed!!!&B\n\r", ch);
    }
    if ( target->autocannon == 1)
    {
      ch_printf( ch, "&YAutocannon installed. %d shots at %d strength max&B\n\r",
    			target->autoammomax,
			target->autodamage );				
    }
    if ( target->interdict == 1)
    {
      send_to_char( "&YInterdiction System Installed!!!&B\n\r", ch );
    }
    if( IS_SET(target->flags, SHIPFLAG_SIMULATOR))
    {
      send_to_char("&YThis Craft is a Ship Simulator!!&B\n\r", ch);
    }
    if(ship->cloak == 1)
      send_to_char("&YThis craft has a Cloaking Device!!&B\n\r", ch);
    ch_printf( ch, "Max Hull: %d  ",
                        target->maxhull);
    ch_printf( ch, "Armor ( %d/%d )  Max Chaff: %d\n\r",
			target->armor,
			target->maxarmor, target->maxchaff);
    ch_printf( ch, "Max Shields: %d   Max Energy(fuel): %d\n\r",
                        target->maxshield,
    		        target->maxenergy);
//    ch_printf( ch, "Maximum Speed: %d   Hyperspeed: %d\n\r",                        target->realspeed, target->hyperspeed );                    
    ch_printf( ch, "Maximum Speed: %d   Hyperspeed: %d  Maximum Cargo: %d\n\r",
                        target->realspeed, target->hyperspeed,
                        target->maxcargo );
    act( AT_PLAIN, "$n checks various gages and displays on the control panel.", ch,
         NULL, argument , TO_ROOM );
	  
}

void do_autorecharge(CHAR_DATA *ch, char *argument )
{
    int chance;
    SHIP_DATA *ship;
    int recharge;
    
        
        if (  (ship = ship_from_cockpit(ch->in_room->vnum))  == NULL )
        {
            send_to_char("&RYou must be in the cockpit of a ship to do that!\n\r",ch);
            return;
        }
        
        if (  (ship = ship_from_coseat(ch->in_room->vnum))  == NULL )
        {
            send_to_char("&RYou must be in the co-pilots seat!\n\r",ch);
            return;
        }

	  if ( ship->interdictactive == TRUE )
        {
            send_to_char("&RYou'll have to disengage the Interdiction field first..\n\r",ch);
    	      return;
	  }

        
        if ( autofly(ship)  )
    	        {
    	            send_to_char("&RYou'll have to turn off the ships autopilot first.\n\r",ch);
    	            return;
    	        }
    	        
        chance = IS_NPC(ch) ? ch->top_level
             : (int)  (ch->pcdata->learned[gsn_shipsystems]) ;
        if ( number_percent( ) > chance )
        {
           send_to_char("&RYou fail to work the controls properly.\n\r",ch);
           learn_from_failure( ch, gsn_shipsystems );
           return;	
        }
    
    act( AT_PLAIN, "$n flips a switch on the control panel.", ch,
         NULL, argument , TO_ROOM );

    if ( !str_cmp(argument,"on" ) )
    {
        ship->autorecharge=TRUE;
        send_to_char( "&GYou power up the shields.\n\r", ch);
        echo_to_cockpit( AT_YELLOW , ship , "Shields ON. Autorecharge ON.");
    }
    else if ( !str_cmp(argument,"off" ) )
    {
        ship->autorecharge=FALSE;
        send_to_char( "&GYou shutdown the shields.\n\r", ch);
        echo_to_cockpit( AT_YELLOW , ship , "Shields OFF. Shield strength set to 0. Autorecharge OFF.");
        ship->shield = 0;
    }
    else if ( !str_cmp(argument,"idle" ) )
    {
        ship->autorecharge=FALSE;
        send_to_char( "&GYou let the shields idle.\n\r", ch);
        echo_to_cockpit( AT_YELLOW , ship , "Autorecharge OFF. Shields IDLEING.");
    }
    else
    {   
        if (ship->autorecharge == TRUE)
        {
           ship->autorecharge=FALSE;
           send_to_char( "&GYou toggle the shields.\n\r", ch);
           echo_to_cockpit( AT_YELLOW , ship , "Autorecharge OFF. Shields IDLEING.");
        }
        else
        {
           ship->autorecharge=TRUE;
           send_to_char( "&GYou toggle the shields.\n\r", ch);
           echo_to_cockpit( AT_YELLOW , ship , "Shields ON. Autorecharge ON");
        }   
    }
    
    if (ship->autorecharge)
    {
       recharge  = URANGE( 1, ship->maxshield-ship->shield, 25+ship->class*25 );
       recharge  = UMIN( recharge, ship->energy*5 + 100 );
       ship->shield += recharge;
       ship->energy -= ( recharge*2 + recharge * ship->class );
    }
                                          	  
    learn_from_success( ch, gsn_shipsystems );    	
}

void do_autopilot(CHAR_DATA *ch, char *argument )
{
    SHIP_DATA *ship;
        
        if (  (ship = ship_from_cockpit(ch->in_room->vnum))  == NULL )
        {
            send_to_char("&RYou must be in the cockpit of a ship to do that!\n\r",ch);
            return;
        }
        
        if (  (ship = ship_from_pilotseat(ch->in_room->vnum))  == NULL )
        {
            send_to_char("&RYou must be in the pilots seat!\n\r",ch);
            return;
        }
        
         if ( ! check_pilot(ch,ship) )
       	     {
       	       send_to_char("&RHey! Thats not your ship!\n\r",ch);
       	       return;
       	     }

         if ( ship->target0 || ship->target1 || ship->target2 )
       	     {
       	       send_to_char("&RNot while the ship is enganged with an enemy!\n\r",ch);
       	       return;
       	     }
      if (ship->shipstate == SHIP_DOCKED)
      {
          send_to_char("&RYou can't do that until after you've launched!\n\r",ch);
          return;
      }

        
    act( AT_PLAIN, "$n flips a switch on the control panel.", ch,
         NULL, argument , TO_ROOM );

        if (ship->autopilot == TRUE)
        {
           ship->autopilot=FALSE;
           send_to_char( "&GYou toggle the autopilot.\n\r", ch);
           echo_to_cockpit( AT_YELLOW , ship , "Autopilot OFF.");
        }
        else
        {
           ship->autopilot=TRUE;
           ship->autorecharge = TRUE;
           send_to_char( "&GYou toggle the autopilot.\n\r", ch);
           echo_to_cockpit( AT_YELLOW , ship , "Autopilot ON.");
        }   
    
}

void do_openhatch(CHAR_DATA *ch, char *argument )
{
   SHIP_DATA *ship;
   char buf[MAX_STRING_LENGTH];
   
   if ( !argument || argument[0] == '\0' || !str_cmp(argument,"hatch") )
   {
       ship = ship_from_entrance( ch->in_room->vnum );
       if( ship == NULL)
       {
          send_to_char( "&ROpen what?\n\r", ch );
          return;
       }
       else
       {
          if ( !ship->hatchopen)
       	  {
       	     
                if  ( ship->class == SHIP_PLATFORM )
                {
                   send_to_char( "&RTry one of the docking bays!\n\r" , ch );
                   return;
                }   
       	     if ( ship->location != ship->lastdoc ||
       	        ( ship->shipstate != SHIP_DOCKED && ship->shipstate != SHIP_DISABLED ) )
       	     {
       	       send_to_char("&RPlease wait till the ship lands!\n\r",ch);
       	       return;
       	     }
       	     ship->hatchopen = TRUE;
       	     send_to_char("&GYou open the hatch.\n\r",ch);
       	     act( AT_PLAIN, "$n opens the hatch.", ch, NULL, argument, TO_ROOM );
       	     sprintf( buf , "The hatch on %s opens." , ship->name);  
       	     echo_to_room( AT_YELLOW , get_room_index(ship->location) , buf );
       	       sound_to_room( get_room_index(ship->entrance) , "!!SOUND(door)" );
      		     sound_to_room( get_room_index(ship->location) , "!!SOUND(door)" );	
		return;
       	  }
       	  else
       	  {
       	     send_to_char("&RIt's already open.\n\r",ch);
       	     return;	
       	  }
       }
   }
   
   ship = ship_in_room( ch->in_room , argument );
   if ( !ship )
   {
            act( AT_PLAIN, "I see no $T here.", ch, NULL, argument, TO_CHAR );
           return;
   }

   if ( ship->shipstate != SHIP_DOCKED && ship->shipstate != SHIP_DISABLED )
   {
        send_to_char( "&RThat ship has already started to launch",ch);
        return;
   }
   
   if ( ! check_pilot(ch,ship) )
       	     {
       	       send_to_char("&RHey! Thats not your ship!\n\r",ch);
       	       return;
       	     }
       	     
   if ( !ship->hatchopen)
   {
   	ship->hatchopen = TRUE;
   	act( AT_PLAIN, "You open the hatch on $T.", ch, NULL, ship->name, TO_CHAR );
   	act( AT_PLAIN, "$n opens the hatch on $T.", ch, NULL, ship->name, TO_ROOM );
   	echo_to_room( AT_YELLOW , get_room_index(ship->entrance) , "The hatch opens from the outside." );
   	  sound_to_room( get_room_index(ship->entrance) , "!!SOUND(door)" );
      		     sound_to_room( get_room_index(ship->location) , "!!SOUND(door)" );		
	return;
   }

   send_to_char("&GIts already open!\n\r",ch);

}


void do_closehatch(CHAR_DATA *ch, char *argument )
{
   SHIP_DATA *ship;
   char buf[MAX_STRING_LENGTH];
   
   if ( !argument || argument[0] == '\0' || !str_cmp(argument,"hatch") )
   {
       ship = ship_from_entrance( ch->in_room->vnum );
       if( ship == NULL)
       {
          send_to_char( "&RClose what?\n\r", ch );
          return;
       }
       else
       {
          
                if  ( ship->class == SHIP_PLATFORM )
                {
                   send_to_char( "&RTry one of the docking bays!\n\r" , ch );
                   return;
                }   
          if ( ship->hatchopen)
       	  {
       	     ship->hatchopen = FALSE;
       	     send_to_char("&GYou close the hatch.\n\r",ch);
       	     act( AT_PLAIN, "$n closes the hatch.", ch, NULL, argument, TO_ROOM );  
       	     sprintf( buf , "The hatch on %s closes." , ship->name);  
       	     echo_to_room( AT_YELLOW , get_room_index(ship->location) , buf );
       	      sound_to_room( get_room_index(ship->entrance) , "!!SOUND(door)" );
      		     sound_to_room( get_room_index(ship->location) , "!!SOUND(door)" );		
 	     return;
       	  }
       	  else
       	  {
       	     send_to_char("&RIt's already closed.\n\r",ch);
       	     return;	
       	  }
       }
   }
   
   ship = ship_in_room( ch->in_room , argument );
   if ( !ship )
   {
            act( AT_PLAIN, "I see no $T here.", ch, NULL, argument, TO_CHAR );
           return;
   }

   if ( ship->shipstate != SHIP_DOCKED && ship->shipstate != SHIP_DISABLED )
   {
        send_to_char( "&RThat ship has already started to launch",ch);
        return;
   }
   else
   {
      if(ship->hatchopen)
      {
   	ship->hatchopen = FALSE;
   	act( AT_PLAIN, "You close the hatch on $T.", ch, NULL, ship->name, TO_CHAR );
   	act( AT_PLAIN, "$n closes the hatch on $T.", ch, NULL, ship->name, TO_ROOM );
        echo_to_room( AT_YELLOW , get_room_index(ship->entrance) , "The hatch is closed from outside.");
     sound_to_room( get_room_index(ship->entrance) , "!!SOUND(door)" );
      		     sound_to_room( get_room_index(ship->location) , "!!SOUND(door)" );		

   	return;
      }
      else
      {
      	send_to_char("&RIts already closed.\n\r",ch);
      	return;
      }
   }


}

void do_status(CHAR_DATA *ch, char *argument )
{
    int chance;
    SHIP_DATA *ship;
    SHIP_DATA *target;
   
    if (  (ship = ship_from_cockpit(ch->in_room->vnum))  == NULL )
    {
         send_to_char("&RYou must be in the cockpit, turret or engineroom of a ship to do that!\n\r",ch);
         return;
    }
    
    if (argument[0] == '\0')
       target = ship;
    else
       target = get_ship_here( argument , ship->starsystem );
    	
    if ( target == NULL || (IS_SET(target->flags, SHIPFLAG_CLOAKED) && target != ship))
    {
         send_to_char("&RI don't see that here.\n\rTry the radar, or type status by itself for your ships status.\n\r",ch);
         return;
    }          
    
    if ( abs(target->vx - ship->vx) > 100+ship->sensor*40 ||
         abs(target->vy - ship->vy) > 100+ship->sensor*40 ||
         abs(target->vz - ship->vz) > 100+ship->sensor*40 )  
    {
         send_to_char("&RThat ship is beyond your sensor range.\n\r",ch);
         return;
    }
    
    chance = IS_NPC(ch) ? ch->top_level
        : (int)  (ch->pcdata->learned[gsn_shipsystems]) ;
    if ( number_percent( ) > chance )
    {
        send_to_char("&RYou cant figure out what the readout means.\n\r",ch);
        learn_from_failure( ch, gsn_shipsystems );
        return;	
    }
        
    act( AT_PLAIN, "$n checks various gages and displays on the control panel.", ch,
         NULL, argument , TO_ROOM );
    
    ch_printf( ch, "&W%s:\n\r",target->name);
    ch_printf( ch, "&OCurrent Coordinates:&Y %.0f %.0f %.0f\n\r",
                        target->vx, target->vy, target->vz );
    ch_printf( ch, "&OCurrent Heading:&Y %.0f %.0f %.0f\n\r",
                        target->hx, target->hy, target->hz );
    ch_printf( ch, "&OCurrent Speed:&Y %d&O/%d\n\r",
                        target->currspeed , target->realspeed );                    
    ch_printf( ch, "&OHull:&Y %d&O/&Y%d&O  Armor &Y%d&O/&Y%d  &OShip Condition:&Y %s\n\r",
                        target->hull,
    		        target->maxhull, target->armor, target->maxarmor,	
    			target->shipstate == SHIP_DISABLED ? "Disabled" : "Running");    		
    ch_printf( ch, "&OShields:&Y %d&O/%d   Energy(fuel):&Y %d&O/%d\n\r",
                        target->shield,
    		        target->maxshield,
    		        target->energy,
    		        target->maxenergy);
    ch_printf( ch, "&OCargo: &Y%d/&O%d   Cargo Type: &Y%s&w\n\r",
                        ship->cargo,
                        ship->maxcargo,
                        cargo_names[ship->cargotype]);

    ch_printf( ch, "&OLasers:&Y %d  &OIons:&Y %d  Laser Power: %d\n\r",
                        target->lasers,
				target->ions,
                        target->laserdamage);
    ch_printf( ch, "&OLaser Condition:&Y %s  &OCurrent Target:&Y %s\n\r",
 		        target->statet0 == LASER_DAMAGED ? "Damaged" : "Good" , target->target0 ? target->target0->name : "none");		
    if(target->ions)
        ch_printf( ch, "&OIon Condition:&Y   %s\n\r", target->ionstate == LASER_DAMAGED ? "Damaged" : "Good");
    if (target->turret1)
        ch_printf( ch, "&OTurret One:&Y %s  &OCurrent Target:&Y %s\n\r",
    			target->statet1 == LASER_DAMAGED ? "Damaged" : "Good" , target->target1 ? target->target1->name : "none");		
    if (target->turret2)
        ch_printf( ch, "&OTurret Two:&Y %s  &OCurrent Target:&Y %s\n\r",
    			target->statet2 == LASER_DAMAGED ? "Damaged" : "Good" , target->target2 ? target->target2->name : "none");		
    if (target->autocannon == 1 )
    {
        ch_printf( ch, "&OAutocannon:&Y %d&O/&Y%d&O Rounds.  &Y%d&O Strength",
			target->autoammo,
			target->autoammomax,
			target->autodamage );
    }

    ch_printf( ch, "\n\r&OMissiles:&Y %d&O/%d   Launcher Condition:&Y %s&w",
       			target->missiles,
    			target->maxmissiles,
    			target->missilestate == MISSILE_DAMAGED ? "Damaged" : "Good");		
    ch_printf( ch, "\n\r&OTorpedos:&Y %d&O/%d   Launcher Condition:&Y %s&w",
                        target->torpedos,
                        target->maxtorpedos,
                        target->torpedostate == MISSILE_DAMAGED ? "Damaged" : "Good");
    ch_printf( ch, "\n\r&ORockets:&Y  %d&O/%d   Launcher Condition:&Y %s&w\n\r",
                        target->rockets,
                        target->maxrockets,
                        target->rocketstate == MISSILE_DAMAGED ? "Damaged" : "Good");
   
    if ( target->interdict == 1)
    {
      send_to_char( "&YInterdiction System Installed!!!&O\n\r", ch );
    }
    if ( target->overdrive == 1)
       send_to_char( "&YOverdrive Node Installed!!!&O\n\r", ch);
    learn_from_success( ch, gsn_shipsystems );
}

void do_hyperspace(CHAR_DATA *ch, char *argument )
{
    int chance;
    SHIP_DATA *ship;
    SHIP_DATA *eShip;
    char buf[MAX_STRING_LENGTH];
   
        if (  (ship = ship_from_cockpit(ch->in_room->vnum))  == NULL )
        {
            send_to_char("&RYou must be in the cockpit of a ship to do that!\n\r",ch);
            return;
        }
        
        if ( ship->class > MOBILE_SUIT )
    	        {
    	            send_to_char("&RThis isn't a spacecraft!\n\r",ch);
    	            return;
    	        }
    	        
        
        if (  (ship = ship_from_pilotseat(ch->in_room->vnum))  == NULL )
        {
            send_to_char("&RYou aren't in the pilots seat.\n\r",ch);
            return;
        }
        
        if ( autofly(ship)  )
    	        {
    	            send_to_char("&RYou'll have to turn off the ships autopilot first.\n\r",ch);
    	            return;
    	        }
    	        
        
                if  ( ship->class == SHIP_PLATFORM )
                {
                   send_to_char( "&RPlatforms can't move!\n\r" , ch );
                   return;
                }       
                if (ship->hyperspeed == 0)
                {
                  send_to_char("&RThis ship is not equipped with a hyperdrive!\n\r",ch);
                  return;   
                }
                if (ship->shipstate == SHIP_HYPERSPACE)
                {
                  send_to_char("&RYou are already travelling lightspeed!\n\r",ch);
                  return;   
                }
                if (ship->shipstate == SHIP_DISABLED)
    	        {
    	            send_to_char("&RThe ships drive is disabled. Unable to manuever.\n\r",ch);
    	            return;
    	        }
                if (IS_SET(ship->flags, SHIPFLAG_CLOAKED) )
                {
                    send_to_char("&RYou can't hyperspace while cloaked, it takes too much energy.\n\r", ch);
                    return; 
                }
    	        if (ship->shipstate == SHIP_DOCKED)
    	        {
    	            send_to_char("&RYou can't do that until after you've launched!\n\r",ch);
    	            return;
    	        }

                if ((ship->shipstate2 == SHIP_DOCK) || (ship->shipstate2 == SHIP_DOCK_2))
                {
                    send_to_char("&RNot while docking procedures are going on.\n\r", ch);
                    return;
                }
                
                if (ship->shipstate2 == SHIP_DOCK_3)
                {
                    send_to_char("&RDetach from the docked ship first.\n\r",ch);
                    return;
                }

    	        if (ship->shipstate != SHIP_READY)
    	        {
    	            send_to_char("&RPlease wait until the ship has finished its current maneuver.\n\r",ch);
    	            return;
    	        } 
                if (!ship->currjump)
    	        {
    	            send_to_char("&RYou need to calculate your jump first!\n\r",ch);
    	            return;
    	        } 

        if ( ship->energy < (200+ship->hyperdistance*(1+ship->class)/3))
        {
              send_to_char("&RTheres not enough fuel!\n\r",ch);
              return;
        }

        if ( ship->currspeed <= 0 )
        {
              send_to_char("&RYou need to speed up a little first!\n\r",ch);
              return;
        }
    	
    	for ( eShip = ship->starsystem->first_ship; eShip; eShip = eShip->next_in_starsystem )
    	{
    	   if ( eShip == ship )
    	      continue;
    	   if ( eShip->interdictactive == TRUE &&
		abs( eShip->vx - ship->vx ) < 5000 &&
		abs( eShip->vy - ship->vy ) < 5000 &&
		abs( eShip->vz - ship->vz ) < 5000 )
           {
              ch_printf(ch, "&RIt's impossible to jump to lightspeed while %s is generating a gravity well!!!\n\r", eShip->name );
              ch_printf(ch, "&RYou must force them to lower their field, or flee to a VERY FAR distance!!!\n\r", eShip->name );
              return;
           }    	   
    	   if ( abs( eShip->vx - ship->vx ) < 500 
    	   &&  abs( eShip->vy - ship->vy ) < 500 
    	   &&  abs( eShip->vz - ship->vz ) < 500 )
           {
              ch_printf(ch, "&RYou are too close to %s to make the jump to lightspeed.\n\r", eShip->name );
              return;
           }    	   
    	}
    	        
        if ( ship->class == FIGHTER_SHIP )
             chance = IS_NPC(ch) ? ch->top_level
             : (int)  (ch->pcdata->learned[gsn_starfighters]) ;
        if ( ship->class == MOBILE_SUIT )
             chance = IS_NPC(ch) ? ch->top_level
             : (int)  (ch->pcdata->learned[gsn_starfighters]) ;
        if ( ship->class == MIDSIZE_SHIP )
             chance = IS_NPC(ch) ? ch->top_level
                 : (int)  (ch->pcdata->learned[gsn_midships]) ;
        if ( ship->class == FRIGATE_SHIP )
             chance = IS_NPC(ch) ? ch->top_level
                 : (int)  (ch->pcdata->learned[gsn_frigates]) ;
        if ( ship->class == CAPITAL_SHIP )
              chance = IS_NPC(ch) ? ch->top_level
                 : (int) (ch->pcdata->learned[gsn_capitalships]);
        if ( ship->class == SUPERCAPITAL_SHIP )
              chance = IS_NPC(ch) ? ch->top_level
                 : (int) (ch->pcdata->learned[gsn_supercapitalships]);
        if ( number_percent( ) > chance )
        {
            send_to_char("&RYou can't figure out which lever to use.\n\r",ch);
            if ( ship->class == FIGHTER_SHIP )
               learn_from_failure( ch, gsn_starfighters );
            if ( ship->class == MOBILE_SUIT )
               learn_from_failure( ch, gsn_starfighters );
            if ( ship->class == MIDSIZE_SHIP )   
               learn_from_failure( ch, gsn_midships );
            if ( ship->class == FRIGATE_SHIP )   
               learn_from_failure( ch, gsn_frigates );
            if ( ship->class == CAPITAL_SHIP )
                learn_from_failure( ch, gsn_capitalships );
            if ( ship->class == SUPERCAPITAL_SHIP )
                learn_from_failure( ch, gsn_supercapitalships );
    	   return;	
        }
    sprintf( buf ,"%s disapears from your scanner." , ship->name );
    echo_to_system( AT_YELLOW, ship, buf , NULL );

    ship_from_starsystem( ship , ship->starsystem );
    ship->shipstate = SHIP_HYPERSPACE;
        
    send_to_char( "&GYou push forward the hyperspeed lever.\n\r", ch);
    act( AT_PLAIN, "$n pushes a lever forward on the control panel.", ch,
         NULL, argument , TO_ROOM );
    echo_to_ship( AT_YELLOW , ship , "The ship lurches slightly as it makes the jump to lightspeed." );     
    echo_to_cockpit( AT_YELLOW , ship , "The stars become streaks of light as you enter hyperspace.");	  
    
    ship->energy -= (100+ship->hyperdistance*(1+ship->class)/3);
    
    ship->vx = ship->jx;
    ship->vy = ship->jy;
    ship->vz = ship->jz;
    
    if ( ship->class == FIGHTER_SHIP )
        learn_from_success( ch, gsn_starfighters );
    if ( ship->class == MOBILE_SUIT )
        learn_from_success( ch, gsn_starfighters );
    if ( ship->class == MIDSIZE_SHIP )
        learn_from_success( ch, gsn_midships );
    if ( ship->class == FRIGATE_SHIP )
        learn_from_success( ch, gsn_frigates );
    if ( ship->class == CAPITAL_SHIP )
        learn_from_success( ch, gsn_capitalships );
    if ( ship->class == SUPERCAPITAL_SHIP )
        learn_from_success( ch, gsn_supercapitalships );
    	
}

    
void do_target(CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    int chance;
    SHIP_DATA *ship;
    SHIP_DATA *target;
    char buf[MAX_STRING_LENGTH];
    
    strcpy( arg, argument );    
    
    switch( ch->substate )
    { 
    	default:
    	        if (  (ship = ship_from_turret(ch->in_room->vnum))  == NULL )
    	        {
    	            send_to_char("&RYou must be in the gunners seat or turret of a ship to do that!\n\r",ch);
    	            return;
    	        }
                
                if ( ship->class > MOBILE_SUIT )
    	        {
    	            send_to_char("&RThis isn't a spacecraft!\n\r",ch);
    	            return;
    	        }
    	        
                if (ship->shipstate == SHIP_HYPERSPACE)
                {
                  send_to_char("&RYou can only do that in realspace!\n\r",ch);
                  return;   
                }              
    	        if (! ship->starsystem )
    	        {
    	            send_to_char("&RYou can't do that until you've finished launching!\n\r",ch);
    	            return;
    	        }
                
                if ( autofly(ship) )
    	        {
    	            send_to_char("&RYou'll have to turn off the ships autopilot first....\n\r",ch);
    	            return;
    	        }
    	            
                if (arg[0] == '\0')
    	        {
    	            send_to_char("&RYou need to specify a target!\n\r",ch);
    	            return;
    	        }
                                
                if ( !str_cmp( arg, "none") )
    	        {
    	            send_to_char("&GTarget set to none.\n\r",ch);
    	            if ( ch->in_room->vnum == ship->gunseat )
    	                   ship->target0 = NULL;
    	            if ( ch->in_room->vnum == ship->turret1 )
    	                   ship->target1 = NULL;
    	            if ( ch->in_room->vnum == ship->turret2 )
    	                   ship->target2 = NULL;
    	            return;
    	        }

                if(!str_cmp(arg, "self"))
                {
                   send_to_char("&RUnable to comply, your ship systems cannot lock onto yourself.\n\r", ch);
                   return;
                }

                target = get_ship_here( arg, ship->starsystem );
                if(target == NULL || IS_SET(target->flags, SHIPFLAG_CLOAKED))
                {
                    send_to_char("&RI don't see that ship here.\n\r", ch);
                    return;
                }
                if (  target == ship )
                {
                    send_to_char("&RYou can't target your own ship!\n\r",ch);
                    return;
                }
                
                if ( !str_cmp(target->owner, ship->owner) && str_cmp( target->owner , "" ) )
                {
                    send_to_char("&RWARNING!  TARGETING FRIENDLY SHIP!!!\n\r",ch);
//                    return;
                }
                
                if ( abs(ship->vx-target->vx) > 5000 ||
                     abs(ship->vy-target->vy) > 5000 ||
                     abs(ship->vz-target->vz) > 5000 )
                {
                    send_to_char("&RThat ship is too far away to target.\n\r",ch);
                    return;
                }
                
                chance = IS_NPC(ch) ? ch->top_level
	                 : (int)  (ch->pcdata->learned[gsn_weaponsystems]) ;
                if ( number_percent( ) < chance )
    		{
    		   send_to_char( "&GTracking target.\n\r", ch);
    		   act( AT_PLAIN, "$n makes some adjustments on the targeting computer.", ch,
		        NULL, argument , TO_ROOM );
    		   add_timer ( ch , TIMER_DO_FUN , 1 , do_target , 1 );
    		   ch->dest_buf = str_dup(arg);
    		   return;
	        }
	        send_to_char("&RYou fail to work the controls properly.\n\r",ch);
	        learn_from_failure( ch, gsn_weaponsystems );
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
    		if ( (ship = ship_from_cockpit(ch->in_room->vnum)) == NULL )
    		      return;    		                                   
    	        send_to_char("&RYour concentration is broken. You fail to lock onto your target.\n\r", ch);
    		return;
    }
    
    ch->substate = SUB_NONE;
    
    if ( (ship = ship_from_turret(ch->in_room->vnum)) == NULL )
    {  
       return;
    }
    
    target = get_ship_here( arg, ship->starsystem );
    if (  target == NULL || target == ship || IS_SET(target->flags, SHIPFLAG_CLOAKED) )
    {
           send_to_char("&RThe ship has left the starsytem. Targeting aborted.\n\r",ch);
           return;
    }
    
    if ( ch->in_room->vnum == ship->gunseat )
       ship->target0 = target;
    
    if ( ch->in_room->vnum == ship->turret1 )
       ship->target1 = target;
             
    if ( ch->in_room->vnum == ship->turret2 )
        ship->target2 = target;      
    
    send_to_char( "&GTarget Locked.\n\r", ch);
    if(!IS_SET(ship->flags, SHIPFLAG_CLOAKED))
    {
       sprintf( buf , "You are being targetted by %s." , ship->name);  
       echo_to_cockpit( AT_BLOOD , target , buf );
    }   	      
    sound_to_room( ch->in_room , "!!SOUND(targetlock)" );   	         
    learn_from_success( ch, gsn_weaponsystems );
    	
    if ( autofly(target) && !target->target0 && !IS_SET(ship->flags, SHIPFLAG_CLOAKED))
    {
       sprintf( buf , "You are being targetted by %s." , target->name);  
       echo_to_cockpit( AT_BLOOD , ship , buf );
       target->target0 = ship;
    }
}

void do_fire(CHAR_DATA *ch, char *argument )
{
    int autohits;
    int automiss;
    int chance;
    int bonusdamage = 0; /*Pilot subclass and overcharge code*/
    int linkshot = 0; /* Linked Laser code */
    int shots; /* Linked Laser Code */
    int exshots = 0; /* Redirection Code */
    int bonusrange = 0; /* Overcharge Code */
    SHIP_DATA *ship;
    SHIP_DATA *target;
    char buf[MAX_STRING_LENGTH];
    
        if (  (ship = ship_from_turret(ch->in_room->vnum))  == NULL )
        {
            send_to_char("&RYou must be in the gunners chair or turret of a ship to do that!\n\r",ch);
            return;
        }
        
        if ( ship->class > MOBILE_SUIT )
    	        {
    	            send_to_char("&RThis isn't a spacecraft!\n\r",ch);
    	            return;
    	        }
    	        
        if (ship->shipstate == SHIP_HYPERSPACE)
        {
             send_to_char("&RYou can only do that in realspace!\n\r",ch);
             return;   
        }
    	if (ship->starsystem == NULL)
    	{
    	     send_to_char("&RYou can't do that until after you've finished launching!\n\r",ch);
    	     return;
    	}
    	if ( ship->energy <5 )
        {
             send_to_char("&RTheres not enough energy left to fire!\n\r",ch);
             return;
        }   
  
                if ( autofly(ship) )
    	        {
    	            send_to_char("&RYou'll have to turn off the ships autopilot first.\n\r",ch);
    	            return;
    	        }
        if (IS_SET(ship->flags, SHIPFLAG_CLOAKED))
        {
           send_to_char("You'll have to decloak to engage.\n\r", ch);
           return;
        }
    	if ( ch->in_room->vnum == ship->gunseat && !str_prefix( argument , "lasers") )
    	{
    	
    	     if (ship->statet0 == LASER_DAMAGED)
    	     {
    	        send_to_char("&RThe ships main laser is damaged.\n\r",ch);
    	      	return;
    	     } 
             if(IS_SET(ship->flags, SHIPFLAG_DUALLASER))
                linkshot = 2;
             if(IS_SET(ship->flags, SHIPFLAG_TRILASER))
                linkshot = 3;
             if(IS_SET(ship->flags, SHIPFLAG_QUADLASER))
                linkshot = 4;
             if(linkshot == 0)
                linkshot = 1;
             if(IS_SET(ship->flags, SHIPFLAG_SHIELDRLASER))
                exshots += ship->lasers / 2;
             if(IS_SET(ship->flags, SHIPFLAG_ENGINERLASER))
                exshots += ship->lasers / 2;
             if(IS_SET(ship->flags, SHIPFLAG_LASERRENGINE))
                exshots -= ship->lasers / 2;
             if(IS_SET(ship->flags, SHIPFLAG_LASERRSHIELD))
                exshots -= ship->lasers /2; 
             if (ship->statet0 >= (ship->lasers - linkshot + 1 + exshots))
    	     {
    	     	send_to_char("&RThe lasers are still recharging.\n\r",ch);
    	     	return;
    	     }
    	     if (ship->target0 == NULL )
    	     {
    	     	send_to_char("&RYou need to choose a target first.\n\r",ch);
    	     	return;
    	     }    	    
    	     target = ship->target0;
    	     if (ship->target0->starsystem != ship->starsystem || IS_SET(target->flags, SHIPFLAG_CLOAKED))
    	     {
    	     	send_to_char("&RYour target seems to have left.\n\r",ch);
    	        ship->target0 = NULL;
    	     	return;
    	     } 
             if(ship->statet0 < 0)
             { /* For overcharging. */
                bonusdamage += 1;
                bonusrange = 200;
             }

             if ( abs(target->vx - ship->vx) >(1000 + bonusrange) ||
                  abs(target->vy - ship->vy) >(1000 + bonusrange) ||
                  abs(target->vz - ship->vz) >(1000 + bonusrange))
             {
                send_to_char("&RThat ship is out of laser range.\n\r",ch);
    	     	return;
             } 
             if ( ship->class < 2 && !is_facing( ship, target ) )
             {
                send_to_char("&RThe main laser can only fire forward. You'll need to turn your ship!\n\r",ch);
    	     	return;
             }
             for(shots = 0; shots < linkshot; shots++)
             {      
             if (ship->target0->starsystem != ship->starsystem)
             {
                send_to_char("&RYour target seems to have left.\n\r",ch);
                ship->target0 = NULL;
                return;
             }
             ship->statet0++; /* Arcturus Version */
             chance = IS_NPC(ch) ? ch->top_level
                      : (int) ( get_curr_dex(ch) + ch->pcdata->learned[gsn_spacecombat]/4
                           + ch->pcdata->learned[gsn_spacecombat2]/4 + ch->pcdata->learned[gsn_spacecombat3]/4 );
             chance += (ship->manuever - target->manuever) / 10;
             chance += (ship->currspeed - target->currspeed) / 10;
             chance -= ( abs(target->vx - ship->vx)/120 );
             chance -= ( abs(target->vy - ship->vy)/120 );
             chance -= ( abs(target->vz - ship->vz)/120 );
	     if ( ch->subclass == SUBCLASS_WFOCUS )
             {
		chance += 15;
                chance = URANGE( 15 , chance , 95 );
                bonusdamage += 3;
             }
             else
                chance = URANGE( 10, chance, 90);
             act( AT_PLAIN, "$n presses the fire button.", ch,
                  NULL, argument , TO_ROOM );
           /*Sabotage Bit by Arcturus */
           if(IS_SET(ship->flags, SHIPFLAG_SABOTAGEDLASERS))
           {
              echo_to_room( AT_BLUE , get_room_index(ship->gunseat) , "A small blast damages your lasers, it must be sabotage!\n\r");
              echo_to_room( AT_BLOOD + AT_BLINK , get_room_index(ship->gunseat) , "LASER DAMAGED!\n\r" );
              ship->statet0 = LASER_DAMAGED;
              REMOVE_BIT(ship->flags, SHIPFLAG_SABOTAGEDLASERS);
              return;
           }

             if(evaded(ship, target))
             {
                sprintf( buf , "Lasers that fire from %s at you are evaded." , ship->name);
                echo_to_cockpit( AT_ORANGE , target , buf );
                sprintf( buf , "The ships lasers fire at %s but they evade them." , target->name);
                echo_to_cockpit( AT_ORANGE , ship , buf );
                learn_from_failure( ch, gsn_spacecombat );
                learn_from_failure( ch, gsn_spacecombat2 );
                learn_from_failure( ch, gsn_spacecombat3 );
                sprintf( buf, "Laserfire from %s barely misses %s." , ship->name , target->name );
                echo_to_system( AT_ORANGE , ship , buf , target );
             }
             else if( number_percent( ) > chance )
             {  
                sprintf( buf , "Lasers fire from %s at you but miss." , ship->name);  
                echo_to_cockpit( AT_ORANGE , target , buf );           
                sprintf( buf , "The ships lasers fire at %s but miss." , target->name);  
                echo_to_cockpit( AT_ORANGE , ship , buf );           
                learn_from_failure( ch, gsn_spacecombat ); 
    	        learn_from_failure( ch, gsn_spacecombat2 ); 
    	        learn_from_failure( ch, gsn_spacecombat3 ); 
    	        sprintf( buf, "Laserfire from %s barely misses %s." , ship->name , target->name );
                echo_to_system( AT_ORANGE , ship , buf , target );
             } 
             else
             {
                sprintf( buf, "Laserfire from %s hits %s." , ship->name, target->name );
                echo_to_system( AT_ORANGE , ship , buf , target );
                sprintf( buf , "You are hit by lasers from %s!" , ship->name);  
                echo_to_cockpit( AT_BLOOD , target , buf );           
                sprintf( buf , "Your ships lasers hit %s!." , target->name);  
                echo_to_cockpit( AT_YELLOW , ship , buf );           
                learn_from_success( ch, gsn_spacecombat );
                learn_from_success( ch, gsn_spacecombat2 );
                learn_from_success( ch, gsn_spacecombat3 );
                echo_to_ship( AT_RED , target , "A small explosion vibrates through the ship." );           
                damage_ship_ch( target , (  (ship->laserdamage+bonusdamage)/2 ) , ( (ship->laserdamage+bonusdamage) ) , ch , ship);
             }
             if ( autofly(target) && target->target0 != ship )
             {
                target->target0 = ship; 
                sprintf( buf , "You are being targetted by %s." , target->name);  
                echo_to_cockpit( AT_BLOOD , ship , buf );
             }
        }     
             return;
    	}
        
        if ( ch->in_room->vnum == ship->gunseat && !str_prefix( argument , "missile") )
    	{
             if(IS_SET(ship->flags, SHIPFLAG_DUALMISSILE))
                 linkshot = 1;
             else
                 linkshot = 0;
    	     if (ship->missilestate == MISSILE_DAMAGED)
    	     {
    	        send_to_char("&RThe ships missile launchers are damaged.\n\r",ch);
    	      	return;
    	     } 
             if (ship->missiles <= 0+linkshot)
    	     {
    	     	send_to_char("&RYou have no missiles to fire!\n\r",ch);
    	        return;
    	     }
              /* Arcturus Missile Code */
    	     if (ship->missilestate >= ship->mlaunchers-linkshot  )
    	     {
    	     	send_to_char("&RThe missiles are still reloading.\n\r",ch);
    	     	return;
    	     }
    	     if (ship->target0 == NULL )
    	     {
    	     	send_to_char("&RYou need to choose a target first.\n\r",ch);
    	     	return;
    	     }    	    
    	     target = ship->target0;
             if (ship->target0->starsystem != ship->starsystem || IS_SET(target->flags, SHIPFLAG_CLOAKED))
    	     {
    	     	send_to_char("&RYour target seems to have left.\n\r",ch);
    	        ship->target0 = NULL; 
    	     	return;
    	     } 
             if ( abs(target->vx - ship->vx) >2500 ||
                  abs(target->vy - ship->vy) >2500 ||
                  abs(target->vz - ship->vz) >2500 )
             {
                send_to_char("&RThat ship is out of missile range.\n\r",ch);
    	     	return;
             } 
             if ( ship->class < 2 && !is_facing( ship, target ) )
             {
                send_to_char("&RMissiles can only fire in a forward. You'll need to turn your ship!\n\r",ch);
    	     	return;
             }      
/* Arcturus Version */
             chance = IS_NPC(ch) ? ch->top_level
                    : (int) ( get_curr_dex(ch) + ch->pcdata->learned[gsn_spacecombat]/4
                           + ch->pcdata->learned[gsn_spacecombat2]/4 + ch->pcdata->learned[gsn_spacecombat3]/4 );
             chance += (ship->manuever - target->manuever) / 10;
             chance += (ship->currspeed - target->currspeed) / 10;
             chance -= ( abs(target->vx - ship->vx)/250 );
             chance -= ( abs(target->vy - ship->vy)/250 );
             chance -= ( abs(target->vz - ship->vz)/250 );
             if ( ch->subclass == SUBCLASS_WFOCUS )
             {
                chance += 15;
                chance = URANGE( 20 , chance , 95 );
                bonusdamage = 1;
             }
             else
                chance = URANGE(20, chance, 80); 
             act( AT_PLAIN, "$n presses the fire button.", ch,
                  NULL, argument , TO_ROOM );
/*Sabotage bit by Arcturus */
    if(IS_SET(ship->flags, SHIPFLAG_SABOTAGEDLAUNCHERS))
    {
           echo_to_room( AT_BLUE , get_room_index(ship->gunseat) , "A small blast damages your missile launcher, it must be sabotage!\n\r");
           echo_to_room( AT_BLOOD + AT_BLINK , get_room_index(ship->gunseat) , "MISSILE DAMAGED!\n\r" );
           ship->missilestate = MISSILE_DAMAGED;
           REMOVE_BIT(ship->flags, SHIPFLAG_SABOTAGEDLAUNCHERS);
           return;
    }

             if ( number_percent( ) > chance )
             {
                send_to_char( "&RYou fail to lock onto your target!", ch );
    	        learn_from_failure( ch, gsn_spacecombat );
                learn_from_failure( ch, gsn_spacecombat2 );
                learn_from_failure( ch, gsn_spacecombat3 );
                return;	
             } 
             ship->missilestate += 3;
             new_missile( ship , target , ch , CONCUSSION_MISSILE + bonusdamage );
             ship->missiles-- ;
             act( AT_PLAIN, "$n presses the fire button.", ch,
                  NULL, argument , TO_ROOM );
             echo_to_cockpit( AT_YELLOW , ship , "Missile launched.");
             sprintf( buf , "Incoming missile from %s." , ship->name);  
             echo_to_cockpit( AT_BLOOD , target , buf );
             sprintf( buf, "%s fires a missile towards %s." , ship->name, target->name );
             echo_to_system( AT_ORANGE , ship , buf , target );
             if( IS_SET(ship->flags, SHIPFLAG_DUALMISSILE))
             {
                echo_to_cockpit(AT_YELLOW, ship, "Missile launched.");
                new_missile( ship, target, ch, CONCUSSION_MISSILE +bonusdamage );
                ship->missiles--;
                ship->missilestate += 3;
                sprintf( buf , "Incoming missile from %s." , ship->name);
                echo_to_cockpit( AT_BLOOD , target , buf );
                sprintf( buf, "%s fires a missile towards %s." , ship->name, target->name );
                echo_to_system( AT_ORANGE , ship , buf , target );
             }

             learn_from_success( ch, gsn_spacecombat );
             learn_from_success( ch, gsn_spacecombat2 );
             learn_from_success( ch, gsn_spacecombat3 );

             
             if ( autofly(target) && target->target0 != ship )
             {
                target->target0 = ship; 
                sprintf( buf , "You are being targetted by %s." , target->name);  
                echo_to_cockpit( AT_BLOOD , ship , buf );
             }
             
             return;
    	}
        if ( ch->in_room->vnum == ship->gunseat && !str_prefix( argument , "torpedo") )
    	{
             if(IS_SET(ship->flags, SHIPFLAG_DUALTORPEDO))
                 linkshot = 1;
             else
                 linkshot = 0;
    	     if (ship->torpedostate == MISSILE_DAMAGED)
    	     {
    	        send_to_char("&RThe ships torpedo launchers are damaged.\n\r",ch);
    	      	return;
    	     } 
             if (ship->torpedos <= 0+linkshot)
    	     {
    	     	send_to_char("&RYou have no torpedos to fire!\n\r",ch);
    	        return;
             }
             if (ship->torpedostate >= ship->tlaunchers-linkshot  )
             {
                send_to_char("&RThe torpedos are still reloading.\n\r",ch);
                return;
             }    	     
    	     if (ship->target0 == NULL )
    	     {
    	     	send_to_char("&RYou need to choose a target first.\n\r",ch);
    	     	return;
    	     }    	    
    	     target = ship->target0;
             if (ship->target0->starsystem != ship->starsystem || IS_SET(target->flags, SHIPFLAG_CLOAKED))
    	     {
    	     	send_to_char("&RYour target seems to have left.\n\r",ch);
    	        ship->target0 = NULL; 
    	     	return;
    	     } 
             if ( abs(target->vx - ship->vx) >2500 ||
                  abs(target->vy - ship->vy) >2500 ||
                  abs(target->vz - ship->vz) >2500 )
             {
                send_to_char("&RThat ship is out of torpedo range.\n\r",ch);
    	     	return;
             } 
             if ( ship->class < 2 && !is_facing( ship, target ) )
             {
                send_to_char("&RTorpedos can only fire in a forward direction. You'll need to turn your ship!\n\r",ch);
    	     	return;
             }      
/*Arcturus Version */
            chance = IS_NPC(ch) ? ch->top_level
                 : (int) ( get_curr_dex(ch) + ch->pcdata->learned[gsn_spacecombat]/4
                           + ch->pcdata->learned[gsn_spacecombat2]/4 + ch->pcdata->learned[gsn_spacecombat3]/4 );
             chance += (ship->manuever - target->manuever) / 10;
             chance += (ship->currspeed - target->currspeed) / 10;
             chance -= ( abs(target->vx - ship->vx)/250 );
             chance -= ( abs(target->vy - ship->vy)/250 );
             chance -= ( abs(target->vz - ship->vz)/250 );
             if ( ch->subclass == SUBCLASS_WFOCUS )
             {
                chance += 15;
                chance = URANGE( 20 , chance , 95 );
                bonusdamage = 1;
             }
             else
                chance = URANGE( 20, chance, 80 );
             act( AT_PLAIN, "$n presses the fire button.", ch,
                  NULL, argument , TO_ROOM );
/*Sabotage bit by Arcuturs */
    if(IS_SET(ship->flags, SHIPFLAG_SABOTAGEDTLAUNCHERS))
    {
           echo_to_room( AT_BLUE , get_room_index(ship->gunseat) , "A small blast damages your torpedo launcher, it must be sabotage!\n\r");
           echo_to_room( AT_BLOOD + AT_BLINK , get_room_index(ship->gunseat) , "TORPEDO DAMAGED!\n\r" );
           ship->missilestate = MISSILE_DAMAGED;
           REMOVE_BIT(ship->flags, SHIPFLAG_SABOTAGEDTLAUNCHERS);
           return;
    }

             if ( number_percent( ) > chance )
             {
                send_to_char( "&RYou fail to lock onto your target!", ch );
                learn_from_failure(ch, gsn_spacecombat);
                learn_from_failure(ch, gsn_spacecombat2);
                learn_from_failure(ch, gsn_spacecombat3);
    	        return;	
             } 
             new_missile( ship , target , ch , PROTON_TORPEDO +bonusdamage );
             ship->torpedos--;
             ship->torpedostate += 3;
             act( AT_PLAIN, "$n pre7sses the fire button.", ch,
                  NULL, argument , TO_ROOM );
             echo_to_cockpit( AT_YELLOW , ship , "Torpedo launched.");
             sprintf( buf , "Incoming torpedo from %s." , ship->name);  
             echo_to_cockpit( AT_BLOOD , target , buf );
             sprintf( buf, "%s fires a torpedo towards %s." , ship->name, target->name );
             echo_to_system( AT_ORANGE , ship , buf , target );
             if( IS_SET(ship->flags, SHIPFLAG_DUALTORPEDO))
             {          
                new_missile( ship , target , ch , PROTON_TORPEDO +bonusdamage );
                ship->torpedos--;
                ship->torpedostate += 3;
                echo_to_cockpit( AT_YELLOW , ship , "Torpedo launched.");
                sprintf( buf , "Incoming torpedo from %s." , ship->name);
                echo_to_cockpit( AT_BLOOD , target , buf );
                sprintf( buf, "%s fires a torpedo towards %s." , ship->name, target->name );
                echo_to_system( AT_ORANGE , ship , buf , target );
             }
             learn_from_success(ch, gsn_spacecombat);
             learn_from_success(ch, gsn_spacecombat2);
             learn_from_success(ch, gsn_spacecombat3);      
             if ( autofly(target) && target->target0 != ship )
             {
                target->target0 = ship; 
                sprintf( buf , "You are being targetted by %s." , target->name);  
                echo_to_cockpit( AT_BLOOD , ship , buf );
             }
             
             return;
    	}
    	
        if ( ch->in_room->vnum == ship->gunseat && !str_prefix( argument , "rocket") )
    	{
    	     if (ship->rocketstate == MISSILE_DAMAGED)
    	     {
    	        send_to_char("&RThe ships rocket launchers are damaged.\n\r",ch);
    	      	return;
    	     } 
             if(IS_SET(ship->flags, SHIPFLAG_DUALROCKET))
                 linkshot = 1;
             else
                 linkshot = 0;
             if (ship->rockets <= 0+linkshot)
    	     {
    	     	send_to_char("&RYou don't have enough rockets to fire!\n\r",ch);
    	        return;
    	     }
             if (ship->rocketstate >= ship->rlaunchers-linkshot  )
             {
                send_to_char("&RThe rockets are still reloading.\n\r",ch);
                return;
             }
     	     if (ship->target0 == NULL )
    	     {
    	     	send_to_char("&RYou need to choose a target first.\n\r",ch);
    	     	return;
    	     }    	    
    	     target = ship->target0;
             if (ship->target0->starsystem != ship->starsystem || IS_SET(target->flags, SHIPFLAG_CLOAKED))
    	     {
    	     	send_to_char("&RYour target seems to have left.\n\r",ch);
    	        ship->target0 = NULL; 
    	     	return;
    	     } 
             if ( abs(target->vx - ship->vx) >1500 ||
                  abs(target->vy - ship->vy) >1500 ||
                  abs(target->vz - ship->vz) >1500 )
             {
                send_to_char("&RThat ship is out of rocket range.\n\r",ch);
    	     	return;
             } 
             if ( ship->class < 2 && !is_facing( ship, target ) )
             {
                send_to_char("&RRockets can only fire forward. You'll need to turn your ship!\n\r",ch);
    	     	return;
             }      
/*Arcturus Version */
             chance = IS_NPC(ch) ? ch->top_level
                 : (int) ( get_curr_dex(ch) + ch->pcdata->learned[gsn_spacecombat]/4
                           + ch->pcdata->learned[gsn_spacecombat2]/4 + ch->pcdata->learned[gsn_spacecombat3]/4 );
             chance += (ship->manuever - target->manuever) / 10;
             chance += (ship->currspeed - target->currspeed) / 10;
             chance -= ( abs(target->vx - ship->vx)/150 );
             chance -= ( abs(target->vy - ship->vy)/150 );
             chance -= ( abs(target->vz - ship->vz)/150 );

             if ( ch->subclass == SUBCLASS_WFOCUS )
             {
                chance += 15;
                chance = URANGE( 20 , chance , 95 );
                bonusdamage = 1;
             }
             else
                chance = URANGE( 20, chance, 80 );
             act( AT_PLAIN, "$n presses the fire button.", ch,
                  NULL, argument , TO_ROOM );
/*Sabotage bit by Arcturus */
    if(IS_SET(ship->flags, SHIPFLAG_SABOTAGEDRLAUNCHERS))
    {
           echo_to_room( AT_BLUE , get_room_index(ship->gunseat) , "A small blast damages your rocket launcher, it must be sabotage!\n\r");
           echo_to_room( AT_BLOOD + AT_BLINK , get_room_index(ship->gunseat) , "ROCKET DAMAGED!\n\r" );
           ship->rocketstate = MISSILE_DAMAGED;
           REMOVE_BIT(ship->flags, SHIPFLAG_SABOTAGEDRLAUNCHERS);
           return;
    }

             if ( number_percent( ) > chance )
             {
                send_to_char( "&RYou fail to lock onto your target!", ch );
                learn_from_failure(ch, gsn_spacecombat);
                learn_from_failure(ch, gsn_spacecombat2);
                learn_from_failure(ch, gsn_spacecombat3);
    	        return;	
             } 
             new_missile( ship , target , ch , HEAVY_ROCKET+bonusdamage);
             ship->rockets-- ;
             ship->rocketstate += 3;
             act( AT_PLAIN, "$n presses the fire button.", ch,
                  NULL, argument , TO_ROOM );
             echo_to_cockpit( AT_YELLOW , ship , "Rocket launched.");
             sprintf( buf , "Incoming rocket from %s." , ship->name);  
             echo_to_cockpit( AT_BLOOD , target , buf );
             sprintf( buf, "%s fires a heavy rocket towards %s." , ship->name, target->name );
             echo_to_system( AT_ORANGE , ship , buf , target );
             if( IS_SET(ship->flags, SHIPFLAG_DUALROCKET))
             {          
                new_missile( ship , target , ch , HEAVY_ROCKET +bonusdamage );
                ship->rockets--;
                ship->rocketstate += 3;
                echo_to_cockpit( AT_YELLOW , ship , "Rocket launched.");
                sprintf( buf , "Incoming rocket from %s." , ship->name);
                echo_to_cockpit( AT_BLOOD , target , buf );
                sprintf( buf, "%s fires a rocket towards %s." , ship->name, target->name );
                echo_to_system( AT_ORANGE , ship , buf , target );
             }
             learn_from_success(ch, gsn_spacecombat);
             learn_from_success(ch, gsn_spacecombat2);
             learn_from_success(ch, gsn_spacecombat3);             
             if ( autofly(target) && target->target0 != ship )
             {
                target->target0 = ship; 
                sprintf( buf , "You are being targetted by %s." , target->name);  
                echo_to_cockpit( AT_BLOOD , ship , buf );
             }
             return;
    	}
        
        if ( ch->in_room->vnum == ship->turret1 && !str_prefix( argument , "lasers") )
    	{
    	     if (ship->statet1 == LASER_DAMAGED)
    	     {
    	        send_to_char("&RThe ships turret is damaged.\n\r",ch);
    	      	return;
    	     } 
             if (ship->statet1 > ship->class )
    	     {
    	     	send_to_char("&RThe turbolaser is recharging.\n\r",ch);
    	     	return;
    	     }
    	     if (ship->target1 == NULL )
    	     {
    	     	send_to_char("&RYou need to choose a target first.\n\r",ch);
    	     	return;
    	     }    	    
    	     target = ship->target1;
             if (ship->target1->starsystem != ship->starsystem || IS_SET(target->flags, SHIPFLAG_CLOAKED))
    	     {
    	     	send_to_char("&RYour target seems to have left.\n\r",ch);
    	        ship->target1 = NULL; 
    	     	return;
    	     } 
             if ( abs(target->vx - ship->vx) >1000 ||
                  abs(target->vy - ship->vy) >1000 ||
                  abs(target->vz - ship->vz) >1000 )
             {
                send_to_char("&RThat ship is out of laser range.\n\r",ch);
    	     	return;
             } 
             ship->statet1++;
/*Arcturus Version */
             chance = IS_NPC(ch) ? ch->top_level
                    : (int) ( get_curr_dex(ch) + ch->pcdata->learned[gsn_spacecombat]/4
                           + ch->pcdata->learned[gsn_spacecombat2]/4 + ch->pcdata->learned[gsn_spacecombat3]/4 );
             chance += (ship->manuever - target->manuever) / 10;
             chance += (ship->currspeed - target->currspeed) / 10;
             chance -= ( abs(target->vx - ship->vx)/120 );
             chance -= ( abs(target->vy - ship->vy)/120 );
             chance -= ( abs(target->vz - ship->vz)/120 );
             if ( ch->subclass == SUBCLASS_WFOCUS )
             {
                chance += 15;
                chance = URANGE( 15 , chance , 95 );
                bonusdamage = 3;
             }
             else
                chance = URANGE( 10, chance, 90 );
             act( AT_PLAIN, "$n presses the fire button.", ch,
                  NULL, argument , TO_ROOM );
/*Sabotage bit by Arcturus */
    if(IS_SET(ship->flags, SHIPFLAG_SABOTAGEDTURRET1))
    {
           echo_to_room( AT_BLUE , get_room_index(ship->turret1) , "A small blast damages your laster turret, it must be sabotage!\n\r");
           echo_to_room( AT_BLOOD + AT_BLINK , get_room_index(ship->turret1) , "TURRET DAMAGED!\n\r" );
           ship->statet1 = LASER_DAMAGED;
           REMOVE_BIT(ship->flags, SHIPFLAG_SABOTAGEDTURRET1);
           return;
    }
            if(evaded(ship, target))
            {
                sprintf( buf , "Turbolasers that fire from %s at you are evaded." , ship->name);
                echo_to_cockpit( AT_ORANGE , target , buf );
                sprintf( buf , "The ships turret fires at %s but they evade." , target->name);
                echo_to_cockpit( AT_ORANGE , ship , buf );
                learn_from_failure( ch, gsn_spacecombat );
                learn_from_failure( ch, gsn_spacecombat2 );
                learn_from_failure( ch, gsn_spacecombat3 );
                sprintf( buf, "Laserfire from %s barely misses %s." , ship->name , target->name );
                echo_to_system( AT_ORANGE , ship , buf , target );
            }
             else if ( number_percent( ) > chance )
             {  
                sprintf( buf , "Turbolasers fire from %s at you but miss." , ship->name);  
                echo_to_cockpit( AT_ORANGE , target , buf );           
                sprintf( buf , "Turbolasers fire from the ships turret at %s but miss." , target->name);  
                echo_to_cockpit( AT_ORANGE , ship , buf );           
                sprintf( buf, "%s fires at %s but misses." , ship->name, target->name );
                echo_to_system( AT_ORANGE , ship , buf , target );
                learn_from_failure( ch, gsn_spacecombat ); 
    	        learn_from_failure( ch, gsn_spacecombat2 ); 
    	        learn_from_failure( ch, gsn_spacecombat3 ); 
    	        return;	
             } 
             sprintf( buf, "Turboasers fire from %s, hitting %s." , ship->name, target->name );
             echo_to_system( AT_ORANGE , ship , buf , target );
             sprintf( buf , "You are hit by turbolasers from %s!" , ship->name);  
             echo_to_cockpit( AT_BLOOD , target , buf );           
             sprintf( buf , "Turbolasers fire from the turret, hitting %s!." , target->name);  
             echo_to_cockpit( AT_YELLOW , ship , buf );           
             learn_from_success( ch, gsn_spacecombat );
             learn_from_success( ch, gsn_spacecombat2 );
             learn_from_success( ch, gsn_spacecombat3 );
             echo_to_ship( AT_RED , target , "A small explosion vibrates through the ship." );           
             damage_ship_ch( target , (ship->laserdamage+bonusdamage)  , (ship->laserdamage+bonusdamage)*2 , ch , ship);
             
             if ( autofly(target) && target->target0 != ship )
             {
                target->target0 = ship; 
                sprintf( buf , "You are being targetted by %s." , target->name);  
                echo_to_cockpit( AT_BLOOD , ship , buf );
             }
             
             return;
    	}
        
        if ( ch->in_room->vnum == ship->turret2 && !str_prefix( argument , "lasers") )
    	{
    	     if (ship->statet2 == LASER_DAMAGED)
    	     {
    	        send_to_char("&RThe ships turret is damaged.\n\r",ch);
    	      	return;
    	     } 
             if (ship->statet2 > ship->class )
    	     {
    	     	send_to_char("&RThe turbolaser is still recharging.\n\r",ch);
    	     	return;
    	     }
    	     if (ship->target2 == NULL )
    	     {
    	     	send_to_char("&RYou need to choose a target first.\n\r",ch);
    	     	return;
    	     }    	    
    	     target = ship->target2;
             if (ship->target2->starsystem != ship->starsystem || IS_SET(target->flags, SHIPFLAG_CLOAKED))
    	     {
    	     	send_to_char("&RYour target seems to have left.\n\r",ch);
    	        ship->target2 = NULL; 
    	     	return;
    	     } 
    	     if ( abs(target->vx - ship->vx) >1000 ||
                  abs(target->vy - ship->vy) >1000 ||
                  abs(target->vz - ship->vz) >1000 )
             {
                send_to_char("&RThat ship is out of laser range.\n\r",ch);
    	     	return;
             } 
             ship->statet2++;
/*Arcturus Version */
             chance = IS_NPC(ch) ? ch->top_level
                    : (int) ( get_curr_dex(ch) + ch->pcdata->learned[gsn_spacecombat]/4
                           + ch->pcdata->learned[gsn_spacecombat2]/4 + ch->pcdata->learned[gsn_spacecombat3]/4 );
             chance += (ship->manuever - target->manuever) / 10;
             chance += (ship->currspeed - target->currspeed) / 10;
             chance -= ( abs(target->vx - ship->vx)/120 );
             chance -= ( abs(target->vy - ship->vy)/120 );
             chance -= ( abs(target->vz - ship->vz)/120 );
             if ( ch->subclass == SUBCLASS_WFOCUS )
             {
                chance += 15;
                chance = URANGE( 15 , chance , 95 );
                bonusdamage = 3;
             }
             else
               chance = URANGE(10, chance, 90);

             act( AT_PLAIN, "$n presses the fire button.", ch,
                  NULL, argument , TO_ROOM );
    /*Sabotage bit by Arcturus */
    if(IS_SET(ship->flags, SHIPFLAG_SABOTAGEDTURRET2))
    {
           echo_to_room( AT_BLUE , get_room_index(ship->turret2) , "A small blast damages your laster turret, it must be sabotage!\n\r");
           echo_to_room( AT_BLOOD + AT_BLINK , get_room_index(ship->turret2) , "TURRET DAMAGED!\n\r" );
           ship->statet2 = LASER_DAMAGED;
           REMOVE_BIT(ship->flags, SHIPFLAG_SABOTAGEDTURRET2);
           return;
    }
             if(evaded(ship, target))
             {
                sprintf( buf , "Turbolasers that fire from %s at you are evaded." , ship->name);
                echo_to_cockpit( AT_ORANGE , target , buf );
                sprintf( buf , "The ships turret fires at %s but they evade." , target->name);
                echo_to_cockpit( AT_ORANGE , ship , buf );
                learn_from_failure( ch, gsn_spacecombat );
                learn_from_failure( ch, gsn_spacecombat2 );
                learn_from_failure( ch, gsn_spacecombat3 );
                sprintf( buf, "Laserfire from %s barely misses %s." , ship->name , target->name );
                echo_to_system( AT_ORANGE , ship , buf , target );
             }
             else if ( number_percent( ) > chance )
             {  
                sprintf( buf, "Turbolasers fire from %s barely missing %s." , ship->name, target->name );
                echo_to_system( AT_ORANGE , ship , buf , target );
                sprintf( buf , "Turbolasers fire from %s at you but miss." , ship->name);  
                echo_to_cockpit( AT_ORANGE , target , buf );           
                sprintf( buf , "Turbolasers fire from the turret missing %s." , target->name);  
                echo_to_cockpit( AT_ORANGE , ship , buf );           
                learn_from_failure( ch, gsn_spacecombat ); 
    	        learn_from_failure( ch, gsn_spacecombat2 ); 
    	        learn_from_failure( ch, gsn_spacecombat3 ); 
    	        return;	
             } 
             sprintf( buf, "Turbolasers fire from %s, hitting %s." , ship->name, target->name );
             echo_to_system( AT_ORANGE , ship , buf , target );
             sprintf( buf , "You are hit by turbolasers from %s!" , ship->name);  
             echo_to_cockpit( AT_BLOOD , target , buf );           
             sprintf( buf , "turbolasers fire from the turret hitting %s!." , target->name);  
             echo_to_cockpit( AT_YELLOW , ship , buf );           
             learn_from_success( ch, gsn_spacecombat );
             learn_from_success( ch, gsn_spacecombat2 );
             learn_from_success( ch, gsn_spacecombat3 );
             echo_to_ship( AT_RED , target , "A small explosion vibrates through the ship." );           
             damage_ship_ch( target , ship->laserdamage+bonusdamage  , (ship->laserdamage+bonusdamage)*2  , ch, ship);
             
             if ( autofly(target) && target->target0 != ship )
             {
                target->target0 = ship; 
                sprintf( buf , "You are being targetted by %s." , target->name);  
                echo_to_cockpit( AT_BLOOD , ship , buf );
             }
             
             return;
    	}
        
    	if ( ch->in_room->vnum == ship->gunseat && !str_prefix( argument , "ions") )
    	{
    	
    	     if (ship->ionstate == LASER_DAMAGED)
    	     {
    	        send_to_char("&RThe ships ion cannons are damaged.\n\r",ch);
    	      	return;
    	     } 
             if(IS_SET(ship->flags, SHIPFLAG_DUALION))
                linkshot = 2;
             if(IS_SET(ship->flags, SHIPFLAG_TRIION))
                linkshot = 3;
             if(IS_SET(ship->flags, SHIPFLAG_QUADION))
                linkshot = 4;
             if(linkshot == 0)
                linkshot = 1;
             if(IS_SET(ship->flags, SHIPFLAG_SHIELDRLASER))
                exshots += ship->ions / 2;
             if(IS_SET(ship->flags, SHIPFLAG_ENGINERLASER))
                exshots += ship->ions / 2;
             if(IS_SET(ship->flags, SHIPFLAG_LASERRENGINE))
                exshots -= ship->ions / 2;
             if(IS_SET(ship->flags, SHIPFLAG_LASERRSHIELD))
                exshots -= ship->ions /2;

             if (ship->ionstate >= (ship->ions + 1 - linkshot) + exshots)
    	     {
    	     	send_to_char("&RThe ion cannons are still recharging.\n\r",ch);
    	     	return;
    	     }
    	     if (ship->target0 == NULL )
    	     {
    	     	send_to_char("&RYou need to choose a target first.\n\r",ch);
    	     	return;
    	     }    	    
    	     target = ship->target0;
    	     if (ship->target0->starsystem != ship->starsystem || IS_SET(target->flags, SHIPFLAG_CLOAKED))
    	     {
    	     	send_to_char("&RYour target seems to have left.\n\r",ch);
    	        ship->target0 = NULL; 
    	     	return;
    	     } 
             if(ship->ionstate < 0)
             { /* For overcharging. */
                bonusdamage += 1;
                bonusrange = 200;
             }

             if ( abs(target->vx - ship->vx) >(800+bonusrange) ||
                  abs(target->vy - ship->vy) >(800+bonusrange) ||
                  abs(target->vz - ship->vz) >(800+bonusrange) )
             {
                send_to_char("&RThat ship is out of ion range.\n\r",ch);
    	     	return;
             } 
             if ( ship->class < 2 && !is_facing( ship, target ) )
             {
                send_to_char("&RThe ion cannons can only fire forward. You'll need to turn your ship!\n\r",ch);
    	     	return;
             }      
          for(shots = 0; shots < linkshot; shots++)
          {
             ship->ionstate++;
             if (ship->target0->starsystem != ship->starsystem)
             {
                send_to_char("&RYour target seems to have left.\n\r",ch);
                ship->target0 = NULL;
                return;
             }
/*Arcturus Version */
             chance = IS_NPC(ch) ? ch->top_level
                    : (int) ( get_curr_dex(ch) + ch->pcdata->learned[gsn_spacecombat]/4
                           + ch->pcdata->learned[gsn_spacecombat2]/4 + ch->pcdata->learned[gsn_spacecombat3]/4 );
             chance += (ship->manuever - target->manuever) / 10;
             chance += (ship->currspeed - target->currspeed) / 10;
             chance -= ( abs(target->vx - ship->vx)/120 );
             chance -= ( abs(target->vy - ship->vy)/120 );
             chance -= ( abs(target->vz - ship->vz)/120 );
             if ( ch->subclass == SUBCLASS_WFOCUS )
             {
                chance = chance * 2;
                chance = URANGE( 15 , chance , 95 );
                bonusdamage = 3;
             }
             else
                chance = URANGE( 10, chance, 90 );
             act( AT_PLAIN, "$n presses the fire button.", ch,
                  NULL, argument , TO_ROOM );
/*Sabotage bit by Arcturus */
    if(IS_SET(ship->flags, SHIPFLAG_SABOTAGEDIONS))
    {
           echo_to_room( AT_BLUE , get_room_index(ship->turret2) , "A small blast damages your ion cannons, it must be sabotage!\n\r");
           echo_to_room( AT_BLOOD + AT_BLINK , get_room_index(ship->turret2) , "IONS DAMAGED!\n\r" );
           ship->ionstate = LASER_DAMAGED;
           REMOVE_BIT(ship->flags, SHIPFLAG_SABOTAGEDIONS);
           return;
    }
             if(evaded(ship, target))
             {
                sprintf( buf , "A blast of ionic energy fires from %s at you but you evade it.." , ship->name);
                echo_to_cockpit( AT_BLUE , target , buf );
                sprintf( buf , "The ships ion cannons fire at %s but they evade it." , target->name);
                echo_to_cockpit( AT_BLUE , ship , buf );
                learn_from_failure( ch, gsn_spacecombat );
                learn_from_failure( ch, gsn_spacecombat2 );
                learn_from_failure( ch, gsn_spacecombat3 );
                sprintf( buf, "A blast of ionic energy from %s barely misses %s." , ship->name , target->name );
                echo_to_system( AT_BLUE , ship , buf , target );
             }
             else if ( number_percent( ) > chance )
             {  
                sprintf( buf , "A blast of ionic energy fires from %s at you but miss." , ship->name);  
                echo_to_cockpit( AT_BLUE , target , buf );           
                sprintf( buf , "The ships ion cannons fire at %s but miss." , target->name);  
                echo_to_cockpit( AT_BLUE , ship , buf );           
                learn_from_failure( ch, gsn_spacecombat ); 
    	        learn_from_failure( ch, gsn_spacecombat2 ); 
    	        learn_from_failure( ch, gsn_spacecombat3 ); 
    	        sprintf( buf, "A blast of ionic energy from %s barely misses %s." , ship->name , target->name );
                echo_to_system( AT_BLUE , ship , buf , target );
             }
             else
             { 
                sprintf( buf, "An ion blast from %s hits %s." , ship->name, target->name );
                echo_to_system( AT_BLUE , ship , buf , target );
                sprintf( buf , "You are hit by an ion blast from %s!" , ship->name);  
                echo_to_cockpit( AT_BLUE , target , buf );           
                sprintf( buf , "Your ships ion cannons hit %s!." , target->name);  
                echo_to_cockpit( AT_BLUE , ship , buf );           
                learn_from_success( ch, gsn_spacecombat );
                learn_from_success( ch, gsn_spacecombat2 );
                learn_from_success( ch, gsn_spacecombat3 );
                echo_to_ship( AT_BLUE , target , "Wave of blue sparks fly over the ship!" );
                iondamage_ship_ch( target , (ship->laserdamage+bonusdamage)/2  , (ship->laserdamage+bonusdamage) , ch, ship );
             }
             if ( autofly(target) && target->target0 != ship )
             {
                target->target0 = ship; 
                sprintf( buf , "You are being targetted by %s." , target->name);  
                echo_to_cockpit( AT_BLOOD , ship , buf );
             }
          }   
             return;
    	}

    	if ( ch->in_room->vnum == ship->gunseat && !str_prefix( argument , "autocannon") )
    	{
	     if (ship->autocannon != 1)
	     {
    	     	send_to_char("&RYou may have better luck using a ship WITH an autocannon.\n\r",ch);
    	     	return;
	     }	     
    	     if (ship->target0 == NULL )
    	     {
    	     	send_to_char("&RYou need to choose a target first.\n\r",ch);
    	     	return;
    	     }    	    
    	     target = ship->target0;
    	     if (ship->target0->starsystem != ship->starsystem || IS_SET(target->flags, SHIPFLAG_CLOAKED))
    	     {
    	     	send_to_char("&RYour target seems to have left.\n\r",ch);
    	        ship->target0 = NULL; 
    	     	return;
    	     } 
             if ( abs(target->vx - ship->vx) >1500 ||
                  abs(target->vy - ship->vy) >1500 ||
                  abs(target->vz - ship->vz) >1500 )
             {
                send_to_char("&RThat ship is out of autocannon range.\n\r",ch);
    	     	return;
             } 
             if ( ship->class < 2 && !is_facing( ship, target ) )
             {
                send_to_char("&RThe autocannon can only fire forward. You'll need to turn your ship!\n\r",ch);
    	     	return;
             }
	     if (ship->autoammo == 0)
	     {
    	     	send_to_char("&RThe autocannon cycles up, but has no ammo to fire!\n\r",ch);
    	     	return;
	     }	     
             act( AT_PLAIN, "$n presses the fire button.", ch,
                  NULL, argument , TO_ROOM );
		autohits = 1;
		automiss = 0;
		while ( automiss == 0 )
		{
/*Arcturus Version */
                 chance = IS_NPC(ch) ? ch->top_level
                        : (int) ( get_curr_dex(ch) + ch->pcdata->learned[gsn_spacecombat]/4
                           + ch->pcdata->learned[gsn_spacecombat2]/4 + ch->pcdata->learned[gsn_spacecombat3]/4 );
                  chance += (ship->manuever - target->manuever) / 10;
                  chance += (ship->currspeed - target->currspeed) / 10;
                  chance -= ( abs(target->vx - ship->vx)/120 );
                  chance -= ( abs(target->vy - ship->vy)/120 );
                  chance -= ( abs(target->vz - ship->vz)/120 );
                  if( ch->subclass == SUBCLASS_WFOCUS )
                  {
                      bonusdamage = 1;
                      chance += 15; 
                      chance = URANGE(15, chance, 95);
                  }
                  else
                      chance = URANGE(10, chance, 90);
                  if ( number_percent( ) < chance )  
                  	autohits++;
		else 
				automiss++;
		}
		 if ( autohits > 100 )
			autohits = 100;
             if ( ch->subclass == SUBCLASS_WFOCUS )
                bonusdamage = 3;

             sprintf( buf, "%s fires a stream of autocannon fire at %s." , ship->name, target->name );
             echo_to_system( AT_CYAN , ship , buf , target );
             sprintf( buf , "You are hit by autocannon fire from %s!" , ship->name);  
             echo_to_cockpit( AT_CYAN , target , buf );           
             sprintf( buf , "You fire a burst from the autocannon at %s, scoring %d hits!." , target->name, autohits );  
             echo_to_cockpit( AT_CYAN , ship , buf );           
             learn_from_success( ch, gsn_spacecombat );
             learn_from_success( ch, gsn_spacecombat2 );
             learn_from_success( ch, gsn_spacecombat3 );
             echo_to_ship( AT_CYAN , target , "The ship shudders as thousands of autocannon rounds slam into it!" );
             damage_ship_ch( target , ( (ship->autodamage+bonusdamage) * autohits)  , ( (ship->autodamage+bonusdamage) * autohits * 2) , ch, ship );
             ship->autoammo = ( ship->autoammo - 1 );
             if ( autofly(target) && target->target0 != ship )
             {
                target->target0 = ship; 
                sprintf( buf , "You are being targetted by %s." , target->name);  
                echo_to_cockpit( AT_BLOOD , ship , buf );
             }
             
             return;
    	}

        send_to_char( "&RYou can't fire that!\n\r" , ch);
    	
}


void do_calculate(CHAR_DATA *ch, char *argument )
{
    char arg1[MAX_INPUT_LENGTH];
    char arg2[MAX_INPUT_LENGTH];
    char arg3[MAX_INPUT_LENGTH];  
    int chance , count;
    int distance;
    SHIP_DATA *ship;
    SPACE_DATA *starsystem;
    argument = one_argument( argument , arg1);
    argument = one_argument( argument , arg2);
    argument = one_argument( argument , arg3);
    
    
    	        if (  (ship = ship_from_cockpit(ch->in_room->vnum))  == NULL )
    	        {
    	            send_to_char("&RYou must be in the cockpit of a ship to do that!\n\r",ch);
    	            return;
    	        }
                
                if ( ship->class > MOBILE_SUIT )
    	        {
    	            send_to_char("&RThis isn't a spacecraft!\n\r",ch);
    	            return;
    	        }
    	        
                if (  (ship = ship_from_navseat(ch->in_room->vnum))  == NULL )
    	        {
    	            send_to_char("&RYou must be at a nav computer to calculate jumps.\n\r",ch);
    	            return;
    	        }
    	        
                if ( autofly(ship)  )
    	        {
    	            send_to_char("&RYou'll have to turn off the ships autopilot first....\n\r",ch);
    	            return;
    	        }
    	        
                if  ( ship->class == SHIP_PLATFORM )
                {
                   send_to_char( "&RAnd what exactly are you going to calculate...?\n\r" , ch );
                   return;
                }   
    	        if (ship->hyperspeed == 0)
                {
                  send_to_char("&RThis ship is not equipped with a hyperdrive!\n\r",ch);
                  return;   
                }
                if (ship->shipstate == SHIP_DOCKED)
    	        {
    	            send_to_char("&RYou can't do that until after you've launched!\n\r",ch);
    	            return;
    	        }

                if ((ship->shipstate2 == SHIP_DOCK) || (ship->shipstate2 == SHIP_DOCK_2))
                {
                    send_to_char("&RNot while docking procedures are going on.\n\r", ch);
                    return;
                }
                
                if (ship->shipstate2 == SHIP_DOCK_3)
                {
                    send_to_char("&RDetach from the docked ship first.\n\r",ch);
                    return;
                }

    	        if (ship->starsystem == NULL)
    	        {
    	            send_to_char("&RYou can only do that in realspace.\n\r",ch);
    	            return;
    	        }
    	        if (argument[0] == '\0')
    	        {
    	            send_to_char("&WFormat: Calculate <starsystem> <entry x> <entry y> <entry z>\n\r&wPossible destinations:\n\r",ch);
    	            for ( starsystem = first_starsystem; starsystem; starsystem = starsystem->next )
                    {
                       set_char_color( AT_NOTE, ch );
                       if ( IS_SET(ship->flags, SHIPFLAG_SIMULATOR) )
                       {
                          if ( !str_cmp( starsystem->name, "Simulator" ) )
                          {
                             ch_printf(ch,"%-30s %d\n\r",starsystem->name,
                                      (abs(starsystem->xpos - ship->starsystem->xpos)+
                                       abs(starsystem->ypos - ship->starsystem->ypos))/2);
                             count++;
                          }
                       }
                       else
                       {
                          if ( str_cmp( starsystem->name, "Simulator" ) )
                          {
                          ch_printf(ch,"%-30s %d\n\r",starsystem->name,
                                   (abs(starsystem->xpos - ship->starsystem->xpos)+
                                    abs(starsystem->ypos - ship->starsystem->ypos))/2);
                          count++;
                          }
                       }
                    }
                    return;
    	        }
               chance = IS_NPC(ch) ? ch->top_level
	                 : (int)  (ch->pcdata->learned[gsn_navigation]) ;
                if ( number_percent( ) > chance )
    		{
	           send_to_char("&RYou cant seem to figure the charts out today.\n\r",ch);
	           learn_from_failure( ch, gsn_navigation );
    	   	   return;	
    	   	}

    
    ship->currjump = starsystem_from_name( arg1 );
    ship->jx = atoi(arg2);
    ship->jy = atoi(arg3);
    ship->jz = atoi(argument);
    
    if ( ship->currjump == NULL )
    {
        send_to_char( "&RYou can't seem to find that starsytem on your charts.\n\r", ch);
        return;
    }/* for Simulators by Ackbar, installed by Arcturus */

    distance = (int) sqrt(pow((atoi(arg2) - ship->vx), 2) +  pow((atoi(arg3) - ship->vy), 2)
             + pow((atoi(argument) - ship->vz),2));
    if(!str_cmp(ship->currjump->name, ship->starsystem->name) && abs(distance) < 3000)
    {
//        sprintf(buf, "Ship %s removed from calculation. Distance %d\n\r", ship->name, distance);
//        log_string(buf);
        send_to_char("You can't hyperdrive to something so close, you may overjump it.\n\r", ch);
        ship->currjump = NULL;
        return;
    }
    if (IS_SET(ship->flags, SHIPFLAG_SIMULATOR) && ship->currjump != starsystem_from_name("Simulator") )
    {
        send_to_char( "&RYou can't seem to find that starsytsem on your charts.\n\r", ch);
        return;
    }
    else if (!IS_SET(ship->flags, SHIPFLAG_SIMULATOR) && ship->currjump == starsystem_from_name("Simulator") )
    {
       send_to_char( "&RYou can't seem to find that starsytsem on your charts.\n\r", ch);
       return;
    }
    else
    { 
        SPACE_DATA * starsystem;
      
        starsystem = ship->currjump;
           
          if ( starsystem->star1 && strcmp(starsystem->star1,"") && 
                     abs(ship->jx - starsystem->s1x) < 300 && 
                     abs(ship->jy - starsystem->s1y) < 300 &&
                     abs(ship->jz - starsystem->s1z) < 300 )
                {
                    echo_to_cockpit( AT_RED, ship, "WARNING.. Jump coordinates too close to stellar object.");
                    echo_to_cockpit( AT_RED, ship, "WARNING.. Hyperjump NOT set.");
                    ship->currjump = NULL;
                    return;
                }              
          else if ( starsystem->star2 && strcmp(starsystem->star2,"") && 
               abs(ship->jx - starsystem->s2x) < 300 && 
                     abs(ship->jy - starsystem->s2y) < 300 &&
                     abs(ship->jz - starsystem->s2z) < 300 )
                {
                    echo_to_cockpit( AT_RED, ship, "WARNING.. Jump coordinates too close to stellar object.");
                    echo_to_cockpit( AT_RED, ship, "WARNING.. Hyperjump NOT set.");
                    ship->currjump = NULL;
                    return;
                }            
          else if ( starsystem->planet1 && strcmp(starsystem->planet1,"") && 
               abs(ship->jx - starsystem->p1x) < 300 && 
                     abs(ship->jy - starsystem->p1y) < 300 &&
                     abs(ship->jz - starsystem->p1z) < 300 )
                {
                    echo_to_cockpit( AT_RED, ship, "WARNING.. Jump coordinates too close to stellar object.");
                    echo_to_cockpit( AT_RED, ship, "WARNING.. Hyperjump NOT set.");
                    ship->currjump = NULL;
                    return;
                }            
          else if ( starsystem->planet2 && strcmp(starsystem->planet2,"") && 
               abs(ship->jx - starsystem->p2x) < 300 && 
                     abs(ship->jy - starsystem->p2y) < 300 &&
                     abs(ship->jz - starsystem->p2z) < 300 )
                {
                    echo_to_cockpit( AT_RED, ship, "WARNING.. Jump coordinates too close to stellar object.");
                    echo_to_cockpit( AT_RED, ship, "WARNING.. Hyperjump NOT set.");
                    ship->currjump = NULL;
                    return;
                }            
          else if ( starsystem->planet3 && strcmp(starsystem->planet3,"") && 
               abs(ship->jx - starsystem->p3x) < 300 && 
                     abs(ship->jy - starsystem->p3y) < 300 &&
                     abs(ship->jz - starsystem->p3z) < 300 )
                {
                    echo_to_cockpit( AT_RED, ship, "WARNING.. Jump coordinates too close to stellar object.");
                    echo_to_cockpit( AT_RED, ship, "WARNING.. Hyperjump NOT set.");
                    ship->currjump = NULL;
                    return;
                } 
          else
          {           
             ship->jx += number_range ( -250 , 250 );
             ship->jy += number_range ( -250 , 250 );
             ship->jz += number_range ( -250 , 250 );
          }
    }
    
    ship->hyperdistance  = abs(ship->starsystem->xpos - ship->currjump->xpos) ;
    ship->hyperdistance += abs(ship->starsystem->ypos - ship->currjump->ypos) ;
    ship->hyperdistance /= 5;
    
    if (ship->hyperdistance<100)
      ship->hyperdistance = 100;
    
    ship->hyperdistance += number_range(0, 200);
    
    sound_to_room( ch->in_room , "!!SOUND(computer)" );
    
    send_to_char( "&GHyperspace course set. Ready for the jump to lightspeed.\n\r", ch);
    act( AT_PLAIN, "$n does some calculations using the ships computer.", ch,
		        NULL, argument , TO_ROOM );
	                
    learn_from_success( ch, gsn_navigation );
    	
    WAIT_STATE( ch , 2*PULSE_VIOLENCE );	
}

void do_recharge(CHAR_DATA *ch, char *argument )
{
    int recharge;
    int chance;
    SHIP_DATA *ship;
    
   
        if (  (ship = ship_from_cockpit(ch->in_room->vnum))  == NULL )
        {
            send_to_char("&RYou must be in the cockpit of a ship to do that!\n\r",ch);
            return;
        }
        if (  (ship = ship_from_coseat(ch->in_room->vnum))  == NULL )
        {
            send_to_char("&RThe controls must be at the co-pilot station.\n\r",ch);
            return;
        }
        
                if ( autofly(ship)  )
    	        {
    	            send_to_char("&RUnable to Comply, Autopilot Engaged\n\r",ch);
    	            return;
    	        }
    	        
        /*        if (ship->shipstate == SHIP_DISABLED)
    	        {
    	            send_to_char("&RThe ships drive is disabled. Unable to manuever.\n\r",ch);
    	            return;
    	        } Whats this got to do with shields? - Arcturus */
    	        
        if ( ship->energy < 100 )
        {
              send_to_char("&RTheres not enough energy!\n\r",ch);
              return;
        }
    	        
        chance = IS_NPC(ch) ? ch->top_level
                 : (int) (ch->pcdata->learned[gsn_shipsystems]);
        if ( number_percent( ) > chance )
        {
            send_to_char("&RYou fail to work the controls properly.\n\r",ch);
            learn_from_failure( ch, gsn_shipsystems );
    	   return;	
        }
        
    send_to_char( "&GRecharging shields..\n\r", ch);
    act( AT_PLAIN, "$n pulls back a lever on the control panel.", ch,
         NULL, argument , TO_ROOM );
    
    learn_from_success( ch, gsn_shipsystems );
     
    recharge  = UMIN(  ship->maxshield-ship->shield , ship->energy*5 + 100 );
    recharge  = URANGE( 1, recharge , 25+ship->class*25 );
    ship->shield += recharge;
    if(ship->shield > ship->maxshield)
    {
       recharge = ship->maxshield-ship->shield;
       ship->shield = ship->maxshield;
    }
    ship->energy -= ( recharge*2 + recharge * ship->class );        
}


void do_repairship(CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    int chance, change;
    SHIP_DATA *ship;
    
    strcpy( arg, argument );    
    if(!IS_NPC(ch))
    {
       if(IS_SET(ch->pcdata->flags, PCFLAG_BOUND))
       {
          send_to_char("Not while your bound.\n\r", ch);
          return;
       }
    }

    switch( ch->substate )
    { 
    	default:
    	        if (  (ship = ship_from_engine(ch->in_room->vnum))  == NULL )
    	        {
    	            send_to_char("&RYou must be in the engine room of a ship to do that!\n\r",ch);
    	            return;
    	        }
                
              if ( str_cmp( argument , "hull" ) && str_cmp( argument , "drive" ) && 
                     str_cmp( argument , "launcher" ) && str_cmp( argument , "laser" ) && 
                     str_cmp( argument , "turret 1" ) && str_cmp( argument , "turret 2") &&
	             str_cmp( argument , "ions" )     && str_cmp( argument , "rlauncher") &&
                     str_cmp( argument , "tlauncher") )
                {
                   send_to_char("&RYou need to spceify something to repair:\n\r",ch);
                   send_to_char("&rTry: hull, drive, launcher, tlauncher, rlauncher, laser, ions, turret 1, or turret 2\n\r",ch);
                   return;
                }
                            
                chance = IS_NPC(ch) ? ch->top_level
	                 : (int) (ch->pcdata->learned[gsn_shipmaintenance]);
                if ( number_percent( ) < chance )
    		{
    		   send_to_char( "&GYou begin your repairs\n\r", ch);
    		   act( AT_PLAIN, "$n begins repairing the ships $T.", ch,
		        NULL, argument , TO_ROOM );
                   if(ch->subclass == SUBCLASS_QUICKWORK) 
   		     add_timer ( ch , TIMER_DO_FUN , 3 , do_repairship , 1 );
                   else
                     add_timer ( ch, TIMER_DO_FUN, 10, do_repairship, 1);
    		   ch->dest_buf = str_dup(arg);
    		   return;
	        }
	        send_to_char("&RYou fail to locate the source of the problem.\n\r",ch);
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
    		if ( (ship = ship_from_cockpit(ch->in_room->vnum)) == NULL )
    		      return;    		                                   
    	        send_to_char("&RYou are distracted and fail to finish your repairs.\n\r", ch);
    		return;
    }
    
    ch->substate = SUB_NONE;
    
    if ( (ship = ship_from_engine(ch->in_room->vnum)) == NULL )
    {  
       return;
    }
    
    if ( !str_cmp(arg,"hull") )
    {
        change = URANGE( 0 , 
                         number_range( (int) ( ch->pcdata->learned[gsn_shipmaintenance]) , (int) (ch->pcdata->learned[gsn_shipmaintenance] * 2) ),
                         ( ship->maxhull - ship->hull ) );
        ship->hull += change;
        ch_printf( ch, "&GRepair complete.. Hull strength inreased by %d points.\n\r", change );
    }
    
    if ( !str_cmp(arg,"drive") )
    {  
       if (ship->location == ship->lastdoc)
          ship->shipstate = SHIP_DOCKED;
       else if(ship->shipstate != SHIP_HYPERSPACE)
          ship->shipstate = SHIP_READY;
       send_to_char("&GShips drive repaired.\n\r", ch);
       if(IS_SET(ship->flags, SHIPFLAG_SABOTAGEDENGINE))
       {
          send_to_char("Sabotage detected, removed.\n\r", ch);
          REMOVE_BIT(ship->flags, SHIPFLAG_SABOTAGEDENGINE);
       }		
    }
    
    if ( !str_cmp(arg,"launcher") )
    {  
       ship->missilestate = MISSILE_READY;
       send_to_char("&GMissile launcher repaired.\n\r", ch);
       if(IS_SET(ship->flags, SHIPFLAG_SABOTAGEDLAUNCHERS))
       {
          send_to_char("Sabotage detected, removed.\n\r", ch);
          REMOVE_BIT(ship->flags, SHIPFLAG_SABOTAGEDLAUNCHERS);
       }
    }
    if ( !str_cmp(arg, "tlauncher") )
    {
       ship->torpedostate = MISSILE_READY;
       send_to_char("&GTorpedo launcher repaired.\n\r", ch);
       if(IS_SET(ship->flags, SHIPFLAG_SABOTAGEDTLAUNCHERS))
       {
          send_to_char("Sabotage detected, removed.\n\r", ch);
          REMOVE_BIT(ship->flags, SHIPFLAG_SABOTAGEDTLAUNCHERS);
       }
    }
    if ( !str_cmp(arg, "rlauncher"))
    {
       ship->rocketstate = MISSILE_READY;
       send_to_char("&GRocket launcher repaired.\n\r", ch);
       if(IS_SET(ship->flags, SHIPFLAG_SABOTAGEDRLAUNCHERS))
       {
          send_to_char("Sabotage detected, removed.\n\r", ch);
          REMOVE_BIT(ship->flags, SHIPFLAG_SABOTAGEDRLAUNCHERS);
       }
    }

    if ( !str_cmp(arg,"laser") )
    {  
       ship->statet0 = LASER_READY;
       send_to_char("&GMain laser repaired.\n\r", ch);
       if(IS_SET(ship->flags, SHIPFLAG_SABOTAGEDLASERS))
       {
          send_to_char("Sabotage detected, removed.\n\r", ch);
          REMOVE_BIT(ship->flags, SHIPFLAG_SABOTAGEDLASERS);
       }
    }

    if ( !str_cmp(arg,"ions") )
    {  
       ship->ionstate = LASER_READY;
       send_to_char("&GIon cannons repaired.\n\r", ch);
       if(IS_SET(ship->flags, SHIPFLAG_SABOTAGEDIONS))
       {
          send_to_char("Sabotage detected, removed.\n\r", ch);
          REMOVE_BIT(ship->flags, SHIPFLAG_SABOTAGEDIONS);
       }
    }
    
    if ( !str_cmp(arg,"turret 1") )
    {  
       ship->statet1 = LASER_READY;
       send_to_char("&GLaser Turret 1 repaired.\n\r", ch);
       if(IS_SET(ship->flags, SHIPFLAG_SABOTAGEDTURRET1))
       {
          send_to_char("Sabotage detected, removed.\n\r", ch);
          REMOVE_BIT(ship->flags, SHIPFLAG_SABOTAGEDTURRET1);
       }
    }
    
    if ( !str_cmp(arg,"turret 2") )
    {  
       ship->statet2 = LASER_READY;
       send_to_char("&Laser Turret 2 repaired.\n\r", ch);
       if(IS_SET(ship->flags, SHIPFLAG_SABOTAGEDTURRET2))
       {
          send_to_char("Sabotage detected, removed.\n\r", ch);
          REMOVE_BIT(ship->flags, SHIPFLAG_SABOTAGEDTURRET2);
       }
    }
        
    act( AT_PLAIN, "$n finishes the repairs.", ch,
         NULL, argument , TO_ROOM );

    learn_from_success( ch, gsn_shipmaintenance );
    	
}


void do_addpilot(CHAR_DATA *ch, char *argument )
{
    SHIP_DATA *ship;
    
   if (  (ship = ship_from_cockpit(ch->in_room->vnum))  == NULL )
   {
           send_to_char("&RYou must be in the cockpit of a ship to do that!\n\r",ch);
           return;
   }
    
                if  ( ship->class == SHIP_PLATFORM )
                {
                   send_to_char( "&RYou can't do that here.\n\r" , ch );
                   return;
                }   
    
   if ( str_cmp( ship->owner , ch->name ) )
   {
   
     if ( !IS_NPC(ch) && ch->pcdata && ch->pcdata->clan && !str_cmp(ch->pcdata->clan->name,ship->owner) ) 
         if ( !str_cmp(ch->pcdata->clan->leader,ch->name) )
           ;
         else if ( !str_cmp(ch->pcdata->clan->number1,ch->name) )
           ;
         else if ( !str_cmp(ch->pcdata->clan->number2,ch->name) )
           ;
         else
         {
           send_to_char( "&RThat isn't your ship!" ,ch );
   	   return;
         }
     else
     {     
   	send_to_char( "&RThat isn't your ship!" ,ch );
   	return;
     }
   
   }

   if (argument[0] == '\0')
   {
       send_to_char( "&RAdd which pilot?\n\r" ,ch );
       return;
   }
   
   if ( str_cmp( ship->pilot , "" ) )
   {
        if ( str_cmp( ship->copilot , "" ) )
        {
             send_to_char( "&RYou are ready have a pilot and copilot..\n\r" ,ch );
             send_to_char( "&RTry rempilot first.\n\r" ,ch );
             return;
        }
        
        STRFREE( ship->copilot );
	ship->copilot = STRALLOC( argument );
	send_to_char( "Copilot Added.\n\r", ch );
	save_ship( ship );
	return;
        
        return;
   }
   
   STRFREE( ship->pilot );
   ship->pilot = STRALLOC( argument );
   send_to_char( "Pilot Added.\n\r", ch );
   save_ship( ship );

}

void do_rempilot(CHAR_DATA *ch, char *argument )
{
    SHIP_DATA *ship;
    
   if (  (ship = ship_from_cockpit(ch->in_room->vnum))  == NULL )
   {
           send_to_char("&RYou must be in the cockpit of a ship to do that!\n\r",ch);
           return;
   }
   
                if  ( ship->class == SHIP_PLATFORM )
                {
                   send_to_char( "&RYou can't do that here.\n\r" , ch );
                   return;
                }   
   
   if ( str_cmp( ship->owner , ch->name ) )
   {
   
     if ( !IS_NPC(ch) && ch->pcdata && ch->pcdata->clan && !str_cmp(ch->pcdata->clan->name,ship->owner) ) 
         if ( !str_cmp(ch->pcdata->clan->leader,ch->name) )
           ;
         else if ( !str_cmp(ch->pcdata->clan->number1,ch->name) )
           ;
         else if ( !str_cmp(ch->pcdata->clan->number2,ch->name) )
           ;
         else
         {
           send_to_char( "&RThat isn't your ship!" ,ch );
   	   return;
         }
     else
     {     
   	send_to_char( "&RThat isn't your ship!" ,ch );
   	return;
     }
   
   }

   if (argument[0] == '\0')
   {
       send_to_char( "&RRemove which pilot?\n\r" ,ch );
       return;
   }
   
   if ( !str_cmp( ship->pilot , argument ) )
   {
        STRFREE( ship->pilot );
	ship->pilot = STRALLOC( "" );
	send_to_char( "Pilot Removed.\n\r", ch );
	save_ship( ship );
	return;
   }       
   
   if ( !str_cmp( ship->copilot , argument ) )
   {      
        STRFREE( ship->copilot );
	ship->copilot = STRALLOC( "" );
	send_to_char( "Copilot Removed.\n\r", ch );
	save_ship( ship );
	return;
    }    

    send_to_char( "&RThat person isn't listed as one of the ships pilots.\n\r" ,ch );

}

void do_radar( CHAR_DATA *ch, char *argument )
{
    SHIP_DATA *target;
    int chance; 
    int distance;
    SHIP_DATA *ship;
    MISSILE_DATA *missile;    
   
        if (   (ship = ship_from_cockpit(ch->in_room->vnum))  == NULL )
        {
            send_to_char("&RYou must be in the cockpit or turret of a ship to do that!\n\r",ch);
            return;
        }
        
        if ( ship->class > MOBILE_SUIT )
    	        {
    	            send_to_char("&RThis isn't a spacecraft!\n\r",ch);
    	            return;
    	        }
    	        
        if (ship->shipstate == SHIP_DOCKED)
        {
            send_to_char("&RWait until after you launch!\n\r",ch);
            return;
        }
        
        if (ship->shipstate == SHIP_HYPERSPACE)
        {
            send_to_char("&RYou can only do that in realspace!\n\r",ch);
            return;
        }
        
    	if (ship->starsystem == NULL)
    	{
    	       send_to_char("&RYou can't do that unless the ship is flying in realspace!\n\r",ch);
    	       return;
    	}        
    	        
        chance = IS_NPC(ch) ? ch->top_level
             : (int)  (ch->pcdata->learned[gsn_navigation]) ;
        if ( number_percent( ) > chance )
        {
           send_to_char("&RYou fail to work the controls properly.\n\r",ch);
           learn_from_failure( ch, gsn_navigation );
    	   return;	
        }
        
    
    act( AT_PLAIN, "$n checks the radar.", ch,
         NULL, argument , TO_ROOM );
     
                   sound_to_ship(ship , "!!SOUND(radar) \n\r" );
    	           set_char_color(  AT_WHITE, ch );
    	           ch_printf(ch, "%s\n\r\n\r" , ship->starsystem->name );
    	           set_char_color(  AT_WHITE, ch );
    	           if ( ship->starsystem->star1 && str_cmp(ship->starsystem->star1,"") ) 
    	           {   
                       distance = (int) sqrt(pow(ship->starsystem->s1x - ship->vx, 2) 
                                + pow(ship->starsystem->s1y - ship->vy, 2)
                                + pow(ship->starsystem->s1z - ship->vz, 2) );
                       ch_printf(ch, "%s   %d %d %d   (%d)\n\r" , 
    	                        ship->starsystem->star1,
    	                        ship->starsystem->s1x,
    	                        ship->starsystem->s1y,
    	                        ship->starsystem->s1z, distance );
    	           }
                   if ( ship->starsystem->star2 && str_cmp(ship->starsystem->star2,"")  ) 
    	           {
                       distance = (int) sqrt(pow(ship->starsystem->s2x - ship->vx, 2) 
                                + pow(ship->starsystem->s2y - ship->vy, 2)
                                + pow(ship->starsystem->s2z - ship->vz, 2) );
                       ch_printf(ch, "%s   %d %d %d   (%d)\n\r" , 
    	                        ship->starsystem->star2,
    	                        ship->starsystem->s2x,
    	                        ship->starsystem->s2y,
    	                        ship->starsystem->s2z, distance );
                   } 
                   set_char_color( AT_LBLUE, ch ); 
  	           if ( ship->starsystem->planet1 && str_cmp(ship->starsystem->planet1,"") ) 
    	           {
                       distance = (int) sqrt(pow(ship->starsystem->p1x - ship->vx, 2) 
                                + pow(ship->starsystem->p1y - ship->vy, 2)
                                + pow(ship->starsystem->p1z - ship->vz, 2) );
                       ch_printf(ch, "%s   %d %d %d   (%d)\n\r" , 
    	                        ship->starsystem->planet1,
    	                        ship->starsystem->p1x,
    	                        ship->starsystem->p1y,
    	                        ship->starsystem->p1z, distance );
                   }
    	           if ( ship->starsystem->planet2 && str_cmp(ship->starsystem->planet2,"")  ) 
                   {
                       distance = (int) sqrt(pow(ship->starsystem->p2x - ship->vx, 2) 
                                + pow(ship->starsystem->p2y - ship->vy, 2)
                                + pow(ship->starsystem->p2z - ship->vz, 2) );
    	               ch_printf(ch, "%s   %d %d %d   (%d)\n\r" , 
    	                        ship->starsystem->planet2,
    	                        ship->starsystem->p2x,
    	                        ship->starsystem->p2y,
    	                        ship->starsystem->p2z, distance );
                   }
    	           if ( ship->starsystem->planet3 && str_cmp(ship->starsystem->planet3,"")  ) 
                   {
                       distance = (int) sqrt(pow(ship->starsystem->p3x - ship->vx, 2) 
                                + pow(ship->starsystem->p3y - ship->vy, 2)
                                + pow(ship->starsystem->p3z - ship->vz, 2) );
    	               ch_printf(ch, "%s   %d %d %d   (%d)\n\r" , 
    	                        ship->starsystem->planet3,
    	                        ship->starsystem->p3x,
    	                        ship->starsystem->p3y,
    	                        ship->starsystem->p3z, distance );
                   }
    	           ch_printf(ch,"\n\r");
    	           for ( target = ship->starsystem->first_ship; target; target = target->next_in_starsystem )
                   {       
                        if ( target != ship && !IS_SET(target->flags, SHIPFLAG_CLOAKED)) 
                        {
                            if(!str_cmp(target->owner, "The Empire") || target->type == SHIP_IMPERIAL )
                               set_char_color( AT_GREEN, ch );
                            else if(!str_cmp(target->owner, "New Republic") || target->type == SHIP_REPUBLIC )
                               set_char_color( AT_RED, ch );
                            else if(!str_cmp(target->owner, "Pirates" ) )
                               set_char_color( AT_PURPLE, ch);
                            else
                               set_char_color( AT_BLUE, ch );
                            distance = (int) sqrt(pow(target->vx - ship->vx, 2) 
                                     + pow(target->vy - ship->vy, 2)
                                     + pow(target->vz - ship->vz, 2) );
                            ch_printf(ch, "%s    %.0f %.0f %.0f   (%d)\n\r", 
                            target->name,
                            target->vx,
                            target->vy,
                            target->vz, distance);
                        }
                   }
                   ch_printf(ch,"\n\r");
                   set_char_color( AT_YELLOW, ch);
    	           for ( missile = ship->starsystem->first_missile; missile; missile = missile->next_in_starsystem )
                   {        
                           distance = (int) sqrt(pow(missile->mx - ship->vx, 2) 
                                    + pow(missile->my - ship->vy, 2)
                                    + pow(missile->mz - ship->vz, 2) );
                           ch_printf(ch, "%s    %d %d %d   (%d)\n\r", 
                           	missile->missiletype == CONCUSSION_MISSILE ? "A Concusion missile" : 
    			        ( missile->missiletype ==  PROTON_TORPEDO ? "A Torpedo" : 
    			        ( missile->missiletype ==  HEAVY_ROCKET ? "A Heavy Rocket" : "A Heavy Space Bomb" ) ),
                           	missile->mx,
                           	missile->my,
                                missile->mz, distance );
                   }
                     
                   ch_printf(ch, "\n\r&WYour Coordinates: %.0f %.0f %.0f\n\r" , 
                             ship->vx , ship->vy, ship->vz);   
        
    	        
        learn_from_success( ch, gsn_navigation );
  
}

void do_autotrack( CHAR_DATA *ch, char *argument )
{
   SHIP_DATA *ship;
   int chance;
 
   if (  (ship = ship_from_cockpit(ch->in_room->vnum))  == NULL )
   {
        send_to_char("&RYou must be in the cockpit of a ship to do that!\n\r",ch);
        return;
   }
   
        if ( ship->class > MOBILE_SUIT )
    	        {
    	            send_to_char("&RThis isn't a spacecraft!\n\r",ch);
    	            return;
    	        }
    	        
 
        if ( ship->class == SHIP_PLATFORM )
    	        {
    	            send_to_char("&RPlatforms don't have autotracking systems!\n\r",ch);
    	            return;
    	        }
        if ( ship->class == CAPITAL_SHIP || ship->class == SUPERCAPITAL_SHIP )
    	        {
    	            send_to_char("&RThis ship is too big for autotracking!\n\r",ch);
    	            return;
    	        }
    	        
   if (  (ship = ship_from_pilotseat(ch->in_room->vnum))  == NULL )
   {
        send_to_char("&RYou aren't in the pilots chair!\n\r",ch);
        return;
   }
   
                if ( autofly(ship)  )
    	        {
    	            send_to_char("&RYou'll have to turn off the ships autopilot first....\n\r",ch);
    	            return;
    	        }
    	        
   chance = IS_NPC(ch) ? ch->top_level
             : (int)  (ch->pcdata->learned[gsn_shipsystems]) ;
        if ( number_percent( ) > chance )
        {
           send_to_char("&RYour notsure which switch to flip.\n\r",ch);
           learn_from_failure( ch, gsn_shipsystems );
    	   return;	
        }
   
   act( AT_PLAIN, "$n flips a switch on the control panel.", ch,
         NULL, argument , TO_ROOM );
   if (ship->autotrack)
   {
     ship->autotrack = FALSE;
     echo_to_cockpit( AT_YELLOW , ship, "Autotracking off.");
   }
   else
   {
      ship->autotrack = TRUE;
      echo_to_cockpit( AT_YELLOW , ship, "Autotracking on.");
   }
   
   learn_from_success( ch, gsn_shipsystems );
        
}

void do_jumpvector( CHAR_DATA *ch, char *argument )
{}
void do_reload( CHAR_DATA *ch, char *argument )
{}
void do_closebay( CHAR_DATA *ch, char *argument )
{}

void do_openbay( CHAR_DATA *ch, char *argument )
{
        SHIP_DATA *ship;
        int chance;
         
        if (  (ship = ship_from_cockpit(ch->in_room->vnum))  == NULL )
        {
                send_to_char("&RYou must be in the cockpit of a ship to do that!\n\r",ch);

                return;
        }

        if ( ship->class > MOBILE_SUIT )
        {
                send_to_char("&RThis isn't a spacecraft!\n\r",ch);
                return;
        }
         
                
        if ( ship->class == SHIP_PLATFORM )
        {
                send_to_char("&RPlatforms don't have Bay Doors!\n\r",ch);
                return;
        }

        if (  (ship = ship_from_pilotseat(ch->in_room->vnum))  == NULL )
        {
                send_to_char("&RYou aren't in the pilots chair!\n\r",ch);
                return;
        }
         
        /* I have removed this because I don't think it is right to use  
           but put it back in if you want
         
        if ( autofly(ship)  )
        {
        send_to_char("&RYou'll have to turn off the ships autopilot
        first....\n\r",ch);
        return;
        }
         
    */
           
         
        chance = IS_NPC(ch) ? ch->top_level
       : (int)  (ch->pcdata->learned[gsn_shipsystems]) ;
        if ( number_percent( ) > chance )
        {
                send_to_char("&RYour not sure which switch to flip.\n\r",ch);
                learn_from_failure( ch, gsn_shipsystems );
                return;
        }   
           
        act( AT_PLAIN, "$n flips a switch on the control panel.", ch,
        NULL, argument , TO_ROOM );
         
        if (ship->bayopen)
        {
                ship->bayopen = FALSE;
                echo_to_cockpit( AT_YELLOW , ship, "Closing bay doors.");
        }
        else
        {  
        ship->bayopen = TRUE;
        echo_to_cockpit( AT_YELLOW , ship, "Opening Bay Doors.");
       }

        learn_from_success( ch, gsn_shipsystems );

}

void do_tractorbeam( CHAR_DATA *ch, char *argument )
{

    char arg[MAX_INPUT_LENGTH];
    int chance;
    SHIP_DATA *ship;
    SHIP_DATA *target;
	char buf[MAX_STRING_LENGTH];
    
    strcpy( arg, argument );

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


	if ( !check_pilot( ch , ship ) )
	{
		send_to_char("This isn't your ship!\n\r" , ch );
		return;
	}

	if ( ship->tractorbeam == 0 )
	{
		send_to_char("You might want to install a tractorbeam!\n\r" , ch );
		return;
	}
    
	if ( ship->hanger == 0 )
	{
		send_to_char("No hangar available.\n\r",ch);
		return;
	}

	if ( !ship->bayopen )
	{
		send_to_char("Your hangar is closed.\n\r",ch);
		return;
	}

   	        
	if ( (ship = ship_from_pilotseat(ch->in_room->vnum)) == NULL )
	{
		send_to_char("&RYou need to be in the pilot seat!\n\r",ch);
		return;
	}
    	        
    	            	        
	if (ship->shipstate == SHIP_DISABLED)
	{
		send_to_char("&RThe ships drive is disabled. No power available.\n\r",ch);
		return;
	}

	if (ship->shipstate == SHIP_DOCKED || ship->shipstate2 != SHIP_READY)
	{
		send_to_char("&RYour ship is docked!\n\r",ch);    
		return;	        
	}
    	                			
	if (ship->shipstate == SHIP_HYPERSPACE)		   
	{
					
		send_to_char("&RYou can only do that in realspace!\n\r",ch);
		return;   			
	}

	if (ship->shipstate != SHIP_READY)
	{
		send_to_char("&RPlease wait until the ship has finished its current maneuver.\n\r",ch);
		return;	    
	}



    	        
	if ( argument[0] == '\0' )	        
	{  	
		send_to_char("&RCapture what?\n\r",ch);	
		return;				
	}
   	            
	target = get_ship_here( argument , ship->starsystem );

	if ( target == NULL || IS_SET(target->flags, SHIPFLAG_CLOAKED))            
	{   	                
		send_to_char("&RI don't see that here.\n\r",ch);	 
		return;             
	} 
   	           
	if ( target == ship )   	
	{   	               
		send_to_char("&RYou can't yourself!\n\r",ch);   	 
		return;              
	}   	           
   	         
	if ( target->shipstate == SHIP_LAND )
	{
		send_to_char("&RThat ship is already in a landing sequence.\n\r", ch);
		return;
	}

	if (  (target->vx > ship->vx + 200) || (target->vx < ship->vx - 200) ||
	   (target->vy > ship->vy + 200) || (target->vy < ship->vy - 200) ||
	   (target->vz > ship->vz + 200) || (target->vz < ship->vz - 200) )
	{
		send_to_char("&R That ship is too far away! You'll have to fly a litlle closer.\n\r",ch);
		return;
	}   
    
	if (ship->class <= target->class)
	{
		send_to_char("&RThat ship is too big for your hangar.\n\r",ch);
		return;
	}
    	        
	if  ( target->class == SHIP_PLATFORM )
	{
		send_to_char( "&RYou can't capture platforms.\n\r" , ch );
		return;
	}   
    
	if ( target->class > MIDSIZE_SHIP)
	{
		send_to_char("&RYou can't capture capital ships.\n\r",ch);
		return;
	}


	if ( ship->energy < (25 + 25*target->class) )
	{
		send_to_char("&RTheres not enough fuel!\n\r",ch);
		return;
	}



				           
	chance = IS_NPC(ch) ? ch->top_level
	: (int)  (ch->pcdata->learned[gsn_tractorbeams]);

	/* This is just a first guess chance modifier, feel free to change if needed */

	chance = chance * ( ship->tractorbeam / (target->currspeed+1 ) );
 
	if ( number_percent( ) < chance )
	{    		   
		set_char_color( AT_GREEN, ch );    
		send_to_char( "Capture sequence initiated.\n\r", ch);    		   
		act( AT_PLAIN, "$n begins the capture sequence.", ch,		        
			NULL, argument , TO_ROOM );
		echo_to_ship( AT_YELLOW , ship , "ALERT: Ship is being captured, all hands to docking bay." );
    	echo_to_ship( AT_YELLOW , target , "The ship shudders as a tractorbeam locks on." );
		sprintf( buf , "You are being captured by %s." , ship->name);  
		echo_to_cockpit( AT_BLOOD , target , buf );

		if ( autofly(target) && !target->target0)
			target->target0 = ship;

		target->dest = STRALLOC(ship->name);    		   
		target->shipstate = SHIP_LAND;    		   
		target->currspeed = 0;	           
	                     
		learn_from_success( ch, gsn_tractorbeams );	
		return;
	                  
	}	       
	send_to_char("You fail to work the controls properly.\n\r",ch);
   	echo_to_ship( AT_YELLOW , target , "The ship shudders and then stops as a tractorbeam attemps to lock on." );
	sprintf( buf , "The %s attempted to capture your ship!" , ship->name);  
	echo_to_cockpit( AT_BLOOD , target , buf );
	if ( autofly(target) && !target->target0)
		target->target0 = ship;

                  
	learn_from_failure( ch, gsn_tractorbeams );                
	  	
   	return;	
}

void do_pluogus( CHAR_DATA *ch, char *argument )
{
     bool ch_comlink = FALSE;          
     OBJ_DATA *obj;
     int next_planet, itt;
              
     for ( obj = ch->last_carrying; obj; obj = obj->prev_content )
     {                               
         if (obj->pIndexData->item_type == ITEM_COMLINK || obj->pIndexData->item_type == ITEM_CARMOR)
         ch_comlink = TRUE;
     }
     
     if (!IS_IMMORTAL(ch))
     {                                          
     if ( !ch_comlink )
     {
        send_to_char( "You need a comlink to do that!\n\r", ch);
        return;
     }         
     
   }

     send_to_char( "Serin Pluogus Schedule Information:\n\r", ch );
     
     /* current port */
     
     if ( bus_pos < 7 && bus_pos > 1 )
        ch_printf( ch, "The Pluogus is Currently docked at %s.\n\r", bus_stop[bus_planet] );
     
     /* destinations */
     
     next_planet = bus_planet;
     send_to_char( "Next stops: ", ch);
     
     if ( bus_pos <= 1 )
        ch_printf( ch, "%s  ", bus_stop[next_planet] );
     
     for ( itt = 0 ; itt < 3 ; itt++ )
     {
         next_planet++;
         if ( next_planet >= MAX_BUS_STOP )
            next_planet = 0;
         ch_printf( ch, "%s  ", bus_stop[next_planet] );
     }     
     
     ch_printf( ch, "\n\r\n\r" );

     send_to_char( "Serin Tocca Schedule Information:\n\r", ch );
     
     /* current port */
     
     if ( bus_pos < 7 && bus_pos > 1 )
        ch_printf( ch, "The Tocca is Currently docked at %s.\n\r", bus_stop[bus2_planet] );
     
     /* destinations */
     
     next_planet = bus2_planet;
     send_to_char( "Next stops: ", ch);
     
     if ( bus_pos <= 1 )
        ch_printf( ch, "%s  ", bus_stop[next_planet] );
     
     for ( itt = 0 ; itt < 3 ; itt++ )
     {
         next_planet++;
         if ( next_planet >= MAX_BUS_STOP )
            next_planet = 0;
         ch_printf( ch, "%s  ", bus_stop[next_planet] );
     }     
     
     ch_printf( ch, "\n\r" );
     
}

void do_fly( CHAR_DATA *ch, char *argument )
{}

void do_drive( CHAR_DATA *ch, char *argument )
{
    int dir;
    SHIP_DATA *ship;
    
        if (  (ship = ship_from_cockpit(ch->in_room->vnum))  == NULL )
        {
            send_to_char("&RYou must be in the drivers seat of a land vehicle to do that!\n\r",ch);
            return;
        }
        
        if ( ship->class < CLOUD_CAR )
    	{
    	      send_to_char("&RThis isn't a land vehicle!\n\r",ch);
    	      return;
    	}
    	        
        
        if (ship->shipstate == SHIP_DISABLED)
    	{
    	     send_to_char("&RThe drive is disabled.\n\r",ch);
    	     return;
    	}
    	        
        if ( ship->energy <1 )
        {
              send_to_char("&RTheres not enough fuel!\n\r",ch);
              return;
        }
        
        if ( ( dir = get_door( argument ) ) == -1 )
        {
             send_to_char( "Usage: drive <direction>\n\r", ch );     
             return;
        }
        
        drive_ship( ch, ship, get_exit(get_room_index(ship->location), dir), 0 );

}

ch_ret drive_ship( CHAR_DATA *ch, SHIP_DATA *ship, EXIT_DATA  *pexit , int fall )
{
    ROOM_INDEX_DATA *in_room;
    ROOM_INDEX_DATA *to_room;
    ROOM_INDEX_DATA *from_room;
    ROOM_INDEX_DATA *original;
    char buf[MAX_STRING_LENGTH];
    char *txt;
    char *dtxt;
    ch_ret retcode;
    sh_int door, distance;
    bool drunk = FALSE;
    CHAR_DATA * rch;
    CHAR_DATA * next_rch;
    

    if ( !IS_NPC( ch ) )
      if ( IS_DRUNK( ch, 2 ) && ( ch->position != POS_SHOVE )
	&& ( ch->position != POS_DRAG ) )
	drunk = TRUE;

    if ( drunk && !fall )
    {
      door = number_door();
      pexit = get_exit( get_room_index(ship->location), door );
    }

#ifdef DEBUG
    if ( pexit )
    {
	sprintf( buf, "drive_ship: %s to door %d", ch->name, pexit->vdir );
	log_string( buf );
    }
#endif

    retcode = rNONE;
    txt = NULL;

    in_room = get_room_index(ship->location);
    from_room = in_room;
    if ( !pexit || (to_room = pexit->to_room) == NULL )
    {
	if ( drunk )
	  send_to_char( "You drive into a wall in your drunken state.\n\r", ch );
	 else
	  send_to_char( "Alas, you cannot go that way.\n\r", ch );
	return rNONE;
    }

    door = pexit->vdir;
    distance = pexit->distance;

    if ( IS_SET( pexit->exit_info, EX_WINDOW )
    &&  !IS_SET( pexit->exit_info, EX_ISDOOR ) )
    {
	send_to_char( "Alas, you cannot go that way.\n\r", ch );
	return rNONE;
    }

    if (  IS_SET(pexit->exit_info, EX_PORTAL) 
       && IS_NPC(ch) )
    {
        act( AT_PLAIN, "Mobs can't use portals.", ch, NULL, NULL, TO_CHAR );
	return rNONE;
    }

    if ( IS_SET(pexit->exit_info, EX_NOMOB)
	&& IS_NPC(ch) )
    {
	act( AT_PLAIN, "Mobs can't enter there.", ch, NULL, NULL, TO_CHAR );
	return rNONE;
    }

    if ( IS_SET(pexit->exit_info, EX_CLOSED)
    && (IS_SET(pexit->exit_info, EX_NOPASSDOOR)) )
    {
	if ( !IS_SET( pexit->exit_info, EX_SECRET )
	&&   !IS_SET( pexit->exit_info, EX_DIG ) )
	{
	  if ( drunk )
	  {
	    act( AT_PLAIN, "$n drives into the $d in $s drunken state.", ch,
		NULL, pexit->keyword, TO_ROOM );
	    act( AT_PLAIN, "You drive into the $d in your drunken state.", ch,
		NULL, pexit->keyword, TO_CHAR ); 
	  }
	 else
	  act( AT_PLAIN, "The $d is closed.", ch, NULL, pexit->keyword, TO_CHAR );
	}
       else
	{
	  if ( drunk )
	    send_to_char( "You hit a wall in your drunken state.\n\r", ch );
	   else
	    send_to_char( "Alas, you cannot go that way.\n\r", ch );
	}

	return rNONE;
    }

/*
    if ( distance > 1 )
	if ( (to_room=generate_exit(in_room, &pexit)) == NULL )
	    send_to_char( "Alas, you cannot go that way.\n\r", ch );
*/
    if ( room_is_private( ch, to_room ) )
    {
	send_to_char( "That room is private right now.\n\r", ch );
	return rNONE;
    }

    if ( !IS_IMMORTAL(ch)
    &&  !IS_NPC(ch)
    &&  ch->in_room->area != to_room->area )
    {
	if ( ch->top_level < to_room->area->low_hard_range )
	{
	    set_char_color( AT_TELL, ch );
	    switch( to_room->area->low_hard_range - ch->top_level )
	    {
		case 1:
		  send_to_char( "A voice in your mind says, 'You are nearly ready to go that way...'", ch );
		  break;
		case 2:
		  send_to_char( "A voice in your mind says, 'Soon you shall be ready to travel down this path... soon.'", ch );
		  break;
		case 3:
		  send_to_char( "A voice in your mind says, 'You are not ready to go down that path... yet.'.\n\r", ch);
		  break;
		default:
		  send_to_char( "A voice in your mind says, 'You are not ready to go down that path.'.\n\r", ch);
	    }
	    return rNONE;
	}
	else
	if ( ch->top_level > to_room->area->hi_hard_range )
	{
	    set_char_color( AT_TELL, ch );
	    send_to_char( "A voice in your mind says, 'There is nothing more for you down that path.'", ch );
	    return rNONE;
	}          
    }

    if ( !fall )
    {
        if ( IS_SET( to_room->room_flags, ROOM_INDOORS ) 
        || IS_SET( to_room->room_flags, ROOM_SPACECRAFT )  
        || to_room->sector_type == SECT_INSIDE ) 
	{
		send_to_char( "You can't drive indoors!\n\r", ch );
		return rNONE;
	}
        
        if ( IS_SET( to_room->room_flags, ROOM_NO_DRIVING ) ) 
	{
		send_to_char( "You can't take a vehicle through there!\n\r", ch );
		return rNONE;
	}

	if ( in_room->sector_type == SECT_AIR
	||   to_room->sector_type == SECT_AIR
	||   IS_SET( pexit->exit_info, EX_FLY ) )
	{
            if ( ship->class > CLOUD_CAR || ship->class == MOBILE_SUIT )
	    {
		send_to_char( "You'd need to fly to go there.\n\r", ch );
		return rNONE;
	    }
	}

	if ( in_room->sector_type == SECT_WATER_NOSWIM
	||   to_room->sector_type == SECT_WATER_NOSWIM 
	||   to_room->sector_type == SECT_WATER_SWIM 
	||   to_room->sector_type == SECT_UNDERWATER
	||   to_room->sector_type == SECT_OCEANFLOOR )
	{

	    if ( ship->class == MOBILE_SUIT )
	    {
		send_to_char( "You'd need a Coralskipper to go there.\n\r", ch );
		return rNONE;
	    }
	    	    
	}

	if ( IS_SET( pexit->exit_info, EX_CLIMB ) )
	{

	    if ( ship->class < CLOUD_CAR || ship->class == MOBILE_SUIT )
	    {
		send_to_char( "You need to fly or climb to get up there.\n\r", ch );
		return rNONE;
	    }
	}

    }

    if ( to_room->tunnel > 0 )
    {
	CHAR_DATA *ctmp;
	int count = 0;
	
	for ( ctmp = to_room->first_person; ctmp; ctmp = ctmp->next_in_room )
	  if ( ++count >= to_room->tunnel )
	  {
		  send_to_char( "There is no room for you in there.\n\r", ch );
		return rNONE;
	  }
    }

      if ( fall )
        txt = "falls";
      else
      if ( !txt )
      {
	  if ( ship->class == CLOUD_CAR )
	  {
	      txt = "flies";
	  }
	  if ( ship->class == LAND_SPEEDER )
	  {
	      txt = "drive";
	  }
	  if ( ship->class == WALKER || ship->class == MOBILE_SUIT  )
	  {
	      txt = "stomps";
	  }

      }
      sprintf( buf, "$n %ss the vehicle $T.", txt );
      act( AT_ACTION, buf, ch, NULL, dir_name[door], TO_ROOM );
      sprintf( buf, "You %s the vehicle $T.", txt );
      act( AT_ACTION, buf, ch, NULL, dir_name[door], TO_CHAR );
      sprintf( buf, "%s %ss %s.", ship->name, txt, dir_name[door] );
      echo_to_room( AT_ACTION , get_room_index(ship->location) , buf );

      extract_ship( ship );
      ship_to_room(ship, to_room->vnum );
      
      ship->location = to_room->vnum;
      ship->lastdoc = ship->location;
    
      if ( fall )
        txt = "falls";
      else
	  if ( ship->class == CLOUD_CAR )
	  {
	      txt = "flies in";
	  }
	  if ( ship->class == LAND_SPEEDER )
	  {
	      txt = "drives in";
	  }
	  if ( ship->class == WALKER || ship->class == MOBILE_SUIT  )
	  {
	      txt = "stomps in";
	  }

      switch( door )
      {
      default: dtxt = "somewhere";	break;
      case 0:  dtxt = "the south";	break;
      case 1:  dtxt = "the west";	break;
      case 2:  dtxt = "the north";	break;
      case 3:  dtxt = "the east";	break;
      case 4:  dtxt = "below";		break;
      case 5:  dtxt = "above";		break;
      case 6:  dtxt = "the south-west";	break;
      case 7:  dtxt = "the south-east";	break;
      case 8:  dtxt = "the north-west";	break;
      case 9:  dtxt = "the north-east";	break;
      }

    sprintf( buf, "%s %s from %s.", ship->name, txt, dtxt );
    echo_to_room( AT_ACTION , get_room_index(ship->location) , buf );
    
    for ( rch = ch->in_room->last_person ; rch ; rch = next_rch )
    { 
        next_rch = rch->prev_in_room;
        original = rch->in_room;
        char_from_room( rch );
        char_to_room( rch, to_room );
        do_look( rch, "auto" );
        char_from_room( rch );
        char_to_room( rch, original );
    }
    
/*
    if (  CHECK FOR FALLING HERE
    &&   fall > 0 )
    {
	if (!IS_AFFECTED( ch, AFF_FLOATING )
	|| ( ch->mount && !IS_AFFECTED( ch->mount, AFF_FLOATING ) ) )
	{
	  set_char_color( AT_HURT, ch );
	  send_to_char( "OUCH! You hit the ground!\n\r", ch );
	  WAIT_STATE( ch, 20 );
	  retcode = damage( ch, ch, 50 * fall, TYPE_UNDEFINED );
	}
	else
	{
	  set_char_color( AT_MAGIC, ch );
	  send_to_char( "You lightly float down to the ground.\n\r", ch );
	}
    }

*/    
    return retcode;

}

void do_bomb( CHAR_DATA *ch, char *argument )
{

}

void do_chaff( CHAR_DATA *ch, char *argument )
{
    int chance;
    SHIP_DATA *ship;
    
   
        if (  (ship = ship_from_cockpit(ch->in_room->vnum))  == NULL )
        {
            send_to_char("&RYou must be in the cockpit of a ship to do that!\n\r",ch);
            return;
        }
        
        if ( ship->class > MOBILE_SUIT )
    	        {
    	            send_to_char("&RThis isn't a spacecraft!\n\r",ch);
    	            return;
    	        }
    	        
        
        if (  (ship = ship_from_coseat(ch->in_room->vnum))  == NULL )
        {
            send_to_char("&RThe controls are at the copilots seat!\n\r",ch);
            return;
        }
        
                if ( autofly(ship) )
    	        {
    	            send_to_char("&RYou'll have to turn the autopilot off first...\n\r",ch);
    	            return;
    	        }
    	        
                if (ship->shipstate == SHIP_HYPERSPACE)
                {
                  send_to_char("&RYou can only do that in realspace!\n\r",ch);
                  return;   
                }
    	        if (ship->shipstate == SHIP_DOCKED)
    	        {
    	            send_to_char("&RYou can't do that until after you've launched!\n\r",ch);
    	            return;
    	        }
                if (ship->chaff <= 0 )
    	        {
    	            send_to_char("&RYou don't have any chaff to release!\n\r",ch);
    	            return;
    	        }
                chance = IS_NPC(ch) ? ch->top_level
                 : (int)  (ch->pcdata->learned[gsn_weaponsystems]) ;
        if ( number_percent( ) > chance )
        {
            send_to_char("&RYou can't figure out which switch it is.\n\r",ch);
            learn_from_failure( ch, gsn_weaponsystems );
    	   return;	
        }
    
    ship->chaff--;
    
    ship->chaff_released++;
        
    send_to_char( "You flip the chaff release switch.\n\r", ch);
    act( AT_PLAIN, "$n flips a switch on the control pannel", ch,
         NULL, argument , TO_ROOM );
    echo_to_cockpit( AT_YELLOW , ship , "A burst of chaff is released from the ship.");
	  
    learn_from_success( ch, gsn_weaponsystems );

}

bool autofly( SHIP_DATA *ship )
{
 
     if (!ship)
        return FALSE;
     
     if ( ship->type == MOB_SHIP )
        return TRUE;
     
     if ( ship->autopilot )
        return TRUE;
     
     return FALSE;   
        
}


void do_transship( CHAR_DATA *ch ,char *argument )
{
    char arg1[MAX_INPUT_LENGTH];
        char arg2[MAX_INPUT_LENGTH];
        int arg3;
    SHIP_DATA *ship;

    if ( IS_NPC( ch ) )
    {
        send_to_char( "Huh?\n\r", ch );
        return;
    }

    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );

    ship = get_ship( arg1 );
        if ( !ship )
    {
        send_to_char( "No such ship.\n\r", ch );
        return;
    }
     
        arg3 = atoi( arg2 );
     
         if ( arg1[0] == '\0' || arg2[0] == '\0' || arg1[0] == '\0' )
   {
        send_to_char( "Usage: transship <ship> <vnum>\n\r", ch );
        return;
    }
           
         ship->shipyard = arg3;
     ship->shipstate = SHIP_READY;
     
     if ( ship->class != SHIP_PLATFORM && ship->type != MOB_SHIP )   
     {
           extract_ship( ship );
           ship_to_room( ship , ship->shipyard );
           ship->location = ship->shipyard;
           ship->lastdoc = ship->location;
           ship->shipstate = SHIP_DOCKED;
     }
     if (ship->starsystem)
        ship_from_starsystem( ship, ship->starsystem );
           
     save_ship(ship);
         send_to_char( "Ship Transfered.\n\r", ch );
}




/* Generic Pilot Command To use as template

void do_hmm( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    int chance;
    SHIP_DATA *ship;
    
    strcpy( arg, argument );    
    
    switch( ch->substate )
    { 
    	default:
    	        if (  (ship = ship_from_cockpit(ch->in_room->vnum))  == NULL )
    	        {
    	            send_to_char("&RYou must be in the cockpit of a ship to do that!\n\r",ch);
    	            return;
    	        }
                if (ship->shipstate == SHIP_HYPERSPACE)
                {
                  send_to_char("&RYou can only do that in realspace!\n\r",ch);
                  return;   
                }
                if (ship->shipstate == SHIP_DISABLED)
    	        {
    	            send_to_char("&RThe ships drive is disabled. Unable to manuever.\n\r",ch);
    	            return;
    	        }
    	        if (ship->shipstate == SHIP_DOCKED)
    	        {
    	            send_to_char("&RYou can't do that until after you've launched!\n\r",ch);
    	            return;
    	        }
    	        if (ship->shipstate != SHIP_READY)
    	        {
    	            send_to_char("&RPlease wait until the ship has finished its current maneuver.\n\r",ch);
    	            return;
    	        }
        
                if ( ship->energy <1 )
    	        {
    	           send_to_char("&RTheres not enough fuel!\n\r",ch);
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
    		   send_to_char( "&G\n\r", ch);
    		   act( AT_PLAIN, "$n does  ...", ch,
		        NULL, argument , TO_ROOM );
		   echo_to_room( AT_YELLOW , get_room_index(ship->cockpit) , "");
    		   add_timer ( ch , TIMER_DO_FUN , 1 , do_hmm , 1 );
    		   ch->dest_buf = str_dup(arg);
    		   return;
	        }
	        send_to_char("&RYou fail to work the controls properly.\n\r",ch);
	        if ( ship->class == FIGHTER_SHIP )
                    learn_from_failure( ch, gsn_starfighters );
                if ( ship->class == MIDSIZE_SHIP )
    	            learn_from_failure( ch, gsn_midships );
                if ( ship->class == CAPITAL_SHIP )
                    learn_from_failure( ch, gsn_capitalships );
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
    		if ( (ship = ship_from_cockpit(ch->in_room->vnum)) == NULL )
    		      return;    		                                   
    	        send_to_char("&Raborted.\n\r", ch);
    	        echo_to_room( AT_YELLOW , get_room_index(ship->cockpit) , "");
    		if (ship->shipstate != SHIP_DISABLED)
    		   ship->shipstate = SHIP_READY;
    		return;
    }
    
    ch->substate = SUB_NONE;
    
    if ( (ship = ship_from_cockpit(ch->in_room->vnum)) == NULL )
    {  
       return;
    }

    send_to_char( "&G\n\r", ch);
    act( AT_PLAIN, "$n does  ...", ch,
         NULL, argument , TO_ROOM );
    echo_to_room( AT_YELLOW , get_room_index(ship->cockpit) , "");

         
    if ( ship->class == FIGHTER_SHIP )
        learn_from_success( ch, gsn_starfighters );
    if ( ship->class == MIDSIZE_SHIP )
        learn_from_success( ch, gsn_midships );
    if ( ship->class == CAPITAL_SHIP )
        learn_from_success( ch, gsn_capitalships );
    	
}

void do_hmm( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    int chance;
    SHIP_DATA *ship;
    
   
        if (  (ship = ship_from_cockpit(ch->in_room->vnum))  == NULL )
        {
            send_to_char("&RYou must be in the cockpit of a ship to do that!\n\r",ch);
            return;
        }
        
                if (ship->shipstate == SHIP_HYPERSPACE)
                {
                  send_to_char("&RYou can only do that in realspace!\n\r",ch);
                  return;   
                }
                if (ship->shipstate == SHIP_DISABLED)
    	        {
    	            send_to_char("&RThe ships drive is disabled. Unable to manuever.\n\r",ch);
    	            return;
    	        }
    	        if (ship->shipstate == SHIP_DOCKED)
    	        {
    	            send_to_char("&RYou can't do that until after you've launched!\n\r",ch);
    	            return;
    	        }
    	        if (ship->shipstate != SHIP_READY)
    	        {
    	            send_to_char("&RPlease wait until the ship has finished its current maneuver.\n\r",ch);
    	            return;
    	        } 
        
        if ( ship->energy <1 )
        {
              send_to_char("&RTheres not enough fuel!\n\r",ch);
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
        if ( number_percent( ) > chance )
        {
            send_to_char("&RYou fail to work the controls properly.\n\r",ch);
            if ( ship->class == FIGHTER_SHIP )
               learn_from_failure( ch, gsn_starfighters );
            if ( ship->class == MIDSIZE_SHIP )   
               learn_from_failure( ch, gsn_midships );
            if ( ship->class == CAPITAL_SHIP )
                learn_from_failure( ch, gsn_capitalships );
    	   return;	
        }
        
    send_to_char( "&G\n\r", ch);
    act( AT_PLAIN, "$n does  ...", ch,
         NULL, argument , TO_ROOM );
    echo_to_room( AT_YELLOW , get_room_index(ship->cockpit) , "");
	  
    
    
    if ( ship->class == FIGHTER_SHIP )
        learn_from_success( ch, gsn_starfighters );
    if ( ship->class == MIDSIZE_SHIP )
        learn_from_success( ch, gsn_midships );
    if ( ship->class == CAPITAL_SHIP )
        learn_from_success( ch, gsn_capitalships );
    	
}

*/

void fread_modules( SHIP_DATA *ship, FILE *fp )
{
	char * word;
	int	module;

	ship->modules = 0;

	for ( module=1; module<MAX_MODULES; module++ )
	{
		word   = feof( fp ) ? "End" : fread_word( fp );
		if ( !str_cmp( word, "End" ) )
			return;
		else
		{
			ship->module_vnum[module] = fread_number( fp );
			ship->modules += 1;
		}
	}
	return;

}

void update_ship_modules( SHIP_DATA *ship )
{
	AFFECT_DATA *paf;
	int	module;

	ship->hyperspeed = 0;
	ship->realspeed = 0;
	ship->maxshield = 0;
	ship->lasers = 0;
	ship->tractorbeam = 0;
	ship->maxmissiles = 0;
	ship->maxrockets = 0;
	ship->maxtorpedos = 0;
	ship->maxenergy = 0;
	ship->comm = 0;
	ship->sensor = 0;
	ship->astro_array = 0;
        ship->chaff = 0;
	ship->maxchaff = 0;
	ship->manuever = 0;
	ship->laserdamage = 0;
        ship->ions = 0;
        ship->overdrive = 0;
        ship->armor = 0;
        ship->maxcargo = 0;
        ship->mlaunchers = 0;
        ship->tlaunchers = 0;
        ship->rlaunchers = 0;
        ship->cloak = 0;

        if(IS_SET(ship->flags, SHIPFLAG_SIMULATOR))
            REMOVE_BIT(ship->flags, SHIPFLAG_SIMULATOR);
 	for ( module=1; module<=ship->modules; module++ )
	{
		for ( paf = get_obj_index(ship->module_vnum[module])->first_affect; paf; paf = paf->next )
		{
			if ( paf->location == APPLY_HYPERSPEED)
				ship->hyperspeed += paf->modifier;
			if ( paf->location == APPLY_REALSPEED)
				ship->realspeed += paf->modifier;
			if ( paf->location == APPLY_MAXSHIELD)
				ship->maxshield += paf->modifier;
			if ( paf->location == APPLY_LASERS)
				ship->lasers += paf->modifier;
			if ( paf->location == APPLY_TRACTORBEAM)
				ship->tractorbeam += paf->modifier;
			if ( paf->location == APPLY_MAXMISSILES)
				ship->maxmissiles += paf->modifier;
			if ( paf->location == APPLY_MAXROCKETS)
				ship->maxrockets += paf->modifier;
			if ( paf->location == APPLY_MAXTORPEDOS)
				ship->maxtorpedos += paf->modifier;
			if ( paf->location == APPLY_MAXENERGY)
				ship->maxenergy += paf->modifier;
			if ( paf->location == APPLY_COMM)
			{
				if ( paf->modifier > ship->comm )							
					ship->comm = paf->modifier;
			}
			if ( paf->location == APPLY_SENSOR)
			{
				if ( paf->modifier > ship->sensor )			
					ship->sensor = paf->modifier;
			}
			if ( paf->location == APPLY_ASTRO_ARRAY)
			{
				if ( paf->modifier > ship->astro_array )
					ship->astro_array = paf->modifier;
			}
			if ( paf->location == APPLY_CHAFF)
			{
				ship->chaff += paf->modifier;
				ship->maxchaff += paf->modifier;
			}
			if ( paf->location == APPLY_MANUEVER)
				ship->manuever += paf->modifier;
			if ( paf->location == APPLY_LASERDAMAGE)
				ship->laserdamage += paf->modifier;
                        if ( paf->location == APPLY_IONS)
                                ship->ions += paf->modifier;
                        if ( paf->location == APPLY_OVERDRIVE)
                                ship->overdrive = 1;
                        if ( paf->location == APPLY_ARMOR )
                                ship->armor += paf->modifier;
                        if ( paf->location == APPLY_CARGO )
                                ship->maxcargo += paf->modifier;
                        if ( paf->location == APPLY_MLAUNCHER)
                                ship->mlaunchers += paf->modifier;
                        if ( paf->location == APPLY_TLAUNCHER)
                                ship->tlaunchers += paf->modifier;
                        if ( paf->location == APPLY_RLAUNCHER)
                                ship->rlaunchers += paf->modifier;
                        if ( paf->location == APPLY_SIMULATOR )
                                SET_BIT(ship->flags, SHIPFLAG_SIMULATOR);
                        if ( paf->location == APPLY_CLOAK )
                                ship->cloak += 1;
		}
	}
	if (ship->lasers > 0 && ship->laserdamage == 0)
		ship->laserdamage = 1;
        if(ship->mlaunchers > 2)
            ship->mlaunchers = 2;
        if(ship->tlaunchers > 2)
            ship->tlaunchers = 2;
        if(ship->rlaunchers > 2)
            ship->rlaunchers = 2;
	return;
}

void do_install_module( CHAR_DATA *ch, char *argument )
{

    char arg1[MAX_INPUT_LENGTH];
	char arg2[MAX_INPUT_LENGTH];
    OBJ_DATA *obj;
    SHIP_DATA *ship;
    

    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );

    if ( arg1[0] == '\0' )
    {
	send_to_char( "install what?\n\r", ch );
	return;
    }

    if ( arg2[0] == '\0' )
    {
	send_to_char( "in what?\n\r", ch );
	return;
    }

	ship = ship_in_room( ch->in_room , arg2 );    
        if ( !ship )            
	{    
		act( AT_PLAIN, "I see no $T here.", ch, NULL, argument, TO_CHAR );        
		return;       
        }

	if ( !check_pilot( ch , ship ) || !str_cmp( ship->owner , "Public" ) )    	        
	{    
		send_to_char("&RHey, thats not your ship!\n\r",ch);    	
		return;    	
	}

	if (ship->maxmodules == ship->modules )
	{
		send_to_char( "The ship has no free slots!\n\r", ch );
		return;
	}


        if ( ms_find_obj(ch) )
           return;

	if ( ( obj = get_obj_carry( ch, arg1 ) ) == NULL )
	{
	    send_to_char( "You do not have that item.\n\r", ch );
	    return;
	}


	if ( !(obj->item_type==ITEM_FIGHTERCOMP || obj->item_type==ITEM_MIDCOMP
	     || obj->item_type==ITEM_CAPITALCOMP || obj->item_type == ITEM_FRIGATECOMP
             || obj->item_type==ITEM_SCAPITALCOMP) )
	{
		send_to_char("That isn't a ship module.\n\r",ch);
		return;
	}

	if ( obj->item_type==ITEM_FIGHTERCOMP && ship->class != FIGHTER_SHIP )
	{
		send_to_char( "That module is designed for a fighter class vessel.\n\r",ch);
		return;
	}

	if ( obj->item_type==ITEM_MIDCOMP && ship->class != MIDSIZE_SHIP )
	{
		send_to_char( "That module is designed for a midship class vessel.\n\r",ch);
		return;
	}
       
        if ( obj->item_type==ITEM_FRIGATECOMP && ship->class != FRIGATE_SHIP )
        {
                send_to_char("That module is designed for a frigate class vessel.\n\r", ch);
                return;
        }

	if ( obj->item_type==ITEM_CAPITALCOMP && ship->class != CAPITAL_SHIP )
	{
		send_to_char( "That module is designed for a capital class vessel.\n\r",ch);
		return;
	}
        if( obj->item_type==ITEM_SCAPITALCOMP && ship->class != SUPERCAPITAL_SHIP )
        {
                send_to_char( "That module is designed for a super class vessel.\n\r", ch);
                return;
        }

	if ( !can_drop_obj( ch, obj ) )
	{
	    send_to_char( "You can't let go of it.\n\r", ch );
	    return;
	}

	if ( module_type_install(obj, ship) )
	{
		send_to_char( "Your ship cannot carry another one of those!\n\r", ch);
		return;
	}

	separate_obj( obj );

	act( AT_ACTION, "$n installs $p.", ch, obj, NULL, TO_ROOM );
	act( AT_ACTION, "You install $p.", ch, obj, NULL, TO_CHAR );

	ship->modules += 1;
	ship->module_vnum[ship->modules] = obj->pIndexData->vnum;
	extract_obj(obj);
	save_ship( ship );
	update_ship_modules( ship );
	  
    return;

}


bool module_type_install(OBJ_DATA *obj, SHIP_DATA *ship)
{
	int	module;
	AFFECT_DATA	*pafship;
	AFFECT_DATA *pafobj;


	for ( module=1; module<=ship->modules; module++ )
	{
		for ( pafship = get_obj_index(ship->module_vnum[module])->first_affect; pafship; pafship = pafship->next )
		{
			for ( pafobj = obj->pIndexData->first_affect; pafobj; pafobj = pafobj->next )
			{
				if ( pafship->location == pafobj->location &&
					( pafobj->location == APPLY_HASSEMBLY || pafobj->location == APPLY_ENGINE
					|| pafobj->location == APPLY_GENERATOR || pafobj->location == APPLY_TRACTORBEAM
                                        || pafobj->location == APPLY_OVERDRIVE || pafobj->location == APPLY_LASERBATTERY 
                                        || pafobj->location == APPLY_SIMULATOR || pafobj->location == APPLY_CLOAK ) )
					return TRUE;
				}         
                }
	}
	return FALSE;
}


void do_remove_module( CHAR_DATA *ch, char *argument )
{

    char arg1[MAX_INPUT_LENGTH];
	char arg2[MAX_INPUT_LENGTH];
    OBJ_DATA *obj;
	OBJ_INDEX_DATA *obj_index;
    SHIP_DATA *ship;
	int	module, rmodule;


    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );

    if ( arg1[0] == '\0' )
    {
		send_to_char( "remove what?\n\r", ch );
		return;
    }

    if ( arg2[0] == '\0' )
    {
		send_to_char( "from what?\n\r", ch );
		return;
    }

	ship = ship_in_room( ch->in_room , arg2 );    
    if ( !ship )            
	{    
		act( AT_PLAIN, "I see no $T here.", ch, NULL, argument, 
TO_CHAR );        
		return;       
    }

	if ( !check_pilot( ch , ship ) || !str_cmp( ship->owner , "Public" 
) )    	        
	{    
		send_to_char("&RHey, thats not your ship!\n\r",ch);    	
		return;    	
	}

	for ( module = 1; module <= ship->modules; module++ )
	{
		obj_index=get_obj_index( ship->module_vnum[module] );
		if (!obj_index)
		{
			bug( "Remove module: cannot find module!\n\r", 0 );
			send_to_char("No such module installed\n\r",ch);
			return;
		}
		if ( nifty_is_name( arg1, obj_index->name ) )
		{
			obj = create_object( obj_index, 100 );
                        if(obj->weight > can_carry_w(ch))
                        {
                           send_to_char("You can't possibly remove that. Its much too heavy.\n\r", ch);
                           extract_obj(obj);
                           return;
                        }
			act( AT_ACTION, "$n removes $p.", ch, obj, NULL, TO_ROOM );	
			act( AT_ACTION, "You remove $p.", ch, obj, NULL, TO_CHAR );
			obj = obj_to_char( obj, ch );
			
			for ( rmodule = module; rmodule < ship->modules; rmodule++ )			
			{				
				ship->module_vnum[rmodule] = ship->module_vnum[rmodule+1];			
			}
			ship->modules = ship->modules -1;
			update_ship_modules( ship );
			save_ship(ship);
			return;
		}
	}
	send_to_char("No such module installed\n\r",ch);
	return;
}

void do_show_modules( CHAR_DATA *ch, char *argument )
{

    char arg[MAX_INPUT_LENGTH];
	SHIP_DATA *ship;
	int	module;
	char buf[MAX_STRING_LENGTH];
	OBJ_INDEX_DATA *obj_index;

    argument = one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
		send_to_char( "check what ship?\n\r", ch );
		return;
    }

	ship = ship_in_room( ch->in_room , arg );    
    if ( !ship )            
	{    
		act( AT_PLAIN, "I see no $T here.", ch, NULL, argument, 
TO_CHAR );        
		return;       
    }
/*
	if ( !check_pilot( ch , ship ) || !str_cmp( ship->owner , "Public" 
) )    	        
	{    
		send_to_char("&RHey, thats not your ship!\n\r",ch);    	
		return;    	
	}
*/
	
    sprintf( buf, "Modules installed on %s:\n\r\n\r", ship->name );
	send_to_char(buf,ch);
	if (ship->modules == 0)
	{
		send_to_char( "No Modules installed.\n\r",ch);
		return;
	}
	for ( module = 1; module <= ship->modules; module++ )
	{
		obj_index=get_obj_index( ship->module_vnum[module] );
		if (!obj_index)
		{
			bug( "Show module: cannot find module!\n\r", 0 );
			return;
		}
		sprintf(buf,"%s\n\r",obj_index->short_descr);
		send_to_char(buf,ch);
	}
	return;
}

/* Parnic's docking. Originally written by Soulbender */
void do_docking( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_STRING_LENGTH];
    char arg[MAX_INPUT_LENGTH];
    int chance;
    SHIP_DATA *ship;
    SHIP_DATA *docktarget;
    
    strcpy( arg, argument );
    
    	        if ( (ship = ship_from_cockpit(ch->in_room->vnum)) == NULL )
    	        {
    	            send_to_char("&RYou must be in the cockpit of a ship to do that!\n\r",ch);
    	            return;
    	        }
    	        
    	        if ( ship->class > MOBILE_SUIT )
    	        {
    	            send_to_char("&RThis isn't a spacecraft!\n\r",ch);
    	            return;
    	        }
    	        
    	        if ( (ship = ship_from_pilotseat(ch->in_room->vnum)) == NULL )
    	        {
    	            send_to_char("&RYou need to be in the pilot seat!\n\r",ch);
    	            return;
    	        }
    	        
    	        if ( autofly(ship) )
    	        {
    	            send_to_char("&RYou'll have to turn off the ship's autopilot first.\n\r",ch);
    	            return;
    	        }
    	        
                if  ( ship->class == SHIP_PLATFORM )
                {
                   send_to_char( "&RYou can't dock platforms!\n\r" , ch );
                   return;
                }   
    
    	        if (ship->class == CAPITAL_SHIP)
    	        {
    	            send_to_char("&RCapital ships are too big to dock. You'll have to take a shuttle.\n\r",ch);
		    return;
    	        }
    	        if (ship->shipstate == SHIP_DISABLED)
    	        {
    	            send_to_char("&RThe ship's drive is disabled. Unable to dock.\n\r",ch);
    	            return;
    	        }
                if( IS_SET(ship->flags, SHIPFLAG_CLOAKED))
                {
                   send_to_char("&RThey can't see you to perform in the docking operation.\n\r", ch); 
                   return;
                }
    	        if (IS_DOCKED(ship))
    	        {
    	            send_to_char("&RThe ship is already docked!\n\r",ch);
    	            return;
    	        }
    	                
               if (ship->shipstate == SHIP_HYPERSPACE)
               {
                  send_to_char("&RYou can only do that in realspace!\n\r",ch);
                  return;   
               }

    	        if (ship->shipstate != SHIP_READY)
    	        {
    	            send_to_char("&RPlease wait until the ship has finished its current manuever.\n\r",ch);
    	            return;
    	        }
    	        if ( ship->starsystem == NULL )
    	        {
    	            send_to_char("&Dock with whom?",ch);
    	            return;
    	        }
    	        
    	        if ( ship->energy < (25 + 25*ship->class) )
    	        {
    	           send_to_char("&RThere's not enough fuel!\n\r",ch);
    	           return;
    	        }
    	        
    	        if ( argument[0] == '\0' )
    	        {  
    	           set_char_color(  AT_CYAN, ch );
    	           ch_printf(ch, "Dock with who?\n\r");   	           
    		     return;
    	        }
    	        
    	        docktarget = get_ship_here( arg, ship->starsystem );
                if (  docktarget == NULL || IS_SET(docktarget->flags, SHIPFLAG_CLOAKED))
                {
                    send_to_char("&RThat ship isn't here!\n\r",ch);
                    return;
                }

		if (IS_DOCKED(docktarget))
		{
		   send_to_char("&RThat ship is already docked!\n\r",ch);
		   return;
		}
		
    	            if ( docktarget == ship )
    	            {
    	                send_to_char("&RYou can't dock your ship inside itself!\n\r",ch);
    	                return;
    	            } 
                   if(docktarget->class > MIDSIZE_SHIP && ship->class <= MIDSIZE_SHIP)
                   {
                        send_to_char("&RThat ship is too large to dock with. Try landing.\n\r", ch);
                        return;
                   }
                   if(ship->class > MIDSIZE_SHIP && docktarget->class <= MIDSIZE_SHIP)
                   {
                        send_to_char("&RYour ship is too large to dock with that. Tractor it.\n\r", ch);
                        return;
                   }                    
                   if (  (docktarget->vx > ship->vx + 200) || (docktarget->vx < ship->vx - 200) ||
    	                  (docktarget->vy > ship->vy + 200) || (docktarget->vy < ship->vy - 200) ||
    	                  (docktarget->vz > ship->vz + 200) || (docktarget->vz < ship->vz - 200) )
    	            {
    	                send_to_char("&R That ship is too far away! You'll have to fly a little closer.\n\r",ch);
    	                return;
    	            }       
                                
                chance = IS_NPC(ch) ? ch->top_level
	                 : (int)  (ch->pcdata->learned[gsn_docking]) ;
                if ( number_percent( ) < chance )
    		{
    		   set_char_color( AT_GREEN, ch );
		   sprintf( buf, "You are being docked by %s.\n\r"
		   "Docking sequence initiated.\n\r", ship->name);
    		   echo_to_ship( AT_YELLOW, docktarget, buf );
    		   send_to_char( "Docking sequence initiated.\n\r", ch);
    		   act( AT_PLAIN, "$n begins the docking sequence.", ch,
		        NULL, argument , TO_ROOM );
		   echo_to_ship( AT_YELLOW , ship , "The ship slowly begins its docking approach.");
    		   ship->shipstate2 = SHIP_DOCK;
    		   ship->currspeed = 0;
	           docktarget->shipstate2 = SHIP_DOCK;
	           docktarget->currspeed = 0;
	           ship->docked_ship = docktarget;
	           docktarget->docked_ship = ship;
	           learn_from_success( ch, gsn_docking );
                   return;
	        }
	        send_to_char("You fail to work the controls properly.\n\r",ch);
	        learn_from_failure( ch, gsn_docking );
    	   	return;	
}

void dockship( SHIP_DATA *ship )
{    
    SHIP_DATA *docktarget;
    char buf[MAX_STRING_LENGTH];
    
    docktarget = ship->docked_ship;
   
    ship->shipstate2 = SHIP_DOCK_3;
    ship->vx = docktarget->vx;
    ship->vy = docktarget->vy;
    ship->vz = docktarget->vz;
    docktarget->shipstate2 = SHIP_DOCK_3;
    echo_to_room( AT_YELLOW , get_room_index(ship->pilotseat), "Docking sequence complete.");
    echo_to_ship( AT_YELLOW , ship , "You feel a slight thud as the ship locks in with the next."); 
    echo_to_room( AT_YELLOW , get_room_index(docktarget->pilotseat), "Docking sequence complete.");
    echo_to_ship( AT_YELLOW , docktarget , "You feel a slight thud as the ship locks in with the next.");
    sprintf( buf ,"%s and %s have docked." , ship->name, docktarget->name  );
    echo_to_system( AT_YELLOW, ship, buf , NULL );
    
    sprintf( buf, "%s docks with your ship.", ship->name );
    echo_to_room( AT_YELLOW , get_room_index(ship->location) , buf );
    
    ship->energy = ship->energy - 25 - 25*ship->class;
    
    save_ship(ship);   
}

void do_detach( CHAR_DATA *ch, char *argument)
{
    SHIP_DATA *ship;
    SHIP_DATA *docked_by;
    char buf[MAX_STRING_LENGTH];
    int price = 0;
    
    if ( (ship = ship_from_pilotseat(ch->in_room->vnum)) == NULL )
    {
       send_to_char("&RYou don't seem to be in the pilot seat!\n\r",ch);
       return;
    }

    if (!IS_DOCKED(ship))
    {
       send_to_char("&RThere doesn't seem to be any ships docked to you.\r\n", ch);
       return;
    }
    docked_by = ship->docked_ship;
    
    ship->docked_ship = NULL;
    ship->shipstate2 = SHIP_READY;
    docked_by->docked_ship = NULL;
    docked_by->shipstate2 = SHIP_READY;
    
    sprintf(buf, "You hear air escaping as the airlocks open, and %s detaches.", docked_by->name);
    echo_to_ship( AT_YELLOW, ship, buf);
    sprintf(buf, "You hear air escaping as the airlocks open, and %s detaches.", ship->name);
    echo_to_ship( AT_YELLOW, docked_by, buf);

    if ( ( ship->class == FRIGATE_SHIP || ship->class == CAPITAL_SHIP || ship->class == MOBILE_SUIT) && docked_by->class > ship->class )
    {
	price += ( ship->maxhull-ship->hull );
        price += ( 50 * (ship->maxmissiles-ship->missiles) );
        price += ( 75 * (ship->maxtorpedos-ship->torpedos) );
        price += ( 150 * (ship->maxrockets-ship->rockets) );
        price += ( 500 * (ship->autoammomax-ship->autoammo) );
        if (ship->shipstate == SHIP_DISABLED )
          price += 2000;
        if ( ship->missilestate == MISSILE_DAMAGED )
          price += 1000;
        if ( ship->ionstate == LASER_DAMAGED )
          price += 1000;
        if ( ship->statet0 == LASER_DAMAGED )
          price += 500;
        if ( ship->statet1 == LASER_DAMAGED )
          price += 500;
        if ( ship->statet2 == LASER_DAMAGED )
          price += 500;
    	if ( ch->pcdata && ch->pcdata->clan && !str_cmp(ch->pcdata->clan->name,ship->owner) ) 
        {
          if ( ch->pcdata->clan->funds < price )
          {
            ch_printf(ch, "&R%s doesn't have enough funds to repair and rearm  this ship.\n\r", ch->pcdata->clan->name );
            return;
          }
    
          ch->pcdata->clan->funds -= price;
          ch_printf(ch, "&GIt costs %s %ld credits to repair this ship for launch.\n\r", ch->pcdata->clan->name, price );   
        }
        else if ( str_cmp( ship->owner , "Public" ) )
        {
          if ( ch->gold < price )
          {
            ch_printf(ch, "&RYou don't have enough funds to repair and rearm your ship.\n\r");
            return;
          }
          ch->gold -= price;
          ch_printf(ch, "&GYou pay %ld credits to ready the ship for launch.\n\r", price );   
        }
        ship->autoammo = ship->autoammomax;
        ship->energy = ship->maxenergy;
        ship->chaff = ship->maxchaff;
        ship->missiles = ship->maxmissiles;
        ship->torpedos = ship->maxtorpedos;
        ship->rockets = ship->maxrockets;
        ship->hull = ship->maxhull;
        ship->missilestate = MISSILE_READY;
       	ship->statet0 = LASER_READY;
        ship->statet1 = LASER_READY;
        ship->statet2 = LASER_READY;
        ship->ionstate = LASER_READY;
                        save_ship(ship);
    }

}

void do_remoteopenbay( CHAR_DATA *ch, char *argument )
{
            char arg[MAX_INPUT_LENGTH];
    int chance;
    SHIP_DATA *ship;
    SHIP_DATA *target;
    
 
                if (arg[0] == '\0')
    	        {
    	            send_to_char("&RYou need to specify a target!\n\r",ch);
    	            return;
    	        }
    strcpy( arg, argument );    

        if (  (ship = ship_from_cockpit(ch->in_room->vnum))  == NULL )
        {
                send_to_char("&RYou must be in the cockpit of a ship to do that!\n\r",ch);

                return;
        }

                     target = get_ship_here( arg, ship->starsystem );
                if (  target == NULL )
                {
                    send_to_char("&RThat ship isn't here!\n\r",ch);
                    return;
                }

    	        if ( !check_pilot( ch , target ) )
    	        {
    	            send_to_char("&RHey, thats not your ship!!\n\r",ch);
    	            return;
    	        }

         
        chance = IS_NPC(ch) ? ch->top_level   : (int)  (ch->pcdata->learned[gsn_shipsystems]) ;
        if ( number_percent( ) > chance )
        {
                send_to_char("&RYour notsure which switch to flip.\n\r",ch);
                learn_from_failure( ch, gsn_shipsystems );
                return;
        }   
           
        act( AT_PLAIN, "$n flips a switch on the control panel.", ch,
        NULL, argument , TO_ROOM );
         
        if (target->bayopen)
        {
                target->bayopen = FALSE;
                echo_to_cockpit( AT_YELLOW , ship, "Closing bay doors.");
        }
        else
        {  
        target->bayopen = TRUE;
        echo_to_cockpit( AT_YELLOW , ship, "Opening Bay Doors.");
       }
        learn_from_success( ch, gsn_shipsystems );


}

void do_interdict(CHAR_DATA *ch, char *argument )
{
    SHIP_DATA *ship;
    SHIP_DATA *target;
    char buf[MAX_STRING_LENGTH];
        
        if (  (ship = ship_from_cockpit(ch->in_room->vnum))  == NULL )
        {
            send_to_char("&RYou must be in the cockpit of a ship to do that!\n\r",ch);
            return;
        }
        
        if (  (ship = ship_from_pilotseat(ch->in_room->vnum))  == NULL )
        {
            send_to_char("&RYou must be in the pilots seat!\n\r",ch);
            return;
        }

        if (  ship->interdict < 1 )
        {
            send_to_char("&RNo Interdiction Field Generators Installed.\n\r",ch);
            return;
        }
        
        if ( ! check_pilot(ch,ship) )
       	     {
       	       send_to_char("&RHey! Thats not your ship!\n\r",ch);
       	       return;
       	     }
  
        //Originally had code to target specific ship for interdiction
        //realized bad rp, removed left target pointer incase i 
        //change mid, duping the ship and target so the echotosystem doesn't fail.

        target = ship;
         
        act( AT_PLAIN, "$n flips a switch on the control panel.", ch, NULL, argument , TO_ROOM );

        if (ship->interdictactive == TRUE)
        {
           ship->interdictactive = FALSE;
           send_to_char( "&GYou toggle the interdiction field.\n\r", ch);
           sprintf( buf, "%s has deactivated an interdiction field!" , ship->name);
           echo_to_system( AT_ORANGE , ship , buf , target );
           echo_to_cockpit( AT_YELLOW , ship , "Interdiction Field Deactivated!");
        }
        else
        {
           ship->interdictactive = TRUE;
           ship->autorecharge = FALSE;
           send_to_char( "&GYou toggle the interdiction Field.\n\r", ch);
           send_to_char( "&GAutoRecharge NOT availiable while field active.\n\r", ch);
           sprintf( buf, "%s has activated an interdiction field!" , ship->name);
           echo_to_system( AT_ORANGE , ship , buf , target );
           echo_to_cockpit( AT_YELLOW , ship , "Interdiction Field Active!");
        }   
}

void do_selfdestruct(CHAR_DATA *ch, char *argument )
{
    SHIP_DATA *ship;

        if (  (ship = ship_from_cockpit(ch->in_room->vnum))  == NULL )
        {
            send_to_char("&RYou must be in the cockpit of a ship to do that!\n\r",ch);
            return;
        }

        if (  (ship = ship_from_pilotseat(ch->in_room->vnum))  == NULL )
        {
            send_to_char("&RYou must be in the pilots seat!\n\r",ch);
            return;
        }
 
         if ( ! check_pilot(ch,ship) || is_rental(ch,ship))
             {
               send_to_char("&RHey! Thats not your ship!\n\r",ch);
               return; 
             }
      
        if (argument == '\0' || argument[0] == '\0')
        {
            send_to_char("Syntax: selfdestruct <password>\n\r", ch);
            send_to_char("\"password\" is your character's login password\n\r", ch);
            return;
        }

        if ( strcmp( crypt( argument, ch->pcdata->pwd ), ch->pcdata->pwd ) )
        {
            send_to_char("&RIncorrect Password.\n\r", ch);
            return;
        }

    act( AT_PLAIN, "$n presses a red button on the control panel.", ch,
         NULL, argument , TO_ROOM );
    
    ship->sdestnum = 0;
    echo_to_ship(AT_DANGER, ship, "Self-Destruct Mode Initiated");
}


void do_deleteship( CHAR_DATA *ch, char *argument )
{    
    SHIP_DATA *ship;
    char shiplog[MAX_STRING_LENGTH];
    char buf[MAX_STRING_LENGTH];

     
     ship = get_ship( argument );
     if (ship == NULL)
     {
        send_to_char("&RNo such ship!",ch);
        return;
     } 
     
        resetship(ship);
        sprintf(shiplog,"Ship Deleted: %s",ship->name);
        log_string(shiplog);
        extract_ship( ship );
        ship_to_room( ship , 46 ); 
   	  ship->location = 46;
	  ship->shipyard = 46;
        if (ship->starsystem)
            ship_from_starsystem( ship, ship->starsystem );
            
         sprintf( buf, "%s%s", SHIP_DIR, ship->filename );
         remove(buf);
             
        UNLINK( ship, first_ship, last_ship, next, prev );
	  DISPOSE(ship);

        write_ship_list();
          
}


void do_unload_cargo( CHAR_DATA *ch, char *argument)
{
   SHIP_DATA *ship;
   SHIP_DATA *target;
   int cost;
   PLANET_DATA  *planet;

   if ( argument[0] == '\0' )
   {
      act( AT_PLAIN, "Which ship do you want to unload?.", ch, NULL, NULL, TO_CHAR);
      return;
   }

   target = ship_in_room( ch->in_room , argument );

   if ( !target )
   {
      act( AT_PLAIN, "I see no $T here.", ch, NULL, argument, TO_CHAR );
      return;
   }
   if (!check_pilot(ch, target))
   {
      send_to_char("Hey, that's not your ship!\r\n", ch);
      return;
   }

   if ( target->cargo == 0 )
   {
      send_to_char("You don't have any cargo.\r\n",ch);
      return;
   }

   if ( !IS_SET( ch->in_room->room_flags, ROOM_IMPORTS )
        && !IS_SET( ch->in_room->room_flags, ROOM_SPACECRAFT ))
   {
      send_to_char("You can't do that here!", ch);
      return;
   }
   planet = ch->in_room->area->planet;

   if (!planet)
   {
      ship = ship_from_hanger( ch->in_room->vnum );

      if (!ship)
      {
         send_to_char("You can't do that here!", ch);
         return;
      }     
      if ((ship->maxcargo - ship->cargo) < 1)
      {
         send_to_char("There is no room for anymore cargo\r\n",ch);
         return;
      }
      if (ship->cargo == 0)
          ship->cargotype = CARGO_NONE;

      if ((ship->cargo > 0) && (ship->cargotype != target->cargo))
      {
         send_to_char("They have a differnt type of cargo.\n\r",ch);
         return;
      }
      if (ship->cargotype == CARGO_NONE)
         ship->cargotype = target->cargotype;
      if ((ship->maxcargo - ship->cargo) >= target->cargo)
      {
         ship->cargo += target->cargo;
         target->cargo = 0;
         target->cargo = CARGO_NONE;
          send_to_char("Cargo unloaded.\r\n",ch);
         return;
      }
      else
      {
         target->cargo -= ship->maxcargo - ship->cargo;
         ship->cargo = ship->maxcargo;
         ch_printf(ch, "%s Loaded, %d tons still in %s hold.\r\n",  ship->name, target->cargo, target->name); 
         return;
      }
   }
   if (planet->import[target->cargotype] < 1)
   {
      send_to_char("You can't deliver that here.\r\n",ch);
      return;
   }
   cost = target->cargo;
   cost *= planet->import[target->cargotype];

   ch->gold += cost;
   target->cargo = 0;
   ch_printf(ch,"You recieve %d credits for a load of %s.\r\n", cost, cargo_names[target->cargotype]); 
   target->cargotype = CARGO_NONE;
   return;
}

void do_load_cargo( CHAR_DATA *ch, char *argument)
{
   SHIP_DATA *ship;
   SHIP_DATA *target;
   int cost,cargo, i;
   PLANET_DATA  *planet;
   char arg1[MAX_INPUT_LENGTH];

   argument = one_argument(argument, arg1);

   if ( arg1[0] == '\0' )
   {
      act( AT_PLAIN, "Which ship do you want to load?.", ch, NULL, NULL, TO_CHAR);
      return;
   }

   target = ship_in_room( ch->in_room , arg1 );

   if ( !target )
   {
      act( AT_PLAIN, "I see no $T here.", ch, NULL, argument, TO_CHAR );
      return;
   }

   if (!check_pilot(ch, target))
   {
      send_to_char("Hey, that's not your ship!\r\n", ch);
      return;
   }

   if ( !IS_SET( ch->in_room->room_flags, ROOM_IMPORTS )
        && !IS_SET( ch->in_room->room_flags, ROOM_SPACECRAFT ))
   {
      send_to_char("You can't do that here!", ch);
      return;
   }
   planet = ch->in_room->area->planet;

   if (!planet)
   {
      ship = ship_from_hanger( ch->in_room->vnum );
      if (!ship)
      {
         send_to_char("You can't do that here!", ch);
         return;
      }
      if (ship->cargo == 0)
      {
         send_to_char("They don't have any cargo\n\r", ch);
         return;
      }
      if ((target->maxcargo - target->cargo) < 1) 
      {
         send_to_char("There is no room for anymore cargo\r\n",ch);
         return;
      }
      if ((target->cargotype =! CARGO_NONE) && (ship->cargotype != target->cargotype));
      {
         send_to_char("Maybe you should deliver your cargo first.\n\r",ch);
         return;
      }
      if (target->cargotype == CARGO_NONE)
          target->cargotype = ship->cargotype;

      if ((target->maxcargo - target->cargo) >= ship->cargo)
      {
         target->cargo += ship->cargo;
         ship->cargo = 0;
         send_to_char("Cargo loaded.\r\n",ch);
         return;
      }
      else
      {
         ship->cargo -= target->maxcargo - target->cargo;
         target->cargo = target->maxcargo;
         send_to_char("Cargo Loaded.\r\n",ch);
         return;
      }

   }
   if (argument[0] == '\0')
   {
      send_to_char("&RWhat do you wnat to load&C&w\r\n", ch);
      return;
   }
   if (target->maxcargo - target->cargo <= 0)
   {
      send_to_char("There is no room for more Cargo.\r\n", ch);
      return;
   }
   for(i = 1; i < CARGO_MAX; i++)
   {
      if(!strcmp(argument, cargo_names[i]))
         cargo = i;
   }

   if ((target->cargo > 0) && (target->cargotype != cargo))
   {
      send_to_char("Maybe you should deliver your cargo first\r\n",ch);
      return;
   }

   if (planet->export[cargo] < 1)
   {
      send_to_char("We don't export those goods here\r\n", ch);
      return;
   }

   if (planet->resource[cargo] < 1)
   {
      send_to_char("&RSorry we have none left to ship\r\n", ch);
      return;
   }

   if ((target->maxcargo - target->cargo) >= planet->resource[cargo])
      cost = planet->resource[cargo];
   else
      cost = (target->maxcargo - target->cargo);
  
   cost *= planet->export[cargo];

   if (ch->gold < cost)
   {
      send_to_char("You can't afford it!\r\n", ch);
      return;
   }
   ch->gold -= cost;

   if ((target->maxcargo - target->cargo) >= planet->resource[cargo])
   {
   
     target->cargo += planet->resource[cargo];
     planet->resource[cargo] = 0;
     target->cargotype = cargo;
   }
   else
   {
     planet->resource[cargo] -= target->maxcargo - target->cargo;
     target->cargo = target->maxcargo;
     target->cargotype = cargo;
   }
   
   ch_printf(ch,"You pay %d credits for a load of %s.\r\n", cost, cargo_names[cargo]);
   return;
}


/* OVERDRIVE BY ARCTURUS */
void do_overdrive( CHAR_DATA *ch, char *argument )
{
    int chance;
    SHIP_DATA *ship;
    char buf[MAX_STRING_LENGTH];

    if (  (ship = ship_from_cockpit(ch->in_room->vnum))  == NULL )
    {
       send_to_char("&RYou must be in the cockpit of a ship to do that!\n\r",ch);
        return;
    }

    if ( ship->class > MOBILE_SUIT )
    {
       send_to_char("&RThis isn't a spacecraft!\n\r",ch);
       return;
    }
    if (  (ship = ship_from_pilotseat(ch->in_room->vnum))  == NULL )
    {
       send_to_char("&RThe controls must be at the pilots chair...\n\r",ch);
       return;
    }
    if ( autofly(ship) )
    {
       send_to_char("&RYou'll have to turn off the ships autopilot first.\n\r",ch);
       return;
    }
    if  ( ship->class == SHIP_PLATFORM )
    {
       send_to_char( "&RPlatforms can't move!\n\r" , ch );
       return;
    }

    if (ship->shipstate == SHIP_HYPERSPACE)
    {
       send_to_char("&RYou can only do that in realspace!\n\r",ch);
       return;
    }
    if (ship->shipstate == SHIP_DISABLED)
    {
       send_to_char("&RThe ships drive is disabled. Unable to accelerate.\n\r",ch);
       return;
    }
    if (ship->shipstate == SHIP_DOCKED)
    {
        send_to_char("&RYou can't do that until after you've launched!\n\r",ch);
        return;
    }

    if ((ship->shipstate2 == SHIP_DOCK) || (ship->shipstate2 == SHIP_DOCK_2))
    {
       send_to_char("&RNot while docking procedures are going on.\n\r", ch);
       return;
    }

    if (ship->shipstate2 == SHIP_DOCK_3)
    {
      send_to_char("&RDetach from the docked ship first.\n\r",ch);
      return;
    }        
    if (ship->overdrive == 0)
    {
      send_to_char("&RTry installing a SLAM overdrive node first.\n\r", ch);
      return;
    }
    if (IS_SET(ship->flags, SHIPFLAG_AFTERBURNER))
    {
      send_to_char("&RYour afterburners must be turned off before you can overpower your engine.\n\r", ch);
      return;
    }
    if(ship->energy < 200)
    {
      send_to_char("&RYou don't have enough fuel.", ch);
      return;
    }

    if(IS_SET(ship->flags,  SHIPFLAG_SABOTAGEDENGINE))
    {
       echo_to_cockpit( AT_BLUE , ship , "There is a small explosion and then your ship stops moving. It must have been sabotage!\n\r");
       echo_to_cockpit( AT_BLOOD + AT_BLINK , ship , "Ships Drive DAMAGED!");
       ship->shipstate = SHIP_DISABLED;
       ship->currspeed = 0;
       REMOVE_BIT(ship->flags, SHIPFLAG_SABOTAGEDENGINE);
       return;
    }
    if(!str_cmp(argument, "off"))
    {
      if (IS_SET(ship->flags, SHIPFLAG_OVERDRIVENODE))
      {
        send_to_char("&WYou shut off your overdrive node, and your ship decelerates.\n\r", ch);
        REMOVE_BIT(ship->flags, SHIPFLAG_OVERDRIVENODE);
        act( AT_PLAIN, "$n manipulates the ships controls.", ch, NULL, argument , TO_ROOM );
        if(!IS_SET(ship->flags, SHIPFLAG_CLOAKED) )
        {
           sprintf( buf, "%s begins to slow down." , ship->name );
           echo_to_system( AT_ORANGE , ship , buf , NULL );
        }
        ship->currspeed = UMAX(0, ship->realspeed + speedbonus(ship));
        return;
      }
      else
      {
         send_to_char("&WYour overdrive node isn't active.\n\r", ch);
         return;
      }
    }
      if(IS_SET(ship->flags, SHIPFLAG_OVERDRIVENODE))
      {
          send_to_char("&WYour overdrive is already on. Type overdrive off to turn it off.\n\r", ch);
          return;
      }                
      chance = IS_NPC(ch) ? ch->top_level : (int) (ch->pcdata->learned[gsn_overdrive]);
      if ( number_percent( ) >= chance )
      {
          send_to_char("&RYou fail to work the controls properly.\n\r",ch);
          learn_from_failure(ch, gsn_overdrive);
          return;
      }
      learn_from_success(ch, gsn_overdrive);
      SET_BIT(ship->flags, SHIPFLAG_OVERDRIVENODE);
      send_to_char("Overdrive node active, powering up engines.\n\r", ch);
      act( AT_PLAIN, "$n manipulates the ships controls.", ch, NULL, argument , TO_ROOM );
      if(!IS_SET(ship->flags, SHIPFLAG_CLOAKED) )
      {
         sprintf( buf, "%s begins to speed up." , ship->name );
         echo_to_system( AT_ORANGE , ship , buf , NULL );
      }
      ship->currspeed = UMAX(0, (int) ship->realspeed * speedbonus(ship));   
      ship->energy -= 200; /* fuel used to activate it. */
      return;
}       

/* AFTERBURNER BY ARCTURUS */ 
void do_burn( CHAR_DATA *ch, char *argument )
{
    int chance;
    SHIP_DATA *ship;
    char buf[MAX_STRING_LENGTH];

    if (  (ship = ship_from_cockpit(ch->in_room->vnum))  == NULL )
    {
       send_to_char("&RYou must be in the cockpit of a ship to do that!\n\r",ch);
        return;
    }

    if ( ship->class > MOBILE_SUIT )
    {
       send_to_char("&RThis isn't a spacecraft!\n\r",ch);
       return;
    }
    if (  (ship = ship_from_pilotseat(ch->in_room->vnum))  == NULL )
    {
       send_to_char("&RThe controls must be at the pilots chair...\n\r",ch);
       return;
    }
    if ( autofly(ship) )
    {
       send_to_char("&RYou'll have to turn off the ships autopilot first.\n\r",ch);
       return;
    }
    if  ( ship->class == SHIP_PLATFORM )
    {
       send_to_char( "&RPlatforms can't move!\n\r" , ch );
       return;
    }

    if (ship->shipstate == SHIP_HYPERSPACE)
    {
       send_to_char("&RYou can only do that in realspace!\n\r",ch);
       return;
    }
    if (ship->shipstate == SHIP_DISABLED)
    {
       send_to_char("&RThe ships drive is disabled. Unable to accelerate.\n\r",ch);
       return;
    }
    if (ship->shipstate == SHIP_DOCKED)
    {
        send_to_char("&RYou can't do that until after you've launched!\n\r",ch);
        return;
    }

    if ((ship->shipstate2 == SHIP_DOCK) || (ship->shipstate2 == SHIP_DOCK_2))
    {
       send_to_char("&RNot while docking procedures are going on.\n\r", ch);
       return;
    }

    if (ship->shipstate2 == SHIP_DOCK_3)
    {
      send_to_char("&RDetach from the docked ship first.\n\r",ch);
      return;
    }        

    if (IS_SET(ship->flags, SHIPFLAG_OVERDRIVENODE))
    {
      send_to_char("&RYour overdrive node must be turned off before you can use your afterburners.\n\r", ch);
      return;
    }
    if(ship->energy < 100)
    {
       send_to_char("You don't have enough fuel.\n\r", ch);
       return;
    }

    if(IS_SET(ship->flags,  SHIPFLAG_SABOTAGEDENGINE))
    {
       echo_to_cockpit( AT_BLUE , ship , "There is a small explosion and then your ship stops moving. It must have been sabotage!\n\r");
       echo_to_cockpit( AT_BLOOD + AT_BLINK , ship , "Ships Drive DAMAGED!");
       ship->shipstate = SHIP_DISABLED;
       ship->currspeed = 0;
       REMOVE_BIT(ship->flags, SHIPFLAG_SABOTAGEDENGINE);
       return;
    }
    if(!str_cmp(argument, "off"))
    {
      if (IS_SET(ship->flags, SHIPFLAG_AFTERBURNER))
      {
        send_to_char("&WYou shut off your afterburners, and your ship decelerates.\n\r", ch);
        REMOVE_BIT(ship->flags, SHIPFLAG_AFTERBURNER);
        act( AT_PLAIN, "$n manipulates the ships controls.", ch, NULL, argument , TO_ROOM );
        if(!IS_SET(ship->flags, SHIPFLAG_CLOAKED) )
        {
           sprintf( buf, "%s begins to slow down." , ship->name );
           echo_to_system( AT_ORANGE , ship , buf , NULL );
        }
        ship->currspeed = UMAX(0, ship->realspeed + speedbonus(ship));
        return;
      }
         else
         {
            send_to_char("&WYour afterburners aren't even active.\n\r", ch);
            return;
         }
      }
      if(IS_SET(ship->flags, SHIPFLAG_AFTERBURNER))
      {
          send_to_char("&WYour afterburners are already on. Type burn off to turn them off.\n\r", ch);
          return;
      }                
      chance = IS_NPC(ch) ? ch->top_level : (int) (ch->pcdata->learned[gsn_burn]);
      if ( number_percent( ) >= chance )
      {
          send_to_char("&RYou fail to work the controls properly.\n\r",ch);
          learn_from_failure(ch, gsn_burn);
          return;
      }
      learn_from_success(ch, gsn_burn);
      SET_BIT(ship->flags, SHIPFLAG_AFTERBURNER);
      send_to_char("Afterburners active, accelerating.\n\r", ch);
      act( AT_PLAIN, "$n manipulates the ships controls.", ch, NULL, argument , TO_ROOM );
      if(!IS_SET(ship->flags, SHIPFLAG_CLOAKED) )
      {
         sprintf( buf, "%s begins to speed up." , ship->name );
         echo_to_system( AT_ORANGE , ship , buf , NULL );
      }
      ship->currspeed = UMAX(0, (int) ship->realspeed * speedbonus(ship));
      ship->energy -= 100;
      return;
}       

/*Sabotage by Arcturus */ 
void do_sabotage(CHAR_DATA *ch, char *argument)
{
   char arg[MAX_INPUT_LENGTH];
   SHIP_DATA *ship;
   int chance;
    switch( ch->substate )
    {
       default:
       if (  (ship = ship_from_engine(ch->in_room->vnum))  == NULL )
       {
          send_to_char("&RYou must be in the engine room of a ship to sabotage it\n\r", ch);
          return;
       }

   if ( str_cmp( argument , "lasers" ) && str_cmp( argument , "drive" ) &&
        str_cmp( argument , "launcher" ) && str_cmp( argument , "ions" ) &&
        str_cmp( argument , "turret1" ) && str_cmp( argument , "turret2") && 
        str_cmp( argument , "rlauncher" ) && str_cmp( argument , "tlauncher"))
   {
      send_to_char("Syntax: sabotage <lasers/ions/drive/turret1/turret2/launcher/tlauncher/rlauncher>", ch);
      return;
   }
   strcpy(arg, argument);
   if(!str_cmp("public", ship->owner))
   {
      send_to_char("Its not very polite to sabotage public craft. Not much to gain from it either.\n\r", ch);
      return;
   }
   if(!str_cmp(argument, "lasers"))
   {
      if(IS_SET(ship->flags, SHIPFLAG_SABOTAGEDLASERS))
      {
         send_to_char("Lasers are already sabotaged.\n\r", ch);
         return;
      }
      if(!ship->lasers)
      {
         send_to_char("That ship doesn't have any lasers to sabotage.\n\r", ch);
         return;
      }
   }
   else if(!str_cmp(argument, "ions"))
   {
      if(IS_SET(ship->flags, SHIPFLAG_SABOTAGEDIONS))
      {
         send_to_char("Ions are already sabotaged.\n\r", ch);
         return;
      }
      if(!ship->ions)
      {
         send_to_char("That ship doesn't have any ions to sabotage.\n\r", ch);
         return;
      }
   }
   else if(!str_cmp(argument, "drive"))
   {
      if(IS_SET(ship->flags, SHIPFLAG_SABOTAGEDENGINE))
      {
         send_to_char("Drive is already sabotaged.\n\r", ch);
         return;
      }
   }
   else if(!str_cmp(argument, "turret1"))
   {
       if(IS_SET(ship->flags, SHIPFLAG_SABOTAGEDTURRET1))
       {
          send_to_char("Turret 1 is already sabotaged.\n\r", ch);
          return;
       }
       if(!ship->turret1)
       {
          send_to_char("That ship doesn't have a turret.\n\r", ch);
          return;
       }
   }
   else if(!str_cmp(argument, "turret2"))
   {
      if(IS_SET(ship->flags, SHIPFLAG_SABOTAGEDTURRET2))
      {
         send_to_char("Turret 2 is already sabotaged.\n\r", ch);
         return;
      }
      if(!ship->turret2)
      {
         send_to_char("That ship doesn't have a second turret.\n\r", ch);
         return;
      }
   }
   else if(!str_cmp(argument, "launcher"))
   {
      if(IS_SET(ship->flags, SHIPFLAG_SABOTAGEDLAUNCHERS))
      {
         send_to_char("Launchers are already sabotaged.\n\r", ch);
         return;
      }
      if(!ship->mlaunchers)
      { 
         send_to_char("That ship doesn't have a missile launcher.\n\r", ch);
         return;
      }
   }
   else if(!str_cmp(argument, "tlauncher"))
   {
     if(IS_SET(ship->flags, SHIPFLAG_SABOTAGEDTLAUNCHERS))
     {
        send_to_char("Torpedo Launchers are already sabotaged.\n\r", ch);
        return;
     }
     if(!ship->tlaunchers)
     {
        send_to_char("That ship dosen't have a torpedo launcher.\n\r", ch);
        return;
     }
   }
   else if(!str_cmp(argument, "rlauncher"))
   {
     if(IS_SET(ship->flags, SHIPFLAG_SABOTAGEDRLAUNCHERS))
     {
        send_to_char("Rocket Launchers are already sabotaged.\n\r", ch);
        return;
     }
     if(!ship->rlaunchers)
     {
        send_to_char("That ship doesn't have rocket launchers.\n\r", ch);
        return;
     }
   }
   else
   {
      send_to_char("Syntax: sabotage <lasers/ions/drive/turret1/turret2/launcher/rlauncher/tlauncher>\n\r", ch);
      return;
   }
   chance = IS_NPC(ch) ? ch->top_level
            : (int) (ch->pcdata->learned[gsn_sabotage]);
   if ( number_percent( ) < chance )
   {
      send_to_char("&GYou begin sabotaging the craft.\n\r", ch);
      if(IS_SET(ship->flags, SHIPFLAG_SIMULATOR))
      {
         send_to_char("You realize its a simulator and and begin to upload a virus, while playing pong.\n\r", ch);
      }
      act( AT_PLAIN, "$n begins fiddling around with the ships $T.", ch,
         NULL, argument , TO_ROOM );
      add_timer ( ch , TIMER_DO_FUN , 5 , do_sabotage , 1 );
      ch->dest_buf = str_dup(arg);
      return;
   }
   send_to_char("You aren't quite sure how to sabotage it.\n\r", ch);
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
                if ( (ship = ship_from_cockpit(ch->in_room->vnum)) == NULL )
                      return;                                                  
                send_to_char("&RYou are distracted and fail to finish your sabotage efforts.\n\r", ch);
                return;
       }
    ch->substate = SUB_NONE;
    if ( (ship = ship_from_engine(ch->in_room->vnum)) == NULL )
    {
       return;
    }
   chance = IS_NPC(ch) ? ch->top_level
            : (int) (ch->pcdata->learned[gsn_sabotage]);
   if ( number_percent( ) > chance )
   {
      send_to_char( "&RYou fail to properly sabotage it.\n\r", ch);
      learn_from_failure( ch, gsn_sabotage );
      act( AT_PLAIN, "$n finishes fiddling with something.", ch, NULL, argument, TO_ROOM);
      return;
   }
   send_to_char("&GYou finish sabotaging it, and conceal your efforts.\n\r", ch);
   learn_from_success(ch, gsn_sabotage);
   act( AT_PLAIN, "$n finishes fiddling with something.", ch,
        NULL, argument , TO_ROOM );
   if(!str_cmp(arg, "lasers"))
      SET_BIT(ship->flags, SHIPFLAG_SABOTAGEDLASERS);
   if(!str_cmp(arg, "ions"))
      SET_BIT(ship->flags, SHIPFLAG_SABOTAGEDIONS);
   if(!str_cmp(arg, "drive"))
      SET_BIT(ship->flags, SHIPFLAG_SABOTAGEDENGINE);
   if(!str_cmp(arg, "turret1"))
      SET_BIT(ship->flags, SHIPFLAG_SABOTAGEDTURRET1);
   if(!str_cmp(arg, "turret2"))
      SET_BIT(ship->flags, SHIPFLAG_SABOTAGEDTURRET2);
   if(!str_cmp(arg, "launcher"))
      SET_BIT(ship->flags, SHIPFLAG_SABOTAGEDLAUNCHERS);
   if(!str_cmp(arg, "tlauncher"))
      SET_BIT(ship->flags, SHIPFLAG_SABOTAGEDTLAUNCHERS);
   if(!str_cmp(arg, "rlauncher"))
      SET_BIT(ship->flags, SHIPFLAG_SABOTAGEDRLAUNCHERS);
   return;    
}

/*Laser and Missile linking, by Arcturus */
/* Originally only for lasers/ions. */
void do_linklaser(CHAR_DATA *ch, char *argument)
{
    int chance, llink;
    SHIP_DATA *ship;
    if ( str_cmp( argument , "duallaser" ) && str_cmp( argument , "trilaser" ) &&
         str_cmp( argument , "quadlaser" ) && str_cmp( argument , "dualion" ) &&
         str_cmp( argument , "triion" ) && str_cmp( argument , "quadion")     &&
         str_cmp( argument , "dualmissile") && str_cmp(argument, "dualtorpedo") &&
         str_cmp( argument , "dualrocket"))
    {
       send_to_char("Syntax: link <duallaser/trilaser/quadlaser/dualion/triion/quadion/dualmissile/dualtorpedo/dualrocket>", ch);
       return;
    }
        if (  (ship = ship_from_turret(ch->in_room->vnum))  == NULL )
        {
            send_to_char("&RYou must be in the gunners chair or turret of a ship to do that!\n\r",ch);
            return;
        }
        if ( ship->class > MOBILE_SUIT )
        {
             send_to_char("&RThis isn't a spacecraft!\n\r",ch);
             return;
        }
        if ( autofly(ship) )
        {
             send_to_char("&RYou'll have to turn off the ships autopilot first.\n\r",ch);
             return;
        }
        if (ship->shipstate == SHIP_HYPERSPACE)
        {
             send_to_char("&RYou can only do that in realspace!\n\r",ch);
             return;
        }
        if (ship->starsystem == NULL)
        {
             send_to_char("&RYou can't do that until after you've finished launching!\n\r",ch);
             return;
        }
        if ( ship->energy < 5 )
        {
             send_to_char("&RTheres not enough energy left to link them!\n\r",ch);
             return;
        }
    if(!str_cmp(argument, "duallaser"))
    {
       if(IS_SET(ship->flags, SHIPFLAG_DUALLASER))
       {
          send_to_char("Your lasers are already dual linked.\n\r", ch);
          return;
       }
       if(ship->lasers < 2)
       {
          send_to_char("You can't duallink your lasers.\n\r", ch);
          return;
       }
       llink = 0;
    }
    if(!str_cmp(argument, "trilaser"))
    {
       if(IS_SET(ship->flags, SHIPFLAG_TRILASER))
       {
          send_to_char("Your lasers are already trilinked.\n\r", ch);
          return;
       }
       if(ship->lasers < 3)
       {
          send_to_char("You can't trilink your lasers.\n\r", ch);
          return;
       }
       llink = 1;
    }
    if(!str_cmp(argument, "quadlaser"))
    {
        if(IS_SET(ship->flags, SHIPFLAG_QUADLASER))
       {
          send_to_char("Your lasers are already quadlinked.\n\r", ch);
          return;
       }
       if(ship->lasers < 4)
       {
          send_to_char("You can't quadlink your lasers.\n\r", ch);
          return;
       }
       llink = 2;
    }
    if(!str_cmp(argument, "dualion"))
    {
       if(IS_SET(ship->flags, SHIPFLAG_DUALION))
       {
          send_to_char("Your ions are already dual linked.\n\r", ch);
          return;
       }
       if(ship->ions < 2)
       {
          send_to_char("You can't duallink your ions.\n\r", ch);
          return;
       }
       llink = 3;
    }
    if(!str_cmp(argument, "triion"))
    {
       if(IS_SET(ship->flags, SHIPFLAG_TRIION))
       {
          send_to_char("Your lasers are already trilinked.\n\r", ch);
          return;
       }
       if(ship->ions < 3)
       {
          send_to_char("You can't trilink your ions.\n\r", ch);
          return;
       }
       llink = 4;
    }
    if(!str_cmp(argument, "quadion"))
    {
        if(IS_SET(ship->flags, SHIPFLAG_QUADION))
       {
          send_to_char("Your ions are already quadlinked.\n\r", ch);
          return;
       }
       if(ship->ions < 4)
       {
          send_to_char("You can't quadlink your ions.\n\r", ch);
          return;
       }
       llink = 5;
    }
    if(!str_cmp(argument, "dualmissile"))
    {
        if(IS_SET(ship->flags, SHIPFLAG_DUALMISSILE))
       {
          send_to_char("Your missile launchers are already dual linked.\n\r", ch);
          return;
       }
       if(ship->mlaunchers < 2)
       {
          send_to_char("You can't dual link your missile launchers.\n\r", ch);
          return;
       }
       llink = 6;
    }
    if(!str_cmp(argument, "dualtorpedo"))
    {
        if(IS_SET(ship->flags, SHIPFLAG_DUALTORPEDO))
       {
          send_to_char("Your torpedo launchers are already dual linked.\n\r", ch);
          return;
       }
       if(ship->tlaunchers < 2)
       {
          send_to_char("You can't dual link your torpedo launchers.\n\r", ch);
          return;
       }
       llink = 7;
    }
        if(!str_cmp(argument, "dualrocket"))
    {
    if(IS_SET(ship->flags, SHIPFLAG_DUALROCKET))
       {
          send_to_char("Your torpedo launchers are already dual linked.\n\r", ch);
          return;
       }
       if(ship->rlaunchers < 2)
       {
          send_to_char("You can't dual link your rocket launchers.\n\r", ch);
          return;
       }
       llink = 8;
    }
    chance = IS_NPC(ch) ? ch->top_level
                 : (int)  (ch->pcdata->learned[gsn_weaponsystems]);
    if ( number_percent( ) > chance )
    {
        send_to_char("&RYou can't quite figure out where the switch is.\n\r",ch);
        learn_from_failure( ch, gsn_weaponsystems );
        return;
    }
    switch(llink)
    {
    case 0: 
       SET_BIT(ship->flags, SHIPFLAG_DUALLASER); 
       if(IS_SET(ship->flags, SHIPFLAG_TRILASER))
          REMOVE_BIT(ship->flags, SHIPFLAG_TRILASER);
       if(IS_SET(ship->flags, SHIPFLAG_QUADLASER))
          REMOVE_BIT(ship->flags, SHIPFLAG_QUADLASER);
       send_to_char("You link your lasers up.\n\r", ch);
       act( AT_PLAIN, "$n flips a switch. Laserfire linked.", ch,
          NULL, argument , TO_ROOM );
       break;
    case 1: SET_BIT(ship->flags, SHIPFLAG_TRILASER);
       if(IS_SET(ship->flags, SHIPFLAG_DUALLASER))
          REMOVE_BIT(ship->flags, SHIPFLAG_DUALLASER);
       if(IS_SET(ship->flags, SHIPFLAG_QUADLASER))
          REMOVE_BIT(ship->flags, SHIPFLAG_QUADLASER);
       send_to_char("You link your lasers up.\n\r", ch);
       act( AT_PLAIN, "$n flips a switch. Laserfire linked.", ch,
         NULL, argument , TO_ROOM );
       break;
    case 2: SET_BIT(ship->flags, SHIPFLAG_QUADLASER); 
       if(IS_SET(ship->flags, SHIPFLAG_DUALLASER))
          REMOVE_BIT(ship->flags, SHIPFLAG_DUALLASER);
       if(IS_SET(ship->flags, SHIPFLAG_TRILASER))
          REMOVE_BIT(ship->flags, SHIPFLAG_TRILASER);
       send_to_char("You link your lasers up.\n\r", ch);
       act( AT_PLAIN, "$n flips a switch. Laserfire linked.", ch,
         NULL, argument , TO_ROOM );
       break;
    case 3: SET_BIT(ship->flags, SHIPFLAG_DUALION); 
       if(IS_SET(ship->flags, SHIPFLAG_TRIION))
          REMOVE_BIT(ship->flags, SHIPFLAG_TRIION);
       if(IS_SET(ship->flags, SHIPFLAG_QUADION))
          REMOVE_BIT(ship->flags, SHIPFLAG_QUADION);
       send_to_char("You link your ion cannons up.\n\r", ch);
       act( AT_PLAIN, "$n flips a switch. Ionfire linked.", ch,
         NULL, argument , TO_ROOM );
       break;
    case 4: SET_BIT(ship->flags, SHIPFLAG_TRIION); 
       if(IS_SET(ship->flags, SHIPFLAG_DUALION))
          REMOVE_BIT(ship->flags, SHIPFLAG_DUALION);
       if(IS_SET(ship->flags, SHIPFLAG_QUADION))
          REMOVE_BIT(ship->flags, SHIPFLAG_QUADION);
       send_to_char("You link your ion cannons up.\n\r", ch);
       act( AT_PLAIN, "$n flips a switch. Ionfire linked.", ch,
         NULL, argument , TO_ROOM );
        break;
    case 5: SET_BIT(ship->flags, SHIPFLAG_QUADION); 
       if(IS_SET(ship->flags, SHIPFLAG_DUALION))
         REMOVE_BIT(ship->flags, SHIPFLAG_DUALION);
       if(IS_SET(ship->flags, SHIPFLAG_TRIION))
         REMOVE_BIT(ship->flags, SHIPFLAG_TRIION);
       send_to_char("You link your ion cannons up.\n\r", ch);
       act( AT_PLAIN, "$n flips a switch. Ionfire linked.", ch,
         NULL, argument , TO_ROOM );
       break;
    case 6: SET_BIT(ship->flags, SHIPFLAG_DUALMISSILE);
         send_to_char("You link your missile launchers up.\n\r", ch);
         act( AT_PLAIN, "$n flips a switch. Missile launchers linked.", ch,
         NULL, argument , TO_ROOM );
         break;
    case 7: SET_BIT(ship->flags, SHIPFLAG_DUALTORPEDO);
         send_to_char("You link your torpedo launchers up.\n\r", ch);
         act( AT_PLAIN, "$n flips a switch. Torpedo launchers linked.", ch,
             NULL, argument , TO_ROOM );
         break;
    case 8: SET_BIT(ship->flags, SHIPFLAG_DUALROCKET);
        send_to_char("You link your rocket launchers up.\n\r", ch);
        act( AT_PLAIN, "$n flips a switch. Rocket launchers linked.", ch,
            NULL, argument , TO_ROOM );
        break;
    default: send_to_char("Error in link code, not linked, invalid link value.\n\r", ch); break;
    }
    ship->energy -= 5;
    learn_from_success(ch, gsn_weaponsystems);
    return;
}

/* For Unlinking The Weapons - Arcturus */
void do_unlinklaser(CHAR_DATA *ch, char *argument)
{
    int chance;
    bool unlink;
    SHIP_DATA *ship;
    if ( str_cmp( argument , "laser" ) && str_cmp( argument , "ion" )
      && str_cmp( argument , "missile") && str_cmp( argument, "torpedo")
      && str_cmp( argument , "rocket"))
    {
       send_to_char("Syntax: unlink <laser/ion/missile/torpedo/rocket>", ch);
       return;
    }
        if (  (ship = ship_from_gunseat(ch->in_room->vnum))  == NULL )
        {
            send_to_char("&RYou must be in the gunners chair to do that!\n\r",ch);
            return;
        }
        if ( ship->class > MOBILE_SUIT )
        {
             send_to_char("&RThis isn't a spacecraft!\n\r",ch);
             return;
        }
        if ( autofly(ship) )
        {
             send_to_char("&RYou'll have to turn off the ships autopilot first.\n\r",ch);
             return;
        }
        if (ship->shipstate == SHIP_HYPERSPACE)
        {
             send_to_char("&RYou can only do that in realspace!\n\r",ch);
             return;
        }
        if (ship->starsystem == NULL)
        {
             send_to_char("&RYou can't do that until after you've finished launching!\n\r",ch);
             return;
        }
    unlink = FALSE;
    if(!str_cmp(argument, "laser"))
    {
       if(IS_SET(ship->flags, SHIPFLAG_DUALLASER))
          unlink = TRUE;
       if(IS_SET(ship->flags, SHIPFLAG_TRILASER))
          unlink = TRUE;
       if(IS_SET(ship->flags, SHIPFLAG_QUADLASER))
          unlink = TRUE;
       if(!unlink)
       {
          send_to_char("Your lasers aren't linked.\n\r", ch);
          return;
       }
    }
    if(!str_cmp(argument, "ion"))
    {
       if(IS_SET(ship->flags, SHIPFLAG_DUALION))
          unlink = TRUE;
       if(IS_SET(ship->flags, SHIPFLAG_TRIION))
          unlink = TRUE;
       if(IS_SET(ship->flags, SHIPFLAG_QUADION))
          unlink = TRUE;
       if(!unlink)
       {
          send_to_char("Your ions aren't linked.\n\r", ch);
          return;
       }
     }
    if(!str_cmp(argument, "missile"))
    {
       if(IS_SET(ship->flags, SHIPFLAG_DUALMISSILE))
           unlink = TRUE;
       if(!unlink)
       {
          send_to_char("Your missile launchers aren't linked.\n\r", ch);
          return;
       }
    }
    if(!str_cmp(argument, "torpedo"))
    {
       if(IS_SET(ship->flags, SHIPFLAG_DUALTORPEDO))
          unlink = TRUE;
       if(!unlink)
       {
          send_to_char("Your torpedo launchers aren't linked.\n\r", ch);
          return;
       }
    }
    if(!str_cmp(argument, "rocket"))
    {
       if(IS_SET(ship->flags, SHIPFLAG_DUALROCKET))
          unlink = TRUE;
       if(!unlink)
       {
          send_to_char("Your rocket launchers aren't linked.\n\r", ch);
          return;
       }
    }
    chance = IS_NPC(ch) ? ch->top_level
                 : (int)  (ch->pcdata->learned[gsn_weaponsystems]) ;
    if ( number_percent( ) > chance )
    {
        send_to_char("&RYou can't quite figure out where the switch is.\n\r",ch);
        learn_from_failure( ch, gsn_weaponsystems );
        return;
    }
    if(!str_cmp(argument, "laser"))
    {
       if(IS_SET(ship->flags, SHIPFLAG_DUALLASER))
          REMOVE_BIT(ship->flags, SHIPFLAG_DUALLASER);
       if(IS_SET(ship->flags, SHIPFLAG_TRILASER))
          REMOVE_BIT(ship->flags, SHIPFLAG_TRILASER);
       if(IS_SET(ship->flags, SHIPFLAG_QUADLASER))
          REMOVE_BIT(ship->flags, SHIPFLAG_QUADLASER);
      send_to_char("You unlink your lasers.\n\r", ch);
      act( AT_PLAIN, "$n flips a switch. Unlinked lasers.\n\r", ch,
         NULL, argument , TO_ROOM );
    }
    if(!str_cmp(argument, "ion"))
    {
       if(IS_SET(ship->flags, SHIPFLAG_DUALION))
          REMOVE_BIT(ship->flags, SHIPFLAG_DUALION);
       if(IS_SET(ship->flags, SHIPFLAG_TRIION))
          REMOVE_BIT(ship->flags, SHIPFLAG_TRIION);
       if(IS_SET(ship->flags, SHIPFLAG_QUADION))
          REMOVE_BIT(ship->flags, SHIPFLAG_QUADION);
       send_to_char("You unlink your ion cannons.\n\r", ch);
       act( AT_PLAIN, "$n flips a switch. Unlinked ion cannons.\n\r", ch,
         NULL, argument , TO_ROOM );
     }
    if(!str_cmp(argument, "missile"))
    {
      if(IS_SET(ship->flags, SHIPFLAG_DUALMISSILE))
         REMOVE_BIT(ship->flags, SHIPFLAG_DUALMISSILE);
          send_to_char("You unlink your missile launchers.\n\r", ch);
        act( AT_PLAIN, "$n flips a switch. Unlinked missile launchers.", ch,
         NULL, argument , TO_ROOM );
    }
    if(!str_cmp(argument, "torpedo"))
    {
      if(IS_SET(ship->flags, SHIPFLAG_DUALTORPEDO))
         REMOVE_BIT(ship->flags, SHIPFLAG_DUALTORPEDO);
      send_to_char("You unlink your torpedo launchers.\n\r", ch);
    act( AT_PLAIN, "$n flips a switch. Unlinked torpedo launchers.\n\r", ch,
         NULL, argument , TO_ROOM );
    }
    if(!str_cmp(argument, "rocket"))
    {
       if(IS_SET(ship->flags, SHIPFLAG_DUALROCKET))
          REMOVE_BIT(ship->flags, SHIPFLAG_DUALROCKET);
       send_to_char("You unlink your rocket launchers.\n\r", ch);
    act( AT_PLAIN, "$n flips a switch. Unlinked rocket launchers.", ch,
         NULL, argument , TO_ROOM );
    }
    learn_from_success(ch, gsn_weaponsystems);
    return;   
}

/* Redirection by Arcturus */
void do_redirect(CHAR_DATA *ch, char *argument)
{
    char arg1[MAX_INPUT_LENGTH];
    char buf[MAX_STRING_LENGTH];
    SHIP_DATA *ship;
    int chance;
    argument = one_argument(argument, arg1);

    if ( str_cmp( argument , "laser" ) && str_cmp( argument , "shield" ) && str_cmp(argument, "engine")
      && str_cmp( argument , "lasers" ) && str_cmp( argument, "shields" )
      && str_cmp( arg1, "laser" ) && str_cmp(arg1, "shield") && str_cmp(arg1, "engine") && str_cmp(arg1, "default") 
      && str_cmp( arg1, "lasers" ) && str_cmp(arg1, "shields") )
    {
       send_to_char("Syntax: redirect <laser/shield/engine> <laser/shield/engine/default>\n\r", ch);
       return;
    }
    if(!str_cmp(arg1, argument))
    {
       send_to_char("Syntax for disabling redirection: redirect <laser/shield/engine> default\n\r", ch);
       return;
    }  
        if (  (ship = ship_from_cockpit(ch->in_room->vnum))  == NULL )
        {
            send_to_char("&RYou must be in the cockpit of a ship to do that!\n\r",ch);
            return;
        }
        if (  (ship = ship_from_pilotseat(ch->in_room->vnum))  == NULL )
        {
            send_to_char("&RYou must be in the pilot seat of a ship to do that!\n\r",ch);
            return;
        }
        if(!str_cmp(argument, "laser") || !str_cmp(arg1, "laser"))
        {
           if(ship->lasers == 0)
           {
              send_to_char("Redirect with what lasers?\n\r", ch);
              return;
           }
        }
        if(!str_cmp(argument, "shield") || !str_cmp(arg1, "shield"))
        {
           if(ship->maxshield == 0)
           {
              send_to_char("Redirect with what shields?\n\r", ch);
              return;
           }
        }  
        if ( ship->class > MOBILE_SUIT )
        {
             send_to_char("&RThis isn't a spacecraft!\n\r",ch);
             return;
        }
        if ( autofly(ship) )
        {
             send_to_char("&RYou'll have to turn off the ships autopilot first.\n\r",ch);
             return;
        }
        if (ship->shipstate == SHIP_HYPERSPACE)
        {
             send_to_char("&RYou can only do that in realspace!\n\r",ch);
             return;
        }
        if (ship->starsystem == NULL)
        {
             send_to_char("&RYou can't do that until after you've finished launching!\n\r",ch);
             return;
        }

    chance = IS_NPC(ch) ? ch->top_level
                 : (int)  (ch->pcdata->learned[gsn_shipsystems]) ;
    if ( number_percent( ) > chance )
    {
        send_to_char("&RYou can't quite figure out the controls.\n\r",ch);
        learn_from_failure( ch, gsn_shipsystems );
        return;
    }
    act( AT_PLAIN, "$n manipulates the ships controls.", ch, NULL, argument , TO_ROOM );
    if(!str_cmp(arg1, "laser") || !str_cmp(arg1, "lasers") )
    {
       if(!str_cmp(argument, "default"))
       {
          if(IS_SET(ship->flags, SHIPFLAG_LASERRENGINE))
          {
             REMOVE_BIT(ship->flags, SHIPFLAG_LASERRENGINE);
             if(!IS_SET(ship->flags, SHIPFLAG_CLOAKED) )
             {
                sprintf( buf, "%s begins to slow down." , ship->name );
                echo_to_system( AT_ORANGE , ship , buf , NULL );
             }
             ship->currspeed = UMIN(ship->currspeed, (int) ship->realspeed * speedbonus(ship));
          }
          if(IS_SET(ship->flags, SHIPFLAG_LASERRSHIELD))
             REMOVE_BIT(ship->flags, SHIPFLAG_LASERRSHIELD);
          send_to_char("Lasers recharging at a normal rate.\n\r", ch);
       }
       if(!str_cmp(argument, "shield") || !str_cmp(argument, "shields") )
       {
          if(IS_SET(ship->flags, SHIPFLAG_LASERRENGINE))
          {
             REMOVE_BIT(ship->flags, SHIPFLAG_LASERRENGINE);
             if(!IS_SET(ship->flags, SHIPFLAG_CLOAKED) )
             {
                sprintf( buf, "%s begins to slow down." , ship->name );
                echo_to_system( AT_ORANGE , ship , buf , NULL );
             }
             ship->currspeed = UMIN(ship->currspeed, (int) ship->realspeed * speedbonus(ship));
          } 
          if(!IS_SET(ship->flags, SHIPFLAG_LASERRSHIELD))
             SET_BIT(ship->flags, SHIPFLAG_LASERRSHIELD);
          send_to_char("Laser power redirected to shields, lasers recharging at a decreased rate, shields recharging at increased rate.\n\r", ch);
          if(IS_SET(ship->flags, SHIPFLAG_SHIELDRLASER))
         {
             send_to_char("Shield power transfer to lasers, reverted.\n\r", ch);
             REMOVE_BIT(ship->flags, SHIPFLAG_SHIELDRLASER);
          }
       }
       if(!str_cmp(argument, "engine"))
       {
          if(!IS_SET(ship->flags, SHIPFLAG_LASERRENGINE))
             SET_BIT(ship->flags, SHIPFLAG_LASERRENGINE);
          if(IS_SET(ship->flags, SHIPFLAG_LASERRSHIELD))
             REMOVE_BIT(ship->flags, SHIPFLAG_LASERRSHIELD);
          send_to_char("Laser power being redirected to engines. Lasers recharging at a decreased rate.\n\r", ch);
          if(IS_SET(ship->flags, SHIPFLAG_ENGINERLASER))
          {
             send_to_char("Engine power redirected back to engines.\n\r", ch);
             REMOVE_BIT(ship->flags, SHIPFLAG_ENGINERLASER);
          }
          if(!IS_SET(ship->flags, SHIPFLAG_CLOAKED) )
          {
             sprintf( buf, "%s begins to speed up." , ship->name );
             echo_to_system( AT_ORANGE , ship , buf , NULL );
          }
          ship->currspeed = UMAX(0, (int) ship->realspeed * speedbonus(ship));
       }
    }  

    if(!str_cmp(arg1, "shield") || !str_cmp(arg1, "shields" ) )
    {
       if(!str_cmp(argument, "default"))
       {
          if(IS_SET(ship->flags, SHIPFLAG_SHIELDRENGINE))
          {
             REMOVE_BIT(ship->flags, SHIPFLAG_SHIELDRENGINE);
             if(!IS_SET(ship->flags, SHIPFLAG_CLOAKED) )
             {          
                sprintf( buf, "%s begins to slow down." , ship->name );
                echo_to_system( AT_ORANGE , ship , buf , NULL );
             }
             ship->currspeed = UMIN(ship->currspeed, (int) ship->realspeed * speedbonus(ship));
          }
          if(IS_SET(ship->flags, SHIPFLAG_SHIELDRLASER))
             REMOVE_BIT(ship->flags, SHIPFLAG_SHIELDRLASER);
          send_to_char("Shields recharging at normal rate\n\r", ch);
       }
       if(!str_cmp(argument, "laser") || !str_cmp(argument, "lasers") )
       {
          if(IS_SET(ship->flags, SHIPFLAG_SHIELDRENGINE))
          {
             REMOVE_BIT(ship->flags, SHIPFLAG_SHIELDRENGINE);
             if(!IS_SET(ship->flags, SHIPFLAG_CLOAKED) )
             {
                sprintf( buf, "%s begins to slow down." , ship->name );
                echo_to_system( AT_ORANGE , ship , buf , NULL );
             } 
             ship->currspeed = UMIN(ship->currspeed, (int) ship->realspeed * speedbonus(ship));
          }
          if(!IS_SET(ship->flags, SHIPFLAG_SHIELDRLASER))
             SET_BIT(ship->flags, SHIPFLAG_SHIELDRLASER);
          send_to_char("Shield power redirected to lasers, shield power recharging at a decreased rate.\n\r", ch);
          if(IS_SET(ship->flags, SHIPFLAG_LASERRSHIELD))
          {
             send_to_char("Laser transfer to shields reverted, lasers recharging at an increased rate.\n\r", ch);
             REMOVE_BIT(ship->flags, SHIPFLAG_LASERRSHIELD);
          }
       }
       if(!str_cmp(argument, "engine"))
       {
          if(!IS_SET(ship->flags, SHIPFLAG_SHIELDRENGINE))
             SET_BIT(ship->flags, SHIPFLAG_SHIELDRENGINE);
          if(IS_SET(ship->flags, SHIPFLAG_SHIELDRLASER))
             REMOVE_BIT(ship->flags, SHIPFLAG_SHIELDRLASER);
          send_to_char("Shield power being redirected to engines, shields recharging at a decreased rate.\n\r", ch);
          if(IS_SET(ship->flags, SHIPFLAG_ENGINERSHIELD))
          {
              send_to_char("Engine power redirected back to engines.\n\r", ch);
              REMOVE_BIT(ship->flags, SHIPFLAG_ENGINERSHIELD);
          }
          if(!IS_SET(ship->flags, SHIPFLAG_CLOAKED) )
          {
             sprintf( buf, "%s begins to speed up." , ship->name );
             echo_to_system( AT_ORANGE , ship , buf , NULL );
          }
          ship->currspeed = UMAX(0, (int) ship->realspeed * speedbonus(ship));
       }
    }  
    if(!str_cmp(arg1, "engine"))
    {
       if(!str_cmp(argument, "default"))
       {
          if(IS_SET(ship->flags, SHIPFLAG_ENGINERLASER))
             REMOVE_BIT(ship->flags, SHIPFLAG_ENGINERLASER);
          if(IS_SET(ship->flags, SHIPFLAG_ENGINERSHIELD))
             REMOVE_BIT(ship->flags, SHIPFLAG_ENGINERSHIELD);
          send_to_char("The engines revert power back to normal.\n\r", ch);
          if(!IS_SET(ship->flags, SHIPFLAG_CLOAKED) )
          {
             sprintf( buf, "%s begins to speed up." , ship->name );
             echo_to_system( AT_ORANGE , ship , buf , NULL );
          }
          ship->currspeed = UMAX(0, (int) ship->realspeed * speedbonus(ship));
       }
       if(!str_cmp(argument, "shield") || !str_cmp(argument, "shields") )
       {
          if(IS_SET(ship->flags, SHIPFLAG_ENGINERLASER))
             REMOVE_BIT(ship->flags, SHIPFLAG_ENGINERLASER);
          if(!IS_SET(ship->flags, SHIPFLAG_ENGINERSHIELD))
             SET_BIT(ship->flags, SHIPFLAG_ENGINERSHIELD);
          if(IS_SET(ship->flags, SHIPFLAG_SHIELDRENGINE))
          {
             send_to_char("Shield redirection back to shields.\n\r", ch);
             REMOVE_BIT(ship->flags, SHIPFLAG_SHIELDRENGINE);
          }
          if(!IS_SET(ship->flags, SHIPFLAG_CLOAKED) )
          {
             send_to_char("Redirecting engine power to shields.\n\r", ch);
             sprintf( buf, "%s begins to slow down." , ship->name );
          }
          echo_to_system( AT_ORANGE , ship , buf , NULL );
          ship->currspeed = UMIN(ship->currspeed, (int) ship->realspeed * speedbonus(ship));
       }
       if(!str_cmp(argument, "laser") || !str_cmp(argument, "lasers") )
       {
          if(!IS_SET(ship->flags, SHIPFLAG_ENGINERLASER))
             SET_BIT(ship->flags, SHIPFLAG_ENGINERLASER);
          if(IS_SET(ship->flags, SHIPFLAG_ENGINERSHIELD))
             REMOVE_BIT(ship->flags, SHIPFLAG_ENGINERSHIELD);
          send_to_char("Engine power being redirected to lasers.\n\r", ch);
          if(IS_SET(ship->flags, SHIPFLAG_LASERRENGINE))
          {
              send_to_char("Laser power being redirected back to lasers.\n\r", ch);
              REMOVE_BIT(ship->flags, SHIPFLAG_LASERRENGINE);
          }
          if(!IS_SET(ship->flags, SHIPFLAG_CLOAKED) )
          {
             sprintf( buf, "%s begins to slow down." , ship->name );
             echo_to_system( AT_ORANGE , ship , buf , NULL );
          }
          ship->currspeed = UMIN(ship->currspeed, (int) ship->realspeed * speedbonus(ship));
       }
    }  
    learn_from_success(ch, gsn_shipsystems);
    return;
}

/* For Calculating extra max speed. -Arcturus */
float speedbonus(SHIP_DATA *ship)
{
   float accelbonus;
   accelbonus = 1.0;
   if(IS_SET(ship->flags, SHIPFLAG_OVERDRIVENODE))
      accelbonus *= 2;
   if(IS_SET(ship->flags, SHIPFLAG_AFTERBURNER))
      accelbonus *= 1.1;
   if(IS_SET(ship->flags, SHIPFLAG_LASERRENGINE))
      accelbonus *= 1.1;
   if(IS_SET(ship->flags, SHIPFLAG_SHIELDRENGINE))
      accelbonus *= 1.1;
   if(IS_SET(ship->flags, SHIPFLAG_ENGINERLASER))
      accelbonus *= .9;
   if(IS_SET(ship->flags, SHIPFLAG_ENGINERSHIELD))
      accelbonus *= .9;
   return accelbonus;
}

/* End Simulator Code, Installed by Arcturus, written by Ackbar */
void do_endsimulator( CHAR_DATA *ch, char *argument )
{
  SHIP_DATA *ship;
  char buf[MAX_INPUT_LENGTH];
  if ( (ship = ship_from_cockpit(ch->in_room->vnum)) == NULL )
  {
    send_to_char("You must be in the cockpit of a simulator.\n\r", ch);
    return;
  }	
  if (!IS_SET(ship->flags, SHIPFLAG_SIMULATOR) )
  {
    send_to_char("You must be in the cockpit of a simulator.\n\r", ch);
    return;
  }
	         ship->shipyard = ship->lastdoc;
	  	 ship->shipstate = SHIP_READY;
	         extract_ship( ship );
        	 ship_to_room( ship , ship->shipyard ); 
          	 ship->location = ship->shipyard;
          	 ship->lastdoc = ship->shipyard; 
          	 ship->shipstate = SHIP_DOCKED;    
          	 if (ship->starsystem)
        	 ship_from_starsystem( ship, ship->starsystem );  
                 save_ship(ship);               
          	 send_to_char("The lights dim and the hatch opens.\n\r", ch);
          	 sprintf(buf, "%s suddenly disapears from your viewcreen and off your radar.\n\r", ship->name);
          	 echo_to_system( AT_WHITE, ship , buf , NULL );
}

/* For Evasion- Arcturus */
CHAR_DATA *pilot_from_ship(SHIP_DATA *ship)
{
    int vnum;
    CHAR_DATA *wch;
    vnum = ship->pilotseat;
    for ( wch = first_char; wch; wch = wch->next )
    {
       if(wch->in_room->vnum == vnum && !IS_NPC(wch))
          return wch;
    }
    return NULL;
}      

/* Did he evade or not? -Arcturus */
bool evaded(SHIP_DATA *ship, SHIP_DATA *target)
{
    CHAR_DATA *victim;
    if( (victim = pilot_from_ship(target))) /* Pilot Found */
    {
        int echance;
        echance = victim->pcdata->learned[gsn_evade] / 3+10;
        if(victim->subclass == SUBCLASS_WFOCUS)
           echance += 5;
        if(ship->class > MIDSIZE_SHIP)
           echance += ship->class;
        if(target->class > MIDSIZE_SHIP)
           return FALSE;
        if(number_percent() < echance)
        {
           learn_from_success(victim, gsn_evade);
           return TRUE;
        }
        learn_from_failure(victim, gsn_evade); 
        return FALSE;
     }
     /* No pilot found, can't evade it then ;P */
     return FALSE; 
}

/* Cloaking by Arcturus */
void do_cloak( CHAR_DATA *ch, char *argument )
{
    int chance;
    SHIP_DATA *ship;
    char buf[MAX_STRING_LENGTH];

    if (  (ship = ship_from_cockpit(ch->in_room->vnum))  == NULL )
    {
       send_to_char("&RYou must be in the cockpit of a ship to do that!\n\r",ch);
        return;
    }

    if ( ship->class > MOBILE_SUIT )
    {
       send_to_char("&RThis isn't a spacecraft!\n\r",ch);
       return;
    }
    if (  (ship = ship_from_pilotseat(ch->in_room->vnum))  == NULL )
    {
       send_to_char("&RThe controls must be at the pilots chair...\n\r",ch);
       return;
    }
    if ( autofly(ship) )
    {
       send_to_char("&RYou'll have to turn off the ships autopilot first.\n\r",ch);
       return;
    }
    if (ship->shipstate == SHIP_HYPERSPACE)
    {
       send_to_char("&RYou can only do that in realspace!\n\r",ch);
       return;
    }
    if (ship->shipstate == SHIP_DISABLED)
    {
       send_to_char("&RThe ships drive is disabled. Not enough power to engage cloak.\n\r",ch);
       return;
    }
    if (ship->shipstate == SHIP_DOCKED)
    {
        send_to_char("&RYou can't do that until after you've launched!\n\r",ch);
        return;
    }

    if ((ship->shipstate2 == SHIP_DOCK) || (ship->shipstate2 == SHIP_DOCK_2))
    {
       send_to_char("&RNot while docking procedures are going on.\n\r", ch);
       return;
    }
    if (ship->shipstate2 == SHIP_DOCK_3)
    {
      send_to_char("&RDetach from the docked ship first.\n\r",ch);
      return;
    }
    if (ship->cloak == 0)
    {
      send_to_char("&RTry installing a cloaking device first.\n\r", ch);
      return;
    }
    if(ship->energy < 200)
    {
      send_to_char("&RYou don't have enough fuel.", ch);
      return;
    }
    if(!str_cmp(argument, "off"))
    {
      if (IS_SET(ship->flags, SHIPFLAG_CLOAKED))
      {
        send_to_char("&WYou shut off your cloaking device, and your ship appears.\n\r", ch);
        REMOVE_BIT(ship->flags, SHIPFLAG_CLOAKED);
        act( AT_PLAIN, "$n manipulates the ships controls.", ch, NULL, argument , TO_ROOM );
        sprintf( buf, "%s appears out of nowhere." , ship->name );
        echo_to_system( AT_ORANGE , ship , buf , NULL );
        return;
      }
      else
      {
         send_to_char("&WYour cloaking device isn't active.\n\r", ch);
         return;
      }
    }
      if(IS_SET(ship->flags, SHIPFLAG_CLOAKED))
      {
          send_to_char("&WYour ship is already cloaked. Type cloak off to turn it off.\n\r", ch);
          return;
      }
      chance = IS_NPC(ch) ? ch->top_level : (int) (ch->pcdata->learned[gsn_cloak]);
      if ( number_percent( ) >= chance )
      {
          send_to_char("&RYou fail to work the controls properly.\n\r",ch);
          learn_from_failure(ch, gsn_cloak);
          return;
      }
      learn_from_success(ch, gsn_cloak);
      SET_BIT(ship->flags, SHIPFLAG_CLOAKED);
      send_to_char("Cloaking generator activated.\n\r", ch);
      act( AT_PLAIN, "$n manipulates the ships controls.", ch, NULL, argument , TO_ROOM );
      sprintf( buf, "%s disappears into nowhere." , ship->name );
      echo_to_system( AT_ORANGE , ship , buf , NULL );
      ship->energy -= 200; /* fuel used to activate it. */
      return;
}

