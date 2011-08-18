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

#ifndef COUNTER_H
#define COUNTER_H
//#include "common.h"

#include "object.h"

struct counter_osd; typedef struct counter_osd counter_osa; 

struct counter_osd {
  const object_osa * type_info;
  object_properties_osa * properties;
  int value;
  // methods 
  int (* get)(counter_osa * Self);
  int (* inc)(counter_osa * Self);
  int (* set)(counter_osa * Self, int N);
};// counter_osa;

extern const void * counter_o;

#endif
