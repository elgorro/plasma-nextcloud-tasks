cmake_minimum_required(VERSION 3.16)
project(plasma-nextcloud-tasks)

set(KF_MIN_VERSION "6.0.0")
set(QT_MIN_VERSION "6.6.0")

find_package(ECM ${KF_MIN_VERSION} REQUIRED NO_MODULE)
set(CMAKE_MODULE_PATH ${ECM_MODULE_PATH})

include(KDEInstallDirs)
include(KDECMakeSettings)
include(KDECompilerSettings NO_POLICY_SCOPE)

find_package(Qt6 ${QT_MIN_VERSION} REQUIRED COMPONENTS Core Network Qml Quick)
find_package(KF6 ${KF_MIN_VERSION} REQUIRED COMPONENTS I18n)
find_package(Plasma REQUIRED)

# Ensure KDE_INSTALL_QMLDIR is set — query Qt6 for the real QML path
if(NOT KDE_INSTALL_QMLDIR)
    include(ECMQueryQt)
    ecm_query_qt(qt_qml_dir QT_INSTALL_QML)
    set(KDE_INSTALL_QMLDIR "${qt_qml_dir}")
endif()

plasma_install_package(package com.github.elgorro.nextcloud-tasks)

add_subdirectory(plugin)
