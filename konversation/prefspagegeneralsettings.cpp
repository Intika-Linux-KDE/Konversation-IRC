/*
  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
*/

/*
  prefspagegeneralsettings.cpp  -  Provides a user interface to customize general settings
  begin:     Fre Nov 15 2002
  copyright: (C) 2002 by Dario Abatianni
  email:     eisfuchs@tigress.com
*/

#include <qlayout.h>
#include <qvbox.h>
#include <qhbox.h>
#include <qgrid.h>
#include <qcheckbox.h>
#include <qlabel.h>
#include <qspinbox.h>

#include <klineedit.h>
#include <klocale.h>

#include "prefspagegeneralsettings.h"
#include "preferences.h"

PrefsPageGeneralSettings::PrefsPageGeneralSettings(QFrame* newParent,Preferences* newPreferences) :
                   PrefsPage(newParent,newPreferences)
{
  // Add a Layout to the General Settings pane
  QGridLayout* generalSettingsLayout=new QGridLayout(parentFrame,5,3,marginHint(),spacingHint(),"general_settings_layout");

  QHBox* commandCharBox=new QHBox(parentFrame);
  commandCharBox->setSpacing(spacingHint());

  QLabel* commandCharLabel=new QLabel(i18n("&Command char:"),commandCharBox);
  commandCharInput=new KLineEdit(preferences->getCommandChar(),commandCharBox);
  commandCharInput->setMaxLength(1);
  commandCharLabel->setBuddy(commandCharInput);

  // double click actions
  QVBox* actionBox=new QVBox(parentFrame);
  new QLabel(i18n("Commands to execute when doubleclicked in"),actionBox);

  QHBox* actionEditBox=new QHBox(actionBox);
  actionEditBox->setSpacing(spacingHint());

  QLabel* nickListLabel=new QLabel(i18n("&Nick list:"),actionEditBox);
  channelActionInput=new KLineEdit(preferences->getChannelDoubleClickAction(),actionEditBox);
  nickListLabel->setBuddy(channelActionInput);

  QLabel* notifyListLabel=new QLabel(i18n("N&otify list:"),actionEditBox);
  notifyActionInput=new KLineEdit(preferences->getNotifyDoubleClickAction(),actionEditBox);
  notifyListLabel->setBuddy(notifyActionInput);

  // nick completion special settings
  QVBox* suffixBox=new QVBox(parentFrame);
  new QLabel(i18n("Characters to add on nick completion"),suffixBox);

  QHBox* suffixEditBox=new QHBox(suffixBox);
  suffixEditBox->setSpacing(spacingHint());
  QLabel* startOfLineLabel=new QLabel(i18n("at &start of line:"),suffixEditBox);
  suffixStartInput=new KLineEdit(preferences->getNickCompleteSuffixStart(),suffixEditBox);
  startOfLineLabel->setBuddy(suffixStartInput);

  QLabel* middleLabel=new QLabel(i18n("&Elsewhere:"),suffixEditBox);
  middleLabel->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
  suffixMiddleInput=new KLineEdit(preferences->getNickCompleteSuffixMiddle(),suffixEditBox);
  middleLabel->setBuddy(suffixMiddleInput);

  autoReconnectCheck=new QCheckBox(i18n("A&uto reconnect"),parentFrame,"auto_reconnect_check");
  autoRejoinCheck=new QCheckBox(i18n("Auto re&join"),parentFrame,"auto_rejoin_check");
  autojoinOnInviteCheck=new QCheckBox(i18n("Autojoin channel on &invite"),parentFrame,"autojoin_on_invite_check");
  tabPlacementCheck=new QCheckBox(i18n("Place tab labels on &top"),parentFrame,"tab_placement_check");
  blinkingTabsCheck=new QCheckBox(i18n("&Blinking tabs"),parentFrame,"blinking_tabs_check");
  bringToFrontCheck=new QCheckBox(i18n("Bring new tabs to &front"),parentFrame,"bring_to_front_check");
  fixedMOTDCheck=new QCheckBox(i18n("Show &MOTD in fixed font"),parentFrame,"fixed_motd_check");
  beepCheck=new QCheckBox(i18n("Bee&p on incoming ASCII BEL"),parentFrame,"beep_check");
  rawLogCheck=new QCheckBox(i18n("Show ra&w log window on startup"),parentFrame,"raw_log_check");
  trayIconCheck=new QCheckBox(i18n("Show icon in s&ystem tray"),parentFrame,"tray_icon_check");

  reconnectTimeoutLabel=new QLabel(i18n("&Reconnect timeout:"),parentFrame);
  reconnectTimeoutLabel->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
  reconnectTimeoutSpin=new QSpinBox(1,9999,1,parentFrame,"reconnect_timeout_spin");
  reconnectTimeoutSpin->setValue(preferences->getMaximumLagTime());
  reconnectTimeoutSpin->setSuffix(i18n(" seconds"));
  reconnectTimeoutLabel->setBuddy(reconnectTimeoutSpin);

  autoReconnectCheck->setChecked(preferences->getAutoReconnect());
  // handle ghosing of timeout widget
  autoReconnectChanged(preferences->getAutoReconnect() ? 2 : 0);

  autoRejoinCheck->setChecked(preferences->getAutoRejoin());
  autojoinOnInviteCheck->setChecked(preferences->getAutojoinOnInvite());
  tabPlacementCheck->setChecked(preferences->getTabPlacement()==Preferences::Top);
  blinkingTabsCheck->setChecked(preferences->getBlinkingTabs());
  bringToFrontCheck->setChecked(preferences->getBringToFront());
  fixedMOTDCheck->setChecked(preferences->getFixedMOTD());
  beepCheck->setChecked(preferences->getBeep());
  rawLogCheck->setChecked(preferences->getRawLog());
  trayIconCheck->setChecked(preferences->getShowTrayIcon());

  QHBox* generalSpacer=new QHBox(parentFrame);

  int row=0;
  generalSettingsLayout->addMultiCellWidget(commandCharBox,row,row,0,2);
  row++;
  generalSettingsLayout->addMultiCellWidget(suffixBox,row,row,0,2);
  row++;
  generalSettingsLayout->addMultiCellWidget(actionBox,row,row,0,2);
  row++;
  generalSettingsLayout->addWidget(autoReconnectCheck,row,0);
  generalSettingsLayout->addWidget(reconnectTimeoutLabel,row,1);
  generalSettingsLayout->addWidget(reconnectTimeoutSpin,row,2);
  row++;
  generalSettingsLayout->addMultiCellWidget(autoRejoinCheck,row,row,0,2);
  row++;
  generalSettingsLayout->addMultiCellWidget(autojoinOnInviteCheck,row,row,0,2);
  row++;
  generalSettingsLayout->addMultiCellWidget(tabPlacementCheck,row,row,0,2);
  row++;
  generalSettingsLayout->addMultiCellWidget(blinkingTabsCheck,row,row,0,2);
  row++;
  generalSettingsLayout->addMultiCellWidget(bringToFrontCheck,row,row,0,2);
  row++;
  generalSettingsLayout->addMultiCellWidget(fixedMOTDCheck,row,row,0,2);
  row++;
  generalSettingsLayout->addMultiCellWidget(beepCheck,row,row,0,2);
  row++;
  generalSettingsLayout->addMultiCellWidget(rawLogCheck,row,row,0,2);
  row++;
  generalSettingsLayout->addMultiCellWidget(trayIconCheck,row,row,0,2);
  row++;
  generalSettingsLayout->addMultiCellWidget(generalSpacer,row,row,0,2);
  generalSettingsLayout->setRowStretch(row,10);

  connect(autoReconnectCheck,SIGNAL (stateChanged(int)),this,SLOT (autoReconnectChanged(int)) );
}

PrefsPageGeneralSettings::~PrefsPageGeneralSettings()
{
}

void PrefsPageGeneralSettings::autoReconnectChanged(int state)
{
  reconnectTimeoutLabel->setEnabled(state==2);
  reconnectTimeoutSpin->setEnabled(state==2);
}

void PrefsPageGeneralSettings::applyPreferences()
{
  preferences->setCommandChar(commandCharInput->text());
  preferences->setNickCompleteSuffixStart(suffixStartInput->text());
  preferences->setNickCompleteSuffixMiddle(suffixMiddleInput->text());
  preferences->setChannelDoubleClickAction(channelActionInput->text());
  preferences->setNotifyDoubleClickAction(notifyActionInput->text());

  preferences->setAutoReconnect(autoReconnectCheck->isChecked());
  preferences->setAutoRejoin(autoRejoinCheck->isChecked());
  preferences->setAutojoinOnInvite(autojoinOnInviteCheck->isChecked());
  preferences->setTabPlacement(tabPlacementCheck->isChecked() ? Preferences::Top : Preferences::Bottom);
  preferences->setBlinkingTabs(blinkingTabsCheck->isChecked());
  preferences->setBringToFront(bringToFrontCheck->isChecked());
  preferences->setFixedMOTD(fixedMOTDCheck->isChecked());
  preferences->setBeep(beepCheck->isChecked());
  preferences->setRawLog(rawLogCheck->isChecked());
  preferences->setShowTrayIcon(trayIconCheck->isChecked());

  preferences->setMaximumLagTime(reconnectTimeoutSpin->value());
}

#include "prefspagegeneralsettings.moc"
