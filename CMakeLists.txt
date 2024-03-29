cmake_minimum_required(VERSION 3.16)

# KDE Application Version, managed by release script
set (RELEASE_SERVICE_VERSION_MAJOR "23")
set (RELEASE_SERVICE_VERSION_MINOR "07")
set (RELEASE_SERVICE_VERSION_MICRO "70")
set (KTP_SEND_FILE_VERSION "${RELEASE_SERVICE_VERSION_MAJOR}.${RELEASE_SERVICE_VERSION_MINOR}.${RELEASE_SERVICE_VERSION_MICRO}")

project(ktp-send-file VERSION ${KTP_SEND_FILE_VERSION})

find_package(ECM 1.6.0 REQUIRED NO_MODULE)
set(CMAKE_MODULE_PATH ${ECM_MODULE_PATH})

find_package(Qt5 5.0 REQUIRED COMPONENTS Widgets)
find_package(KF5 5.0 REQUIRED COMPONENTS KCMUtils KIO I18n)
find_package(KTp REQUIRED)

include(KDEInstallDirs)
include(KDECMakeSettings)
include(KDECompilerSettings NO_POLICY_SCOPE)
include(FeatureSummary)

configure_file(version.h.in ${CMAKE_CURRENT_BINARY_DIR}/version.h)

qt5_wrap_ui(KTP_SEND_FILE_SRCS mainwindow.ui)

# Create the plasmoid
add_executable(ktp-send-file mainwindow.cpp main.cpp ${KTP_SEND_FILE_SRCS})

# Link against the needed libraries
target_link_libraries(ktp-send-file
  KTp::CommonInternals
  KTp::Widgets
  KTp::Models
  Qt5::Widgets
  KF5::CoreAddons
  KF5::I18n
  KF5::KIOWidgets
)

install(TARGETS ktp-send-file ${KDE_INSTALL_TARGETS_DEFAULT_ARGS})

ki18n_install(po)

feature_summary(WHAT ALL FATAL_ON_MISSING_REQUIRED_PACKAGES)
