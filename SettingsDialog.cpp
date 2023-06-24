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
SettingsDialog::SettingsDialog(TabDialogDescriptor &Descriptor, QWidget *parent)
    : QDialog(parent)
{

    tabWidget = new QTabWidget;
    for (auto &Tab: Descriptor.getTabs()) {
       tabWidget->addTab(new GeneralTab(Tab), QString::fromStdString(Tab.getTabName()));
    }
//! [0]

//! [1] //! [2]
    buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok
//! [1] //! [3]
                                     | QDialogButtonBox::RestoreDefaults | QDialogButtonBox::Cancel);

    connect(buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);
   // connect(buttonBox, &QDialogButtonBox::HelpRequested, this, &QDialog::reject);
//! [2] //! [3]

//! [4]
    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addWidget(tabWidget);
    mainLayout->addWidget(buttonBox);
    setLayout(mainLayout);
//! [4]

//! [5]
    setWindowTitle(QString::fromStdString(Descriptor.getTitle()));
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
   QVBoxLayout *mainLayout = new QVBoxLayout;

   for (auto &d: Descriptor.getEntries()) {
      // Switch on type in variant
      QLabel *NameLabel = new QLabel(QString::fromStdString(d.Title));
      QHBoxLayout *HBoxLayout = new QHBoxLayout;
      HBoxLayout->addWidget(NameLabel);
      switch(d.CurrentTypeId()) {
         case EntityDescriptor::IdOf<bool>():
            {
               QCheckBox *CheckBox = new QCheckBox();
               if ( d.GetValue<bool>() ) {
                  CheckBox->setChecked(true);
               }
               CheckBox->setToolTip(QString::fromStdString(d.HelpText));
               HBoxLayout->addWidget(CheckBox);
               DescriptorMap[CheckBox] = &d;
               connect(CheckBox, &QCheckBox::stateChanged, this, &GeneralTab::NewState);

            }
         break;
         case EntityDescriptor::IdOf<int>():
            {
               QLineEdit *Edit = new QLineEdit(QString::number(d.GetValue<int>()));
               Edit->setToolTip(QString::fromStdString(d.HelpText));
               Edit->setValidator( new QIntValidator(d.GetLimits<int>().Lower, d.GetLimits<int>().Upper, this));
               HBoxLayout->addWidget(Edit);
               DescriptorMap[Edit] = &d;
               connect(Edit, &QLineEdit::editingFinished, this, &GeneralTab::NewInput);
            }
            break;
         case EntityDescriptor::IdOf<double>():
            {

               QLineEdit *Edit = new QLineEdit(QString::number(d.GetValue<double>()));
               Edit->setToolTip(QString::fromStdString(d.HelpText));
               Edit->setValidator( new QDoubleValidator(d.GetLimits<double>().Lower, d.GetLimits<double>().Upper, 4, this));
               HBoxLayout->addWidget(Edit);
               DescriptorMap[Edit] = &d;
               connect(Edit, &QLineEdit::editingFinished, this, &GeneralTab::NewInput);
            }
         break;
         case EntityDescriptor::IdOf<std::string>():
            {
               QLineEdit *Edit = new QLineEdit(QString::fromStdString(d.GetValue<std::string>()));
               Edit->setToolTip(QString::fromStdString(d.HelpText));
               HBoxLayout->addWidget(Edit);
               DescriptorMap[Edit] = &d;
               connect(Edit, &QLineEdit::editingFinished, this, &GeneralTab::NewInput);
            }
      }

      mainLayout->addLayout(HBoxLayout);

   }

   mainLayout->addStretch(1);
   setLayout(mainLayout);
}

void GeneralTab::NewInput()
{
   QLineEdit* edit = qobject_cast<QLineEdit*>(sender());
   if (edit) {
      // Do something with QLineEdit
      auto Val = DescriptorMap[edit];
      switch(Val->CurrentTypeId()) {
         case EntityDescriptor::IdOf<bool>():
            Val->SetValue<bool>(false);
            break;
         case EntityDescriptor::IdOf<int>():
            Val->SetValue<int>(edit->text().toInt());
            break;
         case EntityDescriptor::IdOf<double>():
            Val->SetValue<double>(edit->text().toDouble());
            break;
         case EntityDescriptor::IdOf<std::string>():
            Val->SetValue<std::string>(edit->text().toStdString()) ;

      }
   }
}
void GeneralTab::NewState(int State)
{
   QCheckBox* Checkbox = qobject_cast<QCheckBox*>(sender());
   if (Checkbox) {
      if (State == Qt::Checked) {
         DescriptorMap[Checkbox]->SetValue<bool>(true);
      } else {
         DescriptorMap[Checkbox]->SetValue<bool>(false);
      }
   }
}
//! [6]





#if 0
void TabDescriptor::AddEntry(const std::string &Name, const std::string &Help, bool Value)
{
   AddEntry(Name, Help, EntityDescriptor::ValueType(Value), EntityDescriptor::ValueType(false), EntityDescriptor::ValueType(true));
}

void TabDescriptor::AddEntry(const std::string &Name, const std::string &Help, int Value, int LowerLimit, int UpperLimit)
{
   AddEntry(Name, Help, EntityDescriptor::ValueType(Value), EntityDescriptor::ValueType(LowerLimit), EntityDescriptor::ValueType(UpperLimit));
}

void TabDescriptor::AddEntry(const std::string &Name, const std::string &Help, double Value, double LowerLimit, double UpperLimit)
{
   AddEntry(Name, Help, EntityDescriptor::ValueType(Value), EntityDescriptor::ValueType(LowerLimit), EntityDescriptor::ValueType(UpperLimit));
}
#endif
#if 0
void TabDescriptor::AddEntry(const std::string &Name, const std::string &Help, std::string Value)
{
   AddEntry(Name, Help, EntityDescriptor::ValueType(Value), EntityDescriptor::ValueType(""), EntityDescriptor::ValueType(""));
}

void TabDescriptor::AddEntry(const std::string &Name, const std::string &Help, EntityDescriptor::ValueType Value, EntityDescriptor::ValueType LimitLow, EntityDescriptor::ValueType LimitHigh)
{
   Entries.push_back(EntityDescriptor(Name, Help, Value, LimitLow, LimitHigh));
}
#endif




void TabDialogDescriptor::Update()
{
   for(auto &t: Tabs) {
      t.Update();
   }
}

TabDescriptor &TabDialogDescriptor::AddTab(std::string Title)
{
   Tabs.push_back(TabDescriptor(Title));
   return Tabs.back();
}


void TabDescriptor::Update()
{
   for(auto &e: Entries) {
      e.Update();
   }
}
