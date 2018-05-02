#include <iostream>
#include <string>
#include <set>

#define VIRTUAL_CLASS(Base) \
class Base { \
public: \
    Base(bool is_par_ = true, std::string name_ = #Base, std::string par_name_ = #Base) { \
        is_par_not_stat = is_par_; \
        par_name = par_name_; \
        name = name_; \
    }; \
    static bool get_par() { return is_par; } \
    bool get_par_not_stat() {return is_par_not_stat; } \
    std::string par_name; \
    std::string name; \
    static std::set<std::string> func_names; \
    static std::set<std::string> der_func_names;


#define END(Base) \
private: \
    static bool is_par; \
    bool is_par_not_stat; \
}; \
bool Base::is_par = true; \
std::set<std::string> Base::func_names = {}; \
std::set<std::string> Base::der_func_names = {};


#define DECLARE_METHOD( Base, Function_Name ) \
str_base = #Base; \
str_dots = "::"; \
str_func_name = #Function_Name; \
if (Base::get_par()) { \
    Base::func_names.insert(str_base + str_dots + str_func_name); \
} else { \
    Base::der_func_names.insert(str_base + str_dots + str_func_name); \
}


#define VIRTUAL_CLASS_DERIVED(Derived, Base) \
class Derived : public Base { \
public: \
    Derived() : Base(false, (std::string)#Derived, (std::string)#Base){}; \
    static bool get_par(){ return is_par; } \
    bool get_par_not_stat() {return is_par_not_stat; } \
    std::string par_name = #Base; \
    std::string name = #Derived;


#define END_DERIVE(Derived, Base) \
private: \
    static bool is_par; \
    bool is_par_not_stat; \
}; \
bool Derived::is_par = false;


#define VIRTUAL_CALL(var, func_name) \
if((*var).get_par_not_stat()) { \
    str_base = (*var).name; \
    str_dots = "::"; \
    str_func_name = #func_name; \
    if((*var).func_names.find(str_base + str_dots + str_func_name) != (*var).func_names.end()) { \
        std::cout << str_base + str_dots + str_func_name << std::endl; \
    } else { \
        std::cout << "There is no such method\n"; \
    } \
} else { \
    str_base = (*var).name; \
    str_dots = "::"; \
    str_func_name = #func_name; \
    if((*var).der_func_names.find(str_base + str_dots + str_func_name) != (*var).der_func_names.end()) { \
        std::cout << str_base + str_dots + str_func_name << std::endl; \
    } else { \
        str_base = (*var).par_name; \
        if((*var).func_names.find(str_base + str_dots + str_func_name) != (*var).func_names.end()) { \
            std::cout << str_base + str_dots + str_func_name << std::endl; \
        } else { \
            std::cout << "There is no such method\n"; \
        } \
    } \
}

// базовый класс
VIRTUAL_CLASS( Base )
int a;
END( Base )

// класс-наследник
VIRTUAL_CLASS_DERIVED( Derived, Base )
int b;
END_DERIVE( Derived, Base )


int main() {
    std::string str_base;
    std::string str_dots;
    std::string str_func_name;

    DECLARE_METHOD( Base, Both )
    DECLARE_METHOD( Base, OnlyBase )
    DECLARE_METHOD( Derived, Both )
    DECLARE_METHOD( Derived, OnlyDerived )
    Base base;
    base.a = 0; // работаем как со структурой
    Derived derived;/* ... как-то создали наследник */
    Base* reallyDerived = reinterpret_cast<Base*>(&derived);
    VIRTUAL_CALL(&base, Both); // печатает “Base::Both a = 0”
    VIRTUAL_CALL(reallyDerived, Both); // печатает “Derived::Both b = 1”
    VIRTUAL_CALL(reallyDerived, OnlyBase);  // печатает “Base::OnlyBase”
    VIRTUAL_CALL(reallyDerived, OnlyDeriveded);
    return 0;
}