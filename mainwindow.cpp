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
#include "mainwindow.hpp"
#include "ui_mainwindow.h"
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow) {
  ui->setupUi(this);
  currDir = QDir::current();
  auto skin{Settings::loadStyle()};
  Settings::setStyle(skin);
  ui->actionLight->setChecked(skin == Settings::Theme::lightFusion);
  ui->actionDark->setChecked(skin == Settings::Theme::darkFusion);
  ui->label_status->setText("Ready...");
  ui->statusbar->addWidget(ui->label_status);
  ifw_path = d.getIfw_path();
  if (ifw_path.isEmpty()) {
    QMessageBox::warning(this, "Error getting Qt Installer Framework Path!",
                         "Please select Qt IFW bins path from \"File\" menu");
    disableWidgets(true);
  }
  ui->stackedWidget->setCurrentIndex(0);
}

MainWindow::~MainWindow() { delete ui; }

void MainWindow::on_toolButton_deploy_clicked() {
  ui->lineEdit_deploy->setText(QFileDialog::getExistingDirectory(
      this, tr("Open Deploy Directory"), currDir.currentPath(),
      QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks));
  QDir deployDir(ui->lineEdit_deploy->text());
  depDirName = deployDir.dirName();
}

void MainWindow::on_pushButton_clicked() {
  createConfigXML();
  createInstallScript();
  createPackageXML();
  ui->plainTextEdit_config->setPlainText(configXML);
  ui->plainTextEdit_installscript->setPlainText(installscriptqs);
  ui->plainTextEdit_package->setPlainText(packageXML);
  ui->stackedWidget->setCurrentIndex(1);
}

void MainWindow::on_toolButton_clicked() {
  QFileInfo targetexe(QFileDialog::getOpenFileName(
      this, tr("Open Deploy Directory"), ui->lineEdit_deploy->text(),
      tr("Executable (*.exe)")));
  ui->lineEdit_target->setText(targetexe.fileName());
}

QString MainWindow::process_redist() {
  QString result{};
  QString data = ui->textEdit->toPlainText();
  QString head =
      "    component.addElevatedOperation(\"Execute\", \"{0,3010,1638,5100}\", "
      "\"@TargetDir@/" +
      depDirName + "/";
  QString tail = ");\n";
  QStringList strList = data.split(QRegExp("[\n]"), QString::SkipEmptyParts);

  for (auto &line : strList) {
    QStringList argList = line.split(QRegExp("[,]"), QString::SkipEmptyParts);
    for (auto i = 0; i < argList.size(); ++i) {
      argList[i] = ((i == 0) ? "" : ",\"") + argList[i] + "\"";
    }
    QString temp{};
    for (auto &var : argList) {
      temp += var;
    }

    result = result + head + temp + tail;
  }
  return result + "};\n";
}

void MainWindow::createProjDirs() {
  if (!ui->lineEdit_deploy->text().isEmpty()) {
    projDir.setCurrent(QFileDialog::getExistingDirectory(
        this, tr("Open Project Directory"), currDir.currentPath(),
        QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks));
    projDir.mkdir(ui->lineEdit_appname->text());
    projDir.cd(ui->lineEdit_appname->text());
    projDir.mkpath("packages/data");
    projDir.mkpath("packages/meta");
  }
}

void MainWindow::createConfigXML() {
  QStringList target_dir;
  target_dir << "@RootDir@"
             << "@ApplicationsDir@";
  configXML =
      "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n\
    <Installer>\n\
        <Name>" +
      ui->lineEdit_appname->text() + "</Name>\n\
        <Version>" +
      ui->lineEdit_version->text() + "</Version>\n\
        <Title>" +
      ui->lineEdit_appname->text() + " Installer</Title>\n\
        <Publisher>" +
      ui->lineEdit_pub->text() + "</Publisher>\n\
        <WizardStyle>" +
      ui->comboBox_style->currentText() + "</WizardStyle>\n" +
      (!ui->lineEdit_banner->text().isEmpty()
           ? "            <Banner>" + ui->lineEdit_banner->text() +
                 "</Banner>\n"
           : "") +
      (!ui->lineEdit_setupicon->text().isEmpty()
           ? "            <InstallerWindowIcon>" +
                 ui->lineEdit_setupicon->text() + "</InstallerWindowIcon>\n\
            <InstallerApplicationIcon>" +
                 ui->lineEdit_setupicon->text() +
                 "</InstallerApplicationIcon>\n"
           : "") +
      "<!-- Directory name is used in component.xml -->\n\
        <StartMenuDir>" +
      ui->lineEdit_appname->text() + "</StartMenuDir>\n\
        <TargetDir>" +
      target_dir.at(ui->comboBox_dest->currentIndex()) + "/" +
      ui->lineEdit_appname->text() + "</TargetDir>\n\
    </Installer>\n";
}

void MainWindow::createInstallScript() {
  auto icon = ui->lineEdit_icon->text().isEmpty();
  installscriptqs =
      "function Component()\n\
{\n\
    // default constructor\n\
}\n\
\n\
Component.prototype.createOperationsForArchive = function(archive)\n\
{\n\
    console.log(\"performing custom extract operation\")\n\
    component.addOperation(\"Extract\", archive, \"@TargetDir@\" + \"/\" + installer.value(\"version\") + \"/\");\n\
}\n\
Component.prototype.createOperations = function()\n\
{\n\
    component.createOperations();\n\
    if (systemInfo.productType === \"windows\") {\n\
" +
      (ui->checkBox_startmenu->isChecked()
           ? +"\
        component.addOperation(\"CreateShortcut\", \"@TargetDir@/" +
                 depDirName + "/" + ui->lineEdit_target->text() +
                 "\", \"@StartMenuDir@/" + ui->lineEdit_nameshortcut->text() +
                 ".lnk\", \"workingDirectory=@TargetDir@\", "
                 "\"iconPath=@TargetDir@/" +
                 depDirName + "/" +
                 (icon ? ui->lineEdit_target->text()
                       : ui->lineEdit_icon->text()) +
                 "\"," + (icon ? "\"iconId=0\", " : " ") + "\"description=" +
                 ui->lineEdit_nameshortcut->text() + " Executable\");\n"
           : "") +
      (ui->checkBox_desktop->isChecked()
           ? +"        component.addOperation(\"CreateShortcut\", "
              "\"@TargetDir@/" +
                 depDirName + "/" + ui->lineEdit_target->text() +
                 "\", \"@DesktopDir@/" + ui->lineEdit_nameshortcut->text() +
                 ".lnk\", \"workingDirectory=@TargetDir@\", "
                 "\"iconPath=@TargetDir@/" +
                 depDirName + "/" +
                 (icon ? ui->lineEdit_target->text()
                       : ui->lineEdit_icon->text()) +
                 "\"," + (icon ? "\"iconId=0\", " : " ") + "\"description=" +
                 ui->lineEdit_nameshortcut->text() + " Executable\");\n"
           : "") +
      "    }\n" +
      (ui->textEdit->toPlainText().isEmpty() ? "};\n" : process_redist());
}

void MainWindow::createPackageXML() {

  packageXML = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n\
    <Package>\n\
        <DisplayName>" +
               ui->lineEdit_appname->text() + "</DisplayName>\n\
        <Description>This is " +
               ui->lineEdit_appname->text() + " installer</Description>\n\
        <Version>" +
               ui->lineEdit_version->text() + "</Version>\n\
        <ReleaseDate>" +
               QDateTime::currentDateTime().toString("yyyy-MM-dd") +
               "</ReleaseDate>\n\
        <Default>true</Default>\n\
        <Script>installscript.qs</Script>\n\
    </Package>\n";
}

void MainWindow::createArchive() {

  QStringList arg;
  arg << projDir.path() + "/packages/data/" + ui->lineEdit_appname->text() +
             ".7z"
      << ui->lineEdit_deploy->text();
  m_proc.start(ifw_path + "/archivegen.exe", arg);
  m_proc.waitForFinished(-1);
}

void MainWindow::createInstaller() {
  QStringList arg;
  arg << "-p" << projDir.path() << "-c" << projDir.path() + "/config.xml"
      << projDir.path() + "/" + ui->lineEdit_installername->text();
  m_proc.start(ifw_path + "/binarycreator.exe", arg);

  if (m_proc.waitForFinished(-1))
    duration = elap_timer.elapsed();
}

void MainWindow::createFiles() {
  QFile file(projDir.path() + "/config.xml");
  if (file.open(QFile::ReadWrite)) {
    QTextStream stream(&file);
    stream << configXML;
    file.flush();
    file.close();
  }

  QFile file1(projDir.path() + "/packages/meta/installscript.qs");
  if (file1.open(QFile::ReadWrite)) {
    QTextStream stream(&file1);
    stream << installscriptqs;
    file1.flush();
    file1.close();
  }

  QFile file2(projDir.path().remove('.') + "/packages/meta/package.xml");
  if (file2.open(QFile::ReadWrite)) {
    QTextStream stream(&file2);
    stream << packageXML;
    file2.flush();
    file2.close();
  }
}

void MainWindow::on_pushButton_back_clicked() {
  ui->plainTextEdit_config->clear();
  ui->plainTextEdit_installscript->clear();
  ui->plainTextEdit_package->clear();
  ui->stackedWidget->setCurrentIndex(0);
}

void MainWindow::on_pushButton_start_clicked() {
  createProjDirs();
  configXML = ui->plainTextEdit_config->document()->toPlainText();
  installscriptqs = ui->plainTextEdit_installscript->document()->toPlainText();
  createFiles();
  thread = QThread::create([&] {
    elap_timer.start();
    createArchive();
    createInstaller();
    projDir.cdUp();
  });
  connect(thread, &QThread::finished, [&] {
    ui->label_status->setText("Finished " + QString::number(duration) + " ms");
    QApplication::restoreOverrideCursor();
    setEnabled(true);
    auto explorer{"explorer.exe /select," +
                  QDir::toNativeSeparators(projDir.absolutePath() + "/" +
                                           ui->lineEdit_appname->text() + "/" +
                                           ui->lineEdit_installername->text())
                      .toStdString()};
    system(explorer.c_str());
  });
  thread->start();
  setEnabled(false);
  ui->label_status->setText("Working...");
  QApplication::setOverrideCursor(Qt::WaitCursor);
}

void MainWindow::on_toolButton_icon_clicked() {

  QFileInfo targeticon(QFileDialog::getOpenFileName(
      this, tr("Select Shortcut Icon"), ui->lineEdit_deploy->text(),
      tr("Icon Files (*.ico)")));

  if ((targeticon.path() != ui->lineEdit_deploy->text()) &&
      QFile::exists(ui->lineEdit_deploy->text() + "/" +
                    targeticon.fileName())) {
    QFile::remove(ui->lineEdit_deploy->text() + "/" + targeticon.fileName());
  }

  QFile::copy(targeticon.absoluteFilePath(),
              ui->lineEdit_deploy->text() + "/" + targeticon.fileName());

  ui->lineEdit_icon->setText(targeticon.fileName());
}

void MainWindow::on_toolButton_setupbanner_clicked() {
  ui->lineEdit_banner->setText(QFileDialog::getOpenFileName(
      this, tr("Select Banner Image"), ui->lineEdit_deploy->text(),
      tr("Image Files (*.png *.jpg *.bmp)")));
}

void MainWindow::on_toolButton_setupicon_clicked() {
  ui->lineEdit_setupicon->setText(QFileDialog::getOpenFileName(
      this, tr("Select Setup Icon"), ui->lineEdit_deploy->text(),
      tr("Icon Files (*.ico)")));
}

/*
         * old search code for auto vcredist finder
         *
         QString directory = ui->lineEdit_deploy->text(); // Where to search
         if (!ui->lineEdit_deploy->text().isEmpty()) {
         QDirIterator it(directory, QDirIterator::Subdirectories);
         while (it.hasNext()) {
           QString filename = it.next();
           QFileInfo file(filename);

           if (file.isDir()) {
             continue;
           }
           if (file.fileName().contains("vcredist", Qt::CaseInsensitive)) {
             vcredist = file.fileName();
           }
         }
         }
         */

void MainWindow::on_actionSet_Qt_IFW_Path_triggered() {
  if (d.exec()) {
    ifw_path = d.getIfw_path();
    disableWidgets(false);
  }
}

void MainWindow::on_actionAbout_triggered() { QApplication::aboutQt(); }

void MainWindow::on_actionLight_triggered() {
  ui->actionDark->setChecked(false);
  Settings::setStyle(Settings::Theme::lightFusion);
  Settings::writeSettings("Style", "Theme",
                          static_cast<int>(Settings::Theme::lightFusion));
}

void MainWindow::on_actionDark_triggered() {
  ui->actionLight->setChecked(false);
  Settings::setStyle(Settings::Theme::darkFusion);
  Settings::writeSettings("Style", "Theme",
                          static_cast<int>(Settings::Theme::darkFusion));
}

void MainWindow::disableWidgets(bool arg) {
  ui->groupBox_2->setDisabled(arg);
  ui->groupBox_3->setDisabled(arg);
  ui->groupBox_6->setDisabled(arg);
  ui->pushButton->setDisabled(arg);
}
