/////////////////////////////////////////////////////////////////////////////
// Name:        wx/xrc/xh_split.h
// Purpose:     XRC resource for wxSplitterWindow
// Author:      panga@freemail.hu, Vaclav Slavik
// Created:     2003/01/26
// RCS-ID:      $Id: xh_split.h,v 1.1 2012/03/04 01:07:54 aliot Exp $
// Copyright:   (c) 2003 panga@freemail.hu, Vaclav Slavik
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_XH_SPLIT_H_
#define _WX_XH_SPLIT_H_

#include "wx/xrc/xmlres.h"

#if wxUSE_XRC && wxUSE_SPLITTER

class WXDLLIMPEXP_XRC wxSplitterWindowXmlHandler : public wxXmlResourceHandler
{
    DECLARE_DYNAMIC_CLASS(wxSplitterWindowXmlHandler)

public:
    wxSplitterWindowXmlHandler();
    virtual wxObject *DoCreateResource();
    virtual bool CanHandle(wxXmlNode *node);
};

#endif // wxUSE_XRC && wxUSE_SPLITTER

#endif // _WX_XH_SPLIT_H_
