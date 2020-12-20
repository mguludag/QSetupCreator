/*****************************************************************************
 * settings.cpp
 *
 * Created: 11/26/2020 2020 by mguludag
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
#include "settings.hpp"

Settings *Settings::m_instance = nullptr;

void Settings::init(Settings::Format format, const QString &name) {
  if (!m_instance)
    m_instance = new Settings(format, name);
}

Settings::Theme Settings::loadStyle() {
  int val;
  m_instance->m_settingsObj->beginGroup("Style");
  val = m_instance->m_settingsObj
            ->value("Theme", static_cast<int>(Theme::lightFusion))
            .toInt(); // default theme is lightFusion
  m_instance->m_settingsObj->endGroup();
  return static_cast<Theme>(val);
}

void Settings::setStyle(const Settings::Theme val) {
  switch (val) {
  case Theme::vista:
    qApp->setStyle(QStyleFactory::create("windowsvista"));
    changePalette(Palette::light);
    break;
  case Theme::classic:
    qApp->setStyle(QStyleFactory::create("windows"));
    changePalette(Palette::light);
    break;
  case Theme::lightFusion:
    qApp->setStyle(QStyleFactory::create("Fusion"));
    changePalette(Palette::light);
    break;
  case Theme::darkFusion:
    qApp->setStyle(QStyleFactory::create("Fusion"));
    changePalette(Palette::dark);
    break;
  default:
    break;
  }
}

QVariant Settings::readSettings(const QString group, const QString key) {
  QVariant val;
  m_instance->m_settingsObj->beginGroup(group);
  val = m_instance->m_settingsObj->value(key);
  m_instance->m_settingsObj->endGroup();
  return val;
}

Settings::Settings(Settings::Format format, const QString &name) {
  (static_cast<bool>(format))
      ? m_settingsObj = new QSettings(name, QSettings::IniFormat)
      : m_settingsObj = new QSettings(name, QApplication::applicationName());
}

void Settings::changePalette(Settings::Palette _palette) {
  static QPalette Palette;
  if (static_cast<bool>(_palette)) {
    Palette.setColor(QPalette::Window, QColor(53, 53, 53));
    Palette.setColor(QPalette::WindowText, Qt::white);
    Palette.setColor(QPalette::Base, QColor(25, 25, 25));
    Palette.setColor(QPalette::AlternateBase, QColor(53, 53, 53));
    Palette.setColor(QPalette::ToolTipBase, QColor(53, 53, 53));
    Palette.setColor(QPalette::ToolTipText, Qt::white);
    Palette.setColor(QPalette::Text, Qt::white);
    Palette.setColor(QPalette::Button, QColor(53, 53, 53));
    Palette.setColor(QPalette::ButtonText, Qt::white);
    Palette.setColor(QPalette::BrightText, Qt::red);
    Palette.setColor(QPalette::Link, QColor(42, 130, 218));
    Palette.setColor(QPalette::Highlight, QColor(42, 130, 218));
    Palette.setColor(QPalette::HighlightedText, Qt::black);
    Palette.setColor(QPalette::Disabled, QPalette::Text, QColor(164, 166, 168));
    Palette.setColor(QPalette::Disabled, QPalette::WindowText,
                     QColor(164, 166, 168));
    Palette.setColor(QPalette::Disabled, QPalette::ButtonText,
                     QColor(164, 166, 168));
    Palette.setColor(QPalette::Disabled, QPalette::HighlightedText,
                     QColor(164, 166, 168));
    Palette.setColor(QPalette::Disabled, QPalette::Base, QColor(68, 68, 68));
    Palette.setColor(QPalette::Disabled, QPalette::Window, QColor(68, 68, 68));
    Palette.setColor(QPalette::Disabled, QPalette::Highlight,
                     QColor(68, 68, 68));
  } else
    Palette = qApp->style()->standardPalette();
  QToolTip::setPalette(Palette);
  qApp->setPalette(Palette);
}
