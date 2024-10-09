# ZxMem
MemoryStream

# Example
basic io operation
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

# Installation
cmake
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

