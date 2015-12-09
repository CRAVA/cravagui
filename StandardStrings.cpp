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
#include <QDir>
#include <QMessageBox>
#include <QTextStream>
#include <QLocale>
#include "StandardStrings.h"


/**
	@author Alf Birger Rustad (RD IRE FRM) <abir@statoil.com> Øystein Arneson (RD IRE FRM) <oyarn@statoil.com>, Erik Bakken <eriba@statoil.com>
*/
const QString& StandardStrings::waveletFormat(){
	static QString* format = new QString("Wavelet file (*.wlt *Swav *storm *crava);;ASCII file (*.asc *.ascii *.txt);;All files(*.*)");
	return *format;
};
const QString& StandardStrings::correlationFormat(){
	static QString* format = new QString("Correlation file (*crava);;ASCII file(*.asc *.ascii *.txt);;All files(*.*)");
	return *format;
};
const QString& StandardStrings::seismicFormat(){
	static QString* format = new QString("Seismic file (*sgy *segy *Sgri *storm *crava);;All files(*.*)");
	return *format;
};
const QString& StandardStrings::surfaceFormat(){
	static QString* format = new QString("Surface file (*storm *.irap *Sgri);;ASCII file(*.asc *.ascii *.txt);;All files(*.*)");
	return *format;
};

const QString& StandardStrings::wellFormat(){
	static QString* format = new QString("Well file (*rms *rmswell);;Las file (*.las);;All files(*.*)");
	return *format;
};

const QString& StandardStrings::xmlFormat(){
	static QString* format = new QString("XML file (*xml);;All files(*.*)");
	return *format;
};

const QString& StandardStrings::asciiFormat(){
	static QString* format = new QString("ASCII file(*.asc *.ascii *.txt);;All files(*.*)");
	return *format;
}

const QString& StandardStrings::cravaVersion(){
	static QString* version = new QString("CRAVA version 4.0 Beta");
	return *version;
};

const QString& StandardStrings::cravaGuiVersion(){
	static QString* version = new QString("CRAVA GUI v.4.0 Beta");
	return *version;
};
const QString& StandardStrings::StandardStrings::inputPath(){
	return inputPath_;
}

void StandardStrings::setinputPath(const QString &path){
	inputPath_=path;
}

QString StandardStrings::relativeFileName(const QString &fullFileName){
	//get the input directory and return the relative path, if the input directory is not specified, 
	//project-settings = 4 io-settings = 2 input-directory = 1
	//the current directory is always the input directory...
	if(!QFileInfo(fullFileName).isAbsolute()){
		return fullFileName;
	}
	return QDir(inputPath()).relativeFilePath(fullFileName);
}

bool StandardStrings::fileExists(const QString &fileName){
	//this will be a relative path, so the current directory of the GUI always have to be set to the input directory.
	if(fileName.isEmpty()){
		return true;
	}
	/*if(!QFileInfo(fullFileName).isAbsolute()){
		return QFile::exists(fileName);
	}*/
	return QFile::exists(QDir(inputPath()).absoluteFilePath(fileName));
}

QString StandardStrings::strippedName(const QString &fullFileName)
{
	return QFileInfo(fullFileName).fileName();
}

QString StandardStrings::checkedString(bool checked){
	if(checked){
		return QString("yes");
	}
	return QString("no");
}
bool StandardStrings::checkedBool(QString checked, QString defaultString){
	//need to be careful to set the ones that have default different than this
	if(checked==QString("yes")){
		return true;
	}
	else if (defaultString==QString("no") || checked==QString("no")){
		return false;
	}
	return true;//defaultString QString("yes")
}

void StandardStrings::convertLasToRms(QWidget *parent, const QString& lasFileName, const QString& rmsFileName, const QString& posFileName){
	//this assumes that MD/Z is the first log in the las file.
	QFile lasFile(lasFileName);
	QFile rmsFile(rmsFileName);
	QFile posFile(posFileName);
	if (!lasFile.open(QFile::ReadOnly | QFile::Text)) {
		QMessageBox::warning(parent, QString("Unable to open file"), QString("Could not open the file."), QMessageBox::Ok);
		return;
	}
	if (!posFile.open(QFile::ReadOnly | QFile::Text)) {
		QMessageBox::warning(parent, QString("Unable to open file"), QString("Could not open the file."), QMessageBox::Ok);
		return;
	}
	if (!rmsFile.open(QFile::WriteOnly | QFile::Text)) {
		QMessageBox::warning(parent, QString("Unable to open file"), QString("Could not open the file."), QMessageBox::Ok);
		return;
	}
	QString wellName;
	QTextStream lasStream(&lasFile);
	QTextStream rmsStream(&rmsFile);
	QTextStream posStream(&posFile);
	QString line = posStream.readLine();
	while(!line.isNull()){
		if(!line.isEmpty()){
			if(line[0]==QChar('-')){
				line = posStream.readLine();
				break;
			}
		}
		line = posStream.readLine();
	}
	QList<double> xList;
	QList<double> yList;
	QList<double> mdList;
	//
	while(!line.isNull()){
		QStringList list =line.simplified().split(" ");
		xList << list[0].toDouble();
		yList << list[1].toDouble();
		mdList << list[5].toDouble();
		line = posStream.readLine();
	}
	line = lasStream.readLine();
	while(!line.isNull()){
		if(line.split(" ").at(0)==QString("WELL")){
			wellName=line.split("\"").at(1);
			while(!wellName[0].isDigit()){
				wellName.remove(0,1);
				if(wellName.isEmpty()){
					QMessageBox::warning(parent, QString("No well name"), QString("Could not find a valid well name."), QMessageBox::Ok);
					return;
				}	
			}
			wellName=wellName.remove(" ");
			break;
		}
		line = lasStream.readLine();
	}
	if(wellName.isEmpty()){
		QMessageBox::warning(parent, QString("No well name"), QString("Could not find a valid well name."), QMessageBox::Ok);
		return;
	}
	QString wellX;
	while(!line.isNull()){
		if(line.split(" ").at(0)==QString("XWELL")){
			QStringList wellXList=line.split(" ");
			for(int i=0;i<wellXList.size();++i){
				if(!wellXList[i].isEmpty()){
					bool ok;
					wellXList[i].toDouble(&ok);
					if(ok){
						wellX=wellXList[i];
					}
				}
			}
			break;
		}
		line = lasStream.readLine();
	}
	if(wellX.isEmpty()){
		QMessageBox::warning(parent, QString("No well x-coordinate"), QString("Could not find a valid well x-coordinate."), QMessageBox::Ok);
		return;
	}
	QString wellY;
	while(!line.isNull()){
		if(line.split(" ").at(0)==QString("YWELL")){
			QStringList wellYList=line.split(" ");
			for(int i=0;i<wellYList.size();++i){
				if(!wellYList[i].isEmpty()){
					bool ok;
					wellYList[i].toDouble(&ok);
					if(ok){
						wellY=wellYList[i];
					}
				}
			}
			break;
		}
		line = lasStream.readLine();
	}
	if(wellY.isEmpty()){
		QMessageBox::warning(parent, QString("No well x-coordinate"), QString("Could not find a valid well x-coordinate."), QMessageBox::Ok);
		return;
	}
	while(!line.isNull()){
		if(!line.isEmpty()){
			if(line[0]==QChar('~')){
				lasStream.readLine();//read away the comments
				lasStream.readLine();
				break;
			}
		}
		line = lasStream.readLine();
	}
	QStringList logNames;
	lasStream.readLine();
	line = lasStream.readLine();
	while(!line.isEmpty()){
		logNames << line.split(".")[0].remove(" ");
		line = lasStream.readLine();
	}
	lasStream.readLine();//~ASCII
	line = lasStream.readLine();
	int minIndex=0;
	rmsStream.QTextStream::setRealNumberNotation(QTextStream::FixedNotation);
	if(wellX.size()>wellY.size()){
		rmsStream.setRealNumberPrecision(wellX.size());
	}
	else{
		rmsStream.setRealNumberPrecision(wellY.size());
	}
	rmsStream.setRealNumberPrecision(2);
	rmsStream << 1.0 << endl;
	rmsStream << "undefined" << endl;
	rmsStream << wellName << " " << wellX << " " << wellY << endl;
	rmsStream << logNames.size() << endl;
	for(int i=0;i<logNames.size();++i){
		rmsStream << logNames[i] << "  " << "UNK lin" << endl;
	}
	bool first=true;
	while(!line.isEmpty()){
		if(!first){
			rmsStream << endl;
		}
		else{
			first=false;
		}
		bool printed=false;
		QStringList lineList = line.simplified().split(" ");
		for(int j=minIndex;j<mdList.size();++j){
			if(mdList[j]==lineList.at(0).toDouble()){
				rmsStream << xList[j] << " " << yList[j];
				for(int i=0;i<lineList.size();++i){
					rmsStream << " " << lineList.at(i);
				}
				printed=true;
				break;
			} else if(mdList[j]<lineList.at(0).toDouble()){
				minIndex=j;
			} else{
				bool overRule=false;
				while(mdList[j]==mdList[minIndex]){
					if(j<mdList.size()-1){
						++j;
					}
					else {
						overRule=true;
						break;
					}
				}
				double fractionDifference=(lineList.at(0).toDouble()-mdList[minIndex])/(mdList[j]-mdList[minIndex]);
				if(overRule){
					fractionDifference=0;
				}
				rmsStream << (xList[j]+fractionDifference*(xList[j]-xList[minIndex])) << " ";
				rmsStream << (yList[j]+fractionDifference*(yList[j]-yList[minIndex]));
				for(int i=0;i<lineList.size();++i){
					rmsStream << " " << lineList.at(i);
				}
				printed=true;
				break;
			}
		}
		if(!printed){
				
				rmsStream << (xList[xList.size()-1]) << " " << (yList[yList.size()-1]);
				for(int i=0;i<lineList.size();++i){
					rmsStream << " " << lineList.at(i);
				}
		}
		line = lasStream.readLine();
	}

}
