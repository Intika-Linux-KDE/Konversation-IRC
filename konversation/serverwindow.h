/*
  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
*/

/*
  serverwindow.h  -  description
  begin:     Sun Jan 20 2002
  copyright: (C) 2002 by Dario Abatianni
  email:     eisfuchs@tigress.com
*/

/* QT specific includes */
#include <qtabwidget.h>
#include <qpushbutton.h>
#include <qcheckbox.h>
#include <qlabel.h>
#include <qvbox.h>
#include <qhbox.h>

/* KDE specific includes */
#include <kmainwindow.h>
#include <kdialog.h>
#include <kaction.h>
#include <kstdaction.h>

/*
  This is the Server Window Class. It shows a window
  with tab folders to store the channels and queries in.

  @author Dario Abatianni
*/

#ifndef SERVERWINDOW_H
#define SERVERWINDOW_H

#include "ledtabwidget.h"
#include "server.h"
#include "ircview.h"
#include "ircinput.h"
#include "highlightbox.h"
#include "quickbuttonsdialog.h"
#include "ignoredialog.h"

class Server;
class ServerWindow : public KMainWindow
{
  Q_OBJECT

  public:
    ServerWindow(Server* server);
    ~ServerWindow();

    void appendToStatus(const QString& type,const QString& message);
    void setServer(Server* server);
    void setLog(bool activated) { log=activated; };
    QTabWidget* getWindowContainer() {  return windowContainer; };
    void addView(QWidget* pane,int color,const QString& name);
    void showView(QWidget* pane);

  signals:
    void prefsChanged();

  public slots:
    void setNickname(const QString&);
    void newText(QWidget* view);
    void changedView(QWidget* view);
    void logText(const QString& text);

  protected slots:
    void statusTextEntered();
    void addStatusView();
    void showToolbar();
    void openPreferences();
    void quitProgram();

    void openHilight();
    void closeHilight(QSize newSize);
    void saveHilight(QStringList newList);

    void openIgnore();
    void applyIgnore(QPtrList<Ignore> newList);
    void closeIgnore(QSize newSize);

    void openButtons();
    void applyButtons(QStringList newList);
    void closeButtons(QSize newSize);

  protected:
    int spacing() {  return KDialog::spacingHint(); };
    int margin() {  return KDialog::marginHint(); };

    void setLogfileName(const QString& name);

    void readOptions();
    void saveOptions();
    bool queryExit();

    OutputFilter filter;
    LedTabWidget* windowContainer;
    QVBox* statusPane;
    IRCView* statusView;

    QPushButton* nicknameButton;
    IRCInput* statusInput;
    QCheckBox* logCheckBox;

    Server* server;
    KToggleAction* showToolBarAction;
    HighLightBox* hilightWindow;
    IgnoreDialog* ignoreDialog;
    QuickButtonsDialog* buttonsDialog;

    QFile logfile;
    bool log;
    bool firstLog;
};

#endif
