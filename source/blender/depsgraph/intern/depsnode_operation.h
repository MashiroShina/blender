/*
 * ***** BEGIN GPL LICENSE BLOCK *****
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 * The Original Code is Copyright (C) 2013 Blender Foundation.
 * All rights reserved.
 *
 * Original Author: Joshua Leung
 * Contributor(s): None Yet
 *
 * ***** END GPL LICENSE BLOCK *****
 */

#ifndef __DEPSNODE_OPERATION_H__
#define __DEPSNODE_OPERATION_H__

#include <stdlib.h>

#include "MEM_guardedalloc.h"

extern "C" {
#include "BLI_utildefines.h"

#include "RNA_access.h"
}

#include "depsnode.h"

#include "depsgraph_util_map.h"
#include "depsgraph_util_set.h"

struct ID;

struct Depsgraph;
struct DepsgraphCopyContext;

/* Identifiers for common operations (as an enum) */
typedef enum eDepsOperation_Code {
	#define DEF_DEG_OPCODE(label) DEG_OPCODE_##label,
	#include "depsnode_opcodes.h"
	#undef DEF_DEG_OPCODE
} eDepsOperation_Code;

/* Flags for Depsgraph Nodes */
typedef enum eDepsOperation_Flag {
	/* node needs to be updated */
	DEPSOP_FLAG_NEEDS_UPDATE       = (1 << 0),
	
	/* node was directly modified, causing need for update */
	/* XXX: intention is to make it easier to tell when we just need to take subgraphs */
	DEPSOP_FLAG_DIRECTLY_MODIFIED  = (1 << 1),

	/* Operation is evaluated using CPython; has GIL and security implications... */
	DEPSOP_FLAG_USES_PYTHON   = (1 << 2),
} eDepsOperation_Flag;

/* Atomic Operation - Base type for all operations */
struct OperationDepsNode : public DepsNode {
	typedef unordered_set<DepsRelation *> Relations;
	
	OperationDepsNode();
	~OperationDepsNode();
	
	void tag_update(Depsgraph *graph);
	
	bool is_noop() const { return (bool)evaluate == false; }
	
	ComponentDepsNode *owner;     /* component that contains the operation */
	
	DepsEvalOperationCb evaluate; /* callback for operation */
	
	Relations inlinks;          /* nodes which this one depends on */
	Relations outlinks;         /* nodes which depend on this one */
	
	uint32_t num_links_pending; /* how many inlinks are we still waiting on before we can be evaluated... */
	float eval_priority;
	bool scheduled;
	
	short optype;                 /* (eDepsOperation_Type) stage of evaluation */
	int   opcode;                 /* (eDepsOperation_Code) identifier for the operation being performed */	
	
	int flag;                     /* (eDepsOperation_Flag) extra settings affecting evaluation */
	int done;                     /* generic tag for traversal algorithms */
	
	DEG_DEPSNODE_DECLARE;
};

void DEG_register_operation_depsnodes();

#endif // __DEPSNODE_OPERATION_H__
