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
#include <QtGui>
#include <QObject>
#include "VariogramDialog.h"
#include "StandardStrings.h"

/**
	@author Alf Birger Rustad (Statoil R&D) <abir@statoil.com>, Øystein Arneson (RD IRE FRM) <oyarn@statoil.com>
*/
VariogramDialog::VariogramDialog(QWidget *parent,bool genexp, QString angle, QString range, QString subrange, QString power) : QDialog(parent){
	setupUi( this );
	setWindowTitle(QString("%1[*] - %2").arg(windowTitle()).arg(StandardStrings::cravaVersion()));
	QButtonGroup *variogramDialogueButtons = new QButtonGroup();
	variogramDialogueButtons->addButton(genexpRadioButton);
	variogramDialogueButtons->addButton(sphericalRadioButton);
	genexpRadioButton->setChecked(true);
	angleLineEdit->setText(angle);
	rangeLineEdit->setText(range);
	normalRangeLineEdit->setText(subrange);
	powerLineEdit->setText(power);
}
QList<QString> VariogramDialog::variogramValues(){
	//variogram-type=0; angle=1; range=2; subrange=3; power=4
	QList<QString> values;
	if(sphericalRadioButton->isChecked()){
		values.append(QString("shperical"));
	}
	else{
		values.append(QString("genexp"));
	}
	values.append(angleLineEdit->text());
	values.append(rangeLineEdit->text());
	values.append(normalRangeLineEdit->text());
	values.append(powerLineEdit->text());
	return values;
}
void VariogramDialog::on_buttonBox_accepted() {
	//should check if the input is ok, if not pop up a warning message
	if(rangeLineEdit->text().isEmpty()){
		QMessageBox::warning(this, QString("Invalid input"), QString("Range must be specified."), QMessageBox::Ok);
		return;
	}
	if(sphericalRadioButton->isChecked()&&(!powerLineEdit->text().isEmpty())){
		QMessageBox::warning(this, QString("Invalid input"), QString("Spherical variogram can not have any power."), QMessageBox::Ok);
		return;
	}
	accept();
}
void VariogramDialog::on_genexpRadioButton_toggled(bool checked){
        powerLabel->setEnabled(true);
        powerLineEdit->setEnabled(true);

}
void VariogramDialog::on_sphericalRadioButton_toggled(bool checked){
        powerLineEdit->clear();
        powerLabel->setEnabled(false);
        powerLineEdit->setEnabled(false);
}
