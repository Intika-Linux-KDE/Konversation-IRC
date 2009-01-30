/*
  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
*/

/*
  Copyright (C) 2006 Eike Hein <hein@kde.org>
*/

#include "statusbar.h" ////// header renamed
#include "mainwindow.h" ////// header renamed
#include "viewcontainer.h"
#include "ssllabel.h"

#include <q3whatsthis.h>
#include <qlabel.h>
#include <qtooltip.h>

#include <kstatusbar.h>
#include <klocale.h>
#include <kiconloader.h>
#include <ksqueezedtextlabel.h>


KonversationStatusBar::KonversationStatusBar(KonversationMainWindow* window)
{
    m_window = window;

    // Initialize status bar.
    m_window->statusBar();

    m_mainLabel = new KSqueezedTextLabel(m_window->statusBar());
    m_mainLabel->setObjectName("mainLabel");
    setMainLabelText(i18n("Ready."));
    m_mainLabel->setSizePolicy(QSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed));
    m_mainLabel->setMinimumWidth(0);

    // KSqueezedLabel calculates the wrong height. Popular workaround.
    int height = m_window->fontMetrics().height()+2;
    m_mainLabel->setFixedHeight(height);

    m_infoLabel = new QLabel(m_window->statusBar());
    m_infoLabel->setObjectName("infoLabel");
    m_infoLabel->hide();
    Q3WhatsThis::add(m_infoLabel, i18n("<qt>This shows the number of users in the channel, and the number of those that are operators (ops).<p>A channel operator is a user that has special privileges, such as the ability to kick and ban users, change the channel modes, make other users operators</qt>"));

    m_lagLabel = new QLabel(i18n("Lag: Unknown"), m_window->statusBar());
    m_lagLabel->setObjectName("lagLabel");
    m_lagLabel->hide();

    m_sslLabel = new SSLLabel(m_window->statusBar());
    m_sslLabel->setObjectName("sslLabel");
    m_sslLabel->setPixmap(SmallIcon("encrypted"));
    m_sslLabel->hide();
    Q3WhatsThis::add(m_sslLabel, i18n("All communication with the server is encrypted.  This makes it harder for someone to listen in on your communications."));

    m_window->statusBar()->addWidget(m_mainLabel, 1, false);
    m_window->statusBar()->addWidget(m_infoLabel, 0, true);
    m_window->statusBar()->addWidget(m_lagLabel, 0, true);
    m_window->statusBar()->addWidget(m_sslLabel, 0, true);

    Q3WhatsThis::add(m_window->statusBar(), i18n("<qt>The status bar shows various messages, including any problems connecting to the server.  On the far right the current delay to the server is shown.  The delay is the time it takes for messages from you to reach the server, and from the server back to you.</qt>"));
}

KonversationStatusBar::~KonversationStatusBar()
{
}

void KonversationStatusBar::updateAppearance()
{
    // KSqueezedLabel calculates the wrong height. Popular workaround.
    int height = m_window->fontMetrics().height()+2;
    m_mainLabel->setFixedHeight(height);
}

void KonversationStatusBar::resetStatusBar()
{
    setMainLabelText(i18n("Ready."));
    setInfoLabelShown(false);
    setLagLabelShown(false);
    clearInfoLabel();
    resetLagLabel();
}

void KonversationStatusBar::setMainLabelText(const QString& text)
{
    m_oldMainLabelText = text;

    // Don't overwrite the temp text if there is any.
    if (m_tempMainLabelText.isEmpty())
        m_mainLabel->setText(text);
}

void KonversationStatusBar::setMainLabelTempText(const QString& text)
{
    if (!text.isEmpty())
    {
        m_tempMainLabelText = text;
        m_mainLabel->setText(text);
    }
    else
        clearMainLabelTempText();
}

void KonversationStatusBar::clearMainLabelTempText()
{
    // Unset the temp text so the next setMainLabelText won't fail.
    m_tempMainLabelText = QString();

    m_mainLabel->setText(m_oldMainLabelText);
}

void KonversationStatusBar::setInfoLabelShown(bool shown)
{
    if (shown)
        m_infoLabel->show();
    else
        m_infoLabel->hide();
}

void KonversationStatusBar::updateInfoLabel(const QString& text)
{
    QString formatted = Konversation::removeIrcMarkup(text);
    m_infoLabel->setText(formatted);

    if (m_infoLabel->isHidden()) m_infoLabel->show();
}

void KonversationStatusBar::clearInfoLabel()
{
    m_infoLabel->setText(QString());
}

void KonversationStatusBar::setLagLabelShown(bool shown)
{
    if (shown)
        m_lagLabel->show();
    else
        m_lagLabel->hide();
}

void KonversationStatusBar::updateLagLabel(Server* lagServer, int msec)
{
    if (lagServer==m_window->getViewContainer()->getFrontServer())
    {
        setMainLabelText(i18n("Ready."));

        QString lagString = lagServer->getServerName() + " - ";

        if (msec == -1)
            lagString += i18n("Lag: Unknown");
        else if (msec < 1000)
            lagString += i18n("Lag: %1 ms", msec);
        else
            lagString += i18n("Lag: %1 s", msec / 1000);

        m_lagLabel->setText(lagString);

        if (m_lagLabel->isHidden()) m_lagLabel->show();
    }
}

void KonversationStatusBar::resetLagLabel()
{
    m_lagLabel->setText(i18n("Lag: Unknown"));
}

void KonversationStatusBar::setTooLongLag(Server* lagServer, int msec)
{
    if ((msec % 5000)==0)
    {
        int seconds  = msec/1000;
        int minutes  = seconds/60;
        int hours    = minutes/60;
        int days     = hours/24;
        QString lagString;

        if (days)
        {
            const QString daysString = i18np("1 day", "%1 days", days);
            const QString hoursString = i18np("1 hour", "%1 hours", (hours % 24));
            const QString minutesString = i18np("1 minute", "%1 minutes", (minutes % 60));
            const QString secondsString = i18np("1 second", "%1 seconds", (seconds % 60));
            lagString = i18nc("%1 = name of server, %2 = (x days), %3 = (x hours), %4 = (x minutes), %5 = (x seconds)", "No answer from server %1 for more than %2, %3, %4, and %5.", lagServer->getServerName(), daysString, hoursString, minutesString, secondsString);
            // or longer than an hour
        }
        else if (hours)
        {
            const QString hoursString = i18np("1 hour", "%1 hours", hours);
            const QString minutesString = i18np("1 minute", "%1 minutes", (minutes % 60));
            const QString secondsString = i18np("1 second", "%1 seconds", (seconds % 60));
            lagString = i18nc("%1 = name of server, %2 = (x hours), %3 = (x minutes), %4 = (x seconds)", "No answer from server %1 for more than %2, %3, and %4.", lagServer->getServerName(), hoursString, minutesString, secondsString);
            // or longer than a minute
        }
        else if (minutes)
        {
            const QString minutesString = i18np("1 minute", "%1 minutes", minutes);
            const QString secondsString = i18np("1 second", "%1 seconds", (seconds % 60));
            lagString = i18nc("%1 = name of server, %2 = (x minutes), %3 = (x seconds)", "No answer from server %1 for more than %2 and %3.", lagServer->getServerName(), minutesString, secondsString);
            // or just some seconds
        }
        else
        {
            lagString = i18np("No answer from server %1 for more than 1 second.", "No answer from server %1 for more than %1 seconds.", seconds, lagServer->getServerName());
        }

        setMainLabelText(lagString);
    }

    if (lagServer==m_window->getViewContainer()->getFrontServer())
    {
        QString lagString = lagServer->getServerName() + " - ";
        lagString.append(i18n("Lag: %1 s", msec/1000));

        if (m_lagLabel->isHidden()) m_lagLabel->show();
        m_lagLabel->setText(lagString);
    }
}

void KonversationStatusBar::updateSSLLabel(Server* server)
{
    if (server == m_window->getViewContainer()->getFrontServer()
        && server->getUseSSL() && server->isConnected())
    {
        disconnect(m_sslLabel,0,0,0);
        connect(m_sslLabel,SIGNAL(clicked()),server,SLOT(showSSLDialog()));
        QToolTip::remove(m_sslLabel);
        QToolTip::add(m_sslLabel,server->getSSLInfo());
        m_sslLabel->show();
    }
    else
        m_sslLabel->hide();
}

void KonversationStatusBar::removeSSLLabel()
{
    disconnect(m_sslLabel,0,0,0);
    m_sslLabel->hide();
}

// #include "./statusbar.moc"
