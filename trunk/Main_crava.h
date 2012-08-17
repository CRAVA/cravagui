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
#ifndef MAIN_CRAVA_H
#define MAIN_CRAVA_H

#include "StandardStrings.h"
#include "ui_Main_crava.h"
#include <QtXml>


/**
	@author Alf Birger Rustad (RD IRE FRM) <abir@statoil.com> Øystein Arneson (RD IRE FRM) <oyarn@statoil.com>, Erik Bakken <eriba@statoil.com>, Andreas B. Lindblad <al587793@statoil.com>
*/
class Main_crava : public QMainWindow, private Ui::Main_crava{
	Q_OBJECT

	friend class ModeDialog;
	friend class SettingsDialog;
	friend class VariogramDialog;

public:
	Main_crava(QWidget *parent =0,bool existing=false,const QString &filename=QString());
	~Main_crava();

	
protected:
	void closeEvent(QCloseEvent *event);
	bool eventFilter(QObject *obj, QEvent *event);//used in this case to draw red borders on necessary fields that are empty.

private:
	/*bool writeXmlFromTree(const QString &fileName, QTreeWidget *tree);
	void recursiveXmlWrite(QXseismicDataFramemlStreamWriter *xmlWriter, QTreeWidgetItem *item);*/
	void updateGuiToTree();//used to put the correct values into all the fields from the tree and check the correct buttons on load
	bool writeXmlToTree(const QString &fileName, QTreeWidget *tree);//used on loading to update the tree with the values in xml
	void recursiveXmlRead(const QDomNode &xmlItem, QTreeWidgetItem *treeItem);//used on loading this is the function actually used to reading the xml
	bool writeXmlFromTree(const QString &fileName, QTreeWidget *tree);//using dom xml tree instead , used on saving to write xml to file from the tree
	void recursiveXmlWrite(QDomDocument *xmlDocument, QDomNode *parent, QTreeWidgetItem *item); //used on saving, this creates the domtree with all the non-empty values from the treewidget
	bool noXmlInfo(QTreeWidgetItem *item);//this is used to check whether things should be written to xml, as only non-empty items should be written.
	void variogram(QTreeWidgetItem *item);//this method opens a variogram modifying the correct data in the xml treewidget
	bool okToCloseCurrent();//this promts the user to save to xml before exiting.
       	bool okToRun();//this prompts the user to save to xml before running crava.
	//file handlers is probably wrong as what they actually handle is putting things in the tree, each is used by at least 2 slots(the line and the browse button).
	//stack file handlers
	void addStack();//adds another stack to the xml treewidget with the correct children and their children etc.
	void seismicFile(const QString &value);//this updates the filename of an anglestack in the tree. 
	void waveletFile(const QString &value);//this updates the filename of a wavelet in the tree for the selected angle stack.
	void shiftFile(const QString &value);//this updates the filename of a shift file for local wavelet in the tree for the selected angle stack.
	void scaleFile(const QString &value);//this updates the filename of a scale file for local wavelet in the tree for the selected angle stack.
	void localNoiseFile(const QString &value);//this updates the filename for local noise in the tree for the selected angle stack.
	//well file handlers
	void addWell();//adds another well to the xml treewidget with correct children and with the given filename.
	void wellFile(const QString & value);//changes the filename of the selected well in the tree.
	void addOptimizePosition();//adds another optimized position for the selected well in the xmltree with correct children.
	//horizon file handlers
	void topTimeFile(const QString & value);//updates the tree with the top time file surface for the inversion volume
	void topDepthFile(const QString & value);//updates the tree with the top depth file surface for depth conversion
	void bottomTimeFile(const QString & value);//updates the tree with the base time file surface for the inversion volume
	void bottomDepthFile(const QString & value);//updates the tree with the base depth file surface for depth conversion
	void velocityFieldFile(const QString & value);//updates the tree with velocity field file for depth conversion
	void referenceSurfaceFile(const QString & value);//updates the tree with the reference time file surface for the inversion volume, used in case there is only one surface
	void areaSurfaceFile(const QString & value);//updates the tree with the surface file for lateral inversion geometry
	void waveletTopFile(const QString & value);//the top time file for limiting the wavelet and noise estimation interval
	void waveletBottomFile(const QString & value);//the base time file for limiting the wavelet and noise estimation interval
	void faciesTopFile(const QString & value);//the top time file for limiting the facies estimation interval
	void faciesBottomFile(const QString & value);//the base time file for limiting the facies estimation interval
	//prior model file handlers
	void vpFile(const QString & value);//updates the tree with the vp file cube for given background
	void vsFile(const QString & value);//updates the tree with the vs file cube for given background
	void densityFile(const QString & value);//updates the tree with the density file cube for given background
	void aiFile(const QString & value);//updates the tree with the ai file cube for given background
	void siFile(const QString & value);//updates the tree with the si file cube for given background
	void vpVsFile(const QString & value);//updates the tree with the Vp/Vs file cube for given background
	void topSurfaceFile(const QString & value);//updates the tree with the correct file for top surface for multizone background
	void baseSurfaceFile(const QString & value);//updates the tree with the correct file for base surface for multizone background
	void velocityFieldPriorFile(const QString & value);//updates the tree with the correct file for the vp trend for extimated background
	void temporalCorrelationFile(const QString & value);//updates the tree with the correct file for temporal correlation for extimated background
	void parameterCorrelationFile(const QString & value);//updates the tree with the correct file for parameter correlation for extimated background
	void correlationDirectionFile(const QString & value);//updates the tree with the correct file for correlation direction, can only be used if two surfaces are given for the inversion interval
       	void addZone();//adds another zone with the correct children to the tree
	void insertZone();//inserts another zone with the correct children in the tree
	void addFacies();//adds another facies with the correct children to the tree
	void probabilityCubeFile(const QString & value);//updates the tree with the given probability cube.
	//earth model
	void earthVpFile(const QString & value);//updates the tree with the earth model vp cube file for forward mode.
	void earthVsFile(const QString & value);//updates the tree with the earth model vs cube file for forward mode.
	void earthDensityFile(const QString & value);//updates the tree with the earth model density cube file for forward mode.
	void earthAiFile(const QString & value);//updates the tree with the earth model AI cube file for forward mode.
	void earthSiFile(const QString & value);//updates the tree with the earth model SI cube file for forward mode.
	void earthVpVsFile(const QString & value);//updates the tree with the earth model Vp/Vs ratio cube file for forward mode.
	//file formats
	QString currentFile_;//holds the path of the current xml file, should not be accessed directly
	const QString& currentFile(){return currentFile_;}//gives the current file.
	bool saveFile(const QString &fileName);//saves the given xml file to disk, changes current file to that file. use writeXmlFromTree
	void setCurrentFile(const QString &fileName);//sets the current file to the given file and updates the name on the titlebar
	//setup
	void setupButtonGroups();//used under setup to fix radiobuttons to work as they are supposed to and hide buttons that are not wanted.
	void createActions();//most of the actions are specified in designer, this sets up shortcuts for them.
	void headerFormat(const QString & value);//used to send the correct value of headerformat into the tree depending on which radiobuttons is checked.
	void bypassCoordinate(const QString & value);//makes the correct value be put into the tree for bypass coordinate depending on the radiobuttons
	void synteticVsLog(const QString & value);//makes the correct value be put into the tree for synthetic vs log depending on the radiobuttons
	void vsForFacies(const QString & value);//makes the correct value be put into the tree for if vs should be used for facies depending on the radiobuttons
	void predictionFacies(const QString & value);//makes the correct value be put into the tree for whether prediction should be used for facies depending on the checkbox
	void absoluteParameters(const QString & value);//makes the correct value be put into the tree for whether absolute or releative parameter should be used for facies depending on the checkbox
	bool mode(bool started, bool *pressedOpen, bool existing, QString &filename);//opens the mode dialog initialized properly depending on whether it was started or not.
	void faciesGui();//updates the gui to show/hide the correct widgets and clear the tree depending on whether facies estimation is on or off
	bool faciesProbabilitiesOn();//wrapper for whether facies estimation is on or off
	bool forwardMode();//wrapper for whether it is forward mode or not
	bool estimationMode();//wrapper for whether it is estimation mode or not
	void forwardGui();//updates the gui to show/hide the correct widgets and clear the tree depending on whether it is forward mode or not
	void estimationGui();//updates the gui to show/hide the correct widgets and clear the tree depending on whether it is estimation mode or not
	void readSettings();//updates the tree with the settings, does not update the gui
	void readGuiSpecificSettings();//loads graphical settings on startup, should maybe be expanded.
	void writeSettings();//stores the tree settings to disk
	StandardStrings *standard;//object that keeps track of which filepath belong to this instance of the program and makes sure relative paths are correct and the correct path is opened for new filedialogs.
	bool getWellHeaders(const QString &fileName);//populates the well header log names list widget with names from the header of the selected log file in the well list widget

	void activateTable(); // sets all the pointers in the table

	void findCorrectAngleGather(QTreeWidgetItem** itemParent); //finds the angle gather which is selected in the list widget. The argument is a pointer to the pointer that one wants to change to the angle gather. IF THE TREE STRUCTURE IS CHANGED, THE INDICES HAVE TO BE CHANGED

	void findCorrectWell(QTreeWidgetItem** itemParent); //finds the well which is selected in the list widget. The argument is a pointer to the pointer that one wants to change to the well. IF THE TREE STRUCTURE IS CHANGED, THE INDICES HAVE TO BE CHANGED

	void findCorrectOptimizePosition(QTreeWidgetItem** itemParent); //finds the optimize position which is selected in the list widget. The argument is a pointer to the pointer that one wants to change to the optimize position. IF THE TREE STRUCTURE IS CHANGED, THE INDICES HAVE TO BE CHANGED

	void findCorrectZone(QTreeWidgetItem** itemParent);//finds the zone which is selected in the list widget. The argument is a pointer to the pointer that one wants to change to the facies. IF THE TREE STRUCTURE IS CHANGED, THE INDICES HAVE TO BE CHANGED

	void findCorrectFacies(QTreeWidgetItem** itemParent); //finds the facies which is selected in the list widget. The argument is a pointer to the pointer that one wants to change to the facies. IF THE TREE STRUCTURE IS CHANGED, THE INDICES HAVE TO BE CHANGED

	QList<QTreeWidgetItem*> getAllAngleGathers(); // returns a list of all angle gathers in the tree.

       	void deleteAllZones(); //deletes alle zones in the tree

	void deleteAllFacies(); //deletes all facies in the tree

	void deleteAllOptimizePosition(); //deletes all optimize position in tree

	void getValueFromAngleGather( QTreeWidgetItem* item, QString itemInAngleGather, QString &value, QString parentName = QString() ); //gets the wanted value in the tree, but only searches through the subtree consisting of the angle gather. First argument is the angle gather. The second argument is the quantity one wants (for instance file-name). The third argument is the return value. The fourth argument is the name of the parent of the second argument (some nodes in the tree have the same name). Recursive.

	void getValueFromWell(QTreeWidgetItem* item, QString itemInWell, QString &value); //gets the wanted value in the tree, but only searches through the subtree consisting of the well. First argument is the well. The second argument is the quantity one wants (for instance file-name). The third argument is the return value. Recursive.

	void getValueFromOptimizePosition(QTreeWidgetItem* item, QString itemInWell, QString &value); //gets the wanted value in the tree, but only searches through the subtree consisting of the optimize position. First argument is the optimize position. The second argument is the quantity one wants (for instance angle). The third argument is the return value. Recursive.

	void getValueFromZone(QTreeWidgetItem* item, QString itemInZone, QString &value); //gets the wanted value in the tree, but only searches through the subtree consisting of the facies. First argument is the zone. The second argument is the quantity one wants (for instance name). The third argument is the return value. Recursive.

	void getValueFromFacies(QTreeWidgetItem* item, QString itemInWell, QString &value); //gets the wanted value in the tree, but only searches through the subtree consisting of the facies. First argument is the facies. The second argument is the quantity one wants (for instance name). The third argument is the return value. Recursive.

	void setValueInAngleGather( QTreeWidgetItem* item, QString itemInAngleGather, QString value, QString parentName = QString() ); //sets the correct value in the tree, but only searches through the subtree consisting of the angle gather. First argument is the angle gather. The second argument is the quantity one wants to change (for instance file-name). The third argument is the value which will be put in the tree. The fourth argument is the name of the parent of the second argument (some nodes in the tree have the same name). Recursive.

	void setValueInWell(QTreeWidgetItem* item, QString itemInWell, QString value); //sets the correct value in the tree, but only searches through the subtree consisting of the well. First argument is the well. The second argument is the quantity one wants to change (for instance file-name). The third argument is the value which will be put in the tree. Recursive.

	void setValueInOptimizePosition(QTreeWidgetItem* item, QString itemInWell, QString value); //sets the correct value in the tree, but only searches through the subtree consisting of the optimize position. First argument is the optimize position. The second argument is the quantity one wants to change (for instance angle). The third argument is the value which will be put in the tree. Recursive.

	void setValueInZone(QTreeWidgetItem* item, QString itemInZone, QString value); //sets the correct value in the tree, but only searches through the subtree consisting of the zones. First argument is the zone. The second argument is the quantity one wants to change (for instance name). The third argument is the value which will be put in the tree. Recursive.

	void setValueInFacies(QTreeWidgetItem* item, QString itemInWell, QString value); //sets the correct value in the tree, but only searches through the subtree consisting of the facies. First argument is the facies. The second argument is the quantity one wants to change (for instance name). The third argument is the value which will be put in the tree. Recursive.


	void setDefaultValues(); // sets default values for some of the values in well-data.

       	QList<QObject*> getNecessaryFields();// returns a list with fields that are necessary. used to set red borders when empty

	void necessaryFieldGui();// sets borders on the necessary fields when empty

	//Table: This is a table which contains a pointer to all the nodes in tree (except for those what are added during the program). Gives easy access to all the values in the tree. The values are set in activateTable. Those that are added during the program have their own functions (for instance findCorrectAngleGather).

	//actions
	QTreeWidgetItem *actionsPointer;
		QTreeWidgetItem *modePointer;
		QTreeWidgetItem *inversion_settingsPointer;
			QTreeWidgetItem *predictionPointer;
			QTreeWidgetItem *simulationPointer;
				QTreeWidgetItem *simulation_seedPointer;
				QTreeWidgetItem *simulation_seed_filePointer;
				QTreeWidgetItem *simulation_number_of_simulationsPointer;
			QTreeWidgetItem *inversion_settings_kriging_to_wellsPointer;
			QTreeWidgetItem *inversion_settings_facies_probabilitiesPointer;
		QTreeWidgetItem *estimation_settingsPointer;
			QTreeWidgetItem *estimation_settings_estimate_backgroundPointer;
			QTreeWidgetItem *estimation_settings_estimate_correlationsPointer;
			QTreeWidgetItem *estimation_settings_estimate_wavelet_or_noisePointer;


	//survey
	QTreeWidgetItem *surveyPointer;
		QTreeWidgetItem *survey_angular_correlationPointer;
			QTreeWidgetItem *survey_variogram_typePointer;
			QTreeWidgetItem *survey_anglePointer;
			QTreeWidgetItem *survey_rangePointer;
			QTreeWidgetItem *survey_subrangePointer;
			QTreeWidgetItem *survey_powerPointer;
		QTreeWidgetItem *survey_segy_start_timePointer;
		QTreeWidgetItem *survey_wavelet_estimation_intervalPointer;
			QTreeWidgetItem *survey_top_surface_filePointer;
			QTreeWidgetItem *survey_base_surface_filePointer;


	//well-data
	QTreeWidgetItem *well_dataPointer;
		QTreeWidgetItem *log_namesPointer;
			QTreeWidgetItem *log_names_timePointer;
			QTreeWidgetItem *log_names_vpPointer;
			QTreeWidgetItem *log_names_dtPointer;
			QTreeWidgetItem *log_names_vsPointer;
			QTreeWidgetItem *log_names_dtsPointer;
			QTreeWidgetItem *log_names_densityPointer;
			QTreeWidgetItem *log_names_faciesPointer;
		QTreeWidgetItem *high_cut_seismic_resolutionPointer;
		QTreeWidgetItem *allowed_parameter_valuesPointer;
			QTreeWidgetItem *minimum_vpPointer;
			QTreeWidgetItem *maximum_vpPointer;
			QTreeWidgetItem *minimum_vsPointer;
			QTreeWidgetItem *maximum_vsPointer;
			QTreeWidgetItem *minimum_densityPointer;
			QTreeWidgetItem *maximum_densityPointer;
			QTreeWidgetItem *minimum_variance_vpPointer;
			QTreeWidgetItem *maximum_variance_vpPointer;
			QTreeWidgetItem *minimum_variance_vsPointer;
			QTreeWidgetItem *maximum_variance_vsPointer;
			QTreeWidgetItem *minimum_variance_densityPointer;
			QTreeWidgetItem *maximum_variance_densityPointer;
			QTreeWidgetItem *minimum_vp_vs_ratioPointer;
			QTreeWidgetItem *maximum_vp_vs_ratioPointer;
		QTreeWidgetItem *maximum_deviation_anglePointer;
		QTreeWidgetItem *maximum_rank_correlationPointer;
		QTreeWidgetItem *maximum_merge_distancePointer;
		QTreeWidgetItem *maximum_offsetPointer;
		QTreeWidgetItem *maximum_shiftPointer;

	//prior-model
	QTreeWidgetItem *prior_modelPointer;
		QTreeWidgetItem *backgroundPointer;
			QTreeWidgetItem *background_ai_filePointer;
			QTreeWidgetItem *background_si_filePointer;
			QTreeWidgetItem *background_vp_vs_ratio_filePointer;
			QTreeWidgetItem *background_vp_filePointer;
			QTreeWidgetItem *background_vs_filePointer;
			QTreeWidgetItem *background_density_filePointer;
			QTreeWidgetItem *background_vp_constantPointer;
			QTreeWidgetItem *background_vs_constantPointer;
			QTreeWidgetItem *background_density_constantPointer;
			QTreeWidgetItem *background_velocity_fieldPointer;
			QTreeWidgetItem *background_lateral_correlationPointer;
				QTreeWidgetItem *background_variogram_typePointer;
				QTreeWidgetItem *background_anglePointer;
				QTreeWidgetItem *background_rangePointer;
				QTreeWidgetItem *background_subrangePointer;
				QTreeWidgetItem *background_powerPointer;
			QTreeWidgetItem *background_high_cut_background_modellingPointer;
				QTreeWidgetItem *background_multizone_modelPointer;
			        QTreeWidgetItem *background_top_surface_filePointer;
				QTreeWidgetItem *background_top_surface_erosion_priorityPointer;
		QTreeWidgetItem *earth_modelPointer;
			QTreeWidgetItem *earth_model_vp_filePointer;
			QTreeWidgetItem *earth_model_vs_filePointer;
			QTreeWidgetItem *earth_model_density_filePointer;
	       	     	QTreeWidgetItem *earth_model_ai_filePointer;
			QTreeWidgetItem *earth_model_si_filePointer;
			QTreeWidgetItem *earth_model_vp_vs_ratio_filePointer;
		QTreeWidgetItem *local_waveletPointer;
			QTreeWidgetItem *local_wavelet_lateral_correlationPointer;
				QTreeWidgetItem *local_wavelet_variogram_typePointer;	
				QTreeWidgetItem *local_wavelet_anglePointer;
				QTreeWidgetItem *local_wavelet_rangePointer;
				QTreeWidgetItem *local_wavelet_subrangePointer;
				QTreeWidgetItem *local_wavelet_powerPointer;
		QTreeWidgetItem *prior_model_lateral_correlationPointer;
			QTreeWidgetItem *prior_model_variogram_typePointer;
			QTreeWidgetItem *prior_model_anglePointer;
			QTreeWidgetItem *prior_model_rangePointer;
			QTreeWidgetItem *prior_model_subrangePointer;
			QTreeWidgetItem *prior_model_powerPointer;
		QTreeWidgetItem *temporal_correlationPointer;
		QTreeWidgetItem *parameter_correlationPointer;
		QTreeWidgetItem *correlation_directionPointer;
		QTreeWidgetItem *prior_model_facies_probabilitiesPointer;
			QTreeWidgetItem *facies_probabilities_use_vsPointer;
			QTreeWidgetItem *facies_probabilities_use_predictionPointer;
			QTreeWidgetItem *facies_probabilities_use_absolute_elastic_parametersPointer;
			QTreeWidgetItem *facies_probabilities_estimation_intervalPointer;
				QTreeWidgetItem *facies_probabilities_top_surface_filePointer;
				QTreeWidgetItem *facies_probabilities_base_surface_filePointer;
			QTreeWidgetItem *prior_probabilitesPointer;
			QTreeWidgetItem *uncertainty_levelPointer;


	//project-settings
	QTreeWidgetItem *project_settingsPointer;
		//project-settings/output-volume
		QTreeWidgetItem *output_volumePointer;
			QTreeWidgetItem *interval_two_surfacesPointer;
				QTreeWidgetItem *interval_two_surfaces_top_surfacePointer;
					QTreeWidgetItem *top_surface_time_filePointer;
					QTreeWidgetItem *top_surface_time_valuePointer;
					QTreeWidgetItem *top_surface_depth_filePointer;
				QTreeWidgetItem *interval_two_surfaces_base_surfacePointer;
					QTreeWidgetItem *base_surface_time_filePointer;
					QTreeWidgetItem *base_surface_time_valuePointer;
					QTreeWidgetItem *base_surface_depth_filePointer;
				QTreeWidgetItem *interval_two_surfaces_number_of_layersPointer;
				QTreeWidgetItem *interval_two_surfaces_velocity_fieldPointer;
				QTreeWidgetItem *interval_two_surfaces_velocity_field_from_inversionPointer;
			QTreeWidgetItem *interval_one_surfacePointer;
				QTreeWidgetItem *interval_one_surface_reference_surfacePointer;
				QTreeWidgetItem *interval_one_surface_shift_to_interval_topPointer;
				QTreeWidgetItem *interval_one_surface_thicknessPointer;
				QTreeWidgetItem *interval_one_surface_sample_densityPointer;
			QTreeWidgetItem *area_from_surfacePointer;
				QTreeWidgetItem *area_from_surface_file_namePointer;
				QTreeWidgetItem *area_from_surface_snap_to_seismic_dataPointer;
			QTreeWidgetItem *utm_coordinatesPointer;
				QTreeWidgetItem *utm_coordinates_reference_point_xPointer;
				QTreeWidgetItem *utm_coordinates_reference_point_yPointer;
				QTreeWidgetItem *utm_coordinates_length_xPointer;
				QTreeWidgetItem *utm_coordinates_length_yPointer;
				QTreeWidgetItem *utm_coordinates_sample_density_xPointer;
				QTreeWidgetItem *utm_coordinates_sample_density_yPointer;
				QTreeWidgetItem *utm_coordinates_anglePointer;
			       	QTreeWidgetItem *utm_coordinates_snap_to_seismic_dataPointer;
			QTreeWidgetItem *inline_crossline_numbersPointer;
				QTreeWidgetItem *il_startPointer;
				QTreeWidgetItem *il_endPointer;
				QTreeWidgetItem *xl_startPointer;
				QTreeWidgetItem *xl_endPointer;
				QTreeWidgetItem *il_stepPointer;
				QTreeWidgetItem *xl_stepPointer;
		//project-settings/time-3D-mapping
		QTreeWidgetItem *time_3D_mappingPointer;
			QTreeWidgetItem *time_3D_mapping_reference_depthPointer;
			QTreeWidgetItem *time_3D_mappingPointer_average_velocity;
			QTreeWidgetItem *time_3D_mappingPointer_reference_time_surface;
		//project-settings/io-settings
		QTreeWidgetItem *io_settingsPointer;
			QTreeWidgetItem *top_directoryPointer;
			QTreeWidgetItem *input_directoryPointer;
			QTreeWidgetItem *output_directoryPointer;
			QTreeWidgetItem *grid_outputPointer;
				QTreeWidgetItem *grid_output_domainPointer;
					QTreeWidgetItem *grid_output_depthPointer;
					QTreeWidgetItem *grid_output_timePointer;
				QTreeWidgetItem *grid_output_formatPointer;
					QTreeWidgetItem *segy_formatPointer;
						QTreeWidgetItem *segy_format_standard_formatPointer;
						QTreeWidgetItem *segy_format_location_xPointer;
						QTreeWidgetItem *segy_format_location_yPointer;
						QTreeWidgetItem *segy_format_location_ilPointer;
						QTreeWidgetItem *segy_format_location_xlPointer;
						QTreeWidgetItem *segy_format_bypass_coordinate_scalingPointer;
						QTreeWidgetItem *segy_format_location_scaling_coefficientPointer;
					QTreeWidgetItem *format_segyPointer;
					QTreeWidgetItem *format_stormPointer;
					QTreeWidgetItem *format_cravaPointer;
					QTreeWidgetItem *format_sgriPointer;
					QTreeWidgetItem *format_asciiPointer;
				QTreeWidgetItem *elastic_parametersPointer;
					QTreeWidgetItem *elastic_parameters_vpPointer;
					QTreeWidgetItem *elastic_parameters_vsPointer;
					QTreeWidgetItem *elastic_parameters_densityPointer;
					QTreeWidgetItem *elastic_parameters_lame_lambdaPointer;
					QTreeWidgetItem *elastic_parameters_lame_muPointer;
					QTreeWidgetItem *elastic_parameters_poisson_ratioPointer;
					QTreeWidgetItem *elastic_parameters_aiPointer;
					QTreeWidgetItem *elastic_parameters_siPointer;
					QTreeWidgetItem *elastic_parameters_vp_vs_ratioPointer;
					QTreeWidgetItem *elastic_parameters_murhoPointer;
					QTreeWidgetItem *elastic_parameters_lambdarhoPointer;
					QTreeWidgetItem *elastic_parameters_backgroundPointer;
					QTreeWidgetItem *elastic_parameters_background_trendPointer;
				QTreeWidgetItem *grid_output_seismic_dataPointer;
					QTreeWidgetItem *seismic_data_originalPointer;
					QTreeWidgetItem *seismic_data_syntheticPointer;
					QTreeWidgetItem *seismic_data_residualsPointer;
					QTreeWidgetItem *seismic_data_synthetic_residualsPointer;
				QTreeWidgetItem *grid_output_other_parametersPointer;
					QTreeWidgetItem *grid_output_facies_probabilitiesPointer;
					QTreeWidgetItem *grid_output_facies_probabilities_with_undefPointer;
			       		QTreeWidgetItem *grid_output_facies_likelihoodPointer;
					QTreeWidgetItem *grid_output_time_to_depth_velocityPointer;
					QTreeWidgetItem *grid_output_extra_gridsPointer;
					QTreeWidgetItem *grid_output_correlationsPointer;
					QTreeWidgetItem *grid_output_seismic_quality_gridPointer;
			QTreeWidgetItem *well_outputPointer;
				QTreeWidgetItem *well_output_formatPointer;
					QTreeWidgetItem *well_output_rmsPointer;
					QTreeWidgetItem *well_output_norsarPointer;
				QTreeWidgetItem *well_output_wellsPointer;
				QTreeWidgetItem *well_output_blocked_wellsPointer;
				QTreeWidgetItem *well_output_blocked_logsPointer;
			QTreeWidgetItem *wavelet_outputPointer;
				QTreeWidgetItem *wavelet_output_formatPointer;
					QTreeWidgetItem *wavelet_output_jasonPointer;
					QTreeWidgetItem *wavelet_output_norsarPointer;
				QTreeWidgetItem *wavelet_output_well_waveletsPointer;
				QTreeWidgetItem *wavelet_output_global_waveletsPointer;
				QTreeWidgetItem *wavelet_output_local_waveletsPointer;
			QTreeWidgetItem *io_settings_other_outputPointer;
				QTreeWidgetItem *io_settings_extra_surfacesPointer;
				QTreeWidgetItem *io_settings_prior_correlationsPointer;
				QTreeWidgetItem *io_settings_background_trend_1dPointer;
				QTreeWidgetItem *io_settings_local_noisePointer;
		       		QTreeWidgetItem *io_settings_rock_physics_distributionsPointer;
			QTreeWidgetItem *io_settings_file_output_prefixPointer;
			QTreeWidgetItem *io_settings_log_levelPointer;
		//project-settings/advanced-settings
		QTreeWidgetItem *advanced_settingsPointer;
			QTreeWidgetItem *fft_grid_paddingPointer;
				QTreeWidgetItem *x_fractionPointer;
				QTreeWidgetItem *y_fractionPointer;
				QTreeWidgetItem *z_fractionPointer;
			QTreeWidgetItem *use_intermediate_disk_storagePointer;
		       	QTreeWidgetItem *vp_vs_ratioPointer;
			QTreeWidgetItem *vp_vs_ratio_from_wellsPointer;
			QTreeWidgetItem *maximum_relative_thickness_differencePointer;
			QTreeWidgetItem *frequency_bandPointer;
				QTreeWidgetItem *frequency_band_low_cutPointer;
				QTreeWidgetItem *frequency_band_high_cutPointer;
			QTreeWidgetItem *energy_tresholdPointer;
			QTreeWidgetItem *wavelet_tapering_lengthPointer;
			QTreeWidgetItem *minimum_relative_wavelet_amplitudePointer;
			QTreeWidgetItem *maximum_wavelet_shiftPointer;
			QTreeWidgetItem *white_noise_component_cutPointer;
			QTreeWidgetItem *reflection_matrixPointer;
			QTreeWidgetItem *kriging_data_limitPointer;
			QTreeWidgetItem *debug_levelPointer;
			QTreeWidgetItem *smooth_kriged_parametersPointer;
		       	QTreeWidgetItem *guard_zonePointer;

//all the slots starting with on_ are autoconnected by moc.
private slots:
	void on_wellHeaderPushButton_clicked();//autoslot for the button that calls getWellHeaders with correct name
	void on_xmlTreeWidget_itemChanged();//makes prompt on save work, though this seems to trigger too often.
	void on_aboutAction_triggered();//opens a message box with information about the program, all information is within the slot
	void on_manualAction_triggered();//opens up the manual with the desktopmanager. currently given with relative path, though that means it can only be opened if the binary is not moved around too much...
	void on_wikiAction_triggered();//makes the desktopmanager open the url to the wiki, needs statoil access to open though...
	void on_runAction_triggered();//starts CRAVA in a terminal with the last saved filename. Does not currently save before it is done so might produce unexpected results if called directly.
	void on_modeAction_triggered();//opens the mode(true) dialog with the started parameter set properly
	void on_settingsAction_triggered();//opens the settings dialog.
	bool on_saveAction_triggered();//saves the current file, if it is empty calls saveAs use saveFile(QString)
	bool on_saveAsAction_triggered();//open savefiledialog and saves the xml file to that file. Use saveFile(QString)
	void on_newAction_triggered();//starts a clear run in a new window
	void on_openAction_triggered();//opens an existing xml file in a new window
	//survey information:
	void on_stackListWidget_currentRowChanged ( int currentRow );//makes sure the frame for the stack is updated correctly according to which stack is selected.
	void on_addStackPushButton_clicked();//adds a new seismic stack
	void on_deleteStackPushButton_clicked();//removes the selected stack, should be undoable
	void on_angleLineEdit_editingFinished();//update the offset angle for the selected stack in the XML file
	void on_applyToAllStacksPushButton_clicked(); //gives the other stacks the same settings
	//PP - PS buttons 
	void on_ppRadioButton_toggled(bool checked);//makes sure the tree is updated with the correct ps/pp values for the currently selected stack
	//void on_psRadioButton_toggled(bool checked); //since a bool is passed in only one slot is needed for both buttons
	void on_defaultStartTimeLineEdit_editingFinished();//update the start time for the selected stack in the XML file
	//header format information buttons
	void on_headerAutoDetectRadioButton_toggled(bool checked);//calls headerFormat(const QString & value) with the correct value
	void on_headerSeisWorksRadioButton_toggled(bool checked);//calls headerFormat(const QString & value) with the correct value
	void on_headerIesxRadioButton_toggled(bool checked);//calls headerFormat(const QString & value) with the correct value
	void on_headerSipRadioButton_toggled(bool checked);//calls headerFormat(const QString & value) with the correct value
	void on_headerCharismaRadioButton_toggled(bool checked);//calls headerFormat(const QString & value) with the correct value
	void on_headerUserDefinedRadioButton_toggled(bool checked);//calls headerFormat(const QString & value) with the correct value and displays the frame for making your own header
	void on_xCoordLineEdit_editingFinished();//update the XML tree with the new x coordinate format for the header
	void on_yCoordLineEdit_editingFinished();//update the XML tree with the new y coordinate format for the header
	void on_crosslineLineEdit_editingFinished();//update the XML tree with the new crossline format for the header
	void on_inlineLineEdit_editingFinished();//update the XML file tree the new inline format for the header
	//buttons bypass
	void on_bypassCoordinateYesRadioButton_toggled(bool checked);//calls bypassCoordinate(const QString & value) with the correct value
	void on_bypassCoordinateNoRadioButton_toggled(bool checked);//calls bypassCoordinate(const QString & value) with the correct value
	void on_bypassCoordinateEstimateRadioButton_toggled(bool checked);//calls bypassCoordinate(const QString & value) with the correct value
	void on_locationScalingLineEdit_editingFinished();//update the XML tree with the location scaling coefficient, integer
	//buttons global wavelet
	void on_estimateWaveCheckBox_toggled(bool checked);//updates the tree with the correct value for this stack and hides/show the frame as needed.
	//need to implement 3D wavelet
	void on_waveletFileLineEdit_editingFinished();//update the XML tree with the file if it is correct, wavelet file for the seieismic stack
	void on_waveletBrowsePushButton_clicked();//browse for the wavelet file then update the XML tree, update the field
	void on_rickerLineEdit_editingFinished();//update the XML tree with the peak frecuency for the ricker wavelet.
	void on_waveletFileRadioButton_toggled(bool checked);//updates the XML tree by deleting the unchecked values
	//buttons wavelet scale
	void on_fileScaleRadioButton_toggled(bool checked);//shows the correct input line, updates the tree
	void on_estimateScaleRadioButton_toggled(bool checked);//updates the tree with the correct value, clears the others
	void on_manualScaleRadioButton_toggled(bool checked);//updates the tree with the correct value, clears the others
	void on_scaleLineEdit_editingFinished();//update the scale for the given wavelet for the selected stack in the XML tree
	void on_localWaveletCheckBox_toggled(bool checked);//update whether local wavelet should be estimated in the XML tree, also disable/enable adding files
	void on_shiftLocalWaveletCheckBox_toggled(bool checked);//updates tree and show/hide input of file if possible.
	void on_scaleLocalWaveletCheckBox_toggled(bool checked);//updates tree and show/hide input of file if possible.
	void on_shiftFileLineEdit_editingFinished();//update the XML tree with the file if it is correct, local wavelet shift file for the seieismic stack
	void on_shiftFileBrowsePushButton_clicked();//browse for the local wavelet shift file then update the XML tree, update the field
	void on_scaleFileLineEdit_editingFinished();//update the XML tree with the file if it is correct, local wavelet scale file for the seieismic stack
	void on_scaleFileBrowsePushButton_clicked();//browse for the local wavelet scale file then update the XML tree, update the field
	//signal to noise ratio buttons
	void on_signalToNoiseCheckBox_toggled(bool checked);//shows/hides the line for filling in the signal to noise ratio as appropriate.
	void on_signalToNoiseLineEdit_editingFinished();//update the XML tree with the signal to noise ratio
	void on_localNoiseCheckBox_toggled(bool checked);//update the XML tree with whether local noise should be enabled and 
	//local noise radio buttons
	void on_localNoiseEstimateCheckBox_toggled(bool checked);//shows / hides the possiblity of inputting a file, also updates the tree with whether to estimate or not.
	void on_localNoiseFileLineEdit_editingFinished();//update the XML tree with the file if it is correct, local noise file for the seieismic stack
	void on_localNoiseBrowsePushButton_clicked();//browse for the local noise file then update the XML file, update the field
	void on_matchEnergiesCheckBox_toggled(bool checked);//update whether energies should be matched in XML file for the selected stack
	//non-stack survey
	void on_startTimeLineEdit_editingFinished();//update the XML tree with the new start time
	void on_angularCorrelationPushButton_clicked();//open variogram edit window
	//wells, format info
	void on_timeLineEdit_editingFinished();//update the XML three with the new time format
	void on_densityLineEdit_editingFinished();//update the XML tree with the new density format
	void on_faciesLineEdit_editingFinished();//update the XML tree with the new facis format
	//button vp
	void on_vpRadioButton_toggled(bool checked);//handles which of vp/dt are shown and make sure the tree can't be wrong
	void on_vpLineEdit_editingFinished();//update the XML tree with the new vp format, remove dt
	//button dt
	void on_dtLineEdit_editingFinished();//update the XML tree with the new dt format, remove vp
	//button vs
	void on_vsRadioButton_toggled(bool checked);//handles which of vs/dts are shown and make sure the tree can't be wrong
	void on_vsLineEdit_editingFinished();//update the XML tree with the new vs format, remove dts
	//button dts
	void on_dtsLineEdit_editingFinished();//update the XML tree with the new dts format, remove vs
	//well input
	void on_wellListWidget_currentRowChanged ( int currentRow );//makes sure the proper widgets are enabled and that the correct information is displayed regarding to the selected well.
	void on_addWellPushButton_clicked();//add a new well for input
	void on_deleteWellPushButton_clicked();//remove the selected well
        void on_openWellPushButton_clicked();//opens the selected well in a text editor
	void on_waveletEstimationCheckBox_toggled(bool checked);//update whether this well should be used for wavelet estimationin XML tree
	void on_backgroundTrendCheckBox_toggled(bool checked);//update whether this well should be used for background trend estimationin XML tree
	void on_faciesProbabilitiesCheckBox_toggled(bool checked);//update whether this well should be used for facies estimationin XML tree
	void on_filterElasticCheckBox_toggled(bool checked);//update whether this well should multi-parameter-filter the elastic logs after inversion in XML tree
	/*	void on_convertLasToRmsPushButton_clicked(); */
	//syntetic vs log buttons
	void on_synteticVsYesRadioButton_toggled(bool checked);//calls synteticVsLog(const QString & value) with correct value
	void on_synteticVsNoRadioButton_toggled(bool checked);//calls synteticVsLog(const QString & value) with correct value
	void on_synteticVsDetectRadioButton_toggled(bool checked);//calls synteticVsLog(const QString & value) with correct value
	void on_optimizePositionCheckBox_toggled(bool checked);//makes it possible to optimize positions for a given well
	void on_optimizePositionListWidget_currentRowChanged ( int currentRow );//makes sure the correct info is displayed for the selected optimization and the correct widgets enabled/disabled.
	void on_optimizePositionPushButton_clicked();//adds a position optimization
	void on_deleteOptimizationPushButton_clicked();//removes the selected position optimization, should be undoable
	void on_anglePositionlineEdit_editingFinished();//updates the angle for the optimized position in XML
	void on_weightLineEdit_editingFinished();//updates the weight of the optimized position in XML
	//well parameter info
	//horizon
	void on_twoSurfaceRadioButton_toggled(bool checked);//makes sure the proper frame is shown for this way of setting the volume
	void on_topSurfaceRadioButton_toggled(bool checked);//makes sure the proper frame is shown for this way of setting the volume
	void on_baseSurfaceRadioButton_toggled(bool checked);//makes sure the proper frame is shown for this way of setting the volume
	void on_oneSurfaceRadioButton_toggled(bool checked);//makes sure the proper frame is shown for this way of setting the volume
	void on_constantInversionRadioButton_toggled(bool checked);//makes sure the proper frame is shown for this way of setting the volume
	void on_correlationSurfaceRadioButton_toggled(bool checked);//makes sure the proper frame is shown for this way of setting the volume
	void on_depthSurfacesCheckBox_toggled(bool checked);//shows/hides the depth input as apropriate.
	//two surface case:
	//button time file top
	//void on_topTimeFileRadioButton_toggled(bool checked); should not be decided here, but by the top level radio buttons
	void on_topTimeFileLineEdit_editingFinished();//update the XML tree with the file if it is correct, top time file
	void on_topTimeFileBrowsePushButton_clicked();//browse for the top time file then update the XML tree if the above is not triggered, update the field
	//button time value top
	void on_topTimeValueLineEdit_editingFinished();//update the XML tree with the new top time value
	void on_topDepthFileLineEdit_editingFinished();//update the XML tree with the file if it is correct, top depth file
	void on_topDepthFileBrowsePushButton_clicked();//browse for the top time file then update the XML tree if the above is not triggered, update the field
	//button time file bottom
	//void on_bottomTimeFileRadioButton_toggled(bool checked); should not be decided here, but by the top level radio buttons
	void on_bottomTimeFileLineEdit_editingFinished();//update the XML tree with the file if it is correct, bottom time file
	void on_bottomTimeFileBrowsePushButton_clicked();//browse for the top time file then update the XML tree if the above is not triggered, update the field
	//button time value bottom
	void on_bottomTimeValueLineEdit_editingFinished();//update the XML tree with the new bottom time value
	void on_bottomDepthFileLineEdit_editingFinished();//update the XML tree with the file if it is correct, top depth file
	void on_bottomDepthFileBrowsePushButton_clicked();//browse for the bottom time file then update the XML tree if the above is not triggered, update the field
	void on_layersLineEdit_editingFinished();//update the XML for the number of layers
	//buttons for velocity field
	void on_velocityFieldInvesionRadioButton_toggled(bool checked);
	void on_velocityFieldFileRadioButton_toggled(bool checked);
	void on_velocityFieldLineEdit_editingFinished();//update the XML tree with the file if it is correct, velocity field file
	void on_velocityFieldBrowsePushButton_clicked();//browse for the velocity field file then update the XML tree if the above is not triggered, update the field
	//one surface case
	void on_referenceSurfaceFileLineEdit_editingFinished();//update the XML tree with the file if it is correct, reference surface file
	void on_referenceSurfaceBrowsePushButton_clicked();//browse for the reference surface file then update the XML tree if the above is not triggered, update the field
	void on_distanceTopLineEdit_editingFinished();//update the XML tree with the distance to the top layer from the referance surface
	void on_thicknessLineEdit_editingFinished();//update the XML tree with the thickness for the volume of the entire inversion
	void on_layerThicknessLineEdit_editingFinished();//update the XML tree with the thickness for each layer
	//other horizons
	void on_waveletTopLineEdit_editingFinished();//update the XML tree with the file if it is correct, wavelet top file
	void on_waveletTopBrowsePushButton_clicked();//browse for the wavelet top time file then update the XML tree if the above is not triggered, update the field
	void on_waveletBottomLineEdit_editingFinished();//update the XML tree with the file if it is correct, autocomplete would be nice, wavelet bottom time file
	void on_waveletBottomBrowsePushButton_clicked();//browse for the wavelet bottom time file then update the XML tree if the above is not triggered, update the field
	void on_faciesTopLineEdit_editingFinished();//update the XML tree with the file if it is correct, autocomplete would be nice, facies top file
	void on_faciesTopBrowsePushButton_clicked();//browse for the facies top time file then update the XML tree if the above is not triggered, update the field
	void on_faciesBottomLineEdit_editingFinished();//update the XML tree with the file if it is correct, facies bottom time file
	void on_faciesBottomBrowsePushButton_clicked();//browse for the facies bottom time file then update the XML tree if the above is not triggered, update the field
	//prior model
	void on_backgroundRadioButton_toggled(bool checked);//determines whether the regular background model should be given or not.
	void on_estimateBackgroundRadioButton_toggled(bool checked);//determines whether the beckground model should be estimated or not.
	void on_multizoneBackgroundRadioButton_toggled(bool checked);//determines whether the multizone background model should be given or not.
	void on_vpVsRhoRadioButton_toggled(bool checked);//determines whether it is the combination Vp,Vs and density that should be given as parameters.
	void on_aiVpVsRhoRadioButton_toggled(bool checked);//determines whether it is the combination AI,Vp/Vs and density that should be given as parameters.
	void on_aiSiRhoRadioButton_toggled(bool checked);//determines whether it is the combination AI,SI and density that should be given as parameters.

	         //buttons for vp/vs/density/ai/si/vp-vs
	void on_vpFile1RadioButton_toggled(bool checked);//gives whether vp should be given from a cube or constant in vpVsRhoFrame. displays proper widgets for each case
	void on_vsFile1RadioButton_toggled(bool checked);//gives whether vs should be given from a cube or constant in vpVsRhoFrame. displays proper widgets for each case
	void on_densityFile1RadioButton_toggled(bool checked);//gives whether density should be given from a cube or constant in vpVsRhoFrame. displays proper widgets for each case
	void on_densityFile2RadioButton_toggled(bool checked);//gives whether density should be given from a cube or constant in aiVpVsRhoFrame. displays proper widgets for each case
	void on_densityFile3RadioButton_toggled(bool checked);//gives whether density should be given from a cube or constant in aiSiRhoFrame. displays proper widgets for each case
        	//vpVsRho
	void on_vpConstant1LineEdit_editingFinished();//update the XML tree with constant vp for the background model
	void on_vpFile1LineEdit_editingFinished();//update the XML tree with the file if it is correct, prior model vp file
	void on_vpFile1BrowsePushButton_clicked();//browse for the prior model vp file then update the XML tree if the above is not triggered, update the field
	void on_vsConstant1LineEdit_editingFinished();//update the XML tree with constant vs for the background model
	void on_vsFile1LineEdit_editingFinished();//update the XML tree with the file if it is correct, prior model vs file
	void on_vsFile1BrowsePushButton_clicked();//browse for the prior model vs file then update the XML tree if the above is not triggered, update the field
	void on_densityConstant1LineEdit_editingFinished();//update the XML tree with constant density for the background model
	void on_densityFile1LineEdit_editingFinished();//update the XML tree with the file if it is correct, prior model density file
	void on_densityFile1BrowsePushButton_clicked();//browse for the prior model density file then update the XML tree if the above is not triggered, update the field
        	//aiVpVsRho
 	void on_aiFile2LineEdit_editingFinished();//update the XML tree with the file if it is correct, prior model ai file
	void on_aiFile2BrowsePushButton_clicked();//browse for the prior model ai file and update the XML tree if the above is not triggered.
	void on_vpVsFile2LineEdit_editingFinished();//update the XML tree with the file if it is correct, prior model Vp/Vs file
	void on_vpVsFile2BrowsePushButton_clicked();//browse for the prior model Vp/Vs file and update the XML tree if the above is not triggered.
	void on_densityConstant2LineEdit_editingFinished();//update the XML tree with constant density for the background model
	void on_densityFile2LineEdit_editingFinished();//update the XML tree with the file if it is correct, prior model density file
	void on_densityFile2BrowsePushButton_clicked();//browse for the prior model density file then update the XML tree if the above is not triggered, update the field
         	//aiSiRho
	void on_aiFile3LineEdit_editingFinished();//update the XML tree with the file if it is correct, prior model ai file
	void on_aiFile3BrowsePushButton_clicked();//browse for the prior model ai file and update the XML tree if the above is not triggered.
	void on_siFile3LineEdit_editingFinished();//update the XML tree with the file if it is correct, prior model SI file
	void on_siFile3BrowsePushButton_clicked();//browse for the prior model SI file and update the XML tree if the above is not triggered.
	void on_densityConstant3LineEdit_editingFinished();//update the XML tree with constant density for the background model
	void on_densityFile3LineEdit_editingFinished();//update the XML tree with the file if it is correct, prior model density file
	void on_densityFile3BrowsePushButton_clicked();//browse for the prior model density file then update the XML tree if the above is not triggered, update the field

	void on_velocityFieldPriorFileLineEdit_editingFinished();//update the XML tree with the file if it is correct, prior model velocity field
	void on_backgroundEstimatedConfigurationCheckBox_toggled(bool checked);//shows the proper widgets for inputting background estimation info, does not modify the tree in any way.
	void on_velocityFieldPriorFileBrowsePushButton_clicked();//browse for the prior model velocity field file then update the XML tree and the above field
	void on_lateralCorrelationBackgroundPushButton_clicked();//pop up the variogram edit window for background
	void on_highCutFrequencyLineEdit_editingFinished();//update the XML tree with the high cut frequency
       	void on_topSurfaceFileLineEdit_editingFinished();//update the XML tree with top surface file
       	void on_topSurfaceFileBrowsePushButton_clicked();//browse for the top surface file, then update the XML tree and the above field.
	void on_topPrioritySpinBox_editingFinished();//update the XML tree with the erosion priority
	void on_addZonePushButton_clicked();//adds a new zone for multizone background model
	void on_insertZonePushButton_clicked();//inserts a new zone for multizone background model
	void on_deleteZonePushButton_clicked();//remove the selected zone, should be undoable
	void on_zoneListWidget_currentRowChanged(int currentRow);//makes sure the correct widgets and information is displayed depending on which zone is selected
	void on_baseSurfaceFileLineEdit_editingFinished();//updates the XML with the base surface file
	void on_baseSurfaceFileBrowsePushButton_clicked();//browse for the base surface file, then update the XML tree and the above field.
	void on_basePrioritySpinBox_editingFinished();//updates the XML with the base surface file
	void on_topCorrelationRadioButton_toggled(bool checked);//updates the XML with "top" at correlation structure
	void on_baseCorrelationRadioButton_toggled(bool checked);//updates the XML with "base" at correlation structure
	void on_compactionCorrelationRadioButton_toggled(bool checked);//updates the XML with "compaction" at correlation structure
	void on_surfaceUncertaintyLineEdit_editingFinished();//updates the XML with the surface uncertainty
	void on_correlationLocalWaveletCheckBox_toggled(bool checked);//shows the proper widgets for inputting correlation for wavelet info, does not modify the tree in any way.
	void on_lateralCorrelationWaveletPushButton_clicked();//pop up the variogram edit window for wavelet
	void on_correlationElasticParametersCheckBox_toggled(bool checked);//shows the proper widgets for inputting correlation of parameters info, does not modify the tree in any way.
	void on_lateralCorrelationParametersPushButton_clicked();//pop up the variogram edit window for parameters
	void on_temporalCorrelationLineEdit_editingFinished();//update the XML tree with the file if it is correct, prior model temporal corr
	void on_temporalCorrelationBrowsePushButton_clicked();//browse for prior model temporal correlation file then update the XML tree and the above field
	void on_parameterCorrelationLineEdit_editingFinished();//update the XML tree with the file if it is correct, prior model parameter corr
	void on_parameterCorrelationBrowsePushButton_clicked();//browse for prior model parameter corr file then update the XML tree and the above field
	void on_correlationDirectionFileLineEdit_editingFinished();//update the XML tree with the file if it is correct, prior model corr direction
	void on_correlationDirectionBrowsePushButton_clicked();//browse for prior model corr direction file then update the XML tree and the above field
	void on_faciesEstimateCheckBox_toggled(bool checked);//should the prior model for facies be estimated or given
	void on_vsForFaciesCheckBox_toggled(bool checked);//should vs be used for facies estimaton, update XML tree
	void on_predictionFaciesCheckBox_toggled(bool checked);//should sampled inversion logs be used instead of filtered logs for facies estimaton, update XML tree
	void on_absoluteParametersCheckBox_toggled(bool checked);//should absolute parameters be used instead of parameters minus trend for facies estimaton, update XML tree
	void on_faciesListWidget_currentRowChanged ( int currentRow );//makes sure the correct widgets and information is displayed depending on which facies is selected.
	void on_addFaciesPushButton_clicked();//adds a new facies for prior probabilities
	void on_deleteFaciesPushButton_clicked();//removes the selected facis, should be undoable
	void on_faciesNameLineEdit_editingFinished();//changes the name of the facies, both displayed and in XML tree
	//probability radio buttons
	void on_probabilityConstantRadioButton_toggled(bool checked);//makes sure that the correct widgets are displayed depending on what is selected from the radio buttons and that the info in the tree is correct.
	void on_probabilityConstantLineEdit_editingFinished();//update the XML tree with constant probability for the selected facies
	void on_probabilityCubeLineEdit_editingFinished();//update the XML tree with the file if it is correct, autocomplete would be nice, facis probability cube file
	void on_probabilityCubeBrowsePushButton_clicked();//browse for the facis probability cube file then update the XML tree, update the field
	void on_uncertaintyLevelLineEdit_editingFinished();//update the XML tree with the uncertainty level
	void on_writeXmlPushButton_clicked();//opens output dialog, then saves the file and finally runs.
	//inversion area
	void on_areaSeismicRadioButton_toggled(bool checked);//makes sure the correct frame is displayed for input of lateral inversion geometry and that the info is not wrong in tree.
	void on_areaFileRadioButton_toggled(bool checked);//makes sure the correct frame is displayed for input of lateral inversion geometry and that the info is not wrong in tree.
	void on_areaUtmRadioButton_toggled(bool checked);//makes sure the correct frame is displayed for input of lateral inversion geometry and that the info is not wrong in tree.
	void on_areaInCrossRadioButton_toggled(bool checked);//makes sure the correct frame is displayed for input of lateral inversion geometry and that the info is not wrong in tree.
	void on_areaSurfaceLineEdit_editingFinished();//updates the tree via the filehandler for lateral inversion geometry
	void on_areaSurfaceFileBrowsePushButton_clicked();//updates the tree via the filehandler for lateral inversion geometry
       	void on_surfaceSnapCheckBox_toggled(bool checked);//updates the tree with the correct snap to seismic data value (yes or no)
	void on_areaXRefLineEdit_editingFinished();//updates the tree with the correct utm value reference coordinate x
	void on_areaYRefLineEdit_editingFinished();//updates the tree with the correct utm value reference coordinate y
	void on_areaXLengthLineEdit_editingFinished();//updates the tree with the correct utm value length x
	void on_areaYLengthLineEdit_editingFinished();//updates the tree with the correct utm value length y
	void on_areaXSampleDensityLineEdit_editingFinished();//updates the tree with the correct utm value sample density x
	void on_areaYSampleDensityLineEdit_editingFinished();//updates the tree with the correct utm value sample density y
	void on_areaUtmAngleLineEdit_editingFinished();//updates the tree with the correct utm value angle
	void on_utmSnapCheckBox_toggled(bool checked);//updates the tree with the correct snap to seismic data value (yes or no)
	void on_inlineStartLineEdit_editingFinished();//updates the tree with the correct inline value start
	void on_crosslineStartLineEdit_editingFinished();//updates the tree with the correct crossline value start
	void on_inlineEndLineEdit_editingFinished();//updates the tree with the correct inline value end
	void on_crosslineEndLineEdit_editingFinished();//updates the tree with the correct crossline value end
	void on_inlineStepLineEdit_editingFinished();//updates the tree with the correct inline value step
	void on_crosslineStepLineEdit_editingFinished();//updates the tree with the correct crossline value step
	void on_earthVpAiLineEdit_editingFinished();//updates the tree for earth model Vp or AI (depending on vpComboBox)
	void on_earthVsSiVpVsLineEdit_editingFinished();//updates the tree for earth model Vs, SI or Vp/Vs ratio (depending on vsComboBox)
	void on_earthDensityLineEdit_editingFinished();//updates the tree for earth model density
	void on_earthVpAiBrowsePushButton_clicked();//updates the tree via the filehandler for earth model Vp or AI (depending on vpComboBox)
	void on_earthVsSiVpVsBrowsePushButton_clicked();//updates the tree via the filehandler for earth model Vs, SI or Vp/Vs ratio (depending on vsComboBox)
	void on_earthDensityBrowsePushButton_clicked();//updates the tree via the filehandler for earth model density.
	void on_vpComboBox_currentIndexChanged(int index);//updates the tree by removing values that should not be there when a particular index is chosen.
	void on_vsComboBox_currentIndexChanged(int index);//updates the tree by removing values that should not be there when a particular index is chosen.
	void on_oOutputDirectoryBrowsePushButton_clicked();//updates the tree by setting output directory via filehandler.
	void on_oOutputDirectoryLineEdit_editingFinished();//updates the tree with the correct output directory.
	void on_oPrefixLineEdit_editingFinished();//updates the tree with the correct prefix for output files.
	void on_oDomainDepthCheckBox_toggled(bool checked);//updates the tree - should seismic output in depth domain be generated?
	void on_oDomainTimeCheckBox_toggled(bool checked);//updates the tree - should seismic output in time domain be generated?
	void on_oSeismicOriginalCheckBox_toggled(bool checked);//updates the tree - should the original seismic cube be a part of the output?
	void on_oSeismicSyntheticCheckBox_toggled(bool checked);//updates the tree - should a synthetic cube be generated in the output?
	void on_oSeismicResidualCheckBox_toggled(bool checked);//updates the tree - should a residual seismic cube be generated in the output?
	void on_oSeismicSyntheticResidualCheckBox_toggled(bool checked);//updates the tree - should a synthetic residual seismic cube be generated in the output?
	void on_oVpCheckBox_toggled(bool checked);//updates the tree - should the velocity of pressure waves be a part of the output?
	void on_oVsCheckBox_toggled(bool checked);//updates the tree - should the velocity of shear waves be a part of the output?
	void on_oDensityCheckBox_toggled(bool checked);//updates the tree - should the density be a part of the output?
	void on_oAiCheckBox_toggled(bool checked);//updates the tree - should the AI be a part of the output?
	void on_oSiCheckBox_toggled(bool checked);//updates the tree - should SI be a part of the output?
	void on_oPoissonRatioCheckBox_toggled(bool checked);//updates the tree - should Poisson ratio be a part of the output?
	void on_oVpVsRatioCheckBox_toggled(bool checked);//updates the tree - should pressure wave velocity over shear wave velocity ratio be a part of the output?
	void on_oLameLamCheckBox_toggled(bool checked);//updates the tree - should lambda (Lame's parameter) be a part of the output?
	void on_oLameMuCheckBox_toggled(bool checked);//updates the tree - should mu (Lame's parameter) be a part of the output?
	void on_oLambdaRhoCheckBox_toggled(bool checked);//updates the tree - should lambda*rho be a part of the output?
	void on_oMuRhoCheckBox_toggled(bool checked);//updates the tree - should mu*rho be a part of the output?
	void on_oBackgroundCheckBox_toggled(bool checked);//updates the tree - should a background be a part of the output?
	void on_oBackgroundTrendCheckBox_toggled(bool checked);//updates the tree - should a background trend be a part of the output?
	void on_oExtraGridsCheckBox_toggled(bool checked);//updates the tree - should estimated background files (extra grids) be a part of the output?
	void on_oBackgroundTrend1DCheckBox_toggled(bool checked);//updates the tree - should a background trend as a 1D curve be a part of the output?
	void on_oWellCheckBox_toggled(bool checked);//updates the tree - should the original sampled wells be a part of the output?
	void on_oBlockedWellCheckBox_toggled(bool checked);//updates the tree - should blocked wells be a part of the output?
	void on_oWaveletWellCheckBox_toggled(bool checked);//updates the tree - should well wavelet files be a part of the output?
	void on_oWaveletGlobalCheckBox_toggled(bool checked);//updates the tree - should global wavelet files be a part of the output?
	void on_oWaveletLocalCheckBox_toggled(bool checked);//updates the tree - should local wavelet files be a part of the output?
	void on_oTimeDepthCheckBox_toggled(bool checked);//updates the tree - should "time to depth velocity" be a part of the output?
	void on_oCorrelationsCheckBox_toggled(bool checked);//updates the tree - should posterior correlations be a part of the output?
	void on_oFaciesProbabilitiesCheckBox_toggled(bool checked);//updates the tree - should facies probabilities be a part of the output?
	void on_oFaciesProbabilitiesUndefinedCheckBox_toggled(bool checked);//updates the tree - should facies probabilities with undefined facies be a part of the output?
     	void on_oFaciesLikelihoodCheckBox_toggled(bool checked);//updates the tree - should facies likelihood be a part of the output?
	void on_oPriorCorrelationCheckBox_toggled(bool checked);//updates the tree - should prior correlations be a part of the output?
	void on_oLocalNoiseCheckBox_toggled(bool checked);//updates the tree - should local noise be a part of the output?
	void on_oFaciesQualityGridCheckBox_toggled(bool checked);//updates the tree - should seismic quality grid be a part of the output?
	void on_oRockPhysicsCheckBox_toggled(bool checked);//updates the tree - should rock physics distributions of each facies be a part of the output?
	void showContextMenu(const QPoint& pos); //used to activate right clicking for wellHeaderListWidget
};

#endif
