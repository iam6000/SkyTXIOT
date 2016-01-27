/* !
*	This file support logSystem for IOT SDK
*	@add by 6K at 2015/12/17 
*	@Cutting out from PJSIP log system 
*/

/*$Id: log.h 4584 2013-08-30 04:03:22Z bennylp $ */
/* 
 * Copyright (C) 2008-2011 Teluu Inc. (http://www.teluu.com)
 * Copyright (C) 2003-2008 Benny Prijono <benny@prijono.org>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA 
 */
 
#ifndef _SKY_LOG_H
#define _SKY_LOG_H


#include <stdarg.h>

#define SKY_LOG_MAX_LEVEL  6
#define SKY_HAS_THREADS 0
#define SKY_LOG_ENABLE_INDENT        1
#define SKY_LOG_INDENT_CHAR	    '.'
#define SKY_UNUSED_ARG(arg)  (void)arg
#define SKY_TERM_HAS_COLOR 1 


/**
 *
 *
 * 	To write to the log, one uses construct like below:
 *    SKY_LOG(3, ("main.c", "Starting hello..."));
 *
 */

/**
 * Color code combination.
 */
enum {
    SKY_TERM_COLOR_R	= 2,    /**< Red            */
    SKY_TERM_COLOR_G	= 4,    /**< Green          */
    SKY_TERM_COLOR_B	= 1,    /**< Blue.          */
    SKY_TERM_COLOR_BRIGHT = 8    /**< Bright mask.   */
};

/**
 * Representation of time value in this library.
 * This type can be used to represent either an interval or a specific time
 * or date. 
 */
typedef struct sky_time_val
{
    /** The seconds part of the time. */
    long    sec;

    /** The miliseconds fraction of the time. */
    long    msec;

} sky_time_val;

/**
 * This structure represent the parsed representation of time.
 * It is acquired by calling #sky_time_decode().
 */
typedef struct sky_parsed_time
{
    /** This represents day of week where value zero means Sunday */
    int wday;

    /* This represents day of the year, 0-365, where zero means
     *  1st of January.
     */
    /*int yday; */

    /** This represents day of month: 1-31 */
    int day;

    /** This represents month, with the value is 0 - 11 (zero is January) */
    int mon;

    /** This represent the actual year (unlike in ANSI libc where
     *  the value must be added by 1900).
     */
    int year;

    /** This represents the second part, with the value is 0-59 */
    int sec;

    /** This represents the minute part, with the value is: 0-59 */
    int min;

    /** This represents the hour part, with the value is 0-23 */
    int hour;

    /** This represents the milisecond part, with the value is 0-999 */
    int msec;

} sky_parsed_time;
 

/**
 * Log decoration flag, to be specified with #sky_log_set_decor().
 */
enum sky_log_decoration
{
    SKY_LOG_HAS_DAY_NAME   =    1, /**< Include day name [default: no] 	      */
    SKY_LOG_HAS_YEAR       =    2, /**< Include year digit [no]		      */
    SKY_LOG_HAS_MONTH	  =    4, /**< Include month [no]		      */
    SKY_LOG_HAS_DAY_OF_MON =    8, /**< Include day of month [no]	      */
    SKY_LOG_HAS_TIME	  =   16, /**< Include time [yes]		      */
    SKY_LOG_HAS_MICRO_SEC  =   32, /**< Include microseconds [yes]             */
    SKY_LOG_HAS_SENDER	  =   64, /**< Include sender in the log [yes] 	      */
    SKY_LOG_HAS_NEWLINE	  =  128, /**< Terminate each call with newline [yes] */
    SKY_LOG_HAS_CR	  =  256, /**< Include carriage return [no] 	      */
    SKY_LOG_HAS_SPACE	  =  512, /**< Include two spaces before log [yes]    */
    SKY_LOG_HAS_COLOR	  = 1024, /**< Colorize logs [yes on win32]	      */
    SKY_LOG_HAS_LEVEL_TEXT = 2048, /**< Include level text string [no]	      */
    SKY_LOG_HAS_THREAD_ID  = 4096, /**< Include thread identification [no]     */
    SKY_LOG_HAS_THREAD_SWC = 8192, /**< Add mark when thread has switched [yes]*/
    SKY_LOG_HAS_INDENT     =16384  /**< Indentation. Say yes! [yes]            */  
};

/**
 * Write log message.
 * This is the main macro used to write text to the logging backend. 
 *
 * @param level	    The logging verbosity level. Lower number indicates higher
 *		    importance, with level zero indicates fatal error. Only
 *		    numeral argument is permitted (e.g. not variable).
 * @param arg	    Enclosed 'printf' like arguments, with the first 
 *		    argument is the sender, the second argument is format 
 *		    string and the following arguments are variable number of 
 *		    arguments suitable for the format string.
 *
 * Sample:
 * \verbatim
   SKY_LOG(2, (__FILE__, "current value is %d", value));
   \endverbatim
 * @hideinitializer
 */
#define SKY_LOG(level,arg)	do { \
				    if (level <= sky_log_get_level()) \
					sky_log_wrapper_##level(arg); \
				} while (0)

/**
 * Signature for function to be registered to the logging subsystem to
 * write the actual log message to some output device.
 *
 * @param level	    Log level.
 * @param data	    Log message, which will be NULL terminated.
 * @param len	    Message length.
 */
typedef void sky_log_func(int level, const char *data, int len);

/**
 * Default logging writer function used by front end logger function.
 * This function will print the log message to stdout only.
 * Application normally should NOT need to call this function, but
 * rather use the SKY_LOG macro.
 *
 * @param level	    Log level.
 * @param buffer    Log message.
 * @param len	    Message length.
 */
void sky_log_write(int level, const char *buffer, int len);


#if SKY_LOG_MAX_LEVEL >= 1

/**
 * Write to log.
 *
 * @param sender    Source of the message.
 * @param level	    Verbosity level.
 * @param format    Format.
 * @param marker    Marker.
 */
void sky_log(const char *sender, int level, 
		     const char *format, va_list marker);

/**
 * Change log output function. The front-end logging functions will call
 * this function to write the actual message to the desired device. 
 * By default, the front-end functions use sky_log_write() to write
 * the messages, unless it's changed by calling this function.
 *
 * @param func	    The function that will be called to write the log
 *		    messages to the desired device.
 */
void sky_log_set_log_func( sky_log_func *func );

/**
 * Get the current log output function that is used to write log messages.
 *
 * @return	    Current log output function.
 */
sky_log_func*  sky_log_get_log_func(void);

/**
 * Set maximum log level. Application can call this function to set 
 * the desired level of verbosity of the logging messages. The bigger the
 * value, the more verbose the logging messages will be printed. However,
 * the maximum level of verbosity can not exceed compile time value of
 * sky_LOG_MAX_LEVEL.
 *
 * @param level	    The maximum level of verbosity of the logging
 *		    messages (6=very detailed..1=error only, 0=disabled)
 */
void sky_log_set_level(int level);

/**
 * Get current maximum log verbositylevel.
 *
 * @return	    Current log maximum level.
 */

int sky_log_get_level(void);


/**
 * Set log decoration. The log decoration flag controls what are printed
 * to output device alongside the actual message. For example, application
 * can specify that date/time information should be displayed with each
 * log message.
 *
 * @param decor	    Bitmask combination of #sky_log_decoration to control
 *		    the layout of the log message.
 */
void sky_log_set_decor(unsigned decor);

/**
 * Get current log decoration flag.
 *
 * @return	    Log decoration flag.
 */
unsigned sky_log_get_decor(void);

/**
 * Add indentation to log message. Indentation will add SKY_LOG_INDENT_CHAR
 * before the message, and is useful to show the depth of function calls.
 *
 * @param indent    The indentation to add or substract. Positive value
 * 		    adds current indent, negative value subtracts current
 * 		    indent.
 */
void sky_log_add_indent(int indent);

/**
 * Push indentation to the right by default value (SKY_LOG_INDENT).
 */
void sky_log_push_indent(void);

/**
 * Pop indentation (to the left) by default value (SKY_LOG_INDENT).
 */
void sky_log_pop_indent(void);

/**
 * Set color of log messages.
 *
 * @param level	    Log level which color will be changed.
 * @param color	    Desired color.
 */
void sky_log_set_color(int level, int color);


/**
 * Internal function to be called by sky_init()
 */
int sky_log_init(void);

#else	/* #if SKY_LOG_MAX_LEVEL >= 1 */

/**
 * Change log output function. The front-end logging functions will call
 * this function to write the actual message to the desired device. 
 * By default, the front-end functions use sky_log_write() to write
 * the messages, unless it's changed by calling this function.
 *
 * @param func	    The function that will be called to write the log
 *		    messages to the desired device.
 */
#  define sky_log_set_log_func(func)

/**
 * Write to log.
 *
 * @param sender    Source of the message.
 * @param level	    Verbosity level.
 * @param format    Format.
 * @param marker    Marker.
 */
#  define sky_log(sender, level, format, marker)

/**
 * Set maximum log level. Application can call this function to set 
 * the desired level of verbosity of the logging messages. The bigger the
 * value, the more verbose the logging messages will be printed. However,
 * the maximum level of verbosity can not exceed compile time value of
 * SKY_LOG_MAX_LEVEL.
 *
 * @param level	    The maximum level of verbosity of the logging
 *		    messages (6=very detailed..1=error only, 0=disabled)
 */
#  define sky_log_set_level(level)

/**
 * Set log decoration. The log decoration flag controls what are printed
 * to output device alongside the actual message. For example, application
 * can specify that date/time information should be displayed with each
 * log message.
 *
 * @param decor	    Bitmask combination of #sky_log_decoration to control
 *		    the layout of the log message.
 */
#  define sky_log_set_decor(decor)

/**
 * Add indentation to log message. Indentation will add SKY_LOG_INDENT_CHAR
 * before the message, and is useful to show the depth of function calls.
 *
 * @param indent    The indentation to add or substract. Positive value
 * 		    adds current indent, negative value subtracts current
 * 		    indent.
 */
#  define sky_log_add_indent(indent)

/**
 * Push indentation to the right by default value (SKY_LOG_INDENT).
 */
#  define sky_log_push_indent()

/**
 * Pop indentation (to the left) by default value (SKY_LOG_INDENT).
 */
#  define sky_log_pop_indent()

/**
 * Set color of log messages.
 *
 * @param level	    Log level which color will be changed.
 * @param color	    Desired color.
 */
#  define sky_log_set_color(level, color)

/**
 * Get current maximum log verbositylevel.
 *
 * @return	    Current log maximum level.
 */
#define sky_log_get_level()	0

/**
 * Get current log decoration flag.
 *
 * @return	    Log decoration flag.
 */
#define sky_log_get_decor()	0

/**
 * Get color of log messages.
 *
 * @param level	    Log level which color will be returned.
 * @return	    Log color.
 */
#define sky_log_get_color(level) 0


/**
 * Internal.
 */
#define sky_log_init()	0

#endif	/* #if SKY_LOG_MAX_LEVEL >= 1 */

/** 
 * @}
 */

/* **************************************************************************/
/*
 * Log functions implementation prototypes.
 * These functions are called by SKY_LOG macros according to verbosity
 * level specified when calling the macro. Applications should not normally
 * need to call these functions directly.
 */

/**
 * @def sky_log_wrapper_1(arg)
 * Internal function to write log with verbosity 1. Will evaluate to
 * empty expression if SKY_LOG_MAX_LEVEL is below 1.
 * @param arg       Log expression.
 */
#if SKY_LOG_MAX_LEVEL >= 1
    #define sky_log_wrapper_1(arg)	sky_log_1 arg
    /** Internal function. */
    void sky_log_1(const char *src, const char *format, ...);
#else
    #define sky_log_wrapper_1(arg)
#endif

/**
 * @def sky_log_wrapper_2(arg)
 * Internal function to write log with verbosity 2. Will evaluate to
 * empty expression if SKY_LOG_MAX_LEVEL is below 2.
 * @param arg       Log expression.
 */
#if SKY_LOG_MAX_LEVEL >= 2
    #define sky_log_wrapper_2(arg)	sky_log_2 arg
    /** Internal function. */
    void sky_log_2(const char *src, const char *format, ...);
#else
    #define sky_log_wrapper_2(arg)
#endif

/**
 * @def sky_log_wrapper_3(arg)
 * Internal function to write log with verbosity 3. Will evaluate to
 * empty expression if sky_LOG_MAX_LEVEL is below 3.
 * @param arg       Log expression.
 */
#if SKY_LOG_MAX_LEVEL >= 3
    #define sky_log_wrapper_3(arg)	sky_log_3 arg
    /** Internal function. */
    void sky_log_3(const char *src, const char *format, ...);
#else
    #define sky_log_wrapper_3(arg)
#endif

/**
 * @def sky_log_wrapper_4(arg)
 * Internal function to write log with verbosity 4. Will evaluate to
 * empty expression if SKY_LOG_MAX_LEVEL is below 4.
 * @param arg       Log expression.
 */
#if SKY_LOG_MAX_LEVEL >= 4
    #define sky_log_wrapper_4(arg)	sky_log_4 arg
    /** Internal function. */
    void sky_log_4(const char *src, const char *format, ...);
#else
    #define sky_log_wrapper_4(arg)
#endif

/**
 * @def sky_log_wrapper_5(arg)
 * Internal function to write log with verbosity 5. Will evaluate to
 * empty expression if SKY_LOG_MAX_LEVEL is below 5.
 * @param arg       Log expression.
 */
#if SKY_LOG_MAX_LEVEL >= 5
    #define sky_log_wrapper_5(arg)	sky_log_5 arg
    /** Internal function. */
    void sky_log_5(const char *src, const char *format, ...);
#else
    #define sky_log_wrapper_5(arg)
#endif

/**
 * @def sky_log_wrapper_6(arg)
 * Internal function to write log with verbosity 6. Will evaluate to
 * empty expression if SKY_LOG_MAX_LEVEL is below 6.
 * @param arg       Log expression.
 */
#if SKY_LOG_MAX_LEVEL >= 6
    #define sky_log_wrapper_6(arg)	sky_log_6 arg
    /** Internal function. */
    void sky_log_6(const char *src, const char *format, ...);
#else
    #define sky_log_wrapper_6(arg)
#endif

#endif  /* __SKY_LOG_H__ */


