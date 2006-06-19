/* This file is part of the KDE project
   Copyright (C) 2000 Malte Starostik <malte@kde.org>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

#include "konq_faviconmgr.moc"

#include <kglobal.h>
#include <kmimetype.h>
#include <dbus/qdbus.h>
#include "konq_faviconmgr.h"

#define FAVICONINTERFACE "org.kde.FavIcon"

KonqFavIconMgr::KonqFavIconMgr(QObject *parent)
    : QObject(parent)
{
     m_favIconsModule =
         QDBus::sessionBus().findInterface("org.kde.kded", "/modules/favicons",
                                           FAVICONINTERFACE);
     if ( !m_favIconsModule->isValid() )
         kWarning() << "kded not running?" << endl;
     else
         connect( m_favIconsModule, SIGNAL(iconChanged(bool,QString,QString)),
                  this, SLOT(notifyChange(bool,QString,QString)));
}

KonqFavIconMgr::~KonqFavIconMgr()
{
    delete m_favIconsModule;
}

void KonqFavIconMgr::setIconForURL(const KUrl &url, const KUrl &iconURL)
{
    QDBusInterfacePtr favicon("org.kde.kded", "/modules/favicons", FAVICONINTERFACE);
    favicon->call(QDBusInterface::NoWaitForReply, "setIconForURL", url.url(), iconURL.url());
}

void KonqFavIconMgr::downloadHostIcon(const KUrl &url)
{
    QDBusInterfacePtr favicon("org.kde.kded", "/modules/favicons", FAVICONINTERFACE);
    favicon->call(QDBusInterface::NoWaitForReply, "downloadHostIcon", url.url());
}
