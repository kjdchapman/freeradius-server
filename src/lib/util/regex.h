#pragma once
/*
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 2 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program; if not, write to the Free Software
 *   Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301, USA
 */
#ifdef HAVE_REGEX
/** Wrappers around various regular expression libraries
 *
 * @file src/lib/util/regex.h
 *
 * @copyright 2014 The FreeRADIUS server project
 */
RCSIDH(regex_h, "$Id$")

#  ifdef __cplusplus
extern "C" {
#  endif

#include <freeradius-devel/build.h>
#include <freeradius-devel/missing.h>

#include <stdbool.h>
#include <stdint.h>
#include <talloc.h>
#include <unistd.h>

#  ifdef HAVE_REGEX_PCRE2
#    define PCRE2_CODE_UNIT_WIDTH 8
#    include <pcre2.h>
/*
 *  libpcre defines its matches as an array of ints which is a
 *  multiple of three.
 */

/** libpcre2 has its own matchdata struct, we wrap it so we can use talloc destructors
 *
 */
typedef struct {
	pcre2_match_data	*match_data;	//!< Match data containing the subject
						///< and various match offsets.
	size_t			used;		//!< Number of slots filled with match data.
} fr_regmatch_t;

typedef struct {
	pcre2_code		*compiled;	//!< Compiled regular expression.
	uint32_t		subcaptures;	//!< Number of subcaptures contained within the expression.

	bool			precompiled;	//!< Whether this regex was precompiled,
						///< or compiled for one off evaluation.
	bool			jitd;		//!< Whether JIT data is available.
} regex_t;
#  elif defined(HAVE_REGEX_PCRE)
#    include <pcre.h>
/*
 *  Versions older then 8.20 didn't have the JIT functionality
 *  so, gracefully degrade.
 */
#    ifndef PCRE_STUDY_JIT_COMPILE
#      define PCRE_STUDY_JIT_COMPILE 0
#    endif
/*
 *  libpcre defines its matches as an array of ints which is a
 *  multiple of three.
 */
typedef struct {
	int a;
	int b;
	int c;
} regmatch_t;

/** Emulates the functionality of the pcre2_match_data struct
 *
 */
typedef struct {
	regmatch_t		*match_data;	//!< Slots for matches.
	size_t			allocd;		//!< Number of slots allocated for match data.
	size_t			used;		//!< Number of slots filled with match data.
	char const		*subject;	//!< A local copy of the subject.
} fr_regmatch_t;

/** Bundles compiled regular expression structures together
 *
 */
typedef struct {
	pcre			*compiled;	//!< Compiled regular expression.
	pcre_extra		*extra;		//!< Result of studying a regular expression.
	uint32_t		subcaptures;	//!< Number of subcaptures contained within the expression.

	bool			precompiled;	//!< Whether this regex was precompiled, or compiled for one off evaluation.
	bool			jitd;		//!< Whether JIT data is available.
} regex_t;
#  else
#    include <regex.h>
/*
 *  Allow REG_EXTENDED and REG_NOSUB to be or'd with flags
 *  if they're not defined.
 */
#    ifndef REG_EXTENDED
#      define REG_EXTENDED (0)
#    endif

#    ifndef REG_NOSUB
#      define REG_NOSUB (0)
#    endif

/** Emulates the functionality of the pcre2_match_data struct
 *
 */
typedef struct {
	regmatch_t		*match_data;	//!< Slots for matches.
	size_t			allocd;		//!< Number of slots allocated for match data.
	size_t			used;		//!< Number of slots filled with match data.
	char const		*subject;	//!< A local copy of the subject.
} fr_regmatch_t;

#  endif
ssize_t		regex_compile(TALLOC_CTX *ctx, regex_t **out, char const *pattern, size_t len,
			      bool ignore_case, bool multiline, bool subcaptures, bool runtime);
int		regex_exec(regex_t *preg, char const *subject, size_t len, fr_regmatch_t *regmatch);
uint32_t	regex_subcapture_count(regex_t const *preg);
fr_regmatch_t	*regex_match_data_alloc(TALLOC_CTX *ctx, uint32_t count);
#  ifdef __cplusplus
}
#  endif
#endif
