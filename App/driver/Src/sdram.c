
#include "sdram.h"

uint32_t FMC_GetKernelClock_Hz(void)
{
    // PLL2 settings (네 PeriphCommonClock_Config 값과 반드시 일치해야 함)
    const uint32_t HSE_HZ = 25000000UL;
    const uint32_t PLL2M  = 15;
    const uint32_t PLL2N  = 228;
    const uint32_t PLL2R  = 2;

    uint32_t pll2_vco = (HSE_HZ / PLL2M) * PLL2N;  // 380 MHz
    uint32_t pll2_r   = pll2_vco / PLL2R;         // 190 MHz

    return pll2_r; // FMC kernel clock
}

uint32_t SDRAM_RefreshCount_IS42S16400J(uint32_t sdclk_hz)
{
    // tREFI = 64ms / 4096 = 15.625us
    // COUNT = tREFI * sdclk - 20
    // COUNT = (sdclk_hz * 15625ns) - 20
    // 정수 계산: sdclk_hz(Hz) * 15625 / 1e9
    uint64_t cnt = ((uint64_t)sdclk_hz * 15625ULL + 500000000ULL) / 1000000000ULL; // round
    if (cnt > 20ULL) cnt -= 20ULL;
    else cnt = 1ULL;
    return (uint32_t)cnt;
}