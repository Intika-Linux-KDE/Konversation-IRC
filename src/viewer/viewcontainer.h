/*
  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
*/

/*
  Copyright (C) 2006-2008 Eike Hein <hein@kde.org>
*/

#ifndef VIEWCONTAINER_H
#define VIEWCONTAINER_H

#include "mainwindow.h"
#include "common.h"
#include "server.h"

#include <QAbstractItemModel>
#include <QMimeData>
#include <QTabWidget>

class QSplitter;

class KActionCollection;

class MainWindow;
class ViewTree;
class ChatWindow;
class Server;
class Images;
class UrlCatcher;
class NicksOnline;
class QueueTuner;
class ViewSpringLoader;

namespace Konversation
{
    class InsertCharDialog;
    class ServerGroupSettings;

    namespace DCC
    {
        class Chat;
    }
}

class ViewMimeData : public QMimeData
{
    public:
        explicit ViewMimeData(ChatWindow *view);
        ~ViewMimeData();

        ChatWindow* view() const;

    private:
        ChatWindow *m_view;
};

class TabWidget : public QTabWidget
{
    Q_OBJECT

    public:
        explicit TabWidget(QWidget* parent = 0);
        ~TabWidget();

    Q_SIGNALS:
        void contextMenu(QWidget* widget, const QPoint& pos);
        void tabBarMiddleClicked(int index);

    protected:
        void contextMenuEvent(QContextMenuEvent* event) Q_DECL_OVERRIDE;
        void mouseReleaseEvent(QMouseEvent* event) Q_DECL_OVERRIDE;
};

class ViewContainer : public QAbstractItemModel
{
    Q_OBJECT

    Q_PROPERTY(QObject* currentView READ getFrontView NOTIFY viewChanged) // WIPQTQUICK TODO Fix inconsistent naming
    Q_PROPERTY(QObject* currentServer READ getFrontServer NOTIFY viewChanged) // WIPQTQUICK

    public:
        enum DataRoles {
            ColorRole = Qt::UserRole + 1,
            DisabledRole,
            HighlightRole,
            ChatWindowRole, // WIPQTQUICK
        };
        Q_ENUM(DataRoles)

        explicit ViewContainer(MainWindow* window);
        ~ViewContainer();

        QHash<int, QByteArray> roleNames() const override;

        Q_INVOKABLE void setCurrentNick(const QString &text);

        QSplitter* getWidget() { return m_viewTreeSplitter; }
        MainWindow* getWindow() { return m_window; }
        KActionCollection* actionCollection() { return m_window->actionCollection(); }

        QPointer<ChatWindow> getFrontView() { return m_frontView; }
        Server* getFrontServer() { return m_frontServer; }

        void prepareShutdown();

        int rowCount(const QModelIndex & parent = QModelIndex()) const Q_DECL_OVERRIDE;
        int columnCount(const QModelIndex& parent = QModelIndex()) const Q_DECL_OVERRIDE;

        QModelIndex index(int row, int column, const QModelIndex& parent = QModelIndex()) const Q_DECL_OVERRIDE;
        QModelIndex indexForView(ChatWindow* view) const;
        QModelIndex parent(const QModelIndex& index) const Q_DECL_OVERRIDE;

        QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const Q_DECL_OVERRIDE;

        Qt::DropActions supportedDragActions() const Q_DECL_OVERRIDE;
        Qt::DropActions supportedDropActions() const Q_DECL_OVERRIDE;
        Qt::ItemFlags flags(const QModelIndex &index) const Q_DECL_OVERRIDE;
        QStringList mimeTypes() const Q_DECL_OVERRIDE;
        QMimeData* mimeData(const QModelIndexList &indexes) const Q_DECL_OVERRIDE;
        bool canDropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column, const QModelIndex &parent) const Q_DECL_OVERRIDE;
        bool dropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column, const QModelIndex &parent) Q_DECL_OVERRIDE;

        bool removeRows(int row, int count, const QModelIndex &parent = QModelIndex()) Q_DECL_OVERRIDE;

        QString currentViewTitle();
        QString currentViewURL(bool passNetwork = true);

        void appendToFrontmost(const QString& type,const QString& message,ChatWindow* serverView,
                               const QHash<QString, QString> &messageTags = QHash<QString, QString>(), bool parseURL = true);

        void showQueueTuner(bool);

        int getViewIndex(QWidget* widget);
        ChatWindow* getViewAt(int index);

        QList<QPair<QString,QString> > getChannelsURI();

    public Q_SLOTS:
        void updateAppearance();
        void saveSplitterSizes();
        void setViewTreeShown(bool show = false);

        void updateViews(const Konversation::ServerGroupSettingsPtr serverGroup = Konversation::ServerGroupSettingsPtr());
        void setViewNotification(ChatWindow* widget, const Konversation::TabNotifyType& type);
        void unsetViewNotification(ChatWindow* view);
        void toggleViewNotifications();
        void toggleAutoJoin();
        void toggleConnectOnStartup();

        void showView(QObject* view); // WIPQTQUICK
        void goToView(int page);
        Q_INVOKABLE void showNextView();
        Q_INVOKABLE void showPreviousView();
        void showNextActiveView();
        void showLastFocusedView();

        bool canMoveViewLeft() const;
        bool canMoveViewRight() const;
        void moveViewLeft();
        void moveViewRight();

        void closeView(int view);
        void closeView(ChatWindow* view);
        void closeViewMiddleClick(int view);
        void closeCurrentView();
        void renameKonsole();
        void cleanupAfterClose(ChatWindow* view);

        void changeViewCharset(int index);
        void updateViewEncoding(ChatWindow* view);

        void showViewContextMenu(QWidget* tab, const QPoint& pos);

        void clearView();
        void clearAllViews();

        void findText();
        void findNextText();
        void findPrevText();

        void insertCharacter();
        void insertChar(uint chr);
        void insertIRCColor();
        void doAutoReplace();

        void focusInputBox();

        void clearViewLines();
        void insertRememberLine();
        void cancelRememberLine();
        void insertMarkerLine();
        void insertRememberLines(Server* server);

        void openLogFile();
        void openLogFile(const QString& caption, const QString& file);

        void addKonsolePanel();

        void zoomIn();
        void zoomOut();
        void resetFont();

        void addUrlCatcher();
        void closeUrlCatcher();

        void toggleDccPanel();
        void addDccPanel();
        void closeDccPanel();
        void deleteDccPanel();
        ChatWindow* getDccPanel();

        void addDccChat(Konversation::DCC::Chat* myNick);

        StatusPanel* addStatusView(Server* server);
        RawLog* addRawLog(Server* server);
        void disconnectFrontServer();
        void reconnectFrontServer();
        void showJoinChannelDialog();
        void connectionStateChanged(Server* server, Konversation::ConnectionState state);
        void channelJoined(Channel* channel);

        Channel* addChannel(Server* server, const QString& name);
        void rejoinChannel();
        void openChannelSettings();
        void toggleChannelNicklists();

        Query* addQuery(Server* server,const NickInfoPtr & name, bool weinitiated=true);
        void updateQueryChrome(ChatWindow* view, const QString& name);
        void closeQueries();

        ChannelListPanel* addChannelListPanel(Server* server);
        void openChannelList(Server* server = 0, const QString& filter = QString(), bool getList = false);

        void openNicksOnlinePanel();
        void closeNicksOnlinePanel();

    Q_SIGNALS:
        void viewChanged(const QModelIndex& idx);
        void currentNickChanged(); // WIPQTQUICK
        void currentTopicChanged(); // WIPQTQUICK
        void currentUsersModelChanged(); // WIPQTQUICK
        void setWindowCaption(const QString& caption);
        void updateChannelAppearance();
        void contextMenuClosed();
        void resetStatusBar();
        void setStatusBarTempText(const QString& text);
        void clearStatusBarTempText();
        void setStatusBarInfoLabel(const QString& text);
        void clearStatusBarInfoLabel();
        void setStatusBarLagLabelShown(bool shown);
        void updateStatusBarLagLabel(Server* server, int msec);
        void resetStatusBarLagLabel(Server* server);
        void setStatusBarLagLabelTooLongLag(Server* server, int msec);
        void updateStatusBarSSLLabel(Server* server);
        void removeStatusBarSSLLabel();
        void autoJoinToggled(const Konversation::ServerGroupSettingsPtr);
        void autoConnectOnStartupToggled(const Konversation::ServerGroupSettingsPtr);

        void frontServerChanging(Server*);

    private Q_SLOTS:
        void setupIrcContextMenus();
        void viewSwitched(int newIndex);
        void onViewTreeDestroyed(QObject *object);

    private:
        void setupTabWidget();
        void setupViewTree();
        void removeViewTree();
        void updateTabWidgetAppearance();

        void addView(ChatWindow* view, const QString& label, bool weinitiated=true);
        int insertIndex(ChatWindow* view);
        void unclutterTabs();

        void updateViewActions(int index);
        void updateFrontView();

        void setFrontServer(Server *);

        void initializeSplitterSizes();
        bool m_saveSplitterSizesLock;

        MainWindow* m_window;

        QSplitter* m_viewTreeSplitter;
        TabWidget* m_tabWidget;
        ViewTree* m_viewTree;
        QWidget* m_vbox;
        QueueTuner* m_queueTuner;

        Images* images;

        QPointer<Server> m_frontServer;
        QPointer<Server> m_contextServer;
        QPointer<ChatWindow> m_frontView;
        QPointer<ChatWindow> m_searchView;

        QPointer<ChatWindow> m_currentView;
        QPointer<ChatWindow> m_lastFocusedView;

        UrlCatcher* m_urlCatcherPanel;
        NicksOnline* m_nicksOnlinePanel;

        ChatWindow* m_dccPanel;
        bool m_dccPanelOpen;

        Konversation::InsertCharDialog* m_insertCharDialog;

        int m_popupViewIndex;
        int m_queryViewCount;

        QList<ChatWindow*> m_activeViewOrderList;

        ViewSpringLoader* m_viewSpringLoader;
};

#endif
