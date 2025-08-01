# === This file is part of Calamares - <https://calamares.io> ===
#
#   SPDX-FileCopyrightText: 2020 Adriaan de Groot <groot@kde.org>
#   SPDX-License-Identifier: BSD-2-Clause
#
find_package(${qtname} ${QT_VERSION} CONFIG REQUIRED DBus Network)

find_package(LIBPARTED)
if(LIBPARTED_FOUND)
    set(PARTMAN_SRC checker/partman_devices.c)
    set(PARTMAN_LIB ${LIBPARTED_LIBRARY})
else()
    set(PARTMAN_SRC)
    set(PARTMAN_LIB)
    add_definitions(-DWITHOUT_LIBPARTED)
endif()

calamares_add_plugin(stagechoose
    TYPE viewmodule
    EXPORT_MACRO PLUGINDLLEXPORT_PRO
    SOURCES
        checker/CheckerContainer.cpp
        checker/GeneralRequirements.cpp
        checker/ResultDelegate.cpp
        checker/ResultsListWidget.cpp
        ${PARTMAN_SRC}
        stagechooseViewStep.cpp
        Config.cpp
        Config.h
        stagechoosePage.cpp
    UI
        stagechoosePage.ui
    RESOURCES
        stagechoose.qrc
    LINK_PRIVATE_LIBRARIES
        ${PARTMAN_LIB}
        ${qtname}::DBus
        ${qtname}::Network
    SHARED_LIB
)

calamares_add_test(
    stagechoosetest
    SOURCES checker/GeneralRequirements.cpp ${PARTMAN_SRC} Config.cpp Tests.cpp
    LIBRARIES ${PARTMAN_LIB} ${qtname}::DBus ${qtname}::Network ${qtname}::Widgets Calamares::calamaresui
)
