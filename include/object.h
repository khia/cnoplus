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

#ifndef OBJECT_H
#define OBJECT_H
#include <printf.h> // we need printf_info definition from here

/*
 * clone function has namespace clash with /usr/include/bits/sched.h
 * so we redefine all our own using of clone with clone_object
 */
#define clone object_clone

/*
  Predifined objects types
*/
enum object_types_e {
  OBJECT_ILLEGAL_EE = -1,
  OBJECT_root_EE = 0,
  OBJECT_exception_stack_EE = 1,
  OBJECT_counter_EE = 2,
  OBJECT_LAST
};

//#define OBJECT_INDEX 3 //__COUNTER__ + OBJECT_LAST

/*
#ifndef OBJECT_INDEX
# define OBJECT_INDEX OBJECT_LAST
#else
# define OBJECT_INDEX_T OBJECT_INDEX
# define OBJECT_INDEX OBJECT_INDEX_T + 1
# undefine OBJECT_INDEX_T
#endif
*/
// we cannot use typedef because: typedef does not define macros.
// const may be bound in unexpected ways in the context of a typedef
struct object_osd; typedef struct object_osd object_osa; 
struct object_properties_osd;
typedef struct object_properties_osd object_properties_osa;

struct object_osd { // object structure definition
  int object_flag;
  size_t size;  
  long type; // for get_type(ptr) function.
  char * name;
  void * (* ctor) (void * Self, void * Properties, va_list * Args);
  void * (* dtor) (void * Self);
  void * (* clone) (void * Self);
  int (* differ) (void * Self, const void * B);
  int (* print) (void * Self, char ** Buffer,
				 const struct printf_info * Info);
};

#define OBJECT_FLAG 679674
/*
  next macro using naming conventions:
  1. coonsctuctors has name suffix _ctor
  2. destructors has name suffix _dtor
  3. type enumeration has prefix OBJECT_ and suffix _EE
  4. structure that describes the object has suffix _osd
  5. result object variabe has suffix _od
  When expanded this macro produces following structure
  static const struct object_osd BaseName_od = {
      679674,
      sizeof(struct BaseName_osd),
      OBJECT_BaseName_EE,
      "BaseName_osd",
      BaseName_ctor,
      BaseName_dtor,
      Args
  };
*/
//#define OBJECT(BaseName, Clone, Differ, Print)					
#define OBJECT(BaseName, Args...)                           \
  static const struct object_osd BaseName ## _od = {	 \
    OBJECT_FLAG,                                                    \
    sizeof(struct BaseName ## _osd),                           \
    OBJECT_ ## BaseName ## _EE,                             \
    #BaseName"_osd",                                                \
    BaseName ## _ctor,                                               \
    BaseName ## _dtor,                                               \
    Args                                                                     \
  };																
//  const void * BaseName ## _o = & ## BaseName ## _od

struct object_properties_osd {
  void * root;
  //  pthread_mutex_t mutex; 
  pthread_rwlock_t * lock;
  void * parent; // pointer to parrent
  int reference; // number of references
};

void * new(const void * Object_v, const void * Parent,...);
void delete(void * Self_v);
void init_oop(void);
long type_of(void * Self);
char * name_of(void * Self);
void * root(void * Self);
void * parent(void * Self);
void * clone(void * Self);
int is_object(void * Self_v);
#define REFERENCE(Object)						\
  (((root_osa *)(Object))->properties->reference)

//#define clone(Object) object_clone(Object)
#endif
