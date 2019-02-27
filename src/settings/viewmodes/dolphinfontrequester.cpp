/***************************************************************************
 *   Copyright (C) 2008 by Peter Penz <peter.penz19@gmail.com>             *
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

#include "dolphinfontrequester.h"

#include <KLocalizedString>

#include <QComboBox>
#include <QFontDatabase>
#include <QFontDialog>
#include <QHBoxLayout>
#include <QPushButton>

DolphinFontRequester::DolphinFontRequester(QWidget* parent) :
    QWidget(parent),
    m_modeCombo(nullptr),
    m_chooseFontButton(nullptr),
    m_mode(SystemFont),
    m_customFont()
{
    QHBoxLayout* topLayout = new QHBoxLayout(this);
    topLayout->setContentsMargins(0, 0, 0, 0);

    m_modeCombo = new QComboBox(this);
    m_modeCombo->addItem(i18nc("@item:inlistbox Font", "System Font"));
    m_modeCombo->addItem(i18nc("@item:inlistbox Font", "Custom Font"));
    connect(m_modeCombo, static_cast<void(QComboBox::*)(int)>(&QComboBox::activated),
            this, &DolphinFontRequester::changeMode);

    m_chooseFontButton = new QPushButton(i18nc("@action:button Choose font", "Choose..."), this);
    connect(m_chooseFontButton, &QPushButton::clicked,
            this, &DolphinFontRequester::openFontDialog);

    changeMode(m_modeCombo->currentIndex());

    topLayout->addWidget(m_modeCombo);
    topLayout->addWidget(m_chooseFontButton);
}

DolphinFontRequester::~DolphinFontRequester()
{
}

void DolphinFontRequester::setMode(Mode mode)
{
    m_mode = mode;
    m_modeCombo->setCurrentIndex(m_mode);
    m_chooseFontButton->setEnabled(m_mode == CustomFont);
}

DolphinFontRequester::Mode DolphinFontRequester::mode() const
{
    return m_mode;
}

QFont DolphinFontRequester::currentFont() const
{
    return (m_mode == CustomFont) ? m_customFont : QFontDatabase::systemFont(QFontDatabase::GeneralFont);
}

void DolphinFontRequester::setCustomFont(const QFont& font)
{
    m_customFont = font;
}

QFont DolphinFontRequester::customFont() const
{
    return m_customFont;
}

void DolphinFontRequester::openFontDialog()
{
    bool ok = false;
    const QFont font = QFontDialog::getFont(&ok, this);
    if (ok) {
        m_customFont = font;
        m_modeCombo->setFont(m_customFont);
        emit changed();
    }
}

void DolphinFontRequester::changeMode(int index)
{
    setMode((index == CustomFont) ? CustomFont : SystemFont);
    emit changed();
}

