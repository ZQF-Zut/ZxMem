# ZxMem
MemoryStream

## Example
**basic io operation**
```cpp
#include <print>
#include <iostream>
#include <Zut/ZxMem.h>


auto main(void) -> int 
{
    try
    {
        ZxMem mem{ 100 };
        
        {
            std::uint32_t val_0{ 0xFFAABB };
            mem << val_0;

            std::double_t val_1{ 1.02 };
            mem << val_1;

            std::array<std::uint32_t, 3> val_2{ 10086, 112233, 666 };
            mem << val_2;

            std::string val_3{ "12329th832h9t93t23" };
            mem << std::span{ val_3 };
        }

        mem.PosRewind();
        {
            const auto val_0_load{ mem.Get<std::uint32_t>() };

            std::double_t val_1_load{};
            mem >> val_1_load;

            const auto val_2_load{ mem.Get<std::array<std::uint32_t, 3>>() };

            std::string val_3_load;
            val_3_load.resize(18);
            mem >> std::span{ val_3_load };

            int a = 0;
        }

        mem.PosRewind();
        {
            std::array<std::uint32_t, 3> val{ 10086, 112233, 666 };
            mem.Resize(std::span{ val }.size_bytes());
            mem.Put(val);
            for (const auto& v : mem.Span<std::uint32_t>())
            {
                std::println("{}", v);
            }
            mem.Resize(100);
        }

        mem.PosRewind();
        {
            mem
                << std::uint32_t(0xFF)
                << std::uint64_t(0xAAAAAAAA)
                << std::uint16_t(0xBB)
                << std::span{ "abcdxyz" };
            mem.Save("1.bin");
        }

        mem.PosRewind();
        {
            mem.Load("1.bin");
            std::uint32_t v0{};
            std::uint64_t v1{};
            std::uint16_t v2{};
            std::array<char, 8> v3{};

            mem >> v0 >> v1 >> v2 >> v3;
        }
    }
    catch (const std::exception& err)
    {
        std::println(std::cerr, "std::exception: {}", err.what());
    }
}
```

**reflex serialization, deserialization of binary data**
```cpp
#include <print>
#include <iostream>
#include <Zut/ZxMem.h>


struct foo0
{
    struct foox
    {
        std::size_t n{};
        std::string x;
    };

    std::size_t n{};
    std::string x;
    foox m;
};

struct foo1
{
public:
    std::uint32_t a{};
    std::uint64_t b{};
    std::double_t c{};
    std::string d;
    std::array<char, 16> e{};
    std::vector<int> f;
    std::list<std::string> g;
    std::set<int> h;
    std::map<std::string, std::size_t> i;
    std::unordered_map<std::string, std::pair<std::size_t, std::size_t>> j;
    foo0 k;

public:
    ZXMEM_REFLEX_ALL
};

struct foo2
{
public:
    std::uint32_t a{};
    std::uint64_t b{};
    std::double_t c{};
    std::string d;
    std::array<char, 16> e{};
    std::vector<int> f;
    std::list<std::string> g;
    std::set<int> h;
    std::map<std::string, std::size_t> i;
    std::unordered_map<std::string, std::pair<std::size_t, std::size_t>> j;
    foo0 k;

public:
    ZXMEM_REFLEX(a, b, c, d, e, f, g, h, i,j,k)
};

struct foo3
{
public:
    std::uint32_t a{};
    std::uint64_t b{};
    std::double_t c{};
    std::string d;
    std::array<char, 16> e{};
    std::vector<int> f;
    std::list<std::string> g;
    std::set<int> h;
    std::map<std::string, std::size_t> i;
    std::unordered_map<std::string, std::pair<std::size_t, std::size_t>> j;
    foo0 k;
};

auto main(void) -> int
{
    try
    {
        {
            foo1 fool1_store
            {
                .a = 0xAABBCCDD,
                .b = 0x1122334455667788,
                .c = 1.03,
                .d = "test st::string",
                .e = "std::array",
                .f = {1,2,3,4,5,6,7},
                .g = {"123","321","666","777","888"},
                .h = {0xAA,0xBB,0xCC,0xEE,0xFF},
                .i = {{"1008611",0x01008611},{"AAVV", 0xAABB},{"shfuis",0x22}},
                .j = {{"1008611",std::pair{0x01008611, 0x1}},{"AAVV", std::pair{0xAABB,0x1}},{"shfuis",std::pair{0x22,0x1}}},
                .k = {123,"666",{321, "999"}}
            };
            fool1_store.ReflexBinaryStore().Save("foo1.bin");

            foo1 foo1_load;
            foo1_load.ReflexBinaryLoad("foo1.bin");
        }

        {
            foo2 fool2_store
            {
                .a = 0xAABBCCDD,
                .b = 0x1122334455667788,
                .c = 1.03,
                .d = "test st::string",
                .e = "std::array",
                .f = {1,2,3,4,5,6,7},
                .g = {"123","321","666","777","888"},
                .h = {0xAA,0xBB,0xCC,0xEE,0xFF},
                .i = {{"1008611",0x01008611},{"AAVV", 0xAABB},{"shfuis",0x22}}
                .j = {{"1008611",std::pair{0x01008611, 0x1}},{"AAVV", std::pair{0xAABB,0x1}},{"shfuis",std::pair{0x22,0x1}}},
                .k = {123,"666",{321, "999"}}
            };
            fool2_store.ReflexBinaryStore().Save("foo2.bin");

            foo2 foo2_load;
            foo2_load.ReflexBinaryLoad("foo2.bin");
        }

        {
            foo3 fool3_store
            {
                .a = 0xAABBCCDD,
                .b = 0x1122334455667788,
                .c = 1.03,
                .d = "test st::string",
                .e = "std::array",
                .f = {1,2,3,4,5,6,7},
                .g = {"123","321","666","777","888"},
                .h = {0xAA,0xBB,0xCC,0xEE,0xFF},
                .i = {{"1008611",0x01008611},{"AAVV", 0xAABB},{"shfuis",0x22}}
                .j = {{"1008611",std::pair{0x01008611, 0x1}},{"AAVV", std::pair{0xAABB,0x1}},{"shfuis",std::pair{0x22,0x1}}},
                .k = {123,"666",{321, "999"}}
            };
            ZxMemReflex::BinaryStore(fool3_store).Save("3.bin");

            foo3 foo3_load;
            ZxMemReflex::BinaryLoad(foo3_load, "3.bin");
        }

        [[maybe_unused]] int aa{};

    }
    catch (const std::exception& err)
    {
        std::println(std::cerr, "std::exception: {}", err.what());
    }
}
```

## Installation
**CMake**
```cmake
cmake_minimum_required (VERSION 3.25)

include(FetchContent)
FetchContent_Declare(
  ZxMem
  GIT_REPOSITORY https://github.com/ZQF-Zut/ZxMem.git
  GIT_TAG 1698749d37fe23a3672c212f161a453bae71f86e)

FetchContent_MakeAvailable(ZxMem)

add_executable (my_project "main.cpp" )
target_link_libraries(my_project PRIVATE Zut::ZxMem)
target_compile_features(my_project PRIVATE cxx_std_23)
```

