/////////////////////////////////////////////////////////////////////////////
// Name:        wx/print.h
// Purpose:     Base header for printer classes
// Author:      Julian Smart
// Modified by:
// Created:
// RCS-ID:      $Id: print.h,v 1.1 2012/03/04 01:07:26 aliot Exp $
// Copyright:   (c) Julian Smart
// Licence:     wxWindows Licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_PRINT_H_BASE_
#define _WX_PRINT_H_BASE_

#include "wx/defs.h"

#if wxUSE_PRINTING_ARCHITECTURE

#if defined(__WXMSW__) && !defined(__WXUNIVERSAL__)

#include "wx/msw/printwin.h"

#elif defined(__WXMAC__)

#include "wx/mac/printmac.h"

#elif defined(__WXPM__)

#include "wx/os2/printos2.h"

#else

#include "wx/generic/printps.h"

#endif

#endif // wxUSE_PRINTING_ARCHITECTURE
#endif
    // _WX_PRINT_H_BASE_
