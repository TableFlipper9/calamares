calamares_add_plugin(stagechoose
    TYPE viewmodule
    EXPORT_MACRO PLUGINDLLEXPORT_PRO
    SOURCES
        Config.cpp
        StageChooseViewStep.cpp
        StageChoosePage.cpp
        SetStage3Job.cpp
        StageFetcher.cpp
    UI
        StageChoosePage.ui
    SHARED_LIB
)
