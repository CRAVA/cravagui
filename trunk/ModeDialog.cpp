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
#include "ModeDialog.h"
#include <QFileDialog> 
#include <QMessageBox>
#include <QTime>
#include <QSettings>
#include "StandardStrings.h"

#include <iostream>
using namespace std;

/**
	@author Alf Birger Rustad (RD IRE FRM) <abir@statoil.com> Øystein Arneson (RD IRE FRM) <oyarn@statoil.com>, Erik Bakken <eriba@statoil.com>
*/
ModeDialog::ModeDialog(Main_crava *main_crava, bool started, bool *pressedOpen, bool existing, QWidget *parent): QDialog(parent) {
	this->hasPressedOpen = pressedOpen;
	this->main_crava = main_crava;
	setupUi( this );
	setWindowTitle(QString("%1[*] - %2").arg(windowTitle()).arg(StandardStrings::cravaVersion()));
	titleLabel->setText("<h2>" + StandardStrings::cravaGuiVersion() + " for " +StandardStrings::cravaVersion() + "</h2><p>CRAVA (<b>C</b>ondition <b>R</b>eservoir variables on <b>A</b>mplitude <b>V</b>ersus <b>A</b>ngle)");
	//this makes sure the pages switch as apropriate and display the correct widgets for each mode.
	QSignalMapper *signalMapper=new QSignalMapper(this);
	signalMapper->setMapping(inversionRadioButton,0);
	signalMapper->setMapping(estimateRadioButton,1);
	signalMapper->setMapping(forwardRadioButton,2);
	connect(inversionRadioButton,SIGNAL(clicked()),signalMapper,SLOT(map()));
	connect(estimateRadioButton,SIGNAL(clicked()),signalMapper,SLOT(map()));
	connect(forwardRadioButton,SIGNAL(clicked()),signalMapper,SLOT(map()));
	connect(signalMapper,SIGNAL(mapped(int)),stackedWidget,SLOT(setCurrentIndex(int)));
	//initalize default values for the checkboxes.
	inversionRadioButton->setChecked(true);
	predictionCheckBox->setChecked(true);
	krigingCheckBox->setChecked(true);
	faciesCheckBox->setChecked(true);
	backgroundCheckBox->setChecked(true);
	correlationCheckBox->setChecked(true);
	waveletNoiseCheckBox->setChecked(true);
	seedCheckBox->setChecked(true);
	frame->setVisible(false);
	if (existing == true){
		for (int i=0; i<modeDialogToolBox->count(); i++){
			if ( modeDialogToolBox->itemText(i) == QString("Open previous project") ){
				modeDialogToolBox->setItemEnabled(i, false);
			}
			if ( modeDialogToolBox->itemText(i) == QString("Make new project") ){
				modeDialogToolBox->setItemText( i, QString("Mode") );
			}
		}
	}
	else{
		modeDialogToolBox->setCurrentWidget( modeDialogToolBox->widget(0) );
	}
	topDirectoryLineEdit->setText( main_crava->top_directoryPointer->text(1) );
	inputDirectoryLineEdit->setText( main_crava->top_directoryPointer->text(1) + main_crava->input_directoryPointer->text(1) );
	QSettings settings("Statoil","CRAVA");
	settings.beginGroup("crava");
	settings.beginGroup("GUI");
	openProjectLineEdit->setText( settings.value( QString("openProject"),QString("")).toString() );
	settings.endGroup();
	settings.endGroup();
	if(started){
		topDirectoryLabel->setEnabled(false);
		topDirectoryLineEdit->setEnabled(false);
		topDirectoryBrowsePushButton->setEnabled(false);
		inputDirectoryLabel->setEnabled(false);
		inputDirectoryLineEdit->setEnabled(false);
		inputDirectoryBrowsePushButton->setEnabled(false);
		if(main_crava->modePointer->text(1)==QString("forward")){
			forwardRadioButton->click();
		}
		else if(main_crava->modePointer->text(1)==QString("estimation")){
			estimateRadioButton->click();
			bool temp=!(main_crava->estimation_settings_estimate_backgroundPointer->text(1)==QString("no"));
			backgroundCheckBox->setChecked(temp);
			temp=!(main_crava->estimation_settings_estimate_correlationsPointer->text(1)==QString("no"));
			correlationCheckBox->setChecked(temp);
			temp=!(main_crava->estimation_settings_estimate_wavelet_or_noisePointer->text(1)==QString("no"));
			waveletNoiseCheckBox->setChecked(temp);
		}
		else {
			inversionRadioButton->click();
			bool temp=!(main_crava->predictionPointer->text(1)==QString("no"));
			predictionCheckBox->setChecked(temp);
			if(main_crava->simulationPointer->text(1)==QString("yes")){
				simulationCheckBox->setChecked(true);
				if(!main_crava->simulation_seedPointer->text(1).isEmpty()){
					seedCheckBox->setChecked(false);
				}
				seedLineEdit->setText( main_crava->simulation_seedPointer->text(1) );
				simulationsLineEdit->setText( main_crava->simulation_number_of_simulationsPointer->text(1) );
			}
			temp=!(main_crava->inversion_settings_kriging_to_wellsPointer->text(1)==QString("no"));
			krigingCheckBox->setChecked(temp);
			temp=!(main_crava->inversion_settings_facies_probabilitiesPointer->text(1)==QString("no"));
			faciesCheckBox->setChecked(temp);
		}
	}
}


void ModeDialog::modes(){
	
	if(estimateRadioButton->isChecked()){
		main_crava->modePointer->setText(1,QString("estimation"));
		//loop over inversion-settings
		
		for(int i=0;i<main_crava->inversion_settingsPointer->childCount();++i){
			main_crava->inversion_settingsPointer->child(i)->setText(1,QString(""));
			for(int j=0;j<main_crava->inversion_settingsPointer->child(i)->childCount();++j){
				main_crava->inversion_settingsPointer->child(i)->child(j)->setText(1,QString(""));
			}
		}
		//jump to estimation settings
			//"estimate-background"
			if(backgroundCheckBox->isChecked()){
				main_crava->estimation_settings_estimate_backgroundPointer->setText(1,QString("yes"));
			}
			else{
				main_crava->estimation_settings_estimate_backgroundPointer->setText(1,QString("no"));
			}
			//estimate-correlations
			if(correlationCheckBox->isChecked()){
				main_crava->estimation_settings_estimate_correlationsPointer->setText(1,QString("yes"));
			}
			else{
				main_crava->estimation_settings_estimate_correlationsPointer->setText(1,QString("no"));
			}
			//estimate-wavelet-or-noise
			if(waveletNoiseCheckBox->isChecked()){
				main_crava->estimation_settings_estimate_wavelet_or_noisePointer->setText(1,QString("yes"));
			}
			else{
				main_crava->estimation_settings_estimate_wavelet_or_noisePointer->setText(1,QString("no"));
			}
	} 
	else if(forwardRadioButton->isChecked()){
		main_crava->modePointer->setText(1,QString("forward"));
		
		for(int i=0;i<main_crava->inversion_settingsPointer->childCount();++i){
			main_crava->inversion_settingsPointer->child(i)->setText(1,QString(""));
			for(int j=0;j<main_crava->inversion_settingsPointer->child(i)->childCount();++j){
				main_crava->inversion_settingsPointer->child(i)->child(j)->setText(1,QString(""));
			}
		}
		
		for(int i=0;i<main_crava->estimation_settingsPointer->childCount();++i){
			main_crava->estimation_settingsPointer->child(i)->setText(1,QString(""));
		}
	}
	else {
		main_crava->modePointer->setText(1,QString("inversion"));
		//jump to inversion-settings
			//prediction
			if(predictionCheckBox->isChecked()){//what is the default?
				main_crava->predictionPointer->setText(1,QString("yes"));
			}
			else {
				main_crava->predictionPointer->setText(1,QString("no"));
			}
			//simulation
			if(simulationCheckBox->isChecked()){
				
				if(!seedCheckBox->isChecked()){
					//seed
					main_crava->simulation_seedPointer->setText(1,seedLineEdit->text());
					//seed-fileshould create and use a random file if the checkbox is checked, now it does nothing
					main_crava->simulation_seed_filePointer->setText(1,QString(""));
				}
				else{
					QSettings settings("Statoil","CRAVA");
					settings.beginGroup("crava");
					settings.beginGroup("GUI");
					settings.beginGroup("tree");
					settings.beginGroup(main_crava->simulationPointer->text(0));
					//seed-fileshould create and use a random file if the checkbox is checked, now it does nothing
					main_crava->simulation_seed_filePointer->setText(1,settings.value(main_crava->simulation_seed_filePointer->text(0),QString("")).toString());
					//seed
					main_crava->simulation_seedPointer->setText(1,QString(""));
				}
				//number-of-simulations
				main_crava->simulation_number_of_simulationsPointer->setText(1,simulationsLineEdit->text());
			}//simulation
			else {
				for(int i=0;i<main_crava->simulationPointer->childCount();++i){
					main_crava->simulationPointer->child(i)->setText(1,QString(""));
				}
			}
			//kriging-to-wells
			if(krigingCheckBox->isChecked()){//what is the default?
				main_crava->inversion_settings_kriging_to_wellsPointer->setText(1,QString("yes"));
			}
			else {
				main_crava->inversion_settings_kriging_to_wellsPointer->setText(1,QString("no"));
			}
			
			if(faciesCheckBox->isChecked()){//what is the default?
				main_crava->inversion_settings_facies_probabilitiesPointer->setText(1,QString("yes"));
			}
			else {
				main_crava->inversion_settings_facies_probabilitiesPointer->setText(1,QString("no"));
			}
		for(int i=0;i<main_crava->estimation_settingsPointer->childCount();++i){
			main_crava->estimation_settingsPointer->child(i)->setText(1,QString(""));
		}
	}
}

void ModeDialog::on_simulationCheckBox_toggled(bool checked){
	frame->setVisible(checked);
	frame->setEnabled(checked);
}

void ModeDialog::on_seedCheckBox_toggled(bool checked){
	seedLineEdit->setEnabled(!checked);
	seedPushButton->setEnabled(!checked);
	seedLineEdit->setVisible(!checked);
	seedPushButton->setVisible(!checked);
}

void ModeDialog::on_topDirectoryBrowsePushButton_clicked(){
	QString dirName = QFileDialog::getExistingDirectory(this, QString("Open File"), QDir::currentPath());
	if(!dirName.isNull()){
		topDirectoryLineEdit->setText(dirName);
	}
}

void ModeDialog::on_inputDirectoryBrowsePushButton_clicked(){
	QString dirName = QFileDialog::getExistingDirectory(this, QString("Open File"), QDir::currentPath());
	if(!dirName.isNull()){
		inputDirectoryLineEdit->setText(dirName);
	}
}

void ModeDialog::on_openProjectBrowsePushButton_clicked(){
	QDir dir(main_crava->top_directoryPointer->text(1));
	QString dirName = QFileDialog::getOpenFileName(this, QString("Open File"), dir.path(), StandardStrings::xmlFormat());
	if(!dirName.isNull()){
		openProjectLineEdit->setText(dirName);
	}
}

void ModeDialog::on_buttonBox_accepted(){
	if ( modeDialogToolBox->itemText( modeDialogToolBox->currentIndex() ) == QString("Open previous project") ){
		*(this->hasPressedOpen) = true; //if the "Open previous project" tab is open, hasPressedOpen will be true, and this will make the program load the project later on in the code.
	}
	else if ( modeDialogToolBox->itemText( modeDialogToolBox->currentIndex() ) == QString("Make new project") ){ //check if the directories are valid.
		//if the folders exist, ok, otherwise message box.
		QDir topDirectory(topDirectoryLineEdit->text());
		QDir inputDirectory(inputDirectoryLineEdit->text());
		if(!topDirectory.exists()){
			return directoryInvalid();//should return a call to a void method making a message box.
		}
		if(inputDirectory.isRelative()){
			if(!((inputDirectory=topDirectory).cd(inputDirectoryLineEdit->text()))){
				return directoryInvalid();
			}
		}
		else{
			if(!inputDirectory.exists()){
				return directoryInvalid();
			}
		}
	}
	else{
		//nothing should be done.
	}
	accept();
}

void ModeDialog::directoryInvalid(){
	QMessageBox::warning(this, QString("Invalid directory"), QString("The specified working and/or input directory does not exist."), QMessageBox::Ok);
}

void ModeDialog::on_seedPushButton_clicked(){
	QTime t;
	qsrand (t.msecsTo(QTime::currentTime()));
	seedLineEdit->setText(QString("%1").arg(qrand()));
}

