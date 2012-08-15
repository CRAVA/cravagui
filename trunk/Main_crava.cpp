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

#include "Main_crava.h"
#include <QtGui>
#include <QObject>
#include "VariogramDialog.h"
#include "ModeDialog.h"
#include "SettingsDialog.h"

#include <string>


/**
	@author Alf Birger Rustad (RD IRE FRM) <abir@statoil.com> Øystein Arneson (RD IRE FRM) <oyarn@statoil.com>, Erik Bakken <eriba@statoil.com>, Andreas B. Lindblad <al587793@statoil.com>
*/
Main_crava::Main_crava(QWidget *parent, bool existing, const QString &filename) :QMainWindow(parent){

	setupUi( this );
	standard=new StandardStrings();//makes sure that the filepaths are relative to this instance.
	setCurrentFile(filename);
	setupButtonGroups();
	createActions();
	//survey information frames
	seismicDataFrame->setEnabled(false);
	deleteStackPushButton->setEnabled(false);
	applyToAllStacksPushButton->setEnabled(false);
	formatChangeFrame->setVisible(false);
	formatChangeFrame->setEnabled(false);
	waveletFrame->setVisible(false);
	waveletFrame->setEnabled(false);
	localWaveletFrame->setVisible(false);
	localWaveletFrame->setEnabled(false);
	localNoiseFrame->setVisible(false);
	localNoiseFrame->setEnabled(false);
	//well frames
	wellFrame->setEnabled(false);
	deleteWellPushButton->setEnabled(false);
	openWellPushButton->setEnabled(false);
	anglePositionlineEdit->setEnabled(false);
	weightLineEdit->setEnabled(false);
	optimizePositionFrame->setVisible(false);
	optimizePositionFrame->setEnabled(false);
	//horizon
	velocityFieldLineEdit->setVisible(false);
	velocityFieldBrowsePushButton->setVisible(false);
	velocityFieldLineEdit->setEnabled(false);
	velocityFieldBrowsePushButton->setEnabled(false);
	//Prior Model
	faciesFrame->setEnabled(false);
	deleteFaciesPushButton->setEnabled(false);
	//scrolling
	QScrollArea *scrollArea=new QScrollArea(this);
	scrollArea->setWidget(centralwidget);
	scrollArea->setWidgetResizable(true);
	setCentralWidget(scrollArea);
	setAttribute(Qt::WA_DeleteOnClose);
	QList<QWidget*> widgets=QObject::findChildren<QWidget*>();
	foreach (QWidget* widget, widgets){//turns all the text of tooltips into rich text so it linebreaks nicely.
		if(!widget->toolTip().isEmpty()){
			widget->setToolTip(QString("<qt>")+widget->toolTip());//could do ...+ QString("</qt>")
		}
	}
	activateTable();
	readGuiSpecificSettings();
	bool *pressedOpen = new bool; // a bool that indicates if the user has pressed the open project button
	*pressedOpen = false;

	QString fileNameOfOpenFile = QString(); // not to be confused with filename
	if(!filename.isEmpty()){//used on loading
		int r = QMessageBox::warning(this, QString("Overwrite settings"), QString("Do you want to overwrite the settings by loading the settings in the specified file?"),
						QMessageBox::Yes | QMessageBox::No);
		writeXmlToTree(filename,xmlTreeWidget);
		QDir dir(top_directoryPointer->text(1));
		dir.cd(input_directoryPointer->text(1));
		standard->StandardStrings::setinputPath(dir.path());
		if (r == QMessageBox::Yes) {
			writeSettings();
		}
		else{ 
			readSettings();
		}
		faciesGui();
		forwardGui();
		estimationGui();
		updateGuiToTree();
		if (r == QMessageBox::Yes) {
			setCurrentFile(filename);
		}
		
	}
	else {//used when not loading
		readSettings();
		if (!mode(false,pressedOpen, existing, fileNameOfOpenFile)){
			if(existing){//calling close on the first main window does not close the program
				setCurrentFile(filename);
				close();
				return;
			}
		}
		updateGuiToTree();
		setCurrentFile(filename);
		setDefaultValues();
	}

	if (*pressedOpen == true){
		if(!fileNameOfOpenFile.isEmpty()){
			int r = QMessageBox::warning(this, QString("Overwrite settings"), QString("Do you want to overwrite the settings by loading the settings in the specified file?"),
							QMessageBox::Yes | QMessageBox::No);
			writeXmlToTree(fileNameOfOpenFile, xmlTreeWidget);
			QDir dir(top_directoryPointer->text(1));
			dir.cd(input_directoryPointer->text(1));
			standard->StandardStrings::setinputPath(dir.path());
			if (r == QMessageBox::Yes) {
				writeSettings();
			}
			else{
				readSettings();
			}
			faciesGui();
			forwardGui();
			estimationGui();
			updateGuiToTree();
			if (r == QMessageBox::Yes) {
				setCurrentFile(fileNameOfOpenFile);
			}
		}
	}

	xmlTreeWidget->expandAll();
	xmlTreeWidget->resizeColumnToContents(0);
	xmlTreeWidget->resizeColumnToContents(1);
	xmlTreeWidget->setMinimumWidth(xmlTreeWidget->columnWidth(0)+xmlTreeWidget->columnWidth(1));//makes the treewidget not have to scroll all the time.
	tabWidget->setCurrentIndex(0);//survey information
	toolBox->setCurrentIndex(1);

	delete pressedOpen;
	
	QList<QObject*> fields = getNecessaryFields();
	foreach(QObject* field, fields){
	   field->installEventFilter(this);
	}
	if(stackListWidget->count()<1)	angleLineEdit->setStyleSheet("");
}

Main_crava::~Main_crava(){//qt automatically deletes all child widgets.
	delete standard;
}

void Main_crava::createActions(){
	//this is probably all editable from designer, not the standard sequences though...
	modeAction->setShortcut(QString("Ctrl+M"));
	newAction->setShortcut(QKeySequence::New);
	saveAction->setShortcut(QKeySequence::Save);
	openAction->setShortcut(QKeySequence::Open);
	quitAction->setShortcut(QString("Ctrl+Q"));
	runAction->setShortcut(QString("Ctrl+R"));
	manualAction->setShortcut(QString(QKeySequence::HelpContents));
	connect(quitAction, SIGNAL(triggered()), this, SLOT(close()));
	wellHeaderListWidget->setContextMenuPolicy(Qt::CustomContextMenu);
	connect(wellHeaderListWidget, SIGNAL(customContextMenuRequested(const QPoint&)), this, SLOT(showContextMenu(const QPoint&))); //right click on wellHeaderListWidget
}

void Main_crava::setupButtonGroups(){
	//survey
	//also sets up non-radiobuttons
	//can't check the buttons in the constructor since there is no seismic data stack yet.
	QButtonGroup *seismicStackType = new QButtonGroup(seismicDataFrame);
	seismicStackType->addButton(ppRadioButton);
	seismicStackType->addButton(psRadioButton);
	QButtonGroup *headerFormatInformation = new QButtonGroup(seismicDataFrame);
	headerFormatInformation->addButton(headerAutoDetectRadioButton);
	headerFormatInformation->addButton(headerSeisWorksRadioButton);
	headerFormatInformation->addButton(headerIesxRadioButton);
	headerFormatInformation->addButton(headerSipRadioButton);
	headerFormatInformation->addButton(headerCharismaRadioButton);
	headerFormatInformation->addButton(headerUserDefinedRadioButton);
	QButtonGroup *waveletType = new QButtonGroup(seismicDataFrame);
	waveletType->addButton(wavelet1DRadioButton);
	waveletType->addButton(wavelet3DRadioButton);
	wavelet1DRadioButton->setChecked(true);//this should be in update gui function, but only 1D is possible currently, when 3D is enabled it needs to be moved.
	wavelet3DRadioButton->setEnabled(false);
	wavelet3DFrame->setEnabled(false);
	wavelet3DFrame->setVisible(false);
	//wells
	QButtonGroup *vpFormat = new QButtonGroup(wellsTab);
	vpFormat->addButton(vpRadioButton);
	vpFormat->addButton(dtRadioButton);
	QButtonGroup *vsFormat = new QButtonGroup(wellsTab);
	vsFormat->addButton(vsRadioButton);
	vsFormat->addButton(dtsRadioButton);
	//horizon
	QButtonGroup *verticalInterval = new QButtonGroup(horizonsTab);
	verticalInterval->addButton(twoSurfaceRadioButton);
	verticalInterval->addButton(topSurfaceRadioButton);
	verticalInterval->addButton(baseSurfaceRadioButton);
	verticalInterval->addButton(correlationSurfaceRadioButton);
	verticalInterval->addButton(oneSurfaceRadioButton);
	verticalInterval->addButton(constantInversionRadioButton);
	QButtonGroup *lateralInterval = new QButtonGroup(horizonsTab);
	lateralInterval->addButton(areaSeismicRadioButton);
	lateralInterval->addButton(areaFileRadioButton);
	lateralInterval->addButton(areaUtmRadioButton);
	lateralInterval->addButton(areaInCrossRadioButton);
	//prior model
	QButtonGroup *vp1Prior = new QButtonGroup(vpVsRhoFrame);
	vp1Prior->addButton(vpConstant1RadioButton);
	vp1Prior->addButton(vpFile1RadioButton);
	QButtonGroup *vs1Prior = new QButtonGroup(vpVsRhoFrame);
	vs1Prior->addButton(vsConstant1RadioButton);
	vs1Prior->addButton(vsFile1RadioButton); 
	QButtonGroup *density1Prior = new QButtonGroup(vpVsRhoFrame);
	density1Prior->addButton(densityConstant1RadioButton);
	density1Prior->addButton(densityFile1RadioButton);
	QButtonGroup *density2Prior = new QButtonGroup(aiVpVsRhoFrame);
	density2Prior->addButton(densityConstant2RadioButton);
	density2Prior->addButton(densityFile2RadioButton);
	QButtonGroup *density3Prior = new QButtonGroup(aiSiRhoFrame);
	density3Prior->addButton(densityConstant3RadioButton);
	density3Prior->addButton(densityFile3RadioButton);
	//QCheckBox:checked:disabled{}; checked and disabled buttons shows as not checked. This is a bug in Qt, using a newer version to compile would fix it
	absoluteParametersCheckBox->setVisible(false);//Not wanted functionlity?
	absoluteParametersCheckBox->setEnabled(false);//Not wanted functionlity?
	matchEnergiesCheckBox->setVisible(false);//Not wanted functionlity
	matchEnergiesCheckBox->setEnabled(false);//Not wanted functionlity
	topSurfaceRadioButton->setVisible(false);//not working
	baseSurfaceRadioButton->setVisible(false);//not working
	topSurfaceRadioButton->setEnabled(false);//not working
	baseSurfaceRadioButton->setEnabled(false);//not working
	wellHeaderPushButton->setEnabled(false);//should only be clickable if there are any items in the well list widget.
}

void  Main_crava::readGuiSpecificSettings(){//makes the program remember how it looked like last time it was ran. Does not work correctly with 2 screens for xwin for some reason.
	QSettings settings("Statoil","CRAVA");
	settings.beginGroup("crava");
	settings.beginGroup("GUI");
	resize(settings.value("size", QSize(1280, 720)).toSize());
	move(settings.value("position", QPoint(0, 0)).toPoint());
	xmlTreeWidget->setVisible(settings.value(QString("showtree"),true).toBool());
	settings.beginGroup("tree");
	settings.beginGroup(io_settingsPointer->text(0));//io-settings
	//directories , not output directory
	top_directoryPointer->setText(1,settings.value(top_directoryPointer->text(0),QString("")).toString());
	input_directoryPointer->setText(1,settings.value(input_directoryPointer->text(0),QString("")).toString());
	//directories here so having multiple windows open does not potentially cause trouble.
	settings.endGroup();
	settings.endGroup();
}

void Main_crava::readSettings(){
	//second argument is default
	//could have been done dynamically by searching for same name then putting it in there.
	QSettings settings("Statoil","CRAVA");
	settings.beginGroup("crava");
	settings.beginGroup("GUI");
	settings.beginGroup("tree");
	//iterate over all the various options that can be in settings, if the values in the tree are non-empty and different
	//project-settings
	settings.beginGroup(io_settingsPointer->text(0));//io-settings
	settings.beginGroup(grid_outputPointer->text(0));//grid-output
	settings.beginGroup(grid_output_formatPointer->text(0));//format
	if(!forwardMode()){
		settings.beginGroup(segy_formatPointer->text(0));//segy-format
		for(int j=0;j<segy_formatPointer->childCount();++j){
			segy_formatPointer->child(j)->setText(1,settings.value(segy_formatPointer->child(j)->text(0),QString("")).toString());
		}
		settings.endGroup();//end segy-format
	}
	//grid format
	format_segyPointer->setText(1,settings.value(format_segyPointer->text(0),QString("")).toString());
	format_stormPointer->setText(1,settings.value(format_stormPointer->text(0),QString("")).toString());
	format_cravaPointer->setText(1,settings.value(format_cravaPointer->text(0),QString("")).toString());
	format_sgriPointer->setText(1,settings.value(format_sgriPointer->text(0),QString("")).toString());
	format_asciiPointer->setText(1,settings.value(format_asciiPointer->text(0),QString("")).toString());
	settings.endGroup();//end format
	settings.endGroup();//end grid-output
	if(!forwardMode()){
		settings.beginGroup(well_outputPointer->text(0));//well-output
		settings.beginGroup(well_output_formatPointer->text(0));//format

		well_output_norsarPointer->setText(1,settings.value(well_output_norsarPointer->text(0),QString("")).toString()); //why only norsar??

		settings.endGroup();//end format
		settings.endGroup();//end well-output
		settings.beginGroup(wavelet_outputPointer->text(0));//wavelet-output
		settings.beginGroup(wavelet_output_formatPointer->text(0));//format

		wavelet_output_norsarPointer->setText(1,settings.value(wavelet_output_norsarPointer->text(0),QString("")).toString());

		settings.endGroup();//end format
		settings.endGroup();//end wavelet-output
	}
	io_settings_log_levelPointer->setText(1,settings.value(io_settings_log_levelPointer->text(0),QString("")).toString());//log-level
	settings.endGroup();//end io-settings
	if(!forwardMode()){
		settings.beginGroup(advanced_settingsPointer->text(0));//advanced-settings
		if(estimationMode()){
		        vp_vs_ratioPointer->setText(1,settings.value(vp_vs_ratioPointer->text(0),QString("")).toString());
			vp_vs_ratio_from_wellsPointer->setText(1,settings.value(vp_vs_ratio_from_wellsPointer->text(0),QString("")).toString());
			high_cut_seismic_resolutionPointer->setText(1,settings.value(high_cut_seismic_resolutionPointer->text(0),QString("")).toString());
			energy_tresholdPointer->setText(1,settings.value(energy_tresholdPointer->text(0),QString("")).toString());
			wavelet_tapering_lengthPointer->setText(1,settings.value(wavelet_tapering_lengthPointer->text(0),QString("")).toString());
			minimum_relative_wavelet_amplitudePointer->setText(1,settings.value(minimum_relative_wavelet_amplitudePointer->text(0),QString("")).toString());
			maximum_wavelet_shiftPointer->setText(1,settings.value(maximum_wavelet_shiftPointer->text(0),QString("")).toString());
			white_noise_component_cutPointer->setText(1,settings.value(white_noise_component_cutPointer->text(0),QString("")).toString());
			//reflection matrix should not be written, relative path...
			kriging_data_limitPointer->setText(1,settings.value(kriging_data_limitPointer->text(0),QString("")).toString());
		       	guard_zonePointer->setText(1,settings.value(guard_zonePointer->text(0),QString("")).toString());
			debug_levelPointer->setText(1,settings.value(debug_levelPointer->text(0),QString("")).toString());
			smooth_kriged_parametersPointer->setText(1,settings.value(smooth_kriged_parametersPointer->text(0),QString("")).toString());
		}
		else{
			x_fractionPointer->setText(1,settings.value(x_fractionPointer->text(0),QString("")).toString());
			y_fractionPointer->setText(1,settings.value(y_fractionPointer->text(0),QString("")).toString());
			z_fractionPointer->setText(1,settings.value(z_fractionPointer->text(0),QString("")).toString());
			use_intermediate_disk_storagePointer->setText(1,settings.value(use_intermediate_disk_storagePointer->text(0),QString("")).toString());
		        vp_vs_ratioPointer->setText(1,settings.value(vp_vs_ratioPointer->text(0),QString("")).toString());
			vp_vs_ratio_from_wellsPointer->setText(1,settings.value(vp_vs_ratio_from_wellsPointer->text(0),QString("")).toString());
			maximum_relative_thickness_differencePointer->setText(1,settings.value(maximum_relative_thickness_differencePointer->text(0),QString("")).toString());
			frequency_band_low_cutPointer->setText(1,settings.value(frequency_band_low_cutPointer->text(0),QString("")).toString());
			frequency_band_high_cutPointer->setText(1,settings.value(frequency_band_high_cutPointer->text(0),QString("")).toString());
			high_cut_seismic_resolutionPointer->setText(1,settings.value(high_cut_seismic_resolutionPointer->text(0),QString("")).toString());
			energy_tresholdPointer->setText(1,settings.value(energy_tresholdPointer->text(0),QString("")).toString());
			wavelet_tapering_lengthPointer->setText(1,settings.value(wavelet_tapering_lengthPointer->text(0),QString("")).toString());
			minimum_relative_wavelet_amplitudePointer->setText(1,settings.value(minimum_relative_wavelet_amplitudePointer->text(0),QString("")).toString());
			maximum_wavelet_shiftPointer->setText(1,settings.value(maximum_wavelet_shiftPointer->text(0),QString("")).toString());
			white_noise_component_cutPointer->setText(1,settings.value(white_noise_component_cutPointer->text(0),QString("")).toString());
			//reflection matrix should not be written, relative path...
			kriging_data_limitPointer->setText(1,settings.value(kriging_data_limitPointer->text(0),QString("")).toString());
		       	guard_zonePointer->setText(1,settings.value(guard_zonePointer->text(0),QString("")).toString());
			debug_levelPointer->setText(1,settings.value(debug_levelPointer->text(0),QString("")).toString());
			smooth_kriged_parametersPointer->setText(1,settings.value(smooth_kriged_parametersPointer->text(0),QString("")).toString());
		}

		settings.endGroup();//end advanced settings
		settings.beginGroup(allowed_parameter_valuesPointer->text(0));//allowed-parameter-values
		for(int j=0;j<allowed_parameter_valuesPointer->childCount();++j){
			allowed_parameter_valuesPointer->child(j)->setText(1,settings.value(allowed_parameter_valuesPointer->child(j)->text(0),QString("")).toString());
		}

		maximum_deviation_anglePointer->setText(1,settings.value(maximum_deviation_anglePointer->text(0),QString("")).toString());
		maximum_rank_correlationPointer->setText(1,settings.value(maximum_rank_correlationPointer->text(0),QString("")).toString());
		maximum_merge_distancePointer->setText(1,settings.value(maximum_merge_distancePointer->text(0),QString("")).toString());
		maximum_offsetPointer->setText(1,settings.value(maximum_offsetPointer->text(0),QString("")).toString());
		maximum_shiftPointer->setText(1,settings.value(maximum_shiftPointer->text(0),QString("")).toString());
		settings.endGroup();
	}
	settings.endGroup();//end tree
	settings.endGroup();//end GUI
	settings.endGroup();//end crava
}
void Main_crava::writeSettings(){
	QSettings settings("Statoil","CRAVA");
	settings.beginGroup("crava");
	settings.beginGroup("GUI");
	settings.setValue( QString("openProject"), currentFile() );
	settings.setValue("size", size());
	settings.setValue("position", pos());
	settings.beginGroup("tree");
	//iterate over all the various options that can be in settings, if the values in the tree are non-empty and different
	settings.beginGroup(io_settingsPointer->text(0));//io-settings

	settings.setValue(top_directoryPointer->text(0),top_directoryPointer->text(1));
	settings.setValue(input_directoryPointer->text(0),input_directoryPointer->text(1));

	settings.beginGroup(grid_outputPointer->text(0));//grid-output
	settings.beginGroup(grid_output_formatPointer->text(0));//format
	settings.beginGroup(segy_formatPointer->text(0));//segy-format
	for(int j=0;j<segy_formatPointer->childCount();++j){
		settings.setValue(segy_formatPointer->child(j)->text(0), segy_formatPointer->child(j)->text(1));
	}
	settings.endGroup();//end segy-format

	settings.setValue(format_segyPointer->text(0), format_segyPointer->text(1));
	settings.setValue(format_stormPointer->text(0), format_stormPointer->text(1));
	settings.setValue(format_cravaPointer->text(0), format_cravaPointer->text(1));
	settings.setValue(format_sgriPointer->text(0), format_sgriPointer->text(1));
	settings.setValue(format_asciiPointer->text(0), format_asciiPointer->text(1));

	settings.endGroup();//end format
	settings.endGroup();//end grid-output
	if(!forwardMode()){
		settings.beginGroup(well_outputPointer->text(0));//well-output
		settings.beginGroup(well_output_formatPointer->text(0));//format
		settings.setValue(well_output_norsarPointer->text(0), well_output_norsarPointer->text(1));

		settings.endGroup();//end format
		settings.endGroup();//end well-output
		settings.beginGroup(wavelet_outputPointer->text(0));//wavelet-output
		settings.beginGroup(wavelet_output_formatPointer->text(0));//format
		settings.setValue(wavelet_output_norsarPointer->text(0),wavelet_output_norsarPointer->text(1));
		settings.endGroup();//end format
		settings.endGroup();//end wavelet-output
	}
	settings.setValue(io_settings_log_levelPointer->text(0),io_settings_log_levelPointer->text(1));//log-level
	settings.endGroup();//end io-settings
	if(!forwardMode()){
		settings.beginGroup(advanced_settingsPointer->text(0));//advanced-settings
		if(estimationMode()){
		        settings.setValue(vp_vs_ratioPointer->text(0),vp_vs_ratioPointer->text(1));
			settings.setValue(vp_vs_ratio_from_wellsPointer->text(0),vp_vs_ratio_from_wellsPointer->text(1));
			settings.setValue(high_cut_seismic_resolutionPointer->text(0),high_cut_seismic_resolutionPointer->text(1));
			settings.setValue(energy_tresholdPointer->text(0),energy_tresholdPointer->text(1));
			settings.setValue(wavelet_tapering_lengthPointer->text(0),wavelet_tapering_lengthPointer->text(1));
			settings.setValue(minimum_relative_wavelet_amplitudePointer->text(0),minimum_relative_wavelet_amplitudePointer->text(1));
			settings.setValue(maximum_wavelet_shiftPointer->text(0),maximum_wavelet_shiftPointer->text(1));
			settings.setValue(white_noise_component_cutPointer->text(0),white_noise_component_cutPointer->text(1));
			//reflection matrix should not be written, relative path...
			settings.setValue(kriging_data_limitPointer->text(0),kriging_data_limitPointer->text(1));
		       	settings.setValue(guard_zonePointer->text(0),guard_zonePointer->text(1));
			settings.setValue(debug_levelPointer->text(0),debug_levelPointer->text(1));
			settings.setValue(smooth_kriged_parametersPointer->text(0),smooth_kriged_parametersPointer->text(1));
		}
		else{
			settings.setValue(x_fractionPointer->text(0),x_fractionPointer->text(1));
			settings.setValue(y_fractionPointer->text(0),y_fractionPointer->text(1));
			settings.setValue(z_fractionPointer->text(0),z_fractionPointer->text(1));
			settings.setValue(use_intermediate_disk_storagePointer->text(0),use_intermediate_disk_storagePointer->text(1));
			settings.setValue(vp_vs_ratioPointer->text(0),vp_vs_ratioPointer->text(1));
			settings.setValue(vp_vs_ratio_from_wellsPointer->text(0),vp_vs_ratio_from_wellsPointer->text(1));
			settings.setValue(maximum_relative_thickness_differencePointer->text(0),maximum_relative_thickness_differencePointer->text(1));
			settings.setValue(frequency_band_low_cutPointer->text(0),frequency_band_low_cutPointer->text(1));
			settings.setValue(frequency_band_high_cutPointer->text(0),frequency_band_high_cutPointer->text(1));
		       	settings.setValue(high_cut_seismic_resolutionPointer->text(0),high_cut_seismic_resolutionPointer->text(1));
			settings.setValue(energy_tresholdPointer->text(0),energy_tresholdPointer->text(1));
			settings.setValue(wavelet_tapering_lengthPointer->text(0),wavelet_tapering_lengthPointer->text(1));
			settings.setValue(minimum_relative_wavelet_amplitudePointer->text(0),minimum_relative_wavelet_amplitudePointer->text(1));
			settings.setValue(maximum_wavelet_shiftPointer->text(0),maximum_wavelet_shiftPointer->text(1));
			settings.setValue(white_noise_component_cutPointer->text(0),white_noise_component_cutPointer->text(1));
			//reflection matrix should not be written, relative path...
			settings.setValue(kriging_data_limitPointer->text(0),kriging_data_limitPointer->text(1));
		       	settings.setValue(guard_zonePointer->text(0),guard_zonePointer->text(1));
			settings.setValue(debug_levelPointer->text(0),debug_levelPointer->text(1));
			settings.setValue(smooth_kriged_parametersPointer->text(0),smooth_kriged_parametersPointer->text(1));
		}

		settings.endGroup();

		settings.beginGroup(allowed_parameter_valuesPointer->text(0));//allowed-parameter-values
		for(int j=0;j<allowed_parameter_valuesPointer->childCount();++j){
			settings.setValue(allowed_parameter_valuesPointer->child(j)->text(0),allowed_parameter_valuesPointer->child(j)->text(1));
		}
		settings.setValue(maximum_deviation_anglePointer->text(0),maximum_deviation_anglePointer->text(1));
		settings.setValue(maximum_rank_correlationPointer->text(0),maximum_rank_correlationPointer->text(1));
		settings.setValue(maximum_merge_distancePointer->text(0),maximum_merge_distancePointer->text(1));
		settings.setValue(maximum_offsetPointer->text(0),maximum_offsetPointer->text(1));
		settings.setValue(maximum_shiftPointer->text(0),maximum_shiftPointer->text(1));

		settings.endGroup();
	}
	settings.endGroup();
	settings.endGroup();
	settings.endGroup();
}

void Main_crava::updateGuiToTree(){
	//survey
	defaultStartTimeLineEdit->setText(survey_segy_start_timePointer->text(1));//segy-start-time
	//angle-gather already handled by the slot
	waveletTopLineEdit->setText(survey_top_surface_filePointer->text(1));//wavelet estimation interval
	waveletBottomLineEdit->setText(survey_base_surface_filePointer->text(1));
	//well-data
	//log names
	timeLineEdit->setText(log_names_timePointer->text(1));
	densityLineEdit->setText(log_names_densityPointer->text(1));
	faciesLineEdit->setText(log_names_faciesPointer->text(1));
	if(!log_names_dtPointer->text(1).isEmpty()){
		dtRadioButton->setChecked(true);
		dtLineEdit->setText(log_names_dtPointer->text(1));
		on_vpRadioButton_toggled(false);
	}
	else{//default
		vpRadioButton->setChecked(true);
		vpLineEdit->setText(log_names_vpPointer->text(1));
	}
	if(!log_names_dtsPointer->text(1).isEmpty()){
		dtsRadioButton->setChecked(true);
		dtsLineEdit->setText(log_names_dtsPointer->text(1));
		on_vsRadioButton_toggled(false);
	}
	else{//default
		vsRadioButton->setChecked(true);
		vsLineEdit->setText(log_names_vsPointer->text(1));
	}

	//the huge nested ifs check which vertical inversion interval frames should be visible.
	if((!top_surface_time_valuePointer->text(1).isEmpty()) ||
	 (!base_surface_time_valuePointer->text(1).isEmpty())){//constant
		constantInversionRadioButton->setChecked(true);
		topTimeValueLineEdit->setText(top_surface_time_valuePointer->text(1));
		bottomTimeValueLineEdit->setText(base_surface_time_valuePointer->text(1));
		on_oneSurfaceRadioButton_toggled(false);
	}
	else if((!(interval_one_surface_reference_surfacePointer->text(1).isEmpty())) ||
			 (!(interval_one_surface_shift_to_interval_topPointer->text(1).isEmpty() )) ||
			 (!(interval_one_surface_thicknessPointer->text(1).isEmpty())) ||
			 (!(interval_one_surface_sample_densityPointer->text(1).isEmpty()))){
			on_constantInversionRadioButton_toggled(false);
			oneSurfaceRadioButton->setChecked(true);
			referenceSurfaceFileLineEdit->setText(interval_one_surface_reference_surfacePointer->text(1));
			distanceTopLineEdit->setText(interval_one_surface_shift_to_interval_topPointer->text(1));
			thicknessLineEdit->setText(interval_one_surface_thicknessPointer->text(1));
			layerThicknessLineEdit->setText(interval_one_surface_sample_densityPointer->text(1));
		}
	else{
		topTimeFileLineEdit->setText(top_surface_time_filePointer->text(1));
		bottomTimeFileLineEdit->setText(base_surface_time_filePointer->text(1));

		if(!top_surface_depth_filePointer->text(1).isEmpty()||
		!base_surface_depth_filePointer->text(1).isEmpty()){
			depthSurfacesCheckBox->setChecked(true);
			topDepthFileLineEdit->setText(top_surface_depth_filePointer->text(1));
			bottomDepthFileLineEdit->setText(base_surface_depth_filePointer->text(1));
		}
		else{
			depthSurfacesCheckBox->setChecked(false);
			on_depthSurfacesCheckBox_toggled(false);//this should work by default...
		}
		//the depth is checked first because it is cleared if the other buttons are checked..
		on_oneSurfaceRadioButton_toggled(false);
		on_constantInversionRadioButton_toggled(false);
		if(top_surface_time_filePointer->text(1).isEmpty()){//top
			if(base_surface_time_filePointer->text(1).isEmpty()){
				twoSurfaceRadioButton->setChecked(true);
			}
			else{
				baseSurfaceRadioButton->setChecked(true);
			}
		}
		else if(base_surface_time_filePointer->text(1).isEmpty()){//bottom
			topSurfaceRadioButton->setChecked(true);
		}
		else{
			twoSurfaceRadioButton->setChecked(true);
		}
	}
	layersLineEdit->setText( interval_two_surfaces_number_of_layersPointer->text(1) );
	if(!interval_two_surfaces_velocity_fieldPointer->text(1).isEmpty()){
		velocityFieldFileRadioButton->setChecked(true);
		velocityFieldLineEdit->setText(interval_two_surfaces_velocity_fieldPointer->text(1));
	}
	else if(interval_two_surfaces_velocity_field_from_inversionPointer->text(1)==QString("yes")){
		velocityFieldInvesionRadioButton->setChecked(true);
	}
	else {//default
		velocityFieldNoneRadioButton->setChecked(true);
	}
	//prior-model
	//zone list is handled by the reading of the tree

	if(zoneListWidget->count()>1 || !background_top_surface_filePointer->text(1).isEmpty() || !background_top_surface_erosion_priorityPointer->text(1).isEmpty()){ //checks if the multizone background model radio button should be checked.
	        multizoneBackgroundRadioButton->setChecked(true);
		topSurfaceFileLineEdit->setText(background_top_surface_filePointer->text(1));
		topPrioritySpinBox->setValue(background_top_surface_erosion_priorityPointer->text(1).toInt());
	}
	else if(background_vs_filePointer->text(1).isEmpty() && background_vp_filePointer->text(1).isEmpty() && background_density_filePointer->text(1).isEmpty() && background_ai_filePointer->text(1).isEmpty() && background_si_filePointer->text(1).isEmpty() && background_vp_vs_ratio_filePointer->text(1).isEmpty() && background_vp_constantPointer->text(1).isEmpty() && background_vs_constantPointer->text(1).isEmpty() && background_density_constantPointer->text(1).isEmpty()){//checks whether the estimate background model radio button should be checked or not.	

	  estimateBackgroundRadioButton->setChecked(true);
	  if(background_velocity_fieldPointer->text(1).isEmpty() && background_high_cut_background_modellingPointer->text(1).isEmpty()){
	    backgroundEstimatedConfigurationCheckBox->setChecked(false);
	    velocityFieldLabel->setVisible(false);
	    velocityFieldPriorFileLineEdit->setVisible(false);
	    velocityFieldPriorFileBrowsePushButton->setVisible(false);
	    lateralCorrelationLabel->setVisible(false);
	    lateralCorrelationBackgroundPushButton->setVisible(false);
	    highCutFrequencyLabel->setVisible(false);
	    highCutFrequencyLineEdit->setVisible(false);
	    hzLabel->setVisible(false);
	  }
	  else{
	    backgroundEstimatedConfigurationCheckBox->setChecked(true);
	    velocityFieldPriorFileLineEdit->setText(background_velocity_fieldPointer->text(1));
	    highCutFrequencyLineEdit->setText(background_high_cut_background_modellingPointer->text(1));
	  }
	  
	}
	else{
	  backgroundRadioButton->setChecked(true);
	  if(!background_vp_filePointer->text(1).isEmpty()){
	    vpVsRhoRadioButton->setChecked(true);
	    vpFile1RadioButton->setChecked(true);
	    vpFile1LineEdit->setText(background_vp_filePointer->text(1));
	  }
	  else if(!background_vp_constantPointer->text(1).isEmpty()){
	    vpVsRhoRadioButton->setChecked(true);
	    vpConstant1RadioButton->setChecked(true);
	    vpConstant1LineEdit->setText(background_vp_constantPointer->text(1));
	  }
	  if(!background_vs_filePointer->text(1).isEmpty()){
	    vpVsRhoRadioButton->setChecked(true);
	    vsFile1RadioButton->setChecked(true);
	    vsFile1LineEdit->setText(background_vs_filePointer->text(1));
	  }
	  else if(!background_vs_constantPointer->text(1).isEmpty()){
	    vpVsRhoRadioButton->setChecked(true);
	    vsConstant1RadioButton->setChecked(true);
	    vsConstant1LineEdit->setText(background_vs_constantPointer->text(1));
	  }
	  if(!background_vp_vs_ratio_filePointer->text(1).isEmpty()){
	    aiVpVsRhoRadioButton->setChecked(true);
	    vpVsFile2LineEdit->setText(background_vp_vs_ratio_filePointer->text(1));
	  }
	  if(!background_si_filePointer->text(1).isEmpty()){
	    aiSiRhoRadioButton->setChecked(true);
	    siFile3LineEdit->setText(background_si_filePointer->text(1));
	  }
	  if(!background_ai_filePointer->text(1).isEmpty()){
	    if(aiSiRhoRadioButton->isChecked()){
	      aiFile3LineEdit->setText(background_ai_filePointer->text(1));
	    }
	    else{
	      aiVpVsRhoRadioButton->setChecked(true);
	      aiFile2LineEdit->setText(background_ai_filePointer->text(1));
	    }
	  }
	  if(!background_density_filePointer->text(1).isEmpty()){
	    if(aiSiRhoRadioButton->isChecked()){
	      densityFile3RadioButton->setChecked(true);
	      densityFile3LineEdit->setText(background_density_filePointer->text(1));	
	    }
	    else if(aiVpVsRhoRadioButton->isChecked()){
	      densityFile2RadioButton->setChecked(true);
	      densityFile2LineEdit->setText(background_density_filePointer->text(1));
	    }
	    else{
	      vpVsRhoRadioButton->setChecked(true);
	      densityFile1RadioButton->setChecked(true);
	      densityFile1LineEdit->setText(background_density_filePointer->text(1));
	    }
	  }
	  else if(!background_density_constantPointer->text(1).isEmpty()){
	    if(aiSiRhoRadioButton->isChecked()){
	      densityConstant3RadioButton->setChecked(true);
	      densityConstant3LineEdit->setText(background_density_constantPointer->text(1));
	    }
	    else if(aiVpVsRhoRadioButton->isChecked()){
	      densityConstant2RadioButton->setChecked(true);
	      densityConstant2LineEdit->setText(background_density_constantPointer->text(1));
	    }
	    else{
	      vpVsRhoRadioButton->setChecked(true);
	      densityConstant1RadioButton->setChecked(true);
	      densityConstant1LineEdit->setText(background_density_constantPointer->text(1));
	    }
	  }
	}
	  
	 
	//correlation variograms handled by the appropriate dialogs, the checkboxes must be handled.
	bool modified=false;
	for(int i=0;i<local_wavelet_lateral_correlationPointer->childCount();++i){//local wavelet variogram background
		if(!local_wavelet_lateral_correlationPointer->child(i)->text(1).isEmpty()){
			modified=true;
		}
	}
	correlationLocalWaveletCheckBox->setChecked(modified);
	on_correlationLocalWaveletCheckBox_toggled(modified);
	modified=false;
	for(int i=0;i<prior_model_lateral_correlationPointer->childCount();++i){//lateral correlation parameters background.
		if(!prior_model_lateral_correlationPointer->child(i)->text(1).isEmpty()){
			modified=true;
		}
	}

	temporalCorrelationLineEdit->setText(temporal_correlationPointer->text(1));
	parameterCorrelationLineEdit->setText(parameter_correlationPointer->text(1));
	if(!temporal_correlationPointer->text(1).isEmpty()||!parameter_correlationPointer->text(1).isEmpty()){
		modified=true;
	}
	correlationElasticParametersCheckBox->setChecked(modified);
	on_correlationElasticParametersCheckBox_toggled(modified);
	if(!correlation_directionPointer->text(1).isEmpty()){//correlation-direction
		correlationSurfaceRadioButton->setChecked(true);
		correlationDirectionFileLineEdit->setText(correlation_directionPointer->text(1));
	}
	else{
		on_correlationSurfaceRadioButton_toggled(false); // hide the inputs if the button is not checked.
	}
	//facies list is handled by the reading to tree
	//facies-probabilities=7
	if(faciesProbabilitiesOn()){
		if(faciesListWidget->count()<=1){//default
			faciesEstimateCheckBox->setChecked(true);
		}
		if(facies_probabilities_use_vsPointer->text(1)!=QString("no")){
			vsForFaciesCheckBox->setChecked(true);//default
		}
		if(facies_probabilities_use_predictionPointer->text(1)==QString("yes")){
			predictionFaciesCheckBox->setChecked(true);//not-default
		}
		if(facies_probabilities_use_absolute_elastic_parametersPointer->text(1)==QString("yes")){
			absoluteParametersCheckBox->setChecked(true);//not-default
		}
		faciesTopLineEdit->setText(facies_probabilities_top_surface_filePointer->text(1));
		faciesBottomLineEdit->setText(facies_probabilities_base_surface_filePointer->text(1));
		uncertaintyLevelLineEdit->setText(uncertainty_levelPointer->text(1));
	}
       	if(!earth_model_vp_filePointer->text(1).isEmpty()){
	        vpComboBox->setCurrentIndex(0);
	        earthVpAiLineEdit->setText(earth_model_vp_filePointer->text(1));//earth model
        }
        else if(!earth_model_ai_filePointer->text(1).isEmpty()){
	        vpComboBox->setCurrentIndex(1);
	        earthVpAiLineEdit->setText(earth_model_ai_filePointer->text(1));//earth model
	}
	else{
	  	vpComboBox->setCurrentIndex(0);
	}
	if(!earth_model_vs_filePointer->text(1).isEmpty()){
	        vsComboBox->setCurrentIndex(0);
	        earthVsSiVpVsLineEdit->setText(earth_model_vs_filePointer->text(1));//earth model
        }
        else if(!earth_model_si_filePointer->text(1).isEmpty()){
	        vsComboBox->setCurrentIndex(1);
	        earthVsSiVpVsLineEdit->setText(earth_model_si_filePointer->text(1));//earth model
	}
	else if(!earth_model_vp_vs_ratio_filePointer->text(1).isEmpty()){
	        vsComboBox->setCurrentIndex(2);
	        earthVsSiVpVsLineEdit->setText(earth_model_vp_vs_ratio_filePointer->text(1));//earth model
	}
	else{
	        vsComboBox->setCurrentIndex(0);
	}
        densityComboBox->setCurrentIndex(0);
	earthDensityLineEdit->setText(earth_model_density_filePointer->text(1));//earth model
	//project-settings inversion-area
	//ifs check which frames should be visible.
	//output-volume
	if(!area_from_surface_file_namePointer->text(1).isEmpty() || area_from_surface_snap_to_seismic_dataPointer->text(1)==QString("yes")){//area-from-surface
		areaFileRadioButton->setChecked(true);
		areaSurfaceLineEdit->setText(area_from_surface_file_namePointer->text(1));
		surfaceSnapCheckBox->setChecked(StandardStrings::checkedBool(area_from_surface_snap_to_seismic_dataPointer->text(1)));
		on_areaUtmRadioButton_toggled(false);
		on_areaInCrossRadioButton_toggled(false);
	}
	else if(!utm_coordinates_reference_point_xPointer->text(1).isEmpty() || !utm_coordinates_reference_point_yPointer->text(1).isEmpty() 
	|| !utm_coordinates_length_xPointer->text(1).isEmpty() || !utm_coordinates_length_yPointer->text(1).isEmpty()
	|| !utm_coordinates_sample_density_xPointer->text(1).isEmpty() || !utm_coordinates_sample_density_yPointer->text(1).isEmpty()
	|| !utm_coordinates_anglePointer->text(1).isEmpty() || utm_coordinates_snap_to_seismic_dataPointer->text(1)==QString("yes")){//utm-coordinates
		areaUtmRadioButton->setChecked(true);
		on_areaInCrossRadioButton_toggled(false);
		on_areaFileRadioButton_toggled(false);
		areaXRefLineEdit->setText(utm_coordinates_reference_point_xPointer->text(1));
		areaYRefLineEdit->setText(utm_coordinates_reference_point_yPointer->text(1));
		areaXLengthLineEdit->setText(utm_coordinates_length_xPointer->text(1));
		areaYLengthLineEdit->setText(utm_coordinates_length_yPointer->text(1));
		areaXSampleDensityLineEdit->setText(utm_coordinates_sample_density_xPointer->text(1));
		areaYSampleDensityLineEdit->setText(utm_coordinates_sample_density_yPointer->text(1));
		areaUtmAngleLineEdit->setText(utm_coordinates_anglePointer->text(1));
		utmSnapCheckBox->setChecked(StandardStrings::checkedBool(utm_coordinates_snap_to_seismic_dataPointer->text(1)));
	}//inline-crossline-numbers
	else if(!il_startPointer->text(1).isEmpty() || !il_endPointer->text(1).isEmpty() 
	|| !xl_startPointer->text(1).isEmpty() || !xl_endPointer->text(1).isEmpty()
	|| !il_stepPointer->text(1).isEmpty() || !xl_stepPointer->text(1).isEmpty()){
		areaInCrossRadioButton->setChecked(true);
		on_areaFileRadioButton_toggled(false);
		on_areaUtmRadioButton_toggled(false);
		inlineStartLineEdit->setText(il_startPointer->text(1));
		inlineEndLineEdit->setText(il_endPointer->text(1));
		crosslineStartLineEdit->setText(xl_startPointer->text(1));
		crosslineEndLineEdit->setText(xl_endPointer->text(1));
		inlineStepLineEdit->setText(il_stepPointer->text(1));
		crosslineStepLineEdit->setText(xl_stepPointer->text(1));
	}
	else {//default is to use area from seismic
		areaSeismicRadioButton->setChecked(true);
	}
       	//Everything in the output-tab
	if((!forwardMode())&&(!estimationMode())){
	        toolBox->setVisible(true);
		toolBox->setEnabled(true);
		oDomainDepthCheckBox->setChecked(StandardStrings::checkedBool(grid_output_depthPointer->text(1)));//depth
		oDomainTimeCheckBox->setChecked(StandardStrings::checkedBool(grid_output_timePointer->text(1),QString("yes")));//time

		oSeismicOriginalCheckBox->setChecked(StandardStrings::checkedBool(seismic_data_originalPointer->text(1)));//original
		oSeismicSyntheticCheckBox->setChecked(StandardStrings::checkedBool(seismic_data_syntheticPointer->text(1)));//synthetic
		oSeismicResidualCheckBox->setChecked(StandardStrings::checkedBool(seismic_data_residualsPointer->text(1)));//residual
		oSeismicSyntheticResidualCheckBox->setChecked(StandardStrings::checkedBool(seismic_data_synthetic_residualsPointer->text(1)));//synthetic residual

		oVpCheckBox->setChecked(StandardStrings::checkedBool(elastic_parameters_vpPointer->text(1),QString("yes")));//vp
		oVsCheckBox->setChecked(StandardStrings::checkedBool(elastic_parameters_vsPointer->text(1),QString("yes")));//vs
		oDensityCheckBox->setChecked(StandardStrings::checkedBool(elastic_parameters_densityPointer->text(1),QString("yes")));//density
		oLameLamCheckBox->setChecked(StandardStrings::checkedBool(elastic_parameters_lame_lambdaPointer->text(1)));//lame-lambda
		oLameMuCheckBox->setChecked(StandardStrings::checkedBool(elastic_parameters_lame_muPointer->text(1)));//lame-mu
		oPoissonRatioCheckBox->setChecked(StandardStrings::checkedBool(elastic_parameters_poisson_ratioPointer->text(1)));//poisson-ratio
		oAiCheckBox->setChecked(StandardStrings::checkedBool(elastic_parameters_aiPointer->text(1)));//ai
		oSiCheckBox->setChecked(StandardStrings::checkedBool(elastic_parameters_siPointer->text(1)));//si
		oVpVsRatioCheckBox->setChecked(StandardStrings::checkedBool(elastic_parameters_vp_vs_ratioPointer->text(1)));//vp-vs-ratio
		oLambdaRhoCheckBox->setChecked(StandardStrings::checkedBool(elastic_parameters_murhoPointer->text(1)));//murho
		oMuRhoCheckBox->setChecked(StandardStrings::checkedBool(elastic_parameters_lambdarhoPointer->text(1)));//lambdarho
		oBackgroundCheckBox->setChecked(StandardStrings::checkedBool(elastic_parameters_backgroundPointer->text(1)));//background
		oBackgroundTrendCheckBox->setChecked(StandardStrings::checkedBool(elastic_parameters_background_trendPointer->text(1)));//background-trend

		if(faciesProbabilitiesOn()){
		        oFaciesProbabilitiesCheckBox->setChecked(StandardStrings::checkedBool(grid_output_facies_probabilitiesPointer->text(1),QString("yes")));
			oFaciesProbabilitiesUndefinedCheckBox->setChecked(StandardStrings::checkedBool(grid_output_facies_probabilities_with_undefPointer->text(1)));//facies-probabilities-with-undefined
			oFaciesLikelihoodCheckBox->setChecked(StandardStrings::checkedBool(grid_output_facies_likelihoodPointer->text(1)));//facies-likelihood
			oFaciesQualityGridCheckBox->setChecked(StandardStrings::checkedBool(grid_output_seismic_quality_gridPointer->text(1)));//seismic-quality grid
			oRockPhysicsCheckBox->setChecked(StandardStrings::checkedBool(io_settings_rock_physics_distributionsPointer->text(1)));//rock-physics-distributions
		}
		else{
			oFaciesProbabilitiesCheckBox->setVisible(false);
			oFaciesProbabilitiesUndefinedCheckBox->setVisible(false);
			oFaciesLikelihoodCheckBox->setVisible(false);
			oFaciesQualityGridCheckBox->setVisible(false);
			oRockPhysicsCheckBox->setVisible(false);
			oFaciesProbabilitiesCheckBox->setEnabled(false);
			oFaciesProbabilitiesUndefinedCheckBox->setEnabled(false);
			oFaciesLikelihoodCheckBox->setEnabled(false);
		       	oFaciesQualityGridCheckBox->setEnabled(false);
			oRockPhysicsCheckBox->setEnabled(false);
		}
		oTimeDepthCheckBox->setChecked(StandardStrings::checkedBool(grid_output_time_to_depth_velocityPointer->text(1)));//time-to-depth-velocity
		oExtraGridsCheckBox->setChecked(StandardStrings::checkedBool(grid_output_extra_gridsPointer->text(1)));//extra-grids
		oCorrelationsCheckBox->setChecked(StandardStrings::checkedBool(grid_output_correlationsPointer->text(1)));//correlations

		//well-output
		oWellCheckBox->setChecked(StandardStrings::checkedBool(well_output_wellsPointer->text(1)));//wells
		oBlockedWellCheckBox->setChecked(StandardStrings::checkedBool(well_output_blocked_wellsPointer->text(1)));//blocked-wells
		//blocke-logs does nothing atm

		//wavelet-output
		oWaveletWellCheckBox->setChecked(StandardStrings::checkedBool(wavelet_output_well_waveletsPointer->text(1)));//well-wavelets
		oWaveletGlobalCheckBox->setChecked(StandardStrings::checkedBool(wavelet_output_global_waveletsPointer->text(1)));//global-wavelets
		oWaveletLocalCheckBox->setChecked(StandardStrings::checkedBool(wavelet_output_local_waveletsPointer->text(1)));//local-wavelets
		//other-output
		//extra surface...
		oPriorCorrelationCheckBox->setChecked(StandardStrings::checkedBool(io_settings_prior_correlationsPointer->text(1)));//prior-correlations
		oBackgroundTrend1DCheckBox->setChecked(StandardStrings::checkedBool(io_settings_background_trend_1dPointer->text(1)));//background-trend-1d
		oLocalNoiseCheckBox->setChecked(StandardStrings::checkedBool(io_settings_local_noisePointer->text(1)));//local-noise
	}
	else{
		toolBox->setVisible(false);
		toolBox->setEnabled(false);
	}
	oPrefixLineEdit->setText(io_settings_file_output_prefixPointer->text(1) );//file-output-prefix
	oOutputDirectoryLineEdit->setText(top_directoryPointer->text(1)+output_directoryPointer->text(1));
       	necessaryFieldGui();
}

void Main_crava::on_aboutAction_triggered(){
	QMessageBox::about(this,QString("About"), QString("<h2>"+StandardStrings::cravaGuiVersion()+"</h2>"
							  "<p>Copyright &copy; 2010 Statoil"
							  "<p>CRAVA GUI is a program that defines and edits xml files for running CRAVA in an intuitive and seamless way."
							  " It is written for " +StandardStrings::cravaVersion()+ "<p>The GUI is created using Qt and C++"
							  "<p>CRAVA (Condition Reservoir variables on Amplitude Versus Angle) is an seismic AVA inversion software. CRAVA is developed by "
							  "Norsk Regnesentral together with Statoil R&D."
"<pre>"
"    This program is free software: you can redistribute it and/or modify    <br>"
"    it under the terms of the GNU General Public License as published by    <br>"
"    the Free Software Foundation, either version 3 of the License, or    <br>"
"    (at your option) any later version.<br><br>"

"    This program is distributed in the hope that it will be useful,    <br>"
"    but WITHOUT ANY WARRANTY; without even the implied warranty of    <br>"
"    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the    <br>"
"    GNU General Public License for more details.    <br><br>"

"    You should have received a copy of the GNU General Public License    <br>"
"    along with this program.  If not, see &lt;http://www.gnu.org/licenses/&gt;.    "
"</pre>"
));
}
void Main_crava::on_manualAction_triggered(){
	QSettings settings("Statoil","CRAVA");
	settings.beginGroup("crava");
	if(!QDesktopServices::openUrl(settings.value("manual",QString("manual/CRAVA_user_manual.pdf")).toString())){
		QMessageBox::warning(this, QString("Invalid URL"), QString("Could not find the file for the manual, check settings."), QMessageBox::Ok);
	}
}
void Main_crava::on_wikiAction_triggered(){
	QSettings settings("Statoil","CRAVA");
	settings.beginGroup("crava");
	if(!QDesktopServices::openUrl(settings.value("wiki",QString("")).toString())){
		QMessageBox::warning(this, QString("Invalid URL"), QString("Could not find the specified url for the wiki page, check settings."), QMessageBox::Ok);
	}
}
void Main_crava::on_runAction_triggered(){
	if(!currentFile().isEmpty()){//this is nowhere near platform independent... only works on linux, unfortunately the terminals do not have the same syntax so require hacks to get it to work properly, if there was a terminal widget that might have been better.
		QSettings settings("Statoil","CRAVA");
		settings.beginGroup("crava");
		QString program;
		QStringList arguments;
		if(settings.value(QString("useterminal"),true).toBool()){
			program=settings.value(QString("terminal"),QString("konsole")).toString();
			if(program==QString("gnome-terminal")){//sets title of window
				arguments << QString("-t");
			}
			else{
				arguments << QString("-T");
			}
			arguments << QString(StandardStrings::cravaVersion() + QString(" ") + StandardStrings::strippedName(currentFile()));
			if(program==QString("konsole")){//these flags only work for konsole
				arguments << QString("--workdir");//sets working directory
				if(!top_directoryPointer->text(1).isEmpty()){
					arguments << top_directoryPointer->text(1);
				}
				else{
					arguments << QString(".");
				}
				arguments << QString("--noclose");//makes the terminal not close on exit
			}
			
			if(program==QString("gnome-terminal")){//sets executable
				arguments << QString("-x");
				arguments << QString(settings.value(QString("executable"),QString()).toString());
			}
			else{
				arguments << QString("-e");
				arguments << settings.value(QString("executable"),QString()).toString();
			}
		}
		else{//if it is to not run in terminal, just start the program directly
			program=settings.value(QString("executable"),QString()).toString();
		}
		arguments << currentFile();
		QProcess *cravaRun = new QProcess();
		cravaRun->start(program,arguments);
	}
	else{
		QMessageBox::warning(this, QString("No file"), QString("The file needs to be saved on disk to run crava."), QMessageBox::Ok);
	}
}

void Main_crava::on_openAction_triggered(){//opens in a new window and deletes the old window
        QDir dir(top_directoryPointer->text(1)); //dir is the working directory
        QString fileName = QFileDialog::getOpenFileName(this, QString("Open File"), dir.path(), StandardStrings::xmlFormat());
	if(!fileName.isNull()){
		QPointer<Main_crava> newWin = new Main_crava(0,true,fileName);
		newWin->show();
                delete this; //deletes the old window 
        }
}

void Main_crava::on_newAction_triggered(){//opens in a new window
	QPointer<Main_crava> newWin = new Main_crava(0,true);
	newWin->show();
}

bool Main_crava::saveFile(const QString &fileName){
	if(!writeXmlFromTree(fileName, xmlTreeWidget)){
	  	statusBar()->showMessage("Saved " + StandardStrings::strippedName(currentFile()),2000);
		return false;//saving failed
	}
	else {
		setCurrentFile(fileName);
		return true;
	}
}
void Main_crava::on_xmlTreeWidget_itemChanged(){
	setWindowModified(true);
}
void Main_crava::setCurrentFile(const QString &fileName){
	currentFile_=fileName;//update which file this is for
	QString showName = QString("untitled");
	if (standard->StandardStrings::fileExists(currentFile())&&(!currentFile().isEmpty())){
		showName = StandardStrings::strippedName(currentFile());
       		oXmlFilenameLineEdit->setText(showName.remove(showName.lastIndexOf(".xml"),4));
	}
	setWindowTitle(QString("%1[*] - %2").arg(showName).arg(StandardStrings::cravaVersion()));//update the titlebar
	setWindowModified(false);
}

bool Main_crava::on_saveAction_triggered(){
        QString string = oXmlFilenameLineEdit->text();
        if (string.isEmpty()) {
		return on_saveAsAction_triggered();//trigger saveAsAction
	} else {
                if(string.endsWith(".xml")){
	              string.remove(string.lastIndexOf(".xml"),4);
          	}
	        QString sep = "/";
	        if(output_directoryPointer->text(1).isEmpty()) sep = "";
	        return saveFile(top_directoryPointer->text(1) + output_directoryPointer->text(1) + sep + oXmlFilenameLineEdit->text() + QString(".xml"));
	}
}
bool Main_crava::on_saveAsAction_triggered(){
  QString fileName = QFileDialog::getSaveFileName(this, QString("Save File"),top_directoryPointer->text(1)+output_directoryPointer->text(1), StandardStrings::xmlFormat());
	if(!fileName.isNull()){
	   if(fileName.endsWith(".xml")){
		return saveFile(fileName);
	   }
	   else{
	        return saveFile(fileName + QString(".xml"));
	   }
	}
	return false;
}

void Main_crava::closeEvent(QCloseEvent *event){
	if(okToCloseCurrent()) {//is the file modified? promt for save
		writeSettings();//write settings on close
		event->accept();//actually close
	}
	else {
		event->ignore();//return to the program.
	}
}

bool Main_crava::okToCloseCurrent(){//makes the program prompt for save on exit
	if (isWindowModified()) {//checks if window is modified isWindowModified()
		int r = QMessageBox::warning(this, QString("Save"), QString("The document has been modified.\nDo you want to save your changes?"),
						QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);
		if (r == QMessageBox::Yes) {
			return on_saveAction_triggered();
		} else if (r == QMessageBox::Cancel) {
			return false;
		}
	}
		return true;
}
bool Main_crava::okToRun(){//makes the program prompt for save on run crava
	if (isWindowModified()) {//checks if window is modified isWindowModified()
		int r = QMessageBox::warning(this, QString("Save"), QString("The document has been modified.\nPlease save in order to run CRAVA!"),
						QMessageBox::Save | QMessageBox::Cancel);
		if (r == QMessageBox::Save) {
			return on_saveAction_triggered();
		} 
		else if (r == QMessageBox::Cancel) {
			return false;
		}
	} 
}
void Main_crava::on_settingsAction_triggered(){

	QPointer<SettingsDialog> settings= new SettingsDialog(this, this, forwardMode(),estimationMode(),standard);
	if(settings->exec()){
		settings->updateSettings();
		writeSettings();//only some settings a written, depends on what mode it is.
		QSettings settings("Statoil","CRAVA");
		settings.beginGroup("crava");
		settings.beginGroup("GUI");
		xmlTreeWidget->setVisible(settings.value(QString("showtree"),true).toBool());
	}
	delete settings;
}

void Main_crava::on_modeAction_triggered(){
	bool *falsePointer = new bool;
	*falsePointer = false; //false because the program should not load from mode dialog (should load from Main_crava) when the program is already running.
	QString empty = QString(); // just to fill in the third argument in mode. Not used.
	mode( true, falsePointer, true, empty );
	delete falsePointer;
}

bool Main_crava::mode(bool started, bool *pressedOpen, bool existing, QString &filename){
	QPointer<ModeDialog> mode;
	QString modeString=modePointer->text(1);//the existing mode.
	if(started){
		QList<QString> values;
		values.append(modePointer->text(1));//mode
		if(modePointer->text(1)==QString("inversion")){
			values.append(predictionPointer->text(1));//prediction
			bool simulation=false;
			for (int i=0;i<simulationPointer->childCount();++i) {//will loop over the 3 children 
				if(!simulationPointer->child(i)->text(1).isEmpty()){
					simulation=true;
				}
			}
			if(simulation){//simulation=1
				values.append(QString("yes"));
				for (int i=0;i<simulationPointer->childCount();++i) {//will loop over the 3 children 
					values.append(simulationPointer->child(i)->text(1));
				}
			}
			else {
				values.append(QString("no"));
			}
			values.append(inversion_settings_kriging_to_wellsPointer->text(1));//kriging-to-wells
			values.append(inversion_settings_facies_probabilitiesPointer->text(1));//facies-probabilities
			
		}
		else if(actionsPointer->text(1)==QString("estimation")){
			values.append(estimation_settings_estimate_backgroundPointer->text(1));//estimate-background
			values.append(estimation_settings_estimate_correlationsPointer->text(1));//estimate-correlations
			values.append(estimation_settings_estimate_wavelet_or_noisePointer->text(1));//estimate-wavelet-or-noise
		}
		mode=new ModeDialog(this, started, pressedOpen, existing, this);//should be initialized with the correct parameters
	}
	else {
		mode=new ModeDialog(this, started, pressedOpen, existing, this);
	}
	if(!(mode->exec() == QDialog::Accepted)){
		if(started){
			delete mode;
			return false;
		}
		else {
			mode->modes();//fill the tree in with the information related to actions
			top_directoryPointer->setText(1, mode->topDirectoryLineEdit->text());
			QDir dir( mode->topDirectoryLineEdit->text() );
			if ( !dir.relativeFilePath( mode->inputDirectoryLineEdit->text() ).isEmpty() ){
				input_directoryPointer->setText( 1, dir.relativeFilePath( mode->inputDirectoryLineEdit->text() ) );
			}
			else{
				input_directoryPointer->setText( 1, dir.relativeFilePath( mode->inputDirectoryLineEdit->text() ) );
			}
			
			dir.cd( mode->inputDirectoryLineEdit->text() );
			standard->StandardStrings::setinputPath(dir.path());//makes sure all the relative paths work
			/*if(!started)writeSettings();//overwrites with empty settings if mode is foward... */
			delete mode;
			//close(); called from the constructor if needed
			faciesGui();
			forwardGui();
			estimationGui();
			return false;
		}
	}

	mode->modes();
	if(!started){
		top_directoryPointer->setText(1, mode->topDirectoryLineEdit->text());
		QDir dir( mode->topDirectoryLineEdit->text() );
		if ( !dir.relativeFilePath( mode->inputDirectoryLineEdit->text() ).isEmpty() ){
			input_directoryPointer->setText( 1, dir.relativeFilePath( mode->inputDirectoryLineEdit->text() ) );
		}
		else{
			input_directoryPointer->setText( 1, dir.relativeFilePath( mode->inputDirectoryLineEdit->text() ) );
		}
		dir.cd( mode->inputDirectoryLineEdit->text() );
		standard->StandardStrings::setinputPath(dir.path());//makes sure all the relative paths work
		writeSettings();//can only be changed the first time mode is run which means started is false
	}
	else{
		if(modeString!=modePointer->text(1)){//if the mode have changed it does not count as the same file.
			setCurrentFile(QString());
		}
	}
	filename = mode->openProjectLineEdit->text();
	delete mode;
	//updates the gui depending on what modes have been chosen.
	faciesGui();
	forwardGui();
	estimationGui();
	return true;
}

void Main_crava::estimationGui(){
	if(estimationMode()){
	        QList<QCheckBox*> outputs=toolBox->QObject::findChildren<QCheckBox*>();
		foreach (QCheckBox* output, outputs){
			output->setChecked(false);
		}

	  	//project-settings
		for(int j=0; j<grid_output_seismic_dataPointer->childCount(); ++j){
			grid_output_seismic_dataPointer->child(j)->setText(1,QString(""));
		}


		for(int j=0; j<grid_output_domainPointer->childCount(); ++j){
			grid_output_domainPointer->child(j)->setText(1,QString(""));
		}

		for(int j=0; j<elastic_parametersPointer->childCount(); ++j){
			elastic_parametersPointer->child(j)->setText(1,QString(""));
		}

		for(int j=0; j<grid_output_other_parametersPointer->childCount(); ++j){
			grid_output_other_parametersPointer->child(j)->setText(1,QString(""));
		}

		//clear io-settings well, wavelet and other
		for(int j=0;j<well_outputPointer->childCount();++j){
			well_outputPointer->child(j)->setText(1,QString(""));
			for(int k=0;k<well_outputPointer->child(j)->childCount();++k){
				well_outputPointer->child(j)->child(k)->setText(1,QString(""));
			}
		}
		for(int j=0;j<wavelet_outputPointer->childCount();++j){
			wavelet_outputPointer->child(j)->setText(1,QString(""));
			for(int k=0;k<wavelet_outputPointer->child(j)->childCount();++k){
				wavelet_outputPointer->child(j)->child(k)->setText(1,QString(""));
			}
		}
		for(int j=0;j<io_settings_other_outputPointer->childCount();++j){
			io_settings_other_outputPointer->child(j)->setText(1,QString(""));
			for(int k=0;k<io_settings_other_outputPointer->child(j)->childCount();++k){
				io_settings_other_outputPointer->child(j)->child(k)->setText(1,QString(""));
			}
		}

		//clear advanced settings, up to energy-treshold
		x_fractionPointer->setText(1,QString(""));
		y_fractionPointer->setText(1,QString(""));
		z_fractionPointer->setText(1,QString(""));
		use_intermediate_disk_storagePointer->setText(1,QString(""));
		maximum_relative_thickness_differencePointer->setText(1,QString(""));
		frequency_band_low_cutPointer->setText(1,QString(""));
		frequency_band_high_cutPointer->setText(1,QString(""));
		vp_vs_ratioPointer->setText(1,QString(""));
		vp_vs_ratioPointer->setText(1,QString(""));

		toolBox->setVisible(false);
		toolBox->setEnabled(false);

	       	if(stackListWidget->count()<1)	angleLineEdit->setStyleSheet("");
	}
	else{
		readSettings();//restore settings
      		if(!forwardMode()){
		  updateGuiToTree();
		  toolBox->setVisible(true);
		  toolBox->setEnabled(true);
		}
	
	}
	
}

void Main_crava::forwardGui(){
	//checks if mode if forward, hides/show things as appropriate and clears the tree as needed.
	if(forwardMode()){
		if(stackListWidget->count()>0){
			QTreeWidgetItem* angleGather;
			findCorrectAngleGather(&angleGather);
			QString fileName;
			getValueFromAngleGather( angleGather, QString("file-name"), fileName, QString("seismic-data") );
			if(!fileName.isEmpty()){
				while(stackListWidget->count()>0){//can break loading if someone puts in an angle stack without a filename (though then it will just remove the angle stack which may be for the best)
					deleteStackPushButton->click();
				}
			}
		}
		QList<QWidget*> widgets=seismicDataFrame->QObject::findChildren<QWidget*>();
		foreach (QWidget* widget, widgets){
			widget->setVisible(false);
		}
		seismicAngelLabel->setVisible(true);
		angleLineEdit->setVisible(true);
	       	if(stackListWidget->count()<1)	angleLineEdit->setStyleSheet("");
	       	QTreeWidgetItem *angleGather;
		findCorrectAngleGather(&angleGather);
		waveletFrame->setVisible(true);
		waveletWidget->setVisible(true);
		waveletLabel->setVisible(true);
		waveletFileRadioButton->setVisible(true);
	       	rickerRadioButton->setVisible(true);
		QString val;
		getValueFromAngleGather(angleGather,QString("ricker"),val);
		if(val.isEmpty()){
		     waveletFileLineEdit->setVisible(true);
		     waveletBrowsePushButton->setVisible(true);
		}
		else{
		     rickerLineEdit->setVisible(true);
		}
	
		QTreeWidgetItem * item=surveyPointer;//survey
		for(int i=0;i<survey_angular_correlationPointer->childCount();++i){
			survey_angular_correlationPointer->child(i)->setText(1,QString(""));//angular-correlation
		}
		survey_segy_start_timePointer->setText(1,QString(""));//segy-start-time
		survey_top_surface_filePointer->setText(1,QString(""));//wavelet estimation interval
		survey_base_surface_filePointer->setText(1,QString(""));
		item=well_dataPointer;//well
		while(wellListWidget->count()>0){//wells
			deleteWellPushButton->click();
		}
		QList<QLineEdit*> fields=wellsTab->QObject::findChildren<QLineEdit*>();
		foreach (QLineEdit* field, fields){
			field->clear();
		}
		for(int i=0;i<item->childCount();++i){
			for(int j=0;j<item->child(i)->childCount();++j){
				item->child(i)->child(j)->setText(1,QString(""));
			}
			item->child(i)->setText(1,QString(""));
		};
		item=prior_modelPointer;//prior
		fields=priorModelTab->QObject::findChildren<QLineEdit*>();
		foreach (QLineEdit* field, fields){
			field->clear();
		}

		for(int i=0;i<item->childCount();++i){//can break loading, not anymore
			if( item->child(i)->text(0) == QString("earth-model") ){//should not clear the earth model
				continue;
			}
			for(int j=0;j<item->child(i)->childCount();++j){
				for(int k=0;k<item->child(i)->child(j)->childCount();++k){
					item->child(i)->child(j)->child(k)->setText(1,QString(""));
				}
				item->child(i)->child(j)->setText(1,QString(""));
			}
			item->child(i)->setText(1,QString(""));
		}

		QList<QCheckBox*>outputs=toolBox->QObject::findChildren<QCheckBox*>();
		foreach (QCheckBox* output, outputs){
		         output->setChecked(false);
		}
		//project-settings
		for(int j=0;j<segy_formatPointer->childCount();++j){ //clear only segy-format
			segy_formatPointer->child(j)->setText(1,QString(""));
		}
		
		for(int j=0; j<grid_output_seismic_dataPointer->childCount(); ++j){
			grid_output_seismic_dataPointer->child(j)->setText(1,QString(""));
		}


		for(int j=0; j<grid_output_domainPointer->childCount(); ++j){
			grid_output_domainPointer->child(j)->setText(1,QString(""));
		}

		for(int j=0; j<elastic_parametersPointer->childCount(); ++j){
			elastic_parametersPointer->child(j)->setText(1,QString(""));
		}

		for(int j=0; j<grid_output_other_parametersPointer->childCount(); ++j){
			grid_output_other_parametersPointer->child(j)->setText(1,QString(""));
		}

		//clear io-settings well, wavelet and other
		for(int j=0;j<well_outputPointer->childCount();++j){
			well_outputPointer->child(j)->setText(1,QString(""));
			for(int k=0;k<well_outputPointer->child(j)->childCount();++k){
				well_outputPointer->child(j)->child(k)->setText(1,QString(""));
			}
		}
		for(int j=0;j<wavelet_outputPointer->childCount();++j){
			wavelet_outputPointer->child(j)->setText(1,QString(""));
			for(int k=0;k<wavelet_outputPointer->child(j)->childCount();++k){
				wavelet_outputPointer->child(j)->child(k)->setText(1,QString(""));
			}
		}
		for(int j=0;j<io_settings_other_outputPointer->childCount();++j){
			io_settings_other_outputPointer->child(j)->setText(1,QString(""));
			for(int k=0;k<io_settings_other_outputPointer->child(j)->childCount();++k){
				io_settings_other_outputPointer->child(j)->child(k)->setText(1,QString(""));
			}
		}

		for(int i=0;i<advanced_settingsPointer->childCount();++i){//clear advanced settings
			for(int j=0;j<advanced_settingsPointer->child(i)->childCount();++j){
				advanced_settingsPointer->child(i)->child(j)->setText(1,QString(""));
			}
			advanced_settingsPointer->child(i)->setText(1,QString(""));
		}
		tabWidget->insertTab(tabWidget->count()-2,earthModelTab,QString("Earth Model"));
		tabWidget->setCurrentIndex(0);
		toolBox->setVisible(false);
		toolBox->setEnabled(false);
	}
	else{
		tabWidget->removeTab(tabWidget->indexOf(earthModelTab));
		if(stackListWidget->count()>0){
			QTreeWidgetItem* angleGather;
			findCorrectAngleGather(&angleGather);
			QString fileName;
			getValueFromAngleGather( angleGather, QString("file-name"), fileName, QString("seismic-data") );	
			if(fileName.isEmpty()){
				while(stackListWidget->count()>0){//breaks loading, have to check if the names are/are not empty
				//either remove the pure angle stacks or remove the seismic stacks
					deleteStackPushButton->click();
				}
			}
		}
		//show the widgets that were hidden when facies mode was turned on.
		QList<QLabel*> labels=seismicDataFrame->QObject::findChildren<QLabel*>();
		foreach (QLabel* label, labels){
			label->setVisible(true);
		}
		QList<QAbstractButton*> buttons=seismicDataFrame->QObject::findChildren<QAbstractButton*>();
		foreach (QAbstractButton* button, buttons){
			button->setVisible(true);
		}
		matchEnergiesCheckBox->setVisible(false);
		QList<QLineEdit*> fields=seismicDataFrame->QObject::findChildren<QLineEdit*>();
		foreach (QLineEdit* field, fields){
			field->setVisible(true);
		}

       		on_waveletFileRadioButton_toggled(waveletFileRadioButton->isChecked());
		on_manualScaleRadioButton_toggled(manualScaleRadioButton->isChecked());
		on_signalToNoiseCheckBox_toggled(signalToNoiseCheckBox->isChecked());
		on_shiftLocalWaveletCheckBox_toggled(shiftLocalWaveletCheckBox->isChecked());
		on_scaleLocalWaveletCheckBox_toggled(scaleLocalWaveletCheckBox->isChecked());
		on_localNoiseEstimateCheckBox_toggled(localNoiseEstimateCheckBox->isChecked());
		for(int i=0;i<earth_modelPointer->childCount();++i){//clears earth-model
			earth_modelPointer->child(i)->setText(1,QString(""));
		}
		fields=earthModelTab->QObject::findChildren<QLineEdit*>();
		foreach (QLineEdit* field, fields){
			field->clear();
		}
		readSettings();//updates with the settings that were removed
		if(!forwardMode()) updateGuiToTree();
	}
       	scaleWidget->setVisible(!forwardMode());
	scaleWidget->setEnabled(!forwardMode());
	tabWidget->setTabEnabled(tabWidget->indexOf(wellsTab),!forwardMode());//wellsTab, can't hide them without removing
	tabWidget->setTabEnabled(tabWidget->indexOf(priorModelTab),!forwardMode());//Prior model tab
	waveletIntervalLabel->setVisible(!forwardMode());
	waveletTopSurfaceLabel->setVisible(!forwardMode());
	waveletTopLineEdit->setVisible(!forwardMode());
	waveletTopBrowsePushButton->setVisible(!forwardMode());
	waveletBottomSurfaceLabel->setVisible(!forwardMode());
	waveletBottomLineEdit->setVisible(!forwardMode());
	waveletBottomBrowsePushButton->setVisible(!forwardMode());
	waveletIntervalLabel->setEnabled(!forwardMode());
	waveletTopSurfaceLabel->setEnabled(!forwardMode());
	waveletTopLineEdit->setEnabled(!forwardMode());
	waveletTopBrowsePushButton->setEnabled(!forwardMode());
	waveletBottomSurfaceLabel->setEnabled(!forwardMode());
	waveletBottomLineEdit->setEnabled(!forwardMode());
	waveletBottomBrowsePushButton->setEnabled(!forwardMode());
	defaultStartTimeLabel->setVisible(!forwardMode());
	defaultStartTimeLabel->setEnabled(!forwardMode());
	defaultStartTimeLineEdit->setVisible(!forwardMode());
	defaultStartTimeLineEdit->setEnabled(!forwardMode());
	angularCorrelationLabel->setVisible(!forwardMode());
	angularCorrelationLabel->setEnabled(!forwardMode());
	angularCorrelationPushButton->setVisible(!forwardMode());
	angularCorrelationPushButton->setEnabled(!forwardMode());
}

void Main_crava::faciesGui(){
	//use for facies in wells? log name in wells? They don't cause the program to crash, but they should not be visible not matter.
	//probably ineffecient to evaluate the bool so many times, but looks cleaner, making sure the correct widgets are displayed.
	faciesProbabilityLabel->setVisible(faciesProbabilitiesOn());
	faciesEstimateCheckBox->setVisible(faciesProbabilitiesOn());
	faciesEstimateFrame->setVisible(faciesProbabilitiesOn());
	uncertaintyLevelLabel->setVisible(faciesProbabilitiesOn());
	uncertaintyLevelLineEdit->setVisible(faciesProbabilitiesOn());
	faciesProbabilityLabel->setEnabled(faciesProbabilitiesOn());
	faciesEstimateCheckBox->setEnabled(faciesProbabilitiesOn());
	faciesEstimateFrame->setEnabled(faciesProbabilitiesOn());
	uncertaintyLevelLabel->setEnabled(faciesProbabilitiesOn());
	uncertaintyLevelLineEdit->setEnabled(faciesProbabilitiesOn());
	faciesIntervalLabel->setVisible(faciesProbabilitiesOn());
	faciesTopLabel->setVisible(faciesProbabilitiesOn());
	faciesTopLineEdit->setVisible(faciesProbabilitiesOn());
	faciesTopBrowsePushButton->setVisible(faciesProbabilitiesOn());
	faciesBottomLabel->setVisible(faciesProbabilitiesOn());
	faciesBottomLineEdit->setVisible(faciesProbabilitiesOn());
	faciesBottomBrowsePushButton->setVisible(faciesProbabilitiesOn());
	faciesIntervalLabel->setEnabled(faciesProbabilitiesOn());
	faciesTopLabel->setEnabled(faciesProbabilitiesOn());
	faciesTopLineEdit->setEnabled(faciesProbabilitiesOn());
	faciesTopBrowsePushButton->setEnabled(faciesProbabilitiesOn());
	faciesBottomLabel->setEnabled(faciesProbabilitiesOn());
	faciesBottomLineEdit->setEnabled(faciesProbabilitiesOn());
	faciesBottomBrowsePushButton->setEnabled(faciesProbabilitiesOn());
	faciesFormatLabel->setVisible(faciesProbabilitiesOn());
	faciesLineEdit->setVisible(faciesProbabilitiesOn());
	faciesProbabilitiesCheckBox->setVisible(faciesProbabilitiesOn());
	faciesFormatLabel->setEnabled(faciesProbabilitiesOn());
	faciesLineEdit->setEnabled(faciesProbabilitiesOn());
	faciesProbabilitiesCheckBox->setEnabled(faciesProbabilitiesOn());
       	oFaciesProbabilitiesCheckBox->setVisible(faciesProbabilitiesOn());
	oFaciesProbabilitiesUndefinedCheckBox->setVisible(faciesProbabilitiesOn());
	oFaciesLikelihoodCheckBox->setVisible(faciesProbabilitiesOn());
	oFaciesQualityGridCheckBox->setVisible(faciesProbabilitiesOn());
	oRockPhysicsCheckBox->setVisible(faciesProbabilitiesOn());
	oFaciesProbabilitiesCheckBox->setEnabled(faciesProbabilitiesOn());
	oFaciesProbabilitiesUndefinedCheckBox->setEnabled(faciesProbabilitiesOn());
	oFaciesLikelihoodCheckBox->setEnabled(faciesProbabilitiesOn());
	oFaciesQualityGridCheckBox->setEnabled(faciesProbabilitiesOn());
	oRockPhysicsCheckBox->setEnabled(faciesProbabilitiesOn());
	if(faciesProbabilitiesOn()){
		updateGuiToTree();//updates the gui with whatever values(default), are in the tree
	}
	else{
		//needs to clear all the data that causes error if facies are off
		while(faciesListWidget->count()>0){
			deleteFaciesPushButton->click();
		}

		for(int i=0;i<prior_model_facies_probabilitiesPointer->childCount();++i){
			prior_model_facies_probabilitiesPointer->child(i)->setText(1,QString(""));
			for(int j=0;j<prior_model_facies_probabilitiesPointer->child(i)->childCount();++j){
				prior_model_facies_probabilitiesPointer->child(i)->child(j)->setText(1,QString(""));
			}
		}
		log_names_faciesPointer->setText( 1, QString("") );

		grid_output_facies_probabilitiesPointer->setText(1,QString(""));
		grid_output_facies_probabilities_with_undefPointer->setText(1,QString(""));
		grid_output_facies_likelihoodPointer->setText(1,QString(""));
		grid_output_seismic_quality_gridPointer->setText(1,QString(""));
		io_settings_rock_physics_distributionsPointer->setText(1,QString(""));
		faciesGivenFrame->setEnabled(false);
		faciesGivenFrame->setVisible(false);
	}
}

bool Main_crava::faciesProbabilitiesOn(){
	// here it will be false for estimation and forward as well.
	return (inversion_settings_facies_probabilitiesPointer->text(1)==QString("yes"));
}
bool Main_crava::forwardMode(){
	return (modePointer->text(1)==QString("forward"));
}

bool Main_crava::estimationMode(){
	return (modePointer->text(1)==QString("estimation"));
}

void Main_crava::on_wellHeaderPushButton_clicked(){
	if(wellListWidget->count()>0){
		QTreeWidgetItem* well;
		findCorrectWell(&well);
		QString wellFilename;
		getValueFromWell(well, QString("file-name"), wellFilename);

		getWellHeaders( QDir(standard->inputPath()).absoluteFilePath(wellFilename) );
	}
}
bool Main_crava::getWellHeaders(const QString &fileName){
	wellHeaderListWidget->clear();
	QFile file(fileName);
	if (!file.open(QFile::ReadOnly | QFile::Text)) {
		QMessageBox::warning(this, QString("Unable to open file"), QString("Could not open the file."), QMessageBox::Ok);
		return false;
	}
	QTextStream stream(&file);
	stream.readLine();//version?
	stream.readLine();//undefined?
	stream.readLine();//name, but filename is more unifiable
	int numberOfLogs = stream.readLine().toInt();//number of logs
	for(int i=0;i<numberOfLogs;++i){
		wellHeaderListWidget->addItem(stream.readLine().split(" ").at(0));
	}
	file.close();
	if (file.error()) {
		QMessageBox::warning(this, QString("File error"), QString("There was an error closing the file."), QMessageBox::Ok);
		return false;
	}
	return true;
}

bool Main_crava::writeXmlToTree(const QString &fileName, QTreeWidget *tree){
	//this might as well not have a pointer to the tree as it is very gui specific... will only work if the correct tree is specified.
	QFile file(fileName);
	if (!file.open(QFile::ReadOnly | QFile::Text)) {
		QMessageBox::warning(this, QString("Unable to open file"), QString("Could not open the file."), QMessageBox::Ok);
		return false;
	}
	//needs to clear the comments that crava support, they are not normal comments.
	QString comentlessDocument("");
	QTextStream originalStream(&file);
	QTextStream commentlessStream(&comentlessDocument);
	while(!originalStream.atEnd()){
		QString line = originalStream.readLine().trimmed();
		if(line[0] != QChar('#')){
			commentlessStream << line;
		}
	}
	QDomDocument doc;
	QString errorStr;
	int errorLine;
	int errorColumn;
	//this poppulates the entire domdocument assuming the xml is well formed.
	if (!doc.setContent(comentlessDocument, false, &errorStr, &errorLine,&errorColumn)) {
		QMessageBox::warning(this, QString("Unable to parse xml"), QString("Error: Parse error at line " + QString("%1").arg(errorLine) + ", "+ "column " + QString("%1").arg(errorColumn) + ": "+ errorStr), QMessageBox::Ok);
		return false;
	}
	QDomElement root = doc.documentElement();
	if (root.tagName() != "crava") {//is this a crava xml file
		QMessageBox::warning(this, QString("Invalid file"), QString("This is not a crava xml file."), QMessageBox::Ok);
		return false;
	}
	QDir dir;
	if(!root.firstChildElement("project-settings").firstChildElement("io-settings").firstChildElement("top-directory").text().trimmed().isEmpty()){
		dir=QDir(root.firstChildElement("project-settings").firstChildElement("io-settings").firstChildElement("top-directory").text().trimmed());
	}
	else{//if the top directory is not given, crava expects the current directory to be the wroking directory
		dir = QDir(QDir::current());
	}
	//sets the directories, since even empty directory have to overwrite whatever is in settings for crava to behave properly
	top_directoryPointer->setText(1,root.firstChildElement("project-settings").firstChildElement("io-settings").firstChildElement("top-directory").text().trimmed());
	input_directoryPointer->setText(1,root.firstChildElement("project-settings").firstChildElement("io-settings").firstChildElement("input-directory").text().trimmed());
	if(!dir.cd(root.firstChildElement("project-settings").firstChildElement("io-settings").firstChildElement("input-directory").text().trimmed())){
		QMessageBox::warning(this, QString("Invalid directory"), QString("Can not locate the input directory specified."), QMessageBox::Ok);
	}
	standard->setinputPath(dir.path());//have to set the input directory first for the relative paths to make sense when updating the list widgets.

	recursiveXmlRead(root, tree->topLevelItem(0));

	file.close();
	if (file.error()) {
		QMessageBox::warning(this, QString("File error"), QString("There was an error closing the file."), QMessageBox::Ok);
		return false;
	}
	return true;
}

void Main_crava::recursiveXmlRead(const QDomNode &xmlItem, QTreeWidgetItem *treeItem){
	//this assumes the tree is fresh, no variograms, angle-gather, wells and facies

	QDomNode  xmlChild = xmlItem.firstChild();
	if(xmlChild.isText ()){//a node can only have one child text node and all nodes without children are text nodes
		treeItem->setText(1,xmlItem.toElement().text().trimmed());
		return;
	}
	QTreeWidgetItem* treeChild;
	while(!xmlChild.isNull()){
		//since there can be multiple angle-gather, wells, optimize positions, zones and facies as well as variograms...
		//these checks could all cause trouble if the names are changed
		if(xmlChild.toElement().tagName() == QString("angle-gather")){
			//angle gathers are not in the tree already and needs to be added before they can be populated
		        stackListWidget->addItem( QString("angle-gather") );
			addStack();
			recursiveXmlRead(xmlChild,treeItem->child(1+stackListWidget->count()));
			stackListWidget->setCurrentRow(stackListWidget->count()-1);
			if(forwardMode()){
			  on_angleLineEdit_editingFinished();
			}
			else{
			  QTreeWidgetItem *angleGather;
			  findCorrectAngleGather(&angleGather);
			  QString name;
			  getValueFromAngleGather(angleGather,QString("file-name"),name,QString("seismic-data"));
			  stackListWidget->currentItem()->setText(name);
			}
		}
		else if(xmlChild.toElement().tagName() == QString("well")&& (xmlItem.toElement().tagName()==QString("well-data"))){
			//wells are not in the tree already and needs to be added before they can be populated
			wellListWidget->addItem(QString("well log"));
			addWell();
			wellListWidget->setCurrentRow(wellListWidget->count()-1);
			recursiveXmlRead(xmlChild,treeItem->child(wellListWidget->count()));
			on_wellListWidget_currentRowChanged(wellListWidget->currentRow());
		}
		else if(xmlChild.toElement().tagName() == QString("optimize-position")){
			//optimized positions are no in the tree already and needs to be added before they can be populated
			optimizePositionCheckBox->setChecked(false);
			optimizePositionListWidget->addItem(QString("optimization"));
			addOptimizePosition();
			recursiveXmlRead(xmlChild,treeItem->child(5+optimizePositionListWidget->count()));
		}
	     	else if(xmlChild.toElement().tagName() == QString("zone")){
		        multizoneBackgroundRadioButton->setChecked(true);
			zoneListWidget->addItem(QString("zone " + QString::number(zoneListWidget->count()+1)));
			addZone();
			zoneListWidget->setCurrentRow(zoneListWidget->count()-1);
			recursiveXmlRead(xmlChild,treeItem->child(1+zoneListWidget->count()));
			on_zoneListWidget_currentRowChanged(zoneListWidget->currentRow());
		}
		else if( (xmlChild.toElement().tagName() == QString("facies")) && (xmlItem.toElement().tagName() == QString("prior-probabilities")) ){
			//facies are no in the tree already and needs to be added before they can be populated
			faciesEstimateCheckBox->setChecked(false);
			faciesListWidget->addItem(QString("facies"));
			addFacies();
			recursiveXmlRead(xmlChild,treeItem->child(faciesListWidget->count()-1));
			faciesListWidget->setCurrentRow(faciesListWidget->count()-1);
			on_faciesNameLineEdit_editingFinished();//fixes name in list
		}
		else {
			for(int i=0;i<treeItem->childCount();++i){//dig deeper
				treeChild=treeItem->child(i);
				if(xmlChild.toElement().tagName() == treeChild->text(0)){
					recursiveXmlRead(xmlChild,treeChild);
				}
			}
		}
		xmlChild=xmlChild.nextSibling();
	}
}

bool Main_crava::writeXmlFromTree(const QString &fileName, QTreeWidget *tree){//using dom xml tree instead
	const int Indent = 4;//this should be editable in settings somewhere
	QFile file(fileName);
	if (!file.open(QFile::WriteOnly | QFile::Text)) {
		QMessageBox::warning(this, QString("Unable to open file"), QString("Could not open the file."), QMessageBox::Ok);
		return false;
	}
	QDomDocument *xmlDocument = new QDomDocument();//crava doesn't handle xml comments
	recursiveXmlWrite(xmlDocument,xmlDocument,tree->topLevelItem(0));//populates xmlDocument with all the info
	QTextStream out(&file);
	out << '#' << StandardStrings::cravaGuiVersion() << endl << '#' << StandardStrings::cravaVersion() << endl;//writes comments in the header that crava can handle.
	xmlDocument->save(out, Indent);
	file.close();
	if (file.error()) {
		QMessageBox::warning(this, QString("File error"), QString("There was an error closing the file."), QMessageBox::Ok);
		delete xmlDocument;
		return false;
	}
	delete xmlDocument;
	return true;
}

void Main_crava::recursiveXmlWrite(QDomDocument *xmlDocument, QDomNode *parent, QTreeWidgetItem *item){//using dom xml tree instead
	//takes advantage of the fact that no tags have attributes and the only tags with text are the ones with no children
	if(noXmlInfo(item)){//should not write out empty tags or tags with only empty children.
		return;
	}
	if(item->childCount()==0){//writes the text of end nodes.
		QDomElement element = xmlDocument->createElement(item->text(0));
		parent->appendChild(element);
		element.appendChild(xmlDocument->createTextNode(item->text(1)));
	}
	else {//digs further from parent nodes.
		QDomElement element = xmlDocument->createElement(item->text(0));
		parent->appendChild(element);
		for(int i = 0; i<item->childCount();i++){
			recursiveXmlWrite(xmlDocument, &element, item->child(i));
		}
	}
}

bool Main_crava::noXmlInfo(QTreeWidgetItem *item){
	//this function check whether this item should be printed. tags with no text in between or parents of only tags with no text in between should not be printed
	if(!item->text(1).isEmpty()){
		return false;//there is info here
	}
	
	for(int i = 0; i<item->childCount();i++){
		if(!noXmlInfo(item->child(i))){
			return false;//there is info here
		}
	}
	return true;//there was no info here
}


void Main_crava::variogram(QTreeWidgetItem *item){//the item is the parent of the dialog 
	//variogram-type=0 angle=1 range=2 subrange=3 power=4
	QPointer<VariogramDialog> dialog;
	dialog = new VariogramDialog(this, !(item->child(0)->text(1)==QString("shperical")), item->child(1)->text(1),
				     item->child(2)->text(1), item->child(3)->text(1), item->child(4)->text(1));//initialize the dialog
	if(dialog->exec() == QDialog::Accepted){
		QList<QString> list=dialog->variogramValues();//get the values from the dialog and set them under
		item->child(0)->setText(1,list.takeFirst());
		item->child(1)->setText(1,list.takeFirst());
		item->child(2)->setText(1,list.takeFirst());
		item->child(3)->setText(1,list.takeFirst());
		item->child(4)->setText(1,list.takeFirst());
	}
	delete dialog;
}

void Main_crava::addStack(){
	//the strings makes it obvious what items are added. this creates all the needed children.
	QString label = QString("angle-gather");
	QTreeWidgetItem* parent=surveyPointer;//moves down the tree to survey
	QTreeWidgetItem* item = new QTreeWidgetItem(parent,parent->child(stackListWidget->count()));//adds another angle gather after start-time
	item->setText(0,label);
		QTreeWidgetItem* child = new QTreeWidgetItem(item);//adds first child to the stack, offset-angle
		child->setText(0,QString("offset-angle"));
		child=new QTreeWidgetItem(item);
		child->setText(0,QString("seismic-data"));
			QTreeWidgetItem* nestedChild = new QTreeWidgetItem(child);
			nestedChild->setText(0,QString("file-name"));
			nestedChild = new QTreeWidgetItem(child);
			nestedChild->setText(0,QString("start-time"));
			nestedChild = new QTreeWidgetItem(child);
			nestedChild->setText(0,QString("segy-format"));
				QTreeWidgetItem* nestedChildChild = new QTreeWidgetItem(nestedChild);
				nestedChildChild->setText(0,QString("standard-format"));
				//specify the format
				nestedChildChild = new QTreeWidgetItem(nestedChild);
				nestedChildChild->setText(0,QString("location-x"));
				nestedChildChild = new QTreeWidgetItem(nestedChild);
				nestedChildChild->setText(0,QString("location-y"));
				nestedChildChild = new QTreeWidgetItem(nestedChild);
				nestedChildChild->setText(0,QString("location-il"));
				nestedChildChild = new QTreeWidgetItem(nestedChild);
				nestedChildChild->setText(0,QString("location-xl"));
				nestedChildChild = new QTreeWidgetItem(nestedChild);
				nestedChildChild->setText(0,QString("bypass-coordinate-scaling"));
				nestedChildChild = new QTreeWidgetItem(nestedChild);
				nestedChildChild->setText(0,QString("location-scaling-coefficient"));
			nestedChild = new QTreeWidgetItem(child);
			nestedChild->setText(0,QString("type"));
		child=new QTreeWidgetItem(item);
		child->setText(0,QString("wavelet"));
			nestedChild = new QTreeWidgetItem(child);
			nestedChild->setText(0,QString("file-name"));
			nestedChild = new QTreeWidgetItem(child);
		       	nestedChild->setText(0,QString("ricker"));
			nestedChild = new QTreeWidgetItem(child);
			nestedChild->setText(0,QString("scale"));
			nestedChild = new QTreeWidgetItem(child);
			nestedChild->setText(0,QString("estimate-scale"));
			nestedChild = new QTreeWidgetItem(child);
			nestedChild->setText(0,QString("local-wavelet"));
				nestedChildChild = new QTreeWidgetItem(nestedChild);
				nestedChildChild->setText(0,QString("shift-file"));
				nestedChildChild = new QTreeWidgetItem(nestedChild);
				nestedChildChild->setText(0,QString("scale-file"));
				nestedChildChild = new QTreeWidgetItem(nestedChild);
				nestedChildChild->setText(0,QString("estimate-shift"));
				nestedChildChild = new QTreeWidgetItem(nestedChild);
				nestedChildChild->setText(0,QString("estimate-scale"));
		child=new QTreeWidgetItem(item);
		child->setText(0,QString("wavelet-3d"));//needs to be implemented / expanded, widgets are there but they don't do anything.
		child=new QTreeWidgetItem(item);
		child->setText(0,QString("match-energies"));
		child=new QTreeWidgetItem(item);
		child->setText(0,QString("signal-to-noise-ratio"));
		child=new QTreeWidgetItem(item);
		child->setText(0,QString("local-noise-scaled"));
		child=new QTreeWidgetItem(item);
		child->setText(0,QString("estimate-local-noise"));
}

void Main_crava::addWell(){
	//the strings makes it obvious what items are added. this creates all the needed children.
	//well-data=2 well = 1
	QString label = QString("well");
	QTreeWidgetItem* parent = well_dataPointer;//moves down the tree to well-data
	QTreeWidgetItem* item = new QTreeWidgetItem(parent,parent->child(wellListWidget->count()-1));//adds another well after log-names, and previous wells
	item->setText(0,label);
		QTreeWidgetItem* child = new QTreeWidgetItem(item);//adds first child to the stack, file-name
		child->setText(0,QString("file-name"));
		child=new QTreeWidgetItem(item);
		child->setText(0,QString("use-for-wavelet-estimation"));
		child=new QTreeWidgetItem(item);
		child->setText(0,QString("use-for-background-trend"));
		child=new QTreeWidgetItem(item);
		child->setText(0,QString("use-for-facies-probabilities"));
		child=new QTreeWidgetItem(item);
		child->setText(0,QString("synthetic-vs-log"));
		child=new QTreeWidgetItem(item);
		child->setText(0,QString("filter-elastic-logs"));
		//child=new QTreeWidgetItem(item);
		//child->setText(0,QString("optimize-position")); added by it's own stack
}

void Main_crava::addOptimizePosition(){
	//the strings makes it obvious what items are added. this creates all the needed children.
	//well-data=2 well = 1+wellListWidget()->currentRow();
	QString label = QString("optimize-position");
	QTreeWidgetItem* parent;
	findCorrectWell(&parent);
	QTreeWidgetItem* item = new QTreeWidgetItem(parent,parent->child(optimizePositionListWidget->count()+4));//adds another optimization after last postion
	item->setText(0,label);
		QTreeWidgetItem* child = new QTreeWidgetItem(item);//adds first child to the stack, angle
		child->setText(0,QString("angle"));
		child=new QTreeWidgetItem(item);
		child->setText(0,QString("weight"));
}
void Main_crava::insertZone(){
  //the strings make it obvious what items are inserted. this creates all the needed children.
	QTreeWidgetItem* zone;
	findCorrectZone(&zone);

	int childNumber = zone->parent()->indexOfChild(zone);

        QString label = QString("zone");
	QTreeWidgetItem* parent = background_multizone_modelPointer;//move to the parent
	QTreeWidgetItem* precedingItem = background_multizone_modelPointer->child(childNumber-1);//which node will it precede.
	QTreeWidgetItem* item = new QTreeWidgetItem(parent,precedingItem);//insert another zone

	item->setText(0,label);
                QTreeWidgetItem* child = new QTreeWidgetItem(item);
		child->setText(0,QString("base-surface-file"));
		child = new QTreeWidgetItem(item);
		child->setText(0,QString("erosion-priority"));
		child = new QTreeWidgetItem(item);
		child->setText(0,QString("correlation-structure"));
		child = new QTreeWidgetItem(item);
		child->setText(0,QString("surface-uncertainty"));
}

void Main_crava::addZone(){
  //the strings make it obvious what items are added. this creates all the needed children.
        QString label = QString("zone");
	QTreeWidgetItem* parent = background_multizone_modelPointer;//move to the parent
	QTreeWidgetItem* item = new QTreeWidgetItem(parent);//insert another zone
	item->setText(0,label);
                QTreeWidgetItem* child = new QTreeWidgetItem(item);
		child->setText(0,QString("base-surface-file"));
		child = new QTreeWidgetItem(item);
		child->setText(0,QString("erosion-priority"));
		child = new QTreeWidgetItem(item);
		child->setText(0,QString("correlation-structure"));
		child = new QTreeWidgetItem(item);
		child->setText(0,QString("surface-uncertainty"));
}

void Main_crava::addFacies(){
	//the strings makes it obvious what items are added. this creates all the needed children.
	QString label = QString("facies");
	QTreeWidgetItem* parent = prior_probabilitesPointer;//moves down the tree to prior-probabilities
	QTreeWidgetItem* item = new QTreeWidgetItem(parent);//adds another facies
	item->setText(0,label);
		QTreeWidgetItem* child = new QTreeWidgetItem(item);//adds first child to the stack, name
		child->setText(0,QString("name"));
		child=new QTreeWidgetItem(item);
		child->setText(0,QString("probability"));
		child=new QTreeWidgetItem(item);
		child->setText(0,QString("probability-cube"));
}

void Main_crava::on_stackListWidget_currentRowChanged ( int currentRow ){
	//survey=1 angle-gather = 2+stackListWidget->currentRow()
	if(currentRow == -1){
		seismicDataFrame->setEnabled(false);
		deleteStackPushButton->setEnabled(false);
		applyToAllStacksPushButton->setEnabled(false);
		seismicDataFrame->setEnabled(false);
		waveletFrame->setVisible(false);
		angleLineEdit->setStyleSheet("");
		return;
	}

	//debugLabel->setText(QString("%1").arg(currentRow));
	//moves down the tree to survey
	QTreeWidgetItem* item;
	//QTreeWidgetItem** tempItem;
	//tempItem = &item;
	//findCorrectAngleGather(tempItem);
	findCorrectAngleGather(&item); //get the correct angle gather
		//move to offset-angle
		QString offsetAngle;
		getValueFromAngleGather( item, QString("offset-angle"), offsetAngle );
		angleLineEdit->setText(offsetAngle);//sets the correct offset angle
		//move to seismic data
			QString angleGatherFileName;
			getValueFromAngleGather( item, QString("file-name"), angleGatherFileName, QString("seismic-data") );
			//move to file-name
			seismicNameLabel->setText(StandardStrings::strippedName(angleGatherFileName));
			//move to start-time
			QString startTime;
			getValueFromAngleGather(item, QString("start-time"), startTime);
			startTimeLineEdit->setText(startTime);
			//move to segy-format
			//move to standard-format
			QString standardFormat;
			getValueFromAngleGather(item, QString("standard-format"), standardFormat);
			if(standardFormat.isEmpty()){
				//need to check bypass coordinate first as it is checked when the user defined button is checked
				//needs to check if all are empty, if they are, auto-detect
				//move to location-x
				QString xCoord;
				getValueFromAngleGather(item, QString("location-x"), xCoord);

				//move to location-y
				QString yCoord;
				getValueFromAngleGather(item, QString("location-y"), yCoord);

				//move to location-il
				QString inLine;
				getValueFromAngleGather(item, QString("location-il"), inLine);

				//move to location-xl
				QString crossline;
				getValueFromAngleGather(item, QString("location-xl"), crossline);

				//move to bypass-coordinate-scaling
				QString bypassCoordinate;
				getValueFromAngleGather(item, QString("bypass-coordinate-scaling"), bypassCoordinate);

				//move to location-scaling-coefficient
				QString locationScaling;
				getValueFromAngleGather(item, QString("location-scaling-coefficient"), locationScaling);

				if((!xCoord.isEmpty()) || (!yCoord.isEmpty()) || (!inLine.isEmpty()) || (!crossline.isEmpty()) ||
				  (!bypassCoordinate.isEmpty()) || (!locationScaling.isEmpty())){
					headerUserDefinedRadioButton->setChecked(true);
					xCoordLineEdit->setText(xCoord);
					yCoordLineEdit->setText(yCoord);
					inlineLineEdit->setText(inLine);
					crosslineLineEdit->setText(crossline);
					if(bypassCoordinate==QString("yes")){
						bypassCoordinateYesRadioButton->setChecked(true);
					}
					else if(bypassCoordinate==QString("no")){
						bypassCoordinateNoRadioButton->setChecked(true);
					}
					else if(bypassCoordinate.isEmpty()){
						bypassCoordinateEstimateRadioButton->setChecked(true);
					}
					locationScalingLineEdit->setText(locationScaling);
				}
				else {//default
					headerAutoDetectRadioButton->setChecked(true);
				}
			}
			else if(standardFormat==QString("seisworks")){
				headerSeisWorksRadioButton->setChecked(true);
			}
			else if(standardFormat==QString("iesx")){
				headerIesxRadioButton->setChecked(true);
			}
			else if(standardFormat==QString("SIP")){
				headerSipRadioButton->setChecked(true);
			}
			else if(standardFormat==QString("charisma")){
				headerCharismaRadioButton->setChecked(true);
			}
			if(!forwardMode()){//no pp/ps in forwardmode
				//move to type
				QString type;
				getValueFromAngleGather(item, QString("type"), type);
				if(type==QString("ps")){
					psRadioButton->setChecked(true);
				}
				else {
					ppRadioButton->setChecked(true);
				}
			}
		//move to wavelet
			//since checking the wave button have the possiblity of affecting local wavelets (can't give a local wavelet if normal wavelet is estimated, these values need to be stored
			//move to local-wavelet
				//move to shift-file
				QString shift;
				getValueFromAngleGather(item, QString("shift-file"), shift);

				//move to scale-file
				QString scaleFile;
				getValueFromAngleGather(item, QString("scale-file"), scaleFile);

				//move to estimate-shift
				QString eShift;
				getValueFromAngleGather(item, QString("estimate-shift"), eShift);
				
				//move to estimate-scale
				QString localWaveletEstimateScale;
				getValueFromAngleGather( item, QString("estimate-scale"), localWaveletEstimateScale, QString("local-wavelet") ); //local-wavelet is the parent
				//QString eScale=QString(nestedChildChild->text(1));
			//move to file-name
			QString waveletFileName;
			getValueFromAngleGather(item, QString("file-name"), waveletFileName, QString("wavelet") ); //wavelet is the parent 
			if(waveletFileName.isEmpty()){
				estimateWaveCheckBox->setChecked(true);
				fileScaleRadioButton->setChecked(true);
			}
			else {
				estimateWaveCheckBox->setChecked(false);//have to change when changed to checkbox
				waveletFileLineEdit->setText(waveletFileName);
				waveletFrame->setVisible(true);
				waveletFrame->setEnabled(true);

				QString scale;
				getValueFromAngleGather(item, QString("scale"), scale);
				//move to scale
				if(!scale.isEmpty()){
					scaleLineEdit->setText(scale);//have to set the line edit first otherwise the number in the box overwrite the current
					manualScaleRadioButton->setChecked(true);
				}
				else {
					//move to estimate-scale
					QString estimateScale;
					getValueFromAngleGather( item, QString("estimate-scale"), estimateScale, QString("wavelet") ); //wavelet is the parent
					if(!estimateScale.isEmpty()){//estimate-scale is never no, just yes or empty
						estimateScaleRadioButton->setChecked(true);
					}
					else {
						fileScaleRadioButton->setChecked(true);
					}
				}
			}
				//options here for neither either only scale or only shift or both, local wavelet
				if ((!shift.isEmpty()) && (!scaleFile.isEmpty())) {
					localWaveletCheckBox->setChecked(true);
					shiftLocalWaveletCheckBox->setChecked(false);
					scaleLocalWaveletCheckBox->setChecked(false);
					shiftFileLineEdit->setText(shift);
					scaleFileLineEdit->setText(scaleFile);
				}
				else if (!shift.isEmpty()) {
					localWaveletCheckBox->setChecked(true);
					shiftLocalWaveletCheckBox->setChecked(false);
					scaleLocalWaveletCheckBox->setChecked((localWaveletEstimateScale==QString("yes")));
					shiftFileLineEdit->setText(shift);
				}
				else if (!scaleFile.isEmpty()) {
					localWaveletCheckBox->setChecked(true);
					shiftLocalWaveletCheckBox->setChecked((eShift==QString("yes")));
					scaleLocalWaveletCheckBox->setChecked(false);
					scaleFileLineEdit->setText(scaleFile);
				}
				else {
					if ((eShift==QString("yes")) || (localWaveletEstimateScale==QString("yes"))) {
						localWaveletCheckBox->setChecked(true);
						shiftLocalWaveletCheckBox->setChecked(eShift==QString("yes"));
						scaleLocalWaveletCheckBox->setChecked(localWaveletEstimateScale==QString("yes"));
						
					}
					else {
						localWaveletCheckBox->setChecked(false);
					}
				}
				//needed to show/hide the inputs in case the buttons does not toggle
				on_shiftLocalWaveletCheckBox_toggled(shiftLocalWaveletCheckBox->isChecked());
				on_scaleLocalWaveletCheckBox_toggled(scaleLocalWaveletCheckBox->isChecked());
				if(forwardMode()){
					waveletFileLabel->setVisible(true);
					waveletFileLineEdit->setVisible(true);
					waveletBrowsePushButton->setVisible(true);
					waveletFrame->setVisible(true);
					waveletFileLabel->setVisible(true);
					waveletFileLineEdit->setEnabled(true);
					waveletBrowsePushButton->setEnabled(true);
					waveletFrame->setEnabled(true);
					//file-name
					waveletFileLineEdit->setText(waveletFileName);
				}
		//move to wavelet-3d doesn't do anything yet
		//move to match-energies				
		QString matchEnergies;
		getValueFromAngleGather(item, QString("match-energies"), matchEnergies);
		matchEnergiesCheckBox->setChecked(!matchEnergies.isEmpty());//this only works if match energies remains as yes - "" , maybe better to check with if
		
		//move to signal-to-noise-ratio
		QString signalToNoiseRatio;
		getValueFromAngleGather(item, QString("signal-to-noise-ratio"), signalToNoiseRatio);
		if(signalToNoiseRatio.isEmpty()){
			signalToNoiseCheckBox->setChecked(true);
		}
		else {//crashes if the buttons are checked before the text is set...
			signalToNoiseLineEdit->setText(signalToNoiseRatio);//fix the field
			signalToNoiseCheckBox->setChecked(false);
		}
		//fix radio buttons
		//move to local-noise-scaled
		QString localNoise;
		getValueFromAngleGather(item, QString("local-noise-scaled"), localNoise);
		//move to estimate-local-noise
		QString estimateLocalNoise;
		getValueFromAngleGather(item, QString("estimate-local-noise"), estimateLocalNoise);
		if(localNoise.isEmpty() && estimateLocalNoise.isEmpty()){
			localNoiseCheckBox->setChecked(false);
		}
		else {
			localNoiseCheckBox->setChecked(true);
			localNoiseFileLineEdit->setText(localNoise);
			localNoiseEstimateCheckBox->setChecked(localNoise.isEmpty());
		}
	seismicDataFrame->setEnabled(true);//needs to be enabled if there is an object selected
	angleLineEdit->setFocus();
	deleteStackPushButton->setEnabled(true);
	applyToAllStacksPushButton->setEnabled(true);
}

void Main_crava::on_addStackPushButton_clicked(){
	//can select multiple files, loops through them all and adds them.
	if(!forwardMode()){
		QStringList files=QFileDialog::getOpenFileNames ( this, QString("Open Files"), 
					standard->StandardStrings::inputPath(), StandardStrings::seismicFormat());
		QStringList list = files;//should operate on a copy of the file list according to Qt documentation but does not explain why.
		QStringList::Iterator fileName = list.begin();
		while(fileName != list.end()) {
			if(!fileName->isNull()){
				stackListWidget->addItem( StandardStrings::strippedName(*fileName) );
				addStack();
				stackListWidget->setCurrentItem(stackListWidget->item(stackListWidget->count()-1));
				seismicFile(*fileName);
			}
			++fileName;
		}
	}
	else{
		stackListWidget->addItem(QString("Synthetic seismic angle"));//synthetic_seismic_angle_10
		addStack();
		stackListWidget->setCurrentItem(stackListWidget->item(stackListWidget->count()-1));
	}
} 

void Main_crava::on_deleteStackPushButton_clicked(){
	QTreeWidgetItem* angleGather;
	findCorrectAngleGather(&angleGather);

	int childNumber = angleGather->parent()->indexOfChild(angleGather);
	int deleteIndex=stackListWidget->currentRow();
	if(deleteIndex==0&&stackListWidget->count()>1){
		stackListWidget->setCurrentItem(stackListWidget->item(1));
	}
	delete surveyPointer->child(childNumber);
	delete stackListWidget->takeItem(deleteIndex);
};//removes the selected stack, should be undoable

void Main_crava::seismicFile(const QString &value){
	if(stackListWidget->count()>0){
		QTreeWidgetItem* angleGather;
		findCorrectAngleGather(&angleGather);

		if (standard->StandardStrings::fileExists(value)){//only existing files should be in the tree
			setValueInAngleGather( angleGather, QString("file-name"), standard->StandardStrings::relativeFileName(value), QString("seismic-data") );
			seismicNameLabel->setText(StandardStrings::strippedName(value));
			stackListWidget->currentItem()->setText(StandardStrings::strippedName(value));
			seismicFileLineEdit->setText( standard->StandardStrings::relativeFileName(value) );
		}
	}
}

void Main_crava::on_seismicFileLineEdit_editingFinished(){
	seismicFile(seismicFileLineEdit->text());
};//update the XML three with the file if it is correct, autocomplete would be nice, seismic stack file

void Main_crava::on_seismicBrowsePushButton_clicked(){
	QString fileName = QFileDialog::getOpenFileName(this, QString("Open File"), standard->StandardStrings::inputPath(), StandardStrings::seismicFormat());
	if(!fileName.isNull()){
		seismicFile(fileName);
// 		seismicFileLineEdit->setText(fileName);
	}
}//browse for the seismic stack file then update the XML file, update the field

void Main_crava::on_angleLineEdit_editingFinished(){
	if(stackListWidget->count()>0){
		QTreeWidgetItem* angleGather;
		findCorrectAngleGather(&angleGather);
		QString value = angleLineEdit->text();
		setValueInAngleGather(angleGather, QString("offset-angle"), value);
		if(forwardMode()){//need to change the displayed name in forward mode
			stackListWidget->currentItem()->setText(QString("Synthetic seismic angle %1").arg(value));
		}
	}
};//update the offset angle for the selected stack in the XML file

void Main_crava::on_applyToAllStacksPushButton_clicked(){

	QList<QTreeWidgetItem*> angleGathers = getAllAngleGathers();
	for (int i = 0; i<angleGathers.count(); i++){
		QTreeWidgetItem* angleGather;
		findCorrectAngleGather(&angleGather);

		//start time
		QString startTime;
		getValueFromAngleGather(angleGather, QString("start-time"), startTime);
		setValueInAngleGather(angleGathers.at(i), QString("start-time"), startTime);

		//segy-format
		QString standardFormat;
		getValueFromAngleGather(angleGather, QString("standard-format"), standardFormat);
		setValueInAngleGather(angleGathers.at(i), QString("standard-format"), standardFormat);

		QString locationX;
		getValueFromAngleGather(angleGather, QString("location-x"), locationX);
		setValueInAngleGather(angleGathers.at(i), QString("location-x"), locationX);

		QString locationY;
		getValueFromAngleGather(angleGather, QString("location-y"), locationY);
		setValueInAngleGather(angleGathers.at(i), QString("location-y"), locationY);

		QString locationIl;
		getValueFromAngleGather(angleGather, QString("location-il"), locationIl);
		setValueInAngleGather(angleGathers.at(i), QString("location-il"), locationIl);

		QString locationXl;
		getValueFromAngleGather(angleGather, QString("location-xl"), locationXl);
		setValueInAngleGather(angleGathers.at(i), QString("location-xl"), locationXl);

		QString bypassCoordinateScaling;
		getValueFromAngleGather(angleGather, QString("bypass-coordinate-scaling"), bypassCoordinateScaling);
		setValueInAngleGather(angleGathers.at(i), QString("bypass-coordinate-scaling"), bypassCoordinateScaling);

		QString locationScalingCoefficient;
		getValueFromAngleGather(angleGather, QString("location-scaling-coefficient"), locationScalingCoefficient);
		setValueInAngleGather(angleGathers.at(i), QString("location-scaling-coefficient"), locationScalingCoefficient);

		//type
		QString type;
		getValueFromAngleGather(angleGather, QString("type"), type);
		setValueInAngleGather(angleGathers.at(i), QString("type"), type);

		//wavelet
		QString waveletFileName;
		getValueFromAngleGather( angleGather, QString("file-name"), waveletFileName, QString("wavelet") );
		QString waveletFileNameInList;
		getValueFromAngleGather( angleGathers.at(i), QString("file-name"), waveletFileNameInList, QString("wavelet") );
		if ( waveletFileNameInList.isEmpty() ){ //only changes the wavelet filename if the current filename is empty.
			setValueInAngleGather( angleGathers.at(i), QString("file-name"), waveletFileName, QString("wavelet") );
		}

		QString scale;
		getValueFromAngleGather(angleGather, QString("scale"), scale);
		setValueInAngleGather(angleGathers.at(i), QString("scale"), scale);

		QString waveletEstimateScale;
		getValueFromAngleGather( angleGather, QString("estimate-scale"), waveletEstimateScale, QString("wavelet") );
		setValueInAngleGather( angleGathers.at(i), QString("estimate-scale"), waveletEstimateScale, QString("wavelet") );

		//local-wavelet
		QString shiftFile;
		getValueFromAngleGather(angleGather, QString("shift-file"), shiftFile);
		setValueInAngleGather(angleGathers.at(i), QString("shift-file"), shiftFile);

		QString scaleFile;
		getValueFromAngleGather(angleGather, QString("scale-file"), scaleFile);
		setValueInAngleGather(angleGathers.at(i), QString("scale-file"), scaleFile);

		QString estimateShift;
		getValueFromAngleGather(angleGather, QString("estimate-shift"), estimateShift);
		setValueInAngleGather(angleGathers.at(i), QString("estimate-shift"), estimateShift);

		QString localWaveletEstimateScale;
		getValueFromAngleGather( angleGather, QString("estimate-scale"), localWaveletEstimateScale, QString("local-wavelet") );
		setValueInAngleGather( angleGathers.at(i), QString("estimate-scale"), localWaveletEstimateScale, QString("local-wavelet") );

 		//wavelet-3d
		QString wavelet3d;
		getValueFromAngleGather(angleGather, QString("wavelet-3d"), wavelet3d);
		setValueInAngleGather(angleGathers.at(i), QString("wavelet-3d"), wavelet3d);

		//match-energies
		QString matchEnergies;
		getValueFromAngleGather(angleGather, QString("match-energies"), matchEnergies);
		setValueInAngleGather(angleGathers.at(i), QString("match-energies"), matchEnergies);

		//signal-to-noise-ratio
		QString signalToNoiseRatio;
		getValueFromAngleGather(angleGather, QString("signal-to-noise-ratio"), signalToNoiseRatio);
		setValueInAngleGather(angleGathers.at(i), QString("signal-to-noise-ratio"), signalToNoiseRatio);

		//local-noise-scaled
		QString localNoiseScaled;
		getValueFromAngleGather(angleGather, QString("local-noise-scaled"), localNoiseScaled);
		setValueInAngleGather(angleGathers.at(i), QString("local-noise-scaled"), localNoiseScaled);

		//estimate-local-noise
		QString estimateLocalNoise;
		getValueFromAngleGather(angleGather, QString("estimate-local-noise"), estimateLocalNoise);
		setValueInAngleGather(angleGathers.at(i), QString("estimate-local-noise"), estimateLocalNoise);
	}

}

//PP - PS buttons 
void Main_crava::on_ppRadioButton_toggled(bool checked){
	if(stackListWidget->count()>0){
		QTreeWidgetItem* angleGather;
		findCorrectAngleGather(&angleGather);

		QString value;
		if(checked){//handle both options.
			value = "pp";
		}
		else {
			value = "ps";
		}
		setValueInAngleGather(angleGather, QString("type"), value);
	}
}//only one slot needed

void Main_crava::on_startTimeLineEdit_editingFinished(){
	if(stackListWidget->count()>0){
		QTreeWidgetItem* angleGather;
		findCorrectAngleGather(&angleGather);
		QString value = startTimeLineEdit->text();
		setValueInAngleGather(angleGather, QString("start-time"), value);
	}
}//update the start time for the selected stack in the XML file

	//header format information buttons
void Main_crava::headerFormat(const QString &value){
	if(stackListWidget->count()>0){
		QTreeWidgetItem* angleGather;
		findCorrectAngleGather(&angleGather);
		setValueInAngleGather(angleGather, QString("standard-format"), value);
	}
}

void Main_crava::on_headerAutoDetectRadioButton_toggled(bool checked){
	if(checked){
		headerFormat(QString(""));
	}
}

void Main_crava::on_headerSeisWorksRadioButton_toggled(bool checked){
	if(checked){
		headerFormat(QString("seisworks"));
	}
}

void Main_crava::on_headerIesxRadioButton_toggled(bool checked){
	if(checked){
		headerFormat(QString("iesx"));
	}
}

void Main_crava::on_headerSipRadioButton_toggled(bool checked){
	if(checked){
		headerFormat(QString("SIP"));
	}
}

void Main_crava::on_headerCharismaRadioButton_toggled(bool checked){
	if(checked){
		headerFormat(QString("charisma"));
	}
}

void Main_crava::on_headerUserDefinedRadioButton_toggled(bool checked){
	formatChangeFrame->setVisible(checked);
	formatChangeFrame->setEnabled(checked);
	if(checked){
		if ( bypassCoordinateYesRadioButton->isChecked() ){
			locationScalingLineEdit->setVisible(true);
			location_scaling_label->setVisible(true);
		}
		else{
			locationScalingLineEdit->setVisible(false);
			QTreeWidgetItem* angleGather;
			findCorrectAngleGather(&angleGather);	
			setValueInAngleGather( angleGather, QString("location-scaling-coefficient"), QString("") );
			location_scaling_label->setVisible(false);
		}
		headerFormat(QString(""));
		bypassCoordinateEstimateRadioButton->setChecked(true);//might be better to check what is actually in the tree for better modibility
		xCoordLineEdit->setFocus();
	}
	else {
		//clears the user defined settings if the button is deselected
		QTreeWidgetItem* angleGather;
		findCorrectAngleGather(&angleGather);

		setValueInAngleGather( angleGather, QString("standard-format"), QString("") );
		setValueInAngleGather( angleGather, QString("location-x"), QString("") );
		setValueInAngleGather( angleGather, QString("location-y"), QString("") );
		setValueInAngleGather( angleGather, QString("location-il"), QString("") );
		setValueInAngleGather( angleGather, QString("location-xl"), QString("") );
		setValueInAngleGather( angleGather, QString("bypass-coordinate-scaling"), QString("") );
		setValueInAngleGather( angleGather, QString("location-scaling-coefficient"), QString("") );

		QList<QLineEdit*> fields=formatChangeFrame->QObject::findChildren<QLineEdit*>();//this causes some sort of warning... bad cast of void pointer with qt 4.2?
		foreach (QLineEdit* field, fields){
			field->clear();
		}
	}
	//should enable editing
}

void Main_crava::on_xCoordLineEdit_editingFinished(){
	if(stackListWidget->count()>0){
		QTreeWidgetItem* angleGather;
		findCorrectAngleGather(&angleGather);
		QString value = xCoordLineEdit->text();
		setValueInAngleGather(angleGather, QString("location-x"), value);
	}
}//update the XML file with the new x coordinate format for the header

void Main_crava::on_yCoordLineEdit_editingFinished(){
	if(stackListWidget->count()>0){
		QTreeWidgetItem* angleGather;
		findCorrectAngleGather(&angleGather);
		QString value = yCoordLineEdit->text();
		setValueInAngleGather(angleGather, QString("location-y"), value);
	}
}//update the XML file with the new y coordinate format for the header

void Main_crava::on_crosslineLineEdit_editingFinished(){
	if(stackListWidget->count()>0){
		QTreeWidgetItem* angleGather;
		findCorrectAngleGather(&angleGather);
		QString value = crosslineLineEdit->text();
		setValueInAngleGather(angleGather, QString("location-xl"), value);
	}
};//update the XML file with the new crossline format for the header

void Main_crava::on_inlineLineEdit_editingFinished(){
	if(stackListWidget->count()>0){
		QTreeWidgetItem* angleGather;
		findCorrectAngleGather(&angleGather);
		QString value = inlineLineEdit->text();
		setValueInAngleGather(angleGather, QString("location-il"), value);
	}
}//update the XML file with the new inline format for the header

//bypass coordinate information buttons
void Main_crava::bypassCoordinate(const QString &value){
	if(stackListWidget->count()>0){
		QTreeWidgetItem* angleGather;
		findCorrectAngleGather(&angleGather);
		setValueInAngleGather(angleGather, QString("bypass-coordinate-scaling"), value);
	}
}

void Main_crava::on_bypassCoordinateYesRadioButton_toggled(bool checked){
	locationScalingLineEdit->setVisible(checked);
	location_scaling_label->setVisible(checked);
	if(checked){
		bypassCoordinate(QString("yes"));
	}
	else{
		QTreeWidgetItem* angleGather;
		findCorrectAngleGather(&angleGather);	
		setValueInAngleGather( angleGather, QString("location-scaling-coefficient"), QString("") );
	}
}

void Main_crava::on_bypassCoordinateNoRadioButton_toggled(bool checked){
	if(checked){
		bypassCoordinate(QString("no"));
	}
}

void Main_crava::on_bypassCoordinateEstimateRadioButton_toggled(bool checked){
	if(checked){
		bypassCoordinate(QString(""));
	}
}

void Main_crava::on_locationScalingLineEdit_editingFinished(){
	if(stackListWidget->count()>0){
		QTreeWidgetItem* angleGather;
		findCorrectAngleGather(&angleGather);
		QString value = locationScalingLineEdit->text();
		setValueInAngleGather(angleGather, QString("location-scaling-coefficient"), value);
	}
};//update the XML file with the location scaling coefficient, integer

//buttons global wavelet
void Main_crava::on_estimateWaveCheckBox_toggled(bool checked){
	//would be dangerous to call this without any items in the stack if(stackListWidget->count()>0)
	//can only give local wavelet if global wavelet is given, but can estimate it no matter.
	waveletFrame->setVisible(!checked);
	waveletFrame->setEnabled(!checked);
	if(checked){
		QString fileName=QString("");
		waveletFile(fileName);
		waveletFileLineEdit->setText(fileName);
		fileScaleRadioButton->setChecked(true);
		shiftFileLabel->setVisible(false);
		shiftFileLineEdit->setVisible(false);
		shiftFileBrowsePushButton->setVisible(false);
		shiftFileLabel->setEnabled(false);
		shiftFileLineEdit->setEnabled(false);
		shiftFileBrowsePushButton->setEnabled(false);
		shiftFile(QString(""));
		shiftFileLineEdit->setText(QString(""));
		scaleFileLabel->setVisible(false);
		scaleFileLineEdit->setVisible(false);
		scaleFileBrowsePushButton->setVisible(false);
		scaleFileLabel->setEnabled(false);
		scaleFileLineEdit->setEnabled(false);
		scaleFileBrowsePushButton->setEnabled(false);
		scaleFile(QString(""));
		scaleFileLineEdit->setText(QString(""));
	}
	else{
		QTreeWidgetItem* angleGather;
		findCorrectAngleGather(&angleGather);

		waveletFileLineEdit->setFocus();
		//local wavelet scale and shift can only be given if global wavelet is given, if global wavelet is estimated, they can be estimated.
		QString estimateShift;
		getValueFromAngleGather(angleGather, QString("estimate-shift"), estimateShift);
		if( estimateShift == QString("yes") ){
			shiftFileLabel->setVisible(false);
			shiftFileLineEdit->setVisible(false);
			shiftFileBrowsePushButton->setVisible(false);
			shiftFileLabel->setEnabled(false);
			shiftFileLineEdit->setEnabled(false);
			shiftFileBrowsePushButton->setEnabled(false);
			shiftFile(QString(""));
			shiftFileLineEdit->setText(QString(""));
		}
		else {
			shiftFileLabel->setVisible(true);
			shiftFileLineEdit->setVisible(true);
			shiftFileBrowsePushButton->setVisible(true);
			shiftFileLabel->setEnabled(true);
			shiftFileLineEdit->setEnabled(true);
			shiftFileBrowsePushButton->setEnabled(true);
		}
		//local wavelet scale and shift can only be given if global wavelet is given, if global wavelet is estimated, they can be estimated.
		QString localWaveletEstimateScale;
		getValueFromAngleGather( angleGather, QString("estimate-scale"), localWaveletEstimateScale, QString("local-wavelet") );
		if( localWaveletEstimateScale == QString("yes") ){
			scaleFileLabel->setVisible(false);
			scaleFileLineEdit->setVisible(false);
			scaleFileBrowsePushButton->setVisible(false);
			scaleFileLabel->setEnabled(false);
			scaleFileLineEdit->setEnabled(false);
			scaleFileBrowsePushButton->setEnabled(false);
			scaleFile(QString(""));
			scaleFileLineEdit->setText(QString(""));
		}
		else {
			scaleFileLabel->setVisible(true);
			scaleFileLineEdit->setVisible(true);
			scaleFileBrowsePushButton->setVisible(true);
			scaleFileLabel->setEnabled(true);
			scaleFileLineEdit->setEnabled(true);
			scaleFileBrowsePushButton->setEnabled(true);
		}
	}
}
//need to implement 3D wavelet
void Main_crava::waveletFile(const QString &value){
	if(stackListWidget->count()>0){
		QTreeWidgetItem* angleGather;
		findCorrectAngleGather(&angleGather);
		if (standard->StandardStrings::fileExists(value)){
			setValueInAngleGather(angleGather, QString("file-name"), standard->StandardStrings::relativeFileName(value), QString("wavelet"));
		}
	}
}
void Main_crava::on_waveletFileLineEdit_editingFinished(){
	waveletFile(waveletFileLineEdit->text());
}//update the XML three with the file if it is correct, wavelet file for the seieismic stack

void Main_crava::on_waveletBrowsePushButton_clicked(){
	QString fileName = QFileDialog::getOpenFileName(this, QString("Open File"), standard->StandardStrings::inputPath(), StandardStrings::waveletFormat());
	if(!fileName.isNull()){
		waveletFile(fileName);
		waveletFileLineEdit->setText(fileName);
	}
}//browse for the wavelet file then update the XML file, update the field

void Main_crava::on_fileScaleRadioButton_toggled(bool checked){
	if(checked){
		scaleLineEdit->setVisible(false);
		scaleLineEdit->setEnabled(false);
	}
}

void Main_crava::on_estimateScaleRadioButton_toggled(bool checked){
	if(stackListWidget->count()>0){
		QTreeWidgetItem* angleGather;
		findCorrectAngleGather(&angleGather);
		QString value;
		if(checked){
			value=QString("yes");
		}
		else {
			value=QString("");//to be able to give scale this has to be empty.
		}
		setValueInAngleGather(angleGather, QString("estimate-scale"), value, QString("wavelet"));
	}
}//update whether global scale should be estimated in the XML file

void Main_crava::on_manualScaleRadioButton_toggled(bool checked){
	scaleLineEdit->setVisible(checked);
	scaleLineEdit->setEnabled(checked);
	QString value;
	if(checked){
		scaleLineEdit->setFocus();
	}
	else{//clear if unchecked
		if(stackListWidget->count()>0){
			QTreeWidgetItem* angleGather;
			findCorrectAngleGather(&angleGather);
			value = QString("");
			scaleLineEdit->setText(value);
			setValueInAngleGather(angleGather, QString("scale"), value);
		}
	}
}

void Main_crava::on_scaleLineEdit_editingFinished(){
	if(stackListWidget->count()>0){
		QTreeWidgetItem* angleGather;
		findCorrectAngleGather(&angleGather);
		QString value = scaleLineEdit->text();
		setValueInAngleGather(angleGather, QString("scale"), value);
	}
}//update the scale for the given wavelet for the selected stack in the XML file

void Main_crava::on_localWaveletCheckBox_toggled(bool checked){
	localWaveletFrame->setEnabled(checked);
	shiftLocalWaveletCheckBox->setChecked(checked);
	scaleLocalWaveletCheckBox->setChecked(checked);
	localWaveletFrame->setVisible(checked);
	if(!checked){
		scaleFile(QString(""));
		shiftFile(QString(""));
	}
}

void Main_crava::on_shiftLocalWaveletCheckBox_toggled(bool checked){
	//can only give local wavelet if global wavelet is given, but can estimate it no matter.
	if(estimateWaveCheckBox->isChecked()){//can only be given if global wavelet is given
		shiftFileLabel->setVisible(false);
		shiftFileLineEdit->setVisible(false);
		shiftFileBrowsePushButton->setVisible(false);
		shiftFileLabel->setEnabled(false);
		shiftFileLineEdit->setEnabled(false);
		shiftFileBrowsePushButton->setEnabled(false);
		shiftFile(QString(""));
		shiftFileLineEdit->setText(QString(""));
	}
	else{
		shiftFileLabel->setVisible(!checked);
		shiftFileLineEdit->setVisible(!checked);
		shiftFileBrowsePushButton->setVisible(!checked);
		shiftFileLabel->setEnabled(!checked);
		shiftFileLineEdit->setEnabled(!checked);
		shiftFileBrowsePushButton->setEnabled(!checked);
	}
	if(stackListWidget->count()>0){
		QTreeWidgetItem* angleGather;
		findCorrectAngleGather(&angleGather);
		QString value;
		if(checked){//can't both give file and estimate
			shiftFile(QString(""));
			value=QString("yes");
			shiftFileLineEdit->setText(QString(""));
		}
		else {
			value=QString("");
			shiftFileLineEdit->setFocus();
		}
		setValueInAngleGather(angleGather, QString("estimate-shift"), value);
	}
}//update whether local wavelet should be estimated in the XML file, also disable/enable adding files

void Main_crava::on_scaleLocalWaveletCheckBox_toggled(bool checked){
	//can only give local wavelet if global wavelet is given, but can estimate it no matter.
	if(estimateWaveCheckBox->isChecked()){//can only be given if global wavelet is given
		scaleFileLabel->setVisible(false);
		scaleFileLineEdit->setVisible(false);
		scaleFileBrowsePushButton->setVisible(false);
		scaleFileLabel->setEnabled(false);
		scaleFileLineEdit->setEnabled(false);
		scaleFileBrowsePushButton->setEnabled(false);
		scaleFile(QString(""));
		scaleFileLineEdit->setText(QString(""));
	}
	else{
		scaleFileLabel->setVisible(!checked);
		scaleFileLineEdit->setVisible(!checked);
		scaleFileBrowsePushButton->setVisible(!checked);
		scaleFileLabel->setEnabled(!checked);
		scaleFileLineEdit->setEnabled(!checked);
		scaleFileBrowsePushButton->setEnabled(!checked);
	}
	if(stackListWidget->count()>0){//can't both give file and estimate
		QTreeWidgetItem* angleGather;
		findCorrectAngleGather(&angleGather);
		QString value;
		if(checked){
			scaleFile(QString(""));
			value=QString("yes");
			scaleFileLineEdit->setText(QString(""));
		}
		else {
			value=QString("");
			scaleFileLineEdit->setFocus();
		}
		setValueInAngleGather( angleGather, QString("estimate-scale"), value, QString("local-wavelet") );
	}
}

void Main_crava::shiftFile(const QString &value){
	if(stackListWidget->count()>0){
		QTreeWidgetItem* angleGather;
		findCorrectAngleGather(&angleGather);
		if (standard->StandardStrings::fileExists(value)){
			setValueInAngleGather( angleGather, QString("shift-file"), standard->StandardStrings::relativeFileName(value) );
		}
	}
}

void Main_crava::on_shiftFileLineEdit_editingFinished(){
	shiftFile(shiftFileLineEdit->text());
};//update the XML three with the file if it is correct, local wavelet shift file for the seieismic stack

void Main_crava::on_shiftFileBrowsePushButton_clicked(){
	QString fileName = QFileDialog::getOpenFileName(this, QString("Open File"), standard->StandardStrings::inputPath(), StandardStrings::surfaceFormat());
	if(!fileName.isNull()){
		shiftFile(fileName);
		shiftFileLineEdit->setText(fileName);
	}
}//browse for the local wavelet shift file then update the XML file, update the field

void Main_crava::scaleFile(const QString &value){
	if(stackListWidget->count()>0){
		QTreeWidgetItem* angleGather;
		findCorrectAngleGather(&angleGather);
		if (standard->StandardStrings::fileExists(value)){
			setValueInAngleGather( angleGather, QString("scale-file"), standard->StandardStrings::relativeFileName(value) );
		}
	}
}

void Main_crava::on_scaleFileLineEdit_editingFinished(){
	scaleFile(scaleFileLineEdit->text());
}//update the XML three with the file if it is correct, local wavelet scale file for the seieismic stack

void Main_crava::on_scaleFileBrowsePushButton_clicked(){
	QString fileName = QFileDialog::getOpenFileName(this, QString("Open File"), standard->StandardStrings::inputPath(), StandardStrings::surfaceFormat());
	if(!fileName.isNull()){
		scaleFile(fileName);
		scaleFileLineEdit->setText(fileName);
	}
}//browse for the local wavelet scale file then update the XML file, update the field

//signal to noise ratio buttons
void Main_crava::on_signalToNoiseCheckBox_toggled(bool checked){
	signalToNoiseLineEdit->setEnabled(!checked);
	if(checked){
		signalToNoiseLineEdit->setVisible(false);
		if(stackListWidget->count()>0){
			QTreeWidgetItem* angleGather;
			findCorrectAngleGather(&angleGather);
			QString value= QString("");
			signalToNoiseLineEdit->setText(value);
			setValueInAngleGather(angleGather, QString("signal-to-noise-ratio"),value);
		}
	}
	else{
		signalToNoiseLineEdit->setFocus();
		signalToNoiseLineEdit->setVisible(true);
	}
}

void Main_crava::on_signalToNoiseLineEdit_editingFinished(){
	if(stackListWidget->count()>0){
		QTreeWidgetItem* angleGather;
		findCorrectAngleGather(&angleGather);
		QString value = signalToNoiseLineEdit->text();
		setValueInAngleGather(angleGather, QString("signal-to-noise-ratio"),value);
	}
}//update the XML three with the signal to noise ratio

void Main_crava::on_localNoiseCheckBox_toggled(bool checked){
	//can either estimate or give, not both
	if(checked){
		localNoiseEstimateCheckBox->setChecked(true);
	}
	else {
		if(stackListWidget->count()>0){
			QTreeWidgetItem* angleGather;
			findCorrectAngleGather(&angleGather);
			QString value("");
			setValueInAngleGather(angleGather, QString("local-noise-scaled"), value);
			setValueInAngleGather(angleGather, QString("estimate-local-noise"), value);
		}
	}
	localNoiseFrame->setVisible(checked);
	localNoiseFrame->setEnabled(checked);
}//update the XML with whether local noise should be enabled and

//local noise radio buttons
void Main_crava::on_localNoiseEstimateCheckBox_toggled(bool checked){
	//can either estimate or give, not both
	localNoiseFileLineEdit->setVisible(!checked);
	localNoiseBrowsePushButton->setVisible(!checked);
	localNoiseFileLineEdit->setEnabled(!checked);
	localNoiseBrowsePushButton->setEnabled(!checked);
	if(stackListWidget->count()>0){
		QTreeWidgetItem* angleGather;
		findCorrectAngleGather(&angleGather);
		QString value;
		if(checked){//can't both give and estimate
			value=QString("yes");
			setValueInAngleGather(angleGather, QString("estimate-local-noise"), value);
			value=QString("");
			setValueInAngleGather(angleGather, QString("local-noise-scaled"), value);
			localNoiseFileLineEdit->setText(value);
		}
		else{
			value=QString("");
			setValueInAngleGather(angleGather, QString("estimate-local-noise"), value);
			localNoiseFileLineEdit->setFocus();
		}
	}
}

void Main_crava::localNoiseFile(const QString &value){
	if(stackListWidget->count()>0){
		QTreeWidgetItem* angleGather;
		findCorrectAngleGather(&angleGather);
		if (standard->StandardStrings::fileExists(value)){
			setValueInAngleGather( angleGather, QString("local-noise-scaled"), standard->StandardStrings::relativeFileName(value) );
		}
	}
}

void Main_crava::on_localNoiseFileLineEdit_editingFinished(){
	localNoiseFile(localNoiseFileLineEdit->text());
}//update the XML three with the file if it is correct, local noise file for the seieismic stack

void Main_crava::on_localNoiseBrowsePushButton_clicked(){
	QString fileName = QFileDialog::getOpenFileName(this, QString("Open File"), standard->StandardStrings::inputPath(), StandardStrings::seismicFormat());
	if(!fileName.isNull()){
		localNoiseFile(fileName);
		localNoiseFileLineEdit->setText(fileName);
	}
}//browse for the local noise file then update the XML file, update the field

void Main_crava::on_matchEnergiesCheckBox_toggled(bool checked){
	if(stackListWidget->count()>0){
		QTreeWidgetItem* angleGather;
		findCorrectAngleGather(&angleGather);
		QString value;
		if(checked){
			value=QString("yes");
		}
		else{
			value=QString("");
		}
		setValueInAngleGather(angleGather, QString("match-energies"), value);
	}
}//update whether energies should be matched in XML file for the selected stack

//non-stack survey
void Main_crava::on_defaultStartTimeLineEdit_editingFinished(){
	survey_segy_start_timePointer->setText( 1, defaultStartTimeLineEdit->text() );
}//update the XML three with the new start time

void Main_crava::on_angularCorrelationPushButton_clicked(){
	variogram(survey_angular_correlationPointer);
}//open variogram edit window

//wells, format info

void Main_crava::on_timeLineEdit_editingFinished(){
	log_names_timePointer->setText( 1, timeLineEdit->text() );
}//update the XML three with the new time format

void Main_crava::on_densityLineEdit_editingFinished(){
	log_names_densityPointer->setText(1 , densityLineEdit->text() );
}//update the XML three with the new density format

void Main_crava::on_faciesLineEdit_editingFinished(){
	log_names_faciesPointer->setText( 1, faciesLineEdit->text() );
}//update the XML three with the new facis format

//button vp
void Main_crava::on_vpRadioButton_toggled(bool checked){
	//makes sure the correct widgets are visible and the focus makes sure signals trigger to make the info in the tree correct.
	vpLineEdit->setVisible(checked);
	dtLineEdit->setVisible(!checked);
	vpLineEdit->setEnabled(checked);
	dtLineEdit->setEnabled(!checked);
	if(checked){
		vpLineEdit->setFocus();
	}
	else{
		dtLineEdit->setFocus();
	}
}
void Main_crava::on_vpLineEdit_editingFinished(){
	// need to implement removal of dt
	//can either have vp or dt but not both
	log_names_vpPointer->setText( 1, vpLineEdit->text() );
	//for removal of dt
	log_names_dtPointer->setText( 1, QString() );
}//update the XML three with the new vp format, remove dt
//button dt
void Main_crava::on_dtLineEdit_editingFinished(){
	// need to implement removal of vp
	//can either have vp or dt but not both
	log_names_dtPointer->setText( 1, dtLineEdit->text() );
	//for removal of vp
	log_names_vpPointer->setText( 1, QString() );
}//update the XML three with the new dt format, remove vp
//button vs
void Main_crava::on_vsRadioButton_toggled(bool checked){
	//can either have vs or dts but not both
	vsLineEdit->setVisible(checked);
	dtsLineEdit->setVisible(!checked);
	vsLineEdit->setEnabled(checked);	
	dtsLineEdit->setEnabled(!checked);
	if(checked){
		vsLineEdit->setFocus();
	}
	else{
		dtsLineEdit->setFocus();
	}
}
void Main_crava::on_vsLineEdit_editingFinished(){
	// need to implement removal of dts
	//can either have vs or dts but not both
	log_names_vsPointer->setText( 1, vsLineEdit->text() );
	//for removal of dts
	log_names_dtsPointer->setText( 1, QString() );
}//update the XML three with the new vs format, remove dts

//button dts
void Main_crava::on_dtsLineEdit_editingFinished(){
	// need to implement removal of vs
	//can either have vs or dts but not both
	log_names_dtsPointer->setText( 1, dtsLineEdit->text() );
	//for removal of vs
	log_names_vsPointer->setText( 1, QString() );
}//update the XML three with the new dts format, remove vs

//well input
void Main_crava::on_wellListWidget_currentRowChanged ( int currentRow ){

	if(currentRow==-1){
		wellFrame->setEnabled(false);
		deleteWellPushButton->setEnabled(false);
		wellHeaderPushButton->setEnabled(false);
		openWellPushButton->setEnabled(false);
		return;
	}
	//debugLabel->setText(QString("%1").arg(currentRow));

	QTreeWidgetItem* item;
	findCorrectWell(&item); //move to correct well

		//move to file-name
		QString fileName;
		getValueFromWell( item, QString("file-name"), fileName );
		wellFileLineEdit->setText(fileName);//sets the correct file-name
		wellNameLabel->setText(StandardStrings::strippedName(fileName));
		//move to use-for-wavelet-estimation
		QString useForWaveletEstimation;
		getValueFromWell(item, QString("use-for-wavelet-estimation"), useForWaveletEstimation);
		if(useForWaveletEstimation==QString("no")){
			waveletEstimationCheckBox->setChecked(false);
		}
		else {
			waveletEstimationCheckBox->setChecked(true);
		}

		QString useForBackgroundTrend;
		getValueFromWell(item, QString("use-for-background-trend"), useForBackgroundTrend);
		//move to use-for-background-trend
		if(useForBackgroundTrend==QString("no")){
			backgroundTrendCheckBox->setChecked(false);
		}
		else {
			backgroundTrendCheckBox->setChecked(true);
		}
		//move to use-for-facies-probabilities , probably need to facies are estimated
		QString useForFaciesProbabilities;
		getValueFromWell(item, QString("use-for-facies-probabilities"), useForFaciesProbabilities);
		if(useForFaciesProbabilities==QString("no")){
			faciesProbabilitiesCheckBox->setChecked(false);
		}
		else {
			faciesProbabilitiesCheckBox->setChecked(true);
		}
		//move to synthetic-vs-log
		QString syntheticVsLog;
		getValueFromWell(item, QString("synthetic-vs-log"), syntheticVsLog);

		if(syntheticVsLog==QString("no")){
			synteticVsNoRadioButton->setChecked(true);
		}
		else if(syntheticVsLog==QString("yes")){
			synteticVsYesRadioButton->setChecked(true);
		}
		else {
			synteticVsDetectRadioButton->setChecked(true);
		}
		//move to filter-elastic-logs
		QString filterElasticLogs;
		getValueFromWell(item, QString("filter-elastic-logs"), filterElasticLogs);
		if(filterElasticLogs==QString("yes")){
			filterElasticCheckBox->setChecked(true);
		}
		else {
			filterElasticCheckBox->setChecked(false);
		}
		optimizePositionListWidget->clear();
		if(6<item->childCount()){
			optimizePositionCheckBox->setChecked(true);
			for(int i=6;i<item->childCount();i++){//fills the optimize position list widget.
				optimizePositionListWidget->addItem(QString("optimization"));
				optimizePositionListWidget->setCurrentItem(optimizePositionListWidget->item(0));//trigger another signal
			}
		}
		else {
			optimizePositionCheckBox->setChecked(false);
			deleteOptimizationPushButton->setEnabled(false);
		}
	wellFrame->setEnabled(true);
	deleteWellPushButton->setEnabled(true);
	openWellPushButton->setEnabled(true);
	wellHeaderPushButton->setEnabled(true);
}
void Main_crava::on_addWellPushButton_clicked(){
	QStringList files=QFileDialog::getOpenFileNames ( this, QString("Open Files"), 
				standard->StandardStrings::inputPath(), StandardStrings::wellFormat());
	QStringList list = files;
	QStringList::Iterator fileName = list.begin();
	while(fileName != list.end()) {
		if(!fileName->isNull()){
			wellListWidget->addItem( StandardStrings::strippedName(*fileName) );
			addWell();
			wellListWidget->setCurrentItem(wellListWidget->item(wellListWidget->count()-1));
			wellFile(*fileName);
			on_waveletEstimationCheckBox_toggled(waveletEstimationCheckBox->isChecked());//updates xml tree
			on_backgroundTrendCheckBox_toggled(backgroundTrendCheckBox->isChecked());//updates xml tree
			on_faciesProbabilitiesCheckBox_toggled(faciesProbabilitiesCheckBox->isChecked());//updates xml tree
			on_filterElasticCheckBox_toggled(filterElasticCheckBox->isChecked());//updates xml tree
		}
		++fileName;
	}
}//add a new well for input
void Main_crava::on_deleteWellPushButton_clicked(){
	QTreeWidgetItem* well;
	findCorrectWell(&well);
	int childNumber = well->parent()->indexOfChild(well); //the child index of the well

	int deleteIndex=wellListWidget->currentRow();
	if(deleteIndex==0&&wellListWidget->count()>1){
		wellListWidget->setCurrentItem(wellListWidget->item(1));
	}
	else if(deleteIndex==0){
		wellListWidget->setCurrentRow(-1); //this should not be needed as when there are no items in the list no items should be selected and current row should be -1...
	}
	delete well_dataPointer->child(childNumber);
	delete wellListWidget->takeItem(deleteIndex);
}//remove the selected well

void Main_crava::on_openWellPushButton_clicked(){
	if (wellListWidget->currentRow() >=0){
		QTreeWidgetItem* well;
		findCorrectWell(&well);
		QString filename;
		getValueFromWell(well, QString("file-name"), filename);
		filename = QDir(standard->inputPath()).absoluteFilePath(filename);
		if(!filename.isEmpty()){
			QSettings settings("Statoil","CRAVA");
			settings.beginGroup("crava");

			QStringList arguments;
			arguments << filename;

			QString program;
			program = settings.value(QString("editor"),QString("emacs")).toString();

			QProcess *openWell = new QProcess();
			openWell->start(program, arguments); //open the well in emacs
		}
	}
}//opens the selected well in a text editor

void Main_crava::wellFile(const QString & value){
	QTreeWidgetItem* well;
	findCorrectWell(&well);
	if (standard->StandardStrings::fileExists(value)){
		setValueInWell( well, QString("file-name"), standard->StandardStrings::relativeFileName(value) );
		wellNameLabel->setText(StandardStrings::strippedName(value));
		wellListWidget->currentItem()->setText(StandardStrings::strippedName(value));
		wellFileLineEdit->setText( standard->StandardStrings::relativeFileName(value) );
	}
}
void Main_crava::on_wellFileLineEdit_editingFinished(){
	wellFile(wellFileLineEdit->text());
}//update the XML three with the file if it is correct, ell file, rms format, would be nice to have a converter from las

void Main_crava::on_wellBrowsePushButton_clicked(){
	QString fileName = QFileDialog::getOpenFileName(this, QString("Open File"), standard->StandardStrings::inputPath(), StandardStrings::wellFormat());//well format maybe?
	if(!fileName.isNull()){
		wellFile(fileName);
	}
}//browse for the well file then update the XML file, update the field

void Main_crava::on_waveletEstimationCheckBox_toggled(bool checked){
	QTreeWidgetItem* well;
	findCorrectWell(&well);
	QString value;
	if(checked){
		value=QString("yes");
	}
	else {
		value=QString("no");
	}
	setValueInWell(well, QString("use-for-wavelet-estimation"), value);
}//update whether this well should be used for wavelet estimationin XML file

void Main_crava::on_backgroundTrendCheckBox_toggled(bool checked){
	QTreeWidgetItem* well;
	findCorrectWell(&well);
	QString value;
	if(checked){
		value=QString("yes");
	}
	else {
		value=QString("no");
	}
	setValueInWell(well, QString("use-for-background-trend"), value);
}//update whether this well should be used for background trend estimationin XML file

void Main_crava::on_faciesProbabilitiesCheckBox_toggled(bool checked){
	QTreeWidgetItem* well;
	findCorrectWell(&well);
	QString value;
	if(checked){
		value=QString("yes");
	}
	else {
		value=QString("no");
	}
	setValueInWell(well, QString("use-for-facies-probabilities"), value);
}//update whether this well should be used for facies estimationin XML file

void Main_crava::on_filterElasticCheckBox_toggled(bool checked){
	QTreeWidgetItem* well;
	findCorrectWell(&well);
	QString value;
	if(checked){
		value=QString("yes");
	}
	else {
		value=QString("no");
	}
	setValueInWell(well, QString("filter-elastic-logs"), value);
}//update whether this well should multi-parameter-filter the elastic logs after inversion in XML file

void Main_crava::on_convertLasToRmsPushButton_clicked(){
	//converts las to rms, needs to be manually fixed for discrete variables since they do not have names in las logs.
	if(wellListWidget->count()>0){
		QString fileName = QFileDialog::getOpenFileName(this, QString("Select position file"), standard->StandardStrings::inputPath(), StandardStrings::asciiFormat());
		if(!fileName.isNull()){
			QTreeWidgetItem* well;
			findCorrectWell(&well);
			QString filename;
			getValueFromWell(well, QString("file-name"), filename);
			QString lasWellfilePath( QDir(standard->StandardStrings::inputPath()).absoluteFilePath(filename) );//las file is the current file in the listwidget.
			if(QFileInfo(lasWellfilePath).suffix()!=QString("las")){//should only convert if the file is a las file
				QMessageBox::warning(this, QString("Wrong file format"), QString("Not a las file."), QMessageBox::Ok);
				return;
			}
			QString rmsWellfilePath(QFileInfo(lasWellfilePath).path()+QDir::separator() + QFileInfo(lasWellfilePath).baseName() + QString(".rms"));
			StandardStrings::convertLasToRms(this, lasWellfilePath, rmsWellfilePath, fileName);//converts the file, this might have been better off as a seperate process.
			wellFile(rmsWellfilePath);//change the in the tree/displayed to the rms version of the file.
		}
	}
}


//syntetic vs log buttons
void Main_crava::synteticVsLog(const QString &value){
	QTreeWidgetItem* well;
	findCorrectWell(&well);
	setValueInWell(well, QString("synthetic-vs-log"), value);
}

void Main_crava::on_synteticVsYesRadioButton_toggled(bool checked){
	if(checked){
		synteticVsLog(QString("yes"));
	}
}

void Main_crava::on_synteticVsNoRadioButton_toggled(bool checked){
	if(checked){
		synteticVsLog(QString("no"));
	}
}

void Main_crava::on_synteticVsDetectRadioButton_toggled(bool checked){
	if(checked){
		synteticVsLog(QString(""));
	}
}

void Main_crava::on_optimizePositionCheckBox_toggled(bool checked){
	optimizePositionFrame->setVisible(checked);
	optimizePositionFrame->setEnabled(checked);
}

void Main_crava::on_optimizePositionListWidget_currentRowChanged ( int currentRow ){
	if(currentRow==-1){//no items left
		anglePositionlineEdit->setText(QString());
		weightLineEdit->setText(QString());
		anglePositionlineEdit->setEnabled(false);
		weightLineEdit->setEnabled(false);
		deleteOptimizationPushButton->setEnabled(false);
		return;
	}

	//moves to the correct optimization
	QTreeWidgetItem* optimizePosition;
	findCorrectOptimizePosition(&optimizePosition);

	//move to angle
	QString angle;
	getValueFromOptimizePosition(optimizePosition, QString("angle"), angle);
	anglePositionlineEdit->setText(angle);


	//move to weight
	QString weight;
	getValueFromOptimizePosition(optimizePosition, QString("weight"), weight);
	weightLineEdit->setText(weight);

	anglePositionlineEdit->setEnabled(true);
	weightLineEdit->setEnabled(true);
	deleteOptimizationPushButton->setEnabled(true);
}//updates the tree with the correct children.

void Main_crava::on_optimizePositionPushButton_clicked(){
	optimizePositionListWidget->addItem(QString("optimization"));
	addOptimizePosition();
	optimizePositionListWidget->setCurrentItem(optimizePositionListWidget->item(optimizePositionListWidget->count()-1));
}//adds a position optimization

void Main_crava::on_deleteOptimizationPushButton_clicked(){
	int deleteIndex=optimizePositionListWidget->currentRow();

	QTreeWidgetItem* optimizePosition;
	findCorrectOptimizePosition(&optimizePosition);

	int childNumber1 = optimizePosition->parent()->indexOfChild(optimizePosition); // finds the index of optimizePosition
	int childNumber0 = optimizePosition->parent()->parent()->indexOfChild( optimizePosition->parent() );// finds the index of the parent of optimizePosition

	if(deleteIndex==0&&optimizePositionListWidget->count()>1){
		optimizePositionListWidget->setCurrentItem(optimizePositionListWidget->item(1));
	}
	delete well_dataPointer->child(childNumber0)->child(childNumber1);
	delete optimizePositionListWidget->takeItem(deleteIndex);
}//removes the selected position optimization, should be undoable

void Main_crava::on_anglePositionlineEdit_editingFinished(){
	QTreeWidgetItem* optimizePosition;
	findCorrectOptimizePosition(&optimizePosition);

	QString value = anglePositionlineEdit->text();
	setValueInWell(optimizePosition, QString("angle"), value);
}//updates the angle for the optimized position in XML

void Main_crava::on_weightLineEdit_editingFinished(){
	QTreeWidgetItem* optimizePosition;
	findCorrectOptimizePosition(&optimizePosition);

	QString value = weightLineEdit->text();
	setValueInWell(optimizePosition, QString("weight"), value);
}//updates the weight of the optimized position in XML

//well parameter info
void Main_crava::on_seismicResolutionLineEdit_editingFinished(){
	high_cut_seismic_resolutionPointer->setText( 1, seismicResolutionLineEdit->text() );
}//update the XML three with the high cut seismic resolution

//horizon
void Main_crava::on_twoSurfaceRadioButton_toggled(bool checked){
	//needs to remove the values off the other case.
	if(!checked){
		//if it goes to base, top disapears, if it goes to top, base disapears, goes to constant, everything disapears, goes to one surface everything disapears
	}
	else {
		bottomTimeSurfaceLabel->setVisible(true);
		bottomTimeFileLineEdit->setVisible(true);
		bottomTimeFileBrowsePushButton->setVisible(true);
		bottomDepthFileLabel->setVisible(true);
		bottomDepthFileLineEdit->setVisible(true);
		bottomDepthFileBrowsePushButton->setVisible(true);
		bottomTimeSurfaceLabel->setEnabled(true);
		bottomTimeFileLineEdit->setEnabled(true);
		bottomTimeFileBrowsePushButton->setEnabled(true);
		bottomDepthFileLabel->setEnabled(true);
		bottomDepthFileLineEdit->setEnabled(true);
		bottomDepthFileBrowsePushButton->setEnabled(true);
		topTimeSurfaceLabel->setVisible(true);
		topTimeFileLineEdit->setVisible(true);
		topTimeFileBrowsePushButton->setVisible(true);
		topDepthFileLabel->setVisible(true);
		topDepthFileLineEdit->setVisible(true);
		topDepthFileBrowsePushButton->setVisible(true);
		topTimeSurfaceLabel->setEnabled(true);
		topTimeFileLineEdit->setEnabled(true);
		topTimeFileBrowsePushButton->setEnabled(true);
		topDepthFileLabel->setEnabled(true);
		topDepthFileLineEdit->setEnabled(true);
		topDepthFileBrowsePushButton->setEnabled(true);
		//velocityFieldNoneRadioButton->setChecked(true);
		on_depthSurfacesCheckBox_toggled(depthSurfacesCheckBox->isChecked());//might break loading, handled
	}
}
void Main_crava::on_topSurfaceRadioButton_toggled(bool checked){
	//should remove base if checked
	if(checked){
		bottomTimeSurfaceLabel->setVisible(false);
		bottomTimeFileLineEdit->setVisible(false);
		bottomTimeFileBrowsePushButton->setVisible(false);
		bottomDepthFileLabel->setVisible(false);
		bottomDepthFileLineEdit->setVisible(false);
		bottomDepthFileBrowsePushButton->setVisible(false);
		bottomTimeSurfaceLabel->setEnabled(false);
		bottomTimeFileLineEdit->setEnabled(false);
		bottomTimeFileBrowsePushButton->setEnabled(false);
		bottomDepthFileLabel->setEnabled(false);
		bottomDepthFileLineEdit->setEnabled(false);
		bottomDepthFileBrowsePushButton->setEnabled(false);
		topTimeSurfaceLabel->setVisible(true);
		topTimeFileLineEdit->setVisible(true);
		topTimeFileBrowsePushButton->setVisible(true);
		topDepthFileLabel->setVisible(true);
		topDepthFileLineEdit->setVisible(true);
		topDepthFileBrowsePushButton->setVisible(true);
		topTimeSurfaceLabel->setEnabled(true);
		topTimeFileLineEdit->setEnabled(true);
		topTimeFileBrowsePushButton->setEnabled(true);
		topDepthFileLabel->setEnabled(true);
		topDepthFileLineEdit->setEnabled(true);
		topDepthFileBrowsePushButton->setEnabled(true);
		//velocityFieldNoneRadioButton->setChecked(true);
		for (int i=0;i<interval_two_surfaces_base_surfacePointer->childCount();i++){//clear the tree
			interval_two_surfaces_base_surfacePointer->child(i)->setText(1,QString(""));
		}
		bottomTimeFileLineEdit->clear();
		bottomDepthFileLineEdit->clear();
		on_depthSurfacesCheckBox_toggled(depthSurfacesCheckBox->isChecked());//might break loading, handled.
	}
}
void Main_crava::on_baseSurfaceRadioButton_toggled(bool checked){
	if(checked){
		for (int i=0;i<interval_two_surfaces_top_surfacePointer->childCount();i++){
			interval_two_surfaces_top_surfacePointer->child(i)->setText(1,QString(""));
		}
		topTimeFileLineEdit->clear();
		topDepthFileLineEdit->clear();
		bottomTimeSurfaceLabel->setVisible(true);
		bottomTimeFileLineEdit->setVisible(true);
		bottomTimeFileBrowsePushButton->setVisible(true);
		bottomDepthFileLabel->setVisible(true);
		bottomDepthFileLineEdit->setVisible(true);
		bottomDepthFileBrowsePushButton->setVisible(true);
		bottomTimeSurfaceLabel->setEnabled(true);
		bottomTimeFileLineEdit->setEnabled(true);
		bottomTimeFileBrowsePushButton->setEnabled(true);
		bottomDepthFileLabel->setEnabled(true);
		bottomDepthFileLineEdit->setEnabled(true);
		bottomDepthFileBrowsePushButton->setEnabled(true);
		topTimeSurfaceLabel->setVisible(false);
		topTimeFileLineEdit->setVisible(false);
		topTimeFileBrowsePushButton->setVisible(false);
		topDepthFileLabel->setVisible(false);
		topDepthFileLineEdit->setVisible(false);
		topDepthFileBrowsePushButton->setVisible(false);
		topTimeSurfaceLabel->setEnabled(false);
		topTimeFileLineEdit->setEnabled(false);
		topTimeFileBrowsePushButton->setEnabled(false);
		topDepthFileLabel->setEnabled(false);
		topDepthFileLineEdit->setEnabled(false);
		topDepthFileBrowsePushButton->setEnabled(false);
		//velocityFieldNoneRadioButton->setChecked(true);
		on_depthSurfacesCheckBox_toggled(depthSurfacesCheckBox->isChecked());//might break loading handled
	}
}

void Main_crava::on_correlationSurfaceRadioButton_toggled(bool checked){
	//needs to be called with false on startup...
	correlationDirectionLabel->setVisible(checked);
	correlationDirectionFileLineEdit->setVisible(checked);
	correlationDirectionBrowsePushButton->setVisible(checked);
	correlationDirectionLabel->setEnabled(checked);
	correlationDirectionFileLineEdit->setEnabled(checked);
	correlationDirectionBrowsePushButton->setEnabled(checked);
	if(!checked){
		correlationDirectionFileLineEdit->setText(QString(""));
		correlationDirectionFile(QString(""));
	}
	else {
		on_twoSurfaceRadioButton_toggled(true);
	}
}

void Main_crava::on_oneSurfaceRadioButton_toggled(bool checked){
	surfaceOneFrame->setVisible(checked);
	surfaceOneFrame->setEnabled(checked);
	surfaceTwoFrame->setVisible(!checked);
	surfaceTwoFrame->setEnabled(!checked);
	if(!checked){//clears one surface;
		for (int i=0;i<interval_one_surfacePointer->childCount();i++){
				interval_one_surfacePointer->child(i)->setText(1,QString(""));
		}
		QList<QLineEdit*> fields=surfaceOneFrame->findChildren<QLineEdit*>();//this causes some sort of warning... bad cast of voidsurfaceTwoFrame pointer with qt 4.2?
		foreach (QLineEdit* field, fields){
			field->clear();
		}
	}
	else {//clears two surface
		for (int i=0;i<interval_two_surfacesPointer->childCount();i++){
			if(interval_two_surfacesPointer->child(i)->childCount()>0){
				for (int j=0;j<interval_two_surfacesPointer->child(i)->childCount();j++){
					interval_two_surfacesPointer->child(i)->child(j)->setText(1,QString(""));
				}
			}
			else {
				interval_two_surfacesPointer->child(i)->setText(1,QString(""));
			}
		}
		QList<QLineEdit*> fields=surfaceTwoFrame->QObject::findChildren<QLineEdit*>();//this causes some sort of warning... bad cast of void pointer with qt 4.2?
		foreach (QLineEdit* field, fields){
			field->clear();
		}
	}
}
void Main_crava::on_constantInversionRadioButton_toggled(bool checked){
	//needs to be called with false on startup
	topTimeConstantLabel->setVisible(checked);
	topTimeValueLineEdit->setVisible(checked);
	bottomTimeConstantLabel->setVisible(checked);
	bottomTimeValueLineEdit->setVisible(checked);
	topTimeConstantLabel->setEnabled(checked);
	topTimeValueLineEdit->setEnabled(checked);
	bottomTimeConstantLabel->setEnabled(checked);
	bottomTimeValueLineEdit->setEnabled(checked);
	depthSurfacesCheckBox->setVisible(!checked);
	//depthSurfaceFrame->setVisible(!checked);
	//depthSurfaceFrame->setEnabled(!checked);
	depthSurfacesCheckBox->setEnabled(!checked);
	if(checked) {
		//needs to clear the files and the fileedits
		QString value=QString("");
		topTimeFile(value);
		bottomTimeFile(value);
		depthSurfacesCheckBox->setChecked(false);
		on_depthSurfacesCheckBox_toggled(false);
		topTimeFileLineEdit->setText(QString(""));
		topDepthFileLineEdit->setText(QString(""));
		bottomTimeFileLineEdit->setText(QString(""));
		bottomDepthFileLineEdit->setText(QString(""));
		bottomTimeSurfaceLabel->setVisible(false);
		bottomTimeFileLineEdit->setVisible(false);
		bottomTimeFileBrowsePushButton->setVisible(false);
		bottomTimeSurfaceLabel->setEnabled(false);
		bottomTimeFileLineEdit->setEnabled(false);
		bottomTimeFileBrowsePushButton->setEnabled(false);
		topTimeSurfaceLabel->setVisible(false);
		topTimeFileLineEdit->setVisible(false);
		topTimeFileBrowsePushButton->setVisible(false);
		topTimeSurfaceLabel->setEnabled(false);
		topTimeFileLineEdit->setEnabled(false);
		topTimeFileBrowsePushButton->setEnabled(false);
		//velocityFieldNoneRadioButton->setChecked(true);
	}
	else{//clear the constants
		top_surface_time_valuePointer->setText(1,QString(""));
		base_surface_time_valuePointer->setText(1,QString(""));
		topTimeValueLineEdit->setText(QString(""));
		bottomTimeValueLineEdit->setText(QString(""));
	}
}

void Main_crava::on_depthSurfacesCheckBox_toggled(bool checked){
	depthSurfaceFrame->setVisible(checked);
	depthSurfaceFrame->setEnabled(checked);
	if(!checked){
		topDepthFile(QString());
		bottomDepthFile(QString());
	}
}
//two surface case
void Main_crava::topTimeFile(const QString & value){
	// needs to remove the value
	if (standard->StandardStrings::fileExists(value)){
		top_surface_time_filePointer->setText( 1, standard->StandardStrings::relativeFileName(value) );
	}
}
void Main_crava::on_topTimeFileLineEdit_editingFinished(){
	topTimeFile(topTimeFileLineEdit->text());
}//update the XML three with the file if it is correct, autocomplete would be nice, top time file

void Main_crava::on_topTimeFileBrowsePushButton_clicked(){
	QString fileName = QFileDialog::getOpenFileName(this, QString("Open File"), standard->StandardStrings::inputPath(), StandardStrings::surfaceFormat());
	if(!fileName.isNull()){
		topTimeFileLineEdit->setText(fileName);
		topTimeFile(fileName);
	}
}//browse for the top time file then update the XML file if the above is not triggered, update the field

	//button time value top
void Main_crava::on_topTimeValueLineEdit_editingFinished(){
	top_surface_time_valuePointer->setText( 1, topTimeValueLineEdit->text() );
};//update the XML file with the new top time value

void Main_crava::topDepthFile(const QString & value){
	if (standard->StandardStrings::fileExists(value)){
		top_surface_depth_filePointer->setText( 1, standard->StandardStrings::relativeFileName(value) );
	}
};
void Main_crava::on_topDepthFileLineEdit_editingFinished(){
	topDepthFile(topDepthFileLineEdit->text());
};//update the XML three with the file if it is correct, autocomplete would be nice, top depth file

void Main_crava::on_topDepthFileBrowsePushButton_clicked(){
	QString fileName = QFileDialog::getOpenFileName(this, QString("Open File"), standard->StandardStrings::inputPath(), StandardStrings::surfaceFormat());
	if(!fileName.isNull()){
		topDepthFileLineEdit->setText(fileName);
		topDepthFile(fileName);
	}
};//browse for the top time file then update the XML file if the above is not triggered, update the field

void Main_crava::bottomTimeFile(const QString & value){
	if (standard->StandardStrings::fileExists(value)){
		base_surface_time_filePointer->setText( 1, standard->StandardStrings::relativeFileName(value) );
	}
};

void Main_crava::on_bottomTimeFileLineEdit_editingFinished(){
	bottomTimeFile(bottomTimeFileLineEdit->text());
};//update the XML three with the file if it is correct, autocomplete would be nice, bottom time file

void Main_crava::on_bottomTimeFileBrowsePushButton_clicked(){
	QString fileName = QFileDialog::getOpenFileName(this, QString("Open File"), standard->StandardStrings::inputPath(), StandardStrings::surfaceFormat());
	if(!fileName.isNull()){
		bottomTimeFileLineEdit->setText(fileName);
		bottomTimeFile(fileName);
	}
}//browse for the top time file then update the XML file if the above is not triggered, update the field

	//button time value bottom
void Main_crava::on_bottomTimeValueLineEdit_editingFinished(){
	base_surface_time_valuePointer->setText( 1, bottomTimeValueLineEdit->text() );
};//update the XML file with the new bottom time value

void Main_crava::bottomDepthFile(const QString & value){
	if (standard->StandardStrings::fileExists(value)){
		base_surface_depth_filePointer->setText( 1, standard->StandardStrings::relativeFileName(value) );
	}
};

void Main_crava::on_bottomDepthFileLineEdit_editingFinished(){
	bottomDepthFile(bottomDepthFileLineEdit->text());
};//update the XML three with the file if it is correct, autocomplete would be nice, top depth file

void Main_crava::on_bottomDepthFileBrowsePushButton_clicked(){
	QString fileName = QFileDialog::getOpenFileName(this, QString("Open File"), standard->StandardStrings::inputPath(), StandardStrings::surfaceFormat());
	if(!fileName.isNull()){
		bottomDepthFileLineEdit->setText(fileName);
		bottomDepthFile(fileName);
	}
};//browse for the bottom time file then update the XML file if the above is not triggered, update the field

void Main_crava::on_layersLineEdit_editingFinished(){
	interval_two_surfaces_number_of_layersPointer->setText( 1, layersLineEdit->text() );
}//update the XML for the number of layers

	//buttons for velocity field
void Main_crava::on_velocityFieldInvesionRadioButton_toggled(bool checked){
	QString value;
	if(checked){
		value=QString("yes");
	}
	else {
		value=QString("");
	}
	interval_two_surfaces_velocity_field_from_inversionPointer->setText(1, value);
}
void Main_crava::on_velocityFieldFileRadioButton_toggled(bool checked){
	if(!checked){//needs to be cleared if one of the other options are chosen.
		QString value=QString("");
		interval_two_surfaces_velocity_fieldPointer->setText(1, value);
		velocityFieldLineEdit->setText(QString(""));
	}
	else{
		velocityFieldLineEdit->setFocus();
	}
	velocityFieldLineEdit->setVisible(checked);
	velocityFieldBrowsePushButton->setVisible(checked);
	velocityFieldLineEdit->setEnabled(checked);
	velocityFieldBrowsePushButton->setEnabled(checked);
}
void Main_crava::velocityFieldFile(const QString & value){
	// needs to turn off the velocity field from inversion

	if (standard->StandardStrings::fileExists(value)){
		interval_two_surfaces_velocity_fieldPointer->setText( 1, standard->StandardStrings::relativeFileName(value) );
	}
}
void Main_crava::on_velocityFieldLineEdit_editingFinished(){
	velocityFieldFile(velocityFieldLineEdit->text());
};//update the XML three with the file if it is correct, autocomplete would be nice, velocity field file

void Main_crava::on_velocityFieldBrowsePushButton_clicked(){
	QString fileName = QFileDialog::getOpenFileName(this, QString("Open File"), standard->StandardStrings::inputPath(), StandardStrings::surfaceFormat());
	if(!fileName.isNull()){
		velocityFieldLineEdit->setText(fileName);
		velocityFieldFile(fileName);
	}
}//browse for the velocity field file then update the XML file if the above is not triggered, update the field

//one surface case
void Main_crava::referenceSurfaceFile(const QString & value){
	if (standard->StandardStrings::fileExists(value)){
		interval_one_surface_reference_surfacePointer->setText( 1, standard->StandardStrings::relativeFileName(value) );
	}
}

void Main_crava::on_referenceSurfaceFileLineEdit_editingFinished(){
	referenceSurfaceFile(referenceSurfaceFileLineEdit->text());
};//update the XML three with the file if it is correct, autocomplete would be nice, reference surface file

void Main_crava::on_referenceSurfaceBrowsePushButton_clicked(){
	QString fileName = QFileDialog::getOpenFileName(this, QString("Open File"), standard->StandardStrings::inputPath(), StandardStrings::seismicFormat());
	if(!fileName.isNull()){
		referenceSurfaceFileLineEdit->setText(fileName);
		referenceSurfaceFile(fileName);
	}
};//browse for the reference surface file then update the XML file if the above is not triggered, update the field

void Main_crava::on_distanceTopLineEdit_editingFinished(){
	interval_one_surface_shift_to_interval_topPointer->setText( 1, distanceTopLineEdit->text() );
};//update the XML three with the distance to the top layer from the referance surface

void Main_crava::on_thicknessLineEdit_editingFinished(){
	interval_one_surface_thicknessPointer->setText( 1, thicknessLineEdit->text() );
};//update the XML three with the thickness for the volume of the entire inversion

void Main_crava::on_layerThicknessLineEdit_editingFinished(){
	interval_one_surface_sample_densityPointer->setText( 1, layerThicknessLineEdit->text() );
};//update the XML three with the thickness for each layer

//other horizons
void Main_crava::waveletTopFile(const QString & value){
	if (standard->StandardStrings::fileExists(value)){
		survey_top_surface_filePointer->setText( 1, standard->StandardStrings::relativeFileName(value) );
	}
}

void Main_crava::on_waveletTopLineEdit_editingFinished(){
	waveletTopFile(waveletTopLineEdit->text());
};//update the XML three with the file if it is correct, autocomplete would be nice, wavelet top file

void Main_crava::on_waveletTopBrowsePushButton_clicked(){
	QString fileName = QFileDialog::getOpenFileName(this, QString("Open File"), standard->StandardStrings::inputPath(), StandardStrings::surfaceFormat());
	if(!fileName.isNull()){
		waveletTopLineEdit->setText(fileName);
		waveletTopFile(fileName);
	}
};//browse for the wavelet top time file then update the XML file if the above is not triggered, update the field

void Main_crava::waveletBottomFile(const QString & value){
	if (standard->StandardStrings::fileExists(value)){
		survey_base_surface_filePointer->setText( 1, standard->StandardStrings::relativeFileName(value) );
	}
}

void Main_crava::on_waveletBottomLineEdit_editingFinished(){
	waveletBottomFile(waveletBottomLineEdit->text());
};//update the XML three with the file if it is correct, autocomplete would be nice, wavelet bottom time file

void Main_crava::on_waveletBottomBrowsePushButton_clicked(){
	QString fileName = QFileDialog::getOpenFileName(this, QString("Open File"), standard->StandardStrings::inputPath(), StandardStrings::surfaceFormat());
	if(!fileName.isNull()){
		waveletBottomLineEdit->setText(fileName);
		waveletBottomFile(fileName);
	}
};//browse for the wavelet bottom time file then update the XML file if the above is not triggered, update the field

void Main_crava::faciesTopFile(const QString & value){
	if (standard->StandardStrings::fileExists(value)){
		facies_probabilities_top_surface_filePointer->setText( 1, standard->StandardStrings::relativeFileName(value) );
	}
}

void Main_crava::faciesBottomFile(const QString & value){
	if (standard->StandardStrings::fileExists(value)){
		facies_probabilities_base_surface_filePointer->setText( 1, standard->StandardStrings::relativeFileName(value) );
	}
}

void Main_crava::on_faciesTopLineEdit_editingFinished(){ 
	faciesTopFile(faciesTopLineEdit->text());
};//update the XML three with the file if it is correct, autocomplete would be nice, facies top file

void Main_crava::on_faciesTopBrowsePushButton_clicked(){
	QString fileName = QFileDialog::getOpenFileName(this, QString("Open File"), standard->StandardStrings::inputPath(), StandardStrings::surfaceFormat());
	if(!fileName.isNull()){
		faciesTopLineEdit->setText(fileName);
		faciesTopFile(fileName);
	}
};//browse for the facies top time file then update the XML file if the above is not triggered, update the field

void Main_crava::on_faciesBottomLineEdit_editingFinished(){
	faciesBottomFile(faciesBottomLineEdit->text());
};//update the XML three with the file if it is correct, autocomplete would be nice, facies bottom time file

void Main_crava::on_faciesBottomBrowsePushButton_clicked(){
	QString fileName = QFileDialog::getOpenFileName(this, QString("Open File"), standard->StandardStrings::inputPath(), StandardStrings::surfaceFormat());
	if(!fileName.isNull()){
		faciesBottomLineEdit->setText(fileName);
		faciesBottomFile(fileName);
	}
};//browse for the facies bottom time file then update the XML file if the above is not triggered, update the field

	//prior model
void Main_crava::on_backgroundModelCheckBox_toggled(bool checked){
	//needs to fix the values as well
	backgroundGivenFrame->setVisible(!checked);
	backgroundEstimateFrame->setVisible(checked);
	backgroundGivenFrame->setEnabled(!checked);
	backgroundEstimateFrame->setEnabled(checked);
	if(checked){//clears the parameters for given background
		vpFileRadioButton->setChecked(true);
		vsFileRadioButton->setChecked(true);
		densityFileRadioButton->setChecked(true);
		QList<QLineEdit*> fields=backgroundGivenFrame->QObject::findChildren<QLineEdit*>();//this causes some sort of warning... bad cast of void pointer with qt 4.2?
		foreach (QLineEdit* field, fields){
			field->clear();
		}
		background_vp_filePointer->setText(1,QString(""));
		background_vs_filePointer->setText(1,QString(""));
		background_density_filePointer->setText(1,QString(""));
		background_vp_constantPointer->setText(1,QString(""));
		background_vs_constantPointer->setText(1,QString(""));
		background_density_constantPointer->setText(1,QString(""));
	}
	else{//clears the parameters for estimated background
		velocityFieldPriorFileLineEdit->clear();
		highCutFrequencyLineEdit->clear();

		QString value("");
		background_velocity_fieldPointer->setText(1, value);
		background_high_cut_background_modellingPointer->setText(1, value);
		background_lateral_correlationPointer->setText(1, value); //isn't this always empty??
	}
};//determining wether the background model should be estimated or given

	//buttons for vp/vs/density
void Main_crava::on_vpFileRadioButton_toggled(bool checked){//either constant or from file fixes displayed widgets
	vpFileLineEdit->setVisible(checked);
	vpBrowsePushButton->setVisible(checked);
	vpConstantLineEdit->setVisible(!checked);
	vpFileLineEdit->setEnabled(checked);
	vpBrowsePushButton->setEnabled(checked);
	vpConstantLineEdit->setEnabled(!checked);
	if(checked){
		vpFileLineEdit->setFocus();
	}
	else{
		vpConstantLineEdit->setFocus();
	}
}
void Main_crava::on_vsFileRadioButton_toggled(bool checked){//either constant or from file fixes displayed widgets
	vsFileLineEdit->setVisible(checked);
	vsBrowsePushButton->setVisible(checked);
	vsConstantLineEdit->setVisible(!checked);
	vsFileLineEdit->setEnabled(checked);
	vsBrowsePushButton->setEnabled(checked);
	vsConstantLineEdit->setEnabled(!checked);
	if(checked){
		vsFileLineEdit->setFocus();
	}
	else{
		vsConstantLineEdit->setFocus();
	}
}

void Main_crava::on_densityFileRadioButton_toggled(bool checked){//either constant or from file fixes displayed widgets
	densityFileLineEdit->setVisible(checked);
	densityBrowsePushButton->setVisible(checked);
	densityConstantLineEdit->setVisible(!checked);
	densityFileLineEdit->setEnabled(checked);
	densityBrowsePushButton->setEnabled(checked);
	densityConstantLineEdit->setEnabled(!checked);
	if(checked){
		densityFileLineEdit->setFocus();
	}
	else{
		densityConstantLineEdit->setFocus();
	}
}

void Main_crava::on_vpConstantLineEdit_editingFinished(){
	// should remove the file from the tree
	background_vp_constantPointer->setText( 1, vpConstantLineEdit->text() );
	background_vp_filePointer->setText( 1, QString() );
};//update the XML three with constant vp for the background model

void Main_crava::vpFile(const QString &value){
	// should remove the constant from the tree
	if (standard->StandardStrings::fileExists(value)){
		background_vp_filePointer->setText( 1, standard->StandardStrings::relativeFileName(value) );
		background_vp_constantPointer->setText( 1, QString() );
	}
}

void Main_crava::on_vpFileLineEdit_editingFinished(){
	vpFile(vpFileLineEdit->text());
};//update the XML three with the file if it is correct, autocomplete would be nice, prior model vp file
void Main_crava::on_vpBrowsePushButton_clicked(){
	QString fileName = QFileDialog::getOpenFileName(this, QString("Open File"), standard->StandardStrings::inputPath(), StandardStrings::seismicFormat());
	if(!fileName.isNull()){
		vpFileLineEdit->setText(fileName);
		vpFile(fileName);
	}
};//browse for the prior model vp file then update the XML file if the above is not triggered, update the field

void Main_crava::on_vsConstantLineEdit_editingFinished(){
	background_vs_constantPointer->setText( 1, vsConstantLineEdit->text() );
	background_vs_filePointer->setText( 1, QString() );
};//update the XML three with constant vs for the background model

void Main_crava::vsFile(const QString &value){
	//should remove the constant from the tree
	if (standard->StandardStrings::fileExists(value)){
		background_vs_filePointer->setText( 1, standard->StandardStrings::relativeFileName(value) );
		background_vs_constantPointer->setText( 1, QString() );
	}
}

void Main_crava::on_vsFileLineEdit_editingFinished(){
	vsFile(vsFileLineEdit->text());
};//update the XML three with the file if it is correct, autocomplete would be nice, prior model vs file

void Main_crava::on_vsBrowsePushButton_clicked(){
	QString fileName = QFileDialog::getOpenFileName(this, QString("Open File"), standard->StandardStrings::inputPath(), StandardStrings::seismicFormat());
	if(!fileName.isNull()){
		vsFileLineEdit->setText(fileName);
		vsFile(fileName);
	}
};//browse for the prior model vs file then update the XML file if the above is not triggered, update the field

void Main_crava::on_densityConstantLineEdit_editingFinished(){
	// should remove the file from the tree
	background_density_constantPointer->setText( 1, densityConstantLineEdit->text() );
	background_density_filePointer->setText( 1, QString() );
};//update the XML three with constant density for the background model

void Main_crava::densityFile(const QString &value){
	// should remove the constant from the tree
	if (standard->StandardStrings::fileExists(value)){
		background_density_filePointer->setText( 1, standard->StandardStrings::relativeFileName(value) );
		background_density_constantPointer->setText( 1, QString() );
	}
}

void Main_crava::on_densityFileLineEdit_editingFinished(){
	densityFile(densityFileLineEdit->text());	
};//update the XML tree with the file if it is correct, prior model density file

void Main_crava::on_densityBrowsePushButton_clicked(){
	QString fileName = QFileDialog::getOpenFileName(this, QString("Open File"), standard->StandardStrings::inputPath(), StandardStrings::seismicFormat());
	if(!fileName.isNull()){
		densityFileLineEdit->setText(fileName);
		densityFile(fileName);
	}
};//browse for the prior model density file then update the XML file if the above is not triggered, update the field
void Main_crava::on_backgroundEstimatedConfigurationCheckBox_toggled(bool checked){//does not modify the tree in any way, just changes what widgets are displayed.
	velocityFieldLabel->setVisible(checked);
	velocityFieldPriorFileLineEdit->setVisible(checked);
	velocityFieldPriorFileBrowsePushButton->setVisible(checked);
	lateralCorrelationLabel->setVisible(checked);
	lateralCorrelationBackgroundPushButton->setVisible(checked);
	highCutFrequencyLabel->setVisible(checked);
	highCutFrequencyLineEdit->setVisible(checked);
	hzLabel->setVisible(checked);
	velocityFieldLabel->setEnabled(checked);
	velocityFieldPriorFileLineEdit->setEnabled(checked);
	velocityFieldPriorFileBrowsePushButton->setEnabled(checked);
	lateralCorrelationLabel->setEnabled(checked);
	lateralCorrelationBackgroundPushButton->setEnabled(checked);
	highCutFrequencyLabel->setEnabled(checked);
	highCutFrequencyLineEdit->setEnabled(checked);
	hzLabel->setEnabled(checked);
}

void Main_crava::velocityFieldPriorFile(const QString &value){
	// should remove the constant from the tree
	if (standard->StandardStrings::fileExists(value)){
		background_velocity_fieldPointer->setText( 1, standard->StandardStrings::relativeFileName(value) );
	}
}

void Main_crava::on_velocityFieldPriorFileLineEdit_editingFinished(){
	velocityFieldPriorFile(velocityFieldPriorFileLineEdit->text());
};//update the XML three with the file if it is correct, autocomplete would be nice, prior model velocity field

void Main_crava::on_velocityFieldPriorFileBrowsePushButton_clicked(){
	QString fileName = QFileDialog::getOpenFileName(this, QString("Open File"), standard->StandardStrings::inputPath(), StandardStrings::seismicFormat());
	if(!fileName.isNull()){
		velocityFieldPriorFileLineEdit->setText(fileName);
		velocityFieldPriorFile(fileName);
	}
};//browse for the prior model velocity field file then update the XML tree and the above field

void Main_crava::on_lateralCorrelationBackgroundPushButton_clicked(){
	variogram(background_lateral_correlationPointer);
};//pop up the variogram edit window for background

void Main_crava::on_highCutFrequencyLineEdit_editingFinished(){
	background_high_cut_background_modellingPointer->setText( 1, highCutFrequencyLineEdit->text() );
};//update the XML three with the high cut frequency

void Main_crava::on_correlationLocalWaveletCheckBox_toggled(bool checked){
	lateralCorrelationWaveletPushButton->setVisible(checked);
	lateralCorrelationWaveletPushButton->setEnabled(checked);
	if(!checked){
		for(int i=0;i<local_wavelet_lateral_correlationPointer->childCount();++i){
			local_wavelet_lateral_correlationPointer->child(i)->setText(1,QString());//clear
		}
	}
}

void Main_crava::on_lateralCorrelationWaveletPushButton_clicked(){
	variogram(local_wavelet_lateral_correlationPointer);
};//pop up the variogram edit window for wavelet

void Main_crava::on_correlationElasticParametersCheckBox_toggled(bool checked){
	parameterCorrelationFrame->setVisible(checked);
	parameterCorrelationFrame->setEnabled(checked);
	temporalCorrelationLineEdit->clear();
	parameterCorrelationLineEdit->clear();
	temporalCorrelationFile(QString());
	parameterCorrelationFile(QString());

	if(!checked){
		for(int i=0;i<background_lateral_correlationPointer->childCount();++i){
			background_lateral_correlationPointer->child(i)->setText(1,QString());//clear
		}
	}
}

void Main_crava::on_lateralCorrelationParametersPushButton_clicked(){
	variogram( prior_model_lateral_correlationPointer );
};//pop up the variogram edit window for parameters

void Main_crava::temporalCorrelationFile(const QString &value){
	if (standard->StandardStrings::fileExists(value)){
		temporal_correlationPointer->setText( 1, standard->StandardStrings::relativeFileName(value) );
	}
}
void Main_crava::on_temporalCorrelationLineEdit_editingFinished(){
	temporalCorrelationFile(temporalCorrelationLineEdit->text());
};//update the XML three with the file if it is correct, autocomplete would be nice, prior model temporal corr

void Main_crava::on_temporalCorrelationBrowsePushButton_clicked(){
	QString fileName = QFileDialog::getOpenFileName(this, QString("Open File"), standard->StandardStrings::inputPath(), StandardStrings::correlationFormat());
	if(!fileName.isNull()){
		temporalCorrelationLineEdit->setText(fileName);
		temporalCorrelationFile(fileName);
	}
};//browse for prior model temporal correlation file then update the XML file and the above field

void Main_crava::parameterCorrelationFile(const QString &value){
	if (standard->StandardStrings::fileExists(value)){
		parameter_correlationPointer->setText( 1, standard->StandardStrings::relativeFileName(value) );
	}
}

void Main_crava::on_parameterCorrelationLineEdit_editingFinished(){
	parameterCorrelationFile(parameterCorrelationLineEdit->text());
};//update the XML three with the file if it is correct, autocomplete would be nice, prior model parameter corr

void Main_crava::on_parameterCorrelationBrowsePushButton_clicked(){
	QString fileName = QFileDialog::getOpenFileName(this, QString("Open File"), standard->StandardStrings::inputPath(), StandardStrings::correlationFormat());
	if(!fileName.isNull()){
		parameterCorrelationLineEdit->setText(fileName);
		parameterCorrelationFile(fileName);
	}
};//browse for prior model parameter corr file then update the XML file and the above field

void Main_crava::correlationDirectionFile(const QString &value){
	if (standard->StandardStrings::fileExists(value)){
		correlation_directionPointer->setText( 1, standard->StandardStrings::relativeFileName(value) );
	}
}

void Main_crava::on_correlationDirectionFileLineEdit_editingFinished(){
	correlationDirectionFile(correlationDirectionFileLineEdit->text());
};//update the XML three with the file if it is correct, autocomplete would be nice, prior model corr direction

void Main_crava::on_correlationDirectionBrowsePushButton_clicked(){
	QString fileName = QFileDialog::getOpenFileName(this, QString("Open File"), standard->StandardStrings::inputPath(), StandardStrings::correlationFormat());
	if(!fileName.isNull()){
		correlationDirectionFileLineEdit->setText(fileName);
		correlationDirectionFile(fileName);
	}
};//browse for prior model corr direction file then update the XML file and the above field

void Main_crava::on_faciesEstimateCheckBox_toggled(bool checked){
	//needs to remove the info, especially the checkboxinfo
	faciesGivenFrame->setVisible(!checked);
	faciesGivenFrame->setEnabled(!checked);
	if(checked){
		//clear the list
		faciesListWidget->clear();
		// insert an empty prior-probabilities node
		prior_probabilitesPointer->setText( 1, QString() );
	}
	else{
		// clears all estimate-facis options, no estimate-only options currently
	}
};//should the prior model for facies be estimated or given

void Main_crava::vsForFacies(const QString &value){
	facies_probabilities_use_vsPointer->setText(1, value);
}

void Main_crava::predictionFacies(const QString &value){
	facies_probabilities_use_predictionPointer->setText(1, value);
}

void Main_crava::absoluteParameters(const QString &value){
	facies_probabilities_use_absolute_elastic_parametersPointer->setText(1, value);
}

void Main_crava::on_vsForFaciesCheckBox_toggled(bool checked){
	if(checked){
		vsForFacies(QString("yes"));
	}
	else {
		vsForFacies(QString("no"));
	}
};//should vs be used for facies estimaton, update XML

void Main_crava::on_predictionFaciesCheckBox_toggled(bool checked){
	if(checked){
		predictionFacies(QString("yes"));
	}
	else {
		predictionFacies(QString("no"));
	}
};//should sampled inversion logs be used instead of filtered logs for facies estimaton, update XML

void Main_crava::on_absoluteParametersCheckBox_toggled(bool checked){
	if(checked){
		absoluteParameters(QString("yes"));
	}
	else {
		absoluteParameters(QString("no"));
	}
};//should absolute parameters be used instead of parameters minus trend for facies estimaton, update XML

void Main_crava::on_faciesListWidget_currentRowChanged ( int currentRow ){
	if(currentRow==-1){//disable if there are no items left.
		faciesNameLineEdit->setText(QString());
		probabilityConstantLineEdit->setText(QString());
		probabilityCubeLineEdit->setText(QString());
		faciesFrame->setEnabled(false);
		deleteFaciesPushButton->setEnabled(false);
		return;
	}
	QTreeWidgetItem* facies;
	findCorrectFacies(&facies);
	//the selected facies, current row is updated after 

	//move to name
	QString name;
	getValueFromOptimizePosition(facies, QString("name"), name);
	faciesNameLineEdit->setText(name);

	//move to probability
	QString probability;
	getValueFromOptimizePosition(facies, QString("probability"), probability);
	probabilityConstantLineEdit->setText(probability);

	//move to probability-cube
	QString probabilityCube;
	getValueFromOptimizePosition(facies, QString("probability-cube"), probabilityCube);
	probabilityCubeLineEdit->setText(probabilityCube);
	if(probabilityCube.isEmpty()){
		probabilityConstantRadioButton->setChecked(true);

	}
	else {
		probabiltyCubeRadioButton->setChecked(true);
	}

	faciesFrame->setEnabled(true);
	deleteFaciesPushButton->setEnabled(true);
}

void Main_crava::on_addFaciesPushButton_clicked(){//update the tree and the list.
	faciesListWidget->addItem(QString("facies"));
	addFacies();
	faciesListWidget->setCurrentItem(faciesListWidget->item(faciesListWidget->count()-1));
};//adds a new facies for prior probabilities

void Main_crava::on_deleteFaciesPushButton_clicked(){
	int deleteIndex=faciesListWidget->currentRow();
	QTreeWidgetItem* facies;
	findCorrectFacies(&facies);

	int childNumber = facies->parent()->indexOfChild(facies);
	if(deleteIndex==0&&faciesListWidget->count()>1){
		faciesListWidget->setCurrentItem(faciesListWidget->item(1));
	}
	delete prior_probabilitesPointer->child(childNumber);
	delete faciesListWidget->takeItem(deleteIndex);
};//removes the selected facis, should be undoable

void Main_crava::on_faciesNameLineEdit_editingFinished(){
	QTreeWidgetItem* facies;
	findCorrectFacies(&facies);

	QString value=faciesNameLineEdit->text();
	faciesListWidget->currentItem()->setText(value);
	setValueInFacies(facies, QString("name"), value);
};//changes the name of the facies, both displayed and in XML

	//probability radio buttons
void Main_crava::on_probabilityConstantRadioButton_toggled(bool checked){
	probabilityConstantLineEdit->setVisible(checked);
	probabilityCubeLineEdit->setVisible(!checked);
	probabilityCubeBrowsePushButton->setVisible(!checked);
	probabilityConstantLineEdit->setEnabled(checked);
	probabilityCubeLineEdit->setEnabled(!checked);
	probabilityCubeBrowsePushButton->setEnabled(!checked);
	if(checked){
		probabilityConstantLineEdit->setFocus();
	}
	else{
		probabilityCubeLineEdit->setFocus();
	}
}

void Main_crava::on_probabilityConstantLineEdit_editingFinished(){
	QTreeWidgetItem* facies;
	findCorrectFacies(&facies);

	QString value=probabilityConstantLineEdit->text();
	setValueInFacies(facies, QString("probability"), value);
	//can't give both constant probability and a file
	value=QString("");
	setValueInFacies(facies, QString("probability-cube"), value);
};//update the XML three with constant probability for the selected facies

void Main_crava::probabilityCubeFile(const QString &value){
	QTreeWidgetItem* facies;
	findCorrectFacies(&facies);
	if (standard->StandardStrings::fileExists(value)){
		setValueInFacies( facies, QString("probability-cube"), standard->StandardStrings::relativeFileName(value) );
		//can't give both constant probability and a file
			setValueInFacies( facies, QString("probability"), QString("") );
	}
}

void Main_crava::on_probabilityCubeLineEdit_editingFinished(){
	probabilityCubeFile(probabilityCubeLineEdit->text());
};//update the XML three with the file if it is correct, autocomplete would be nice, facis probability cube file

void Main_crava::on_probabilityCubeBrowsePushButton_clicked(){
	QString fileName = QFileDialog::getOpenFileName(this, QString("Open File"), standard->StandardStrings::inputPath(), StandardStrings::seismicFormat());
	if(!fileName.isNull()){
		probabilityCubeLineEdit->setText(fileName);
		probabilityCubeFile(fileName);
	}
};//browse for the facis probability cube file then update the XML file, update the field

void Main_crava::on_uncertaintyLevelLineEdit_editingFinished(){
	uncertainty_levelPointer->setText( 1, uncertaintyLevelLineEdit->text() );
};//update the XML file with the uncertainty level

void Main_crava::on_writeXmlPushButton_clicked(){
	bool pressedOK = false; //bool that becomes true if the users presses the Ok button
	QString *xmlFilename = new QString; //the name of the xml file
	QPointer<OutputDialog> output = new OutputDialog(this, xmlFilename, this, faciesProbabilitiesOn(),forwardMode(),estimationMode());
	if(output->exec()){
		output->updateOutput();//updates the tree
		output->updateXmlName(); //updates the xml filename
		QSettings settings("Statoil","CRAVA");
		settings.beginGroup("crava");
		settings.beginGroup("GUI");
		settings.setValue(QString("xmlName"), *xmlFilename);
		settings.endGroup();
		settings.endGroup();
		delete output;
		pressedOK = true;
	}
	delete output;
	//cout << pressedOK << endl;
	if (pressedOK == true){
		//saves the file and then runs
		if( saveFile( top_directoryPointer->text(1) + output_directoryPointer->text(1) + QString("/") + *xmlFilename + QString(".xml") ) ){ //the full path + the name of the xml file + .xml 
			on_runAction_triggered();
		}
	}
	delete xmlFilename;
}

void Main_crava::on_areaSeismicRadioButton_toggled(bool checked){
	//remove the values off the other cases.
	if(checked){
		on_areaFileRadioButton_toggled(false);
		on_areaUtmRadioButton_toggled(false);
		on_areaInCrossRadioButton_toggled(false);
	}
}

void Main_crava::on_areaFileRadioButton_toggled(bool checked){
	areaFileFrame->setVisible(checked);
	areaFileFrame->setEnabled(checked);
	if(!checked){
		areaSurfaceFile(QString(""));
		areaSurfaceLineEdit->clear();
	}
}

void Main_crava::on_areaUtmRadioButton_toggled(bool checked){
	areaUtmFrame->setVisible(checked);
	areaUtmFrame->setEnabled(checked);
	if(!checked){
		for (int i=0;i<utm_coordinatesPointer->childCount();i++){//remove the values if unchecked, both tree and displayed.
			utm_coordinatesPointer->child(i)->setText(1,QString(""));
		}
		QList<QLineEdit*> fields=areaUtmFrame->QObject::findChildren<QLineEdit*>();//this causes some sort of warning... bad cast of void pointer with qt 4.2?
		foreach (QLineEdit* field, fields){
			field->clear();
		}
	}
}

void Main_crava::on_areaInCrossRadioButton_toggled(bool checked){
	areaInCrossFrame->setVisible(checked);
	areaInCrossFrame->setEnabled(checked);
	if(!checked){//remove the values if unchecked, both tree and displayed.
		for (int i=0;i<inline_crossline_numbersPointer->childCount();i++){
			inline_crossline_numbersPointer->child(i)->setText(1,QString(""));
		}
		QList<QLineEdit*> fields=areaInCrossFrame->QObject::findChildren<QLineEdit*>();//this causes some sort of warning... bad cast of void pointer with qt 4.2?
		foreach (QLineEdit* field, fields){
			field->clear();
		}
	}
}

void Main_crava::areaSurfaceFile(const QString &value){
	if (standard->StandardStrings::fileExists(value)){
		area_from_surface_file_namePointer->setText( 1, standard->StandardStrings::relativeFileName(value) );
	}
}

void Main_crava::on_areaSurfaceLineEdit_editingFinished(){
	areaSurfaceFile(areaSurfaceLineEdit->text());
}

void Main_crava::on_areaSurfaceFileBrowsePushButton_clicked(){
	QString fileName = QFileDialog::getOpenFileName(this, QString("Open File"), standard->StandardStrings::inputPath(), StandardStrings::surfaceFormat());
	if(!fileName.isNull()){
		areaSurfaceLineEdit->setText(fileName);
		areaSurfaceFile(fileName);
	}
}

void Main_crava::on_areaXRefLineEdit_editingFinished(){
	utm_coordinates_reference_point_xPointer->setText( 1, areaXRefLineEdit->text() );
}

void Main_crava::on_areaYRefLineEdit_editingFinished(){
	utm_coordinates_reference_point_yPointer->setText( 1, areaYRefLineEdit->text() );
}

void Main_crava::on_areaXLengthLineEdit_editingFinished(){
	utm_coordinates_length_xPointer->setText( 1, areaXLengthLineEdit->text() );
}

void Main_crava::on_areaYLengthLineEdit_editingFinished(){
	utm_coordinates_length_yPointer->setText( 1, areaYLengthLineEdit->text() );
}

void Main_crava::on_areaXSampleDensityLineEdit_editingFinished(){
	utm_coordinates_sample_density_xPointer->setText( 1, areaXSampleDensityLineEdit->text() );
}

void Main_crava::on_areaYSampleDensityLineEdit_editingFinished(){
	utm_coordinates_sample_density_yPointer->setText( 1, areaYSampleDensityLineEdit->text() );
}

void Main_crava::on_areaUtmAngleLabelineEdit_editingFinished(){
	utm_coordinates_anglePointer->setText( 1, areaUtmAngleLabelineEdit->text() );
}

void Main_crava::on_inlineStartLineEdit_editingFinished(){
	il_startPointer->setText( 1, inlineStartLineEdit->text() );
}

void Main_crava::on_crosslineStartLineEdit_editingFinished(){
	xl_startPointer->setText( 1, crosslineStartLineEdit->text() );
}

void Main_crava::on_inlineEndLineEdit_editingFinished(){
	il_endPointer->setText( 1, inlineEndLineEdit->text() );
}

void Main_crava::on_crosslineEndLineEdit_editingFinished(){
	xl_endPointer->setText( 1, crosslineEndLineEdit->text() );
}

void Main_crava::on_inlineStepLineEdit_editingFinished(){
	il_stepPointer->setText( 1, inlineStepLineEdit->text() );
}

void Main_crava::on_crosslineStepLineEdit_editingFinished(){
	xl_stepPointer->setText( 1, crosslineStepLineEdit->text() );
}

void Main_crava::earthVpFile(const QString & value){
	earth_model_vp_filePointer->setText( 1, standard->StandardStrings::relativeFileName(value) );
}

void Main_crava::earthVsFile(const QString & value){
	earth_model_vs_filePointer->setText( 1, standard->StandardStrings::relativeFileName(value) );
}

void Main_crava::earthDensityFile(const QString & value){
	earth_model_density_filePointer->setText( 1, standard->StandardStrings::relativeFileName(value) );
}

void Main_crava::on_earthVpFileLineEdit_editingFinished(){
	earthVpFile(earthVpFileLineEdit->text());
}

void Main_crava::on_earthVsFileLineEdit_editingFinished(){
	earthVsFile(earthVsFileLineEdit->text());
}

void Main_crava::on_earthDensityFileLineEdit_editingFinished(){
	earthDensityFile(earthDensityFileLineEdit->text());
}

void Main_crava::on_earthVpFileBrowsePushButton_clicked(){
	QString fileName = QFileDialog::getOpenFileName(this, QString("Open File"), standard->StandardStrings::inputPath(), StandardStrings::seismicFormat());
	if(!fileName.isNull()){
		earthVpFileLineEdit->setText(fileName);
		earthVpFile(fileName);
	}
}

void Main_crava::on_earthVsFileBrowsePushButton_clicked(){
	QString fileName = QFileDialog::getOpenFileName(this, QString("Open File"), standard->StandardStrings::inputPath(), StandardStrings::seismicFormat());
	if(!fileName.isNull()){
		earthVsFileLineEdit->setText(fileName);
		earthVsFile(fileName);
	}
}

void Main_crava::on_earthDensityFileBrowsePushButton_clicked(){
	QString fileName = QFileDialog::getOpenFileName(this, QString("Open File"), standard->StandardStrings::inputPath(), StandardStrings::seismicFormat());
	if(!fileName.isNull()){
		earthDensityFileLineEdit->setText(fileName);
		earthDensityFile(fileName);
	}
}

void Main_crava::showContextMenu(const QPoint& pos)
{
	QPoint globalPos = wellHeaderListWidget->viewport()->mapToGlobal(pos);
	
	//add all the options given when right clicking
	QMenu myMenu;
	myMenu.addAction( QString("Time") );
	myMenu.addAction("Density");
	myMenu.addAction("Vp");
	myMenu.addAction("DT");
	myMenu.addAction("Vs");
	myMenu.addAction("DTS");
	if (inversion_settings_facies_probabilitiesPointer->text(1) == QString("yes") ){
		myMenu.addAction("Facies");
	}
	
	//This determines what each button should do
	QAction* selectedItem = myMenu.exec(globalPos);
	if (selectedItem)
	{

		if (wellHeaderListWidget->currentRow() >=0){
			if (selectedItem->text() == QString("Time") ){
				QString time = wellHeaderListWidget->currentItem()->text();
				timeLineEdit->setText( time );
				log_names_timePointer->setText(1, time );
			}
			else if (selectedItem->text() == QString("Vp") ){
				QString vp = wellHeaderListWidget->currentItem()->text();
				vpLineEdit->setText( vp );
				vpRadioButton->setChecked(true);
				log_names_vpPointer->setText(1, vp );
				log_names_dtPointer->setText(1, QString() ); //vp and dt cannot be active at the same time
			}
			else if (selectedItem->text() == QString("DT") ){
				QString dt = wellHeaderListWidget->currentItem()->text();
				dtLineEdit->setText( dt );
				dtRadioButton->setChecked(true);
				log_names_dtPointer->setText(1, dt );
				log_names_vsPointer->setText(1, QString() );
			}
			else if (selectedItem->text() == QString("Vs") ){
				QString vs = wellHeaderListWidget->currentItem()->text();
				vsLineEdit->setText( vs );
				vsRadioButton->setChecked(true);
				log_names_vsPointer->setText(1, vs );
				log_names_dtsPointer->setText(1, QString() ); //vs and dts cannot be active at the same time
			}
			else if (selectedItem->text() == QString("DTS") ){
				QString dts = wellHeaderListWidget->currentItem()->text();
				dtsLineEdit->setText( dts );
				dtsRadioButton->setChecked(true);
				log_names_dtsPointer->setText(1, dts );
				log_names_vsPointer->setText(1, QString() );
			}
			else if (selectedItem->text() == QString("Density") ){
				QString density = wellHeaderListWidget->currentItem()->text();
				densityLineEdit->setText( density );
				log_names_densityPointer->setText(1, density );
			}
			else if (selectedItem->text() == QString("Facies") ){
				QString facies = wellHeaderListWidget->currentItem()->text();
				faciesLineEdit->setText( facies );
				log_names_faciesPointer->setText(1, facies );
			}
		}
	}
	else
	{
		//nothing was chosen so nothing happens
	}
}

void Main_crava::findCorrectAngleGather(QTreeWidgetItem** itemParent){
	*itemParent = xmlTreeWidget->topLevelItem(0)->child(1)->child( 2+stackListWidget->currentRow() ); //changes the pointer
}

void Main_crava::findCorrectWell(QTreeWidgetItem** itemParent){
	*itemParent = xmlTreeWidget->topLevelItem(0)->child(2)->child( 1+wellListWidget->currentRow() ); //changes the pointer
}

void Main_crava::findCorrectOptimizePosition(QTreeWidgetItem** itemParent){
	*itemParent = xmlTreeWidget->topLevelItem(0)->child(2)->child( 1+wellListWidget->currentRow() )->child( 6+optimizePositionListWidget->currentRow() ); //changes the pointer
}

void Main_crava::findCorrectFacies(QTreeWidgetItem** itemParent){
	*itemParent = xmlTreeWidget->topLevelItem(0)->child(3)->child(7)->child(4)->child( faciesListWidget->currentRow() ); //changes the pointer
}

QList<QTreeWidgetItem*> Main_crava::getAllAngleGathers(){
	QList<QTreeWidgetItem*> items = xmlTreeWidget->findItems( QString("angle-gather"), Qt::MatchExactly | Qt::MatchRecursive, 0);
	return items;
}

void Main_crava::getValueFromAngleGather(QTreeWidgetItem* item, QString itemInAngleGather, QString &value, QString parentName){
	if (item->text(0) == itemInAngleGather){
		if ( itemInAngleGather == QString("file-name") || itemInAngleGather == QString("estimate-scale") ){//these two values appear twice so an extra check is needed
			if (item->parent()->text(0) == parentName ){
				value = item->text(1);
			}
		}
		else{
			value = item->text(1);
		}
	}
	for(int i=0; i<item->childCount(); i++){
		getValueFromAngleGather(item->child(i), itemInAngleGather, value, parentName); //recursive call to all the children
	}
}

void Main_crava::getValueFromWell(QTreeWidgetItem* item, QString itemInWell, QString &value){
	if (item->text(0) == itemInWell){
		value = item->text(1);
	}
	for(int i=0; i<item->childCount(); i++){
		getValueFromWell(item->child(i), itemInWell, value); //recursive call to all the children
	}
}

void Main_crava::getValueFromOptimizePosition(QTreeWidgetItem* item, QString itemInOptimizePosition, QString &value){
	if (item->text(0) == itemInOptimizePosition){
		value = item->text(1);
	}
	for(int i=0; i<item->childCount(); i++){
		getValueFromOptimizePosition(item->child(i), itemInOptimizePosition, value); //recursive call to all the children
	}
}

void Main_crava::getValueFromFacies(QTreeWidgetItem* item, QString itemInFacies, QString &value){
	if (item->text(0) == itemInFacies){
		value = item->text(1);
	}
	for(int i=0; i<item->childCount(); i++){
		getValueFromFacies(item->child(i), itemInFacies, value); //recursive call to all the children
	}
}

void Main_crava::setValueInAngleGather(QTreeWidgetItem* item, QString itemInAngleGather, QString value, QString parentName){
	if (item->text(0) == itemInAngleGather){
		if ( itemInAngleGather == QString("file-name") || itemInAngleGather == QString("estimate-scale") ){//these two values appear twice so an extra check is needed
			if (item->parent()->text(0) == parentName ){
				item->setText(1, value);
			}
		}
		else{
			item->setText(1, value);
		}
	}
	for(int i=0; i<item->childCount(); i++){
		setValueInAngleGather(item->child(i), itemInAngleGather, value, parentName); //recursive call to all the children
	}
}

void Main_crava::setValueInWell(QTreeWidgetItem* item, QString itemInWell, QString value){
	if (item->text(0) == itemInWell){
		item->setText(1, value);
	}
	for(int i=0; i<item->childCount(); i++){
		setValueInWell(item->child(i), itemInWell, value); //recursive call to all the children
	}
}

void Main_crava::setValueInOptimizePosition(QTreeWidgetItem* item, QString itemInOptimizePosition, QString value){
	if (item->text(0) == itemInOptimizePosition){
		item->setText(1, value);
	}
	for(int i=0; i<item->childCount(); i++){
		setValueInOptimizePosition(item->child(i), itemInOptimizePosition, value); //recursive call to all the children
	}
}

void Main_crava::setValueInFacies(QTreeWidgetItem* item, QString itemInFacies, QString value){
	if (item->text(0) == itemInFacies){
		item->setText(1, value);
	}
	for(int i=0; i<item->childCount(); i++){
		setValueInFacies(item->child(i), itemInFacies, value); //recursive call to all the children
	}
}

void Main_crava::setDefaultValues(){
	minimum_vpPointer->setText( 1, QString("1300.0") );
	maximum_vpPointer->setText( 1, QString("7000.0") );
	minimum_vsPointer->setText( 1, QString("200.0") );
	maximum_vsPointer->setText( 1, QString("4200.0") );
	minimum_densityPointer->setText( 1, QString("1.4") );
	maximum_densityPointer->setText( 1, QString("3.3") );
	minimum_variance_vpPointer->setText( 1, QString("0.0005") );
	maximum_variance_vpPointer->setText( 1, QString("0.0250") );
	minimum_variance_vsPointer->setText( 1, QString("0.0010") );
	maximum_variance_vsPointer->setText( 1, QString("0.0500") );
	minimum_variance_densityPointer->setText( 1, QString("0.0002") );
	maximum_variance_densityPointer->setText( 1, QString("0.0100") );
	minimum_vp_vs_ratioPointer->setText( 1, QString("1.4") );
	maximum_vp_vs_ratioPointer->setText( 1, QString("3.0") );

	maximum_deviation_anglePointer->setText( 1, QString("15.0") );
	maximum_rank_correlationPointer->setText( 1, QString("0.99") );
	maximum_merge_distancePointer->setText( 1, QString("0.01") );
	maximum_offsetPointer->setText( 1, QString("250.0") );
	maximum_shiftPointer->setText( 1, QString("11.0") );

}

void Main_crava::activateTable(){
	//table

	//actions
	actionsPointer = xmlTreeWidget->topLevelItem(0)->child(0);
		modePointer = xmlTreeWidget->topLevelItem(0)->child(0)->child(0);
		inversion_settingsPointer = xmlTreeWidget->topLevelItem(0)->child(0)->child(1);
			predictionPointer = xmlTreeWidget->topLevelItem(0)->child(0)->child(1)->child(0);
				simulationPointer = xmlTreeWidget->topLevelItem(0)->child(0)->child(1)->child(1);
					simulation_seedPointer = xmlTreeWidget->topLevelItem(0)->child(0)->child(1)->child(1)->child(0);
					simulation_seed_filePointer = xmlTreeWidget->topLevelItem(0)->child(0)->child(1)->child(1)->child(1);
					simulation_number_of_simulationsPointer = xmlTreeWidget->topLevelItem(0)->child(0)->child(1)->child(1)->child(2);
				inversion_settings_kriging_to_wellsPointer = xmlTreeWidget->topLevelItem(0)->child(0)->child(1)->child(2);
				inversion_settings_facies_probabilitiesPointer = xmlTreeWidget->topLevelItem(0)->child(0)->child(1)->child(3);
			estimation_settingsPointer = xmlTreeWidget->topLevelItem(0)->child(0)->child(2);
				estimation_settings_estimate_backgroundPointer = xmlTreeWidget->topLevelItem(0)->child(0)->child(2)->child(0);
				estimation_settings_estimate_correlationsPointer = xmlTreeWidget->topLevelItem(0)->child(0)->child(2)->child(1);
				estimation_settings_estimate_wavelet_or_noisePointer = xmlTreeWidget->topLevelItem(0)->child(0)->child(2)->child(2);

	//survey
	surveyPointer = xmlTreeWidget->topLevelItem(0)->child(1);
		survey_angular_correlationPointer = xmlTreeWidget->topLevelItem(0)->child(1)->child(0);
			survey_variogram_typePointer = xmlTreeWidget->topLevelItem(0)->child(1)->child(0)->child(0);
			survey_anglePointer = xmlTreeWidget->topLevelItem(0)->child(1)->child(0)->child(1);
			survey_rangePointer = xmlTreeWidget->topLevelItem(0)->child(1)->child(0)->child(2);
			survey_subrangePointer = xmlTreeWidget->topLevelItem(0)->child(1)->child(0)->child(3);
			survey_powerPointer = xmlTreeWidget->topLevelItem(0)->child(1)->child(0)->child(4);
		survey_segy_start_timePointer = xmlTreeWidget->topLevelItem(0)->child(1)->child(1);
		survey_wavelet_estimation_intervalPointer = xmlTreeWidget->topLevelItem(0)->child(1)->child(2);
			survey_top_surface_filePointer = xmlTreeWidget->topLevelItem(0)->child(1)->child(2)->child(0);
			survey_base_surface_filePointer = xmlTreeWidget->topLevelItem(0)->child(1)->child(2)->child(1);

	//well-data
	well_dataPointer = xmlTreeWidget->topLevelItem(0)->child(2);
		log_namesPointer = xmlTreeWidget->topLevelItem(0)->child(2)->child(0);
			log_names_timePointer = xmlTreeWidget->topLevelItem(0)->child(2)->child(0)->child(0);
			log_names_vpPointer = xmlTreeWidget->topLevelItem(0)->child(2)->child(0)->child(1);
			log_names_dtPointer = xmlTreeWidget->topLevelItem(0)->child(2)->child(0)->child(2);
			log_names_vsPointer = xmlTreeWidget->topLevelItem(0)->child(2)->child(0)->child(3);
			log_names_dtsPointer = xmlTreeWidget->topLevelItem(0)->child(2)->child(0)->child(4);
			log_names_densityPointer = xmlTreeWidget->topLevelItem(0)->child(2)->child(0)->child(5);
			log_names_faciesPointer = xmlTreeWidget->topLevelItem(0)->child(2)->child(0)->child(6);
		high_cut_seismic_resolutionPointer = xmlTreeWidget->topLevelItem(0)->child(2)->child(1);
		allowed_parameter_valuesPointer = xmlTreeWidget->topLevelItem(0)->child(2)->child(2);
			minimum_vpPointer = xmlTreeWidget->topLevelItem(0)->child(2)->child(2)->child(0);
			maximum_vpPointer = xmlTreeWidget->topLevelItem(0)->child(2)->child(2)->child(1);
			minimum_vsPointer = xmlTreeWidget->topLevelItem(0)->child(2)->child(2)->child(2);
			maximum_vsPointer = xmlTreeWidget->topLevelItem(0)->child(2)->child(2)->child(3);
			minimum_densityPointer = xmlTreeWidget->topLevelItem(0)->child(2)->child(2)->child(4);
			maximum_densityPointer = xmlTreeWidget->topLevelItem(0)->child(2)->child(2)->child(5);
			minimum_variance_vpPointer = xmlTreeWidget->topLevelItem(0)->child(2)->child(2)->child(6);
			maximum_variance_vpPointer = xmlTreeWidget->topLevelItem(0)->child(2)->child(2)->child(7);
			minimum_variance_vsPointer = xmlTreeWidget->topLevelItem(0)->child(2)->child(2)->child(8);
			maximum_variance_vsPointer = xmlTreeWidget->topLevelItem(0)->child(2)->child(2)->child(9);
			minimum_variance_densityPointer = xmlTreeWidget->topLevelItem(0)->child(2)->child(2)->child(10);
			maximum_variance_densityPointer = xmlTreeWidget->topLevelItem(0)->child(2)->child(2)->child(11);
			minimum_vp_vs_ratioPointer = xmlTreeWidget->topLevelItem(0)->child(2)->child(2)->child(12);
			maximum_vp_vs_ratioPointer = xmlTreeWidget->topLevelItem(0)->child(2)->child(2)->child(13);
		maximum_deviation_anglePointer = xmlTreeWidget->topLevelItem(0)->child(2)->child(3);
		maximum_rank_correlationPointer = xmlTreeWidget->topLevelItem(0)->child(2)->child(4);
		maximum_merge_distancePointer = xmlTreeWidget->topLevelItem(0)->child(2)->child(5);
		maximum_offsetPointer = xmlTreeWidget->topLevelItem(0)->child(2)->child(6);
		maximum_shiftPointer = xmlTreeWidget->topLevelItem(0)->child(2)->child(7);

	//prior-model
	prior_modelPointer = xmlTreeWidget->topLevelItem(0)->child(3);
		backgroundPointer = xmlTreeWidget->topLevelItem(0)->child(3)->child(0);
			background_vp_filePointer = xmlTreeWidget->topLevelItem(0)->child(3)->child(0)->child(0);
			background_vs_filePointer = xmlTreeWidget->topLevelItem(0)->child(3)->child(0)->child(1);
			background_density_filePointer = xmlTreeWidget->topLevelItem(0)->child(3)->child(0)->child(2);
			background_vp_constantPointer = xmlTreeWidget->topLevelItem(0)->child(3)->child(0)->child(3);
			background_vs_constantPointer = xmlTreeWidget->topLevelItem(0)->child(3)->child(0)->child(4);
			background_density_constantPointer = xmlTreeWidget->topLevelItem(0)->child(3)->child(0)->child(5);
			background_velocity_fieldPointer = xmlTreeWidget->topLevelItem(0)->child(3)->child(0)->child(6);
			background_lateral_correlationPointer = xmlTreeWidget->topLevelItem(0)->child(3)->child(0)->child(7);
				background_variogram_typePointer = xmlTreeWidget->topLevelItem(0)->child(3)->child(0)->child(7)->child(0);
				background_anglePointer = xmlTreeWidget->topLevelItem(0)->child(3)->child(0)->child(7)->child(1);
				background_rangePointer = xmlTreeWidget->topLevelItem(0)->child(3)->child(0)->child(7)->child(2);
				background_subrangePointer = xmlTreeWidget->topLevelItem(0)->child(3)->child(0)->child(7)->child(3);
				background_powerPointer = xmlTreeWidget->topLevelItem(0)->child(3)->child(0)->child(7)->child(4);
			background_high_cut_background_modellingPointer = xmlTreeWidget->topLevelItem(0)->child(3)->child(0)->child(8);
		earth_modelPointer = xmlTreeWidget->topLevelItem(0)->child(3)->child(1);
			earth_model_vp_filePointer = xmlTreeWidget->topLevelItem(0)->child(3)->child(1)->child(0);
			earth_model_vs_filePointer = xmlTreeWidget->topLevelItem(0)->child(3)->child(1)->child(1);
			earth_model_density_filePointer = xmlTreeWidget->topLevelItem(0)->child(3)->child(1)->child(2);
		local_waveletPointer = xmlTreeWidget->topLevelItem(0)->child(3)->child(2);
			local_wavelet_lateral_correlationPointer = xmlTreeWidget->topLevelItem(0)->child(3)->child(2)->child(0);
				local_wavelet_variogram_typePointer = xmlTreeWidget->topLevelItem(0)->child(3)->child(2)->child(0)->child(0);
				local_wavelet_anglePointer = xmlTreeWidget->topLevelItem(0)->child(3)->child(2)->child(0)->child(1);
				local_wavelet_rangePointer = xmlTreeWidget->topLevelItem(0)->child(3)->child(2)->child(0)->child(2);
				local_wavelet_subrangePointer = xmlTreeWidget->topLevelItem(0)->child(3)->child(2)->child(0)->child(3);
				local_wavelet_powerPointer = xmlTreeWidget->topLevelItem(0)->child(3)->child(2)->child(0)->child(4);
		prior_model_lateral_correlationPointer = xmlTreeWidget->topLevelItem(0)->child(3)->child(3);
			prior_model_variogram_typePointer = xmlTreeWidget->topLevelItem(0)->child(3)->child(3)->child(0);
			prior_model_anglePointer = xmlTreeWidget->topLevelItem(0)->child(3)->child(3)->child(1);
			prior_model_rangePointer = xmlTreeWidget->topLevelItem(0)->child(3)->child(3)->child(2);
			prior_model_subrangePointer = xmlTreeWidget->topLevelItem(0)->child(3)->child(3)->child(3);
			prior_model_powerPointer = xmlTreeWidget->topLevelItem(0)->child(3)->child(3)->child(4);
		temporal_correlationPointer = xmlTreeWidget->topLevelItem(0)->child(3)->child(4);
		parameter_correlationPointer = xmlTreeWidget->topLevelItem(0)->child(3)->child(5);	
		correlation_directionPointer = xmlTreeWidget->topLevelItem(0)->child(3)->child(6);
		prior_model_facies_probabilitiesPointer = xmlTreeWidget->topLevelItem(0)->child(3)->child(7);
			facies_probabilities_use_vsPointer = xmlTreeWidget->topLevelItem(0)->child(3)->child(7)->child(0);
			facies_probabilities_use_predictionPointer = xmlTreeWidget->topLevelItem(0)->child(3)->child(7)->child(1);
			facies_probabilities_use_absolute_elastic_parametersPointer = xmlTreeWidget->topLevelItem(0)->child(3)->child(7)->child(2);
			facies_probabilities_estimation_intervalPointer = xmlTreeWidget->topLevelItem(0)->child(3)->child(7)->child(3);
				facies_probabilities_top_surface_filePointer = xmlTreeWidget->topLevelItem(0)->child(3)->child(7)->child(3)->child(0);
				facies_probabilities_base_surface_filePointer = xmlTreeWidget->topLevelItem(0)->child(3)->child(7)->child(3)->child(1);
			prior_probabilitesPointer = xmlTreeWidget->topLevelItem(0)->child(3)->child(7)->child(4);
			uncertainty_levelPointer = xmlTreeWidget->topLevelItem(0)->child(3)->child(7)->child(5);


	//project-settings
	project_settingsPointer = xmlTreeWidget->topLevelItem(0)->child(4);
		//project-settings/output-volume
		output_volumePointer = xmlTreeWidget->topLevelItem(0)->child(4)->child(0);
			interval_two_surfacesPointer = xmlTreeWidget->topLevelItem(0)->child(4)->child(0)->child(0);
				interval_two_surfaces_top_surfacePointer = xmlTreeWidget->topLevelItem(0)->child(4)->child(0)->child(0)->child(0);
					top_surface_time_filePointer = xmlTreeWidget->topLevelItem(0)->child(4)->child(0)->child(0)->child(0)->child(0);
					top_surface_time_valuePointer = xmlTreeWidget->topLevelItem(0)->child(4)->child(0)->child(0)->child(0)->child(1);
					top_surface_depth_filePointer = xmlTreeWidget->topLevelItem(0)->child(4)->child(0)->child(0)->child(0)->child(2);
				interval_two_surfaces_base_surfacePointer = xmlTreeWidget->topLevelItem(0)->child(4)->child(0)->child(0)->child(1);
					base_surface_time_filePointer = xmlTreeWidget->topLevelItem(0)->child(4)->child(0)->child(0)->child(1)->child(0);
					base_surface_time_valuePointer = xmlTreeWidget->topLevelItem(0)->child(4)->child(0)->child(0)->child(1)->child(1);
					base_surface_depth_filePointer = xmlTreeWidget->topLevelItem(0)->child(4)->child(0)->child(0)->child(1)->child(2);
				interval_two_surfaces_number_of_layersPointer = xmlTreeWidget->topLevelItem(0)->child(4)->child(0)->child(0)->child(2);
				interval_two_surfaces_velocity_fieldPointer = xmlTreeWidget->topLevelItem(0)->child(4)->child(0)->child(0)->child(3);
				interval_two_surfaces_velocity_field_from_inversionPointer = xmlTreeWidget->topLevelItem(0)->child(4)->child(0)->child(0)->child(4);
			interval_one_surfacePointer = xmlTreeWidget->topLevelItem(0)->child(4)->child(0)->child(1);
				interval_one_surface_reference_surfacePointer = xmlTreeWidget->topLevelItem(0)->child(4)->child(0)->child(1)->child(0);
				interval_one_surface_shift_to_interval_topPointer = xmlTreeWidget->topLevelItem(0)->child(4)->child(0)->child(1)->child(1);
				interval_one_surface_thicknessPointer = xmlTreeWidget->topLevelItem(0)->child(4)->child(0)->child(1)->child(2);
				interval_one_surface_sample_densityPointer = xmlTreeWidget->topLevelItem(0)->child(4)->child(0)->child(1)->child(3);
			area_from_surfacePointer = xmlTreeWidget->topLevelItem(0)->child(4)->child(0)->child(2);
				area_from_surface_file_namePointer = xmlTreeWidget->topLevelItem(0)->child(4)->child(0)->child(2)->child(0);
			utm_coordinatesPointer = xmlTreeWidget->topLevelItem(0)->child(4)->child(0)->child(3);
				utm_coordinates_reference_point_xPointer = xmlTreeWidget->topLevelItem(0)->child(4)->child(0)->child(3)->child(0);
				utm_coordinates_reference_point_yPointer = xmlTreeWidget->topLevelItem(0)->child(4)->child(0)->child(3)->child(1);
				utm_coordinates_length_xPointer = xmlTreeWidget->topLevelItem(0)->child(4)->child(0)->child(3)->child(2);
				utm_coordinates_length_yPointer = xmlTreeWidget->topLevelItem(0)->child(4)->child(0)->child(3)->child(3);
				utm_coordinates_sample_density_xPointer = xmlTreeWidget->topLevelItem(0)->child(4)->child(0)->child(3)->child(4);
				utm_coordinates_sample_density_yPointer = xmlTreeWidget->topLevelItem(0)->child(4)->child(0)->child(3)->child(5);
				utm_coordinates_anglePointer = xmlTreeWidget->topLevelItem(0)->child(4)->child(0)->child(3)->child(6);
			inline_crossline_numbersPointer = xmlTreeWidget->topLevelItem(0)->child(4)->child(0)->child(4);
				il_startPointer = xmlTreeWidget->topLevelItem(0)->child(4)->child(0)->child(4)->child(0);
				il_endPointer = xmlTreeWidget->topLevelItem(0)->child(4)->child(0)->child(4)->child(1);
				xl_startPointer = xmlTreeWidget->topLevelItem(0)->child(4)->child(0)->child(4)->child(2);
				xl_endPointer = xmlTreeWidget->topLevelItem(0)->child(4)->child(0)->child(4)->child(3);
				il_stepPointer = xmlTreeWidget->topLevelItem(0)->child(4)->child(0)->child(4)->child(4);
				xl_stepPointer = xmlTreeWidget->topLevelItem(0)->child(4)->child(0)->child(4)->child(5);
		//project-settings/time-3D-mapping
		time_3D_mappingPointer = xmlTreeWidget->topLevelItem(0)->child(4)->child(1);
			time_3D_mapping_reference_depthPointer = xmlTreeWidget->topLevelItem(0)->child(4)->child(1)->child(0);
			time_3D_mappingPointer_average_velocity = xmlTreeWidget->topLevelItem(0)->child(4)->child(1)->child(1);
			time_3D_mappingPointer_reference_time_surface = xmlTreeWidget->topLevelItem(0)->child(4)->child(1)->child(2);
		//project-settings/io-settings
		io_settingsPointer = xmlTreeWidget->topLevelItem(0)->child(4)->child(2);
			top_directoryPointer = xmlTreeWidget->topLevelItem(0)->child(4)->child(2)->child(0);
			input_directoryPointer = xmlTreeWidget->topLevelItem(0)->child(4)->child(2)->child(1);
			output_directoryPointer = xmlTreeWidget->topLevelItem(0)->child(4)->child(2)->child(2);
			grid_outputPointer = xmlTreeWidget->topLevelItem(0)->child(4)->child(2)->child(3);
				grid_output_domainPointer = xmlTreeWidget->topLevelItem(0)->child(4)->child(2)->child(3)->child(0);
					grid_output_depthPointer = xmlTreeWidget->topLevelItem(0)->child(4)->child(2)->child(3)->child(0)->child(0);
					grid_output_timePointer = xmlTreeWidget->topLevelItem(0)->child(4)->child(2)->child(3)->child(0)->child(1);
				grid_output_formatPointer = xmlTreeWidget->topLevelItem(0)->child(4)->child(2)->child(3)->child(1);
					segy_formatPointer = xmlTreeWidget->topLevelItem(0)->child(4)->child(2)->child(3)->child(1)->child(0);
						segy_format_standard_formatPointer = xmlTreeWidget->topLevelItem(0)->child(4)->child(2)->child(3)->child(1)->child(0)->child(0);
						segy_format_location_xPointer = xmlTreeWidget->topLevelItem(0)->child(4)->child(2)->child(3)->child(1)->child(0)->child(1);
						segy_format_location_yPointer = xmlTreeWidget->topLevelItem(0)->child(4)->child(2)->child(3)->child(1)->child(0)->child(2);
						segy_format_location_ilPointer = xmlTreeWidget->topLevelItem(0)->child(4)->child(2)->child(3)->child(1)->child(0)->child(3);
						segy_format_location_xlPointer = xmlTreeWidget->topLevelItem(0)->child(4)->child(2)->child(3)->child(1)->child(0)->child(4);
						segy_format_bypass_coordinate_scalingPointer = xmlTreeWidget->topLevelItem(0)->child(4)->child(2)->child(3)->child(1)->child(0)->child(5);
						segy_format_location_scaling_coefficientPointer = xmlTreeWidget->topLevelItem(0)->child(4)->child(2)->child(3)->child(1)->child(0)->child(6);
					format_segyPointer = xmlTreeWidget->topLevelItem(0)->child(4)->child(2)->child(3)->child(1)->child(1);
					format_stormPointer = xmlTreeWidget->topLevelItem(0)->child(4)->child(2)->child(3)->child(1)->child(2);
					format_cravaPointer = xmlTreeWidget->topLevelItem(0)->child(4)->child(2)->child(3)->child(1)->child(3);
					format_sgriPointer = xmlTreeWidget->topLevelItem(0)->child(4)->child(2)->child(3)->child(1)->child(4);
					format_asciiPointer = xmlTreeWidget->topLevelItem(0)->child(4)->child(2)->child(3)->child(1)->child(5);
				elastic_parametersPointer = xmlTreeWidget->topLevelItem(0)->child(4)->child(2)->child(3)->child(2);
					elastic_parameters_vpPointer = xmlTreeWidget->topLevelItem(0)->child(4)->child(2)->child(3)->child(2)->child(0);
					elastic_parameters_vsPointer = xmlTreeWidget->topLevelItem(0)->child(4)->child(2)->child(3)->child(2)->child(1);
					elastic_parameters_densityPointer = xmlTreeWidget->topLevelItem(0)->child(4)->child(2)->child(3)->child(2)->child(2);
					elastic_parameters_lame_lambdaPointer = xmlTreeWidget->topLevelItem(0)->child(4)->child(2)->child(3)->child(2)->child(3);
					elastic_parameters_lame_muPointer = xmlTreeWidget->topLevelItem(0)->child(4)->child(2)->child(3)->child(2)->child(4);
					elastic_parameters_poisson_ratioPointer = xmlTreeWidget->topLevelItem(0)->child(4)->child(2)->child(3)->child(2)->child(5);
					elastic_parameters_aiPointer = xmlTreeWidget->topLevelItem(0)->child(4)->child(2)->child(3)->child(2)->child(6);
					elastic_parameters_siPointer = xmlTreeWidget->topLevelItem(0)->child(4)->child(2)->child(3)->child(2)->child(7);
					elastic_parameters_vp_vs_ratioPointer = xmlTreeWidget->topLevelItem(0)->child(4)->child(2)->child(3)->child(2)->child(8);
					elastic_parameters_murhoPointer = xmlTreeWidget->topLevelItem(0)->child(4)->child(2)->child(3)->child(2)->child(9);
					elastic_parameters_lambdarhoPointer = xmlTreeWidget->topLevelItem(0)->child(4)->child(2)->child(3)->child(2)->child(10);
					elastic_parameters_backgroundPointer = xmlTreeWidget->topLevelItem(0)->child(4)->child(2)->child(3)->child(2)->child(11);
					elastic_parameters_background_trendPointer = xmlTreeWidget->topLevelItem(0)->child(4)->child(2)->child(3)->child(2)->child(12);
				grid_output_seismic_dataPointer = xmlTreeWidget->topLevelItem(0)->child(4)->child(2)->child(3)->child(3);
					seismic_data_originalPointer = xmlTreeWidget->topLevelItem(0)->child(4)->child(2)->child(3)->child(3)->child(0);
					seismic_data_syntheticPointer = xmlTreeWidget->topLevelItem(0)->child(4)->child(2)->child(3)->child(3)->child(1);
					seismic_data_residualsPointer = xmlTreeWidget->topLevelItem(0)->child(4)->child(2)->child(3)->child(3)->child(2);
				grid_output_other_parametersPointer = xmlTreeWidget->topLevelItem(0)->child(4)->child(2)->child(3)->child(4);
					grid_output_facies_probabilitiesPointer = xmlTreeWidget->topLevelItem(0)->child(4)->child(2)->child(3)->child(4)->child(0);
					grid_output_facies_probabilities_with_undefPointer = xmlTreeWidget->topLevelItem(0)->child(4)->child(2)->child(3)->child(4)->child(1);
					grid_output_time_to_depth_velocityPointer = xmlTreeWidget->topLevelItem(0)->child(4)->child(2)->child(3)->child(4)->child(2);
					grid_output_extra_gridsPointer = xmlTreeWidget->topLevelItem(0)->child(4)->child(2)->child(3)->child(4)->child(3);
					grid_output_correlationsPointer = xmlTreeWidget->topLevelItem(0)->child(4)->child(2)->child(3)->child(4)->child(4);
			well_outputPointer = xmlTreeWidget->topLevelItem(0)->child(4)->child(2)->child(4);
				well_output_formatPointer = xmlTreeWidget->topLevelItem(0)->child(4)->child(2)->child(4)->child(0);
					well_output_rmsPointer = xmlTreeWidget->topLevelItem(0)->child(4)->child(2)->child(4)->child(0)->child(0);
					well_output_norsarPointer = xmlTreeWidget->topLevelItem(0)->child(4)->child(2)->child(4)->child(0)->child(1);
				well_output_wellsPointer = xmlTreeWidget->topLevelItem(0)->child(4)->child(2)->child(4)->child(1);
				well_output_blocked_wellsPointer = xmlTreeWidget->topLevelItem(0)->child(4)->child(2)->child(4)->child(2);
				well_output_blocked_logsPointer = xmlTreeWidget->topLevelItem(0)->child(4)->child(2)->child(4)->child(3);
			wavelet_outputPointer = xmlTreeWidget->topLevelItem(0)->child(4)->child(2)->child(5);
				wavelet_output_formatPointer = xmlTreeWidget->topLevelItem(0)->child(4)->child(2)->child(5)->child(0);
					wavelet_output_jasonPointer = xmlTreeWidget->topLevelItem(0)->child(4)->child(2)->child(5)->child(0)->child(0);
					wavelet_output_norsarPointer = xmlTreeWidget->topLevelItem(0)->child(4)->child(2)->child(5)->child(0)->child(1);
				wavelet_output_well_waveletsPointer = xmlTreeWidget->topLevelItem(0)->child(4)->child(2)->child(5)->child(1);
				wavelet_output_global_waveletsPointer = xmlTreeWidget->topLevelItem(0)->child(4)->child(2)->child(5)->child(2);
				wavelet_output_local_waveletsPointer = xmlTreeWidget->topLevelItem(0)->child(4)->child(2)->child(5)->child(3);
			io_settings_other_outputPointer = xmlTreeWidget->topLevelItem(0)->child(4)->child(2)->child(6);
				io_settings_extra_surfacesPointer = xmlTreeWidget->topLevelItem(0)->child(4)->child(2)->child(6)->child(0);
				io_settings_prior_correlationsPointer = xmlTreeWidget->topLevelItem(0)->child(4)->child(2)->child(6)->child(1);
				io_settings_background_trend_1dPointer = xmlTreeWidget->topLevelItem(0)->child(4)->child(2)->child(6)->child(2);
				io_settings_local_noisePointer = xmlTreeWidget->topLevelItem(0)->child(4)->child(2)->child(6)->child(3);
			io_settings_file_output_prefixPointer = xmlTreeWidget->topLevelItem(0)->child(4)->child(2)->child(7);
			io_settings_log_levelPointer = xmlTreeWidget->topLevelItem(0)->child(4)->child(2)->child(8);
		//project-settings/advanced-settings
		advanced_settingsPointer = xmlTreeWidget->topLevelItem(0)->child(4)->child(3);
			fft_grid_paddingPointer = xmlTreeWidget->topLevelItem(0)->child(4)->child(3)->child(0);
				x_fractionPointer = xmlTreeWidget->topLevelItem(0)->child(4)->child(3)->child(0)->child(0);
				y_fractionPointer = xmlTreeWidget->topLevelItem(0)->child(4)->child(3)->child(0)->child(1);
				z_fractionPointer = xmlTreeWidget->topLevelItem(0)->child(4)->child(3)->child(0)->child(2);
			use_intermediate_disk_storagePointer = xmlTreeWidget->topLevelItem(0)->child(4)->child(3)->child(1);
			maximum_relative_thickness_differencePointer = xmlTreeWidget->topLevelItem(0)->child(4)->child(3)->child(2);
			frequency_bandPointer = xmlTreeWidget->topLevelItem(0)->child(4)->child(3)->child(3);
				frequency_band_low_cutPointer = xmlTreeWidget->topLevelItem(0)->child(4)->child(3)->child(3)->child(0);
				frequency_band_high_cutPointer = xmlTreeWidget->topLevelItem(0)->child(4)->child(3)->child(3)->child(1);
			energy_tresholdPointer = xmlTreeWidget->topLevelItem(0)->child(4)->child(3)->child(4);
			wavelet_tapering_lengthPointer = xmlTreeWidget->topLevelItem(0)->child(4)->child(3)->child(5);
			minimum_relative_wavelet_amplitudePointer = xmlTreeWidget->topLevelItem(0)->child(4)->child(3)->child(6);
			maximum_wavelet_shiftPointer = xmlTreeWidget->topLevelItem(0)->child(4)->child(3)->child(7);
			white_noise_component_cutPointer = xmlTreeWidget->topLevelItem(0)->child(4)->child(3)->child(8);
			reflection_matrixPointer = xmlTreeWidget->topLevelItem(0)->child(4)->child(3)->child(9);
			kriging_data_limitPointer = xmlTreeWidget->topLevelItem(0)->child(4)->child(3)->child(10);
			debug_levelPointer = xmlTreeWidget->topLevelItem(0)->child(4)->child(3)->child(11);
			smooth_kriged_parametersPointer = xmlTreeWidget->topLevelItem(0)->child(4)->child(3)->child(12);
}
