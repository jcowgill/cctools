/******************************************************************************
 * This file is part of CCTools.                                              *
 *                                                                            *
 * CCTools is free software: you can redistribute it and/or modify            *
 * it under the terms of the GNU General Public License as published by       *
 * the Free Software Foundation, either version 3 of the License, or          *
 * (at your option) any later version.                                        *
 *                                                                            *
 * CCTools is distributed in the hope that it will be useful,                 *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of             *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the              *
 * GNU General Public License for more details.                               *
 *                                                                            *
 * You should have received a copy of the GNU General Public License          *
 * along with CCTools.  If not, see <http://www.gnu.org/licenses/>.           *
 ******************************************************************************/

#include "TestSetup.h"

#include <QSettings>
#include <QLabel>
#include <QDialogButtonBox>
#include <QToolButton>
#include <QGridLayout>
#include <QCompleter>
#include <QDirModel>
#include <QFileDialog>

#ifdef Q_OS_WIN
    #define POSIX_OFFSET 0
    #define EXE_FILTER "Executables (*.exe)"
    #define WINEXE_FILTER "Executables (*.exe)"
    #define EXE_LIST QStringList() << "*.exe"
#else
    #define POSIX_OFFSET 1
    #define EXE_FILTER "Executables (*)"
    #define WINEXE_FILTER "Windows Executables (*.exe *.EXE)"
    #define EXE_LIST QStringList()
#endif

TestSetupDialog::TestSetupDialog(QWidget* parent)
               : QDialog(parent)
{
    setWindowTitle(tr("Setup testing parameters"));

    QSettings settings("CCTools", "CCEdit");
    QCompleter* exeCompleter = new QCompleter(this);
    exeCompleter->setModel(new QDirModel(EXE_LIST,
            QDir::AllDirs | QDir::AllEntries | QDir::NoDotAndDotDot | QDir::Executable,
            QDir::Name, exeCompleter));
    QCompleter* winExeCompleter = new QCompleter(this);
    winExeCompleter->setModel(new QDirModel(QStringList() << "*.exe",
            QDir::AllDirs | QDir::AllEntries | QDir::NoDotAndDotDot,
            QDir::Name, winExeCompleter));

#ifndef Q_OS_WIN
    m_winePath = new QLineEdit(settings.value("WineExe").toString(), this);
    m_winePath->setCompleter(exeCompleter);
    QLabel* lblWinePath = new QLabel(tr("&WINE Path:"), this);
    lblWinePath->setBuddy(m_winePath);
    QToolButton* browseWine = new QToolButton(this);
    browseWine->setIcon(QIcon(":/res/document-open-folder-sm.png"));
    browseWine->setAutoRaise(true);
#endif

    m_msccPath = new QLineEdit(settings.value("ChipsExe").toString(), this);
    m_msccPath->setCompleter(winExeCompleter);
    QLabel* lblMsccPath = new QLabel(tr("MS&CC Path:"), this);
    lblMsccPath->setBuddy(m_msccPath);
    QToolButton* browseChips = new QToolButton(this);
    browseChips->setIcon(QIcon(":/res/document-open-folder-sm.png"));
    browseChips->setAutoRaise(true);
    m_tworldPath = new QLineEdit(settings.value("TWorldExe").toString(), this);
    m_tworldPath->setCompleter(exeCompleter);
    QLabel* lblTWorldPath = new QLabel(tr("&Tile World Path:"), this);
    lblTWorldPath->setBuddy(m_tworldPath);
    QToolButton* browseTWorld = new QToolButton(this);
    browseTWorld->setIcon(QIcon(":/res/document-open-folder-sm.png"));
    browseTWorld->setAutoRaise(true);
    QDialogButtonBox* buttons = new QDialogButtonBox(
            QDialogButtonBox::Save | QDialogButtonBox::Cancel,
            Qt::Horizontal, this);

    m_useCCPatch = new QCheckBox(tr("MSCC: Use CCPatch"), this);
    m_useCCPatch->setChecked(settings.value("TestCCPatch", true).toBool());
    m_usePGPatch = new QCheckBox(tr("MSCC: Use PGChip (Ice Blocks)"), this);
    m_usePGPatch->setChecked(settings.value("TestPGPatch", false).toBool());

    QGridLayout* layout = new QGridLayout(this);
    layout->setContentsMargins(8, 8, 8, 8);
    layout->setVerticalSpacing(4);
    layout->setHorizontalSpacing(4);
#ifndef Q_OS_WIN
    layout->addWidget(lblWinePath, 0, 0);
    layout->addWidget(m_winePath, 0, 1);
    layout->addWidget(browseWine, 0, 2);
#endif
    layout->addWidget(lblMsccPath, POSIX_OFFSET + 0, 0);
    layout->addWidget(m_msccPath, POSIX_OFFSET + 0, 1);
    layout->addWidget(browseChips, POSIX_OFFSET + 0, 2);
    layout->addWidget(m_useCCPatch, POSIX_OFFSET + 1, 1);
    layout->addWidget(m_usePGPatch, POSIX_OFFSET + 2, 1);
    layout->addWidget(lblTWorldPath, POSIX_OFFSET + 3, 0);
    layout->addWidget(m_tworldPath, POSIX_OFFSET + 3, 1);
    layout->addWidget(browseTWorld, POSIX_OFFSET + 3, 2);
#ifndef Q_OS_WIN
    layout->addWidget(new QLabel(
            tr("Note: Leave WINE or Tile World paths empty to use system-installed locations"),
            this), POSIX_OFFSET + 4, 0, 1, 3);
#else
    layout->addWidget(new QLabel(
            tr("Note: MSCC will not work on 64-bit Windows platforms"),
            this), POSIX_OFFSET + 4, 0, 1, 3);
#endif
    layout->addItem(new QSpacerItem(0, 0, QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding), POSIX_OFFSET + 5, 0, 1, 3);
    layout->addWidget(buttons, POSIX_OFFSET + 6, 0, 1, 3);
    resize(400, sizeHint().height());

    connect(buttons, SIGNAL(rejected()), SLOT(reject()));
    connect(buttons, SIGNAL(accepted()), SLOT(onSaveSettings()));
#ifndef Q_OS_WIN
    connect(browseWine, SIGNAL(clicked()), SLOT(onBrowseWine()));
#endif
    connect(browseChips, SIGNAL(clicked()), SLOT(onBrowseChips()));
    connect(browseTWorld, SIGNAL(clicked()), SLOT(onBrowseTWorld()));
}

void TestSetupDialog::onSaveSettings()
{
    QSettings settings("CCTools", "CCEdit");
#ifndef Q_OS_WIN
    settings.setValue("WineExe", m_winePath->text());
#endif
    settings.setValue("ChipsExe", m_msccPath->text());
    settings.setValue("TWorldExe", m_tworldPath->text());
    settings.setValue("TestCCPatch", m_useCCPatch->isChecked());
    settings.setValue("TestPGPatch", m_usePGPatch->isChecked());
    accept();
}

void TestSetupDialog::onBrowseWine()
{
#ifndef Q_OS_WIN
    QString path = QFileDialog::getOpenFileName(this, tr("Browse for Wine executable"),
                                m_winePath->text(), EXE_FILTER);
    if (!path.isEmpty())
        m_winePath->setText(path);
#else
    qCritical("onBrowseWine: Not supported on Windows platforms");
#endif
}

void TestSetupDialog::onBrowseChips()
{
    QString path = QFileDialog::getOpenFileName(this, tr("Browse for MSCC executable"),
                                m_msccPath->text(), WINEXE_FILTER);
    if (!path.isEmpty())
        m_msccPath->setText(path);
}

void TestSetupDialog::onBrowseTWorld()
{
    QString path = QFileDialog::getOpenFileName(this, tr("Browse for Tile World executable"),
                                m_tworldPath->text(), EXE_FILTER);
    if (!path.isEmpty())
        m_tworldPath->setText(path);
}
