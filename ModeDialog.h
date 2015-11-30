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
#ifndef MODEDIALOG_H
#define MODEDIALOG_H
#include <QTreeWidgetItem>
#include <QSignalMapper>
#include "ui_ModeDialog.h"

#include "Main_crava.h"

/**
	@author Alf Birger Rustad (RD IRE FRM) <abir@statoil.com> Øystein Arneson (RD IRE FRM) <oyarn@statoil.com>, Erik Bakken <eriba@statoil.com>, Andreas B. Lindblad <al587793@statoil.com>
*/
class ModeDialog : public QDialog, private Ui::ModeDialog
{
    Q_OBJECT
public:
	ModeDialog( Main_crava *main_crava,bool started, bool *pressedOpen, bool existing, QWidget *parent=0);//constructor, makes sure the correct values are in the correct fields. First argument: A pointer to the main object. Second argument: Is false the first time you get to the mode dialog whether you have just started the program or pressed "New" from the main window. Third argument: Will return true if the program should load a previous project. Fourth argument: Is only false when the program starts. When it is true, loading the project is no longer possible from "Mode" as it should be done from the main window. Fifth argument: The parent of this object.

	friend class Main_crava;
private:
	Main_crava *main_crava; //a pointer to the Main_crava object which gives access to its members.
	void modes();//retrives the values of the fields to update the tree
	void directoryInvalid();//popup box for telling the user that the directories have to be valid
	bool *hasPressedOpen; // a bool which is true if the user had the tab "Open previous project" open when pushing the Ok button.
	QList<QString> getDirectories();//retrives the paths to the directories to update the tree/fix the paths.
private slots:
	void on_oldProjectRadioButton_toggled(bool checked);//controls what is shown in the mode dialog
	void on_simulationCheckBox_toggled(bool checked);//slot responsible for showing/hiding the apropriate widgets for simulation
	void on_seedCheckBox_toggled(bool checked);//slot for hiding/showing the apropriate widgets for seed generation
	void on_seedPushButton_clicked(); //generates a new seed into the seed field
	void on_topDirectoryBrowsePushButton_clicked();//open a filedialog for filling in the correct field.
	void on_inputDirectoryBrowsePushButton_clicked();//open a filedialog for filling in the correct field.
	void on_buttonBox_accepted();//makes sure that the behavior on clicking ok is correct (error message if things are not correct). Checks which tab is open.
	void on_openProjectBrowsePushButton_clicked(); //open a filedialog for filling in the correct field.
};

#endif
