/* !
*	This file support logSystem for IOT SDK
*	@add by 6K at 2015/12/17 
*	@Cutting out from PJSIP log system 
*/
/* 
 * Copyright (C) 2008-2011 Teluu Inc. (http://www.teluu.com)
 * Copyright (C) 2003-2008 Benny Prijono <benny@prijono.org>
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

#include<linux/time.h>
#include<time.h>
#include<assert.h>
#include<string.h>
#include<unistd.h>
#include<stdio.h>
#include<TXSDKCommonDef.h>  // use bool defined in TXSDKCommon  
#include"skyLog.h"

#if SKY_LOG_MAX_LEVEL >= 1

static int sky_log_max_level = SKY_LOG_MAX_LEVEL;


static void *g_last_thread;

#if SKY_HAS_THREADS
static long thread_suspended_tls_id = -1;
#endif

#if !SKY_HAS_THREADS
static int log_indent;
#endif

static sky_log_func *log_writer = &sky_log_write;

// set log features 
static unsigned log_decor = SKY_LOG_HAS_TIME | SKY_LOG_HAS_MICRO_SEC |
			    SKY_LOG_HAS_SENDER | SKY_LOG_HAS_NEWLINE |
			    SKY_LOG_HAS_SPACE | SKY_LOG_HAS_THREAD_SWC |
			    SKY_LOG_HAS_INDENT |SKY_LOG_HAS_LEVEL_TEXT |
			    SKY_LOG_HAS_COLOR | SKY_LOG_HAS_YEAR |
			    SKY_LOG_HAS_MONTH | SKY_LOG_HAS_DAY_OF_MON 
			    ;


static int SKY_LOG_COLOR_0 = SKY_TERM_COLOR_BRIGHT | SKY_TERM_COLOR_R;
static int SKY_LOG_COLOR_1 = SKY_TERM_COLOR_BRIGHT | SKY_TERM_COLOR_R;
static int SKY_LOG_COLOR_2 = SKY_TERM_COLOR_BRIGHT | 
				   SKY_TERM_COLOR_R | 
				   SKY_TERM_COLOR_G;
static int SKY_LOG_COLOR_3 = SKY_TERM_COLOR_BRIGHT | 
				   SKY_TERM_COLOR_R | 
				   SKY_TERM_COLOR_G | 
				   SKY_TERM_COLOR_B;
static int SKY_LOG_COLOR_4 = SKY_TERM_COLOR_R | 
				   SKY_TERM_COLOR_G | 
				   SKY_TERM_COLOR_B;
static int SKY_LOG_COLOR_5 = SKY_TERM_COLOR_R | 
				   SKY_TERM_COLOR_G | 
				   SKY_TERM_COLOR_B;
static int SKY_LOG_COLOR_6 = SKY_TERM_COLOR_R | 
				   SKY_TERM_COLOR_G | 
				   SKY_TERM_COLOR_B;
/* Default terminal color */
static int SKY_LOG_COLOR_77 = SKY_TERM_COLOR_R | 
				    SKY_TERM_COLOR_G | 
				    SKY_TERM_COLOR_B;


static char log_buffer[2500];


#define LOG_MAX_INDENT		80


/**
 * Set terminal color.
 */
int sky_term_set_color(int color)
{
    /* put bright prefix to ansi_color */
    char ansi_color[12] = "\033[01;3";

    if (color & SKY_TERM_COLOR_BRIGHT) {
	color ^= SKY_TERM_COLOR_BRIGHT;
    } else {
	strcpy(ansi_color, "\033[00;3");
    }

    switch (color) {
    case 0:
	/* black color */
	strcat(ansi_color, "0m");
	break;
    case SKY_TERM_COLOR_R:
	/* red color */
	strcat(ansi_color, "1m");
	break;
    case SKY_TERM_COLOR_G:
	/* green color */
	strcat(ansi_color, "2m");
	break;
    case SKY_TERM_COLOR_B:
	/* blue color */
	strcat(ansi_color, "4m");
	break;
    case SKY_TERM_COLOR_R | SKY_TERM_COLOR_G:
	/* yellow color */
	strcat(ansi_color, "3m");
	break;
    case SKY_TERM_COLOR_R | SKY_TERM_COLOR_B:
	/* magenta color */
	strcat(ansi_color, "5m");
	break;
    case SKY_TERM_COLOR_G | SKY_TERM_COLOR_B:
	/* cyan color */
	strcat(ansi_color, "6m");
	break;
    case SKY_TERM_COLOR_R | SKY_TERM_COLOR_G | SKY_TERM_COLOR_B:
	/* white color */
	strcat(ansi_color, "7m");
	break;
    default:
	/* default console color */
	strcpy(ansi_color, "\033[00m");
	break;
    }

    fputs(ansi_color, stdout);

    return 0;
}


void term_set_color(int level)
{
#if defined(SKY_TERM_HAS_COLOR) && SKY_TERM_HAS_COLOR != 0
    sky_term_set_color(sky_log_get_color(level));
#else
    SKY_UNUSED_ARG(level);
#endif
}

void term_restore_color(void)
{
#if defined(SKY_TERM_HAS_COLOR) && SKY_TERM_HAS_COLOR != 0
    /* Set terminal to its default color */
    sky_term_set_color(sky_log_get_color(77));
#endif
}


void sky_log_write(int level, const char *buffer, int len)
{    
    SKY_UNUSED_ARG(len);

    /* Copy to terminal/file. */
    if (sky_log_get_decor() & SKY_LOG_HAS_COLOR) {
	term_set_color(level);	
	printf("[SkyWorth IOT] >>>> %s", buffer);
	term_restore_color();
    } else {    
	printf("%s", buffer);
    }
}


static void log_set_indent(int indent)
{
    log_indent = indent;
    if (log_indent < 0) log_indent = 0;
}

static int log_get_raw_indent(void)
{
    return log_indent;
}


static int log_get_indent(void)
{
    int indent = log_get_raw_indent();
    return indent > LOG_MAX_INDENT ? LOG_MAX_INDENT : indent;
}

void sky_log_add_indent(int indent)
{
    log_set_indent(log_get_raw_indent() + indent);
}

void sky_log_push_indent(void)
{
    sky_log_add_indent(1);
}

void sky_log_pop_indent(void)
{
    sky_log_add_indent(-1);
}

int sky_log_init(void)
{
    g_last_thread = NULL;
    return 0;
}

void sky_log_set_decor(unsigned decor)
{
    log_decor = decor;
}

unsigned sky_log_get_decor(void)
{
    return log_decor;
}

void sky_log_set_color(int level, int color)
{
    switch (level) 
    {
	case 0: SKY_LOG_COLOR_0 = color; 
	    break;
	case 1: SKY_LOG_COLOR_1 = color; 
	    break;
	case 2: SKY_LOG_COLOR_2 = color; 
	    break;
	case 3: SKY_LOG_COLOR_3 = color; 
	    break;
	case 4: SKY_LOG_COLOR_4 = color; 
	    break;
	case 5: SKY_LOG_COLOR_5 = color; 
	    break;
	case 6: SKY_LOG_COLOR_6 = color; 
	    break;
	/* Default terminal color */
	case 77: SKY_LOG_COLOR_77 = color; 
	    break;
	default:
	    /* Do nothing */
	    break;
    }
}

int sky_log_get_color(int level)
{
    switch (level) {
	case 0:
	    return SKY_LOG_COLOR_0;
	case 1:
	    return SKY_LOG_COLOR_1;
	case 2:
	    return SKY_LOG_COLOR_2;
	case 3:
	    return SKY_LOG_COLOR_3;
	case 4:
	    return SKY_LOG_COLOR_4;
	case 5:
	    return SKY_LOG_COLOR_5;
	case 6:
	    return SKY_LOG_COLOR_6;
	default:
	    /* Return default terminal color */
	    return SKY_LOG_COLOR_77;
    }
}

void sky_log_set_level(int level)
{
    sky_log_max_level = level;
}


int sky_log_get_level(void)
{
    return sky_log_max_level;
}


void sky_log_set_log_func( sky_log_func *func )
{
    log_writer = func;
}

sky_log_func* sky_log_get_log_func(void)
{
    return log_writer;
}


static void suspend_logging(int *saved_level)
{
	/* Save the level regardless, just in case SKYLIB is shutdown
	 * between suspend and resume.
	 */
	*saved_level = sky_log_max_level;
    
	sky_log_max_level = 0;
    
}

/* Resume logging facility for this thread */
static void resume_logging(int *saved_level)
{  
	if (sky_log_max_level==0 && *saved_level)
	{
		 sky_log_max_level = *saved_level;
	}
	   
  
}

/* Is logging facility suspended for this thread? */
static bool is_logging_suspended(void)
{
  
	return sky_log_max_level == 0;
    
}

int sky_gettimeofday(sky_time_val *tv)
{
    struct timeval tval;  	
    struct  timezone   tz;
    gettimeofday(&tval,&tz);    
    tv->sec = tval.tv_sec;
    tv->msec = tval.tv_usec / 1000;
    return 0;
}


int sky_time_decode(const sky_time_val *tv, sky_parsed_time *pt)
{
    struct tm *local_time;  
    local_time = localtime((time_t*)&tv->sec);
    pt->year = local_time->tm_year+1900;
    pt->mon = local_time->tm_mon;
    pt->day = local_time->tm_mday;
    pt->hour = local_time->tm_hour;
    pt->min = local_time->tm_min;
    pt->sec = local_time->tm_sec;
    pt->wday = local_time->tm_wday;
    pt->msec = tv->msec;

    return 0;

}

int sky_utoa_pad( unsigned long val, char *buf, int min_dig, int pad)
{
    char *p;
    int len; 

    p = buf;
    do {
        unsigned long digval = (unsigned long) (val % 10);
        val /= 10;
        *p++ = (char) (digval + '0');
    } while (val > 0);

    len = (int)(p-buf);
    while (len < min_dig) {
	*p++ = (char)pad;
	++len;
    }
    *p-- = '\0';

    do {
        char temp = *p;
        *p = *buf;
        *buf = temp;
        --p;
        ++buf;
    } while (buf < p);

    return len;
}


int sky_utoa(unsigned long val, char *buf)
{
    return sky_utoa_pad(val, buf, 0, 0);
}

int vsnprintf(char *s1, size_t len, const char *s2, va_list arg)
{
#define MARK_CHAR   ((char)255)
    int rc;

    s1[len-1] = MARK_CHAR;

    rc = vsprintf(s1,s2,arg);

    assert(s1[len-1] == MARK_CHAR || s1[len-1] == '\0');

    return rc;
}

int snprintf(char *s1, size_t len, const char *s2, ...)
{
    int ret;
    va_list arg;

    SKY_UNUSED_ARG(len);

    va_start(arg, s2);
    ret = vsprintf(s1, s2, arg);
    va_end(arg);
    
    return ret;
}




void  sky_log( const char *sender, int level, 
		     const char *format, va_list marker)
{
    sky_time_val now;
    sky_parsed_time ptime;
    char *pre;
    char log_buffer[2500];
    int saved_level, len, print_len, indent;   
    
    if (level > sky_log_max_level)
	return;

    if (is_logging_suspended())
	return;

    suspend_logging(&saved_level);

    /* Get current date/time. */
    sky_gettimeofday(&now);
    sky_time_decode(&now, &ptime);

    pre = log_buffer;
    // if has  set LOG Level ,show log level   
    if (log_decor & SKY_LOG_HAS_LEVEL_TEXT) 
    {
		static const char *ltexts[] = { "[FATAL]:", "[ERROR]:", "[WARN ]:", 
			     	 "[INFO ]:", "[DEBUG]:", "[TRACE]:", "[DETRC]:"};
		strcpy(pre, ltexts[level]);
		pre += 8;
    }

    // if set Day , show Day , useless ......
    if (log_decor & SKY_LOG_HAS_DAY_NAME) 
    {
		static const char *wdays[] = { "Sun", "Mon", "Tue", "Wed",
				       	"Thu", "Fri", "Sat"};
		strcpy(pre, wdays[ptime.wday]);
		pre += 3;
    }

    //if set Year   show year info 
    if (log_decor & SKY_LOG_HAS_YEAR) 
    {
		if (pre!=log_buffer)
		{
			*pre++ = ' ';
		}
		// add the left "[" to begin the [ xxxx ] 
		*pre++ = '[' ;
		pre += sky_utoa(ptime.year, pre);
    }

    // if Set Month , show month
    if (log_decor & SKY_LOG_HAS_MONTH) 
    {
		*pre++ = '-';
		pre += sky_utoa_pad(ptime.mon+1, pre, 2, '0');
    }

    // show Day 
    if (log_decor & SKY_LOG_HAS_DAY_OF_MON) 
    {
		*pre++ = '-';
		pre += sky_utoa_pad(ptime.day, pre, 2, '0');
    }

    // show TIME in sec 
    if (log_decor & SKY_LOG_HAS_TIME) 
    {
		if (pre!=log_buffer)
		{
			*pre++ = ' ';
		}
		pre += sky_utoa_pad(ptime.hour, pre, 2, '0');
		*pre++ = ':';
		pre += sky_utoa_pad(ptime.min, pre, 2, '0');
		*pre++ = ':';
		pre += sky_utoa_pad(ptime.sec, pre, 2, '0');
    }
    //show micro sec 
    if (log_decor & SKY_LOG_HAS_MICRO_SEC) 
    {
		*pre++ = '.';
		pre += sky_utoa_pad(ptime.msec, pre, 3, '0');
		// set the right "]"  to end the [ xxxxx ]
		*pre++ = ']' ;
    }
    
    if (log_decor & SKY_LOG_HAS_SENDER) {
	enum { SENDER_WIDTH = 14 };
	size_t sender_len = strlen(sender);
	if (pre!=log_buffer) *pre++ = ' ';
	if (sender_len <= SENDER_WIDTH) {
	    while (sender_len < SENDER_WIDTH)
		*pre++ = ' ', ++sender_len;
	    while (*sender)
		*pre++ = *sender++;
	} else {
	    int i;
	    for (i=0; i<SENDER_WIDTH; ++i)
		*pre++ = *sender++;
	}
    }
    
    if (log_decor != 0 && log_decor != SKY_LOG_HAS_NEWLINE)
	*pre++ = ' ';

	if (log_decor & SKY_LOG_HAS_SPACE) {
	*pre++ = ' ';
    }

#if SKY_LOG_ENABLE_INDENT
    if (log_decor & SKY_LOG_HAS_INDENT) {
	indent = log_get_indent();
	if (indent > 0) {
	    memset(pre, SKY_LOG_INDENT_CHAR, indent);
	    pre += indent;
	}
    }
#endif

    len = (int)(pre - log_buffer);

    /* Print the whole message to the string log_buffer. */
    print_len = vsnprintf(pre, sizeof(log_buffer)-len, format, 
				  marker);
    if (print_len < 0) {
	level = 1;
	print_len = snprintf(pre, sizeof(log_buffer)-len, 
				     "<logging error: msg too long>");
    }
    if (print_len < 1 || print_len >= (int)(sizeof(log_buffer)-len)) {
	print_len = sizeof(log_buffer) - len - 1;
    }
    len = len + print_len;
    if (len > 0 && len < (int)sizeof(log_buffer)-2) {
	if (log_decor & SKY_LOG_HAS_CR) {
	    log_buffer[len++] = '\r';
	}
	if (log_decor & SKY_LOG_HAS_NEWLINE) {
	    log_buffer[len++] = '\n';
	}
	log_buffer[len] = '\0';
    } else {
	len = sizeof(log_buffer)-1;
	if (log_decor & SKY_LOG_HAS_CR) {
	    log_buffer[sizeof(log_buffer)-3] = '\r';
	}
	if (log_decor & SKY_LOG_HAS_NEWLINE) {
	    log_buffer[sizeof(log_buffer)-2] = '\n';
	}
	log_buffer[sizeof(log_buffer)-1] = '\0';
    }

    /* It should be safe to resume logging at this point. Application can
     * recursively call the logging function inside the callback.
     */
    resume_logging(&saved_level);

    if (log_writer)
	(*log_writer)(level, log_buffer, len);
}



void sky_log_1(const char *obj, const char *format, ...)
{
    va_list arg;
    va_start(arg, format);
    sky_log(obj, 1, format, arg);
    va_end(arg);
}
#endif	/* SKY_LOG_MAX_LEVEL >= 1 */

#if SKY_LOG_MAX_LEVEL >= 2
void sky_log_2(const char *obj, const char *format, ...)
{
    va_list arg;
    va_start(arg, format);
    sky_log(obj, 2, format, arg);
    va_end(arg);
}
#endif

#if SKY_LOG_MAX_LEVEL >= 3
void sky_log_3(const char *obj, const char *format, ...)
{
    va_list arg;
    va_start(arg, format);
    sky_log(obj, 3, format, arg);
    va_end(arg);
}
#endif

#if SKY_LOG_MAX_LEVEL >= 4
void sky_log_4(const char *obj, const char *format, ...)
{
    va_list arg;
    va_start(arg, format);
    sky_log(obj, 4, format, arg);
    va_end(arg);
}
#endif

#if SKY_LOG_MAX_LEVEL >= 5
void sky_log_5(const char *obj, const char *format, ...)
{
    va_list arg;
    va_start(arg, format);
    sky_log(obj, 5, format, arg);
    va_end(arg);
}
#endif

#if SKY_LOG_MAX_LEVEL >= 6
void sky_log_6(const char *obj, const char *format, ...)
{
    va_list arg;
    va_start(arg, format);
    sky_log(obj, 6, format, arg);
    va_end(arg);
}
#endif

