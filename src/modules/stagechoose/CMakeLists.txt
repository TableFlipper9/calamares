calamares_add_plugin(stagechooser
    TYPE viewmodule
    EXPORT_MACRO PLUGINDLLEXPORT_PRO
    SOURCES
        Config.cpp
        StageChooseViewStep.cpp
        StageChoosePage.cpp
        SetStage3Job.cpp
    UI
        StageChoosePage.ui
    SHARED_LIB
    LINK_LIBRARIES
        Qt5::Core
        Qt5::Widgets
)
