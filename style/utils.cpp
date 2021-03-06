/*
  QtCurve (C) Craig Drummond, 2007 - 2010 craig.p.drummond@gmail.com

  ----

  This program is free software; you can redistribute it and/or
  modify it under the terms of the GNU General Public
  License version 2 as published by the Free Software Foundation.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; see the file COPYING.  If not, write to
  the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
  Boston, MA 02110-1301, USA.
*/

#include "utils.h"
#ifdef QTC_X11
#  include "xcb_utils.h"
#include <QApplication>
#include <QDesktopWidget>
#endif
#include <stdio.h>

#if defined QTC_QT_ONLY
#undef KDE_IS_VERSION
#define KDE_IS_VERSION(A, B, C) 0
#else
#include <kdeversion.h>
#include <KDE/KWindowSystem>
#endif

namespace QtCurve
{
namespace Utils
{

bool compositingActive()
{
#if defined QTC_QT_ONLY
#ifdef QTC_X11
    static xcb_atom_t atom;
    if (!atom) {
        char atomName[100] = "_NET_WM_CM_S";
        size_t len = strlen("_NET_WM_CM_S");
        len += sprintf(atomName + len, "%d",
                       QApplication::desktop()->primaryScreen());
        atom = XcbUtils::getAtom(atomName);
        if (!atom) {
            return false;
        }
    }
    auto reply = XcbCall(get_selection_owner, atom);
    bool res = false;
    if (reply) {
        res = reply->owner != 0;
        free(reply);
    }
    return res;
#else // QTC_X11
    return false;
#endif // QTC_X11
#else // QTC_QT_ONLY
    return KWindowSystem::compositingActive();
#endif // QTC_QT_ONLY
}

bool hasAlphaChannel(const QWidget *widget)
{
#ifdef QTC_X11
    if (compositingActive()) {
        WId wid = widget ? widget->window()->winId() : 0;
        if (wid) {
            auto reply = XcbCall(get_geometry, wid);
            bool res = false;
            if (reply) {
                res = reply->depth == 32;
                free(reply);
            }
            return res;
        }
        return true;
    } else {
        return false;
    }
#else
    Q_UNUSED(widget);
    return compositingActive();
#endif
}

}
}
