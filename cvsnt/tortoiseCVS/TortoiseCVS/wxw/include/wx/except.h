///////////////////////////////////////////////////////////////////////////////
// Name:        wx/except.h
// Purpose:     C++ exception related stuff
// Author:      Vadim Zeitlin
// Modified by:
// Created:     17.09.2003
// RCS-ID:      $Id: except.h,v 1.1 2012/03/04 01:07:25 aliot Exp $
// Copyright:   (c) 2003 Vadim Zeitlin <vadim@wxwidgets.org>
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_EXCEPT_H_
#define _WX_EXCEPT_H_

#include "wx/defs.h"

// ----------------------------------------------------------------------------
// macros working whether wxUSE_EXCEPTIONS is 0 or 1
// ----------------------------------------------------------------------------

#if wxUSE_EXCEPTIONS
    #define wxTRY try
    #define wxCATCH_ALL(code) catch ( ... ) { code }
#else // !wxUSE_EXCEPTIONS
    #define wxTRY
    #define wxCATCH_ALL(code)
#endif // wxUSE_EXCEPTIONS/!wxUSE_EXCEPTIONS

#endif // _WX_EXCEPT_H_

