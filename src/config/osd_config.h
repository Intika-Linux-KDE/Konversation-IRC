/*
  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
*/

/*
  Copyright (C) 2005 Ivor Hewitt <ivor@ivor.org>
  Copyright (C) 2005 Ismail Donmez <ismail@kde.org>
  Copyright (C) 2006 Dario Abatianni <eisfuchs@tigress.com>
  Copyright (C) 2006 John Tapsell <johnflux@gmail.com>
  Copyright (C) 2006 Peter Simonsson <psn@linux.se>
*/

#ifndef OSD_CONFIG_H
#define OSD_CONFIG_H

#include "ui_osd_configui.h"
#include "settingspage.h"

class OSDPreviewWidget;


class OSD_Config : public QWidget, public KonviSettingsPage, private Ui::OSD_ConfigUI
{
    Q_OBJECT

    public:
        explicit OSD_Config( QWidget* parent = nullptr, const char* name = nullptr, Qt::WindowFlags fl = {} );
        ~OSD_Config() Q_DECL_OVERRIDE;

        void restorePageToDefaults() Q_DECL_OVERRIDE;
        void saveSettings() Q_DECL_OVERRIDE;
        void loadSettings() Q_DECL_OVERRIDE;

        bool hasChanged() Q_DECL_OVERRIDE;  // implement the interface, will not be used here, though

    protected Q_SLOTS:
        void slotOSDEnabledChanged(bool on);
        void slotCustomColorsChanged(bool on);
        void slotTextColorChanged(const QColor& color);
        void slotBackgroundColorChanged(const QColor& color);
        void slotScreenChanged(int index);
        void slotDrawShadowChanged(bool on);
        void slotUpdateFont(const QFont& font);
        void slotPositionChanged();

    protected:
        void showEvent(QShowEvent* event) Q_DECL_OVERRIDE;
        void hideEvent(QHideEvent* event) Q_DECL_OVERRIDE;

    private:
        OSDPreviewWidget* m_pOSDPreview;
};

#endif // OSD_CONFIG_H
