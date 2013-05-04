/*
===========================================================================
Copyright (C) 1999-2005 Id Software, Inc.

This file is part of Daemon.

Daemon is free software; you can redistribute it
and/or modify it under the terms of the GNU General Public License as
published by the Free Software Foundation; either version 2 of the License,
or (at your option) any later version.

Daemon is distributed in the hope that it will be
useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Daemon; if not, write to the Free Software
Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
===========================================================================
*/
#include "g_local.h"

#ifndef __BOT_AI_HEADER
#define __BOT_AI_HEADER

// integer constants given to the behavior tree to use as parameters
// values E_A_SPAWN to E_H_REPEATER are meant to have the same
// integer values as the corresponding enum in buildable_t
// TODO: get rid of dependence on buildable_t
typedef enum
{
	E_NONE,
	E_A_SPAWN,
	E_A_OVERMIND,
	E_A_BARRICADE,
	E_A_ACIDTUBE,
	E_A_TRAPPER,
	E_A_BOOSTER,
	E_A_HIVE,
	E_H_SPAWN,
	E_H_MGTURRET,
	E_H_TESLAGEN,
	E_H_ARMOURY,
	E_H_DCC,
	E_H_MEDISTAT,
	E_H_REACTOR,
	E_H_REPEATER,
	E_GOAL,
	E_ENEMY,
	E_DAMAGEDBUILDING
} AIEntity_t;

// all behavior tree nodes must return one of 
// these status when finished
typedef enum
{
	STATUS_FAILURE = 0,
	STATUS_SUCCESS,
	STATUS_RUNNING
} AINodeStatus_t;

// behavior tree node types
typedef enum
{
	SELECTOR_NODE,
	ACTION_NODE,
	CONDITION_NODE
} AINode_t;

typedef struct
{
	char name[ MAX_QPATH ];
	AINode_t *root;
} AIBehaviorTree_t;

struct AIGenericNode_s;

typedef AINodeStatus_t ( *AINodeRunner )( gentity_t *, struct AIGenericNode_s * );

// all behavior tree nodes must conform to this interface
typedef struct AIGenericNode_s
{
	AINode_t type;
	AINodeRunner run;
} AIGenericNode_t;

#define MAX_NODE_LIST 20
typedef struct
{
	AINode_t type;
	AINodeRunner run;
	AIGenericNode_t *list[ MAX_NODE_LIST ];
	int numNodes;
	int maxFail;
} AINodeList_t;

typedef struct
{
	AINode_t type;
	AINodeRunner run;
	weapon_t weapon;
	upgrade_t upgrades[ 3 ];
	int numUpgrades;
} AIBuyNode_t;

typedef struct
{
	AINode_t type;
	AINodeRunner run;
	AIEntity_t ent;
	float range;
} AIMoveToNode_t;

// operations used in condition nodes
// ordered according to precedence
// lower values == higher precedence
typedef enum
{
	OP_NOT,
	OP_LESSTHAN,
	OP_LESSTHANEQUAL,
	OP_GREATERTHAN,
	OP_GREATERTHANEQUAL,
	OP_EQUAL,
	OP_NEQUAL,
	OP_AND,
	OP_OR,
	OP_NONE
} AIOpType_t;

// types of expressions in condition nodes
typedef enum
{
	EX_OP,
	EX_VALUE,
	EX_FUNC
} AIExpType_t;

typedef enum
{
	VALUE_FLOAT,
	VALUE_INT
} AIValueType_t;

typedef struct
{
	AIExpType_t             expType;
	AIValueType_t           valType;

	union
	{
		float floatValue;
		int   intValue;
	} l;
} AIValue_t;

typedef AIValue_t (*AIFunc)( gentity_t *self, const AIValue_t *params );

typedef struct
{
	AIExpType_t   expType;
	AIValueType_t retType;
	AIFunc        func;
	AIValue_t     *params;
	int           nparams;
} AIValueFunc_t;

// all ops must conform to this interface
typedef struct
{
	AIExpType_t expType;
	AIOpType_t  opType;
} AIOp_t;

typedef struct 
{
	AIExpType_t expType;
	AIOpType_t  opType;
	AIExpType_t *exp1;
	AIExpType_t *exp2;
} AIBinaryOp_t;

typedef struct
{
	AIExpType_t expType;
	AIOpType_t  opType;
	AIExpType_t *exp;
} AIUnaryOp_t;

typedef struct
{
	AINode_t        type;
	AINodeRunner    run;
	AIGenericNode_t *child;
	AIExpType_t     *exp;
} AIConditionNode_t;

typedef struct
{
	AIBehaviorTree_t **trees;
	int numTrees;
	int maxTrees;
} AITreeList_t;

void              FreeBehaviorTree( AIBehaviorTree_t *tree );
AIBehaviorTree_t *ReadBehaviorTree( const char *name, AITreeList_t *list );
void              FreeTreeList( AITreeList_t *list );
void              InitTreeList( AITreeList_t *list );

// standard behavior tree control-flow nodes
AINodeStatus_t BotEvaluateNode( gentity_t *self, AIGenericNode_t *node );
AINodeStatus_t BotConditionNode( gentity_t *self, AIGenericNode_t *node );
AINodeStatus_t BotSelectorNode( gentity_t *self, AIGenericNode_t *node );
AINodeStatus_t BotSequenceNode( gentity_t *self, AIGenericNode_t *node );
AINodeStatus_t BotPriorityNode( gentity_t *self, AIGenericNode_t *node );
AINodeStatus_t BotParallelNode( gentity_t *self, AIGenericNode_t *node );

// action nodes
AINodeStatus_t BotActionFight( gentity_t *self, AIGenericNode_t *node );
AINodeStatus_t BotActionBuy( gentity_t *self, AIGenericNode_t *node );
AINodeStatus_t BotActionRepair( gentity_t *self, AIGenericNode_t *node );
AINodeStatus_t BotActionEvolve ( gentity_t *self, AIGenericNode_t *node );
AINodeStatus_t BotActionHealH( gentity_t *self, AIGenericNode_t *node );
AINodeStatus_t BotActionHealA( gentity_t *self, AIGenericNode_t *node );
AINodeStatus_t BotActionHeal( gentity_t *self, AIGenericNode_t *node );
AINodeStatus_t BotActionFlee( gentity_t *self, AIGenericNode_t *node );
AINodeStatus_t BotActionRoam( gentity_t *self, AIGenericNode_t *node );
AINodeStatus_t BotActionMoveTo( gentity_t *self, AIGenericNode_t *node );
AINodeStatus_t BotActionRush( gentity_t *self, AIGenericNode_t *node );
#endif