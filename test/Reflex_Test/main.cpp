#include <cmath>
#include <print>
#include <iostream>
#include <Zut/ZxMem.h>


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

public:
    ZXMEM_REFLEX(a,b,c,d,e,f,g,h,i)
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
                .i = {{"1008611",0x01008611},{"AAVV", 0xAABB},{"shfuis",0x22}}
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
