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

#include <QApplication>
#include "Main_crava.h"
#include <QScrollArea>

#include <iostream>
using namespace std;

/**
	@author Alf Birger Rustad (RD IRE FRM) <abir@statoil.com> Øystein Arneson (RD IRE FRM) <oyarn@statoil.com>
*/
int main(int argc, char *argv[])
{
	QApplication app(argc, argv);//starts the application
	//there should probably be some code that calls Main_crava(0,true,fileName) if a an argument is an exisiting xml file.
	QPointer<Main_crava> crava=new Main_crava;//creates the application main window
	crava->show();//shows it.
	return app.exec();//pass control
 }
