/***************************************************************************
 *   Copyright (C) 2010 by Statoil                                         *
 *   abir@statoil.com sebn@statoil.com                                     *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 3 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include "ui_SettingsDialog.h"
#include <QTreeWidgetItem>
#include "StandardStrings.h"

#include "Main_crava.h"

/**
	@author Alf Birger Rustad (RD IRE FRM) <abir@statoil.com> Øystein Arneson (RD IRE FRM) <oyarn@statoil.com>, Erik Bakken <eriba@statoil.com>, Andreas B. Lindblad <al587793@statoil.com>
*/
class SettingsDialog : public QDialog, private Ui::SettingsDialog{
	Q_OBJECT

public:
	SettingsDialog(Main_crava *main_crava, QWidget *parent =0, bool forwardOn=false, bool estimationOn=false, StandardStrings *standard=0);//constructor, makes sure that the proper widgets are displayed and correct values
	friend class Main_crava;

private:
	void updateFields();//this updates the fields with what is in the tree, used in initialization
	bool forward;//variable holding whether forward mode is on and hence determines some on the settings that can be specified.
	bool estimation;//variable holding whether estimation mode is on and hence determines some on the settings that can be specified.
	StandardStrings *standard;//makes sure that the correct directories are used for input and for relative paths.
	Main_crava *main_crava; //a pointer to the Main_crava object which gives access to its members.
	void updateSettings();//could return pointer to a new treewidget item, but now it just updates the current tree, this updates the tree with the settings currently set.

private slots:
	void on_headerUserDefinedRadioButton_toggled(bool checked);//makes sure the frame for making a user defined header format is displayed when that is required.
       	void on_bypassCoordinateNoRadioButton_toggled(bool checked);//shows locationScalingLineEdit if the No-button is toggled.
	void on_reflectionMatrixBrowsePushButton_clicked();//files checked for legality on close of settings Updates the field with the selected file
	void on_cravaPathBrowsePushButton_clicked();//files checked for legality on close of settings Updates the field with the selected file
	void on_textEditorBrowsePushButton_clicked(); //Updates the field with the selected file
	void on_terminalPathBrowsePushButton_clicked();//hard to check legality of commands? Updates the field with the selected file
	void on_referenceTimeSurface3DPushButton_clicked();//files checked for legality on close of settings Updates the field with the selected file
	void on_seedFileBrowsePushButton_clicked();//files checked for legality on close of settings Updates the field with the selected file
	void on_manualPathBrowsePushButton_clicked();//files checked for legality on close of settings Updates the field with the selected file
	void on_terminalCheckBox_toggled(bool checked);//makes the input for what terminal to run enabled/disabled.
	void on_segyCheckBox_toggled(bool checked);//can only define header format if segy is selected.
	void on_vpVsUserDefinedRadioButton_toggled(bool checked);//determines whether vpVsUserDefinedLineEdit is to be shown or not.
};
#endif
