project(telepathy-kde-send-file)

set (CMAKE_MODULE_PATH
     "${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules"
     ${CMAKE_MODULE_PATH}
)

find_package(KDE4 REQUIRED)
find_package(TelepathyQt4 0.7.1 REQUIRED)

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
  ${TELEPATHY_QT4_INCLUDE_DIR}

#   ${SOPRANO_INCLUDE_DIR}
)

# Insert your sources here
set(TELEPATHY_KDE_SEND_FILE_SRCS
    mainwindow.cpp
    main.cpp
    accounts-model-item.cpp
    account-filter-model.cpp
    accounts-model.cpp
    contact-model-item.cpp
    tree-node.cpp
    flat-model-proxy.cpp
)

set(TELEPATHY_KDE_SEND_FILE_UI
    mainwindow.ui)

kde4_add_ui_files(TELEPATHY_KDE_SEND_FILE_SRCS ${TELEPATHY_KDE_SEND_FILE_UI})

# Create the plasmoid
kde4_add_executable(telepathy-kde-send-file ${TELEPATHY_KDE_SEND_FILE_SRCS})

# Link against the needed libraries
target_link_libraries(telepathy-kde-send-file
  ${TELEPATHY_QT4_LIBRARIES}
  ${KDE4_KDEUI_LIBS}
  ${KDE4_KIO_LIBS}

)

install(TARGETS telepathy-kde-send-file ${INSTALL_TARGETS_DEFAULT_ARGS})
install(FILES telepathy-kde-send-file.desktop DESTINATION ${XDG_APPS_INSTALL_DIR})

macro_display_feature_log()


