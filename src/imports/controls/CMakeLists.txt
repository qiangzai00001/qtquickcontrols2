# Generated from controls.pro.

#####################################################################
## qtquickcontrols2plugin Plugin:
#####################################################################

qt_internal_add_qml_module(qtquickcontrols2plugin
    URI "QtQuick.Controls"
    VERSION "6.0" # special case
    DESIGNER_SUPPORTED
    CLASSNAME QtQuickControls2Plugin
    SKIP_TYPE_REGISTRATION
    IMPORTS
        QtQuick.Controls.impl/auto
    OPTIONAL_IMPORTS
        QtQuick.Controls.Basic/auto
        QtQuick.Controls.Fusion/auto
        QtQuick.Controls.Material/auto
        QtQuick.Controls.Imagine/auto
        QtQuick.Controls.Universal/auto
        QtQuick.Controls.Windows/auto
        QtQuick.Controls.macOS/auto
    SOURCES
        qtquickcontrols2plugin.cpp
    DEFINES
        QT_NO_CAST_FROM_ASCII
        QT_NO_CAST_TO_ASCII
    LIBRARIES
        Qt::CorePrivate
        Qt::GuiPrivate
        Qt::QmlPrivate
        Qt::QuickControls2ImplPrivate
        Qt::QuickControls2Private
        Qt::QuickPrivate
        Qt::QuickTemplates2Private
    PUBLIC_LIBRARIES
        Qt::Core
        Qt::Gui
        Qt::Qml
        Qt::Quick
        Qt::QuickControls2
)

#### Keys ignored in scope 1:.:.:controls.pro:<TRUE>:
# OTHER_FILES = "qmldir" "$$PWD/snippets/*.qml" "$$PWD/src/*.qdoc" "$$PWD/src/templates/*.qdoc"
# QML_IMPORT_VERSION = "$$QT_VERSION"
# TARGETPATH = "QtQuick/Controls"

## Scopes:
#####################################################################
qt_internal_add_docs(qtquickcontrols2plugin
    doc/qtquickcontrols.qdocconf
)

