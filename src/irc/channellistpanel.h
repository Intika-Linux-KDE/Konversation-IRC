/*
  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
*/

/*
  Shows the list of channels

  Copyright (C) 2003 Dario Abatianni <eisfuchs@tigress.com>
  Copyright (C) 2009 Travis McHenry <wordsizzle@gmail.com>
*/

#ifndef CHANNELLISTPANEL_H
#define CHANNELLISTPANEL_H

#include "chatwindow.h"
#include "ui_channellistpanelui.h"

#include <QAbstractListModel>
#include <QSortFilterProxyModel>

class KToolBar;

struct ChannelItem
{
    QString name;
    int users;
    QString topic;
};

class ChannelListProxyModel : public QSortFilterProxyModel
{
    Q_OBJECT

    public:
        explicit ChannelListProxyModel(QObject *parent = nullptr);

        int filterMinimumUsers() { return m_minUsers; }
        int filterMaximumUsers() { return m_maxUsers; }
        bool filterTopic() { return m_filterTopic; }
        bool filterChannel() { return m_filterChannel; }

    public Q_SLOTS:
        void setFilterMinimumUsers(int users);
        void setFilterMaximumUsers(int users);

        void setFilterTopic(bool filter);
        void setFilterChannel(bool filter);

    protected:
        bool filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const Q_DECL_OVERRIDE;

    private:
        bool usersInRange(int users) const;
        int m_minUsers;
        int m_maxUsers;
        bool m_filterTopic;
        bool m_filterChannel;
};

class ChannelListModel : public QAbstractListModel
{
    Q_OBJECT

    public:
        explicit ChannelListModel(QObject* parent);

        void append(const ChannelItem& item);

        int columnCount(const QModelIndex& parent = QModelIndex()) const Q_DECL_OVERRIDE;
        int rowCount(const QModelIndex& parent = QModelIndex()) const Q_DECL_OVERRIDE;

        QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const Q_DECL_OVERRIDE;
        QVariant headerData (int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const Q_DECL_OVERRIDE;

    private:
        QList<ChannelItem> m_channelList;
};

class ChannelListPanel : public ChatWindow, private Ui::ChannelListWidgetUI
{
    Q_OBJECT

    public:
        explicit ChannelListPanel(QWidget* parent);
        ~ChannelListPanel() override;

        using ChatWindow::closeYourself;
        virtual bool closeYourself();
        void emitUpdateInfo() Q_DECL_OVERRIDE;

        bool isInsertSupported() Q_DECL_OVERRIDE { return true; }
        QString getTextInLine() Q_DECL_OVERRIDE { return m_filterLine->text(); }

    Q_SIGNALS:
        void refreshChannelList();
        void joinChannel(const QString& channelName);

    public Q_SLOTS:
        void refreshList();
        void addToChannelList(const QString& channel,int users,const QString& topic);
        void endOfChannelList();
        void applyFilterClicked();

        void appendInputText(const QString&, bool fromCursor) Q_DECL_OVERRIDE;
        void setFilter(const QString& filter);

    protected Q_SLOTS:
        void saveList();

        void filterChanged();
        void updateFilter();

        void updateUsersChannels();
        void currentChanged(const QModelIndex &current,const QModelIndex &previous);
        void setProgress();

        void joinChannelClicked();
        void contextMenu(const QPoint& pos);
        void openURL();
        //Used to disable functions when not connected
        void serverOnline(bool online) Q_DECL_OVERRIDE;

    protected:

        /** Called from ChatWindow adjustFocus */
        void childAdjustFocus()Q_DECL_OVERRIDE {}

        void countUsers(const QModelIndex& index, int pos);

        int m_numChannels;
        int m_numUsers;
        int m_visibleChannels;
        int m_visibleUsers;
        bool m_online;
        bool m_firstRun;
        bool m_regexState;

        QTimer* m_progressTimer;
        QTimer* m_filterTimer;
        QTimer* m_tempTimer;

        ChannelListModel* m_channelListModel;
        ChannelListProxyModel* m_proxyModel;

        KToolBar *m_toolBar;
        QAction *m_saveList;
        QAction *m_refreshList;
        QAction *m_joinChannel;
};

#endif
