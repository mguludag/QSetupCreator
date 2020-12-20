/*****************************************************************************
 * dialog.hpp
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
#ifndef DIALOG_HPP
#define DIALOG_HPP

#include "settings.hpp"

#include <QDialog>
#include <QFileDialog>

namespace Ui {
class Dialog;
}

class Dialog : public QDialog {
  Q_OBJECT

public:
  explicit Dialog(QWidget *parent = nullptr);
  ~Dialog();

  QString getIfw_path() const;

private slots:
  void on_buttonBox_accepted();

  void on_toolButton_qtifw_clicked();

private:
  Ui::Dialog *ui;
  QString ifw_path;
};

#endif // DIALOG_HPP
