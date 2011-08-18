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
#include <stdarg.h> // we need va_start, va_end macros from here
#include <string.h>
#include <malloc.h>
#include <stdlib.h> // we need exit from here
#include <pthread.h> // mutexes from here
#include <setjmp.h>

#include "common.h"

// START of root object definition

static int root_get_type(root_osa * Self);
static void * root_get_stack(root_osa * Self);
static void * root_set_stack(root_osa * Self, exception_stack_osa * Stack);

static void * root_ctor(void * Self_v, void * Properties, va_list * Args) {
  struct root_osd * Self = Self_v;
  DEBUG(ENTRY, "About to start \"%s(%p, %p,...)\" ctor.", root_ctor,
		"root_ctor", Self_v, Properties);
  ASSERT(NULL == Self, NULL_POINTER_OBJECT, NULL,
		 "Constructor was invoked without buffer allocated.");  
  Self->properties = Properties;  
  Self->exception_stack = NULL;
  Self->get_type = root_get_type;
  Self->get_stack = root_get_stack;
  Self->set_stack = root_set_stack;
  return Self;
}

static void * root_dtor(void * Self_v) {
  struct root_osd * Self = Self_v;
  DEBUG(ENTRY, "About to start \"%s(%p)\" dtor.", root_dtor, "root_dtor", Self_v);
  return Self;
}

/*
  next macro using naming conventions:
  1. coonsctuctors has name suffix _ctor
  2. destructors has name suffix _dtor
  3. type enumeration has prefix OBJECT_ and suffix _EE
  4. structure that describes the object has suffix _osd
  5. result object variabe has suffix _od
*/
OBJECT(root, NULL, NULL, NULL);
const void * root_o = &root_od;

static int root_get_type(root_osa * Self) {
  ASSERT(NULL == Self, NULL_POINTER_OBJECT, NOK,
		 "You are trying to root_get_type from object at address NULL.");
  return Self->type_idx->inc(Self->type_idx);
}

static void * root_get_stack(root_osa * Self) {
  ASSERT(NULL == Self, NULL_POINTER_OBJECT, NOK,
		 "You are trying to root_get_stack from object at address NULL.");
  return Self->exception_stack;  
}

static void * root_set_stack(root_osa * Self, exception_stack_osa * Stack) {
  ASSERT(NULL == Self, NULL_POINTER_OBJECT, NOK,
		 "You are trying to root_set_stack for object at address NULL.");
  ASSERT(NULL == Stack, NULL_POINTER_OBJECT, NOK,
		 "You are trying to root_set_stack with Stack at address NULL.");
  Self->exception_stack = Stack;
  return Self->exception_stack;
}

// END of root object definition
