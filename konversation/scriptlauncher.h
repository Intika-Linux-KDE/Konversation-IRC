/*
  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
*/

/*
  scriptlauncher.h  -  Launches shell scripts
  begin:     Mit M�r 12 2003
  copyright: (C) 2003 by Dario Abatianni
  email:     eisfuchs@tigress.com

  $Id$
*/

#include <qobject.h>

#ifndef SCRIPTLAUNCHER_H
#define SCRIPTLAUNCHER_H

/*
  @author Dario Abatianni
*/

class ScriptLauncher : public QObject
{
  Q_OBJECT

  public:
    ScriptLauncher(const QString& server,const QString& target);
    ~ScriptLauncher();

  public slots:
    void launchScript(const QString& parameter);

  protected:
    QString server;
    QString target;
};

#endif
