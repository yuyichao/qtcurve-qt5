/*
  QtCurve KWin window decoration
  Copyright (C) 2007 Craig Drummond <Craig.Drummond@lycos.co.uk>

  based on the window decoration "Plastik":
  Copyright (C) 2003-2005 Sandro Giessl <sandro@giessl.com>

  based on the window decoration "Web":
  Copyright (C) 2001 Rik Hemsley (rikkus) <rik@kde.org>

  This program is free software; you can redistribute it and/or
  modify it under the terms of the GNU General Public
  License as published by the Free Software Foundation; either
  version 2 of the License, or (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; see the file COPYING.  If not, write to
  the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
  Boston, MA 02110-1301, USA.
 */

#include <klocale.h>
#include <QBitmap>
#include <QDateTime>
#include <QFontMetrics>
#include <QImage>
#include <QLabel>
#include <QLayout>
#include <QPainter>
#include <QPixmap>
#include <QStyleOptionTitleBar>
#include <QStyle>
#include <qdesktopwidget.h>
#include "qtcurvehandler.h"
#include "qtcurveclient.h"
#include "qtcurvebutton.h"

namespace KWinQtCurve
{

QtCurveClient::QtCurveClient(KDecorationBridge *bridge, KDecorationFactory *factory)
             : KCommonDecoration (bridge, factory),
               itsTitleFont(QFont())
{
}

QString QtCurveClient::visibleName() const
{
    return i18n("QtCurve");
}

bool QtCurveClient::decorationBehaviour(DecorationBehaviour behaviour) const
{
    switch (behaviour)
    {
        case DB_MenuClose:
            return true; // Handler()->menuClose();
        case DB_WindowMask:
            return true;
        default:
            return KCommonDecoration::decorationBehaviour(behaviour);
    }
}

int QtCurveClient::layoutMetric(LayoutMetric lm, bool respectWindowState,
                                const KCommonDecorationButton *btn) const
{
    bool maximized = maximizeMode()==MaximizeFull && !options()->moveResizeMaximizedWindows();

    switch (lm)
    {
        case LM_BorderLeft:
        case LM_BorderRight:
        case LM_BorderBottom:
            return respectWindowState && maximized ? 0 : Handler()->borderSize();
        case LM_TitleEdgeTop:
            return respectWindowState && maximized ? 0 : 3;
        case LM_TitleEdgeBottom:
            return /*respectWindowState && maximized ? 1 : */ 3;
        case LM_TitleEdgeLeft:
        case LM_TitleEdgeRight:
            return respectWindowState && maximized ? 0 : 3;
        case LM_TitleBorderLeft:
        case LM_TitleBorderRight:
            return 5;
        case LM_ButtonWidth:
        case LM_ButtonHeight:
        case LM_TitleHeight:
            return respectWindowState && isToolWindow() ? Handler()->titleHeightTool() : Handler()->titleHeight();
        case LM_ButtonSpacing:
            return 1;
        case LM_ButtonMarginTop:
            return 0;
        default:
            return KCommonDecoration::layoutMetric(lm, respectWindowState, btn);
    }
}

KCommonDecorationButton *QtCurveClient::createButton(ButtonType type)
{
    switch (type)
    {
        case MenuButton:
            return new QtCurveButton(MenuButton, this);
        case OnAllDesktopsButton:
            return new QtCurveButton(OnAllDesktopsButton, this);
        case HelpButton:
            return new QtCurveButton(HelpButton, this);
        case MinButton:
            return new QtCurveButton(MinButton, this);
        case MaxButton:
            return new QtCurveButton(MaxButton, this);
        case CloseButton:
            return new QtCurveButton(CloseButton, this);
        case AboveButton:
            return new QtCurveButton(AboveButton, this);
        case BelowButton:
            return new QtCurveButton(BelowButton, this);
        case ShadeButton:
            return new QtCurveButton(ShadeButton, this);
        default:
            return 0;
    }
}

void QtCurveClient::init()
{
    itsTitleFont = isToolWindow() ? Handler()->titleFontTool() : Handler()->titleFont();

    KCommonDecoration::init();
}

QRegion QtCurveClient::cornerShape(WindowCorner corner)
{
    int w(widget()->width()),
        h(widget()->height());

    switch (corner)
    {
        case WC_TopLeft:
            if (layoutMetric(LM_TitleEdgeLeft) > 0)
                return QRegion(0, 0, 1, 2) + QRegion(1, 0, 1, 1);
            else
                return QRegion();
        case WC_TopRight:
            if (layoutMetric(LM_TitleEdgeRight) > 0)
                return QRegion(w-1, 0, 1, 2) + QRegion(w-2, 0, 1, 1);
            else
                return QRegion();
        case WC_BottomLeft:
            if (layoutMetric(LM_BorderBottom) > 0)
                return QRegion(0, h-1, 1, 1);
            else
                return QRegion();
        case WC_BottomRight:
            if (layoutMetric(LM_BorderBottom) > 0)
                return QRegion(w-1, h-1, 1, 1);
            else
                return QRegion();
        default:
            return QRegion();
    }
}

void QtCurveClient::drawBtnBgnd(QPainter *p, const QRect &r, bool active)
{
    QRect                br(r);
    QStyleOptionTitleBar opt;

    br.adjust(-3, -3, 3, 3);
    opt.rect=br;

    opt.state=QStyle::State_Horizontal|QStyle::State_Enabled|QStyle::State_Raised|
             (active ? QStyle::State_Active : QStyle::State_None);
    opt.titleBarState=(active ? QStyle::State_Active : QStyle::State_None);
    Handler()->wStyle()->drawComplexControl(QStyle::CC_TitleBar, &opt, p, widget());
}

void QtCurveClient::paintEvent(QPaintEvent *e)
{
    QRegion              region(e->region());
    QPainter             painter(widget());
    QRect                r(widget()->rect());
    QStyleOptionTitleBar opt;
    bool                 active(isActive());
    const int            titleHeight = layoutMetric(LM_TitleHeight),
                         titleEdgeTop = layoutMetric(LM_TitleEdgeTop),
                         titleEdgeBottom = layoutMetric(LM_TitleEdgeBottom),
                         titleEdgeLeft = layoutMetric(LM_TitleEdgeLeft),
                         titleEdgeRight = layoutMetric(LM_TitleEdgeRight);
    int                  rectX, rectY, rectX2, rectY2;

    r.getCoords(&rectX, &rectY, &rectX2, &rectY2);

    const int titleEdgeBottomBottom = rectY+titleEdgeTop+titleHeight+titleEdgeBottom-1;
    QRect     titleRect(rectX+titleEdgeLeft+buttonsLeftWidth(), rectY+titleEdgeTop,
                        rectX2-titleEdgeRight-buttonsRightWidth()-(rectX+titleEdgeLeft+buttonsLeftWidth()),
                        titleEdgeBottomBottom-(rectY+titleEdgeTop));

    painter.setClipRegion(region);
    painter.fillRect(r.adjusted(1, 1, -1, -1), widget()->palette().background());
    opt.init(widget());

    if(MaximizeFull==maximizeMode())
        r.adjust(-3, -3, 3, 0);
    opt.rect=QRect(r.x(), r.y()+4, r.width(), r.height()-4);
    opt.state=QStyle::State_Horizontal|QStyle::State_Enabled|QStyle::State_Raised|
             (active ? QStyle::State_Active : QStyle::State_None);

    Handler()->wStyle()->drawPrimitive(QStyle::PE_FrameWindow, &opt, &painter, widget());

    opt.rect=QRect(r.x(), r.y(), r.width(), titleHeight+titleEdgeTop+titleEdgeBottom+
                                            (MaximizeFull==maximizeMode() ? 3 : 0));
    opt.titleBarState=(active ? QStyle::State_Active : QStyle::State_None);
    Handler()->wStyle()->drawComplexControl(QStyle::CC_TitleBar, &opt, &painter, widget());

    itsCaptionRect = captionRect(); // also update itsCaptionRect!

    if(!caption().isEmpty())
    {
        const int maxCaptionLength = 300; // truncate captions longer than this!
        QString c(caption());
        if (c.length() > maxCaptionLength)
        {
            c.truncate(maxCaptionLength);
            c.append(" [...]");
        }

        QFontMetrics fm(itsTitleFont);
        int          captionHeight(fm.height());

        painter.setFont(itsTitleFont);
        QPoint tp(itsCaptionRect.x(), itsCaptionRect.y()+captionHeight-3);

        painter.setPen(shadowColor(KDecoration::options()->color(KDecoration::ColorFont, active)));
        painter.drawText(tp+QPoint(1, 1), c);
        painter.setPen(KDecoration::options()->color(KDecoration::ColorFont, active));
        painter.drawText(tp, c);
    }
    painter.setClipping(false);
    painter.end();
}

QRect QtCurveClient::captionRect() const
{
    QRect r = widget()->rect();

    const int titleHeight = layoutMetric(LM_TitleHeight),
              titleEdgeBottom = layoutMetric(LM_TitleEdgeBottom),
              titleEdgeTop = layoutMetric(LM_TitleEdgeTop),
              titleEdgeLeft = layoutMetric(LM_TitleEdgeLeft),
              marginLeft = layoutMetric(LM_TitleBorderLeft),
              marginRight = layoutMetric(LM_TitleBorderRight),
              titleLeft = r.left() + titleEdgeLeft + buttonsLeftWidth() + marginLeft,
              titleWidth = r.width() -
                           titleEdgeLeft - layoutMetric(LM_TitleEdgeRight) -
                           buttonsLeftWidth() - buttonsRightWidth() -
                           marginLeft - marginRight;

    return QRect(titleLeft, r.top()+titleEdgeTop, titleWidth, titleHeight+titleEdgeBottom);
}

void QtCurveClient::updateCaption()
{
    QRect oldCaptionRect(itsCaptionRect);

    itsCaptionRect = QtCurveClient::captionRect();

    if (oldCaptionRect.isValid() && itsCaptionRect.isValid())
        widget()->update(oldCaptionRect|itsCaptionRect);
    else
        widget()->update();
}

void QtCurveClient::reset(unsigned long changed)
{
    if (changed & SettingColors)
    {
        // repaint the whole thing
        widget()->update();
        updateButtons();
    }
    else if (changed & SettingFont)
    {
        // font has changed -- update title height and font
        itsTitleFont = isToolWindow() ? Handler()->titleFontTool() : Handler()->titleFont();

        updateLayout();
        widget()->update();
    }

    KCommonDecoration::reset(changed);
}

}
