#include <string.h>
#include "mud.h"

ALIAS_DATA *find_alias( CHAR_DATA *ch, char *argument )
{
    ALIAS_DATA *pal;
    char buf[MAX_INPUT_LENGTH];
    
    if (!ch || !ch->pcdata)
        return(NULL);
    
    one_argument(argument, buf);
    
    for (pal=ch->pcdata->first_alias;pal;pal=pal->next)
        if ( !str_prefix(buf, pal->name) )
            return(pal);
    
    return(NULL);
}

void do_alias( CHAR_DATA *ch, char *argument )
{
    ALIAS_DATA *pal = NULL;
    char arg[MAX_INPUT_LENGTH];
    char *p;
    
    if( IS_NPC(ch) )
        return;

    for( p = argument; *p != '\0'; p++ )
    {
       if ( *p == '~' )
 	 {
	    send_to_char( "Command not acceptable, cannot use the ~ character.\n\r", ch );
	    return;
	 }
    }

    argument = one_argument(argument, arg);
    
    if ( !*arg ) 
    {
        if( !ch->pcdata->first_alias )
        {
            send_to_char( "You have no aliases defined!\n\r", ch );
            return;
        }
        pager_printf( ch, "%-20s What it does\n\r", "Alias" );
        for( pal = ch->pcdata->first_alias; pal; pal = pal->next )
            pager_printf( ch, "%-20s %s\n\r", pal->name, pal->cmd );
        return;
    }
    
    if ( !*argument )
    {
        if ( (pal = find_alias(ch, arg)) != NULL )
        {
            DISPOSE(pal->name);
            DISPOSE(pal->cmd);
            UNLINK(pal, ch->pcdata->first_alias, ch->pcdata->last_alias, next, prev);
            DISPOSE(pal);
            send_to_char("Deleted Alias.\n\r", ch);
        } 
	  else
            send_to_char("That alias does not exist.\n\r", ch);
        return;
    }
    
    if( ( pal = find_alias( ch, arg ) ) == NULL )
    {
        CREATE( pal, ALIAS_DATA, 1 );
        pal->name = str_dup( arg );
        pal->cmd  = str_dup( argument );
        LINK( pal, ch->pcdata->first_alias, ch->pcdata->last_alias, next, prev );
        send_to_char( "Created Alias.\n\r", ch );
    } 
    else 
    {
        if( pal->cmd );
        DISPOSE( pal->cmd );
        pal->cmd  = str_dup( argument );
        send_to_char( "Modified Alias.\n\r", ch );
    }
}

void free_aliases( CHAR_DATA *ch )
{
    ALIAS_DATA *pal, *next_pal;
    
    if (!ch || !ch->pcdata)
        return;
    
    for (pal=ch->pcdata->first_alias;pal;pal=next_pal)
    {
        next_pal=pal->next;
        if (pal->name)
            DISPOSE(pal->name);
        if (pal->cmd)
            DISPOSE(pal->cmd);
        DISPOSE( pal );
    }
}

bool check_alias( CHAR_DATA *ch, char *command, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    ALIAS_DATA *alias;

    if ( (alias=find_alias(ch,command)) == NULL )
	return FALSE;

    if (!alias->cmd || !*alias->cmd)
	return FALSE;

    sprintf(arg, "%s", alias->cmd);

    if (ch->cmd_recurse==-1 || ++ch->cmd_recurse>50)
    {
	if (ch->cmd_recurse!=-1)
	{
	    send_to_char("Unable to further process command, recurses too much.\n\r", ch);
	    ch->cmd_recurse=-1;
	}
	return FALSE;
    }

    if (argument && *argument!='\0')
    {
	strcat(arg, " ");
	strcat(arg, argument);
    }

    interpret(ch, arg);
    return TRUE;
}
