/////////////////////////////////////////////////////////////////////////////
// Name:        wx/xrc/xh_slidr.h
// Purpose:     XML resource handler for wxSlider
// Author:      Bob Mitchell
// Created:     2000/03/21
// RCS-ID:      $Id: xh_slidr.h,v 1.1 2012/03/04 01:07:54 aliot Exp $
// Copyright:   (c) 2000 Bob Mitchell and Verant Interactive
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_XH_SLIDR_H_
#define _WX_XH_SLIDR_H_

#include "wx/xrc/xmlres.h"

#if wxUSE_XRC && wxUSE_SLIDER

class WXDLLIMPEXP_XRC wxSliderXmlHandler : public wxXmlResourceHandler
{
    DECLARE_DYNAMIC_CLASS(wxSliderXmlHandler)
    enum
    {
        wxSL_DEFAULT_VALUE = 0,
        wxSL_DEFAULT_MIN = 0,
        wxSL_DEFAULT_MAX = 100
    };

public:
    wxSliderXmlHandler();
    virtual wxObject *DoCreateResource();
    virtual bool CanHandle(wxXmlNode *node);
};

#endif // wxUSE_XRC && wxUSE_SLIDER

#endif // _WX_XH_SLIDR_H_
