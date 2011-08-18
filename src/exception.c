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
  GLOBAL VARIABLES
  ==============================================================
*/

exception_handler_osa * e_exception_cannot_push_g = NULL;
exception_handler_osa * e_exception_cannot_pop_g = NULL;
exception_handler_osa * e_exception_cannot_print_g = NULL;
/*
  ==============================================================
  END OF GLOBAL VARIABLES
  ==============================================================
*/

// START of exception_handler object definition
// TODO implement function to invoke handler by id
static void * exception_handler_ctor(void * Self_v, void * Properties,
									 va_list * Args) {
  struct exception_handler_osd * Self = Self_v;
  DEBUG(ENTRY, "About to start \"%s(%p,%p,...)\" ctor.", exception_handler_ctor,
		"exception_handler_ctor", Self, Properties);
  Self->properties = Properties;  
  Self->name =va_arg(* Args, char *);
  Self->id = va_arg(* Args, long);
  Self->handler = va_arg(* Args, void *); 
  return Self;
}

static void * exception_handler_dtor(void * Self_v) {
  struct exception_handler_osd * Self = Self_v;
  DEBUG(ENTRY, "About to start \"%s(%p)\" dtor.", exception_handler_dtor,
		"exception_handler_dtor", Self);
  ASSERT(NULL == Self, NULL_POINTER_OBJECT, NULL,
		 "You are trying to remove exception_stack at NULL address.");
  return Self;
}
/*
static const struct object_osd exception_handler_od = {
  sizeof(struct exception_handler_osd),
  OBJECT_EXCEPTION_HANDLER_EE, "exception_handler",  
  exception_handler_ctor, exception_handler_dtor,
  NULL, NULL, NULL
};
*/
OBJECT(exception_handler, NULL, NULL, NULL);

const void * exception_handler_o = &exception_handler_od;


// END of exception_handler object definition
// 
// START of exception_stack_node object definition

static void * exception_stack_node_ctor(void * Self_v, void * Properties,
										va_list * Args) {
  struct exception_stack_node_osd * Self = Self_v;
  DEBUG(HEAVY, "About to start \"%s(%p,%p,...)\" ctor.",
		exception_stack_node_ctor, 
		"exception_stack_node_ctor", Self, Properties);
  char * Module = va_arg(* Args, char *); 
  char * Function = va_arg(* Args, char *); 
  int Line = va_arg(* Args, int); 
  Self->properties = Properties;
  Self->module = Module;
  Self->function = Function;
  Self->line = Line;
  return Self;
}

static void * exception_stack_node_dtor(void * Self_v) {
  struct exception_stack_node_osd * Self = Self_v;
  DEBUG(ENTRY, "About to start \"%s(%p)\" dtor.", exception_stack_node_dtor,
		"exception_stack_node_dtor", Self);
  ASSERT(NULL == Self, NULL_POINTER_OBJECT, NULL,
		 "You are trying to remove exception_stack_node at NULL address.");
  return Self;
}

int exception_stack_node_print(void * Self_v, char ** Buffer,
							   const struct printf_info * Info) {
  struct exception_stack_node_osd * Self = Self_v;
  DEBUG(ENTRY, "About to start \"%s\".", exception_stack_node_print,
		"exception_stack_node_print");
  ASSERT(NULL == Self, NULL_POINTER_OBJECT, NOK,
		 "You are trying to print object at NULL address.");
  ASSERT(NULL == Buffer, NULL_POINTER_OBJECT, NOK,
		 "You are trying to print object in buffer at NULL address.");

  char * Output;
  int Len = asprintf(&Output, "<%s %p: [module = %s, function = %s, line = %i]>",
				 name_of(Self), Self, Self->module,
				 Self->function, Self->line);

  ASSERT(0 > Len, e_exception_cannot_print_g, NOK,
		 "Cannot print exception_stack_node.");
  
  DEBUG(HEAVY, "Result \"%s\" (%p). Buffer = %p *Buffer = %p",		
		exception_stack_node_print, Output, Output, Buffer, *Buffer);
  *Buffer = Output;
  return Len;
}
/*
static const struct object_osd exception_stack_node_od = {
  sizeof(struct exception_stack_node_osd),
  OBJECT_EXCEPTION_STACK_NODE_EE, "exception_stack_node", 
  exception_stack_node_ctor,
  exception_stack_node_dtor,
  NULL, NULL,
  exception_stack_node_print  
};
*/
OBJECT(exception_stack_node, NULL, NULL, exception_stack_node_print);

const void * exception_stack_node_o = &exception_stack_node_od;

// END of exception_stack_node object definition

void default_exception_handler(int  Return_Code, char * Message, 
			      exception_stack_osa * Stack) {
//  ASSERT(NULL == Message, NULL_POINTER_OBJECT, NOK,
//		 "You are trying to Message at NULL address.");  
  fprintf(stderr, "%i: %s%s", Stack->current_exception, Message, NEWLINE);
  fprintf(stderr, "\n%O\n", Stack);
  free(Message);
  delete(Stack);
  exit (Return_Code);  
}

void e_exception_cannot_push_handler(
		int Cause, char * Message, exception_stack_osa * Stack) {
  DEBUG(ENTRY, "About to %s(%i,%s,%p).", e_exception_cannot_push_handler,
		"e_exception_cannot_push_handler", Cause, Message, Stack);
  // here we can redifine message or stack
  // and clean our memory 
  // -1 reason for program termination
  default_exception_handler(-1, Message, Stack); 
}

void e_exception_cannot_pop_handler(
		int Cause, char * Message, exception_stack_osa * Stack) {
  DEBUG(ENTRY, "About to %s(%i,%s,%p).", e_exception_cannot_pop_handler,
		"e_exception_cannot_pop_handler", Cause, Message, Stack);
  // here we can redifine message or stack
  // and clean our memory 
  // -1 reason for program termination
  default_exception_handler(-1, Message, Stack); 
}

void e_exception_cannot_print_handler(
		int Cause, char * Message, exception_stack_osa * Stack) {
  DEBUG(ENTRY, "About to %s(%i,%s,%p).", e_exception_cannot_print_handler,
		"e_exception_cannot_print_handler", Cause, Message, Stack);
  // here we can redifine message or stack
  // and clean our memory 
  // -1 reason for program termination
  default_exception_handler(-1, Message, Stack); 
}

// START of exception_stack object definition

void * exception_stack_push(exception_stack_osa * Self, 
                            char * Module, char * Function, int Line);
void * exception_stack_pop(exception_stack_osa * Self);
int exception_stack_print(void * Self, char ** Buffer,
                          const struct printf_info * Info);
exception_handler_osa * exception_stack_add_handler(
        exception_stack_osa * Self,
        const char * ExceptionName,
        exception_handler_osa * Handler);
int exception_stack_catch(exception_stack_osa * Self,
                          exception_handler_osa * Exception);
int add_exception_handlers(exception_stack_osa * Stack);

/* I cannot use ASSERT macro because exception_stack is not ready yet */
static void * exception_stack_ctor(void * Self_v, void * Properties,
								   va_list * Args){
  struct exception_stack_osd * Self = Self_v;
  DEBUG(ENTRY, "About to start \"%s(%p,%p,...)\" ctor.", exception_stack_ctor,
		"exception_stack_ctor", Self, Properties);
  if (NULL == Self) {
	ERROR("Exception stack constructor was invoked without buffer allocated");
	return NULL;
  }
  Self->is_ready = FALSE;
  Self->properties = Properties;
  Self->stack = new(linked_list_o, Self);
  if (NULL == Self->stack) {
	ERROR("Cannot create stack for exceptions.");
	return NULL;
  }

  Self->exceptions = new(linked_list_o, Self);
  if (NULL == Self->exceptions) {
	ERROR("Cannot create stack for exceptions.");
	delete(Self->stack);
	return NULL;
  }

  counter_osa * Counter = new(counter_o, Self, 0);
  if (NULL == Counter) {
	ERROR("Cannot create counter for exceptions.");
	delete(Self->exceptions);
	delete(Self->stack);
	return NULL;
  }

  Self->index_counter = Counter;
  Self->push = exception_stack_push;
  Self->pop = exception_stack_pop;
  Self->catch = exception_stack_catch;
  Self->add_handler = exception_stack_add_handler;

  // Creating base exceptions
  e_DivisionOnZero_g = Self->add_handler(Self, "DivisionOnZero", &default_exception_handler);
  e_NotEnoughMemory_g = Self->add_handler(Self, "NotEnoughMemory", &default_exception_handler);
  e_UncathedException_g = Self->add_handler(Self, "UncathedException", &default_exception_handler);
  e_NullPointer_g = Self->add_handler(Self, "NullPointer", &default_exception_handler);
  if ((NULL == e_DivisionOnZero_g)
      || (NULL == e_NotEnoughMemory_g)
      || (NULL == e_UncathedException_g)
      || (NULL == e_NullPointer_g)) {
    ERROR("Cannot create base exceptions.");
    delete(Counter);
    delete(Self->exceptions);
    delete(Self->stack);
    return NULL;     
  }
  
  // exceptions_stack ready for using  
  e_exception_cannot_push_g =
    Self->add_handler(Self, "e_exception_cannot_push_g", &e_exception_cannot_push_handler);
  ASSERT(NULL == e_exception_cannot_push_g, NULL_POINTER_OBJECT, NOK,
         "Cannot create exception handler %s.", "e_exception_cannot_push_g");

  e_exception_cannot_pop_g =
    Self->add_handler(Self, "e_exception_cannot_pop_g", &e_exception_cannot_pop_handler);
  ASSERT(NULL == e_exception_cannot_push_g, NULL_POINTER_OBJECT, NOK,
         "Cannot create exception handler %s.", "e_exception_cannot_push_g");

  e_exception_cannot_print_g =
    Self->add_handler(Self, "e_exception_cannot_print_g", &e_exception_cannot_print_handler);
  ASSERT(NULL == e_exception_cannot_print_g, NULL_POINTER_OBJECT, NOK,
         "Cannot create exception handler %s.", "e_exception_cannot_print_g");
  Self->print=exception_stack_print;
  Self->is_ready = TRUE;
  return Self;  
}

static void * exception_stack_dtor(void * Self_v) {
  struct exception_stack_osd * Self = Self_v;
  DEBUG(ENTRY, "About to start \"%s(%p)\" dtor.", exception_stack_dtor,
		"exception_stack_dtor", Self);
  ASSERT(NULL == Self, NULL_POINTER_OBJECT, NULL,
		 "Destructor was invoked with NULL argumant.");
  if (NULL == Self->stack) {
	return Self;
  }
  delete(Self->stack);
  return Self;
}

int exception_stack_print(void * Self_v, char ** Buffer,
							   const struct printf_info * Info) {
  struct exception_stack_osd * Self = Self_v;
  DEBUG(ENTRY, "About to %s(%p).", exception_stack_print,
		"exception_stack_print", Self);
  ASSERT(NULL == Self, NULL_POINTER_OBJECT, -1,
		 "You are trying to print linked_element at address NULL.");
  ASSERT(NULL == Self->stack, NULL_POINTER_OBJECT, -1,
		 "Self->stack situated at address NULL.");
  ASSERT(NULL == Buffer, NULL_POINTER_OBJECT, NOK,
		 "You are trying to print object in buffer at NULL address.");
  object_osa * TypeInfo = (object_osa *)Self->stack->type_info;
  ASSERT(NULL == TypeInfo, NULL_POINTER_OBJECT, -1,
		 "Self->stack->type_info situated at address NULL.");
  int Len = TypeInfo->print(Self->stack, Buffer, Info);
  ASSERT(0 > Len, e_exception_cannot_print_g, NOK,
		 "Cannot print exception_stack_node.");
  return Len;
}

/*
static const struct object_osd exception_stack_od = {
  sizeof(struct exception_stack_osd),
  OBJECT_EXCEPTION_STACK_EE, "exception_stack",
  exception_stack_ctor, exception_stack_dtor,
  NULL, NULL, exception_stack_print
};
*/
OBJECT(exception_stack, NULL, NULL, exception_stack_print);

const void * exception_stack_o = &exception_stack_od;

void * exception_stack_push(exception_stack_osa * Self, 
			   char * Module, char * Function, int Line) {
  DEBUG(ENTRY, "About to %s(%p, %s, %s, %i).", exception_stack_push,
		"exception_stack_push", Self, Module, Function, Line);  
  ASSERT(NULL == Self, NULL_POINTER_OBJECT, NULL,
		 "You are trying to exception_stack_push to stack at NULL adress.");
  exception_stack_node_osa * Node =
	new(exception_stack_node_o, Self, Module, Function, Line);
  ASSERT(NULL == Node, e_exception_cannot_push_g, NULL, 
		 "Cannot create object to store module, function, line of current "
		 "invokation.");
  ASSERT(NULL == Self->stack, e_exception_cannot_push_g, NULL, 
		 "Cannot push object to stack Self->stack is NULL.");
  Self->stack = Self->stack->push_l(Self->stack, Node);
  ASSERT(NULL == Self->stack, e_exception_cannot_push_g, NULL, 
		 "Cannot push object to store module, function, line of current "
		 "invokation into stack.");  
  return Node;
}

void * exception_stack_pop(exception_stack_osa * Self) {
  DEBUG(ENTRY, "About to %s(%p).", exception_stack_pop,
		"exception_stack_pop", Self);
  ASSERT(NULL == Self, NULL_POINTER_OBJECT, NULL,
		 "You are trying to exception_stack_pop from stack at NULL adress.");
  ASSERT(NULL == Self->stack, e_exception_cannot_pop_g, NULL, 
		 "Cannot pop object from stack Self->stack is NULL.");
  linked_list_osa * Stack = Self->stack;
  exception_stack_node_osa * Node = Stack->pop_l(Stack);
  ASSERT(NULL == Node, e_exception_cannot_pop_g, NULL,
         "Cannot pop object from stack.");
  DEBUG(ENTRY, "Rollback %s:%i:%s", exception_stack_pop,
        Node->module, Node->line, Node->function);
  return (Node);
}

exception_handler_osa * exception_stack_add_handler(
        exception_stack_osa * Self, const char * ExceptionName,
        exception_handler_osa * Handler) {
  exception_handler_osa * e_NewException;
  DEBUG(ENTRY, "About to %s(%p, %p).", exception_stack_add_handler,
		"exception_stack_add_handler", Self, Handler);
  if (NULL == Self) {
    ERROR("exceptions == NULL, check that you used init_exceptions"
	  "(exceptions * except) earlier."); 
    THROW(NULL_POINTER_OBJECT);
    // never reach this point return only to remove warning    
    return NULL;
  }

  if (NULL == Handler) {
    ERROR("Cannot add handler which address is NULL"); 
    THROW(NULL_POINTER_OBJECT);
    // never reach this point return only to remove warning
    return NULL;
  }
  e_NewException =
    new(exception_handler_o, (void *)Self, ExceptionName,
        Self->index_counter->inc(Self->index_counter),
        Handler);
  return e_NewException;  
}

int exception_stack_catch(exception_stack_osa * Self,
						  exception_handler_osa * Exception) {
  ASSERT(NULL == Self, NULL_POINTER_OBJECT, -1,
		 "You are trying to exception_stack_catch on stack at NULL address.");
  DEBUG(ENTRY, "Catched: %i.", exception_stack_catch, Self->current_exception);

  if (Self->current_exception != Exception->id) {
    return FALSE;
  }
  linked_list_osa * Stack = Self->stack;
  //delete(Self->pop(Self));/* pop element placed with throw */
  delete(Self->pop(Self));/* pop element placed with try */
  /* consult pointer to external function to return */
  if (Stack->len(Stack) > 0) {
    Self->stack_head = Stack->peak_l(Stack);
  }
  Self->current_exception = 0;
  return TRUE;
}

