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
#include "mailbox.h"
/*
  ==============================================================
  GLOBAL VARIABLES
  ==============================================================
*/


/*
  ==============================================================
  END OF GLOBAL VARIABLES
  ==============================================================
*/
//static int loop(mailbox_osa * Self);
static void * mailbox_put(mailbox_osa * Self, void * Message);
static void * mailbox_get(mailbox_osa * Self);

static void * mailbox_ctor(void * Self_v, void * Properties, va_list * Args) {
  struct mailbox_osd * Self = Self_v;
  DEBUG(ENTRY, "About to start \"%s(%p, %p,...)\" ctor.", mailbox_ctor,
		"mailbox_ctor", Self_v, Properties);
  ASSERT(NULL == Self, NULL_POINTER_OBJECT, NULL,
		 "Constructor was invoked without buffer allocated.");  
  Self->properties = Properties;
//  Self->type =va_arg(* Args, int);
  Self->get = mailbox_get;
  Self->put = mailbox_put;  
  return Self;
}

static void * mailbox_dtor(void * Self_v) {
  struct mailbox_osd * Self = Self_v;
  DEBUG(ENTRY, "About to start \"%s(%p)\" dtor.", mailbox_dtor, "mailbox_dtor", Self_v);
  return Self;
}

/*
  next macro using naming conventions:
  1. constructors has name sumailboxx _ctor
  2. destructors has name sumailboxx _dtor
  3. type enumeration has prefix OBJECT_ and sumailboxx _EE
  4. structure that describes the object has sumailboxx _osd
  5. result object variabe has sumailboxx _od
*/
OBJECT(mailbox, NULL, NULL, NULL);
const void * mailbox_o = &mailbox_od;

static void * mailbox_put(mailbox_osa * Self, void * Message) {
//  struct mailbox_osd * Self = Self_v;
  return Self->queue->push_r(Self->queue, Message);  
}

static void * mailbox_get(mailbox_osa * Self) {
//  struct mailbox_osd * Self = Self_v;
  return Self->queue->pop_r(Self->queue);  
}

// END of mailbox object definition
