/*
===========================================================================

Daemon GPL Source Code
Copyright (C) 2012 Unvanquished Developers

This file is part of the Daemon GPL Source Code (Daemon Source Code).

Daemon Source Code is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Daemon Source Code is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Daemon Source Code.  If not, see <http://www.gnu.org/licenses/>.

In addition, the Daemon Source Code is also subject to certain additional terms.
You should have received a copy of these additional terms immediately following the
terms and conditions of the GNU General Public License which accompanied the Daemon
Source Code.  If not, please request a copy in writing from id Software at the address
below.

If you have questions concerning this license or the applicable additional terms, you
may contact in writing id Software LLC, c/o ZeniMax Media Inc., Suite 120, Rockville,
Maryland 20850 USA.

===========================================================================
*/

#include "cg_local.h"

vmCvar_t rocket_menuFiles;

typedef struct
{
	vmCvar_t   *vmCvar;
	const char *cvarName;
	const char *defaultString;
	int        cvarFlags;
} cvarTable_t;

static const cvarTable_t rocketCvarTable[] =
{
	{ &rocket_menuFiles, "rocket_menuFiles", "rocket.txt", CVAR_ARCHIVE }
};

static const size_t rocketCvarTableSize = ARRAY_LEN( rocketCvarTable );

/*
=================
CG_RegisterRocketCvars
=================
*/
void CG_RegisterRocketCvars( void )
{
	int         i;
	const cvarTable_t *cv;

	for ( i = 0, cv = rocketCvarTable; i < rocketCvarTableSize; i++, cv++ )
	{
		trap_Cvar_Register( cv->vmCvar, cv->cvarName,
		                    cv->defaultString, cv->cvarFlags );
	}
}

void CG_Rocket_Init( void )
{
	int i, len;
	char *token, *text_p;
	char text[ 20000 ];
	fileHandle_t f;

	// Version check...
	trap_SyscallABIVersion( SYSCALL_ABI_VERSION_MAJOR, SYSCALL_ABI_VERSION_MINOR );

	// Init Rocket
	trap_Rocket_Init();

	// load overrides
	BG_InitClassConfigs();
	BG_InitBuildableConfigs();
	BG_InitAllowedGameElements();

	// Dynamic memory
	BG_InitMemory();

	// rocket cvars
	CG_RegisterRocketCvars();

	// Preload all the menu files...
	len = trap_FS_FOpenFile( rocket_menuFiles.string, &f, FS_READ );
	if (  len <= 0 )
	{
		Com_Error( ERR_DROP, "Unable to load %s. No rocket menus loaded.", rocket_menuFiles.string );
	}

	if ( len >= sizeof( text ) - 1 )
	{
		trap_FS_FCloseFile( f );
		Com_Error( ERR_DROP, "File %s too long.", rocket_menuFiles.string );
	}

	trap_FS_Read( text, len, f );
	text[ len ] = 0;
	text_p = text;
	trap_FS_FCloseFile( f );

	// Parse files to load...
	while ( 1 )
	{
		token = COM_Parse2( &text_p );

		// Closing bracket. EOF
		if ( !*token || *token == '}' )
		{
			break;
		}

		// Ignore openning bracket
		if ( *token == '{' )
		{
			continue;
		}

		// Set the cursor
		if ( !Q_stricmp( token, "cursor" ) )
		{
			token = COM_Parse2( &text_p );

			// Skip non-RML files
			if ( Q_stricmp( token + strlen( token ) - 4, ".rml" ) )
			{
				continue;
			}

			trap_Rocket_LoadCursor( token );
			continue;
		}

		// Skip non-RML files
		if ( Q_stricmp( token + strlen( token ) - 4, ".rml" ) )
		{
			continue;
		}

		trap_Rocket_LoadDocument( token );
	}

	trap_Rocket_DocumentAction( "main", "open" );
}

static void CG_Rocket_EventOpen( const char *args )
{
	trap_Rocket_DocumentAction( args, "open" );
}

static void CG_Rocket_EventClose( const char *args )
{
	trap_Rocket_DocumentAction( args, "close" );
}

static void CG_Rocket_EventGoto( const char *args )
{
	trap_Rocket_DocumentAction( args, "goto" );
}

static void CG_Rocket_Test( const char *args )
{
	char data[ MAX_INFO_STRING ] = { 0 };
	int i;
	trap_Rocket_RegisterDataSource( "high_scores" );

	for ( i = 0; i < 5; ++i )
	{
		Com_Memset( &data, 0, sizeof( data ) );
		Info_SetValueForKey( data, "name", "Ishq", qfalse );
		Info_SetValueForKey( data, "colour", "Red", qfalse );
		Info_SetValueForKey( data, "wave", va( "%d", i ), qfalse );
		Info_SetValueForKey( data, "score", va( "%d", i * rand() % 50 ), qfalse );
		trap_Rocket_DSAddRow( "high_scores", "scores", data );
	}

}


typedef struct
{
	const char *command;
	void ( *exec ) ( const char *args );
} eventCmd_t;

static const eventCmd_t eventCmdList[] =
{
	{ "add_score", &CG_Rocket_Test },
	{ "close", &CG_Rocket_EventClose },
	{ "goto", &CG_Rocket_EventGoto },
	{ "open", &CG_Rocket_EventOpen },
	{ "show", &CG_Rocket_EventOpen }
};

static const size_t eventCmdListCount = ARRAY_LEN( eventCmdList );

static int eventCmdCmp( const void *a, const void *b )
{
	return Q_stricmp( ( const char * ) a, ( ( eventCmd_t * ) b )->command );
}

void CG_Rocket_ProcessEvents( int handle )
{
	static char commands[ 2000 ];
	char *tail, *head;
	eventCmd_t *cmd;

	// Get the even command
	trap_Rocket_GetEvent( handle, commands, sizeof( commands ) );

	head = commands;
	while ( 1 )
	{
		char *p, *args;

		// Parse it. Check for semicolons first
		tail = strchr( head, ';' );
		if ( tail )
		{
			*tail = '\0';
		}

		p = strchr( head, ' ' );
		if ( p )
		{
			*p = '\0';
		}

		args = head + strlen( head ) + 1;

		cmd = bsearch( head, eventCmdList, eventCmdListCount, sizeof( eventCmd_t ), eventCmdCmp );

		if ( cmd )
		{
			cmd->exec( args );
		}

		head = args + strlen( args ) + 1;

		if ( !*head )
		{
			break;
		}

		// Skip whitespaces
		while ( *head == ' ' )
		{
			head++;
		}
	}

	trap_Rocket_DeleteEvent( handle );
}

