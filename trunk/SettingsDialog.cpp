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
#include "SettingsDialog.h"
#include <QFileDialog> 
#include "Main_crava.h"

/**
	@author Alf Birger Rustad (RD IRE FRM) <abir@statoil.com> Øystein Arneson (RD IRE FRM) <oyarn@statoil.com>, Erik Bakken <eriba@statoil.com>, Andreas B. Lindblad <al587793@statoil.com>
*/
SettingsDialog::SettingsDialog(Main_crava *main_crava, QWidget *parent, bool forwardOn, bool estimationOn, StandardStrings *standard):QDialog(parent){
	this->standard=standard;
	this->main_crava = main_crava;
	setupUi(this);
	setWindowTitle(QString("%1[*] - %2").arg(windowTitle()).arg(StandardStrings::cravaVersion()));
	QButtonGroup *headerButtonGroup = new QButtonGroup(inputOutputPage);
	headerButtonGroup->addButton(headerSeisWorksRadioButton);
	headerButtonGroup->addButton(headerIesxRadioButton);
	headerButtonGroup->addButton(headerSipRadioButton);
	headerButtonGroup->addButton(headerCharismaRadioButton);
	headerButtonGroup->addButton(headerUserDefinedRadioButton);
	on_headerUserDefinedRadioButton_toggled(false);
       	on_vpVsUserDefinedRadioButton_toggled(!main_crava->vp_vs_ratioPointer->text(1).isEmpty());
	forward=forwardOn;
	estimation=estimationOn;
	updateFields();
}

void SettingsDialog::updateFields(){

	referenceDepth3DLineEdit->setText(main_crava->time_3D_mapping_reference_depthPointer->text(1));
	averageVelocity3DLineEdit->setText(main_crava->time_3D_mappingPointer_average_velocity->text(1));
	referenceTimeSurface3DLineEdit->setText(main_crava->time_3D_mappingPointer_reference_time_surface->text(1));
	segyCheckBox->setChecked(StandardStrings::checkedBool(main_crava->format_segyPointer->text(1)));
	stormCheckBox->setChecked(StandardStrings::checkedBool(main_crava->format_stormPointer->text(1),QString("yes")));
	cravaCheckBox->setChecked(StandardStrings::checkedBool(main_crava->format_cravaPointer->text(1)));
	sgriCheckBox->setChecked(StandardStrings::checkedBool(main_crava->format_sgriPointer->text(1)));
	asciiCheckBox->setChecked(StandardStrings::checkedBool(main_crava->format_asciiPointer->text(1)));
	if(logComboBox->findText(main_crava->io_settings_log_levelPointer->text(1))!=-1){
		logComboBox->setCurrentIndex(logComboBox->findText(main_crava->io_settings_log_levelPointer->text(1)));
	}

	QSettings settings("Statoil","CRAVA");
	settings.beginGroup("crava");
	cravaPathLineEdit->setText(settings.value(QString("executable"),QString("/project/res/x86_64_RH_5/bin/crava")).toString());
	textEditorLineEdit->setText(settings.value(QString("editor"),QString("emacs")).toString());
	terminalPathLineEdit->setText(settings.value(QString("terminal"),QString("konsole")).toString());
	terminalCheckBox->setChecked(settings.value(QString("useterminal"),true).toBool());
	on_terminalCheckBox_toggled(terminalCheckBox->isChecked());
	wikiPathLineEdit->setText(settings.value(QString("wiki"),QString()).toString());
	manualPathLineEdit->setText(settings.value(QString("manual"),QString("manual/CRAVA_user_manual.pdf")).toString());
	settings.beginGroup("GUI");
	xmlTreeCheckBox->setChecked(settings.value(QString("showtree"),true).toBool());
	if(!forward){
		if(main_crava->segy_format_standard_formatPointer->text(1)==QString("seisworks")){
			headerSeisWorksRadioButton->setChecked(true);
		}
		else if(main_crava->segy_format_standard_formatPointer->text(1)==QString("iesx")){
			headerIesxRadioButton->setChecked(true);
		}
		else if(main_crava->segy_format_standard_formatPointer->text(1)==QString("SIP")){
			headerSipRadioButton->setChecked(true);
		}
		else if(main_crava->segy_format_standard_formatPointer->text(1)==QString("charisma")){
			headerCharismaRadioButton->setChecked(true);
		}
		else {
			headerUserDefinedRadioButton->setChecked(true);
			formatChangeFrame->setVisible(true);
			if ( bypassCoordinateNoRadioButton->isChecked() ){
				locationScalingLineEdit->setVisible(true);
				location_scaling_label_2->setVisible(true);
			}
			else{
			       	locationScalingLineEdit->setVisible(false);
				location_scaling_label_2->setVisible(false);
			}

			xCoordLineEdit->setText(main_crava->segy_format_location_xPointer->text(1));
			yCoordLineEdit->setText(main_crava->segy_format_location_yPointer->text(1));
			inlineLineEdit->setText(main_crava->segy_format_location_ilPointer->text(1));
			crosslineLineEdit->setText(main_crava->segy_format_location_xlPointer->text(1));
			locationScalingLineEdit->setText(main_crava->segy_format_location_scaling_coefficientPointer->text(1));
			if(main_crava->segy_format_bypass_coordinate_scalingPointer->text(1)==QString("yes")){
				bypassCoordinateYesRadioButton->setChecked(true);
			}
			else if(main_crava->segy_format_bypass_coordinate_scalingPointer->text(1)==QString("no")){
				bypassCoordinateNoRadioButton->setChecked(true);
			}
			else {
				bypassCoordinateEstimateRadioButton->setChecked(true);
			}
		}
		on_segyCheckBox_toggled(segyCheckBox->isChecked());
		rmsCheckBox->setChecked(StandardStrings::checkedBool(main_crava->well_output_rmsPointer->text(1),QString("yes")));
		norsarWellCheckBox->setChecked(StandardStrings::checkedBool(main_crava->well_output_norsarPointer->text(1)));
		jasonCheckBox->setChecked(StandardStrings::checkedBool(main_crava->wavelet_output_jasonPointer->text(1),QString("yes")));
		norsarWaveletCheckBox->setChecked(StandardStrings::checkedBool(main_crava->wavelet_output_norsarPointer->text(1)));

		if(!estimation){

			settings.beginGroup("tree");
			settings.beginGroup(main_crava->simulationPointer->text(0));
			if(!main_crava->simulation_seed_filePointer->text(1).isEmpty()){
				seedFileLineEdit->setText(main_crava->simulation_seed_filePointer->text(1));
			}
			else {
				seedFileLineEdit->setText(settings.value(main_crava->simulation_seed_filePointer->text(0),QString("")).toString());//seed-file
			}
			xFractionLineEdit->setText(main_crava->x_fractionPointer->text(1));
			yFractionLineEdit->setText(main_crava->y_fractionPointer->text(1));
			zFractionLineEdit->setText(main_crava->z_fractionPointer->text(1));

			if(main_crava->use_intermediate_disk_storagePointer->text(1)==QString("yes")){
				diskStorageCheckBox->setChecked(true);
			}
			/*else{ not needed as it starts unchecked
				checker=QString("no");
			}*/
			maximumRelativeThicknessLineEdit->setText(main_crava->maximum_relative_thickness_differencePointer->text(1));

			lowCutFrequencyBandLineEdit->setText(main_crava->frequency_band_low_cutPointer->text(1));
			highCutFrequencyBandLineEdit->setText(main_crava->frequency_band_high_cutPointer->text(1));
		}
		else{
			fftPaddingLabel->setVisible(false);
			xFractionLabel->setVisible(false);
			yFractionLabel->setVisible(false);
			zFractionLabel->setVisible(false);
			xFractionLineEdit->setVisible(false);
			yFractionLineEdit->setVisible(false);
			zFractionLineEdit->setVisible(false);
			diskStorageCheckBox->setVisible(false);
			maximumRelativeThicknessLabel->setVisible(false);
			maximumRelativeThicknessLineEdit->setVisible(false);
			vpVsRatioLabel->setVisible(false);
			vpVsRatioFrame->setVisible(false);
			frequencyBandLabel->setVisible(false);
			frequencyBandFrame->setVisible(false);
			fftPaddingLabel->setEnabled(false);
			xFractionLabel->setEnabled(false);
			yFractionLabel->setEnabled(false);
			zFractionLabel->setEnabled(false);
			xFractionLineEdit->setEnabled(false);
			yFractionLineEdit->setEnabled(false);
			zFractionLineEdit->setEnabled(false);
			diskStorageCheckBox->setEnabled(false);
			maximumRelativeThicknessLabel->setEnabled(false);
			maximumRelativeThicknessLineEdit->setEnabled(false);
			vpVsRatioLabel->setEnabled(false);
			vpVsRatioFrame->setEnabled(false);
			frequencyBandLabel->setEnabled(false);
			frequencyBandFrame->setEnabled(false);
		}
		//advanced-settings
	       	if(main_crava->vp_vs_ratio_from_wellsPointer->text(1)==QString("yes")){
		  vpVsEstimateWellRadioButton->setChecked(true);
		  
		}
		else if(!main_crava->vp_vs_ratioPointer->text(1).isEmpty()){
		  vpVsUserDefinedRadioButton->setChecked(true);
		  vpVsUserDefinedLineEdit->setText(main_crava->vp_vs_ratioPointer->text(1));
		}
		else{
		  vpVsEstimateBackgroundRadioButton->setChecked(true);
		}
		seismicResolutionLineEdit->setText(main_crava->high_cut_seismic_resolutionPointer->text(1));
		energyTresholdLineEdit->setText(main_crava->energy_tresholdPointer->text(1));
		waveletLengthLineEdit->setText(main_crava->wavelet_tapering_lengthPointer->text(1));
		waveletRelativeMinimumLineEdit->setText(main_crava->minimum_relative_wavelet_amplitudePointer->text(1));
		waveletShiftMaxLineEdit->setText(main_crava->maximum_wavelet_shiftPointer->text(1));
		whiteNoiseLineEdit->setText(main_crava->white_noise_component_cutPointer->text(1));
		reflectionMatrixLineEdit->setText(main_crava->reflection_matrixPointer->text(1));
		krigingDataLineEdit->setText(main_crava->kriging_data_limitPointer->text(1));
		guardZoneLineEdit->setText(main_crava->guard_zonePointer->text(1));
		//not possible to set debug level
		if(main_crava->smooth_kriged_parametersPointer->text(1)==QString("yes")){
			krigedSmoothCheckBox->setChecked(true);
		}

		vpMinLineEdit->setText(main_crava->minimum_vpPointer->text(1));
		vpMaxLineEdit->setText(main_crava->maximum_vpPointer->text(1));
		vsMinLineEdit->setText(main_crava->minimum_vsPointer->text(1));
		vsMaxLineEdit->setText(main_crava->maximum_vsPointer->text(1));
		densityMinLineEdit->setText(main_crava->minimum_densityPointer->text(1));
		densityMaxLineEdit->setText(main_crava->maximum_densityPointer->text(1));
		vpVarianceMinLineEdit->setText(main_crava->minimum_variance_vpPointer->text(1));
		vpVarianceMaxLineEdit->setText(main_crava->maximum_variance_vpPointer->text(1));
		vsVarianceMinLineEdit->setText(main_crava->minimum_variance_vsPointer->text(1));
		vsVarianceMaxLineEdit->setText(main_crava->maximum_variance_vsPointer->text(1));
		densityVarianceMinLineEdit->setText(main_crava->minimum_variance_densityPointer->text(1));
		densityVarianceMaxLineEdit->setText(main_crava->maximum_variance_densityPointer->text(1));
		vpVsRatioMinLineEdit->setText(main_crava->minimum_vp_vs_ratioPointer->text(1));
		vpVsRatioMaxLineEdit->setText(main_crava->maximum_vp_vs_ratioPointer->text(1));

		deviationAngleLineEdit->setText(main_crava->maximum_deviation_anglePointer->text(1));
		vpVsCorrelationLineEdit->setText(main_crava->maximum_rank_correlationPointer->text(1));
		mergeDistanceLineEdit->setText(main_crava->maximum_merge_distancePointer->text(1));
		offsetLineEdit->setText(main_crava->maximum_offsetPointer->text(1));
		shiftLineEdit->setText(main_crava->maximum_shiftPointer->text(1));
	}
	else {
		while(listWidget->count()>2){
			delete stackedWidget->widget(1);
			delete listWidget->takeItem(1);
		}
		headerFormatLabel->setVisible(false);
		headerSeisWorksRadioButton->setVisible(false);
		headerIesxRadioButton->setVisible(false);
		headerSipRadioButton->setVisible(false);
		headerCharismaRadioButton->setVisible(false);
		headerUserDefinedRadioButton->setVisible(false);
		wellFormatLabel->setVisible(false);
		rmsCheckBox->setVisible(false);
		norsarWellCheckBox->setVisible(false);
		waveletFormatLabel->setVisible(false);
		jasonCheckBox->setVisible(false);
		norsarWaveletCheckBox->setVisible(false);
	}
	listWidget->setCurrentRow(0);
}
void SettingsDialog::updateSettings(){

	main_crava->time_3D_mapping_reference_depthPointer->setText(1,referenceDepth3DLineEdit->text());
	main_crava->time_3D_mappingPointer_average_velocity->setText(1,averageVelocity3DLineEdit->text());
		if(standard->StandardStrings::fileExists(referenceTimeSurface3DLineEdit->text())){
			main_crava->time_3D_mappingPointer_reference_time_surface->setText(1,standard->StandardStrings::relativeFileName(referenceTimeSurface3DLineEdit->text()));
		}
	main_crava->format_segyPointer->setText(1,StandardStrings::checkedString(segyCheckBox->isChecked()));
	main_crava->format_stormPointer->setText(1,StandardStrings::checkedString(stormCheckBox->isChecked()));
	main_crava->format_cravaPointer->setText(1,StandardStrings::checkedString(cravaCheckBox->isChecked()));
	main_crava->format_sgriPointer->setText(1,StandardStrings::checkedString(sgriCheckBox->isChecked()));
	main_crava->format_asciiPointer->setText(1,StandardStrings::checkedString(asciiCheckBox->isChecked()));

	QSettings settings("Statoil","CRAVA");
	settings.beginGroup("crava");
	if(standard->StandardStrings::fileExists(cravaPathLineEdit->text())){
		settings.setValue(QString("executable"),cravaPathLineEdit->text());
	}
	settings.setValue( QString("editor"), textEditorLineEdit->text() );
	settings.setValue(QString("terminal"),terminalPathLineEdit->text());
	settings.setValue(QString("useterminal"),terminalCheckBox->isChecked());
	settings.setValue(QString("wiki"),wikiPathLineEdit->text());
	settings.setValue(QString("manual"),manualPathLineEdit->text());
	settings.beginGroup("GUI");
	settings.setValue(QString("showtree"),xmlTreeCheckBox->isChecked());
	if(!forward){
		QString headerformat("");
		if(headerSeisWorksRadioButton->isChecked()){
			headerformat=QString("seisworks");
		} 
		else if(headerIesxRadioButton->isChecked()){
			headerformat=QString("iesx");
		}
		else if(headerSipRadioButton->isChecked()){
			headerformat=QString("SIP");
		}
		else if(headerCharismaRadioButton->isChecked()){
			headerformat=QString("charisma");
		}
		else {
			main_crava->segy_format_location_xPointer->setText(1,xCoordLineEdit->text());
			main_crava->segy_format_location_yPointer->setText(1,yCoordLineEdit->text());
			main_crava->segy_format_location_ilPointer->setText(1,inlineLineEdit->text());
			main_crava->segy_format_location_xlPointer->setText(1,crosslineLineEdit->text());
			main_crava->segy_format_location_scaling_coefficientPointer->setText(1,locationScalingLineEdit->text());

			if(bypassCoordinateYesRadioButton->isChecked()){
				main_crava->segy_format_bypass_coordinate_scalingPointer->setText(1,QString("yes"));
				main_crava->segy_format_location_scaling_coefficientPointer->setText(1,QString(""));
			}
			else if(bypassCoordinateNoRadioButton->isChecked()){        
			      	main_crava->segy_format_bypass_coordinate_scalingPointer->setText(1,QString("no"));;
			}
			else{
			        main_crava->segy_format_bypass_coordinate_scalingPointer->setText(1,QString(""));
				main_crava->segy_format_location_scaling_coefficientPointer->setText(1,QString(""));
			}
	
			
		}

		if(!headerUserDefinedRadioButton->isChecked()){
		        main_crava->segy_format_location_xPointer->setText(1,QString(""));
			main_crava->segy_format_location_yPointer->setText(1,QString(""));
			main_crava->segy_format_location_ilPointer->setText(1,QString(""));
			main_crava->segy_format_location_xlPointer->setText(1,QString(""));
			main_crava->segy_format_bypass_coordinate_scalingPointer->setText(1,QString(""));
			main_crava->segy_format_location_scaling_coefficientPointer->setText(1,QString(""));
		} 
		
		main_crava->segy_format_standard_formatPointer->setText(1,headerformat);
		main_crava->well_output_rmsPointer->setText(1,StandardStrings::checkedString(rmsCheckBox->isChecked()));
		main_crava->well_output_norsarPointer->setText(1,StandardStrings::checkedString(norsarWellCheckBox->isChecked()));

		main_crava->wavelet_output_jasonPointer->setText(1,StandardStrings::checkedString(jasonCheckBox->isChecked()));
		main_crava->wavelet_output_norsarPointer->setText(1,StandardStrings::checkedString(norsarWaveletCheckBox->isChecked()));

		main_crava->io_settings_log_levelPointer->setText(1,logComboBox->currentText());

		QString checker;
		if(!estimation){
			settings.beginGroup("tree");
			settings.beginGroup(main_crava->simulationPointer->text(0));
			if(standard->StandardStrings::fileExists(seedFileLineEdit->text())){
				settings.setValue(main_crava->simulation_seed_filePointer->text(0),seedFileLineEdit->text());//seed-file
				if(!main_crava->simulation_number_of_simulationsPointer->text(1).isEmpty()){
					if(main_crava->simulation_seedPointer->text(1).isEmpty()){
						main_crava->simulation_seed_filePointer->setText(1,seedFileLineEdit->text());
					}
				}
			}
			main_crava->x_fractionPointer->setText(1,xFractionLineEdit->text());
			main_crava->y_fractionPointer->setText(1,yFractionLineEdit->text());
			main_crava->z_fractionPointer->setText(1,zFractionLineEdit->text());
			
			if(diskStorageCheckBox->isChecked()){
				checker=QString("yes");
			}
			else{
				checker=QString("no");
			}
			//advanced-settings
			main_crava->use_intermediate_disk_storagePointer->setText(1,checker);
			main_crava->maximum_relative_thickness_differencePointer->setText(1,maximumRelativeThicknessLineEdit->text());

			main_crava->frequency_band_low_cutPointer->setText(1,lowCutFrequencyBandLineEdit->text());
			main_crava->frequency_band_high_cutPointer->setText(1,highCutFrequencyBandLineEdit->text());
		    	if(vpVsEstimateBackgroundRadioButton->isChecked()){
			        main_crava->vp_vs_ratioPointer->setText(1,QString(""));
				main_crava->vp_vs_ratio_from_wellsPointer->setText(1,QString(""));
			}
			else if(vpVsEstimateWellRadioButton->isChecked()){
		                main_crava->vp_vs_ratio_from_wellsPointer->setText(1,QString("yes"));
				main_crava->vp_vs_ratioPointer->setText(1,QString(""));
			}
			else{
		                main_crava->vp_vs_ratioPointer->setText(1,vpVsUserDefinedLineEdit->text());
				main_crava->vp_vs_ratio_from_wellsPointer->setText(1,QString(""));
			}
		}
		//advanced-settings
		main_crava->high_cut_seismic_resolutionPointer->setText(1,seismicResolutionLineEdit->text());
		main_crava->energy_tresholdPointer->setText(1,energyTresholdLineEdit->text());
		main_crava->wavelet_tapering_lengthPointer->setText(1,waveletLengthLineEdit->text());
		main_crava->minimum_relative_wavelet_amplitudePointer->setText(1,waveletRelativeMinimumLineEdit->text());
		main_crava->maximum_wavelet_shiftPointer->setText(1,waveletShiftMaxLineEdit->text());
		main_crava->white_noise_component_cutPointer->setText(1,whiteNoiseLineEdit->text());
		if(standard->StandardStrings::fileExists(reflectionMatrixLineEdit->text())){
			main_crava->reflection_matrixPointer->setText(1,standard->StandardStrings::relativeFileName(reflectionMatrixLineEdit->text()));
		}
		main_crava->kriging_data_limitPointer->setText(1,krigingDataLineEdit->text());
		main_crava->guard_zonePointer->setText(1,guardZoneLineEdit->text());
		//not possible to set debug level
		if(krigedSmoothCheckBox->isChecked()){
			checker=QString("yes");
		}
		else{
			checker=QString("no");
		}
		main_crava->smooth_kriged_parametersPointer->setText(1,checker);
		main_crava->minimum_vpPointer->setText(1,vpMinLineEdit->text());
		main_crava->maximum_vpPointer->setText(1,vpMaxLineEdit->text());
		main_crava->minimum_vsPointer->setText(1,vsMinLineEdit->text());
		main_crava->maximum_vsPointer->setText(1,vsMaxLineEdit->text());
		main_crava->minimum_densityPointer->setText(1,densityMinLineEdit->text());
		main_crava->maximum_densityPointer->setText(1,densityMaxLineEdit->text());
		main_crava->minimum_variance_vpPointer->setText(1,vpVarianceMinLineEdit->text());
		main_crava->maximum_variance_vpPointer->setText(1,vpVarianceMaxLineEdit->text());
		main_crava->minimum_variance_vsPointer->setText(1,vsVarianceMinLineEdit->text());

		reflectionMatrixLineEdit->setText(main_crava->reflection_matrixPointer->text(1));//reflection-matrix

		main_crava->maximum_variance_vsPointer->setText(1,vsVarianceMaxLineEdit->text());
		main_crava->minimum_variance_densityPointer->setText(1,densityVarianceMinLineEdit->text());
		main_crava->maximum_variance_densityPointer->setText(1,densityVarianceMaxLineEdit->text());
		main_crava->minimum_vp_vs_ratioPointer->setText(1,vpVsRatioMinLineEdit->text());
		main_crava->maximum_vp_vs_ratioPointer->setText(1,vpVsRatioMaxLineEdit->text());

		main_crava->maximum_deviation_anglePointer->setText(1,deviationAngleLineEdit->text());
		main_crava->maximum_rank_correlationPointer->setText(1,vpVsCorrelationLineEdit->text());
		main_crava->maximum_merge_distancePointer->setText(1,mergeDistanceLineEdit->text());
		main_crava->maximum_offsetPointer->setText(1,offsetLineEdit->text());
		main_crava->maximum_shiftPointer->setText(1,shiftLineEdit->text());
	}
}


void SettingsDialog::on_headerUserDefinedRadioButton_toggled(bool checked){
	formatChangeFrame->setVisible(checked);
	formatChangeFrame->setEnabled(checked);
	if(checked){
		bypassCoordinateEstimateRadioButton->setChecked(main_crava->segy_format_bypass_coordinate_scalingPointer->text(1)=="");
	        bypassCoordinateYesRadioButton->setChecked(main_crava->segy_format_bypass_coordinate_scalingPointer->text(1)=="yes");	
	        bypassCoordinateNoRadioButton->setChecked(main_crava->segy_format_bypass_coordinate_scalingPointer->text(1)=="no");	
							  
		if ( bypassCoordinateNoRadioButton->isChecked() ){
			locationScalingLineEdit->setVisible(true);
			location_scaling_label_2->setVisible(true);
		}
		else{
			locationScalingLineEdit->setVisible(false);
			location_scaling_label_2->setVisible(false);
		}
		xCoordLineEdit->setFocus();
        }

	//should enable editing
}

void SettingsDialog::on_reflectionMatrixBrowsePushButton_clicked(){
	QString fileName = QFileDialog::getOpenFileName(this, QString("Open File"), standard->StandardStrings::inputPath(), StandardStrings::asciiFormat());
	if(!fileName.isNull()){
		reflectionMatrixLineEdit->setText(fileName);
	}
}
void SettingsDialog::on_cravaPathBrowsePushButton_clicked(){
	QString fileName = QFileDialog::getOpenFileName(this, QString("Crava Executable"), standard->StandardStrings::inputPath(), QString("All files(*.*)"));//fix the file...
	if(!fileName.isNull()){
		cravaPathLineEdit->setText(fileName);
	}
}

void SettingsDialog::on_textEditorBrowsePushButton_clicked(){
	QString fileName = QFileDialog::getOpenFileName(this, QString("editor"), standard->StandardStrings::inputPath(), QString("All files(*.*)"));//fix the file...
	if(!fileName.isNull()){
		textEditorLineEdit->setText(fileName);
	}
}

void SettingsDialog::on_terminalPathBrowsePushButton_clicked(){
	QString fileName = QFileDialog::getOpenFileName(this, QString("Terminal"), standard->StandardStrings::inputPath(), QString("All files(*.*)"));//fix the file...
	if(!fileName.isNull()){
		terminalPathLineEdit->setText(fileName);
	}
}

void SettingsDialog::on_seedFileBrowsePushButton_clicked(){
	QString fileName = QFileDialog::getOpenFileName(this, QString("Seed file"), standard->StandardStrings::inputPath(), StandardStrings::asciiFormat());
	if(!fileName.isNull()){
		seedFileLineEdit->setText(fileName);
	}
}

void SettingsDialog::on_referenceTimeSurface3DPushButton_clicked(){
	QString fileName = QFileDialog::getOpenFileName(this, QString("Reference time surface"), standard->StandardStrings::inputPath(), StandardStrings::surfaceFormat());
	if(!fileName.isNull()){
		referenceTimeSurface3DLineEdit->setText(fileName);
	}
}
void SettingsDialog::on_manualPathBrowsePushButton_clicked(){
	QString fileName = QFileDialog::getOpenFileName(this, QString("Crava manual"), standard->StandardStrings::inputPath(), QString("All files(*.*)"));//fix the file...
	if(!fileName.isNull()){
		manualPathLineEdit->setText(fileName);
	}
}

void SettingsDialog::on_terminalCheckBox_toggled(bool checked){
	terminalPathLineEdit->setEnabled(checked);
	terminalPathBrowsePushButton->setEnabled(checked);
	terminalPathLabel->setEnabled(checked);
}
void SettingsDialog::on_segyCheckBox_toggled(bool checked){
	if(!checked){
		headerUserDefinedRadioButton->setChecked(true);
		formatChangeFrame->setVisible(false);
	}
	else{
		headerSeisWorksRadioButton->setChecked(true);
	}
	headerFormatLabel->setEnabled(checked);
	headerSeisWorksRadioButton->setEnabled(checked);
	headerIesxRadioButton->setEnabled(checked);
	headerSipRadioButton->setEnabled(checked);
	headerCharismaRadioButton->setEnabled(checked);
	headerUserDefinedRadioButton->setEnabled(checked);
}
