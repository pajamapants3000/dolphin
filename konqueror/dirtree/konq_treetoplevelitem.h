/* This file is part of the KDE project
   Copyright (C) 2000 David Faure <faure@kde.org>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License version 2 as published by the Free Software Foundation.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#ifndef konq_treetoplevelitem_h
#define konq_treetoplevelitem_h

#include "konq_treeitem.h"
class KonqTreeModule;

/**
 * Each toplevel item (created from a desktop file)
 * points to the module that handles it
  --> this doesn't prevent the same module from handling multiple toplevel items,
  but we don't do that currently.
 */
class KonqTreeTopLevelItem : public KonqTreeItem
{
public:
    /**
     * Create a toplevel toplevel-item :)
     * @param module the module handling this toplevel item
     * @param path the path to the desktop file that was the reason for creating this item
     */
    KonqTreeTopLevelItem( KonqTree *parent, KonqTreeModule * module, const QString & path )
        : KonqTreeItem(parent, 0L), m_module(module), m_path(path), m_bTopLevelGroup(false) {}

    /**
     * Create a toplevel-item under a toplevel group
     * @param module the module handling this toplevel item
     * @param path the path to the desktop file that was the reason for creating this item
     */
    KonqTreeTopLevelItem( KonqTreeItem *parentItem, KonqTreeModule * module, const QString & path )
        : KonqTreeItem( parentItem, 0L), m_module(module), m_path(path), m_bTopLevelGroup(false) {}

    virtual bool acceptsDrops( const QStrList & formats );
    virtual void drop( QDropEvent * ev );
    virtual QDragObject * dragObject( QWidget * parent, bool move = false );
    virtual void middleButtonPressed();
    virtual void rightButtonPressed();

    virtual void paste();
    virtual void trash();
    virtual void del();
    virtual void shred();

    virtual void setOpen( bool open );

    // Whether the item is a toplevel item - true
    virtual bool isTopLevelItem() const { return true; }

    virtual KURL externalURL() const { return m_externalURL; }

    virtual void itemSelected();

    // The module should call this for each toplevel item that is passed to it
    // unless it calls setClickable(false)
    void setExternalURL( const KURL & url ) { m_externalURL = url; }

    // Whether the item is a toplevel group. [Only matters for dnd]
    void setTopLevelGroup( bool b ) { m_bTopLevelGroup = b; }
    bool isTopLevelGroup() const { return m_bTopLevelGroup; }

    // The module that handles the subtree below this toplevel item
    KonqTreeModule *module() const { return m_module; }

    // The path to the desktop file responsible for this toplevel item
    QString path() const { return m_path; }

protected:
    void delOperation( int method );
    KonqTreeModule *m_module;
    QString m_path;
    KURL m_externalURL;
    bool m_bTopLevelGroup;
};

#endif
