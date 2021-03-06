/****************************************************************************
**
** Copyright (C) 2020 The Qt Company Ltd.
** Contact: http://www.qt.io/licensing/
**
** This file is part of the Qt Quick Controls 2 module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL3$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see http://www.qt.io/terms-conditions. For further
** information use the contact form at http://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 3 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPLv3 included in the
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 3 requirements
** will be met: https://www.gnu.org/licenses/lgpl.html.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 2.0 or later as published by the Free
** Software Foundation and appearing in the file LICENSE.GPL included in
** the packaging of this file. Please review the following information to
** ensure the GNU General Public License version 2.0 requirements will be
** met: http://www.gnu.org/licenses/gpl-2.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qquickstyleitem.h"

#include <QtCore/qscopedvaluerollback.h>

#include <QtQuick/qsgninepatchnode.h>
#include <QtQuick/private/qquickwindow_p.h>
#include <QtQuick/qquickwindow.h>

#include <QtQuickTemplates2/private/qquickcontrol_p.h>
#include <QtQuickTemplates2/private/qquickbutton_p.h>

#include <QtQml/qqml.h>

#include "qquickstyleitembutton.h"
#include "qquickstylehelper_p.h"

QT_BEGIN_NAMESPACE

QDebug operator<<(QDebug debug, const QQuickStyleMargins &padding)
{
    QDebugStateSaver saver(debug);
    debug.nospace();
    debug << "StyleMargins(";
    debug << padding.left() << ", ";
    debug << padding.top() << ", ";
    debug << padding.right() << ", ";
    debug << padding.bottom();
    debug << ')';
    return debug;
}

QDebug operator<<(QDebug debug, const StyleItemGeometry &cg)
{
    QDebugStateSaver saver(debug);
    debug.nospace();
    debug << "StyleItemGeometry(";
    debug << "implicitSize:" << cg.implicitSize << ", ";
    debug << "contentRect:" << cg.contentRect << ", ";
    debug << "layoutRect:" << cg.layoutRect << ", ";
    debug << "minimumSize:" << cg.minimumSize << ", ";
    debug << "9patchMargins:" << cg.ninePatchMargins;
    debug << ')';
    return debug;
}

QQuickStyleItem::QQuickStyleItem()
{
    setFlag(QQuickItem::ItemHasContents);
}

QQuickStyleItem::~QQuickStyleItem()
{
}

void QQuickStyleItem::connectToControl()
{
    connect(m_control, &QQuickStyleItem::enabledChanged, this, &QQuickStyleItem::markImageDirty);
    connect(m_control, &QQuickItem::activeFocusChanged, this, &QQuickStyleItem::markImageDirty);
    connect(m_control, &QQuickStyleItem::windowChanged, this, &QQuickStyleItem::markImageDirty);
    connect(window(), &QQuickWindow::activeChanged, this, &QQuickStyleItem::markImageDirty);
}

void QQuickStyleItem::markImageDirty()
{
    m_dirty.setFlag(DirtyFlag::Image);
    polish();
}

void QQuickStyleItem::markGeometryDirty()
{
    m_dirty.setFlag(DirtyFlag::Geometry);
    polish();
}

QSGNode *QQuickStyleItem::updatePaintNode(QSGNode *oldNode, QQuickItem::UpdatePaintNodeData *)
{
    QSGNinePatchNode *node = static_cast<QSGNinePatchNode *>(oldNode);
    if (!node)
        node = window()->createNinePatchNode();

    auto texture = window()->createTextureFromImage(m_paintedImage, QQuickWindow::TextureCanUseAtlas);

    QRectF bounds = boundingRect();
    const qreal scale = window()->devicePixelRatio();
    const QSizeF ninePatchImageSize = m_paintedImage.rect().size() / scale;
#ifdef QT_DEBUG
    if (m_debugFlags.testFlag(Unscaled)) {
        bounds = QRectF(QPointF(), ninePatchImageSize);
        qqc2Debug() << "Setting paint node bounds to size of image:" << bounds;
    }
#endif

    QMargins padding = m_useNinePatchImage ? m_styleItemGeometry.ninePatchMargins : QMargins(0, 0, 0, 0);
    if (padding.right() == -1) {
        // Special case: a right padding of -1 means that
        // the image should not scale horizontally.
        bounds.setWidth(ninePatchImageSize.width());
        padding.setLeft(0);
        padding.setRight(0);
    } else if (boundingRect().width() < imageSize().width()) {
        // If the item size is smaller that the image, using nine-patch scaling
        // ends up wrapping it. In that case we scale the whole image instead.
        padding.setLeft(0);
        padding.setRight(0);
    }
    if (padding.bottom() == -1) {
        bounds.setHeight(ninePatchImageSize.height());
        padding.setTop(0);
        padding.setBottom(0);
    } else if (boundingRect().height() < imageSize().height()) {
        padding.setTop(0);
        padding.setBottom(0);
    }

    node->setBounds(bounds);
    node->setTexture(texture);
    node->setDevicePixelRatio(window()->devicePixelRatio());
    node->setPadding(padding.left(), padding.top(), padding.right(), padding.bottom());
    node->update();

    return node;
}

QStyle::State QQuickStyleItem::controlSize(QQuickItem *item)
{
    // TODO: add proper API for small and mini
    if (item->metaObject()->indexOfProperty("qqc2_style_small") != -1)
        return QStyle::State_Small;
    if (item->metaObject()->indexOfProperty("qqc2_style_mini") != -1)
        return QStyle::State_Mini;
    return QStyle::State_None;
}

void QQuickStyleItem::initStyleOptionBase(QStyleOption &styleOption)
{
    Q_ASSERT(m_control);

    styleOption.control = const_cast<QQuickItem *>(control<QQuickItem>());
    styleOption.window = window();
    styleOption.palette = QQuickItemPrivate::get(m_control)->palette()->toQPalette();
    styleOption.rect = QRect(QPoint(0, 0), imageSize());

    styleOption.state = QStyle::State_None;
    styleOption.state |= controlSize(styleOption.control);

    // Note: not all controls inherit from QQuickControl (e.g QQuickTextField)
    if (const auto quickControl = dynamic_cast<QQuickControl *>(m_control.data()))
        styleOption.direction = quickControl->isMirrored() ? Qt::RightToLeft : Qt::LeftToRight;

    if (window()) {
        if (styleOption.window->isActive())
            styleOption.state |= QStyle::State_Active;
        if (m_control->isEnabled())
            styleOption.state |= QStyle::State_Enabled;
        if (m_control->hasActiveFocus())
            styleOption.state |= QStyle::State_HasFocus;
        if (m_control->isUnderMouse())
            styleOption.state |= QStyle::State_MouseOver;
        // Should this depend on the focusReason (e.g. only TabFocus) ?
        styleOption.state |= QStyle::State_KeyboardFocusChange;
    }

    if (m_overrideState != None) {
        // In Button.qml we fade between two versions of
        // the handle, depending on if it's hovered or not
        if (m_overrideState & AlwaysHovered)
            styleOption.state |= QStyle::State_MouseOver;
        else if (m_overrideState & NeverHovered)
            styleOption.state &= ~QStyle::State_MouseOver;
    }

    qqc2Debug() << styleOption;
}

void QQuickStyleItem::geometryChange(const QRectF &newGeometry, const QRectF &oldGeometry)
{
    QQuickItem::geometryChange(newGeometry, oldGeometry);

    // Ensure that we only schedule a new geometry update
    // and polish if this geometry change was caused by
    // something else than us already updating geometry.
    if (!m_polishing)
        markGeometryDirty();
}

void QQuickStyleItem::updateGeometry()
{
    qqc2DebugHeading("GEOMETRY");
    m_dirty.setFlag(DirtyFlag::Geometry, false);

    const QQuickStyleMargins oldContentPadding = contentPadding();
    const QQuickStyleMargins oldLayoutMargins = layoutMargins();
    const QSize oldMinimumSize = minimumSize();

    m_styleItemGeometry = calculateGeometry();

#ifdef QT_DEBUG
    if (m_styleItemGeometry.minimumSize.isEmpty())
        qmlWarning(this) << "minimumSize is empty!";
#endif

    if (m_styleItemGeometry.implicitSize.isEmpty()) {
        // If the item has no contents (or its size is
        // empty), we just use the minimum size as implicit size.
        m_styleItemGeometry.implicitSize = m_styleItemGeometry.minimumSize;
        qqc2Debug() << "implicitSize is empty, using minimumSize instead";
    }

    if (contentPadding() != oldContentPadding)
        emit contentPaddingChanged();
    if (layoutMargins() != oldLayoutMargins)
        emit layoutMarginsChanged();
    if (minimumSize() != oldMinimumSize)
        emit minimumSizeChanged();

    setImplicitSize(m_styleItemGeometry.implicitSize.width(), m_styleItemGeometry.implicitSize.height());

    qqc2Debug() << m_styleItemGeometry
                << "bounding rect:" << boundingRect()
                << "layout margins:" << layoutMargins()
                << "content padding:" << contentPadding()
                << "input content size:" << m_contentSize;
}

void QQuickStyleItem::paintControlToImage()
{
    qqc2DebugHeading("PAINT");
    if (m_styleItemGeometry.minimumSize.isEmpty())
        return;

    m_dirty.setFlag(DirtyFlag::Image, false);
    const qreal scale = window()->devicePixelRatio();
    m_paintedImage = QImage(imageSize() * scale, QImage::Format_ARGB32_Premultiplied);
    m_paintedImage.setDevicePixelRatio(scale);
    m_paintedImage.fill(Qt::transparent);

    QPainter painter(&m_paintedImage);
    paintEvent(&painter);

#ifdef QT_DEBUG
    if (m_debugFlags != NoDebug) {
        painter.setPen(QColor(255, 0, 0, 255));
        if (m_debugFlags.testFlag(ImageRect))
            painter.drawRect(QRect(QPoint(0, 0), m_paintedImage.size() / scale));
        if (m_debugFlags.testFlag(LayoutRect)) {
            const auto m = layoutMargins();
            QRect rect = QRect(QPoint(0, 0), m_paintedImage.size() / scale);
            rect.adjust(m.left(), m.top(), -m.right(), -m.bottom());
            painter.drawRect(rect);
        }
        if (m_debugFlags.testFlag(ContentRect)) {
            const auto p = contentPadding();
            QRect rect = QRect(QPoint(0, 0), m_paintedImage.size() / scale);
            rect.adjust(p.left(), p.top(), -p.right(), -p.bottom());
            painter.drawRect(rect);
        }
        if (m_debugFlags.testFlag(InputContentSize)) {
            const int offset = 2;
            const QPoint p = m_styleItemGeometry.contentRect.topLeft();
            painter.drawLine(p.x() - offset, p.y() - offset, p.x() + m_contentSize.width(), p.y() - offset);
            painter.drawLine(p.x() - offset, p.y() - offset, p.x() - offset, p.y() + m_contentSize.height());
        }
        if (m_debugFlags.testFlag(NinePatchMargins)) {
            const QMargins m = m_styleItemGeometry.ninePatchMargins;
            const int w = int(m_paintedImage.rect().width() / scale);
            const int h = int(m_paintedImage.rect().height() / scale);
            if (m.right() != -1) {
                painter.drawLine(m.left(), 0, m.left(), h);
                painter.drawLine(w - m.right(), 0, w - m.right(), h);
            }
            if (m.bottom() != -1) {
                painter.drawLine(0, m.top(), w, m.top());
                painter.drawLine(0, h - m.bottom(), w, h - m.bottom());
            }
        }
    }
#endif

    update();
}

void QQuickStyleItem::updatePolish()
{
    QScopedValueRollback<bool> guard(m_polishing, true);

    const bool dirtyGeometry = m_dirty & DirtyFlag::Geometry;
    const bool dirtyImage = (m_dirty & DirtyFlag::Image) || (!m_useNinePatchImage && dirtyGeometry);

    if (dirtyGeometry)
        updateGeometry();
    if (dirtyImage)
        paintControlToImage();
}

#ifdef QT_DEBUG
void QQuickStyleItem::addDebugInfo()
{
    // Example debug strings:
    // "QQC2_NATIVESTYLE_DEBUG="myButton output contentRect"
    // "QQC2_NATIVESTYLE_DEBUG="ComboBox ninepatchmargins"
    // "QQC2_NATIVESTYLE_DEBUG="All layoutrect"

    static const auto debugString = qEnvironmentVariable("QQC2_NATIVESTYLE_DEBUG");
    static const auto matchAll = debugString.startsWith(QLatin1String("All "));
    static const auto prefix = QStringLiteral("QQuickStyleItem");
    if (debugString.isEmpty())
        return;

    const auto objectName = m_control->objectName();
    const auto typeName = QString::fromUtf8(metaObject()->className()).remove(prefix);
    const bool matchName = !objectName.isEmpty() && debugString.startsWith(objectName);
    const bool matchType = debugString.startsWith(typeName);

    if (!(matchAll || matchName || matchType))
        return;

#define QQC2_DEBUG_FLAG(FLAG) \
    if (debugString.contains(QLatin1String(#FLAG), Qt::CaseInsensitive)) m_debugFlags |= FLAG

    QQC2_DEBUG_FLAG(Output);
    QQC2_DEBUG_FLAG(ImageRect);
    QQC2_DEBUG_FLAG(ContentRect);
    QQC2_DEBUG_FLAG(LayoutRect);
    QQC2_DEBUG_FLAG(InputContentSize);
    QQC2_DEBUG_FLAG(DontUseNinePatchImage);
    QQC2_DEBUG_FLAG(NinePatchMargins);
    QQC2_DEBUG_FLAG(Unscaled);

    if (m_debugFlags & (DontUseNinePatchImage
                        | InputContentSize
                        | ContentRect
                        | LayoutRect
                        | NinePatchMargins)) {
        // Some rects will not fit inside the drawn image unless
        // we switch off (nine patch) image scaling.
        m_debugFlags |= DontUseNinePatchImage;
        m_useNinePatchImage = false;
    }

    if (m_debugFlags != NoDebug)
        qDebug() << "debug options set for" << typeName << "(" << objectName << "):" << m_debugFlags;
    else
        qDebug() << "available debug options:" << DebugFlags(0xFFFF);
}
#endif

void QQuickStyleItem::componentComplete()
{
    Q_ASSERT_X(m_control, Q_FUNC_INFO, "You need to assign a value to property 'control'");
#ifdef QT_DEBUG
    addDebugInfo();
#endif
    QQuickItem::componentComplete();
    connectToControl();
    polish();
}

qreal QQuickStyleItem::contentWidth()
{
    return m_contentSize.width();
}

void QQuickStyleItem::setContentWidth(qreal contentWidth)
{
    if (qFuzzyCompare(m_contentSize.width(), contentWidth))
        return;

    m_contentSize.setWidth(contentWidth);
    markGeometryDirty();
}

qreal QQuickStyleItem::contentHeight()
{
    return m_contentSize.height();
}

void QQuickStyleItem::setContentHeight(qreal contentHeight)
{
    if (qFuzzyCompare(m_contentSize.height(), contentHeight))
        return;

    m_contentSize.setHeight(contentHeight);
    markGeometryDirty();
}

QQuickStyleMargins QQuickStyleItem::contentPadding() const
{
    const QRect outerRect(QPoint(0, 0), m_styleItemGeometry.implicitSize);
    return QQuickStyleMargins(outerRect, m_styleItemGeometry.contentRect);
}

QQuickStyleMargins QQuickStyleItem::layoutMargins() const
{
    // ### TODO: layoutRect is currently not being used for anything. But
    // eventually this information will be needed by layouts to align the controls
    // correctly. This because the images drawn by QStyle are usually a bit bigger
    // than the control(frame) itself, to e.g make room for shadow effects
    // or focus rects/glow. And this will differ from control to control. The
    // layoutRect will then inform where the frame of the control is.
    const QRect outerRect(QPoint(0, 0), m_styleItemGeometry.implicitSize);
    return QQuickStyleMargins(outerRect, m_styleItemGeometry.layoutRect);
}

QSize QQuickStyleItem::minimumSize() const
{
    // The style item should not be scaled below this size.
    // Otherwise the image will be truncated.
    return m_styleItemGeometry.minimumSize;
}

QSize QQuickStyleItem::imageSize() const
{
    // Returns the size of the QImage (unscaled) that
    // is used to draw the control from QStyle.
    return m_useNinePatchImage ? m_styleItemGeometry.minimumSize : size().toSize();
}

qreal QQuickStyleItem::focusFrameRadius() const
{
    return m_styleItemGeometry.focusFrameRadius;
}

QFont QQuickStyleItem::styleFont(QQuickItem *control)
{
    Q_ASSERT(control);
    // Note: This function should be treated as if it was static
    // (meaning, don't assume anything in this object to be initialized).
    // Resolving the font/font size should be done early on from QML, before we get
    // around to calculate geometry and paint. Otherwise we typically need to do it
    // all over again when/if the font changes. In practice this means that other
    // items in QML that uses a style font, and at the same time, affects our input
    // contentSize, cannot wait for this item to be fully constructed before it
    // gets the font. So we need to resolve it here and now, even if this
    // object might be in a half initialized state (hence also the control
    // argument, instead of relying on m_control to be set).
    return QGuiApplication::font();
}

QT_END_NAMESPACE
