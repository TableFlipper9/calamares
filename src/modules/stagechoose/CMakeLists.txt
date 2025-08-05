calamares_add_plugin(stagechooser
    TYPE viewmodule
    EXPORT_MACRO PLUGINDLLEXPORT_PRO
    SOURCES
        Config.cpp
        StageChooserViewStep.cpp
        StageChooserPage.cpp
        SetStage3Job.cpp
    UI
        StageChooserPage.ui
    SHARED_LIB
    LINK_LIBRARIES
        Qt5::Core
        Qt5::Widgets
)
