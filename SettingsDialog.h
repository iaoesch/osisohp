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

#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

// explicit deduction guide (not needed as of C++20)
template<class... Ts>
overloaded_osi(Ts...) -> overloaded_osi<Ts...>;


#include <QDialog>

QT_BEGIN_NAMESPACE
class QLineEdit;
class QDialogButtonBox;
class QFileInfo;
class QTabWidget;
QT_END_NAMESPACE

template <class... Types>
class EntityDescriptorTemplate {
   template <class T>
   class ValueDescriptor {
      mutable T  NewValue;
      T &Source;
      struct {
         T Lower;
         T Upper;

      } Limits;

   public:
      ValueDescriptor(T &Src, T LowerLimit, T UpperLimit) : NewValue(Src), Source(Src), Limits({LowerLimit, UpperLimit}) {}

      const T &getValue() const { return NewValue;}
      void setValue(const T &newNewValue) {  NewValue = newNewValue;}
      void Update() {  Source = NewValue;}
   };
   template<class V, class ...Ts>
   struct overloaded_osi;

   template<class V>
   struct overloaded_osi<V>  {
      V Vistor;
      overloaded_osi(V visitor) : Vistor(visitor) {}
   };

   template<class V, class T, class ...Ts>
   struct overloaded_osi<V, T,  Ts...> : overloaded_osi<V, Ts...> {
      using overloaded_osi<V, Ts...>::operator();
      using overloaded_osi<V, Ts...>::overloaded_osi;
      void operator() (ValueDescriptor<T> &v) {vistor(v.getValue());}
   };

   template<class... Ts>
   struct overloaded_osili : Ts... { using Ts::operator()...; };
   // explicit deduction guide (not needed as of C++20)
   template<class... Ts>
   overloaded_osili(Ts...) -> overloaded_osili<Ts...>;

public:
   typedef std::variant<ValueDescriptor<Types>...> ValueType;
 //  typedef std::variant<ValueDescriptor<bool>, ValueDescriptor<int>, ValueDescriptor<double>, ValueDescriptor<std::string>> ValueType2;

  // ValueType2 NewVal;
   std::string Title;
   std::string HelpText;
   ValueType Value;

   template <class U>
   EntityDescriptorTemplate(std::string const &Name, std::string const &Help, U Val, U LimitLow, U LimitHigh) :
 //  EntityDescriptorTemplate(std::string const &Name, std::string const &Help, EntityDescriptor::ValueType Val, EntityDescriptor::ValueType LimitLow, EntityDescriptor::ValueType LimitHigh) :
 //  NewVal(Val, Val, LimitLow, LimitHigh),
   Title(Name), HelpText(Help), Value(Val, LimitLow, LimitHigh) {}

   template <class... U>
   void Visit(std::function<void(U &Value, U Lower, U Upper)>... func) const{
      std::visit(overloaded_osi{func...}, Value);
   }

};

typedef EntityDescriptorTemplate<bool, int, double, std::string> EntityDescriptor;

class TabDescriptor {
private:
   std::string TabName;
   std::vector<EntityDescriptor> Entries;

public:
   TabDescriptor(std::string Name) : TabName(Name) {}
   template <class U>
   void AddEntry(std::string const &Name, std::string const &Help, U Value, U LowerLimit = std::numeric_limits<U>::lowest(), U UpperLimit = std::numeric_limits<U>::max())
   {
      AddEntry(Name, Help, EntityDescriptor::ValueType(Value), EntityDescriptor::ValueType(LowerLimit), EntityDescriptor::ValueType(UpperLimit));
   }

   //void AddEntry(std::string const &Name, std::string const &Help, double Value, double LowerLimit = 0.0, double UpperLimit = 100.0E10);
   void AddEntry(std::string const &Name, std::string const &Help, const char *Value) {AddEntry(Name, Help, std::string(Value));}
   void AddEntry(std::string const &Name, std::string const &Help, std::string Value);

   const std::string &getTabName() const
   {
      return TabName;
   }

   const std::vector<EntityDescriptor> &getEntries() const
   {
      return Entries;
   }

private:
   void AddEntry(std::string const &Name, std::string const &Help, EntityDescriptor::ValueType Value, EntityDescriptor::ValueType LimitLow, EntityDescriptor::ValueType LimitHigh);
};

class TabDialogDescriptor {
private:
   std::string Title;
   std::vector<TabDescriptor> Tabs;

public:
   TabDialogDescriptor(std::string Name) : Title(Name) {}
   TabDescriptor &AddTab(std::string Title);
   TabDescriptor &GetTab(size_t Index) {return Tabs[Index];}
   TabDescriptor &GetTab() {return Tabs.back();}
   const std::string &getTitle() const
   {
      return Title;
   }
   const std::vector<TabDescriptor> &getTabs() const
   {
      return Tabs;
   }
};

//! [0]
class GeneralTab : public QWidget
{
    Q_OBJECT

public:
    explicit GeneralTab(const TabDescriptor &Descriptor, QWidget *parent = nullptr);

private slots:
    void NewInput();
    void NewState(int State);

private:
    std::map<QWidget *, const EntityDescriptor *> DescriptorMap;

};
//! [0]



//! [3]
class SettingsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SettingsDialog(TabDialogDescriptor &Descriptor, QWidget *parent = nullptr);

private:

    QTabWidget *tabWidget;
    QDialogButtonBox *buttonBox;

};
//! [3]

#endif
