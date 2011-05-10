project(telepathy-dolphin-file-transfer-app)

set (CMAKE_MODULE_PATH
     "${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules"
     ${CMAKE_MODULE_PATH}
)

find_package(KDE4 REQUIRED)
find_package(TelepathyQt4 REQUIRED)

include(KDE4Defaults)
include(MacroLibrary)



# Add the compiler flags
add_definitions (${QT_DEFINITIONS} ${KDE4_DEFINITIONS})
include_directories(
  ${CMAKE_SOURCE_DIR}
  ${CMAKE_BINARY_DIR}
#   ${QT_QTNETWORK_INCLUDE_DIR}
#   ${QT_QTDBUS_INCLUDE_DIR}
#   ${QT_QTSQL_INCLUDE_DIR}
#   ${QT_QTWEBKIT_INCLUDE_DIR}
  ${KDE4_INCLUDES}
#   ${SOPRANO_INCLUDE_DIR}
)

# Insert your sources here
set(MAINWINDOW_SRCS
    mainwindow.cpp
    main.cpp
)

set(TELEPATHY_KDE_FILE_TRANSFER_UI
    mainwindow.ui)

kde4_add_ui_files(MAINWINDOW_SRCS ${TELEPATHY_KDE_FILE_TRANSFER_UI})

# Create the plasmoid
kde4_add_executable(telepathy-kde-file-transfer ${MAINWINDOW_SRCS})

# Link against the needed libraries
target_link_libraries(telepathy-kde-file-transfer
#   ${QT_QTNETWORK_LIBRARY}
#   ${QT_QTDBUS_LIBRARY}
#   ${QT_QTSQL_LIBRARY}
#   ${QT_QTWEBKIT_LIBRARY}
  ${KDE4_KDEUI_LIBS}
  ${KDE4_KIO_LIBS}
#   ${KDE4_KPARTS_LIBS}
#   ${KDE4_KNEWSTUFF3_LIBS}
#   ${KDE4_PHONON_LIBS}
#   ${KDE4_SOLID_LIBS}
#   ${KDE4_NEPOMUK_LIBS}
#   ${SOPRANO_LIBRARIES}
)

install(TARGETS telepathy-kde-file-transfer ${INSTALL_TARGETS_DEFAULT_ARGS})
install(FILES mainwindow.desktop DESTINATION ${XDG_APPS_INSTALL_DIR})

macro_display_feature_log()


