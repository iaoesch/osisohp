
#ifndef VALUEGROUPDESCRIPTOR_H
#define VALUEGROUPDESCRIPTOR_H

#include <variant>
#include <string>
#include <vector>


template<typename VariantType, typename T, std::size_t index = 0>
constexpr std::size_t variant_index() {
    static_assert(std::variant_size_v<VariantType> > index, "Type not found in variant");
    if constexpr (index == std::variant_size_v<VariantType>) {
        return index;
    } else if constexpr (std::is_same_v<std::variant_alternative_t<index, VariantType>, T>) {
        return index;
    } else {
        return variant_index<VariantType, T, index + 1>();
    }
}

template <class T>
class InvokeUpdate {
public:
   typedef bool ResultType;
   static ResultType Invoke(T &o) {return o.Update();}
};

template <class T>
class InvokeFetch {
public:
   typedef void ResultType;
   static ResultType Invoke(T &o) {o.Fetch();}
};

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
      T Default;

   public:
      ValueDescriptor(T &Src, T DefaultValue, T LowerLimit, T UpperLimit) : NewValue(Src), Source(Src), Limits({LowerLimit, UpperLimit}), Default(DefaultValue) {}
      ValueDescriptor(const ValueDescriptor &src) = default;
      ValueDescriptor(ValueDescriptor &&src) = default;

      const auto &getLimits() const { return Limits;}
      const T &getValue() const { return NewValue;}
      const T &GetDefaultValue() const { return Default;}
      void setValue(const T &newNewValue) const {  NewValue = newNewValue;}
      bool Update() {  bool Modified = (Source != NewValue); Source = NewValue; return Modified;}
      void Fetch() {  NewValue = Source;}
   };
#ifdef Visitor_experiments
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
#endif
   template<template<class> class MemberFct, class ...Ts>
   struct overloaded_Update;

   template<template<class> class MemberFct>
   struct overloaded_Update<MemberFct>  {
      void operator() () {}
   };

   template<template<class> class MemberFct, class T, class ...Ts>
   struct overloaded_Update<MemberFct, T,  Ts...> : overloaded_Update<MemberFct, Ts...> {
      using overloaded_Update<MemberFct, Ts...>::operator();
     // void operator() (ValueDescriptor<T> &v) {v.Update();}
      typename MemberFct<ValueDescriptor<T>>::ResultType operator() (ValueDescriptor<T> &v) {return MemberFct<ValueDescriptor<T>>::Invoke(v);}
   };

public:
   typedef std::variant<ValueDescriptor<Types>...> ValueType;
   typedef std::variant<Types...> VariantType;
   typedef void (*CallbackType)();

   bool Update() {Modified = std::visit(overloaded_Update<InvokeUpdate, Types...>(),Value);
                  if (Modified && (Notify != nullptr)) {Notify();}
                  return Modified;}
   void Fetch() {std::visit(overloaded_Update<InvokeFetch, Types...>(),Value);}
   template <class T>
   static constexpr int IdOf() {return variant_index<ValueType, ValueDescriptor<T>>();}
   int CurrentTypeId() const {return Value.index();}
   template <class T>
   const ValueDescriptor<T> &GetDescriptor() const {return std::get<ValueDescriptor<T>>(Value);}

   template <class T>
   const auto &GetLimits() const {return GetDescriptor<T>().getLimits();}
   template <class T>
   const T &GetDefaultValue() const {return GetDescriptor<T>().GetDefaultValue();}
   template <class T>
   const T &GetValue() const {return GetDescriptor<T>().getValue();}
   template <class T>
   void SetValue(const T &Value) const {GetDescriptor<T>().setValue(Value);}
   //  typedef std::variant<ValueDescriptor<bool>, ValueDescriptor<int>, ValueDescriptor<double>, ValueDescriptor<std::string>> ValueType2;
   bool WasModified() {return Modified; }
   CallbackType RegisterCallback(CallbackType Callback) {CallbackType OldCallback = Notify; Notify = Callback; return OldCallback;}
  // ValueType2 NewVal;
   std::string Title;
   std::string HelpText;
   ValueType Value;
   bool Modified;
   CallbackType Notify;

   template <class U>
   EntityDescriptorTemplate(std::string const &Name, std::string const &Help, U &Val, U Default, U LimitLow, U LimitHigh) :
 //  EntityDescriptorTemplate(std::string const &Name, std::string const &Help, EntityDescriptor::ValueType Val, EntityDescriptor::ValueType LimitLow, EntityDescriptor::ValueType LimitHigh) :
 //  NewVal(Val, Val, LimitLow, LimitHigh),
   Title(Name), HelpText(Help), Value(ValueDescriptor<U>(Val, Default, LimitLow, LimitHigh)), Modified(false), Notify(nullptr) {}

   EntityDescriptorTemplate(const EntityDescriptorTemplate &) = default;
   EntityDescriptorTemplate(EntityDescriptorTemplate &&) = default;

#ifdef Visitor_experiments
   template <class... U>
   void Visit(std::function<void(U &Value, U Lower, U Upper)>... func) const{
      std::visit(overloaded_osi{func...}, Value);
   }
#endif
};

typedef EntityDescriptorTemplate<bool, int, double, std::string> EntityDescriptor;

class GroupDescriptor {
private:
   std::string GroupName;
   std::vector<EntityDescriptor> Entries;


public:
   GroupDescriptor() {}

   typedef EntityDescriptor::VariantType VariantType;
   typedef EntityDescriptor::CallbackType CallbackType;

   GroupDescriptor(std::string Name) : GroupName(Name) {}
   GroupDescriptor(const GroupDescriptor &src) = default;
   template <class U>
   EntityDescriptor &AddEntry(std::string const &Name, std::string const &Help, U &Value, U Default, U LowerLimit = std::numeric_limits<U>::lowest(), U UpperLimit = std::numeric_limits<U>::max())
   {
      Entries.push_back(EntityDescriptor(Name, Help, Value, Default, LowerLimit, UpperLimit));
      return Entries.back();
   }

   //void AddEntry(std::string const &Name, std::string const &Help, double Value, double LowerLimit = 0.0, double UpperLimit = 100.0E10);
  // void AddEntry(std::string const &Name, std::string const &Help, const char *&Value) {AddEntry(Name, Help, std::string(Value));}
   EntityDescriptor & AddEntry(std::string const &Name, std::string const &Help, std::string &Value, std::string Default) {
      return AddEntry<std::string>(Name, Help, Value, Default, std::string(), std::string());
   }

   const std::string &GetGroupName() const
   {
      return GroupName;
   }

   const std::vector<EntityDescriptor> &getEntries() const
   {
      return Entries;
   }

   bool Update()
   {
      bool AnyValueChanged = false;
      for(auto &e: Entries) {
         if (e.Update()) {
            AnyValueChanged = true;
         }
      }
      return AnyValueChanged;
   }

   void Fetch()
   {
      for(auto &e: Entries) {
         e.Fetch();
      }
   }
private:
  // void AddEntry(std::string const &Name, std::string const &Help, EntityDescriptor::ValueType Value, EntityDescriptor::ValueType LimitLow, EntityDescriptor::ValueType LimitHigh);
};


#endif
