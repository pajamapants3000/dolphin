/***************************************************************************
 *   Copyright (C) 2011 by Peter Penz <peter.penz19@gmail.com>             *
 *   Copyright (C) 2011 by Frank Reininghaus <frank78ac@googlemail.com>    *
 *                                                                         *
 *   Based on the Itemviews NG project from Trolltech Labs:                *
 *   http://qt.gitorious.org/qt-labs/itemviews-ng                          *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA            *
 ***************************************************************************/

#include "kitemlistselectionmanager.h"

KItemListSelectionManager::KItemListSelectionManager(QObject* parent) :
    QObject(parent),
    m_currentItem(-1),
    m_anchorItem(-1),
    m_selectedItems(),
    m_isAnchoredSelectionActive(false),
    m_model(nullptr)
{
}

KItemListSelectionManager::~KItemListSelectionManager()
{
}

void KItemListSelectionManager::setCurrentItem(int current)
{
    const int previous = m_currentItem;
    const KItemSet previousSelection = selectedItems();

    if (m_model && current >= 0 && current < m_model->count()) {
        m_currentItem = current;
    } else {
        m_currentItem = -1;
    }

    if (m_currentItem != previous) {
        emit currentChanged(m_currentItem, previous);

        if (m_isAnchoredSelectionActive) {
            const KItemSet selection = selectedItems();
            if (selection != previousSelection) {
                emit selectionChanged(selection, previousSelection);
            }
        }
    }
}

int KItemListSelectionManager::currentItem() const
{
    return m_currentItem;
}

void KItemListSelectionManager::setSelectedItems(const KItemSet& items)
{
    if (m_selectedItems != items) {
        const KItemSet previous = m_selectedItems;
        m_selectedItems = items;
        emit selectionChanged(m_selectedItems, previous);
    }
}

KItemSet KItemListSelectionManager::selectedItems() const
{
    KItemSet selectedItems = m_selectedItems;

    if (m_isAnchoredSelectionActive && m_anchorItem != m_currentItem) {
        Q_ASSERT(m_anchorItem >= 0);
        Q_ASSERT(m_currentItem >= 0);
        const int from = qMin(m_anchorItem, m_currentItem);
        const int to = qMax(m_anchorItem, m_currentItem);

        for (int index = from; index <= to; ++index) {
            selectedItems.insert(index);
        }
    }

    return selectedItems;
}

bool KItemListSelectionManager::isSelected(int index) const
{
    if (m_selectedItems.contains(index)) {
        return true;
    }

    if (m_isAnchoredSelectionActive && m_anchorItem != m_currentItem) {
        Q_ASSERT(m_anchorItem >= 0);
        Q_ASSERT(m_currentItem >= 0);
        const int from = qMin(m_anchorItem, m_currentItem);
        const int to = qMax(m_anchorItem, m_currentItem);

        if (from <= index && index <= to) {
            return true;
        }
    }

    return false;
}

bool KItemListSelectionManager::hasSelection() const
{
    return !m_selectedItems.isEmpty() || (m_isAnchoredSelectionActive && m_anchorItem != m_currentItem);
}

void KItemListSelectionManager::setSelected(int index, int count, SelectionMode mode)
{
    if (index < 0 || count < 1 || !m_model || index >= m_model->count()) {
        return;
    }

    endAnchoredSelection();
    const KItemSet previous = selectedItems();

    count = qMin(count, m_model->count() - index);

    const int endIndex = index + count -1;
    switch (mode) {
    case Select:
        for (int i = index; i <= endIndex; ++i) {
            m_selectedItems.insert(i);
        }
        break;

    case Deselect:
        for (int i = index; i <= endIndex; ++i) {
            m_selectedItems.remove(i);
        }
        break;

    case Toggle:
        for (int i = index; i <= endIndex; ++i) {
            if (m_selectedItems.contains(i)) {
                m_selectedItems.remove(i);
            } else {
                m_selectedItems.insert(i);
            }
        }
        break;

    default:
        Q_ASSERT(false);
        break;
    }

    const KItemSet selection = selectedItems();
    if (selection != previous) {
        emit selectionChanged(selection, previous);
    }
}

void KItemListSelectionManager::clearSelection()
{
    const KItemSet previous = selectedItems();
    if (!previous.isEmpty()) {
        m_selectedItems.clear();
        m_isAnchoredSelectionActive = false;
        emit selectionChanged(KItemSet(), previous);
    }
}

void KItemListSelectionManager::replaceSelection(int index, int count)
{
    const KItemSet previous = selectedItems();
    if (!previous.isEmpty()) {
        m_selectedItems.clear();
        m_isAnchoredSelectionActive = false;
    }
    setSelected(index, count);
}

void KItemListSelectionManager::beginAnchoredSelection(int anchor)
{
    if (anchor >= 0 && m_model && anchor < m_model->count()) {
        m_isAnchoredSelectionActive = true;
        m_anchorItem = anchor;
    }
}

void KItemListSelectionManager::endAnchoredSelection()
{
    if (m_isAnchoredSelectionActive && (m_anchorItem != m_currentItem)) {
        Q_ASSERT(m_anchorItem >= 0);
        Q_ASSERT(m_currentItem >= 0);
        const int from = qMin(m_anchorItem, m_currentItem);
        const int to = qMax(m_anchorItem, m_currentItem);

        for (int index = from; index <= to; ++index) {
            m_selectedItems.insert(index);
        }
    }

    m_isAnchoredSelectionActive = false;
}

bool KItemListSelectionManager::isAnchoredSelectionActive() const
{
    return m_isAnchoredSelectionActive;
}

KItemModelBase* KItemListSelectionManager::model() const
{
    return m_model;
}

void KItemListSelectionManager::setModel(KItemModelBase* model)
{
    m_model = model;
    if (model && model->count() > 0) {
        m_currentItem = 0;
    }
}

void KItemListSelectionManager::itemsInserted(const KItemRangeList& itemRanges)
{
    // Store the current selection (needed in the selectionChanged() signal)
    const KItemSet previousSelection = selectedItems();

    // Update the current item
    if (m_currentItem < 0) {
        setCurrentItem(0);
    } else {
        const int previousCurrent = m_currentItem;
        int inc = 0;
        foreach (const KItemRange& itemRange, itemRanges) {
            if (m_currentItem < itemRange.index) {
                break;
            }
            inc += itemRange.count;
        }
        // Calling setCurrentItem would trigger the selectionChanged signal, but we want to
        // emit it only once in this function -> change the current item manually and emit currentChanged
        m_currentItem += inc;
        if (m_currentItem >= m_model->count()) {
            m_currentItem = -1;
        }
        emit currentChanged(m_currentItem, previousCurrent);
    }

    // Update the anchor item
    if (m_anchorItem < 0) {
        m_anchorItem = 0;
    } else {
        int inc = 0;
        foreach (const KItemRange& itemRange, itemRanges) {
            if (m_anchorItem < itemRange.index) {
                break;
            }
            inc += itemRange.count;
        }
        m_anchorItem += inc;
    }

    // Update the selections
    if (!m_selectedItems.isEmpty()) {
        const KItemSet previous = m_selectedItems;
        m_selectedItems.clear();

        for (int index: previous) {
            int inc = 0;
            foreach (const KItemRange& itemRange, itemRanges) {
                if (index < itemRange.index) {
                    break;
                }
                inc += itemRange.count;
            }
            m_selectedItems.insert(index + inc);
        }
    }

    const KItemSet selection = selectedItems();
    if (selection != previousSelection) {
        emit selectionChanged(selection, previousSelection);
    }
}

void KItemListSelectionManager::itemsRemoved(const KItemRangeList& itemRanges)
{
    // Store the current selection (needed in the selectionChanged() signal)
    const KItemSet previousSelection = selectedItems();
    const int previousCurrent = m_currentItem;

    // Update the current item
    m_currentItem = indexAfterRangesRemoving(m_currentItem, itemRanges, DiscardRemovedIndex);
    if (m_currentItem != previousCurrent) {
        emit currentChanged(m_currentItem, previousCurrent);
        if (m_currentItem < 0) {
            // Calling setCurrentItem() would trigger the selectionChanged signal, but we want to
            // emit it only once in this function -> change the current item manually and emit currentChanged
            m_currentItem = indexAfterRangesRemoving(previousCurrent, itemRanges, AdjustRemovedIndex);
            emit currentChanged(m_currentItem, -1);
        }
    }

    // Update the anchor item
    if (m_anchorItem >= 0) {
        m_anchorItem = indexAfterRangesRemoving(m_anchorItem, itemRanges, DiscardRemovedIndex);
        if (m_anchorItem < 0) {
            m_isAnchoredSelectionActive = false;
        }
    }

    // Update the selections and the anchor item
    if (!m_selectedItems.isEmpty()) {
        const KItemSet previous = m_selectedItems;
        m_selectedItems.clear();

        for (int oldIndex : previous) {
            const int index = indexAfterRangesRemoving(oldIndex, itemRanges, DiscardRemovedIndex);
            if (index >= 0)  {
                m_selectedItems.insert(index);
            }
        }
    }

    const KItemSet selection = selectedItems();
    if (selection != previousSelection) {
        emit selectionChanged(selection, previousSelection);
    }

    Q_ASSERT(m_currentItem < m_model->count());
    Q_ASSERT(m_anchorItem < m_model->count());
}

void KItemListSelectionManager::itemsMoved(const KItemRange& itemRange, const QList<int>& movedToIndexes)
{
    // Store the current selection (needed in the selectionChanged() signal)
    const KItemSet previousSelection = selectedItems();

    // Store whether we were doing an anchored selection
    const bool wasInAnchoredSelection = isAnchoredSelectionActive();

    // endAnchoredSelection() adds all items between m_currentItem and
    // m_anchorItem to m_selectedItems. They can then be moved
    // individually later in this function.
    endAnchoredSelection();

    // Update the current item
    if (m_currentItem >= itemRange.index && m_currentItem < itemRange.index + itemRange.count) {
        const int previousCurrentItem = m_currentItem;
        const int newCurrentItem = movedToIndexes.at(previousCurrentItem - itemRange.index);

        // Calling setCurrentItem would trigger the selectionChanged signal, but we want to
        // emit it only once in this function -> change the current item manually and emit currentChanged
        m_currentItem = newCurrentItem;
        emit currentChanged(newCurrentItem, previousCurrentItem);
    }

    // Start a new anchored selection.
    if (wasInAnchoredSelection) {
        beginAnchoredSelection(m_currentItem);
    }

    // Update the selections
    if (!m_selectedItems.isEmpty()) {
        const KItemSet previous = m_selectedItems;
        m_selectedItems.clear();

        for (int index : previous) {
            if (index >= itemRange.index && index < itemRange.index + itemRange.count) {
                m_selectedItems.insert(movedToIndexes.at(index - itemRange.index));
            }
            else {
                m_selectedItems.insert(index);
            }
        }
    }

    const KItemSet selection = selectedItems();
    if (selection != previousSelection) {
        emit selectionChanged(selection, previousSelection);
    }
}

int KItemListSelectionManager::indexAfterRangesRemoving(int index, const KItemRangeList& itemRanges,
                                                        const RangesRemovingBehaviour behaviour) const
{
    int dec = 0;
    foreach (const KItemRange& itemRange, itemRanges) {
        if (index < itemRange.index) {
            break;
        }

        dec += itemRange.count;

        const int firstIndexAfterRange = itemRange.index + itemRange.count;
        if (index < firstIndexAfterRange) {
            // The index is part of the removed range
            if (behaviour == DiscardRemovedIndex) {
                return -1;
            } else {
                // Use the first item after the range as new index
                index = firstIndexAfterRange;
                break;
            }
        }
    }
    return qBound(-1, index - dec, m_model->count() - 1);
}

