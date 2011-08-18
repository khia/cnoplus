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

#ifndef LOGGER_H
#define LOGGER_H

#define NEWLINE "\r\n"

// debug levels
#define NO_DEBUG 0
#define ENTRY 1
#define HEAVY 2
#define MEMORY 4
#define POINTERS 8
#define LEAVE 16

#define LOG_DEVICE stderr 

//#define CHECK_FUN_NAME(Name)					
//#if Name != __func__
//#error "Wrong usage of DEBUG macro (wrong function name)."

// we cannot replace FunName with func as so we need address of that function
// for get_debug_level
#define DEBUG(Level, MsgAndFormat, FunName, Args...)				\
  do {if (get_debug_level(&FunName) && (Level)) {					\
	  fprintf(LOG_DEVICE, "%s:%i:%s: DEBUG: " MsgAndFormat NEWLINE,		\
			  __FILE__, __LINE__, # FunName, ## Args);				\
    }} while (0) 

#define DEBUG_NONL(Level, MsgAndFormat, FunName, Args...)		\
  do {if (get_debug_level(&FunName) && (Level)) {				\
      fprintf(LOG_DEVICE, "%s:%i:%s: DEBUG: " MsgAndFormat,			\
			  __FILE__, __LINE__, # FunName, ## Args);			\
    }} while (0)

#define ERROR(MsgAndFormat, Args...)								\
  do {fprintf(LOG_DEVICE, "%s:%i:%s: ERROR: " MsgAndFormat NEWLINE,	\
			  __FILE__, __LINE__, __func__, ## Args);				\
  } while (0)

#define ERROR_NONL(MsgAndFormat, Args...)						 \
  do {fprintf(LOG_DEVICE, "%s:%i:%s: ERROR: " MsgAndFormat,	     \
			  __FILE__, __LINE__, __func__, ## Args);			 \
  } while (0)

#define ASSERT(Condition, Exception, Return, MsgAndFormat, Args...)		\
  do {if ((Condition)) {												\
	fprintf(LOG_DEVICE, "%s:%i:%s: ERROR: " MsgAndFormat NEWLINE,		\
			__FILE__, __LINE__, __func__, ## Args);			            \
    THROW((Exception));													\
	/* we never reach this point */										\
	return (Return);													\
	}} while (0)

int get_debug_level(void * fun_addr);
#endif
