/*
 * =====================================================================================
 *
 *       Filename:  init.h
 *
 *    Description:  Board low level initialization.
 *
 *        Version:  1.0
 *        Created:  20/05/2017 17:45:51
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Zakaria ElQotbi (zskdan), zakaria@elqotbi.com
 *
 * =====================================================================================
 */

#ifndef _INIT_H
#define _INIT_H

void setupInit();
void Init_Finish();
void Init_SetFailed();
void Init_SetFatal();
void Init_SetSemiFatal();

#endif // _INIT_H
