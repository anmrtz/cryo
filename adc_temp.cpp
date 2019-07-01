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

unsigned adc_temp::get_adc_reading() const
{
    if (m_spi_handle < 0)
        throw std::runtime_error("adc_temp::get_adc_reading error: SPI handle not initialized\n");

    set_cs_pin(false);

    std::array<uint8_t,1> TX_MSG = {0b1010};
    std::array<uint8_t,2> RX_BUF{0};

    if (spiWrite(m_spi_handle,(char*)TX_MSG.data(),TX_MSG.size()) != TX_MSG.size())
    {
        std::cout << "adc_temp::get_adc_reading error: spiWrite did not write expected number of bytes\n";
        return {};
    }

    if (spiRead(m_spi_handle,(char*)RX_BUF.data(),RX_BUF.size()) != RX_BUF.size())
    {
        std::cout << "adc_temp::get_adc_reading error: spiRead did not read expected number of bytes\n";
        return {};
    }

    std::cout << "\nFirst buf: " << std::hex << (unsigned)RX_BUF.at(0) << ", second buf: " << std::hex << (unsigned)RX_BUF.at(1) << '\n';

    unsigned reading{0};
    RX_BUF[1] &= 0xef;
    reading = RX_BUF[1];
    reading <<= 8;
    reading |= RX_BUF[0];

    set_cs_pin(true);

    return reading;
}

temp_t adc_temp::read_temp()
{
    const auto adc_reading = get_adc_reading();
    if (!adc_reading)
        return 0;

    // parse out temperature value


    return {};
}
