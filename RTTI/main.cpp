#include <string>
#include <functional>
#include <iostream>
#include <vector>
#include <unordered_map>



struct ClassWithParents {
    std::vector<std::string> parents;
    std::vector<int> parents_sizes;
    int self_size;
};

std::unordered_map<std::string, ClassWithParents> class_structure;

void split_string(std::string whole_string, std::vector<std::string>& string_with_parents) {
    std::string curr_string = "";
    for(int i = 0; i < whole_string.size(); i++) {
        if(whole_string[i] == ' ' || whole_string[i] == ',') {
            if(curr_string != "" && curr_string != "public" && curr_string != "private" && curr_string != "protected") {
                string_with_parents.push_back(curr_string);
                curr_string = "";
            } else {
                curr_string = "";
            }
        } else {
            curr_string.push_back(whole_string[i]);
        }
    }
    if(curr_string != "") {
        string_with_parents.push_back(curr_string);
    }
    return;
}

void* AddNewClass(std::string new_class, std::string parents = "") {
    std::vector<std::string> vector_with_parents;
    split_string(parents, vector_with_parents);
    ClassWithParents new_class_struct;
    for(int i = 0; i < vector_with_parents.size(); i++) {
        new_class_struct.parents.push_back(vector_with_parents[i]);
        new_class_struct.parents_sizes.push_back(class_structure[vector_with_parents[i]].self_size);
    }
    class_structure[new_class] = new_class_struct;
    return nullptr;
}

#define Class(class_name) \
void* Add##class_name = AddNewClass(#class_name); \
class class_name : virtual public TypeInfo

#define ClassInheret(class_name, ...) \
void* Add##class_name = AddNewClass(#class_name, #__VA_ARGS__); \
class class_name : virtual public TypeInfo, __VA_ARGS__

class Info {
public:
    Info() {}

    Info(std::string class_name) {
        name = class_name;
        std::hash<std::string> hash_str;
        hash = hash_str(class_name);
    }

    std::string name;
    std::size_t hash;
};

class TypeInfo {
public:
    Info class_info;
};

void UpdateParents(std::string class_name, int size) {
    std::unordered_map<std::string, ClassWithParents>::iterator it;
    it = class_structure.begin();
    for(; it != class_structure.end(); it++) {
        for(int i = 0; i < (*it).second.parents.size(); i++) {
            if ((*it).second.parents[i] == class_name) {
                (*it).second.parents_sizes[i] = size;
            }
        }
    }
}


#define TYPEID(entity) \
(entity).class_info

#define RTTI(class_name) \
std::string class_name_ = #class_name; \
class_info = Info(class_name_); \
class_structure[#class_name].self_size = sizeof(class_name) - sizeof(TypeInfo); \
UpdateParents(#class_name, class_structure[#class_name].self_size); \


std::pair<bool, int> findshift(std::string ptr, std::string class_name) {
    std::string type;
    if(ptr[ptr.size() - 1] == '*') {
        for (int i = 0; i < ptr.size() - 1; i++) {
            type.push_back(ptr[i]);
        }
    } else {
        type = ptr;
    }

    if(class_name == type) {
        return std::make_pair(true, 0);
    }

    int shift = 0;
    std::pair<bool, int> ans;
    for(int i = 0; i < class_structure[type].parents.size(); i++) {
        ans = findshift(class_structure[type].parents[i], class_name);
        if(ans.first) {
            return std::make_pair(true, shift + ans.second);
        }
        shift += class_structure[class_structure[type].parents[i]].self_size;
    }
    return std::make_pair(false, shift);
}


#define DYNAMIC_CAST(curr_obj, type) \
(findshift(TYPEID(*curr_obj).name, #type).first ? reinterpret_cast<type*>(findshift(TYPEID(*curr_obj).name, #type).second + reinterpret_cast<char*>(curr_obj)) : nullptr)


Class(A) {
public:
    A() {
        RTTI(A)
    }
    int a;
};

Class(B) {
public:
    B() {
        RTTI(B)
    }
    int b;
};

ClassInheret(C, public A, public B) {
public:
    C() {
        RTTI(C)
    }
    int c;
};


int main()
{
    A* first_class = new C;
    A* second_class = new A();
    Info info = TYPEID(*first_class);
    std::cout << info.name << " " << info.hash << std::endl << sizeof(A) << std::endl;
    if(DYNAMIC_CAST(first_class, C) != nullptr) {
        std::cout << "True\n";
    } else {
        std::cout << "False\n";
    }
    if(DYNAMIC_CAST(DYNAMIC_CAST(first_class, B), A) != nullptr) {
        std::cout << "True\n";
    } else {
        std::cout << "False\n";
    }
    if(DYNAMIC_CAST(second_class, B) != nullptr) {
        std::cout << "True\n";
    } else {
        std::cout << "False\n";
    }
    return 0;
}