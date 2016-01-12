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
	QApplication app(argc, argv);//starts the application, argc is QString, argv is QList
        QPointer<Main_crava> crava;//declare pointer to the main window
	if(argc==2){//har filename been provided on command line?
        crava=new Main_crava(0,true,argv[1]);//creates the application main window with file from commandline
	}
	else{
	  crava=new Main_crava;//creates application window with no command line parameters
	}
	crava->show();//shows it.
	return app.exec();//pass control
 }
