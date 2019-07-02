#include "adc_temp.hpp"

#include <sstream>
#include <exception>
#include <thread>
#include <chrono>

#include <pigpio.h>

static constexpr unsigned DEFAULT_SPI_BAUDRATE{500'000};
static constexpr unsigned DEFAULT_SPI_CHANNEL{0};
static constexpr unsigned DEFAULT_SPI_FLAGS{0};
static constexpr unsigned DEFAULT_CS_GPIO{22};

adc_temp::adc_temp() :
    m_cs_gpio(DEFAULT_CS_GPIO)
{
    if (gpioInitialise() < 0)
        throw std::runtime_error("adc_temp::adc_temp error: could not initialize GPIO library\n");

    m_spi_handle = spiOpen(DEFAULT_SPI_CHANNEL,DEFAULT_SPI_BAUDRATE,DEFAULT_SPI_FLAGS);

    if (m_spi_handle < 0)
    {
        gpioTerminate();
        throw std::runtime_error("adc_temp::adc_temp error: could not open SPI channel. Is SPI enabled?\n");
    }

    if (gpioSetMode(m_cs_gpio,PI_OUTPUT))
    {
        gpioTerminate();
        throw std::runtime_error("adc_temp::adc_temp error: could not initialize CS GPIO pin\n");
    }

    // default CS high
    set_cs_pin(true);
}

adc_temp::~adc_temp()
{
    if (spiClose(m_spi_handle))
        std::cerr << "adc_temp::~adc_temp error: failed to close SPI channel\n";

    gpioTerminate();
}

void adc_temp::set_cs_pin(bool state) const
{
    if (gpioWrite(m_cs_gpio,state))
        throw std::runtime_error("adc_temp::set_cs_pin error: could not write to CS GPIO\n");
}

#include <bitset>

unsigned adc_temp::get_adc_reading() const
{
    if (m_spi_handle < 0)
        throw std::runtime_error("adc_temp::get_adc_reading error: SPI handle not initialized\n");

    set_cs_pin(false);

    static std::array<char,1> TX_MSG = {0b11000'0};
    static std::array<char,2> RX_BUF{0};

    if (spiWrite(m_spi_handle,TX_MSG.data(),TX_MSG.size()) != TX_MSG.size())
    {
        std::cout << "adc_temp::get_adc_reading error: spiWrite did not write expected number of bytes\n";
        return {};
    }

    if (spiRead(m_spi_handle,RX_BUF.data(),2) != 2)
    {
        std::cout << "adc_temp::get_adc_reading error: spiRead did not read expected number of bytes\n";
        return {};
    }
    set_cs_pin(true);

    RX_BUF[1] >>= 5;  
    std::bitset<8> msb(RX_BUF[0]);
    std::bitset<3> lsb(RX_BUF[1]);
    std::bitset<11> value(msb.to_string() + lsb.to_string());

    return value.to_ulong();
}

temp_t adc_temp::read_temp()
{
    const auto adc_reading = get_adc_reading();
    if (!adc_reading)
        return 0;

    // parse out temperature value


    return {};
}
