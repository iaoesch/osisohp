
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

   template<class ...Ts>
   struct overloaded_Update;

   template<>
   struct overloaded_Update<>  {
      void operator() () {}
   };

   template<class T, class ...Ts>
   struct overloaded_Update<T,  Ts...> : overloaded_Update<Ts...> {
      using overloaded_Update<Ts...>::operator();
      void operator() (ValueDescriptor<T> &v) {v.Update();}
   };

public:
   typedef std::variant<ValueDescriptor<Types>...> ValueType;
   typedef std::variant<Types...> VariantType;

   void Update() {std::visit(overloaded_Update<Types...>(),Value);}
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

  // ValueType2 NewVal;
   std::string Title;
   std::string HelpText;
   ValueType Value;

   template <class U>
   EntityDescriptorTemplate(std::string const &Name, std::string const &Help, U &Val, U Default, U LimitLow, U LimitHigh) :
 //  EntityDescriptorTemplate(std::string const &Name, std::string const &Help, EntityDescriptor::ValueType Val, EntityDescriptor::ValueType LimitLow, EntityDescriptor::ValueType LimitHigh) :
 //  NewVal(Val, Val, LimitLow, LimitHigh),
   Title(Name), HelpText(Help), Value(ValueDescriptor<U>(Val, Default, LimitLow, LimitHigh)) {}

   EntityDescriptorTemplate(const EntityDescriptorTemplate &) = default;
   EntityDescriptorTemplate(EntityDescriptorTemplate &&) = default;

   template <class... U>
   void Visit(std::function<void(U &Value, U Lower, U Upper)>... func) const{
      std::visit(overloaded_osi{func...}, Value);
   }

};

typedef EntityDescriptorTemplate<bool, int, double, std::string> EntityDescriptor;

class GroupDescriptor {
private:
   std::string GroupName;
   std::vector<EntityDescriptor> Entries;

public:
   GroupDescriptor() {}

   typedef EntityDescriptor::VariantType VariantType;

   GroupDescriptor(std::string Name) : GroupName(Name) {}
   GroupDescriptor(const GroupDescriptor &src) = default;
   template <class U>
   void AddEntry(std::string const &Name, std::string const &Help, U &Value, U Default, U LowerLimit = std::numeric_limits<U>::lowest(), U UpperLimit = std::numeric_limits<U>::max())
   {
      Entries.push_back(EntityDescriptor(Name, Help, Value, Default, LowerLimit, UpperLimit));
   }

   //void AddEntry(std::string const &Name, std::string const &Help, double Value, double LowerLimit = 0.0, double UpperLimit = 100.0E10);
  // void AddEntry(std::string const &Name, std::string const &Help, const char *&Value) {AddEntry(Name, Help, std::string(Value));}
   void AddEntry(std::string const &Name, std::string const &Help, std::string &Value, std::string Default) {
      AddEntry<std::string>(Name, Help, Value, Default, std::string(), std::string());
   }

   const std::string &GetGroupName() const
   {
      return GroupName;
   }

   const std::vector<EntityDescriptor> &getEntries() const
   {
      return Entries;
   }

   void Update()
   {
      for(auto &e: Entries) {
         e.Update();
      }
   }
private:
  // void AddEntry(std::string const &Name, std::string const &Help, EntityDescriptor::ValueType Value, EntityDescriptor::ValueType LimitLow, EntityDescriptor::ValueType LimitHigh);
};


#endif
