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
#include "OutputDialog.h"
#include "StandardStrings.h"
#include <QFileDialog> 

#include <iostream>
using namespace std;
/**
	@author Alf Birger Rustad (RD IRE FRM) <abir@statoil.com> Øystein Arneson (RD IRE FRM) <oyarn@statoil.com>, Erik Bakken <eriba@statoil.com>
*/
OutputDialog::OutputDialog(Main_crava *main_crava, QString *xmlFilename, QWidget *parent, bool faciesOn, bool forwardOn, bool estimationOn): QDialog(parent){

	this->xmlName = xmlFilename;
	this->main_crava = main_crava;
	setupUi(this);
	setWindowTitle(QString("%1[*] - %2").arg(windowTitle()).arg(StandardStrings::cravaVersion()));
	facies=faciesOn;
	forward=forwardOn;
	estimation=estimationOn;
	updateFields();
	//if(forward)resize(width(),minimumHeight()); causes problem with x_ConfigureWindow
}

void OutputDialog::updateOutput(){
	QDir topDirectory( main_crava->top_directoryPointer->text(1) );
	QString outputDir = topDirectory.relativeFilePath( outputDirectoryLineEdit->text() ); //output directory is relative to the working directory
	main_crava->output_directoryPointer->setText(1, outputDir);
	//cout << main_crava->output_directoryPointer->text(1).toStdString() << endl;

	if((!forward)&&(!estimation)){
		main_crava->grid_output_depthPointer->setText(1,StandardStrings::checkedString(domainDepthCheckBox->isChecked()));//depth
		main_crava->grid_output_timePointer->setText(1,StandardStrings::checkedString(domainTimeCheckBox->isChecked()));//time


		main_crava->seismic_data_originalPointer->setText(1,StandardStrings::checkedString(seismicOriginalCheckBox->isChecked()));
		main_crava->seismic_data_syntheticPointer->setText(1,StandardStrings::checkedString(seismicSyntheticCheckBox->isChecked()));
		main_crava->seismic_data_residualsPointer->setText(1,StandardStrings::checkedString(seismicResidualCheckBox->isChecked()));
		// elastic-parameters
		main_crava->elastic_parameters_vpPointer->setText(1,StandardStrings::checkedString(vpCheckBox->isChecked()));
		main_crava->elastic_parameters_vsPointer->setText(1,StandardStrings::checkedString(vsCheckBox->isChecked()));
		main_crava->elastic_parameters_densityPointer->setText(1,StandardStrings::checkedString(densityCheckBox->isChecked()));
		main_crava->elastic_parameters_lame_lambdaPointer->setText(1,StandardStrings::checkedString(lameLamCheckBox->isChecked()));
		main_crava->elastic_parameters_lame_muPointer->setText(1,StandardStrings::checkedString(lameMuCheckBox->isChecked()));
		main_crava->elastic_parameters_poisson_ratioPointer->setText(1,StandardStrings::checkedString(poissonRatioCheckBox->isChecked()));
		main_crava->elastic_parameters_aiPointer->setText(1,StandardStrings::checkedString(aiCheckBox->isChecked()));
		main_crava->elastic_parameters_siPointer->setText(1,StandardStrings::checkedString(siCheckBox->isChecked()));
		main_crava->elastic_parameters_vp_vs_ratioPointer->setText(1,StandardStrings::checkedString(vpVsRatioCheckBox->isChecked()));
		main_crava->elastic_parameters_murhoPointer->setText(1,StandardStrings::checkedString(lambdaRhoCheckBox->isChecked()));
		main_crava->elastic_parameters_lambdarhoPointer->setText(1,StandardStrings::checkedString(muRhoCheckBox->isChecked()));
		main_crava->elastic_parameters_backgroundPointer->setText(1,StandardStrings::checkedString(backgroundCheckBox->isChecked()));
		main_crava->elastic_parameters_background_trendPointer->setText(1,StandardStrings::checkedString(backgroundTrendCheckBox->isChecked()));
		//other-parameters
		if(facies){
			main_crava->grid_output_facies_probabilitiesPointer->setText(1,StandardStrings::checkedString(faciesProbabilitiesCheckBox->isChecked()));
			main_crava->grid_output_facies_probabilities_with_undefPointer->setText(1,StandardStrings::checkedString(faciesProbabilitiesUndefinedCheckBox->isChecked()));//facies-probabilities-with-undef=1 , they can be given at the same time
		}
		main_crava->grid_output_time_to_depth_velocityPointer->setText(1,StandardStrings::checkedString(timeDepthCheckBox->isChecked()));
		main_crava->grid_output_extra_gridsPointer->setText(1,StandardStrings::checkedString(extraGridsCheckBox->isChecked()));
		main_crava->grid_output_correlationsPointer->setText(1,StandardStrings::checkedString(correlationsCheckBox->isChecked()));

		//well-output
		main_crava->well_output_wellsPointer->setText(1,StandardStrings::checkedString(wellCheckBox->isChecked()));
		main_crava->well_output_blocked_wellsPointer->setText(1,StandardStrings::checkedString(blockedWellCheckBox->isChecked()));
		//blocke-logs does nothing atm

		//wavelet-output
		main_crava->wavelet_output_well_waveletsPointer->setText(1,StandardStrings::checkedString(waveletWellCheckBox->isChecked()));
		main_crava->wavelet_output_global_waveletsPointer->setText(1,StandardStrings::checkedString(waveletGlobalCheckBox->isChecked()));
		main_crava->wavelet_output_local_waveletsPointer->setText(1,StandardStrings::checkedString(waveletLocalCheckBox->isChecked()));
		//other-output
		//extra surface...
		main_crava->io_settings_prior_correlationsPointer->setText(1,StandardStrings::checkedString(priorCorrelationCheckBox->isChecked()));
		main_crava->io_settings_background_trend_1dPointer->setText(1,StandardStrings::checkedString(backgroundTrend1DCheckBox->isChecked()));
		main_crava->io_settings_local_noisePointer->setText(1,StandardStrings::checkedString(localNoiseCheckBox->isChecked()));
	}
	main_crava->io_settings_file_output_prefixPointer->setText(1,prefixLineEdit->text());//file-output-prefix
}

void OutputDialog::updateXmlName(){
	*(this->xmlName) = xmlFilenameLineEdit->text();
}

void OutputDialog::updateFields(){

	if((!forward)&&(!estimation)){
		domainDepthCheckBox->setChecked(StandardStrings::checkedBool(main_crava->grid_output_depthPointer->text(1)));//depth
		domainTimeCheckBox->setChecked(StandardStrings::checkedBool(main_crava->grid_output_timePointer->text(1),QString("yes")));//time

		seismicOriginalCheckBox->setChecked(StandardStrings::checkedBool(main_crava->seismic_data_originalPointer->text(1)));//original
		seismicSyntheticCheckBox->setChecked(StandardStrings::checkedBool(main_crava->seismic_data_syntheticPointer->text(1)));//synthetic
		seismicResidualCheckBox->setChecked(StandardStrings::checkedBool(main_crava->seismic_data_residualsPointer->text(1)));//residual

		vpCheckBox->setChecked(StandardStrings::checkedBool(main_crava->elastic_parameters_vpPointer->text(1),QString("yes")));//vp
		vsCheckBox->setChecked(StandardStrings::checkedBool(main_crava->elastic_parameters_vsPointer->text(1),QString("yes")));//vs
		densityCheckBox->setChecked(StandardStrings::checkedBool(main_crava->elastic_parameters_densityPointer->text(1),QString("yes")));//density
		lameLamCheckBox->setChecked(StandardStrings::checkedBool(main_crava->elastic_parameters_lame_lambdaPointer->text(1)));//lame-lambda
		lameMuCheckBox->setChecked(StandardStrings::checkedBool(main_crava->elastic_parameters_lame_muPointer->text(1)));//lame-mu
		poissonRatioCheckBox->setChecked(StandardStrings::checkedBool(main_crava->elastic_parameters_poisson_ratioPointer->text(1)));//poisson-ratio
		aiCheckBox->setChecked(StandardStrings::checkedBool(main_crava->elastic_parameters_aiPointer->text(1)));//ai
		siCheckBox->setChecked(StandardStrings::checkedBool(main_crava->elastic_parameters_siPointer->text(1)));//si
		vpVsRatioCheckBox->setChecked(StandardStrings::checkedBool(main_crava->elastic_parameters_vp_vs_ratioPointer->text(1)));//vp-vs-ratio
		lambdaRhoCheckBox->setChecked(StandardStrings::checkedBool(main_crava->elastic_parameters_murhoPointer->text(1)));//murho
		muRhoCheckBox->setChecked(StandardStrings::checkedBool(main_crava->elastic_parameters_lambdarhoPointer->text(1)));//lambdarho
		backgroundCheckBox->setChecked(StandardStrings::checkedBool(main_crava->elastic_parameters_backgroundPointer->text(1)));//background
		backgroundTrendCheckBox->setChecked(StandardStrings::checkedBool(main_crava->elastic_parameters_background_trendPointer->text(1)));//background-trend

		if(facies){
			faciesProbabilitiesCheckBox->setChecked(StandardStrings::checkedBool(main_crava->grid_output_facies_probabilitiesPointer->text(1),QString("yes")));
			faciesProbabilitiesUndefinedCheckBox->setChecked(StandardStrings::checkedBool(main_crava->grid_output_facies_probabilities_with_undefPointer->text(1)));//facies-probabilities-with-undef
		}
		else{
			faciesProbabilitiesCheckBox->setVisible(false);
			faciesProbabilitiesUndefinedCheckBox->setVisible(false);
			faciesProbabilitiesCheckBox->setEnabled(false);
			faciesProbabilitiesUndefinedCheckBox->setEnabled(false);
		}
		timeDepthCheckBox->setChecked(StandardStrings::checkedBool(main_crava->grid_output_time_to_depth_velocityPointer->text(1)));//time-to-depth-velocity
		extraGridsCheckBox->setChecked(StandardStrings::checkedBool(main_crava->grid_output_extra_gridsPointer->text(1)));//extra-grids
		correlationsCheckBox->setChecked(StandardStrings::checkedBool(main_crava->grid_output_correlationsPointer->text(1)));//correlations

		//well-output
		wellCheckBox->setChecked(StandardStrings::checkedBool(main_crava->well_output_wellsPointer->text(1)));//wells
		blockedWellCheckBox->setChecked(StandardStrings::checkedBool(main_crava->well_output_blocked_wellsPointer->text(1)));//blocked-wells
		//blocke-logs does nothing atm

		//wavelet-output
		waveletWellCheckBox->setChecked(StandardStrings::checkedBool(main_crava->wavelet_output_well_waveletsPointer->text(1)));//well-wavelets
		waveletGlobalCheckBox->setChecked(StandardStrings::checkedBool(main_crava->wavelet_output_global_waveletsPointer->text(1)));//global-wavelets
		waveletLocalCheckBox->setChecked(StandardStrings::checkedBool(main_crava->wavelet_output_local_waveletsPointer->text(1)));//local-wavelets
		//other-output
		//extra surface...
		priorCorrelationCheckBox->setChecked(StandardStrings::checkedBool(main_crava->io_settings_prior_correlationsPointer->text(1)));//prior-correlations
		backgroundTrend1DCheckBox->setChecked(StandardStrings::checkedBool(main_crava->io_settings_background_trend_1dPointer->text(1)));//background-trend-1d
		localNoiseCheckBox->setChecked(StandardStrings::checkedBool(main_crava->io_settings_local_noisePointer->text(1)));//local-noise
	}
	else{
		/*QList<QWidget*> widgets=toolBox->QObject::findChildren<QWidget*>(); not needed
		foreach (QWidget* widget, widgets){
			widget->setVisible(false);
			widget->setEnabled(false);
		}*/
		toolBox->setVisible(false);
		toolBox->setEnabled(false);
	}
	prefixLineEdit->setText( main_crava->io_settings_file_output_prefixPointer->text(1) );//file-output-prefix
	outputDirectoryLineEdit->setText( main_crava->top_directoryPointer->text(1) + main_crava->output_directoryPointer->text(1) );

	QSettings settings("Statoil","CRAVA");
	settings.beginGroup("crava");
	settings.beginGroup("GUI");
	xmlFilenameLineEdit->setText( settings.value( QString("xmlName"), QString("") ).toString() );
	settings.endGroup();
	settings.endGroup();

}

void OutputDialog::on_outputDirectoryBrowsePushButton_clicked(){
	QString dirName = QFileDialog::getExistingDirectory( this, QString("Open File"), main_crava->top_directoryPointer->text(1) );
	if(!dirName.isNull()){
		outputDirectoryLineEdit->setText(dirName);
	}
}

