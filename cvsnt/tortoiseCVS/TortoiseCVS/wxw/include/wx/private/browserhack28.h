/////////////////////////////////////////////////////////////////////////////
// Name:        wx/private/browserhack28.h
// Purpose:     Allows GUI library to override base wxLaunchDefaultBrowser.
// Author:      David Elliott
// Modified by:
// Created:     2007-08-19
// RCS-ID:      $Id: browserhack28.h,v 1.1 2012/03/04 01:07:50 aliot Exp $
// Copyright:   (c) David Elliott
// Licence:     wxWidgets licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_PRIVATE_BROWSERHACK28_H_
#define _WX_PRIVATE_BROWSERHACK28_H_

typedef bool (*wxLaunchDefaultBrowserImpl_t)(const wxString& url, int flags);

// Function the GUI library can call to provide a better implementation
WXDLLIMPEXP_BASE void wxSetLaunchDefaultBrowserImpl(wxLaunchDefaultBrowserImpl_t newImpl);

#endif //ndef _WX_PRIVATE_BROWSERHACK28_H_
