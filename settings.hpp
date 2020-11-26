/*This class can r/w settings, set themes*/
/************created by m231**************/
/**************m231@tnctr.com*************/
#ifndef SETTINGS_HPP
#define SETTINGS_HPP
#include <QApplication>
#include <QDesktopWidget>
#include <QPalette>
#include <QSettings>
#include <QStyle>
#include <QStyleFactory>
#include <QToolTip>

class Settings {

public:
  enum class Palette { light, dark };
  enum class Theme { vista, classic, lightFusion, darkFusion };
  enum class Format { regFormat, iniFormat };

  static void init(Format format, const QString &name);

  ~Settings() {
    delete m_instance;
    delete m_settingsObj;
  }

  /*!
  \fn int loadStyle
  \brief Loads current application style from settings file or registry
  */
  static int loadStyle();

  /*!
  \fn void setStyle
  \brief Apply a given style to application
  */
  static void setStyle(const Theme val);

  /*!
  \fn QVariant readSettings
  \brief Read a value stored in settings given group and key
  */
  static QVariant readSettings(const QString group, const QString key);

  template <class T>
  /*!
  \fn void writeSettings
  \brief Write a value in settings given group and key
  */
  static void writeSettings(const QString group, const QString key,
                            const T option) {
    m_instance->m_settingsObj->beginGroup(group);
    m_instance->m_settingsObj->setValue(key, option);
    m_instance->m_settingsObj->endGroup();
  }

private:
  static Settings *m_instance;
  QSettings *m_settingsObj = nullptr;

  Settings() = delete;

  // Construct a Settings object
  Settings(Format format, const QString &name);

  /*!
  \fn void changePalette
  \brief Change the color palette between light or dark
  */
  static void changePalette(Palette _palette);
};

#endif // SETTINGS_HPP
