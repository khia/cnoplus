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

#define _GNU_SOURCE 
#include <stdio.h> // we need [fputs, asprintf] functions from here
#include <stdlib.h> // we need exit function from here
#include <unistd.h> // we need [read, write] functions from here
#include <ctype.h> // we need [islower, isalnum] functions from here
#include <string.h>
#include <malloc.h>
#include <pthread.h>
#include <dlfcn.h>

#include "common.h"
/*
  ==============================================================
  GLOBAL VARIABLES
  ==============================================================
*/

exception_handler_osa * e_DivisionOnZero_g;
exception_handler_osa * e_NotEnoughMemory_g;
exception_handler_osa * e_UncathedException_g; 
exception_handler_osa * e_NullPointer_g; 
root_osa * Root_g;

/*
  ==============================================================
  END OF GLOBAL VARIABLES
  ==============================================================
*/

int main_int(int argc, char * argv[], exception_stack_osa * Stack);

int main (int argc, char * argv[]) {
     init_oop();
     DEBUG(HEAVY, "testing counter %i", main, OBJECT_LAST);
     Root_g = new(root_o, NULL);
     if (NULL == Root_g) {
          ERROR("Cannot create root object.");
          exit (-1);	
     }
     DEBUG(HEAVY, "Root was created %p", main, Root_g);
     exception_stack_osa * Stack = new(exception_stack_o, Root_g);
     if (NULL == Stack) {
          ERROR("Cannot create exception_stack object.");
          exit (-1);
     }
     DEBUG(HEAVY, "Root of Stack object is %p %ld %s", main, root(Stack),
           type_of(Stack),	name_of(Stack));
  
     Root_g->set_stack(Root_g, Stack);
     REGISTER_EXCEPTIONS(Stack);
     TRY {
          main_int(argc, argv, Stack);
     } CATCH (e_DivisionOnZero_g) {
          DEBUG(HEAVY, "DivisionOnZero", main);
          // FIXME It crashes here
          //e_DivisionOnZero_g->handler(e_DivisionOnZero_g->id, "Division on zero.",
          //                            Stack);
     } ENDTRY;
     DEBUG(HEAVY, "RETURN", main);
     return 0;
}
