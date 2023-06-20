/****************************************************************************
**
** Copyright (C) 2016 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** BSD License Usage
** Alternatively, you may use this file under the terms of the BSD license
** as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of The Qt Company Ltd nor the names of its
**     contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include <QtWidgets>

#include "SettingsDialog.h"

//! [0]
SettingsDialog::SettingsDialog(const TabDialogDescriptor &Descriptor, QWidget *parent)
    : QDialog(parent)
{

    tabWidget = new QTabWidget;
    for (auto &Tab: Descriptor.Tabs) {
       tabWidget->addTab(new GeneralTab(Tab), QString::fromStdString(Tab.TabName));
    }
//! [0]

//! [1] //! [2]
    buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok
//! [1] //! [3]
                                     | QDialogButtonBox::Cancel);

    connect(buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);
//! [2] //! [3]

//! [4]
    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addWidget(tabWidget);
    mainLayout->addWidget(buttonBox);
    setLayout(mainLayout);
//! [4]

//! [5]
    setWindowTitle(QString::fromStdString(Descriptor.Title));
}
//! [5]
template<class... Ts>
struct overloaded : Ts... { using Ts::operator()...; };
// explicit deduction guide (not needed as of C++20)
template<class... Ts>
overloaded(Ts...) -> overloaded<Ts...>;



//! [6]
GeneralTab::GeneralTab(const TabDescriptor &Descriptor, QWidget *parent)
    : QWidget(parent)
{
    Descriptor.TabName;
    QVBoxLayout *mainLayout = new QVBoxLayout;

    for (auto &d: Descriptor.Entries) {
       // Switch on type in variant
       QLabel *NameLabel = new QLabel(QString::fromStdString(d.Title));
       std::visit(overloaded{
               [&](bool b){
                        QCheckBox *CheckBox = new QCheckBox(tr("Writable"));
                        if ( b )
                            CheckBox->setChecked(true);
                        CheckBox->setToolTip(QString::fromStdString(d.HelpText));
                        mainLayout->addWidget(NameLabel);
                        mainLayout->addWidget(CheckBox);
                     },
               [&](int i){
                        QLineEdit *Edit = new QLineEdit(QString::number(i));
                        Edit->setToolTip(QString::fromStdString(d.HelpText));
                        mainLayout->addWidget(NameLabel);
                        mainLayout->addWidget(Edit);
                     },
               [&](double f){
                                       QLineEdit *Edit = new QLineEdit(QString::number(f));
                        Edit->setToolTip(QString::fromStdString(d.HelpText));
                                       mainLayout->addWidget(NameLabel);
                                       mainLayout->addWidget(Edit);
                    },
               [&](std::string const &s){
                        QLineEdit *Edit = new QLineEdit(QString::fromStdString(s));
                        Edit->setToolTip(QString::fromStdString(d.HelpText));
                        mainLayout->addWidget(NameLabel);
                        mainLayout->addWidget(Edit);
                     }
           }, d.Value);
    }

    mainLayout->addStretch(1);
    setLayout(mainLayout);
}
//! [6]

