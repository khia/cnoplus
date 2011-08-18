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
#include "linked_list.h"

/*
  ==============================================================
  GLOBAL VARIABLES
  ==============================================================
*/

// can be thrown by linked_list_o object itself
exception_handler_osa * e_linked_list_empty_list_g = NULL;
exception_handler_osa * e_linked_list_out_of_range_g = NULL;
//exception_handler_osa * e_linked_list_cannot_print_element_g = NULL;

/*
  ==============================================================
  END OF GLOBAL VARIABLES
  ==============================================================
*/

// START of linked_element object definition

//static void * linked_element_print(linked_element_osa * Self, int N);

int linked_element_print(void * Self_v, char ** Buffer,
							   const struct printf_info * Info) {
  DEBUG(ENTRY, "About to start \"%s(%p, %p, %p)\" ctor.", linked_element_print,
		"linked_element_print", Self_v, Buffer, Info);
  struct linked_element_osd * Self = Self_v;
  ASSERT(NULL == Self, NULL_POINTER_OBJECT, -1,
		 "You are trying to print linked_element at address NULL.");
  ASSERT(NULL == Self->element, e_linked_list_empty_list_g, -1,
		 "Printed list is empty");
  const struct object_osd ** Properties = Self->element;
  int Res = (*Properties)->print(Self->element, Buffer, Info);
  DEBUG(LEAVE, "Done, the result of invokation is %i",
		linked_element_print, Res);
  return (Res);
}

static void * linked_element_ctor(void * Self_v, void * Properties,
								  va_list * Args) {
  DEBUG(ENTRY, "About to start \"%s(%p, %p,...)\" ctor.", linked_element_ctor,
		"linked_element_ctor", Self_v, Properties);
  struct linked_element_osd * Self = Self_v;
  const void * Object = va_arg(* Args, void *); 
  DEBUG(HEAVY, "Object = %p. Element = %p", linked_element_ctor, Object, Self_v);
  ASSERT(NULL == Self, NULL_POINTER_OBJECT, NULL,
		 "Constructor was invoked without buffer allocated.");
  Self->properties = Properties;
  Self->right = NULL;
  Self->left = NULL;
  Self->element = (void *)Object;
//  Self->print = linked_element_print;
  DEBUG(LEAVE, "Done, the result of invokation is %p",
		linked_element_ctor, Self);
  return Self;
}

static void * linked_element_dtor(void * Self_v) {  
  struct linked_element_osd * Self = Self_v;
  DEBUG(ENTRY, "About to start \"%s(%p)\" dtor.", linked_element_dtor,
		"linked_element_dtor", Self);
  ASSERT(NULL == Self, NULL_POINTER_OBJECT, NULL,
		 "Destructor was invoked with NULL argumant.");
  return Self;
}
OBJECT(linked_element, NULL, NULL, linked_element_print);
const void * linked_element_o = &linked_element_od;

// END of linked_element object definition

// START of linked_list object definition

void e_linked_list_empty_list_handler(
		int Cause, char * Message, exception_stack_osa * Stack) {
  DEBUG(ENTRY, "About to %s(%i,%s,%p).", e_linked_list_empty_list_handler,
		"e_linked_list_empty_list_handler", Cause, Message, Stack);
  // here we can redifine message or stack
  // and clean our memory 
  // -1 reason for program termination
  default_exception_handler(-1, Message, Stack); 
}
void e_linked_list_out_of_range_handler(
		int Cause, char * Message, exception_stack_osa * Stack) {
  DEBUG(ENTRY, "About to %s(%i,%s,%p).", e_linked_list_out_of_range_handler,
		"e_linked_list_out_of_range_handler", Cause, Message, Stack);
  // here we can redifine message or stack
  // and clean our memory 
  // -1 reason for program termination
  default_exception_handler(-1, Message, Stack); 
}
void e_linked_list_cannot_print_element_handler(
		int Cause, char * Message, exception_stack_osa * Stack) {
  DEBUG(ENTRY, "About to %s(%i,%s,%p).",
		e_linked_list_cannot_print_element_handler,
		"e_linked_list_cannot_print_element_handler", Cause, Message, Stack);
  // here we can redifine message or stack
  // and clean our memory 
  // -1 reason for program termination
  default_exception_handler(-1, Message, Stack); 
}


int linked_list_register_exceptions(void * Stack_v) {
  struct exception_stack_osd * Stack = Stack_v;
  counter_osa * Counter = Stack->index_counter;
  e_linked_list_empty_list_g =
	new(exception_handler_o, Stack,	"e_linked_list_empty_list_g",
		Counter->inc(Counter), &e_linked_list_empty_list_handler); 
  ASSERT(NULL == e_linked_list_empty_list_g, NULL_POINTER_OBJECT, NOK,
		 "Cannot create exception handler %s.", "e_linked_list_empty_list_g");
  e_linked_list_out_of_range_g =
	new(exception_handler_o, Stack,	"e_linked_list_out_of_range_g",
		Counter->inc(Counter), &e_linked_list_out_of_range_handler);
  ASSERT(NULL == e_linked_list_out_of_range_g, NULL_POINTER_OBJECT, NOK,
		 "Cannot create exception handler %s.", "e_linked_list_out_of_range_g");
  return OK;
}

static int linked_list_len(linked_list_osa * Self);
static void * linked_list_get_nth_r(linked_list_osa * Self, int N);
static void * linked_list_get_nth_l(linked_list_osa * Self, int N);
static void * linked_list_get_n_from_r(linked_list_osa * Self, int N, int S);
static void * linked_list_get_n_from_l(linked_list_osa * Self, int N, int S);
static void * linked_list_push_r(linked_list_osa * Self, void * Object);
static void * linked_list_push_l(linked_list_osa * Self, void * Object);
static void * linked_list_pop_r(linked_list_osa * Self);
static void * linked_list_pop_l(linked_list_osa * Self);
static void * linked_list_peak_r(linked_list_osa * Self);
static void * linked_list_peak_l(linked_list_osa * Self);
static void * linked_list_map_r(linked_list_osa * Self, map_callback Function);
static void * linked_list_map_l(linked_list_osa * Self, map_callback Function);
static int linked_list_print(void * Self_v, char ** Buffer,
							 const struct printf_info * Info);

static void * linked_list_ctor(void * Self_v, void * Properties,
							   va_list * _Args) {
  struct linked_list_osd * Self = Self_v;
  DEBUG(ENTRY, "About to start \"%s(%p, %p,...)\" ctor.", linked_list_ctor, 
		"linked_list_ctor", Self, Properties);
  ASSERT(NULL == Self, NULL_POINTER_OBJECT, NULL,
		 "Constructor was invoked without buffer allocated.");
  DEBUG(HEAVY || MEMORY, "Self = %p.", linked_list_ctor, Self);

  Self->properties = Properties;
  Self->right_head = NULL;
  Self->left_head = NULL;
  Self->length = 0;

  Self->len = linked_list_len;
  Self->get_nth_r = linked_list_get_nth_r;
  Self->get_nth_l = linked_list_get_nth_l;
  Self->get_n_from_r = linked_list_get_n_from_r;
  Self->get_n_from_l = linked_list_get_n_from_l;
  Self->push_r = linked_list_push_r;
  Self->push_l = linked_list_push_l;
  Self->pop_r = linked_list_pop_r;
  Self->pop_l = linked_list_pop_l;
  Self->peak_r = linked_list_peak_r;
  Self->peak_l = linked_list_peak_l;
  Self->map_r = linked_list_map_r;
  Self->map_l = linked_list_map_l;
//  Self->print = linked_list_print;
  DEBUG(HEAVY || MEMORY, "Self->push_r = %p, fun = %p.", linked_list_ctor,
		Self->push_r, &linked_list_push_r);
  
  DEBUG(LEAVE, "The function %s was succesfully invoked.", linked_list_ctor,
		"linked_list_push_r");
  return Self;
}

static void * delete_elements(void * Object_v) {
  struct root_osd * Object = Object_v;
  delete(Object);
  return NULL;
}

static void * linked_list_dtor(void * Self_v) {
  struct linked_list_osd * Self = Self_v;
  DEBUG(ENTRY, "About to start \"%s(%p)\" dtor.", linked_list_dtor,
		"linked_list_dtor", Self);
  ASSERT(NULL == Self, NULL_POINTER_OBJECT, NULL,
		 "Destructor was invoked with NULL argumant.");
  Self->map_r(Self, delete_elements);
  return Self;
}

int linked_list_print(void * Self_v, char ** Buffer,
							   const struct printf_info * Info) {
  struct linked_list_osd * Self = Self_v;
  DEBUG(ENTRY, "About to start \"%s(%p, %p, %p)\".", linked_list_print,
		"linked_list_print", Self_v, Buffer, Info);
  ASSERT(NULL == Self, NULL_POINTER_OBJECT, -1,
		 "You are trying to print linked_list at address NULL.");
  linked_element_osa * Element = Self->right_head;
  ASSERT(NULL == Element, e_linked_list_empty_list_g, -1,
		 "You are trying to print empty list.");
  int Len = 0;
  int Buffer_Size = 0;
  int Size = 0;
  char * Prefix = "[";
  int Prefix_Length = strlen(Prefix);
  char * Suffix = "]";
  int Suffix_Length = strlen(Suffix);
  char * Delimiter = ","NEWLINE;
  int Delimiter_Length = strlen(Delimiter);
  char * Element_Buffer;
  const struct object_osd ** Properties;
  char * Output = malloc(Prefix_Length);
  ASSERT(NULL == Output, NOT_ENOUGH_MEMORY_OBJECT, -1,
		 "Cannot allocate memory we need %i bytes.", Size);
  memcpy(Output + Buffer_Size, Prefix, Prefix_Length);
  Buffer_Size = Buffer_Size + Prefix_Length;
	
  do {
	Properties = Element->element;
	ASSERT(NULL == Properties, NULL_POINTER_OBJECT, -1,
		   "Element->element has NULL address");
	if (0 > (Len = (*Properties)->print(Element->element,
										&Element_Buffer, Info))) {
	  free(Output);
	  ERROR("Cannot invoke object's print function");
	  return -1;
	}
	Size = Buffer_Size + Len + Delimiter_Length;
	Output = realloc(Output, Size);
	ASSERT(NULL == Output, NOT_ENOUGH_MEMORY_OBJECT, -1,
		   "Cannot reallocate memory we need %i bytes extra.", Size);
	memcpy(Output + Buffer_Size, Element_Buffer, Len);
	Buffer_Size = Buffer_Size + Len;
	// add delimiter after each element except last one 
	if (NULL != Element->left) {
	  memcpy(Output + Buffer_Size, Delimiter, Delimiter_Length);
	  Buffer_Size = Buffer_Size + Delimiter_Length;
	}
	free(Element_Buffer);
  } while (NULL != (Element = Element->left));

  Size = Size + Suffix_Length + 1;
  Output = realloc(Output, Size);
  ASSERT(NULL == Output, NOT_ENOUGH_MEMORY_OBJECT, -1,
		   "Cannot reallocate memory we need %i bytes extra.", Size);
  memcpy(Output + Buffer_Size, Suffix, Suffix_Length);
  // write terminator
  Output[Buffer_Size + Suffix_Length] = 0;

  *Buffer = Output;
  DEBUG(HEAVY, "Res = %s", linked_list_print, Output);
  return Buffer_Size;
}

OBJECT(linked_list, NULL, NULL, linked_list_print);
const void * linked_list_o = &linked_list_od;

// methods
static int linked_list_len(linked_list_osa * Self) {
  DEBUG(ENTRY, "About to start \"%s(%p)\".", linked_list_len,
		"linked_list_len", Self);
  ASSERT(NULL == Self, NULL_POINTER_OBJECT, NULL,
		 "You are trying to get element from linked_list that situated "
		 "at address NULL.");
  return Self->length;
}


static void * linked_list_get_nth_r(linked_list_osa * Self, const int N) {
  DEBUG(ENTRY, "About to start \"%s(%p, %i)\".", linked_list_get_nth_r,
		"linked_list_get_nth_r", Self, N);
  ASSERT(NULL == Self, NULL_POINTER_OBJECT, NULL,
		 "You are trying to get element from linked_list that situated "
		 "at address NULL.");
  linked_element_osa * Element = Self->right_head;
  ASSERT(NULL == Element, e_linked_list_empty_list_g, NULL,
		 "You are trying to get element from empty list.");
  ASSERT(N > Self->length, e_linked_list_out_of_range_g, NULL,
		 "Requested element is more then total number of elements in a list");

  int index = 0;
  do {
	if (N == index) {
	  return Element->element;
	}
	index = index + 1;
  } while (NULL != (Element = Element->left));
  // N > then the number of elements in list
  return NULL;
}

static void * linked_list_get_nth_l(linked_list_osa * Self, int N) {
  DEBUG(ENTRY, "About to start \"%s(%p, %i)\".", linked_list_get_nth_l,
		"linked_list_get_nth_l", Self, N);
  ASSERT(NULL == Self, NULL_POINTER_OBJECT, NULL,
		 "You are trying to get element from linked_list that situated "
		 "at address NULL.");
  linked_element_osa * Element = Self->left_head;
  ASSERT(NULL == Element, e_linked_list_empty_list_g, NULL,
		 "You are trying to get element from empty list.");
  ASSERT(N > Self->length, e_linked_list_out_of_range_g, NULL,
		 "Requested element is more then total number of elements in a list");

  int index = 0;
  do {
	if (N == index) {
	  return Element->element;
	}
	index = index + 1;
  } while (NULL != (Element = Element->right));
  // N > then the number of elements in list
  return NULL;
}
/*
 * Returns: NULL if number of elements in source linked list is less then S.
 * Throws an exception NULL_POINTER_OBJECT in case when:
 *    Self == NULL or Self->left_head == NULL
 */ 
static void * linked_list_get_n_from_r(linked_list_osa * Self, int N, int S) {
  DEBUG(ENTRY, "About to start \"%s(%p, %i, %i)\".", linked_list_get_n_from_r,
		"linked_list_get_n_from_r", Self, N, S);
  ASSERT(NULL == Self, NULL_POINTER_OBJECT, NULL,
		 "You are trying to get element from linked_list that situated "
		 "at address NULL.");

  linked_element_osa * Element = Self->left_head;
  ASSERT(NULL == Element, e_linked_list_empty_list_g, NULL,
		 "You are trying to get element from empty list.");

  ASSERT(N + S > Self->length, e_linked_list_out_of_range_g, NULL,
		 "Requested element is more then total number of elements in a list");

  int index = 0;
  // skip S elements from right side
  do {
	index = index + 1;
  } while ((NULL != (Element = Element->right)) && (index < S));

  // Returns: NULL if number of elements in source linked list is less then S.
  if (NULL == Element) {
	return NULL;
  }
  linked_list_osa * Result = new(linked_list_o, parent(Self));
  ASSERT(NULL == Result, NOT_ENOUGH_MEMORY_OBJECT, NULL,
		 "Cannot create stack for exceptions.");
  index = 0;
  do {	
	// to avoid reversed result
	linked_list_push_l(Result, Element->element);
	index = index + 1;
  } while ((NULL != (Element = Element->right)) && (index < N));
  return Result;
}
static void * linked_list_get_n_from_l(linked_list_osa * Self, int N, int S) {
  DEBUG(ENTRY, "About to start \"%s(%p, %i, %i)\".", linked_list_get_n_from_l,
		"linked_list_get_n_from_l", Self, N, S);
  ASSERT(NULL == Self, NULL_POINTER_OBJECT, NULL,
		 "You are trying to get element from linked_list that situated "
		 "at address NULL.");
  linked_element_osa * Element = Self->right_head;
  ASSERT(NULL == Element, e_linked_list_empty_list_g, NULL,
		 "You are trying to get element from empty list.");
  
  ASSERT(N + S > Self->length, e_linked_list_out_of_range_g, NULL,
		 "Requested element is more then total number of elements in a list");

  int index = 0;
  // skip S elements from right side
  do {
	index = index + 1;
  } while ((NULL != (Element = Element->left)) && (index < S));

  // Returns: NULL if number of elements in source linked list is less then S.
  if (NULL == Element) {
	  return NULL;
  }
  linked_list_osa * Result = new(linked_list_o, parent(Self));
  ASSERT(NULL == Result, NOT_ENOUGH_MEMORY_OBJECT, NULL,
		 "Cannot create stack for exceptions.");
  index = 0;
  do {	
	// to avoid reversed result
	linked_list_push_r(Result, Element->element);
	index = index + 1;
  } while ((NULL != (Element = Element->left)) && (index < N));
  return Result;
}
static void * linked_list_push_r(linked_list_osa * Self, void * Object) {
  DEBUG(ENTRY, "About to %s(%p, %p).", linked_list_push_r, "linked_list_push_r",
	Self, Object);
  ASSERT(NULL == Self, NULL_POINTER_OBJECT, NULL,
		 "You are trying to add new element from linked_list that situated "
		 "at address NULL.");
  linked_element_osa * Element = new(linked_element_o, Self, Object);
  ASSERT(NULL == Element, NOT_ENOUGH_MEMORY_OBJECT, NULL,
		 "Cannot create new element.");
  
  linked_element_osa * Right_Head;
  if (NULL == (Right_Head = Self->right_head)) {
	// we have empty list
	Self->right_head = Element;
	Self->left_head = Element;
	Self->length = Self->length + 1;
	return Self;
  }
  Self->right_head = Element;
  Right_Head->right = Element;  
  Element->left = Right_Head;
  Self->length = Self->length + 1;    
  return Self;
}
static void * linked_list_push_l(linked_list_osa * Self, void * Object) {
  DEBUG(ENTRY, "About to %s(%p, %p).", linked_list_push_l, "linked_list_push_l",
	Self, Object); 
  ASSERT(NULL == Self, NULL_POINTER_OBJECT, NULL,
		 "You are trying to add new element from linked_list that situated "
		 "at address NULL.");
  linked_element_osa * Element = new(linked_element_o, Self, Object);
  ASSERT(NULL == Element, NOT_ENOUGH_MEMORY_OBJECT, NULL,
		 "Cannot create new element.");
  linked_element_osa * Left_Head;
  if (NULL == (Left_Head = Self->left_head)) {
	// we have empty list
	Self->right_head = Element;
	Self->left_head = Element;
	Self->length = Self->length + 1;
	return Self;
  }
  Self->left_head = Element;
  Left_Head->left = Element;  
  Element->right = Left_Head;
  Self->length = Self->length + 1;    
  return Self;
}
static void * linked_list_pop_r(linked_list_osa * Self) {
  DEBUG(ENTRY, "About to \"%s(%p)\".", linked_list_pop_r,
		"linked_list_pop_r", Self);
  ASSERT(NULL == Self, NULL_POINTER_OBJECT, NULL,
		 "You are trying to pop element from linked_list that situated "
		 "at address NULL.");
  linked_element_osa * Element = Self->right_head;
  ASSERT(NULL == Element, e_linked_list_empty_list_g, Self,
		 "No more elements in linked_list to pop");
  Self->right_head = Element->left;  
  Self->length = Self->length - 1;    
  void * Object = Element->element;
  delete(Element);
  return Object;
}
static void * linked_list_pop_l(linked_list_osa * Self) {
  DEBUG(ENTRY, "About to \"%s(%p)\".", linked_list_pop_l,
		"linked_list_pop_l", Self);
  ASSERT(NULL == Self, NULL_POINTER_OBJECT, NULL,
		 "You are trying to pop element from linked_list that situated "
		 "at address NULL.");
  linked_element_osa * Element = Self->left_head;
  ASSERT(NULL == Element, e_linked_list_empty_list_g, Self,
		 "No more elements in linked_list to pop");
  Self->left_head = Element->right;
  Self->length = Self->length - 1;      
  void * Object = Element->element;
  delete(Element);
  return Object;
}
static void * linked_list_peak_r(linked_list_osa * Self) {
  DEBUG(ENTRY, "About to \"%s(%p)\".", linked_list_peak_r,
		"linked_list_peak_r", Self);
  ASSERT(NULL == Self, NULL_POINTER_OBJECT, NULL,
		 "You are trying to peak element from linked_list that situated "
		 "at address NULL.");
  linked_element_osa * Element = Self->right_head;
  ASSERT(NULL == Element, e_linked_list_empty_list_g, Self,
		 "No more elements in linked_list to peak");
  void * Object = Element->element;
  return Object;
}
static void * linked_list_peak_l(linked_list_osa * Self) {
  DEBUG(ENTRY, "About to \"%s(%p)\".", linked_list_peak_l,
		"linked_list_peak_l", Self);
  ASSERT(NULL == Self, NULL_POINTER_OBJECT, NULL,
		 "You are trying to peak element from linked_list that situated "
		 "at address NULL.");
  linked_element_osa * Element = Self->left_head;
  ASSERT(NULL == Element, e_linked_list_empty_list_g, Self,
		 "No more elements in linked_list to peak");
  void * Object = Element->element;
  return Object;
}
static void * linked_list_map_r(linked_list_osa * Self, map_callback Fun) {
  DEBUG(ENTRY, "About to start \"%s(%p, %p)\".", linked_list_map_r,
		"linked_list_map_r", Self, Fun);
  ASSERT(NULL == Self, NULL_POINTER_OBJECT, NULL,
		 "You are trying to map linked_list that situated at address NULL.");
  linked_element_osa * Element = Self->right_head;
  ASSERT(NULL == Element, e_linked_list_empty_list_g, Self,
		 "The linked_list is empty");
  do {
	Element->element =((map_callback *)Fun(Element->element));
  } while (NULL != (Element = Element->left));  
  return Self;
}
static void * linked_list_map_l(linked_list_osa * Self, map_callback Fun) {
  DEBUG(ENTRY, "About to start \"%s(%p, %p)\".", linked_list_map_l,
		"linked_list_map_l", Self, Fun);
  ASSERT(NULL == Self, NULL_POINTER_OBJECT, NULL,
		 "You are trying to map linked_list that situated at address NULL.");
  linked_element_osa * Element = Self->left_head;
  ASSERT(NULL == Element, e_linked_list_empty_list_g, Self,
		 "The linked_list is empty");	
  do {
	  Element->element = ((map_callback *)Fun(Element->element));
  } while (NULL != (Element = Element->right));
  return Self;
}

// END of linked_list object definition

// test functions
