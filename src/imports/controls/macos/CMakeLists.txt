# Generated from macos.pro.

#####################################################################
## qtquickcontrols2macosstyleplugin Plugin:
#####################################################################

qt_internal_add_qml_module(qtquickcontrols2macosstyleplugin
    URI "QtQuick.Controls.macOS"
    VERSION "6.0" # special case
    CLASSNAME QtQuickControls2MacOSStylePlugin
    IMPORTS
        QtQuick.Controls.Fusion/auto
    GENERATE_QMLTYPES
    SOURCES
        qtquickcontrols2macosstyleplugin.cpp
    DEFINES
        QT_NO_CAST_FROM_ASCII
        QT_NO_CAST_TO_ASCII
    LIBRARIES
        Qt::CorePrivate
        Qt::GuiPrivate
        Qt::QmlPrivate
        Qt::QuickControls2Private
        Qt::QuickPrivate
        Qt::QuickTemplates2Private
    PUBLIC_LIBRARIES
        Qt::Core
        Qt::Gui
        Qt::Qml
        Qt::Quick
        Qt::QuickControls2
        Qt::QuickTemplates2
)

#### Keys ignored in scope 1:.:.:macos.pro:<TRUE>:
# OTHER_FILES = "qmldir" "$$QML_FILES"
# QML_IMPORT_NAME = "QtQuick.Controls.macOS"
# QML_IMPORT_VERSION = "$$QT_VERSION"
# TARGETPATH = "QtQuick/Controls/macOS"
# _REQUIREMENTS = "qtConfig(quickcontrols2-macos)"

set(qml_files
    "Button.qml"
    "Slider.qml"
    "GroupBox.qml"
    "CheckBox.qml"
    "RadioButton.qml"
    "SpinBox.qml"
    "TextField.qml"
    "Frame.qml"
    "TextArea.qml"
    "ComboBox.qml"
    "ScrollView.qml"
    "ScrollBar.qml"
    "ProgressBar.qml"
    "Dial.qml"
)
set_source_files_properties(Button.qml PROPERTIES
    QT_QML_SOURCE_VERSION "6.0"
)
set_source_files_properties(Slider.qml PROPERTIES
    QT_QML_SOURCE_VERSION "6.0"
)
set_source_files_properties(GroupBox.qml PROPERTIES
    QT_QML_SOURCE_VERSION "6.0"
)
set_source_files_properties(CheckBox.qml PROPERTIES
    QT_QML_SOURCE_VERSION "6.0"
)
set_source_files_properties(RadioButton.qml PROPERTIES
    QT_QML_SOURCE_VERSION "6.0"
)
set_source_files_properties(SpinBox.qml PROPERTIES
    QT_QML_SOURCE_VERSION "6.0"
)
set_source_files_properties(TextField.qml PROPERTIES
    QT_QML_SOURCE_VERSION "6.0"
)
set_source_files_properties(Frame.qml PROPERTIES
    QT_QML_SOURCE_VERSION "6.0"
)
set_source_files_properties(TextArea.qml PROPERTIES
    QT_QML_SOURCE_VERSION "6.0"
)
set_source_files_properties(ComboBox.qml PROPERTIES
    QT_QML_SOURCE_VERSION "6.0"
)
set_source_files_properties(ScrollView.qml PROPERTIES
    QT_QML_SOURCE_VERSION "6.0"
)
set_source_files_properties(ScrollBar.qml PROPERTIES
    QT_QML_SOURCE_VERSION "6.0"
)
set_source_files_properties(ProgressBar.qml PROPERTIES
    QT_QML_SOURCE_VERSION "6.0"
)
set_source_files_properties(Dial.qml PROPERTIES
    QT_QML_SOURCE_VERSION "6.0"
)

qt6_target_qml_files(qtquickcontrols2macosstyleplugin
    FILES
        ${qml_files}
)
