/*
  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
*/

/*
  copyright: (C) 2004 by Peter Simonsson
  email:     psn@linux.se
*/
#ifndef KONVERSATIONSERVERGROUPDIALOG_H
#define KONVERSATIONSERVERGROUPDIALOG_H

#include <kdialogbase.h>

#include "servergroupsettings.h"

class QLineEdit;
class QComboBox;
class QListBox;
class QCheckBox;

namespace Konversation {

class ServerGroupDialog : public KDialogBase
{
  Q_OBJECT
  public:
    ServerGroupDialog(const QString& title, QWidget* parent = 0, const char* name = 0);
    ~ServerGroupDialog();

    void setServerGroupSettings(const ServerGroupSettings& settings);
    ServerGroupSettings serverGroupSettings();

    void setAvailableGroups(const QStringList& groups);
  
  protected slots:
    void addServer();
    void editServer();
    void deleteServer();
    void moveServerUp();
    void moveServerDown();

    void addChannel();
    void editChannel();
    void deleteChannel();
    void moveChannelUp();
    void moveChannelDown();

  private:
    QLineEdit* m_nameEdit;
    QComboBox* m_groupCBox;
    QComboBox* m_identityCBox;
    QLineEdit* m_commandEdit;
    QListBox* m_serverLBox;
    QListBox* m_channelLBox;
    QCheckBox* m_autoConnectCBox;
    ServerList m_serverList;
    ChannelList m_channelList;

    int m_id;
};

};

#endif
