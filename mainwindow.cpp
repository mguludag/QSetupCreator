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

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow) {
  ui->setupUi(this);
  currDir = QDir::current();
  Settings::init(Settings::Format::regFormat, "QIFWAutomation");
  Settings::setStyle(Settings::Theme::darkFusion);
  ui->label_status->setText("Ready...");
  ui->statusbar->addWidget(ui->label_status);
  ui->lineEdit_qtbinpath->setText(
      Settings::readSettings("QT BINS", "Path").toString());
  ifw_path = ui->lineEdit_qtbinpath->text();
}

MainWindow::~MainWindow() { delete ui; }

void MainWindow::on_toolButton_qtifw_clicked() {
  ui->lineEdit_qtbinpath->setText(QFileDialog::getExistingDirectory(
      this, tr("Open Qt Bin Directory"), currDir.currentPath(),
      QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks));
  Settings::writeSettings("QT BINS", "Path", ui->lineEdit_qtbinpath->text());
  ifw_path = ui->lineEdit_qtbinpath->text();
}

void MainWindow::on_toolButton_deploy_clicked() {
  ui->lineEdit_deploy->setText(QFileDialog::getExistingDirectory(
      this, tr("Open Deploy Directory"), currDir.currentPath(),
      QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks));
}

void MainWindow::on_pushButton_clicked() {
  thread = QThread::create([&] {
    elap_timer.start();
    QDir dir;
    if (!ui->lineEdit_deploy->text().isEmpty()) {
      if (dir.setCurrent(QFileDialog::getExistingDirectory(
              this, tr("Open Project Directory"), currDir.currentPath(),
              QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks))) {
        dir.mkdir(ui->lineEdit_appname->text());
        // dir.setCurrent(dir.path().remove('.') +
        // ui->lineEdit_appname->text());
        QStringList target_dir;
        target_dir << "@RootDir@"
                   << "@ApplicationsDir@";
        dir.cd(ui->lineEdit_appname->text());
        QFile file(dir.path() + "/config.xml");
        if (file.open(QFile::ReadWrite)) {
          QTextStream stream(&file);
          QString config =
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
                   ? "<Banner>" + ui->lineEdit_deploy->text() + "/" +
                         ui->lineEdit_banner->text() + "</Banner>\n"
                   : "") +
              (!ui->lineEdit_setupicon->text().isEmpty()
                   ? "<InstallerWindowIcon>" + ui->lineEdit_deploy->text() +
                         "/" + ui->lineEdit_setupicon->text() +
                         "</InstallerWindowIcon>\n\
          <InstallerApplicationIcon>" +
                         ui->lineEdit_deploy->text() + "/" +
                         ui->lineEdit_setupicon->text() +
                         "</InstallerApplicationIcon>\n"
                   : "") +
              "<!-- Directory name is used in component.xml -->\n\
          <StartMenuDir>" +
              ui->lineEdit_appname->text() + "</StartMenuDir>\n\
          <TargetDir>" +
              target_dir.at(ui->comboBox_dest->currentIndex()) + "/ " +
              ui->lineEdit_appname->text() + "</TargetDir>\n\
        </Installer>\n";
          stream << config;
          file.flush();
          file.close();
        }
        dir.mkpath("packages/data");
        dir.mkpath("packages/meta");

        QDir deployDir(ui->lineEdit_deploy->text());
        depDirName = deployDir.dirName();
        QStringList arg;
        arg << dir.path() + "/packages/data/" + ui->lineEdit_appname->text() +
                   ".7z"
            << deployDir.path();
        m_proc.start(ifw_path + "/archivegen.exe", arg);
        m_proc.waitForFinished(-1);
        QFile file1(dir.path() + "/packages/meta/installscript.qs");
        if (file1.open(QFile::ReadWrite)) {
          QTextStream stream(&file1);
          auto val = ui->lineEdit_icon->text().isEmpty();
          QString installscript =
              "function Component()\n\
    {\n\
        // default constructor\n\
    }\n\
    \n\
    Component.prototype.createOperationsForArchive = function(archive)\n\
    {\n\
        console.log(\"performing custom extract operation\")\n\
        component.addOperation(\"Extract\", archive, \"@TargetDir@\" + \"/\" + installer.value(\"version\") + \"/\");\n\
    \n\
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
                         "\", \"@StartMenuDir@/" +
                         ui->lineEdit_nameshortcut->text() + ".lnk\",\n\
            \"workingDirectory=@TargetDir@\", \"iconPath=@TargetDir@/" +
                         depDirName + "/" +
                         (val ? ui->lineEdit_target->text()
                              : ui->lineEdit_icon->text()) +
                         "\",\n" + (val ? "\"iconId=1\", " : " ") +
                         "\"description=" + ui->lineEdit_nameshortcut->text() +
                         " Executable\");\n"
                   : "") +
              (ui->checkBox_desktop->isChecked()
                   ? +"\
            component.addOperation(\"CreateShortcut\", \"@TargetDir@/" +
                         depDirName + "/" + ui->lineEdit_target->text() +
                         "\", \"@DesktopDir@/" +
                         ui->lineEdit_nameshortcut->text() + ".lnk\",\n\
            \"workingDirectory=@TargetDir@\", \"iconPath=@TargetDir@/" +
                         depDirName + "/" +
                         (val ? ui->lineEdit_target->text()
                              : ui->lineEdit_icon->text()) +
                         "\",\n" + (val ? "\"iconId=1\", " : " ") +
                         "\"description=" + ui->lineEdit_nameshortcut->text() +
                         " Executable\");\n"
                   : "") +
              "}\n" +
              (ui->textEdit->toPlainText().isEmpty() ? "};\n"
                                                     : process_redist());
          stream << installscript;
          file1.flush();
          file1.close();
        }
      }

      QFile file2(dir.path().remove('.') + "/packages/meta/package.xml");
      if (file2.open(QFile::ReadWrite)) {
        QTextStream stream(&file2);
        QString config = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n\
    <Package>\n\
      <DisplayName>" + ui->lineEdit_appname->text() +
                         "</DisplayName>\n\
      <Description>This is " +
                         ui->lineEdit_appname->text() +
                         " installer</Description>\n\
      <Version>" + ui->lineEdit_version->text() +
                         "</Version>\n\
      <ReleaseDate>" + QDateTime::currentDateTime().toString("yyyy-MM-dd") +
                         "</ReleaseDate>\n\
      <Default>true</Default>\n\
      <Script>installscript.qs</Script>\n\
      </Package>\n";
        stream << config;
        file2.flush();
        file2.close();
      }

      QStringList arg;
      arg << "-p" << dir.path().remove('.') << "-c"
          << dir.path() + "/config.xml"
          << dir.path() + "/" + ui->lineEdit_installername->text();
      m_proc.start(ifw_path + "/binarycreator.exe", arg);
      if (m_proc.waitForFinished(-1))
        duration = elap_timer.elapsed();
    }
  });
  connect(thread, &QThread::finished, [&] {
    ui->label_status->setText("Finished " + QString::number(duration) + " ms");
    QApplication::restoreOverrideCursor();
    setEnabled(true);
  });
  thread->start();
  setEnabled(false);
  ui->label_status->setText("Working...");
  QApplication::setOverrideCursor(Qt::WaitCursor);
}

QString MainWindow::process_redist() {
  QString result{};
  QString data = ui->textEdit->toPlainText();
  QString head =
      "component.addElevatedOperation(\"Execute\", \"{0,3010,1638,5100}\", "
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
