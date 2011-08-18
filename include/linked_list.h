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

#ifndef LL_H
#define LL_H

/*
  ==============================================================
  EXPORT DEFINITIONS
  ==============================================================
*/
struct linked_element_osd; typedef struct linked_element_osd linked_element_osa; 
struct linked_list_osd; typedef struct linked_list_osd linked_list_osa; 
typedef void * (*map_callback)(void * Self);
extern const void * linked_list_o;
int linked_list_register_exceptions(void * Stack_v);

/*
  ==============================================================
  IMPORT 
  ==============================================================
*/

#include "exception.h"

/*
  ==============================================================
  GLOBAL VARIABLES
  ==============================================================
*/
extern exception_handler_osa * e_linked_list_empty_list_g;
extern exception_handler_osa * e_linked_list_out_of_range_g;

/*
  ==============================================================
  GLOBAL DEFINITIONS
  ==============================================================
*/

enum linked_list_object_types_e {
  OBJECT_linked_list_EE = OBJECT_LAST,
  OBJECT_linked_element_EE,
  OBJECT_LINKED_LIST_LAST_EE
};
// this is a hack that works only with gcc
#undef OBJECT_LAST
#define OBJECT_LAST OBJECT_LINKED_LIST_LAST_EE

/*
  ==============================================================
  IMPLEMENTATION
  ==============================================================
*/

struct linked_element_osd {
  const object_osa * type_info;
  object_properties_osa * properties;  
  struct linked_element_osd * right;
  struct linked_element_osd * left;
  void * element;
  int (* print) (linked_element_osa * Self, char * Buffer,
				 const struct printf_info * Info);
};

struct linked_list_osd {
  const object_osa * type_info;
  object_properties_osa * properties;  
//  const void * properties;
  linked_element_osa * right_head;
  linked_element_osa * left_head;
  int length;
  // methods
  int (* len)(linked_list_osa * Self);
  void * (* get_nth_r)(linked_list_osa * Self, int N);
  void * (* get_nth_l)(linked_list_osa * Self, int N);
  void * (* get_n_from_r)(linked_list_osa * Self, int N, int S);
  void * (* get_n_from_l)(linked_list_osa * Self, int N, int S);
  void * (* push_r)(linked_list_osa * Self, void * Object);
  void * (* push_l)(linked_list_osa * Self, void * Object);
  void * (* pop_r)(linked_list_osa * Self);
  void * (* pop_l)(linked_list_osa * Self);
  void * (* peak_r)(linked_list_osa * Self);
  void * (* peak_l)(linked_list_osa * Self);
  void * (* map_r)(linked_list_osa * Self, map_callback Function);
  void * (* map_l)(linked_list_osa * Self, map_callback Function);
  int (* print) (linked_list_osa * Self, char * Buffer,
				 const struct printf_info * Info);  
};

#endif
