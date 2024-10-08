#include <cmath>
#include <print>
#include <array>
#include <format>
#include <iostream>
#include <Zut/ZxMem.h>
#include <vector>
#include <list>
#include <map>
#include <set>


struct foo
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
    // ZXMEM_REFLEX(a,b,c,d,e,f,g,h,i)
    ZXMEM_REFLEX_ALL
};

auto main(void) -> int
{
    try
    {
        foo fool2_store
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

         fool2_store.ReflexBinaryStore().Save("2.bin");

        foo foo2_load;
        foo2_load.ReflexBinaryLoad("2.bin");

    }
    catch (const std::exception& err)
    {
        std::println(std::cerr, "std::exception: {}", err.what());
    }
}
