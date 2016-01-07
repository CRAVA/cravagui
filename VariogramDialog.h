/***************************************************************************
 *   Copyright (C) 2010 by Statoil                                         *
 *   abir@statoil.com                                                      *
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
#ifndef VARIOGRAMDIALOG_H
#define VARIOGRAMDIALOG_H

#include "ui_VariogramDialog.h"

#include "Main_crava.h"

/**
	@author Alf Birger Rustad (Statoil R&D) <abir@statoil.com>, Øystein Arneson (RD IRE FRM) <oyarn@statoil.com>
*/
class VariogramDialog : public QDialog, private Ui::VariogramDialog{
	Q_OBJECT

public:
        //constructor, makes sure that the values in the dialog match the values in the tree
	VariogramDialog(QWidget *parent =0,bool genexp=true, QString angle=QString(), QString range=QString(), QString subrange=QString(), QString power=QString());
	friend class Main_crava;

private:
	QList<QString> variogramValues();//returns values for updating the tree.

private slots:
	void on_buttonBox_accepted();//makes it possible to validate that the output is valid before confirming
	void on_genexpRadioButton_toggled(bool checked);//radio button for general exponential variogram
	void on_sphericalRadioButton_toggled(bool checked);//radio button for spherical variogram
};
#endif
