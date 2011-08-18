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

#ifndef EXCEPTION_H
#define EXCEPTION_H
#include <setjmp.h> // we need jmp_buf from here

/*
  ==============================================================
  EXPORT DEFINITIONS
  ==============================================================
*/

struct exception_handler_osd;
typedef struct exception_handler_osd exception_handler_osa;
struct exception_stack_osd;
typedef struct exception_stack_osd exception_stack_osa; 
struct exception_stack_node_osd; typedef struct exception_stack_node_osd
exception_stack_node_osa;
extern const void * exception_stack_o;

void default_exception_handler(int  Return_Code, char * Message, 
							   exception_stack_osa * Stack);


/*
  ==============================================================
  IMPORT 
  ==============================================================
*/

#include "linked_list.h"
#include "object.h"
#include "counter.h"


/*
  ==============================================================
  GLOBAL VARIABLES
  ==============================================================
*/

extern exception_handler_osa * e_DivisionOnZero_g;
extern exception_handler_osa * e_NotEnoughMemory_g;
extern exception_handler_osa * e_UncathedException_g;
extern exception_handler_osa * e_NullPointer_g;

#define ROOT_OBJECT Root_g
#define NULL_POINTER_OBJECT e_NullPointer_g
#define NOT_ENOUGH_MEMORY_OBJECT e_NotEnoughMemory_g

// can be thrown by exception_o object itself
extern exception_handler_osa * e_exception_cannot_push_g;
extern exception_handler_osa * e_exception_cannot_pop_g;
extern exception_handler_osa * e_exception_cannot_print_g;
/*
  ==============================================================
  GLOBAL DEFINITIONS
  ==============================================================
*/

enum exception_object_types_e {
  OBJECT_exception_handler_EE = OBJECT_LAST,
  OBJECT_exception_stack_node_EE,
  OBJECT_EXCEPTION_LAST_EE
};
// this is a hack that works only with gcc
#undef OBJECT_LAST
#define OBJECT_LAST OBJECT_EXCEPTION_LAST_EE

#define CHECK_ROOT(ROOT)												\
  if (NULL == (ROOT)) {													\
	ERROR("Root object unknown.");										\
	exit (-1);															\
  } else if (NULL == (ROOT)->exception_stack) {							\
	ERROR("Root->exception_stack object unknown.");						\
	exit (-1);															\
  } else 

/*
  } else if (NULL == ((ROOT)->exception_stack->stack_head)) {			\
	ERROR("Exception stack is empty THROW without TRY.");				\
	exit (-1);															\
  } else if (NULL == ((ROOT)->exception_stack->stack_head->jmpBuffer)) { \
	ERROR("THROW without TRY.");										\
	exit (-1);															\
*/


#define THROWS(Exceptions...)											\
  CHECK_ROOT(ROOT_OBJECT) {												\
	Stack->push(ROOT_OBJECT->exception_stack, (char *)&__FILE__,		\
				(char *)(int)__func__[], __LINE__);						\
  }

#define TRY                                                                   \
  CHECK_ROOT(ROOT_OBJECT) {												          \
	ROOT_OBJECT->exception_stack->stack_head =			  	\
	  ROOT_OBJECT->exception_stack->push(							    	\
			  ROOT_OBJECT->exception_stack,								          \
			  (char *)&__FILE__, (char *)(int)__func__,	(int)__LINE__);	\
  }																		\
  if (0 == setjmp((ROOT_OBJECT->exception_stack->stack_head->jmpBuffer))) 

#define THROW(Exception)                                                        \
  do {                                                                                        \
    CHECK_ROOT(ROOT_OBJECT) {                                           \
      if (ROOT_OBJECT->exception_stack->is_ready == FALSE) {   \
        ERROR("Exception stack is not initialized yet.");               \
        exit (-1);                                                                          \
      }                                                                                       \
      ROOT_OBJECT->exception_stack->current_exception =        \
        (Exception)->id;                                                                 \
      if (NULL == (ROOT_OBJECT->exception_stack->stack_head)) {  \
        ERROR("Exception stack is empty THROW without TRY.");     \
        exit (-1);                                                                          \
      }                                                                                        \
      longjmp(                                                                             \
              (ROOT_OBJECT->exception_stack->stack_head->jmpBuffer),    \
              (Exception)->id);                                                         \
    }                                                                                          \
  } while (0)

#define CATCH(Exception)                                    \
  else if (ROOT_OBJECT->exception_stack->catch(							\
				   ROOT_OBJECT->exception_stack, (Exception)))

#define ENDTRY															\
  else if (0 != ROOT_OBJECT->exception_stack->current_exception) {		\
	DEBUG(HEAVY, "Uncatched exception", __func__);						\
	char * Message_##__LINE__;											\
	asprintf(&Message_##__LINE__,										\
			 "Uncatched exception: %s:%i:%s", __FILE__, __LINE__, __func__); \
	e_UncathedException_g->handler(-1, Message_##__LINE__,				\
								 ROOT_OBJECT->exception_stack);			\
  } else {																\
	delete(ROOT_OBJECT->exception_stack->pop(ROOT_OBJECT->exception_stack)); \
	ROOT_OBJECT->exception_stack->stack_head =							\
	  ROOT_OBJECT->exception_stack->stack->peak_l(						\
			  ROOT_OBJECT->exception_stack->stack);						\
  }

#define RETURN(Value...)												\
  CHECK_ROOT(ROOT_OBJECT) {												\
	delete(ROOT_OBJECT->exception_stack->pop(ROOT_OBJECT->exception_stack)); \
	return Value;														\
  }

/*
  ==============================================================
  IMPLEMENTATION
  ==============================================================
*/

// TODO hide the implementation inside *.c source file
struct exception_handler_osd {
  const object_osa * type_info;
  object_properties_osa * properties;  
//  const void * properties;
  char * name;  
  long id;
  void (* handler) (int Cause, char * Message, exception_stack_osa * Stack);
};

extern const void * exception_handler_o;

struct exception_stack_node_osd {
  const object_osa * type_info;
  object_properties_osa * properties;  
//  const void * properties;
  char * module;
  char * function;
  int line;
  jmp_buf jmpBuffer;  
};

extern const void * exception_stack_node_o;

struct exception_stack_osd {
  const object_osa * type_info;
  object_properties_osa * properties;  
//  const void * properties;
  linked_list_osa * stack;
  linked_list_osa * exceptions;
  exception_stack_node_osa * stack_head;
  void * result;
  int current_exception;
  counter_osa * index_counter;
  int is_ready;

  jmp_buf jmpBuffer;  

  void * (* push) (exception_stack_osa * Self, char * Module,
				   char * Function, int Line);
  void * (* pop) (exception_stack_osa * Self);
  int (* print) (exception_stack_osa * Self, char * Buffer,
				 const struct printf_info * Info);
  exception_handler_osa * (* add_handler) (exception_stack_osa * Self, const char * Name, exception_handler_osa * H);
  int (* catch) (exception_stack_osa * Self,
				 exception_handler_osa * Exception);
};

#endif
