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

#include "common.h"

// We cannot use const as it gives warning:
// initialization discards qualifiers from pointer target type
// function return -1 if it's failed it's ok as so we use only positive values. 
long type_of(void * Self_v) {
  const struct object_osd ** Self = Self_v;
  if (NULL == *Self) {
	ERROR("Attempt to get_type of object at NULL address.");
	THROW(NULL_POINTER_OBJECT);
	// never reach this point return only to remove warning
	return OBJECT_ILLEGAL_EE;
  }
  return (*Self)->type;
}

char * name_of(void * Self_v) {  
  const struct object_osd ** Self = Self_v;
  if (NULL == *Self) {
	ERROR("Attempt to get_type of object at NULL address.");
	THROW(NULL_POINTER_OBJECT);
	// never reach this point return only to remove warning	
	return NULL;
  }
  return (*Self)->name;
}

/*
 * Assume that we have root object. 
 */ 
void * root(void * Self_v) {  
  const struct root_osd * Self = Self_v;
//  const struct object_properties_osd ** Self = Self_v + sizeof(object_osa);
  if (NULL == Self) {
	ERROR("Attempt to root of object at NULL address.");
	THROW(NULL_POINTER_OBJECT);
	// never reach this point return only to remove warning	
	return NULL;
  }
  return (void *)(Self->properties->root);
}

int is_object(void * Self_v) {  
  const struct object_osd * Self = Self_v;
  if (NULL == Self) {
	ERROR("Attempt to check object at NULL address.");
	THROW(NULL_POINTER_OBJECT);
	// never reach this point return only to remove warning	
	return FALSE;
  }
  DEBUG(HEAVY, "About to invoke is_object(%p) %p", is_object, Self_v, Self);  
  if (OBJECT_FLAG != Self->object_flag) {
	DEBUG(HEAVY, "is_object(%p) == FALSE", is_object, Self_v);
	return FALSE;
  }
  return TRUE;
}

/*
 * Assume that we have root object. 
 */ 
void * parent(void * Self_v) {
  const struct root_osd * Self = Self_v;
//  const struct object_properties_osd ** Self = Self_v + sizeof(object_osa);
  /*
  if (NULL == Self) {
	ERROR("Attempt to get parent of object at NULL address.");
	THROW(NULL_POINTER_OBJECT);
	// never reach this point return only to remove warning	
	return NULL;
  }
  */
  ASSERT(FALSE == is_object(Self_v), NULL_POINTER_OBJECT, NULL,
		 "Malformed object.");
  return (void *)(Self->properties->parent);
}

/*
 * Assume that we have root object. 
 */ 
object_properties_osa * properties_ctor(const void * Self_v,
										const void * Parent_v) {
  const struct root_osd * Parent = Parent_v;
  object_properties_osa * Buffer;
  if (NULL == (Buffer = malloc(sizeof(object_properties_osa)))){
	ERROR("Cannot allocate memory for object in amount of %i bytes.",
		  sizeof(object_properties_osa));
	THROW(NOT_ENOUGH_MEMORY_OBJECT);
	// never reach this point return only to remove warning
	return NULL;
  }

  if (NULL != Parent_v) {
	Buffer->root = Parent->properties->root;
	Buffer->parent = Parent;
  } else {
	// creating root object
	if (Root_g !=NULL) {
	  ERROR("Root object already exist.");
	  free(Buffer);
	  return NULL;
	}
	Root_g = Self_v;
	Buffer->root = Self_v;
	Buffer->parent = NULL;
  }
  return Buffer;
}

void * properties_dtor(const void * Self_v) {
  const struct root_osd * Self = Self_v;
  if (NULL != Self->properties) {
	free(Self->properties);	
  }
  return Self_v;
}

void * new (const void * Object_v, const void * Parent_v,...) {
  const struct object_osd * Object = Object_v;
  DEBUG(ENTRY, "About to new(%p, %p,...), object is %s( %ld ).", new,
		Object, Parent_v, Object->name, Object->type);

  object_properties_osa * Properties;
  
//  const struct object_osd * Parent = Parent_v;
  
  void * Buffer;
  if (NULL == (Buffer = malloc(Object->size))){
	ERROR("Cannot allocate memory for object in amount of %i bytes",
		  Object->size);
	THROW(NOT_ENOUGH_MEMORY_OBJECT);
	// never reach this point return only to remove warning
	return NULL;
  }
  DEBUG(HEAVY || MEMORY,
		"Memory for object \"%s\" was successfully allocated at address %p, "
		"in size %i bytes", new, Object->name, Buffer, Object->size);

  if (NULL == (Properties = properties_ctor(Buffer, Parent_v))) {
	ERROR("Cannot create object properties.");
	free(Buffer);
	THROW(NOT_ENOUGH_MEMORY_OBJECT);
	// never reach this point return only to remove warning
	return NULL;
  }
  
  * (const struct object_osd **) Buffer = Object;
  if (NULL != Object->ctor) {
	va_list Args;
	va_start(Args, Parent_v);
	Buffer = Object->ctor(Buffer, Properties, &Args);
	va_end(Args);
  }
  
//  pthread_rwlock_init(Object->lock, NULL);  
  return Buffer;
}

/*
 * Assume that we have root object. 
 */ 
void delete (void * Self_v) {
//  const struct root_osd * Self = Self_v;
  DEBUG(ENTRY, "About to %s(%p).", delete, "delete", Self_v);

//  const struct object_os ** C = Self;
    const struct object_osd ** Self = Self_v;
	if (NULL == Self) {
	  ERROR("Attempt to free memory from object at NULL address.");
	  THROW(NULL_POINTER_OBJECT);
	// never reach this point return only to remove warning
	  return;
	}

	if (NULL != (*Self)->dtor) {
	  Self = (*Self)->dtor(Self_v);
	  DEBUG(HEAVY, "Destructor was successfully invoked.", delete);
	}

	properties_dtor(Self_v);
//	if (NULL != (*Self)->lock) {
//	  pthread_rwlock_destroy(Self->lock);
//	}
	free(Self_v);
}

void * object_clone(void * Self_v) {
  DEBUG(ENTRY, "About to %s(%p).", clone, "clone", Self_v);  
  const struct object_osd ** Self = Self_v;
  void * Cloned;
  if (NULL == Self) {
	ERROR("Attempt to clone object at NULL address.");
	THROW(NULL_POINTER_OBJECT);
	// never reach this point return only to remove warning
	return NULL;
  }
  
  if (NULL != (*Self)->clone) {
	Cloned = (*Self)->clone(Self_v);
	return Cloned;
  }
  return NULL;
}

int object_print(FILE * Stream, const struct printf_info * Info, va_list * Args){
  DEBUG(ENTRY, "About to \"%s(%p, %p,...)\".", object_print, "object_print",
		Stream, Info);
  void * Object = va_arg(* Args, void *);
  struct object_osd ** Properties = Object;
  char * Buffer;
  int Len;

  if (NULL == (*Properties)->print) {
	ERROR("Print function is not specified ((*Properties)->print == NULL).");
	THROW(NULL_POINTER_OBJECT);
	// never reach this point return only to remove warning
	return -1;
  }
  DEBUG(HEAVY, "(*Properties)->print = %p, object_print = %p, Buffer=%p",
		object_print, (*Properties)->print, object_print, &Buffer);
  if (-1 == (Len = (*Properties)->print(Object, &Buffer, Info))) {
	THROW(NOT_ENOUGH_MEMORY_OBJECT);
	// never reach this point return only to remove warning
	return -1;	
  }

  /* Pad to the minimum field width and print to the stream. */
  Len = fprintf (Stream, "%*s",
				 (Info->left ? -Info->width : Info->width),
				 Buffer);
  
  /* Clean up and return. */
  free (Buffer);
  return Len;
}

int object_print_arginfo(const struct printf_info *info, size_t n,
                            int *argtypes) {
  /* We always take exactly one argument and this is a pointer to the
	 structure.. */
  if (n > 0)
	argtypes[0] = PA_POINTER;
  return 1;
}
 
void init_oop(void) {
  /* Register the print function for objects. */
  register_printf_function ('O', (printf_function *)object_print,
							object_print_arginfo);
}


