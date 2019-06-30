#include "adc_temp.hpp"

#include <sstream>
#include <exception>

#include <pigpio.h>

constexpr unsigned DEFAULT_SPI_BAUDRATE{10'000'000};
constexpr unsigned DEFAULT_SPI_CHANNEL{0};
constexpr unsigned DEFAULT_SPI_FLAGS{0};

adc_temp::adc_temp() :
    m_spi_handle(spiOpen(DEFAULT_SPI_CHANNEL,DEFAULT_SPI_BAUDRATE,DEFAULT_SPI_FLAGS))
{
    if (m_spi_handle < 0)
        throw std::runtime_error("adc_temp::adc_temp error: could not open SPI channel\n");
}

adc_temp::~adc_temp()
{
    if (spiClose(m_spi_handle))
        std::cerr << "adc_temp::~adc_temp error: failed to close SPI channel\n";
}

temp_t adc_temp::read_temp()
{
    //spiXfer

    return {};
}
