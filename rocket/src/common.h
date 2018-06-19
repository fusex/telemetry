/*
 * =====================================================================================
 *
 *       Filename:  common.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  20/05/2017 17:45:51
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Zakaria ElQotbi (zskdan), zakaria@derbsellicon.com
 *        Company:  Derb.io 
 *
 * =====================================================================================
 */

#ifndef _COMMON_H
#define _COMMON_H
#include <stdbool.h>

extern bool initfailed;

void setupCommon();
void setupFinishCommon();
void setupSetFailed();


#endif // _COMMON_H
