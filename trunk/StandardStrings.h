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
#ifndef STANDARDSTRINGS_H
#define STANDARDSTRINGS_H

#include <QString>
#include <QWidget>


/**
	@author Øystein Arneson (RD IRE FRM) <oyarn@statoil.com>
*/
class StandardStrings
//this class handles the string operations that are similar for the entire program and stores fileformats. With multiple windows open it is needed to have one per run
{
public:
	static const QString& waveletFormat();//holds the format for wavelet files, restrict what files the user can select when browsing for files.
	static const QString& correlationFormat();//holds the format for correlation files, restrict what files the user can select when browsing for files.
	static const QString& seismicFormat();//holds the format for seismic files, restrict what files the user can select when browsing for files.
	static const QString& surfaceFormat();//holds the format for surface files, restrict what files the user can select when browsing for files.
	static const QString& wellFormat();//holds the format for well files, restrict what files the user can select when browsing for files.
	static const QString& xmlFormat();//holds the format for xml files, restrict what files the user can select when browsing for files, used for saving.
	static const QString& asciiFormat();//holds the format for ascii files, restrict what files the user can select when browsing for files.
	static const QString& cravaVersion();//holds the current version of the program
	static const QString& cravaGuiVersion();//holds the current version of the gui.
	const QString& inputPath();//the path for input directory, not static as it depends on values entered during this run.
	void setinputPath(const QString &path);//sets input path for this run
	bool fileExists(const QString &fileName);//checks whether a given file exists at the specified(often relative) location given the current input directory
	QString relativeFileName(const QString &fullFileName);//everything is given relative to the input directory
	static QString strippedName(const QString &fullFileName);//returns just the name of the file of the filepath
	static QString checkedString(bool checked);//converts a bool to string for insertion in the tree
	static bool checkedBool(QString checked, QString defaultString=QString("no"));//converts a string to a bool for checking checkboxes
	static void convertLasToRms(QWidget *parent=0, const QString& lasFileName=0, const QString& rmsFileName=0, const QString& posFileName=0);//converts las to rms files for wells, might be better as a seperate process so as to not hang the program while running.
private:
	QString inputPath_;//the input path of this run, not accessible from the outside.
	//StandardStrings *instance;
};

#endif
/*<html><head><meta name="qrichtext" content="1" /><style type="text/css">
p, li { white-space: pre-wrap; }
</style></head><body style=" font-family:'Sans Serif'; font-size:9pt; font-weight:400; font-style:normal; text-decoration:none;">
<p style="-qt-paragraph-type:empty; margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;"></p></body></html> not sure why this was set as default tooltip so i removed it...*/
