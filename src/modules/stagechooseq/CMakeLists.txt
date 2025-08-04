# === This file is part of Calamares - <https://calamares.io> ===
#
#   SPDX-FileCopyrightText: 2020 Adriaan de Groot <groot@kde.org>
#   SPDX-License-Identifier: BSD-2-Clause
#

# This is a re-write of the stagechoose module using QML view steps
# instead of widgets.

if(NOT WITH_QML)
    calamares_skip_module( "stagechooseq (QML is not supported in this build)" )
    return()
endif()

set(_stagechoose ${CMAKE_CURRENT_SOURCE_DIR}/../stagechoose)

include_directories(${_stagechoose})

# DUPLICATED WITH stagechoose MODULE
find_package(${qtname} ${QT_VERSION} CONFIG REQUIRED DBus Network)

find_package(LIBPARTED)
if(LIBPARTED_FOUND)
    set(PARTMAN_SRC ${_stagechoose}/checker/partman_devices.c)
    set(CHECKER_LINK_LIBRARIES ${LIBPARTED_LIBRARY})
else()
    set(PARTMAN_SRC)
    set(CHECKER_LINK_LIBRARIES)
    add_definitions(-DWITHOUT_LIBPARTED)
endif()

set(CHECKER_SOURCES ${_stagechoose}/checker/GeneralRequirements.cpp ${PARTMAN_SRC})

calamares_add_plugin(stagechooseq
    TYPE viewmodule
    EXPORT_MACRO PLUGINDLLEXPORT_PRO
    SOURCES
        ${CHECKER_SOURCES}
        stagechooseQmlViewStep.cpp
        ${_stagechoose}/Config.cpp
    RESOURCES
    stagechooseq${QT_VERSION_SUFFIX}.qrc
    LINK_PRIVATE_LIBRARIES
        ${CHECKER_LINK_LIBRARIES}
        ${qtname}::DBus
        ${qtname}::Network
    SHARED_LIB
)
