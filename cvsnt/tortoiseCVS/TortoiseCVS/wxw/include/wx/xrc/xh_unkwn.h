/////////////////////////////////////////////////////////////////////////////
// Name:        wx/xrc/xh_unkwn.h
// Purpose:     XML resource handler for unkown widget
// Author:      Vaclav Slavik
// Created:     2000/03/05
// RCS-ID:      $Id: xh_unkwn.h,v 1.1 2012/03/04 01:07:54 aliot Exp $
// Copyright:   (c) 2000 Vaclav Slavik
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_XH_UNKWN_H_
#define _WX_XH_UNKWN_H_

#include "wx/xrc/xmlres.h"

#if wxUSE_XRC

class WXDLLIMPEXP_XRC wxUnknownWidgetXmlHandler : public wxXmlResourceHandler
{
    DECLARE_DYNAMIC_CLASS(wxUnknownWidgetXmlHandler)

public:
    wxUnknownWidgetXmlHandler();
    virtual wxObject *DoCreateResource();
    virtual bool CanHandle(wxXmlNode *node);
};

#endif // wxUSE_XRC

#endif // _WX_XH_UNKWN_H_
