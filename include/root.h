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

#ifndef ROOT_H
#define ROOT_H

#include "counter.h"
#include "object.h"
#include "exception.h"

struct root_osd; typedef struct root_osd root_osa; 

struct root_osd {
  const object_osa * type_info;
  object_properties_osa * properties;  
  exception_stack_osa * exception_stack;
  counter_osa * type_idx;
  // methods 
  int (* get_type)(root_osa * Self);
  void * (* get_stack)(root_osa * Self);
  void * (* set_stack)(root_osa * Self, exception_stack_osa * Stack);
};// root_osa;

extern const void * root_o;

#endif
