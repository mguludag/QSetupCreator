/*****************************************************************************
 * dialog.cpp
 *
 * Created: 12/20/2020 2020 by mguludag
 *
 * Copyright 2020 mguludag. All rights reserved.
 *
 * This file may be distributed under the terms of GNU Public License version
 * 3 (GPL v3) as defined by the Free Software Foundation (FSF). A copy of the
 * license should have been included with this file, or the project in which
 * this file belongs to. You may also find the details of GPL v3 at:
 * http://www.gnu.org/licenses/gpl-3.0.txt
 *
 * If you have any questions regarding the use of this file, feel free to
 * contact the author of this file, or the owner of the project in which
 * this file belongs to.
 *****************************************************************************/
#include "dialog.hpp"
#include "ui_dialog.h"

Dialog::Dialog(QWidget *parent) : QDialog(parent), ui(new Ui::Dialog) {
  ui->setupUi(this);
  Settings::init(Settings::Format::iniFormat, "settings.ini");
  ui->lineEdit_qtbinpath->setText(
      Settings::readSettings("Qt_IFW_Bins", "Path").toString());
  ifw_path = ui->lineEdit_qtbinpath->text();
}

Dialog::~Dialog() { delete ui; }

void Dialog::on_buttonBox_accepted() {
  Settings::writeSettings("Qt_IFW_Bins", "Path",
                          ui->lineEdit_qtbinpath->text());
}

QString Dialog::getIfw_path() const { return ifw_path; }

void Dialog::on_toolButton_qtifw_clicked() {
  ui->lineEdit_qtbinpath->setText(QFileDialog::getExistingDirectory(
      this, tr("Open Qt Bin Directory"), QDir::rootPath(),
      QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks));
  ifw_path = ui->lineEdit_qtbinpath->text();
}
