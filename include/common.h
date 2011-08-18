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

#ifndef COMMON_H
#define COMMON_H

#include "object.h"
#include "counter.h"
#include "exception.h"
#include "linked_list.h"
#include "logger.h"
#include "root.h"

#define TRUE 1
#define FALSE 0

#define OK 1
#define NOK 0

typedef unsigned char byte;

#define CURRENT_FUNCTION __PRETTY_FUNCTION__
#ifdef __FUNCTION__
#define CURRENT_FUNCTION __FUNCTION__
#endif

#if __STDC_VERSION__ < 199901L
# if __GNUC__ >= 2
#  define __func__ __FUNCTION__
# else
#  define __func__ "<unknown>"
# endif
#endif

#define REGISTER_EXCEPTIONS(Stack) \
  linked_list_register_exceptions(Stack);    \

/*
  ==============================================================
  GLOBAL VARIABLES
  ==============================================================
*/

extern root_osa * Root_g;

/*
  ==============================================================
  END OF GLOBAL VARIABLES
  ==============================================================
*/

#endif
