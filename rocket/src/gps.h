/*
 * =====================================================================================
 *
 *       Filename:  gps.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  20/05/2017 17:31:08
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Zakaria ElQotbi (zskdan), zakaria@derbsellicon.com
 *        Company:  Derb.io 
 *
 * =====================================================================================
 */
#ifndef _GPS_H
#define _GPS_H

void setupGps();
void loopGps();
void GPS_getDateTime(char* str);
bool GPS_isFixed();

#endif //define _GPS_H
