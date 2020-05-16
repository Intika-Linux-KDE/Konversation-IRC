/*
  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
*/

#include "trayicon.h"
#include "application.h"
#include "config/preferences.h"

namespace Konversation
{

    TrayIcon::TrayIcon(QWidget* parent) : KStatusNotifierItem(parent)
    {
        setCategory(Communications);

        m_notificationEnabled = false;
        m_away = false;

        updateAppearance();

        setToolTip(QStringLiteral("konversation"), i18n("Konversation"), i18n("Konversation - IRC Client"));
    }

    TrayIcon::~TrayIcon()
    {
    }

    void TrayIcon::restore()
    {
        activate(QPoint());
    }

    void TrayIcon::startNotification()
    {
        if (!m_notificationEnabled)
        {
            return;
        }

        setIconByName("konv_message");
        setStatus(NeedsAttention);
    }

    void TrayIcon::endNotification()
    {
        setIconByName("konversation");
        setStatus(Passive);
    }

    void TrayIcon::setAway(bool away)
    {
        m_away = away;

        updateAppearance();
    }

    void TrayIcon::updateAppearance()
    {
        setIconByName(QStringLiteral("konversation"));
        setAttentionIconByName(QStringLiteral("konv_message"));
        setOverlayIconByName(m_away ? QStringLiteral("user-away") : QString());
    }
}


