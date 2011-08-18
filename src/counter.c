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

/*
  ==============================================================
  START OF counter object definition 
  ==============================================================
*/

static int counter_get(counter_osa * Self);
static int counter_inc(counter_osa * Self);
static int counter_set(counter_osa * Self, const int N);

static void * counter_ctor(void * Self_v, void * Properties, va_list * Args) {
  struct counter_osd * Self = Self_v;
  DEBUG(ENTRY, "About to start \"%s(%p, %p,...)\" ctor.", counter_ctor,
		"counter_ctor", Self_v, Properties);
  ASSERT(NULL == Self, NULL_POINTER_OBJECT, NULL,
		 "Constructor was invoked without buffer allocated.");  
  const int Value = va_arg(* Args, int); 
  Self->properties = Properties;
  Self->value = Value;
  Self->get = counter_get;
  Self->inc = counter_inc;
  Self->set = counter_set;
  return Self;
}

static void * counter_dtor(void * Self_v) {
  struct counter_osd * Self = Self_v;
  DEBUG(HEAVY, "About to start \"%s(%p)\" dtor.", counter_dtor,
		"counter_dtor", Self_v);
  return Self;
}

OBJECT(counter, NULL, NULL, NULL);
const void * counter_o = &counter_od;

static int counter_get(counter_osa * Self) {
  ASSERT(NULL == Self, NULL_POINTER_OBJECT, NOK,
		 "You are trying to counter_get from object at adress NULL.");
  return Self->value;
}
static int counter_inc(counter_osa * Self) {
  ASSERT(NULL == Self, NULL_POINTER_OBJECT, NOK,
		 "You are trying to counter_inc from object at adress NULL.");
  Self->value = Self->value + 1;
  return Self->value;  
}
static int counter_set(counter_osa * Self, const int N) {
  ASSERT(NULL == Self, NULL_POINTER_OBJECT, NOK,
		 "You are trying to counter_set from object at adress NULL.");
  Self->value = N;
  return Self->value;
}

/*
  ==============================================================
  END OF counter object definition 
  ==============================================================
*/

