/*
 *    Quick Cache Manager (for QTP preprocessor) version 1.0.3 [header]
 *    Copyright (C) 2004  Denis Slyusarev & ainmarh lab
 *
 *    This program is free software; you can redistribute it and/or modify
 *    it under the terms of the GNU General Public License as published by
 *    the Free Software Foundation; either version 2 of the License, or
 *    (at your option) any later version.
 *
 *    This program is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU General Public License for more details.
 *
 *    You should have received a copy of the GNU General Public License
 *    along with this program; if not, write to the Free Software
 *    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include <stdio.h>

/** WARNING! USE THIS FUNCTIONS BEFORE USE qtp_init() !!!  */
// = = = = = = = = = = = = = = = = = = = = = = = = = = = = //

/** Set QCM/QTP CONTENT-LENGTH buffers size */
void qcm_contentlength_set (size_t internal, size_t temp);

/** Set QTP/QCM CONTENT-LENGTH turn on */
void qcm_contentlength_on();

/** Set QTP/QCM CONTENT-LENGTH turn off */
void qcm_contentlength_off();

/** Set REDIRECTOR prefix for cache redirector */
void qcm_redirector (char *prefix);

/** Set QCM cache dir for working */
void qcm_dir (char *path);

/** WARNING! USE THIS FUNCTIONS BEFORE USE QTP/QCM !!! */
// = = = = = = = = = = = = = = = = = = = = = = = = = = //

/** QCM Init */
void qcm_init();

/** ------------ MAIN PART -- USE AFTER INITIALISATION ---------------- **/
// = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = //
	
/** Set timestamp for check/remove old cache
                                         !!! USE BEFORE USE qcm_open() !!! */
void qcm_checker (char *name, ...);

/** Add parameter to QCM processor list (GET/POST)
 *  if parameter==NULL clear all parameters 
                                         !!! USE BEFORE USE qcm_open() !!! */
void qcm_set (char *parameter);

/** Open QCM file */
FILE *qcm_open (char *name, ...);

/** Close QCM file */
void qcm_close (FILE **fl);

/** Clean current cache dir 
                                         !!! USE AFTER qcm_checker() !!! */
void qcm_clear ();
