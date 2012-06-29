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
#ifndef OUTPUTDIALOG_H
#define OUTPUTDIALOG_H

#include "ui_OutputDialog.h"
#include <QTreeWidgetItem>
#include <QSignalMapper>

#include "Main_crava.h"

/**
	@author Alf Birger Rustad (RD IRE FRM) <abir@statoil.com> Øystein Arneson (RD IRE FRM) <oyarn@statoil.com>, Erik Bakken <eriba@statoil.com>
*/
class OutputDialog : public QDialog, private Ui::OutputDialog{
	Q_OBJECT

public:
	OutputDialog(Main_crava *main_crava, QString *xmlFilename, QWidget *parent =0, bool faciesOn=true, bool forwardOn=false, bool estimationOn=false );//constructor making sure the correct widgets are displayed and the correct values/checks are displayed. First argument: A pointer to the Main_crava object. Second argument: Gives back the name of the xml-file.
	friend class Main_crava;
private:
	Main_crava *main_crava; //a pointer to the Main_crava object which gives access to its members.
	QString *xmlName; // a string that contains the name of the xml file. Used before the program saves and runs.
	void updateFields();//initializer of the dialog used in te constructor
	bool facies;//variable holding whether facies estimation is on and hence determines some on the output that can be specified.
	bool forward;//variable holding whether forward mode is on and hence determines some on the output that can be specified.
	bool estimation;//variable holding whether estimation mode is on and hence determines some on the output that can be specified.
	void updateOutput();//updates the tree with the correct values.
	void updateXmlName(); // updates the name of the xml file
private slots:
	void on_outputDirectoryBrowsePushButton_clicked();//open a filedialog for filling in the correct field.
};
#endif
