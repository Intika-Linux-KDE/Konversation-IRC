/*
  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
*/

/*
  Copyright (C) 2004 Peter Simonsson <psn@linux.se>
  Copyright (C) 2008 Eike Hein <hein@kde.org>
*/

#ifndef KONVERSATIONSERVERSETTINGS_H
#define KONVERSATIONSERVERSETTINGS_H

#include <QString>

namespace Konversation
{

    class ServerSettings
    {
        public:
            ServerSettings();
            ServerSettings(const ServerSettings& settings);
            explicit ServerSettings(const QString& host);

            ServerSettings &operator=(const ServerSettings& settings);

            void setHost(const QString& host);
            QString host() const { return m_host; }

            void setPort(int port) { m_port = port; }
            int port() const { return m_port;}

            void setPassword(const QString& password);
            QString password() const { return m_password; }

            void setSSLEnabled(bool enabled) { m_SSLEnabled = enabled; }
            bool SSLEnabled() const { return m_SSLEnabled; }

            void setBypassProxy(bool bypass) { m_bypassProxy = bypass; }
            bool bypassProxy() const { return m_bypassProxy; }

            bool operator== (const ServerSettings& settings) const;

        private:
            QString m_host;
            int m_port;
            QString m_password;
            bool m_SSLEnabled;
            bool m_bypassProxy;

    };

}
#endif
