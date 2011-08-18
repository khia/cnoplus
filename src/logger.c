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

#include "logger.h"

int get_debug_level(void * fun_addr) {
/*   if (&print_stack_element == fun_addr) { */
/*     return NO_DEBUG; */
/*   } */
/*   if (&print_trace_stack == fun_addr) { */
/*     return NO_DEBUG; */
/*   } */
//  if (&new == fun_addr) {
//  printf("+++++++++++++++++++fun_addr = %p\n", fun_addr);
//  }
  return HEAVY;
}
