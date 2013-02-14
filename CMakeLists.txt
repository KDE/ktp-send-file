project(ktp-send-file)

set (KTP_SEND_FILE_VERSION "0.5.3")

set (CMAKE_MODULE_PATH
     "${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules"
     ${CMAKE_MODULE_PATH}
)

set(KDE_MIN_VERSION "4.7.0")
set(IS_KTP_INTERNAL_MODULE TRUE)
find_package(KDE4 REQUIRED)
find_package(TelepathyQt4 0.8.9 REQUIRED)
find_package(KTp REQUIRED)

include(KDE4Defaults)
include(MacroLibrary)

# Add the compiler flags
add_definitions (${QT_DEFINITIONS} ${KDE4_DEFINITIONS})
include_directories(
  ${CMAKE_SOURCE_DIR}
  ${CMAKE_BINARY_DIR}
  ${KDE4_INCLUDES}
  ${TELEPATHY_QT4_INCLUDE_DIR}
  ${KTP_INCLUDE_DIR}
)

# Insert your sources here
set(KTP_SEND_FILE_SRCS
    mainwindow.cpp
    main.cpp
)

set(KTP_SEND_FILE_UI
    mainwindow.ui)

configure_file(version.h.in ${CMAKE_CURRENT_BINARY_DIR}/version.h)

kde4_add_ui_files(KTP_SEND_FILE_SRCS ${KTP_SEND_FILE_UI})

# Create the plasmoid
kde4_add_executable(ktp-send-file ${KTP_SEND_FILE_SRCS})

# Link against the needed libraries
target_link_libraries(ktp-send-file
  ${TELEPATHY_QT4_LIBRARIES}
  ${KTP_LIBRARIES}
  ${KTP_MODELS_LIBRARIES}
  ${KTP_WIDGETS_LIBRARIES}
  ${KDE4_KDEUI_LIBS}
  ${KDE4_KIO_LIBS}
)

install(TARGETS ktp-send-file ${INSTALL_TARGETS_DEFAULT_ARGS})
install(FILES ktp-send-file.desktop DESTINATION ${SERVICES_INSTALL_DIR}/ServiceMenus)

macro_display_feature_log()


