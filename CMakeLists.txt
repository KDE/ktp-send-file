project(ktp-send-file)

cmake_minimum_required(VERSION 2.8.12)

# KDE Application Version, managed by release script
set (RELEASE_SERVICE_VERSION_MAJOR "21")
set (RELEASE_SERVICE_VERSION_MINOR "08")
set (RELEASE_SERVICE_VERSION_MICRO "1")
set (KTP_SEND_FILE_VERSION "${RELEASE_SERVICE_VERSION_MAJOR}.${RELEASE_SERVICE_VERSION_MINOR}.${RELEASE_SERVICE_VERSION_MICRO}")

set(IS_KTP_INTERNAL_MODULE TRUE)

find_package(ECM 1.3.0 REQUIRED NO_MODULE)
set(CMAKE_MODULE_PATH ${ECM_MODULE_PATH} ${ECM_KDE_MODULE_DIR})

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

install(TARGETS ktp-send-file ${INSTALL_TARGETS_DEFAULT_ARGS})

feature_summary(WHAT ALL FATAL_ON_MISSING_REQUIRED_PACKAGES)
