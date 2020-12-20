/*****************************************************************************
 * %{Cpp:License:FileName}
 *
 * Created: 9/28/2020 2020 by mguludag
 *
 * Copyright 2020 mguludag. All rights reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
 * LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
 * OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
 * WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *****************************************************************************/
#ifndef MAINWINDOW_HPP
#define MAINWINDOW_HPP

#include "dialog.hpp"
#include <QDateTime>
#include <QDir>
#include <QDirIterator>
#include <QElapsedTimer>
#include <QFile>
#include <QFileDialog>
#include <QMainWindow>
#include <QProcess>
#include <QStringList>
#include <QTextStream>
#include <QThread>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
  Q_OBJECT

public:
  MainWindow(QWidget *parent = nullptr);
  ~MainWindow();

private slots:

  void on_toolButton_deploy_clicked();

  void on_pushButton_clicked();

  QString process_redist();

  void createProjDirs();

  void createConfigXML();

  void createInstallScript();

  void createPackageXML();

  void createArchive();

  void createInstaller();

  void createFiles();

  void on_pushButton_back_clicked();

  void on_pushButton_start_clicked();

  void on_toolButton_clicked();

  void on_toolButton_icon_clicked();

  void on_toolButton_setupbanner_clicked();

  void on_toolButton_setupicon_clicked();

  void on_actionSet_Qt_IFW_Path_triggered();

  void on_actionAbout_triggered();

  void on_actionLight_triggered();

  void on_actionDark_triggered();

  void disableWidgets(bool arg);

private:
  qint64 duration = 0;
  QDir currDir, projDir;
  QElapsedTimer elap_timer;
  QThread *thread;
  QProcess m_proc;
  QString depDirName, ifw_path, configXML, packageXML, installscriptqs;
  Ui::MainWindow *ui;
  Dialog d;
};
#endif // MAINWINDOW_HPP
