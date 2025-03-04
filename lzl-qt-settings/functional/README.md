License: MIT
Copyright (c) 2024-2025 李宗霖 (Li Zonglin)
Email: supine0703@outlook.com
GitHub: https://github.com/supine0703
Repository: https://github.com/supine0703/lzl-cpp-lib

Function extraction can extract function types to perform certain behaviors. For example, it can extract types from template functions to call template functions implicitly. It is also possible to extract the parent class of a member function to use class Pointers and function Pointers as arguments.

```cpp
struct Test {
    void* value;
    void printValue(int64_t num) { std::cout << num << std::endl; }
};
template <typename Func>
static void readValue(lzl::utils::trains_class_type<Func>* object, Func read_func) {
    using arg_type = typename lzl::utils::function_traits<Func>::template arg<0>::type;
    static_assert(lzl::utils::function_traits<Func>::arity == 1);
    (object->*read_func)(reinterpret_cast<arg_type>(object->value));
}
void test() {
    Test test{reinterpret_cast<void*>(10)};
    readValue(&test, &Test::printValue);
}
```

More information can be found in this repository [supine0703/qt-settings](https://github.com/supine0703/qt-settings).
