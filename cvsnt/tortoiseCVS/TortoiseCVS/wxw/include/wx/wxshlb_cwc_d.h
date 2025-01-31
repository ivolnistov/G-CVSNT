/////////////////////////////////////////////////////////////////////////////
// Name:        wx_cw_d.h
// Purpose:     wxWidgets definitions for CodeWarrior builds (Debug)
// Author:      Stefan Csomor
// Modified by:
// Created:     12/10/98
// RCS-ID:      $Id: wxshlb_cwc_d.h,v 1.1 2012/03/04 01:07:27 aliot Exp $
// Copyright:   (c) Stefan Csomor
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_CW__
#define _WX_CW__

#if __MWERKS__ >= 0x2400
#pragma old_argmatch on
#endif

#if __option(profile)
#error "profiling is not supported in debug versions"
#else
#ifdef __cplusplus
    #ifdef __MACH__
        #include "wxshlb_Mach++_d.mch"
    #elif __POWERPC__
        #include "wxshlb_Carbon++_d.mch"
    #endif
#else
    #ifdef __MACH__
        #include "wxshlb_Mach_d.mch"
    #elif __POWERPC__
        #include "wxshlb_Carbon_d.mch"
    #endif
#endif
#endif

#endif
    // _WX_CW__
