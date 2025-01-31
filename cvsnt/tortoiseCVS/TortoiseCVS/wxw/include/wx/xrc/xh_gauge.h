/////////////////////////////////////////////////////////////////////////////
// Name:        wx/xrc/xh_gauge.h
// Purpose:     XML resource handler for wxGauge
// Author:      Bob Mitchell
// Created:     2000/03/21
// RCS-ID:      $Id: xh_gauge.h,v 1.1 2012/03/04 01:07:54 aliot Exp $
// Copyright:   (c) 2000 Bob Mitchell and Verant Interactive
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_XH_GAUGE_H_
#define _WX_XH_GAUGE_H_

#include "wx/xrc/xmlres.h"

#if wxUSE_XRC && wxUSE_GAUGE

class WXDLLIMPEXP_XRC wxGaugeXmlHandler : public wxXmlResourceHandler
{
    DECLARE_DYNAMIC_CLASS(wxGaugeXmlHandler)
    enum
    {
        wxGAUGE_DEFAULT_RANGE = 100
    };

public:
    wxGaugeXmlHandler();
    virtual wxObject *DoCreateResource();
    virtual bool CanHandle(wxXmlNode *node);
};

#endif // wxUSE_XRC && wxUSE_GAUGE

#endif // _WX_XH_GAUGE_H_
