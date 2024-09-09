#include <cmath>
#include <print>
#include <array>
#include <format>
#include <iostream>
#include <Zut/ZxMem.h>


static auto TestZxMemIO() -> void
{
    {
        std::array<std::uint16_t, 2> ee = { 131,66 };
        ZxMem mem{ 4 };
        mem.Write(std::span{ ee });
        [[maybe_unused]] int a = 0;

        mem.PosSet<ZxMem::PosWay::Cur>(static_cast<std::size_t>(-4));

        [[maybe_unused]] std::uint16_t x0 = mem.Get<std::uint16_t>();
        [[maybe_unused]] std::uint16_t x1 = mem.Get<std::uint16_t>();
    }


    ZxMem mem(100);

    std::uint32_t tmp_val0 = 222;
    std::double_t tmp_val1 = 111.11;
    std::array<std::uint32_t, 3> tmp_array{ 10086, 112233, 666 };
    mem << tmp_val0;
    mem << tmp_val1;
    mem << 100;
    mem << 1.22;
    mem << tmp_array;

    std::size_t tmp_val2 = 777;
    std::uint8_t tmp_val3 = 0xF;
    std::uint16_t tmp_val4 = 0xFF;
    mem.Put(tmp_val2).Put(tmp_val3).Put(tmp_val4);

    mem.PosSet<ZxMem::PosWay::Set>();

    assert(tmp_val0 == mem.Get<std::uint32_t>());
    assert(tmp_val1 == mem.Get<std::double_t>());
    assert(100 == mem.Get<std::uint32_t>());
    assert(1.22 == mem.Get<std::double_t>());
    auto temp_array_get = mem.Get<std::array<std::uint32_t, 3>>();
    assert(tmp_array == temp_array_get);

    std::size_t tmp_val2_r{};
    mem >> tmp_val2_r;
    assert(tmp_val2 == tmp_val2_r);
    std::uint8_t tmp_val3_r{};
    mem >> tmp_val3_r;
    assert(tmp_val3 == tmp_val3_r);
    std::uint16_t tmp_val4_r{};
    mem >> tmp_val4_r;
    assert(tmp_val4 == tmp_val4_r);

    [[maybe_unused]] int a = 0;
}

auto main(void) -> int
{
    try
    {
        ::TestZxMemIO();
    }
    catch (const std::exception& err)
    {
        std::println(std::cerr, "std::exception: {}", err.what());
    }
}
