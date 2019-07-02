#include "adc_temp.hpp"

#include <sstream>
#include <exception>
#include <thread>
#include <chrono>

#include <pigpio.h>

#include "therm_lookup.hpp"

#include <map>

static const std::map<float,int> THERMISTOR_LOOKUP
{
    {	157.2	,	-30	},
    {	148.1	,	-29	},
    {	139.4	,	-28	},
    {	131.3	,	-27	},
    {	123.7	,	-26	},
    {	116.6	,	-25	},
    {	110	,	-24	},
    {	103.7	,	-23	},
    {	97.9	,	-22	},
    {	92.5	,	-21	},
    {	87.43	,	-20	},
    {	82.79	,	-19	},
    {	78.44	,	-18	},
    {	74.36	,	-17	},
    {	70.53	,	-16	},
    {	66.92	,	-15	},
    {	63.54	,	-14	},
    {	60.34	,	-13	},
    {	57.33	,	-12	},
    {	54.5	,	-11	},
    {	51.82	,	-10	},
    {	49.28	,	-9	},
    {	46.89	,	-8	},
    {	44.62	,	-7	},
    {	42.48	,	-6	},
    {	40.45	,	-5	},
    {	38.53	,	-4	},
    {	36.7	,	-3	},
    {	34.97	,	-2	},
    {	33.33	,	-1	},
    {	31.77	,	0	},
    {	30.25	,	1	},
    {	28.82	,	2	},
    {	27.45	,	3	},
    {	26.16	,	4	},
    {	24.94	,	5	},
    {	23.77	,	6	},
    {	22.67	,	7	},
    {	21.62	,	8	},
    {	20.63	,	9	},
    {	19.68	,	10	},
    {	18.78	,	11	},
    {	17.93	,	12	},
    {	17.12	,	13	},
    {	16.35	,	14	},
    {	15.62	,	15	},
    {	14.93	,	16	},
    {	14.26	,	17	},
    {	13.63	,	18	},
    {	13.04	,	19	},
    {	12.47	,	20	},
    {	11.92	,	21	},
    {	11.41	,	22	},
    {	10.91	,	23	},
    {	10.45	,	24	},
    {	10	,	25	},
    {	9.575	,	26	},
    {	9.17	,	27	},
    {	8.784	,	28	},
    {	8.416	,	29	},
    {	8.064	,	30	},
    {	7.73	,	31	},
    {	7.41	,	32	},
    {	7.106	,	33	},
    {	6.815	,	34	},
    {	6.538	,	35	},
    {	6.273	,	36	},
    {	6.02	,	37	},
    {	5.778	,	38	},
    {	5.548	,	39	},
    {	5.327	,	40	},
    {	5.117	,	41	},
    {	4.915	,	42	},
    {	4.723	,	43	},
    {	4.539	,	44	},
    {	4.363	,	45	},
    {	4.195	,	46	},
    {	4.034	,	47	},
    {	3.88	,	48	},
    {	3.733	,	49	},
    {	3.592	,	50	},
    {	3.457	,	51	},
    {	3.328	,	52	},
    {	3.204	,	53	},
    {	3.086	,	54	},
    {	2.972	,	55	},
    {	2.863	,	56	},
    {	2.759	,	57	},
    {	2.659	,	58	},
    {	2.564	,	59	},
    {	2.472	,	60	},
    {	2.384	,	61	},
    {	2.299	,	62	},
    {	2.218	,	63	},
    {	2.141	,	64	},
    {	2.066	,	65	},
    {	1.994	,	66	},
    {	1.926	,	67	},
    {	1.86	,	68	},
    {	1.796	,	69	},
    {	1.735	,	70	},
    {	1.677	,	71	},
    {	1.621	,	72	},
    {	1.567	,	73	},
    {	1.515	,	74	},
    {	1.465	,	75	},
    {	1.417	,	76	},
    {	1.371	,	77	},
    {	1.326	,	78	},
    {	1.284	,	79	},
    {	1.243	,	80	},
    {	1.203	,	81	},
    {	1.165	,	82	},
    {	1.128	,	83	},
    {	1.093	,	84	},
    {	1.059	,	85	},
    {	1.027	,	86	},
    {	0.9955	,	87	},
    {	0.9654	,	88	},
    {	0.9363	,	89	},
    {	0.9083	,	90	},
    {	0.8812	,	91	},
    {	0.855	,	92	},
    {	0.8297	,	93	},
    {	0.8052	,	94	},
    {	0.7816	,	95	},
    {	0.7587	,	96	},
    {	0.7366	,	97	},
    {	0.7152	,	98	},
    {	0.6945	,	99	},
    {	0.6744	,	100	}
};

static constexpr unsigned DEFAULT_SPI_BAUDRATE{500'000};
static constexpr unsigned DEFAULT_SPI_CHANNEL{0};
static constexpr unsigned DEFAULT_SPI_FLAGS{0};
static constexpr unsigned DEFAULT_CS_GPIO{22};

static constexpr float REFERENCE_RESISTOR_KOHMS{8.2};
static constexpr float REFERENCE_VOLTAGE{3.3};

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

    const float analog_voltage = (float)adc_reading * REFERENCE_VOLTAGE / 1024.0;
    const float thermistor_value = REFERENCE_RESISTOR_KOHMS * analog_voltage / (REFERENCE_VOLTAGE - analog_voltage);
    
    // parse out temperature value

    auto therm_it = THERMISTOR_LOOKUP.lower_bound(thermistor_value);
    if (therm_it == THERMISTOR_LOOKUP.end())
        return {};
    else
        return therm_it->second * 1000;
}
