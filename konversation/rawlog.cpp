/*
  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
*/

/*
  RawLog.cpp  -  provides a view to the raw protocol
  begin:     Die M�r 18 2003
  copyright: (C) 2003 by Dario Abatianni
  email:     eisfuchs@tigress.com
*/

#include <klocale.h>
#include <kdebug.h>

#include "rawlog.h"
#include "ircview.h"
#include "server.h"
#include "konversationapplication.h"

RawLog::RawLog(QWidget* parent) : ChatWindow(parent)
{
  setName(i18n("Raw Log"));
  setType(ChatWindow::RawLog);
  setTextView(new IRCView(this,NULL));  // Server will be set later in setServer()
}

RawLog::~RawLog()
{
}

void RawLog::adjustFocus()
{
  // do nothing, just implement this to make abstract functions work
}

void RawLog::updateFonts()
{
  getTextView()->setFont(KonversationApplication::preferences.getTextFont());
  getTextView()->setViewBackground(KonversationApplication::preferences.getColor("TextViewBackground"),
                                   KonversationApplication::preferences.getBackgroundImageName());
}

void RawLog::closeYourself()
{
  // make the server delete us so server can reset the pointer to us
  server->closeRawLog();
}

bool RawLog::searchView() { return true; }

#include "rawlog.moc"
