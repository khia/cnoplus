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

/*
  ==============================================================
  END OF GLOBAL VARIABLES
  ==============================================================
*/


int divide(int x, int y, exception_stack_osa * Exceptions) {
//  THROWS(Exceptions, DivisionOnZero_g);
  int Res; // place for result
  if (0 == y) {
    DEBUG(HEAVY, "Throwing", divide);
    DEBUG(HEAVY, "Throwing: %i", divide, e_DivisionOnZero_g->id);
    THROW(e_DivisionOnZero_g);									  
  }
  Res = x/y; 
	DEBUG(HEAVY, "Result == %i", divide, Res);
  RETURN (Res);
}

int helper(int x, int y, exception_stack_osa * Stack) {
  int Res = 0;
  TRY{
	  Res = divide(x, y, Stack);
    DEBUG(HEAVY, "Result == %i", helper, Res);
	} CATCH (e_DivisionOnZero_g) {
	  DEBUG(HEAVY, "After == %p", helper, (Stack)->stack_head);
	  THROW(e_NotEnoughMemory_g);
	} ENDTRY;
	DEBUG(HEAVY, "Result == %i", helper, Res);
	RETURN (Res);
}

int main_int(int argc, char * argv[], exception_stack_osa * Stack) {
    int res;
    TRY{
	  res = helper(6, 2, Stack);
	  DEBUG(HEAVY, "!!!res!!!!!! %i",	main_int, res);	
	} CATCH (e_DivisionOnZero_g) {
	  THROW(e_DivisionOnZero_g); 
	} ENDTRY;

	TRY{
    res = divide(6, 0, Stack);
	  DEBUG(HEAVY, "!!!res!!!!!! %i",	main_int, res);	
	} CATCH (e_DivisionOnZero_g) {
	  DEBUG(HEAVY, "!!!THROW!!!!!",	main_int);	
	  THROW(e_DivisionOnZero_g); 
	} ENDTRY;

  TRY{
	  res = helper(6, 0, Stack);
	  DEBUG(HEAVY, "!!!res!!!!!! %i",	main_int, res);	
	} CATCH (e_DivisionOnZero_g) {
	  DEBUG(HEAVY, "!!!THROW!!!!!",	main_int);	
	  THROW(e_DivisionOnZero_g); 
	} ENDTRY;
  
  return 0;
}
