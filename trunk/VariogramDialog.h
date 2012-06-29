/***************************************************************************
 *   Copyright (C) 2010 by Statoil
 *   sebn@statoil.com   *
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
	@author Øystein Arneson (RD IRE FRM) <oyarn@statoil.com>
*/
class VariogramDialog : public QDialog, private Ui::VariogramDialog{
	Q_OBJECT

public:
	VariogramDialog(QWidget *parent =0,bool genexp=true, QString angle=QString(), QString range=QString(), QString subrange=QString(), QString power=QString());//constructor, makes sure that the values in the dialog match the values in the tree
	friend class Main_crava;

private:
	QList<QString> variogramValues();//returns values for updating the tree.

private slots:
	void on_buttonBox_accepted();//makes it possible to validate that the output is valid before confirming
};
#endif
