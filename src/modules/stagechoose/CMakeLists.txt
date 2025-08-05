# === Calamares - stagechooser module ===

set(module_name "stagechooser")

# List of all source files
set(${module_name}_SRCS
    Config.cpp
    StageChooserPage.cpp
    StageChooserViewStep.cpp
    SetStage3Job.cpp
)

# List of all header files (for installation or IDEs)
set(${module_name}_HDRS
    Config.h
    StageChooserPage.h
    StageChooserViewStep.h
    SetStage3Job.h
)

# UI file
set(${module_name}_UIS
    StageChooserPage.ui
)

# Optional: If you embed any resources (e.g., icons)
# set(${module_name}_RCCS stagechooser.qrc)

# Register the module with Calamares
calamares_add_viewmodule(${module_name}
    SOURCES ${${module_name}_SRCS}
    HEADERS ${${module_name}_HDRS}
    UIS ${${module_name}_UIS}
    # RCCS ${${module_name}_RCCS}  # Uncomment if you use .qrc files
)
