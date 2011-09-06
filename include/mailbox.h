/*
DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS HEADER.
Copyright (c) 2009-2011, ILYA Khlopotov. All rights reserved.

The contents of this file are subject to the terms of either the GNU
General Public License ("GPL") version 2, or any later version,
or the Common Development and Distribution License ("CDDL") 
(collectively, the "License"). 
You may not use this file except in compliance with the License. 
 
This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

You should have received a copy of the GNU General Public License
along with this program.  If not, see http://www.gnu.org/licenses/

You should have received a copy of the 
Common Development and Distribution License along with this program.  
If not, see http://www.opensource.org/licenses/
*/

#ifndef MAILBOX_H
#define MAILBOX_H

/*
  ==============================================================
  EXPORT DEFINITIONS
  ==============================================================
*/

struct mailbox_osd; typedef struct mailbox_osd mailbox_osa; 

/*
  ==============================================================
  IMPORT 
  ==============================================================
*/

#include "counter.h"
#include "object.h"
#include "exception.h"

/*
  ==============================================================
  GLOBAL VARIABLES
  ==============================================================
*/
// can be thrown by exception_o object itself
extern exception_handler_osa * e_mailbox_cannot_push_g;
extern exception_handler_osa * e_mailbox_cannot_pop_g;

/*
  ==============================================================
  GLOBAL DEFINITIONS
  ==============================================================
*/

enum mailbox_object_types_e {
  OBJECT_mailbox_EE = OBJECT_LAST,
  OBJECT_MAILBOX_LAST_EE
};
// this is a hack that works only with gcc
#undef OBJECT_LAST
#define OBJECT_LAST OBJECT_MAILBOX_LAST_EE

/*
  ==============================================================
  IMPLEMENTATION
  ==============================================================
*/

struct mailbox_osd {
  const object_osa * type_info;
  object_properties_osa * properties;  
  linked_list_osa * queue;
  int length;
  // methods 
  void * (* get)(mailbox_osa * Self);
  void * (* put)(mailbox_osa * Self, void * Message);
};// mailbox_osa;

extern const void * mailbox_o;

#endif
