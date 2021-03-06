/*
 * This file is protected by Copyright. Please refer to the COPYRIGHT file
 * distributed with this source distribution.
 *
 * This file is part of OpenCPI <http://www.opencpi.org>
 *
 * OpenCPI is free software: you can redistribute it and/or modify it under the
 * terms of the GNU Lesser General Public License as published by the Free
 * Software Foundation, either version 3 of the License, or (at your option) any
 * later version.
 *
 * OpenCPI is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
 * A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more
 * details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

/*
 * THIS FILE WAS ORIGINALLY GENERATED ON Thu Feb 16 18:00:49 2017 EST
 * BASED ON THE FILE: ad9361_config_proxy.xml
 * YOU *ARE* EXPECTED TO EDIT IT
 *
 * This file contains the implementation skeleton for the ad9361_config_proxy worker in C++
 */

#include <sstream> // needed for std::ostringstream
#include "ad9361_config_proxy-worker.hh"

extern "C" {
#include "ad9361_api.h"
#include "parameters.h"
int32_t spi_init(OCPI::RCC::RCCUserSlave* _slave);
}

using namespace OCPI::RCC; // for easy access to RCC data types and constants
using namespace Ad9361_config_proxyWorkerTypes;

#define CHECK_IF_POINTER_IS_NULL(p)        checkIfPointerIsNull(p, #p)

// LIBAD9361_API_INIT                 used for libad9361 calls to ad9361_init()
// LIBAD9361_API_1PARAM(pfun, a1)     used for libad9361 calls with 1 parameter (no parameter printing)
// LIBAD9361_API_1PARAMP(pfun, a1)    used for libad9361 calls with 1 parameter, with parameter printing
// LIBAD9361_API_1PARAMV(pfun, a1)    used for libad9361 calls with 1 parameter and return type of void
// LIBAD9361_API_2PARAM(pfun, a1, a2) used for libad9361 calls with 2 parameters (no parameter printing)
// LIBAD9361_API_CHAN_GET(pfun, a1)   used for libad9361 multichannel get calls
// LIBAD9361_API_CHAN_GETN(pfun, a1)  used for libad9361 multichannel get calls where the second channel should be disabled when ad9361_phy->pdata->rx2tx2 == 0
// LIBAD9361_API_CHAN_SET(pfun, a1)   used for libad9361 multichannel set calls
#define LIBAD9361_API_INIT(pfun,a1)        libad9361_API_init(pfun, a1, #pfun)
#define LIBAD9361_API_1PARAM(pfun, a1)     libad9361_API_1param(pfun, a1, #pfun)
#define LIBAD9361_API_1PARAMP(pfun, a1)    libad9361_API_1paramp(pfun, a1, #pfun)
#define LIBAD9361_API_1PARAMV(pfun, a1)    libad9361_API_1paramv(pfun, a1, #pfun)
#define LIBAD9361_API_2PARAM(pfun, a1, a2) libad9361_API_2param(pfun, a1, a2, #pfun)
#define LIBAD9361_API_CHAN_GET(pfun, a1)   libad9361_API_chan_get(pfun, a1, #pfun)
#define LIBAD9361_API_CHAN_GETN(pfun, a1)  libad9361_API_chan_get(pfun, a1, #pfun, true)
#define LIBAD9361_API_CHAN_SET(pfun, a1)   libad9361_API_chan_set(pfun, a1, #pfun)

class Ad9361_config_proxyWorker : public Ad9361_config_proxyWorkerBase {

  // note that RX_FAST_LOCK_CONFIG_WORD_NUM is in fact applicable to both
  // RX and TX faslock configs
  typedef struct fastlock_profile_s {
    uint32_t id;
    uint8_t values[RX_FAST_LOCK_CONFIG_WORD_NUM];
  } fastlock_profile_t;

  std::vector<fastlock_profile_t>::iterator find_worker_fastlock_profile(
      uint32_t id, std::vector<fastlock_profile_t>& vec)
  {
    for (std::vector<fastlock_profile_t>::iterator it = vec.begin();
         it != vec.end(); ++it)
    {
      if(it->id == id) return it;
    }
    return vec.end();
  }

  std::vector<fastlock_profile_t> m_rx_fastlock_profiles;
  std::vector<fastlock_profile_t> m_tx_fastlock_profiles;
  struct ad9361_rf_phy *ad9361_phy;
  AD9361_InitParam m_default_init_param;

  template<typename T> RCCResult
  checkIfPointerIsNull(T* p, const char* pChar) {
    if(p == ((T*)0))
    {
      std::ostringstream err;
      std::string pStr(pChar);
      err << pStr << " pointer was null";
      return setError(err.str().c_str());
    }
    return RCC_OK;
  }

  template<typename T> void
  libad9361_API_print_idk(std::string functionStr, T param,
                      uint8_t chan = 255) {
    // typical use results in leading '&' on functionStr, erase for prettiness
    std::string functionStdStr(functionStr);
    if(functionStdStr[0] == '&') functionStdStr.erase(functionStdStr.begin());

    if(AD9361_CONFIG_PROXY_OCPI_DEBUG)
    {
      printf("DEBUG: ad9361_config_proxy: libad9361 API call: ");
      // we don't know how to parameters for this function, so just printing ...
      printf("%s(...)\n", functionStdStr.c_str());
    }
  }

  template<typename T> void
  libad9361_API_print(std::string functionStr, T param,
                      uint8_t chan = 255) {
    // typical use results in leading '&' on functionStr, erase for prettiness
    std::string functionStdStr(functionStr);
    if(functionStdStr[0] == '&') functionStdStr.erase(functionStdStr.begin());
    if(AD9361_CONFIG_PROXY_OCPI_DEBUG)
    {
      std::ostringstream paramStr;
      paramStr << (long long) param;
      printf("DEBUG: ad9361_config_proxy: libad9361 API call: ");
      if(chan != 255)
      {
        printf("%s(ad9361_phy, %i, %s)\n", functionStdStr.c_str(),
               chan, paramStr.str().c_str());
      }
      else
      {
        printf("%s(ad9361_phy, %s)\n", functionStdStr.c_str(),
               paramStr.str().c_str());
      }
    }
  }

  /* @brief Function that should be used to make the ad9361_init() API call.
   * @param[in]   function    libad9361 API function pointer
   * @param[in]   param       parameter for ad9361 API function
   * @param[in]   functionStr Stringified function name
   ****************************************************************************/
  RCCResult
  libad9361_API_init(int32_t function(struct ad9361_rf_phy**, AD9361_InitParam*),
      AD9361_InitParam* param, const char* functionStr) {
    libad9361_API_print_idk(functionStr, param);
    
    const uint8_t iostandard    = (uint8_t)slave.get_iostandard();
    const uint8_t port_config   = (uint8_t)slave.get_port_config();
    const uint8_t duplex_config = (uint8_t)slave.get_duplex_config();
    const uint8_t drs           = (uint8_t)slave.get_data_rate_config();
    const bool modeIsCMOS       = (iostandard    == 0);
    const bool modeIsFullDuplex = (duplex_config == 1);
    const bool modeIsDualPort   = (port_config   == 1);
    const bool modeIsDDR        = (drs           == 1);

    param->lvds_rx_onchip_termination_enable = modeIsCMOS ? 0 : 1;

    // FDD is NOT synonymous with full duplex port mode! - e.g., LVDS (which is
    // full duplex) supports TDD
    //param->frequency_division_duplex_mode_enable = ;

    ///-----SPI Register 0x010-Parallel Port Configuration 1-----///
    // D7-PP Tx Swap IQ
    // passed in via param variable

    // D6-PP Rx Swap IQ
    // passed in via param variable

    // D5-TX Channel Swap
    // passed in via param variable

    // D4-RX Channel Swap
    // passed in via param variable

    //const uint8_t rx_ch_swap    = (uint8_t)slave.get_channels_are_swapped();
    //const bool dontCareIfChansSwapped = (rx_ch_swap >= 2);
    //if(!dontCareIfChansSwapped)
    //{
    //  const bool channelsAreSwapped = (rx_ch_swap == 1);
    //  param->rx_channel_swap_enable = channelsAreSwapped ? 1 : 0;
    //}

    // D3-RX Frame Pulse Mode
    const uint8_t rx_fr_usage   = (uint8_t)slave.get_rx_frame_usage();
    const bool rxFrameUsageIsToggle = (rx_fr_usage == 1);
    param->rx_frame_pulse_mode_enable = rxFrameUsageIsToggle ? 1 : 0;

    // D2-2R2T Timing
    // passed in via param variable

    // D1-Invert Data Bus
    const uint8_t data_bus_idx_inv = (uint8_t)slave.get_data_bus_index_direction();
    const bool dataBusIdxDirectionIsInverted = (data_bus_idx_inv == 1);
    param->invert_data_bus_enable = dataBusIdxDirectionIsInverted ? 1 : 0;

    // D0-Invert DATA_CLK
    const uint8_t data_clk_inv = (uint8_t)slave.get_data_clk_is_inverted();
    const bool dataClkIsInverted = (data_clk_inv == 1);
    param->invert_data_clk_enable = dataClkIsInverted ? 1 : 0;

    ///-----SPI Register 0x011-Parallel Port Configuration 2-----///
    //D7-FDD Alt Word Order
#define FDD_ALT_WORD_ORDER_ENABLE 0 ///@TODO / FIXME read this value from FPGA, I think we want 0 for now ??
    param->fdd_alt_word_order_enable = FDD_ALT_WORD_ORDER_ENABLE;

    // D1:D0-Delay Rx Data[1:0]
    // passed in via param variable

    //D2-Invert Rx Frame
    const uint8_t rx_frame_inv = (uint8_t)slave.get_rx_frame_is_inverted();
    const bool rxFrameIsInverted = (rx_frame_inv == 1);
    param->invert_rx_frame_enable = rxFrameIsInverted ? 1 : 0;
    
    ///-----SPI Register 0x012-Parallel Port Configuration 3-----///
    // D7-FDD RX Rate=2*Tx Rate
    // I think we want to leave the initialization-time value default and rely
    // on runtime libad9361 API to set this
    
    // D6-Swap Ports
    const uint8_t p0_p1_are_swapped = (uint8_t)slave.get_p0_p1_are_swapped();
    const bool portsAreSwapped = (p0_p1_are_swapped == 1);
    param->swap_ports_enable = modeIsCMOS ? (portsAreSwapped ? 1 : 0) : 0;
    
    // D4-LVDS Mode
    param->lvds_mode_enable = modeIsCMOS ? 0 : 1;
    if(param->lvds_mode_enable == 1)
    {
      param->half_duplex_mode_enable = 0; // D3-Half-Duplex Mode
      param->single_data_rate_enable = 0; // D5-Single Data Rate
      param->single_port_mode_enable = 0; // D2-Single Port Mode
    }
    else
    {
      param->half_duplex_mode_enable = modeIsFullDuplex ? 0 : 1; // D3-Half-Duplex Mode
      param->single_data_rate_enable = modeIsDDR        ? 0 : 1; // D5-Single Data Rate
      param->single_port_mode_enable = modeIsDualPort   ? 0 : 1; // D2-Single Port Mode
    }
    
    // D1-Full Port
    if((param->half_duplex_mode_enable == 0) &&
       (param->single_port_mode_enable == 0))
    {
      param->full_port_enable = 1;
    }
    else
    {
      param->full_port_enable = 0;
    }
    if(!modeIsCMOS) param->full_port_enable = 0; // not sure why this is needed, but it's how ADI's code works...
 
    // D0-Full Duplex Swap Bit
    // not sure how to use this, leaving initialization-time value default for now
    
    // ADI's UG-570 (DUAL PORT FULL DUPLEX MODE (LVDS) paragraph:
    // "For a system with a 2R1T or a 1R2T configuration, the clock
    // frequencies, bus transfer rates and sample periods, and data
    // capture timing are the same as if configured for a 2R2T system."
    if(!modeIsCMOS)
    {
      // 1 indicates second chan (index starts at 0)
      const bool qadc1_is_present = slave.get_qadc1_is_present();
      const bool qdac1_is_present = slave.get_qdac1_is_present();

      param->two_rx_two_tx_mode_enable = (qadc1_is_present || qdac1_is_present) ?
                                         1 : 0;
    }

    //init_param.two_rx_two_tx_mode_enable =
    //    m_properties.ad9361_init.two_rx_two_tx_mode_enable;

    // assign param->gpio_resetb to the arbitrarily defined GPIO_RESET_PIN so
    // that the platform driver knows to drive the force_reset property of the
    // sub-device
    param->gpio_resetb = GPIO_RESET_PIN;

    if(AD9361_CONFIG_PROXY_OCPI_DEBUG)
    {
      std::cout << "DEBUG: ad9361_config_proxy: param->pp_tx_swap_enable = "            <<
                      (int)param->pp_tx_swap_enable                << std::endl;
      std::cout << "DEBUG: ad9361_config_proxy: param->pp_rx_swap_enable = "            <<
                      (int)param->pp_rx_swap_enable                << std::endl;
      std::cout << "DEBUG: ad9361_config_proxy: param->tx_channel_swap_enable = "       <<
                      (int)param->tx_channel_swap_enable           << std::endl;
      std::cout << "DEBUG: ad9361_config_proxy: param->rx_channel_swap_enable = "       <<
                      (int)param->rx_channel_swap_enable           << std::endl;
      std::cout << "DEBUG: ad9361_config_proxy: param->rx_frame_pulse_mode_enable = "   <<
                      (int)param->rx_frame_pulse_mode_enable       << std::endl;
      std::cout << "DEBUG: ad9361_config_proxy: param->two_t_two_r_timing_enable = "    <<
                      (int)param->two_t_two_r_timing_enable        << std::endl;
      std::cout << "DEBUG: ad9361_config_proxy: param->invert_data_bus_enable = "       <<
                      (int)param->invert_data_bus_enable           << std::endl;
      std::cout << "DEBUG: ad9361_config_proxy: param->invert_data_clk_enable = "       <<
                      (int)param->invert_data_clk_enable           << std::endl;
      std::cout << "DEBUG: ad9361_config_proxy: param->fdd_alt_word_order_enable = "    <<
                      (int)param->fdd_alt_word_order_enable        << std::endl;
      std::cout << "DEBUG: ad9361_config_proxy: param->invert_rx_frame_enable = "       <<
                      (int)param->invert_rx_frame_enable           << std::endl;
      std::cout << "DEBUG: ad9361_config_proxy: param->fdd_rx_rate_2tx_enable = "       <<
                      (int)param->fdd_rx_rate_2tx_enable           << std::endl;
      std::cout << "DEBUG: ad9361_config_proxy: param->swap_ports_enable = "            <<
                      (int)param->swap_ports_enable                << std::endl;
      std::cout << "DEBUG: ad9361_config_proxy: param->single_data_rate_enable = "      <<
                      (int)param->single_data_rate_enable          << std::endl;
      std::cout << "DEBUG: ad9361_config_proxy: param->lvds_mode_enable = "             <<
                      (int)param->lvds_mode_enable                 << std::endl;
      std::cout << "DEBUG: ad9361_config_proxy: param->half_duplex_mode_enable = "      <<
                      (int)param->half_duplex_mode_enable          << std::endl;
      std::cout << "DEBUG: ad9361_config_proxy: param->single_port_mode_enable = "      <<
                      (int)param->single_port_mode_enable          << std::endl;
      std::cout << "DEBUG: ad9361_config_proxy: param->full_port_enable = "             <<
                      (int)param->full_port_enable                 << std::endl;
      std::cout << "DEBUG: ad9361_config_proxy: param->full_duplex_swap_bits_enable = " <<
                      (int)param->full_duplex_swap_bits_enable     << std::endl;
      std::cout << "DEBUG: ad9361_config_proxy: param->delay_rx_data = "                <<
                      (int)param->delay_rx_data                    << std::endl;
      std::cout << "DEBUG: ad9361_config_proxy: param->rx_data_clock_delay = "          <<
                      (int)param->rx_data_clock_delay              << std::endl;
      std::cout << "DEBUG: ad9361_config_proxy: param->rx_data_delay = "                <<
                      (int)param->rx_data_delay                    << std::endl;
      std::cout << "DEBUG: ad9361_config_proxy: param->tx_fb_clock_delay= "             <<
                      (int)param->tx_fb_clock_delay                << std::endl;
      std::cout << "DEBUG: ad9361_config_proxy: param->tx_data_delay = "                <<
                      (int)param->tx_data_delay                    << std::endl;
    }

    slave.set_force_two_r_two_t_timing(param->two_t_two_r_timing_enable);

    // note that it's okay for ad9361_phy to be a null pointer prior
    // to calling function()
    const int32_t res = function(&ad9361_phy, param);
    RCCResult ret = CHECK_IF_POINTER_IS_NULL(ad9361_phy);
    if(ret != RCC_OK) return ret;
    if(res != 0)
    {
      std::ostringstream err;
      // typical use results in leading '&' on functionStr, erase for prettiness
      std::string functionStdStr(functionStr);
      if(functionStdStr[0] == '&') functionStdStr.erase(functionStdStr.begin());
      err << functionStdStr << "() returned: " << res;
      return setError(err.str().c_str());
    }
    set_FPGA_channel_config(); // because channel config potentially changed

    return RCC_OK;
  }
  
  RCCResult ad9361_init_check(AD9361_InitParam init_param,
                              const bool force_init = false)
  {
    static bool ad9361_init_called = false;
    if((!ad9361_init_called) || force_init)
    {
      ad9361_init_called = true;
      return LIBAD9361_API_INIT(&ad9361_init, &init_param);
    }
    return RCC_OK;
  }

  inline RCCResult ad9361_pre_API_call_validation(AD9361_InitParam init_param,
                                                  const bool force_init = false)
  {
    RCCResult ret = ad9361_init_check(init_param, force_init);
    if(ret != RCC_OK) return ret;
    ret  = CHECK_IF_POINTER_IS_NULL(ad9361_phy->pdata);
    if(ret != RCC_OK) return ret;
    ret = CHECK_IF_POINTER_IS_NULL(ad9361_phy);
    return ret;
  }

  /* @brief Function that should be used to make channel-agnostic libad9361 API
   *        calls with 1 parameter with most common return type of int32_t. Also
   *        performs debug printing of function call and passed parameters when
   *        AD9361_CONFIG_PROXY_OCPI_DEBUG defined.
   * @param[in]   function    libad9361 API function pointer
   * @param[in]   param       second and last parameter for ad9361 API function
   * @param[in]   functionStr Stringified function name
   ****************************************************************************/
  template<typename T> RCCResult
  libad9361_API_1paramp(int32_t function(struct ad9361_rf_phy*, T),
      T param, const char* functionStr) {
    libad9361_API_print(functionStr, param);
    return libad9361_API_1param(function, param, functionStr, false);
  }

  /* @brief Function that should be used to make channel-agnostic libad9361 API
   *        calls with 1 parameter with most common return type of int32_t.
   * @param[in]   function    libad9361 API function pointer
   * @param[in]   param       second and last parameter for ad9361 API function
   * @param[in]   functionStr Stringified function name
   * @param[in]   doPrint     Enables debugging printing of function call and
   *                          passed parameters (when
   *                          AD9361_CONFIG_PROXY_OCPI_DEBUG is defined).
   *                          Default is true.
   ****************************************************************************/
  template<typename T> RCCResult
  libad9361_API_1param(int32_t function(struct ad9361_rf_phy*, T),
      T param, const char* functionStr, bool doPrint = true) {
    if (doPrint) libad9361_API_print_idk(functionStr, param);

    RCCResult ret = ad9361_pre_API_call_validation(m_default_init_param);
    if(ret != RCC_OK) return ret;

    const int32_t res = function(ad9361_phy, param);
    if(res != 0)
    {
      std::ostringstream err;
      // typical use results in leading '&' on functionStr, erase for prettiness
      std::string functionStdStr(functionStr);
      if(functionStdStr[0] == '&') functionStdStr.erase(functionStdStr.begin());
      err << functionStdStr << "() returned: " << res;
      return setError(err.str().c_str());
    }
    return RCC_OK;
  }

  /* @brief Function that should be used to make channel-agnostic libad9361 API
   *        calls with 1 parameter with return type of void. Also performs debug
   *        printing of function call and passed parameters when
   *        AD9361_CONFIG_PROXY_OCPI_DEBUG defined.
   * @param[in]   function    libad9361 API function pointer
   * @param[in]   param       second and last parameter for ad9361 API function
   * @param[in]   functionStr Stringified function name
   ****************************************************************************/
  template<typename T> RCCResult
  libad9361_API_1paramv(void function(struct ad9361_rf_phy*, T),
      T param, const char* functionStr) {
    libad9361_API_print_idk(functionStr, param);

    RCCResult ret = ad9361_pre_API_call_validation(m_default_init_param);
    if(ret != RCC_OK) return ret;

    function(ad9361_phy, param);
    return RCC_OK;
  }

  /* @brief Function that should be used to make channel-agnostic libad9361 API
   *        calls with 2 parameters. Also performs debug printing of function
   *        call and passed parameters when AD9361_CONFIG_PROXY_OCPI_DEBUG
   *        defined.
   * @param[in]   function    libad9361 API function pointer
   * @param[in]   param1      second parameter for ad9361 API function
   * @param[in]   param2      third and last parameter for ad9361 API function
   * @param[in]   functionStr Stringified function name
   ****************************************************************************/
  template<typename T, typename R> RCCResult
  libad9361_API_2param(int32_t function(struct ad9361_rf_phy*, T, R),
      T param1, R param2,
      const char* functionStr) {
    libad9361_API_print_idk(functionStr, param1);

    RCCResult ret = ad9361_pre_API_call_validation(m_default_init_param);
    if(ret != RCC_OK) return ret;

    const int32_t res = function(ad9361_phy, param1, param2);
    if(res != 0)
    {
      std::ostringstream err;
      // typical use results in leading '&' on functionStr, erase for prettiness
      std::string functionStdStr(functionStr);
      if(functionStdStr[0] == '&') functionStdStr.erase(functionStdStr.begin());
      err << functionStdStr << "() returned: " << res;
      return setError(err.str().c_str());
    }
    return RCC_OK;
  }

  /* @brief Function that should be used to make channel-dependent libad9361 API
   *        get calls. Also performs debug printing of function call and passed
   *        parameters when AD9361_CONFIG_PROXY_OCPI_DEBUG defined.
   * @param[in]   function    libad9361 API function pointer
   * @param[in]   param       Reference to variable sent as second and last
   *                          parameter for ad9361 API function
   * @param[in]   functionStr Stringified function name
   * @param[in]   ch1Disable  While channel 0 is always processing, channel 1
   *                          may be disabled by setting this to true. This
   *                          allows for handling of some libad9361 *_get_*
   *                          calls falling when ch=1 and
   *                          ad9361_phy->pdata->rx2tx2 == 0
   ****************************************************************************/
  template<typename T> RCCResult
  libad9361_API_chan_get(int32_t function(struct ad9361_rf_phy*,
      uint8_t chan, T*), T* param, const char* functionStr, bool ch1Disable = false) {
    for(uint8_t chan=0; chan<AD9361_CONFIG_PROXY_RX_NCHANNELS; chan++) {
      libad9361_API_print_idk(functionStr, param, chan);

      RCCResult ret = ad9361_pre_API_call_validation(m_default_init_param);
      if(ret != RCC_OK) return ret;

      const int32_t res = function(ad9361_phy, chan, param++);
      if(res != 0) return setError("%s() returned: %i", functionStr, res);
      if(ch1Disable)
      {
        if(ad9361_phy->pdata->rx2tx2 == 0)
        {
          break; // some _get_ calls fail when ch=1 and rx2tx2=0
        }
      }
    }
    return RCC_OK;
  }

  /* @brief Function that should be used to make channel-dependent libad9361 API
   *        set calls. Also performs debug printing of function call and passed
   *        parameters when AD9361_CONFIG_PROXY_OCPI_DEBUG defined.
   * @param[in]   function    libad9361 API function pointer
   * @param[in]   ad9361_phy  first parameter for ad9361 API function
   * @param[in]   param       Constant Reference to variable sent as second and
   *                          last parameter for ad9361 API function
   * @param[in]   functionStr Stringified function name
   ****************************************************************************/
  template<typename T> RCCResult
  libad9361_API_chan_set(int32_t function(struct ad9361_rf_phy*,
      uint8_t chan, T), T* param, const char* functionStr) {

    for(uint8_t chan=0; chan<AD9361_CONFIG_PROXY_RX_NCHANNELS; chan++) {
      libad9361_API_print_idk(functionStr, param, chan);

      RCCResult ret = ad9361_pre_API_call_validation(m_default_init_param);
      if(ret != RCC_OK) return ret;

      const int32_t res = function(ad9361_phy, chan, *param++);
      if(res != 0)
      {
        std::ostringstream err;
        // typical use results in leading '&' on functionStr, erase for prettiness
        std::string functionStdStr(functionStr);
        if(functionStdStr[0] == '&') functionStdStr.erase(functionStdStr.begin());
        err << functionStdStr << "() returned: " << res;
        return setError(err.str().c_str());
      }
      if(ad9361_phy->pdata->rx2tx2 == 0)
      {
        break; // all _set_ calls fail when ch=1 and rx2tx2=0
      }
    }
    return RCC_OK;
  }

  /* @brief The AD9361 register set determines which channel mode is used (1R1T,
   * 1R2T, 2R1T, or 1R2T). This mode eventually determines which timing diagram
   * the AD9361 is expecting for the TX data path pins. This function
   * tells the FPGA bitstream which channel mode should be assumed when
   * generating the TX data path signals.
   ***************************************************************************/
  void set_FPGA_channel_config(void) {
    uint8_t general_tx_enable_filter_ctrl =
        slave.get_general_tx_enable_filter_ctrl();
    bool two_t = TX_CHANNEL_ENABLE(TX_1 | TX_2) ==
        (general_tx_enable_filter_ctrl & 0xc0);
    uint8_t general_rx_enable_filter_ctrl =
        slave.get_general_rx_enable_filter_ctrl();
    bool two_r = RX_CHANNEL_ENABLE(RX_1 | RX_2) ==
        (general_rx_enable_filter_ctrl & 0xc0);
    slave.set_config_is_two_r(two_r);
    slave.set_config_is_two_t(two_t);
  }

  RCCResult initialize() {
    // start MODIFIED copy from no-OS main.c (modified to remove warnings only)
    AD9361_InitParam default_init_param = {
      /* Device selection */
      ID_AD9361,  // dev_sel
      /* Identification number */
      0,    //id_no
      /* Reference Clock */
      40000000UL,  //reference_clk_rate
      /* Base Configuration */
      1,    //two_rx_two_tx_mode_enable *** adi,2rx-2tx-mode-enable
      1,    //one_rx_one_tx_mode_use_rx_num *** adi,1rx-1tx-mode-use-rx-num
      1,    //one_rx_one_tx_mode_use_tx_num *** adi,1rx-1tx-mode-use-tx-num
      1,    //frequency_division_duplex_mode_enable *** adi,frequency-division-duplex-mode-enable
      0,    //frequency_division_duplex_independent_mode_enable *** adi,frequency-division-duplex-independent-mode-enable
      0,    //tdd_use_dual_synth_mode_enable *** adi,tdd-use-dual-synth-mode-enable
      0,    //tdd_skip_vco_cal_enable *** adi,tdd-skip-vco-cal-enable
      0,    //tx_fastlock_delay_ns *** adi,tx-fastlock-delay-ns
      0,    //rx_fastlock_delay_ns *** adi,rx-fastlock-delay-ns
      0,    //rx_fastlock_pincontrol_enable *** adi,rx-fastlock-pincontrol-enable
      0,    //tx_fastlock_pincontrol_enable *** adi,tx-fastlock-pincontrol-enable
      0,    //external_rx_lo_enable *** adi,external-rx-lo-enable
      0,    //external_tx_lo_enable *** adi,external-tx-lo-enable
      5,    //dc_offset_tracking_update_event_mask *** adi,dc-offset-tracking-update-event-mask
      6,    //dc_offset_attenuation_high_range *** adi,dc-offset-attenuation-high-range
      5,    //dc_offset_attenuation_low_range *** adi,dc-offset-attenuation-low-range
      0x28,  //dc_offset_count_high_range *** adi,dc-offset-count-high-range
      0x32,  //dc_offset_count_low_range *** adi,dc-offset-count-low-range
      0,    //split_gain_table_mode_enable *** adi,split-gain-table-mode-enable
      MAX_SYNTH_FREF,  //trx_synthesizer_target_fref_overwrite_hz *** adi,trx-synthesizer-target-fref-overwrite-hz
      0,    // qec_tracking_slow_mode_enable *** adi,qec-tracking-slow-mode-enable
      /* ENSM Control */
      0,    //ensm_enable_pin_pulse_mode_enable *** adi,ensm-enable-pin-pulse-mode-enable
      0,    //ensm_enable_txnrx_control_enable *** adi,ensm-enable-txnrx-control-enable
      /* LO Control */
      2400000000UL,  //rx_synthesizer_frequency_hz *** adi,rx-synthesizer-frequency-hz
      2400000000UL,  //tx_synthesizer_frequency_hz *** adi,tx-synthesizer-frequency-hz
      /* Rate & BW Control */
      {983040000, 245760000, 122880000, 61440000, 30720000, 30720000},//uint32_t  rx_path_clock_frequencies[6] *** adi,rx-path-clock-frequencies
      {983040000, 122880000, 122880000, 61440000, 30720000, 30720000},//uint32_t  tx_path_clock_frequencies[6] *** adi,tx-path-clock-frequencies
      18000000,//rf_rx_bandwidth_hz *** adi,rf-rx-bandwidth-hz
      18000000,//rf_tx_bandwidth_hz *** adi,rf-tx-bandwidth-hz
      /* RF Port Control */
      0,    //rx_rf_port_input_select *** adi,rx-rf-port-input-select
      0,    //tx_rf_port_input_select *** adi,tx-rf-port-input-select
      /* TX Attenuation Control */
      10000,  //tx_attenuation_mdB *** adi,tx-attenuation-mdB
      0,    //update_tx_gain_in_alert_enable *** adi,update-tx-gain-in-alert-enable
      /* Reference Clock Control */
      0,    //xo_disable_use_ext_refclk_enable *** adi,xo-disable-use-ext-refclk-enable
      {8, 5920},  //dcxo_coarse_and_fine_tune[2] *** adi,dcxo-coarse-and-fine-tune
      CLKOUT_DISABLE,  //clk_output_mode_select *** adi,clk-output-mode-select
      /* Gain Control */
      2,    //gc_rx1_mode *** adi,gc-rx1-mode
      2,    //gc_rx2_mode *** adi,gc-rx2-mode
      58,    //gc_adc_large_overload_thresh *** adi,gc-adc-large-overload-thresh
      4,    //gc_adc_ovr_sample_size *** adi,gc-adc-ovr-sample-size
      47,    //gc_adc_small_overload_thresh *** adi,gc-adc-small-overload-thresh
      8192,  //gc_dec_pow_measurement_duration *** adi,gc-dec-pow-measurement-duration
      0,    //gc_dig_gain_enable *** adi,gc-dig-gain-enable
      800,  //gc_lmt_overload_high_thresh *** adi,gc-lmt-overload-high-thresh
      704,  //gc_lmt_overload_low_thresh *** adi,gc-lmt-overload-low-thresh
      24,    //gc_low_power_thresh *** adi,gc-low-power-thresh
      15,    //gc_max_dig_gain *** adi,gc-max-dig-gain
      /* Gain MGC Control */
      2,    //mgc_dec_gain_step *** adi,mgc-dec-gain-step
      2,    //mgc_inc_gain_step *** adi,mgc-inc-gain-step
      0,    //mgc_rx1_ctrl_inp_enable *** adi,mgc-rx1-ctrl-inp-enable
      0,    //mgc_rx2_ctrl_inp_enable *** adi,mgc-rx2-ctrl-inp-enable
      0,    //mgc_split_table_ctrl_inp_gain_mode *** adi,mgc-split-table-ctrl-inp-gain-mode
      /* Gain AGC Control */
      10,    //agc_adc_large_overload_exceed_counter *** adi,agc-adc-large-overload-exceed-counter
      2,    //agc_adc_large_overload_inc_steps *** adi,agc-adc-large-overload-inc-steps
      0,    //agc_adc_lmt_small_overload_prevent_gain_inc_enable *** adi,agc-adc-lmt-small-overload-prevent-gain-inc-enable
      10,    //agc_adc_small_overload_exceed_counter *** adi,agc-adc-small-overload-exceed-counter
      4,    //agc_dig_gain_step_size *** adi,agc-dig-gain-step-size
      3,    //agc_dig_saturation_exceed_counter *** adi,agc-dig-saturation-exceed-counter
      1000,  // agc_gain_update_interval_us *** adi,agc-gain-update-interval-us
      0,    //agc_immed_gain_change_if_large_adc_overload_enable *** adi,agc-immed-gain-change-if-large-adc-overload-enable
      0,    //agc_immed_gain_change_if_large_lmt_overload_enable *** adi,agc-immed-gain-change-if-large-lmt-overload-enable
      10,    //agc_inner_thresh_high *** adi,agc-inner-thresh-high
      1,    //agc_inner_thresh_high_dec_steps *** adi,agc-inner-thresh-high-dec-steps
      12,    //agc_inner_thresh_low *** adi,agc-inner-thresh-low
      1,    //agc_inner_thresh_low_inc_steps *** adi,agc-inner-thresh-low-inc-steps
      10,    //agc_lmt_overload_large_exceed_counter *** adi,agc-lmt-overload-large-exceed-counter
      2,    //agc_lmt_overload_large_inc_steps *** adi,agc-lmt-overload-large-inc-steps
      10,    //agc_lmt_overload_small_exceed_counter *** adi,agc-lmt-overload-small-exceed-counter
      5,    //agc_outer_thresh_high *** adi,agc-outer-thresh-high
      2,    //agc_outer_thresh_high_dec_steps *** adi,agc-outer-thresh-high-dec-steps
      18,    //agc_outer_thresh_low *** adi,agc-outer-thresh-low
      2,    //agc_outer_thresh_low_inc_steps *** adi,agc-outer-thresh-low-inc-steps
      1,    //agc_attack_delay_extra_margin_us; *** adi,agc-attack-delay-extra-margin-us
      0,    //agc_sync_for_gain_counter_enable *** adi,agc-sync-for-gain-counter-enable
      /* Fast AGC */
      64,    //fagc_dec_pow_measuremnt_duration ***  adi,fagc-dec-pow-measurement-duration
      260,  //fagc_state_wait_time_ns ***  adi,fagc-state-wait-time-ns
      /* Fast AGC - Low Power */
      0,    //fagc_allow_agc_gain_increase ***  adi,fagc-allow-agc-gain-increase-enable
      5,    //fagc_lp_thresh_increment_time ***  adi,fagc-lp-thresh-increment-time
      1,    //fagc_lp_thresh_increment_steps ***  adi,fagc-lp-thresh-increment-steps
      /* Fast AGC - Lock Level */
      10,    //fagc_lock_level ***  adi,fagc-lock-level
      1,    //fagc_lock_level_lmt_gain_increase_en ***  adi,fagc-lock-level-lmt-gain-increase-enable
      5,    //fagc_lock_level_gain_increase_upper_limit ***  adi,fagc-lock-level-gain-increase-upper-limit
      /* Fast AGC - Peak Detectors and Final Settling */
      1,    //fagc_lpf_final_settling_steps ***  adi,fagc-lpf-final-settling-steps
      1,    //fagc_lmt_final_settling_steps ***  adi,fagc-lmt-final-settling-steps
      3,    //fagc_final_overrange_count ***  adi,fagc-final-overrange-count
      /* Fast AGC - Final Power Test */
      0,    //fagc_gain_increase_after_gain_lock_en ***  adi,fagc-gain-increase-after-gain-lock-enable
      /* Fast AGC - Unlocking the Gain */
      0,    //fagc_gain_index_type_after_exit_rx_mode ***  adi,fagc-gain-index-type-after-exit-rx-mode
      1,    //fagc_use_last_lock_level_for_set_gain_en ***  adi,fagc-use-last-lock-level-for-set-gain-enable
      1,    //fagc_rst_gla_stronger_sig_thresh_exceeded_en ***  adi,fagc-rst-gla-stronger-sig-thresh-exceeded-enable
      5,    //fagc_optimized_gain_offset ***  adi,fagc-optimized-gain-offset
      10,    //fagc_rst_gla_stronger_sig_thresh_above_ll ***  adi,fagc-rst-gla-stronger-sig-thresh-above-ll
      1,    //fagc_rst_gla_engergy_lost_sig_thresh_exceeded_en ***  adi,fagc-rst-gla-engergy-lost-sig-thresh-exceeded-enable
      1,    //fagc_rst_gla_engergy_lost_goto_optim_gain_en ***  adi,fagc-rst-gla-engergy-lost-goto-optim-gain-enable
      10,    //fagc_rst_gla_engergy_lost_sig_thresh_below_ll ***  adi,fagc-rst-gla-engergy-lost-sig-thresh-below-ll
      8,    //fagc_energy_lost_stronger_sig_gain_lock_exit_cnt ***  adi,fagc-energy-lost-stronger-sig-gain-lock-exit-cnt
      1,    //fagc_rst_gla_large_adc_overload_en ***  adi,fagc-rst-gla-large-adc-overload-enable
      1,    //fagc_rst_gla_large_lmt_overload_en ***  adi,fagc-rst-gla-large-lmt-overload-enable
      0,    //fagc_rst_gla_en_agc_pulled_high_en ***  adi,fagc-rst-gla-en-agc-pulled-high-enable
      0,    //fagc_rst_gla_if_en_agc_pulled_high_mode ***  adi,fagc-rst-gla-if-en-agc-pulled-high-mode
      64,    //fagc_power_measurement_duration_in_state5 ***  adi,fagc-power-measurement-duration-in-state5
      /* RSSI Control */
      1,    //rssi_delay *** adi,rssi-delay
      1000,  //rssi_duration *** adi,rssi-duration
      3,    //rssi_restart_mode *** adi,rssi-restart-mode
      0,    //rssi_unit_is_rx_samples_enable *** adi,rssi-unit-is-rx-samples-enable
      1,    //rssi_wait *** adi,rssi-wait
      /* Aux ADC Control */
      256,  //aux_adc_decimation *** adi,aux-adc-decimation
      40000000UL,  //aux_adc_rate *** adi,aux-adc-rate
      /* AuxDAC Control */
      1,    //aux_dac_manual_mode_enable ***  adi,aux-dac-manual-mode-enable
      0,    //aux_dac1_default_value_mV ***  adi,aux-dac1-default-value-mV
      0,    //aux_dac1_active_in_rx_enable ***  adi,aux-dac1-active-in-rx-enable
      0,    //aux_dac1_active_in_tx_enable ***  adi,aux-dac1-active-in-tx-enable
      0,    //aux_dac1_active_in_alert_enable ***  adi,aux-dac1-active-in-alert-enable
      0,    //aux_dac1_rx_delay_us ***  adi,aux-dac1-rx-delay-us
      0,    //aux_dac1_tx_delay_us ***  adi,aux-dac1-tx-delay-us
      0,    //aux_dac2_default_value_mV ***  adi,aux-dac2-default-value-mV
      0,    //aux_dac2_active_in_rx_enable ***  adi,aux-dac2-active-in-rx-enable
      0,    //aux_dac2_active_in_tx_enable ***  adi,aux-dac2-active-in-tx-enable
      0,    //aux_dac2_active_in_alert_enable ***  adi,aux-dac2-active-in-alert-enable
      0,    //aux_dac2_rx_delay_us ***  adi,aux-dac2-rx-delay-us
      0,    //aux_dac2_tx_delay_us ***  adi,aux-dac2-tx-delay-us
      /* Temperature Sensor Control */
      256,  //temp_sense_decimation *** adi,temp-sense-decimation
      1000,  //temp_sense_measurement_interval_ms *** adi,temp-sense-measurement-interval-ms
      (int8_t)0xCE,  //temp_sense_offset_signed *** adi,temp-sense-offset-signed
      1,    //temp_sense_periodic_measurement_enable *** adi,temp-sense-periodic-measurement-enable
      /* Control Out Setup */
      0xFF,  //ctrl_outs_enable_mask *** adi,ctrl-outs-enable-mask
      0,    //ctrl_outs_index *** adi,ctrl-outs-index
      /* External LNA Control */
      0,    //elna_settling_delay_ns *** adi,elna-settling-delay-ns
      0,    //elna_gain_mdB *** adi,elna-gain-mdB
      0,    //elna_bypass_loss_mdB *** adi,elna-bypass-loss-mdB
      0,    //elna_rx1_gpo0_control_enable *** adi,elna-rx1-gpo0-control-enable
      0,    //elna_rx2_gpo1_control_enable *** adi,elna-rx2-gpo1-control-enable
      0,    //elna_gaintable_all_index_enable *** adi,elna-gaintable-all-index-enable
      /* Digital Interface Control */
      0,    //digital_interface_tune_skip_mode *** adi,digital-interface-tune-skip-mode
      0,    //digital_interface_tune_fir_disable *** adi,digital-interface-tune-fir-disable
      1,    //pp_tx_swap_enable *** adi,pp-tx-swap-enable
      1,    //pp_rx_swap_enable *** adi,pp-rx-swap-enable
      0,    //tx_channel_swap_enable *** adi,tx-channel-swap-enable
      0,    //rx_channel_swap_enable *** adi,rx-channel-swap-enable
      1,    //rx_frame_pulse_mode_enable *** adi,rx-frame-pulse-mode-enable
      0,    //two_t_two_r_timing_enable *** adi,2t2r-timing-enable
      0,    //invert_data_bus_enable *** adi,invert-data-bus-enable
      0,    //invert_data_clk_enable *** adi,invert-data-clk-enable
      0,    //fdd_alt_word_order_enable *** adi,fdd-alt-word-order-enable
      0,    //invert_rx_frame_enable *** adi,invert-rx-frame-enable
      0,    //fdd_rx_rate_2tx_enable *** adi,fdd-rx-rate-2tx-enable
      0,    //swap_ports_enable *** adi,swap-ports-enable
      0,    //single_data_rate_enable *** adi,single-data-rate-enable
      1,    //lvds_mode_enable *** adi,lvds-mode-enable
      0,    //half_duplex_mode_enable *** adi,half-duplex-mode-enable
      0,    //single_port_mode_enable *** adi,single-port-mode-enable
      0,    //full_port_enable *** adi,full-port-enable
      0,    //full_duplex_swap_bits_enable *** adi,full-duplex-swap-bits-enable
      0,    //delay_rx_data *** adi,delay-rx-data
      0,    //rx_data_clock_delay *** adi,rx-data-clock-delay
      4,    //rx_data_delay *** adi,rx-data-delay
      7,    //tx_fb_clock_delay *** adi,tx-fb-clock-delay
      0,    //tx_data_delay *** adi,tx-data-delay
      150,  //lvds_bias_mV *** adi,lvds-bias-mV
      1,    //lvds_rx_onchip_termination_enable *** adi,lvds-rx-onchip-termination-enable
      0,    //rx1rx2_phase_inversion_en *** adi,rx1-rx2-phase-inversion-enable
      0xFF,  //lvds_invert1_control *** adi,lvds-invert1-control
      0x0F,  //lvds_invert2_control *** adi,lvds-invert2-control
      /* GPO Control */
      0,    //gpo0_inactive_state_high_enable *** adi,gpo0-inactive-state-high-enable
      0,    //gpo1_inactive_state_high_enable *** adi,gpo1-inactive-state-high-enable
      0,    //gpo2_inactive_state_high_enable *** adi,gpo2-inactive-state-high-enable
      0,    //gpo3_inactive_state_high_enable *** adi,gpo3-inactive-state-high-enable
      0,    //gpo0_slave_rx_enable *** adi,gpo0-slave-rx-enable
      0,    //gpo0_slave_tx_enable *** adi,gpo0-slave-tx-enable
      0,    //gpo1_slave_rx_enable *** adi,gpo1-slave-rx-enable
      0,    //gpo1_slave_tx_enable *** adi,gpo1-slave-tx-enable
      0,    //gpo2_slave_rx_enable *** adi,gpo2-slave-rx-enable
      0,    //gpo2_slave_tx_enable *** adi,gpo2-slave-tx-enable
      0,    //gpo3_slave_rx_enable *** adi,gpo3-slave-rx-enable
      0,    //gpo3_slave_tx_enable *** adi,gpo3-slave-tx-enable
      0,    //gpo0_rx_delay_us *** adi,gpo0-rx-delay-us
      0,    //gpo0_tx_delay_us *** adi,gpo0-tx-delay-us
      0,    //gpo1_rx_delay_us *** adi,gpo1-rx-delay-us
      0,    //gpo1_tx_delay_us *** adi,gpo1-tx-delay-us
      0,    //gpo2_rx_delay_us *** adi,gpo2-rx-delay-us
      0,    //gpo2_tx_delay_us *** adi,gpo2-tx-delay-us
      0,    //gpo3_rx_delay_us *** adi,gpo3-rx-delay-us
      0,    //gpo3_tx_delay_us *** adi,gpo3-tx-delay-us
      /* Tx Monitor Control */
      37000,  //low_high_gain_threshold_mdB *** adi,txmon-low-high-thresh
      0,    //low_gain_dB *** adi,txmon-low-gain
      24,    //high_gain_dB *** adi,txmon-high-gain
      0,    //tx_mon_track_en *** adi,txmon-dc-tracking-enable
      0,    //one_shot_mode_en *** adi,txmon-one-shot-mode-enable
      511,  //tx_mon_delay *** adi,txmon-delay
      8192,  //tx_mon_duration *** adi,txmon-duration
      2,    //tx1_mon_front_end_gain *** adi,txmon-1-front-end-gain
      2,    //tx2_mon_front_end_gain *** adi,txmon-2-front-end-gain
      48,    //tx1_mon_lo_cm *** adi,txmon-1-lo-cm
      48,    //tx2_mon_lo_cm *** adi,txmon-2-lo-cm
      /* GPIO definitions */
      -1,    //gpio_resetb *** reset-gpios
      /* MCS Sync */
      -1,    //gpio_sync *** sync-gpios
      -1,    //gpio_cal_sw1 *** cal-sw1-gpios
      -1,    //gpio_cal_sw2 *** cal-sw2-gpios
      /* External LO clocks */
      NULL,  //(*ad9361_rfpll_ext_recalc_rate)()
      NULL,  //(*ad9361_rfpll_ext_round_rate)()
      NULL  //(*ad9361_rfpll_ext_set_rate)()
    };
    // end MODIFIED copy from no-OS main.c (modified to remove warnings only)
    m_default_init_param = default_init_param;

    // nasty cast below included since compiler wouldn't let us cast from
    // Ad9361_config_proxyWorkerTypes::Ad9361_config_proxyWorkerBase::Slave to
    // OCPI::RCC_RCCUserSlave since the former inherits privately from the
    // latter inside this worker's generated header
    spi_init(static_cast<OCPI::RCC::RCCUserSlave*>(static_cast<void *>(&slave)));

    m_rx_fastlock_profiles.clear();
    m_tx_fastlock_profiles.clear();

    return RCC_OK;
  }

  // notification that ad9361_init property has been written
  RCCResult ad9361_init_written() {
    AD9361_InitParam init_param = m_default_init_param;
    init_param.reference_clk_rate =
        m_properties.ad9361_init.reference_clk_rate;
    init_param.one_rx_one_tx_mode_use_rx_num =
        m_properties.ad9361_init.one_rx_one_tx_mode_use_rx_num;
    init_param.one_rx_one_tx_mode_use_tx_num =
        m_properties.ad9361_init.one_rx_one_tx_mode_use_tx_num;
    init_param.frequency_division_duplex_mode_enable =
        m_properties.ad9361_init.frequency_division_duplex_mode_enable;
    init_param.xo_disable_use_ext_refclk_enable =
        m_properties.ad9361_init.xo_disable_use_ext_refclk_enable;
    init_param.two_t_two_r_timing_enable =
        m_properties.ad9361_init.two_t_two_r_timing_enable;
    init_param.pp_tx_swap_enable =
        m_properties.ad9361_init.pp_tx_swap_enable;
    init_param.pp_rx_swap_enable =
        m_properties.ad9361_init.pp_rx_swap_enable;
    init_param.tx_channel_swap_enable =
        m_properties.ad9361_init.tx_channel_swap_enable;
    init_param.rx_channel_swap_enable =
        m_properties.ad9361_init.rx_channel_swap_enable;
    init_param.delay_rx_data =
        m_properties.ad9361_init.delay_rx_data;
    init_param.rx_data_clock_delay=
        m_properties.ad9361_init.rx_data_clock_delay;
    init_param.rx_data_delay =
        m_properties.ad9361_init.rx_data_delay;
    init_param.tx_fb_clock_delay =
        m_properties.ad9361_init.tx_fb_clock_delay;
    init_param.tx_data_delay =
        m_properties.ad9361_init.tx_data_delay;
    return ad9361_pre_API_call_validation(init_param, true);
  }
  // notification that ad9361_rf_phy property will be read
  RCCResult ad9361_rf_phy_read() {
    RCCResult res = CHECK_IF_POINTER_IS_NULL(ad9361_phy);
    if(res != RCC_OK) return res;
    res           = CHECK_IF_POINTER_IS_NULL(ad9361_phy->pdata);
    if(res != RCC_OK) return res;
    m_properties.ad9361_rf_phy.clk_refin.rate    = ad9361_phy->clk_refin->rate;
    m_properties.ad9361_rf_phy.pdata.rx2tx2      = ad9361_phy->pdata->rx2tx2;
    m_properties.ad9361_rf_phy.pdata.fdd         = ad9361_phy->pdata->fdd;
    m_properties.ad9361_rf_phy.pdata.use_extclk  = ad9361_phy->pdata->use_extclk;
    m_properties.ad9361_rf_phy.pdata.dcxo_coarse = ad9361_phy->pdata->dcxo_coarse;
    m_properties.ad9361_rf_phy.pdata.dcxo_fine   = ad9361_phy->pdata->dcxo_fine;
    return RCC_OK;
  }
  // notification that en_state_machine_mode property has been written
  RCCResult en_state_machine_mode_written() {
    return LIBAD9361_API_1PARAMP(&ad9361_set_en_state_machine_mode,
                                 m_properties.en_state_machine_mode);
  }
  // notification that en_state_machine_mode property will be read
  RCCResult en_state_machine_mode_read() {
    return LIBAD9361_API_1PARAM(&ad9361_get_en_state_machine_mode,
                                &(m_properties.en_state_machine_mode));
  }
  // notification that rx_rf_gain property has been written
  RCCResult rx_rf_gain_written() {
    return LIBAD9361_API_CHAN_SET(&ad9361_set_rx_rf_gain,
                                  &(m_properties.rx_rf_gain[0]));
  }
  // notification that rx_rf_gain property will be read
  RCCResult rx_rf_gain_read() {
    return LIBAD9361_API_CHAN_GETN(&ad9361_get_rx_rf_gain,
                                  &(m_properties.rx_rf_gain[0]));
  }
  // notification that rx_rf_bandwidth property has been written
  RCCResult rx_rf_bandwidth_written() {
    return LIBAD9361_API_1PARAMP(&ad9361_set_rx_rf_bandwidth,
                                 m_properties.rx_rf_bandwidth);
  }
  // notification that rx_rf_bandwidth property will be read
  RCCResult rx_rf_bandwidth_read() {
    return LIBAD9361_API_1PARAM(&ad9361_get_rx_rf_bandwidth,
                                &(m_properties.rx_rf_bandwidth));
  }
  // notification that rx_sampling_freq property has been written
  //RCCResult rx_sampling_freq_written() {
  //  static bool alreadyWritten = false;
  //  return LIBAD9361_API_1PARAMP(&ad9361_set_rx_sampling_freq,
  //                               m_properties.rx_sampling_freq);
  //}
  // notification that rx_sampling_freq property will be read
  RCCResult rx_sampling_freq_read() {
    return LIBAD9361_API_1PARAM(&ad9361_get_rx_sampling_freq,
                                &(m_properties.rx_sampling_freq));
  }
  // notification that rx_lo_freq property has been written
  RCCResult rx_lo_freq_written() {
    return LIBAD9361_API_1PARAMP(&ad9361_set_rx_lo_freq,
                                 m_properties.rx_lo_freq);
  }
  // notification that rx_lo_freq property will be read
  RCCResult rx_lo_freq_read() {
    return LIBAD9361_API_1PARAM(&ad9361_get_rx_lo_freq,
                                &(m_properties.rx_lo_freq));
  }
  // notification that rx_lo_int_ext property has been written
  RCCResult rx_lo_int_ext_written() {
    return LIBAD9361_API_1PARAM(&ad9361_set_rx_lo_int_ext,
                                m_properties.rx_lo_int_ext);
  }
  // notification that rx_rssi property will be read
  RCCResult rx_rssi_read() {
    rf_rssi rssi[AD9361_CONFIG_PROXY_RX_NCHANNELS];
    RCCResult res = LIBAD9361_API_CHAN_GETN(&ad9361_get_rx_rssi, &(rssi[0]));
    if(res != RCC_OK) return res;
    // explicitly copying struct members so as to avoid potential
    // memory corruption due to the fact that OpenCPI sometimes adds
    // additional "padding" members
    for(uint8_t chan=0; chan<AD9361_CONFIG_PROXY_RX_NCHANNELS; chan++) {
      m_properties.rx_rssi[chan].ant        = rssi[chan].ant;
      m_properties.rx_rssi[chan].symbol     = rssi[chan].symbol;
      m_properties.rx_rssi[chan].preamble   = rssi[chan].preamble;
      m_properties.rx_rssi[chan].multiplier = rssi[chan].multiplier;
      m_properties.rx_rssi[chan].duration   = rssi[chan].duration;
    }
    return RCC_OK;
  }
  // notification that rx_gain_control_mode property has been written
  RCCResult rx_gain_control_mode_written() {
    return LIBAD9361_API_CHAN_SET(&ad9361_set_rx_gain_control_mode,
        &(m_properties.rx_gain_control_mode[0]));
  }
  // notification that rx_gain_control_mode property will be read
  RCCResult rx_gain_control_mode_read() {
    return LIBAD9361_API_CHAN_GET(&ad9361_get_rx_gain_control_mode,
        &(m_properties.rx_gain_control_mode[0]));
  }
  // notification that rx_fir_config_write property has been written
  RCCResult rx_fir_config_write_written() {
    // explicitly copying struct members so as to avoid potential
    // memory corruption due to the fact that OpenCPI sometimes adds
    // additional "padding" members
    AD9361_RXFIRConfig config;
    config.rx = m_properties.rx_fir_config_write.rx;
    config.rx_gain = m_properties.rx_fir_config_write.rx_gain;
    config.rx_dec = m_properties.rx_fir_config_write.rx_dec;
    for(uint8_t idx=0; idx<128; idx++) {
      config.rx_coef[idx] = m_properties.rx_fir_config_write.rx_coef[idx];
    }
    config.rx_coef_size = m_properties.rx_fir_config_write.rx_coef_size;
    // rx_path_clocks is a member of the struct but ad9361_set_rx_fir_config
    // ignores this value, commented it out to avoid the scenario where end user
    // thinks they are setting this value when they really aren't
    //for(uint8_t idx=0; idx<6; idx++) {
    //  config.rx_path_clks[idx] =
    //    m_properties.rx_fir_config_write.rx_path_clks[idx];
    //}
    // rx_bandwidth is a member of the struct but ad9361_set_rx_fir_config_
    // ignores this value, commented it out to avoid the scenario where end user
    // thinks they are setting this value when they really aren't
    //config.rx_bandwidth = m_properties.rx_fir_config_write.rx_bandwidth;

    return LIBAD9361_API_1PARAM(&ad9361_set_rx_fir_config, config);
  }
  // notification that rx_fir_config property will be read
  RCCResult rx_fir_config_read_read() {
    AD9361_RXFIRConfig config[AD9361_CONFIG_PROXY_RX_NCHANNELS];
    RCCResult res = LIBAD9361_API_CHAN_GET(&ad9361_get_rx_fir_config, &(config[0]));
    if(res != RCC_OK) return res;
    // explicitly copying struct members so as to avoid potential
    // memory corruption due to the fact that OpenCPI sometimes adds
    // additional "padding" members
    for(uint8_t chan=0; chan<AD9361_CONFIG_PROXY_RX_NCHANNELS; chan++) {
      // rx is a member of the struct but the information conveyed is redundant
      //m_properties.rx_fir_config_read[chan].rx = config[chan].rx;
      
      m_properties.rx_fir_config_read[chan].rx_gain = config[chan].rx_gain;
      m_properties.rx_fir_config_read[chan].rx_dec = config[chan].rx_dec;
      for(uint8_t idx=0; idx<128; idx++) {
        m_properties.rx_fir_config_read[chan].rx_coef[idx] =
            config[chan].rx_coef[idx];
      }
      m_properties.rx_fir_config_read[chan].rx_coef_size =
          config[chan].rx_coef_size;

      // rx_path_clocks is a member of the struct but ad9361_set_rx_fir_config
      // ignores this value, commented it out to avoid the scenario where end
      // user thinks they are setting this value when they really aren't
      //for(uint8_t idx=0; idx<6; idx++) {
      //  m_properties.rx_fir_config_read[chan].rx_path_clks[idx] =
      //      config[chan].rx_path_clks[idx];
      //}

      // rx_bandwidth is a member of the struct but ad9361_set_rx_fir_config
      // ignores this value, commented it out to avoid the scenario where end
      // user thinks they are setting this value when they really aren't
      //m_properties.rx_fir_config_read[chan].rx_bandwidth =
      //    config[chan].rx_bandwidth;
    }
    return RCC_OK;
  }
  // notification that rx_fir_en_dis property has been written
  RCCResult rx_fir_en_dis_written() {
    return LIBAD9361_API_1PARAM(&ad9361_set_rx_fir_en_dis,
                                m_properties.rx_fir_en_dis);
  }
  // notification that rx_fir_en_dis property will be read
  RCCResult rx_fir_en_dis_read() {
    return LIBAD9361_API_1PARAM(&ad9361_get_rx_fir_en_dis,
                                &(m_properties.rx_fir_en_dis));
  }
  // notification that rx_rfdc_track_en_dis property has been written
  RCCResult rx_rfdc_track_en_dis_written() {
    return LIBAD9361_API_1PARAM(&ad9361_set_rx_rfdc_track_en_dis,
                                m_properties.rx_rfdc_track_en_dis);
  }
  // notification that rx_rfdc_track_en_dis property will be read
  RCCResult rx_rfdc_track_en_dis_read() {
    return LIBAD9361_API_1PARAM(&ad9361_get_rx_rfdc_track_en_dis,
                                &(m_properties.rx_rfdc_track_en_dis));
  }
  // notification that rx_bbdc_track_en_dis property has been written
  RCCResult rx_bbdc_track_en_dis_written() {
    return LIBAD9361_API_1PARAM(&ad9361_set_rx_bbdc_track_en_dis,
                                m_properties.rx_bbdc_track_en_dis);
  }
  // notification that rx_bbdc_track_en_dis property will be read
  RCCResult rx_bbdc_track_en_dis_read() {
    return LIBAD9361_API_1PARAM(&ad9361_get_rx_bbdc_track_en_dis,
                                &(m_properties.rx_bbdc_track_en_dis));
  }
  // notification that rx_quad_track_en_dis property has been written
  RCCResult rx_quad_track_en_dis_written() {
    return LIBAD9361_API_1PARAM(&ad9361_set_rx_quad_track_en_dis,
                                m_properties.rx_quad_track_en_dis);
  }
  // notification that rx_quad_track_en_dis property will be read
  RCCResult rx_quad_track_en_dis_read() {
    return LIBAD9361_API_1PARAM(&ad9361_get_rx_quad_track_en_dis,
                                &(m_properties.rx_quad_track_en_dis));
  }
  // notification that rx_rf_port_input property has been written
  RCCResult rx_rf_port_input_written() {
    return LIBAD9361_API_1PARAMP(&ad9361_set_rx_rf_port_input,
                                 m_properties.rx_rf_port_input);
  }
  // notification that rx_rf_port_input property will be read
  RCCResult rx_rf_port_input_read() {
    return LIBAD9361_API_1PARAM(&ad9361_get_rx_rf_port_input,
                                &(m_properties.rx_rf_port_input));
  }
  // notification that rx_fastlock_store property has been written
  RCCResult rx_fastlock_store_written() {
    return LIBAD9361_API_1PARAMP(&ad9361_rx_fastlock_store,
                                 m_properties.rx_fastlock_store);
  }
  // notification that rx_fastlock_recall property has been written
  RCCResult rx_fastlock_recall_written() {
    return LIBAD9361_API_1PARAMP(&ad9361_rx_fastlock_recall,
                                 m_properties.rx_fastlock_recall);
  }
  // notification that rx_fastlock_load_property will be written
  RCCResult rx_fastlock_load_written() {
    fastlock_profile_t profile_to_load;
    std::vector<fastlock_profile_t>::iterator it =
        find_worker_fastlock_profile(
            m_properties.rx_fastlock_save.worker_profile_id,
            m_rx_fastlock_profiles);
    if(it == m_rx_fastlock_profiles.end())
    {
      return setError("worker_profile_id not found");
    }
    return LIBAD9361_API_2PARAM(&ad9361_rx_fastlock_load,
        m_properties.rx_fastlock_load.ad9361_profile_id,
        profile_to_load.values);
  }
  // notification that rx_fastlock_save property will be written
  RCCResult rx_fastlock_save_written() {
    fastlock_profile_t new_rx_fastlock_profile;
    new_rx_fastlock_profile.id = m_properties.rx_fastlock_save.worker_profile_id;
    RCCResult res = LIBAD9361_API_2PARAM(&ad9361_rx_fastlock_save,
        m_properties.rx_fastlock_save.ad9361_profile_id,
        new_rx_fastlock_profile.values);
    if(res != RCC_OK) return res;
    try
    {
      m_rx_fastlock_profiles.push_back(new_rx_fastlock_profile);
    }
    catch(std::bad_alloc&)
    {
      return setError("Memory allocation failure upon saving AD9361 RX fastlock profile in worker memory");
    }
    return RCC_OK;
  }
  // notification that rx_lo_powerdown property has been written
  /*
  RCCResult rx_lo_powerdown_written() {
    return LIBAD9361_API_1PARAM(&ad9361_set_rx_lo_powerdown,
                                m_properties.rx_lo_powerdown);
  }
  */
  // notification that rx_lo_powerdown property will be read
  /*
  RCCResult rx_lo_powerdown_read() {
    return LIBAD9361_API_1PARAM(&ad9361_get_rx_lo_powerdown,
                                &(m_properties.rx_lo_powerdown));
  }
  */
  // notification that tx_attenuation property has been written
  RCCResult tx_attenuation_written() {
    return LIBAD9361_API_CHAN_SET(&ad9361_set_tx_attenuation,
        &(m_properties.tx_attenuation[0]));
  }
  // notification that tx_attenuation property will be read
  RCCResult tx_attenuation_read() {
    return LIBAD9361_API_CHAN_GETN(&ad9361_get_tx_attenuation,
        &(m_properties.tx_attenuation[0]));
  }
  // notification that tx_rf_bandwidth property has been written
  RCCResult tx_rf_bandwidth_written() {
    return LIBAD9361_API_1PARAMP(&ad9361_set_tx_rf_bandwidth,
                                 m_properties.tx_rf_bandwidth);
  }
  // notification that tx_rf_bandwidth property will be read
  RCCResult tx_rf_bandwidth_read() {
    return LIBAD9361_API_1PARAM(&ad9361_get_tx_rf_bandwidth,
                                &(m_properties.tx_rf_bandwidth));
  }
  // notification that tx_sampling_freq property has been written
  RCCResult tx_sampling_freq_written() {
    return LIBAD9361_API_1PARAMP(&ad9361_set_tx_sampling_freq,
                                 m_properties.tx_sampling_freq);
  }
  // notification that tx_sampling_freq property will be read
  RCCResult tx_sampling_freq_read() {
    return LIBAD9361_API_1PARAM(&ad9361_get_tx_sampling_freq,
                                &(m_properties.tx_sampling_freq));
  }
  // notification that tx_lo_freq property has been written
  RCCResult tx_lo_freq_written() {
    return LIBAD9361_API_1PARAMP(&ad9361_set_tx_lo_freq,
                                 m_properties.tx_lo_freq);
  }
  // notification that tx_lo_freq property will be read
  RCCResult tx_lo_freq_read() {
    return LIBAD9361_API_1PARAM(&ad9361_get_tx_lo_freq,
                                &(m_properties.tx_lo_freq));
  }
  // notification that tx_lo_int_ext property has been written
  RCCResult tx_lo_int_ext_written() {
    return LIBAD9361_API_1PARAM(&ad9361_set_tx_lo_int_ext,
                                m_properties.tx_lo_int_ext);
  }
  // notification that tx_fir_config_write property has been written
  RCCResult tx_fir_config_write_written() {
    // explicitly copying struct members so as to avoid potential
    // memory corruption due to the fact that OpenCPI sometimes adds
    // additional "padding" members
    AD9361_TXFIRConfig config;
    config.tx = m_properties.tx_fir_config_write.tx;
    config.tx_gain = m_properties.tx_fir_config_write.tx_gain;
    config.tx_int = m_properties.tx_fir_config_write.tx_int;
    for(uint8_t idx=0; idx<128; idx++) {
      config.tx_coef[idx] = m_properties.tx_fir_config_write.tx_coef[idx];
    }
    config.tx_coef_size = m_properties.tx_fir_config_write.tx_coef_size;
    // tx_path_clocks is a member of the struct but ad9361_set_tx_fir_config
    // ignores this value, commented it out to avoid the scenario where end user
    // thinks they are setting this value when they really aren't
    //for(uint8_t idx=0; idx<6; idx++) {
    //  config.tx_path_clks[idx] =
    //      m_properties.tx_fir_config_write.tx_path_clks[idx];
    //}
    // tx_bandwidth is a member of the struct but ad9361_set_tx_fir_config_
    // ignores this value, commented it out to avoid the scenario where end user
    // thinks they are setting this value when they really aren't
    //config.tx_bandwidth = m_properties.tx_fir_config_write.tx_bandwidth;

    return LIBAD9361_API_1PARAM(&ad9361_set_tx_fir_config, config);
  }
  // notification that tx_fir_config_read property will be read
  RCCResult tx_fir_config_read_read() {
    AD9361_TXFIRConfig config[AD9361_CONFIG_PROXY_RX_NCHANNELS];
    RCCResult res = LIBAD9361_API_CHAN_GET(&ad9361_get_tx_fir_config, &(config[0]));
    if(res != RCC_OK) return res;
    // explicitly copying struct members so as to avoid potential
    // memory corruption due to the fact that OpenCPI sometimes adds
    // additional "padding" members
    for(uint8_t chan=0; chan<AD9361_CONFIG_PROXY_TX_NCHANNELS; chan++) {
      // tx is a member of the struct but the information conveyed is redundant
      //m_properties.tx_fir_config_read[chan].tx = config.tx;
      //
      m_properties.tx_fir_config_read[chan].tx_gain = config[chan].tx_gain;
      m_properties.tx_fir_config_read[chan].tx_int = config[chan].tx_int;
      for(uint8_t idx=0; idx<128; idx++) {
        m_properties.tx_fir_config_read[chan].tx_coef[idx] =
            config[chan].tx_coef[idx];
      }
      m_properties.tx_fir_config_read[chan].tx_coef_size =
          config[chan].tx_coef_size;

      // tx_path_clocks is a member of the struct but ad9361_set_tx_fir_config
      // ignores this value, commented it out to avoid the scenario where end
      // user thinks they are setting this value when they really aren't
      //for(uint8_t idx=0; idx<6; idx++) {
      //  m_properties.tx_fir_config_read[chan].tx_path_clks[idx] =
      //      config[chan].tx_path_clks[idx];
      //}

      // tx_bandwidth is a member of the struct but ad9361_set_tx_fir_config
      // ignores this value, commented it out to avoid the scenario where end
      // user thinks they are setting this value when they really aren't
      //m_properties.tx_fir_config_read[chan].tx_bandwidth =
      //    config[chan].tx_bandwidth;
    }
    return RCC_OK;
  }
  // notification that tx_fir_en_dis property has been written
  RCCResult tx_fir_en_dis_written() {
    return LIBAD9361_API_1PARAM(&ad9361_set_tx_fir_en_dis,
                                m_properties.tx_fir_en_dis);
  }
  // notification that tx_fir_en_dis property will be read
  RCCResult tx_fir_en_dis_read() {
    return LIBAD9361_API_1PARAM(&ad9361_get_tx_fir_en_dis,
                                &(m_properties.tx_fir_en_dis));
  }
  // notification that tx_rssi property will be read
  RCCResult tx_rssi_read() {
    return LIBAD9361_API_CHAN_GET(&ad9361_get_tx_rssi,
                                  &(m_properties.tx_rssi[0]));
  }
  // notification that tx_rf_port_output property has been written
  RCCResult tx_rf_port_output_written() {
    return LIBAD9361_API_1PARAMP(&ad9361_set_tx_rf_port_output,
                                 m_properties.tx_rf_port_output);
  }
  // notification that tx_rf_port_output property will be read
  RCCResult tx_rf_port_output_read() {
    return LIBAD9361_API_1PARAM(&ad9361_get_tx_rf_port_output,
                                &(m_properties.tx_rf_port_output));
  }
  // notification that tx_auto_cal_en_dis property has been written
  RCCResult tx_auto_cal_en_dis_written() {
    return LIBAD9361_API_1PARAM(&ad9361_set_tx_auto_cal_en_dis,
                                m_properties.tx_auto_cal_en_dis);
  }
  // notification that tx_auto_cal_en_dis property will be read
  RCCResult tx_auto_cal_en_dis_read() {
    return LIBAD9361_API_1PARAM(&ad9361_get_tx_auto_cal_en_dis,
                                &(m_properties.tx_auto_cal_en_dis));
  }
  // notification that tx_fastlock_store property has been written
  RCCResult tx_fastlock_store_written() {
    return LIBAD9361_API_1PARAMP(&ad9361_tx_fastlock_store,
                                 m_properties.tx_fastlock_store);
  }
  // notification that tx_fastlock_recall property has been written
  RCCResult tx_fastlock_recall_written() {
    return LIBAD9361_API_1PARAMP(&ad9361_tx_fastlock_recall,
                                 m_properties.tx_fastlock_recall);
  }
  // notification that tx_fastlock_load_property will be written
  RCCResult tx_fastlock_load_written() {
    fastlock_profile_t profile_to_load;
    std::vector<fastlock_profile_t>::iterator it =
        find_worker_fastlock_profile(
            m_properties.tx_fastlock_save.worker_profile_id,
            m_rx_fastlock_profiles);
    if(it == m_tx_fastlock_profiles.end())
    {
      return setError("worker_profile_id not found");
    }
    return LIBAD9361_API_2PARAM(&ad9361_tx_fastlock_load,
        m_properties.tx_fastlock_load.ad9361_profile_id,
        profile_to_load.values);
  }
  // notification that tx_fastlock_save property will be written
  RCCResult tx_fastlock_save_written() {
    fastlock_profile_t new_tx_fastlock_profile;
    new_tx_fastlock_profile.id = m_properties.tx_fastlock_save.worker_profile_id;
    RCCResult res = LIBAD9361_API_2PARAM(&ad9361_tx_fastlock_save,
        m_properties.tx_fastlock_save.ad9361_profile_id,
        new_tx_fastlock_profile.values);
    if(res != RCC_OK) return res;
    try
    {
      m_tx_fastlock_profiles.push_back(new_tx_fastlock_profile);
    }
    catch(std::bad_alloc&)
    {
      return setError("Memory allocation failure upon saving AD9361 TX fastlock profile in worker memory");
    }
    return RCC_OK;
  }
  // notification that tx_lo_powerdown property has been written
  /*
  RCCResult tx_lo_powerdown_written() {
    return LIBAD9361_API_1PARAM(&ad9361_set_tx_lo_powerdown,
                                m_properties.tx_lo_powerdown);
  }
  */
  // notification that tx_lo_powerdown property will be read
  /*
  RCCResult tx_lo_powerdown_read() {
    return LIBAD9361_API_1PARAM(&ad9361_get_tx_lo_powerdown,
                                &(m_properties.tx_lo_powerdown));
  }
  */
  // notification that trx_path_clks property has been written
  RCCResult trx_path_clks_written() {
    return LIBAD9361_API_2PARAM(&ad9361_set_trx_path_clks,
                                m_properties.trx_path_clks.rx_path_clks,
                                m_properties.trx_path_clks.tx_path_clks);
  }
  // notification that trx_path_clks property will be read
  RCCResult trx_path_clks_read() {
    return LIBAD9361_API_2PARAM(&ad9361_get_trx_path_clks,
                                &(m_properties.trx_path_clks.rx_path_clks[0]),
                                &(m_properties.trx_path_clks.tx_path_clks[0]));
  }
  // notification that trx_lo_powerdown property has been written
  /*
  RCCResult trx_lo_powerdown_written() {
    return LIBAD9361_API_2PARAM(&ad9361_set_trx_lo_powerdown,
                                m_properties.trx_lo_powerdown.pd_rx,
                                m_properties.trx_lo_powerdown.pd_tx);
  }
  */
  // notification that no_ch_mode property has been written
  RCCResult no_ch_mode_written() {
    RCCResult res;

    // if prop value is MODE_2x2, fail if there aren't 2 qadc and 2 qdac workers
    // in the FPGA bitstream (ad9361_config.hdl has this info in its properties)
    const bool qadc1_is_present = slave.get_qadc1_is_present();
    const bool qdac1_is_present = slave.get_qdac1_is_present();
    if((!qadc1_is_present) && (!qdac1_is_present) &
       (m_properties.no_ch_mode == MODE_2x2))
    {
      const bool qadc0_is_present = slave.get_qadc0_is_present();
      int num_rx = qadc0_is_present ? 1 : 0;
      num_rx += qadc1_is_present ? 1 : 0;
      const bool qdac0_is_present = slave.get_qdac0_is_present();
      int num_tx = qdac0_is_present ? 1 : 0;
      num_tx += qdac1_is_present ? 1 : 0;
      std::ostringstream ostr;
      ostr << "no_ch_mode attempted to set to MODE_2x2 but loaded FPGA " <<
              "bitstream only contains device worker support for " <<
              num_rx << " RX and " <<
              num_tx << " TX channels";
      return setError(ostr.str().c_str());
    }
    
    // libad9361's set_no_ch_mode() is known to change TX/RX sampling freqs
    uint32_t tx_sampling_freq;
    res = LIBAD9361_API_1PARAM(&ad9361_get_tx_sampling_freq,
                               &tx_sampling_freq);
    if(res != RCC_OK) return res;

    res = LIBAD9361_API_1PARAMP(&ad9361_set_no_ch_mode,
                                m_properties.no_ch_mode);
    if(res != RCC_OK) return res;

    set_FPGA_channel_config(); // because channel config potentially changed

    // libad9361's set_no_ch_mode() is known to change TX/RX sampling freqs
    return LIBAD9361_API_1PARAMP(&ad9361_set_tx_sampling_freq,
                                 tx_sampling_freq);
  }
  // notification that trx_fir_en_dis property has been written
  RCCResult trx_fir_en_dis_written() {
    return LIBAD9361_API_1PARAM(&ad9361_set_trx_fir_en_dis,
                                m_properties.trx_fir_en_dis);
  }
  // notification that trx_rate_gov property has been written
  RCCResult trx_rate_gov_written() {
    return LIBAD9361_API_1PARAMP(&ad9361_set_trx_rate_gov,
                                 m_properties.trx_rate_gov);
  }
  // notification that trx_rate_gov property will be read
  RCCResult trx_rate_gov_read() {
    return LIBAD9361_API_1PARAM(&ad9361_get_trx_rate_gov,
                                &(m_properties.trx_rate_gov));
  }
  // notification that do_calib property has been written
  RCCResult do_calib_written() {
    return LIBAD9361_API_2PARAM(&ad9361_do_calib,
                                m_properties.do_calib.cal,
                                m_properties.do_calib.arg);
  }
  // notification that trx_load_enable_fir property has been written
  RCCResult trx_load_enable_fir_written() {
    // explicitly copying struct members so as to avoid potential
    // memory corruption due to the fact that OpenCPI sometimes adds
    // additional "padding" members
    AD9361_RXFIRConfig rx_config;
    rx_config.rx = m_properties.trx_load_enable_fir.rx;
    rx_config.rx_gain = m_properties.trx_load_enable_fir.rx_gain;
    rx_config.rx_dec = m_properties.trx_load_enable_fir.rx_dec;
    for(uint8_t idx=0; idx<128; idx++) {
      rx_config.rx_coef[idx] = m_properties.trx_load_enable_fir.rx_coef[idx];
    }
    rx_config.rx_coef_size = m_properties.trx_load_enable_fir.rx_coef_size;
    for(uint8_t idx=0; idx<6; idx++) {
      rx_config.rx_path_clks[idx] =
        m_properties.trx_load_enable_fir.rx_path_clks[idx];
    }
    rx_config.rx_bandwidth = m_properties.trx_load_enable_fir.rx_bandwidth;

    // explicitly copying struct members so as to avoid potential
    // memory corruption due to the fact that OpenCPI sometimes adds
    // additional "padding" members
    AD9361_TXFIRConfig tx_config;
    tx_config.tx = m_properties.trx_load_enable_fir.tx;
    tx_config.tx_gain = m_properties.trx_load_enable_fir.tx_gain;
    tx_config.tx_int = m_properties.trx_load_enable_fir.tx_int;
    for(uint8_t idx=0; idx<128; idx++) {
      tx_config.tx_coef[idx] = m_properties.trx_load_enable_fir.tx_coef[idx];
    }
    tx_config.tx_coef_size = m_properties.trx_load_enable_fir.tx_coef_size;
    for(uint8_t idx=0; idx<6; idx++) {
      tx_config.tx_path_clks[idx] =
          m_properties.trx_load_enable_fir.tx_path_clks[idx];
    }
    tx_config.tx_bandwidth = m_properties.trx_load_enable_fir.tx_bandwidth;

    return LIBAD9361_API_2PARAM(&ad9361_trx_load_enable_fir,
                                rx_config, tx_config);
  }
  // notification that do_dcxo_tune_coarse property has been written
  RCCResult do_dcxo_tune_coarse_written() {
    return LIBAD9361_API_1PARAMP(&ad9361_do_dcxo_tune_coarse,
                                 m_properties.do_dcxo_tune_coarse);
  }
  // notification that do_dcxo_tune_fine property has been written
  RCCResult do_dcxo_tune_fine_written() {
    return LIBAD9361_API_1PARAMP(&ad9361_do_dcxo_tune_fine,
                                 m_properties.do_dcxo_tune_fine);
  }
  // notification that bist_loopback property has been written
  RCCResult bist_loopback_written() {
    return LIBAD9361_API_1PARAMP(&ad9361_bist_loopback,
                                 m_properties.bist_loopback);
  }
  // notification that bist_loopback property will be read
  RCCResult bist_loopback_read() {
    return LIBAD9361_API_1PARAMV(&ad9361_get_bist_loopback,
                                 &(m_properties.bist_loopback));
  }
  // notification that bist_prbs property has been written
  RCCResult bist_prbs_written() {
    return LIBAD9361_API_1PARAMP(&ad9361_bist_prbs,
                                 (ad9361_bist_mode)m_properties.bist_prbs);
  }
  // notification that bist_prbs property will be read
  RCCResult bist_prbs_read() {
    ad9361_bist_mode bist_prbs;

    RCCResult res = LIBAD9361_API_1PARAMV(&ad9361_get_bist_prbs, &bist_prbs);
    if(res != RCC_OK) return res;
    if(bist_prbs > (int)(255))
    {
      std::ostringstream ostr;
      ostr << "ad9361_get_bist_prbs set at unexpected large value which cannot"
           << "be accounted for";
      return setError(ostr.str().c_str());
    }
    m_properties.bist_prbs = (char)(bist_prbs & 0xff);
    return RCC_OK;
  }
  // notification that bist_tone property has been written
  RCCResult bist_tone_written() {
    RCCResult ret = CHECK_IF_POINTER_IS_NULL(ad9361_phy);
    if(ret != RCC_OK) return ret;
    ret           = CHECK_IF_POINTER_IS_NULL(ad9361_phy->pdata);
    if(ret != RCC_OK) return ret;
    if(AD9361_CONFIG_PROXY_OCPI_DEBUG)
    {
      printf("DEBUG: ad9361_config_proxy: libad9361 API call: ");
      // we don't know how to parameters for this function, so just printing ...
      printf("ad9361_bist_tone(...)\n");
    }
    const int32_t res = ad9361_bist_tone(ad9361_phy,
                                  (ad9361_bist_mode)m_properties.bist_tone.mode,
                                  m_properties.bist_tone.freq_Hz,
                                  m_properties.bist_tone.level_dB,
                                  m_properties.bist_tone.mask);
    if(res != 0)
    {
      std::ostringstream err;
      err << "ad9361_bist_tone() returned: " << res;
      return setError(err.str().c_str());
    }
    return RCC_OK;
  }
  // notification that bist_tone property will be read
  RCCResult bist_tone_read() {
    RCCResult res = CHECK_IF_POINTER_IS_NULL(ad9361_phy);
    if(res != RCC_OK) return res;
    res           = CHECK_IF_POINTER_IS_NULL(ad9361_phy->pdata);
    if(res != RCC_OK) return res;
    ad9361_bist_mode mode;
    // ad9361_get_bist_tone() returns void
    ad9361_get_bist_tone(ad9361_phy,
                         &mode,
                         &m_properties.bist_tone.freq_Hz,
                         &m_properties.bist_tone.level_dB,
                         &m_properties.bist_tone.mask);
    if(mode > (int)(255))
    {
      std::ostringstream ostr;
      ostr << "ad9361_get_bist_tone set at unexpected large value which cannot"
           << "be accounted for";
      return setError(ostr.str().c_str());
    }
    m_properties.bist_tone.mode = (char)(mode & 0xff);
    return RCC_OK;
  }
  // notification that rx_sampling_freq_multiplier property has been written
  RCCResult rx_sampling_freq_multiplier_written() {
    if(m_properties.rx_sampling_freq_multiplier != 1)
    {
      return setError("Only supported rx_sampling_freq_multiplier value is 1 for now");
    }
    return RCC_OK;
  }
  // notification rx_sampling_freq_multiplier property will be read
  RCCResult rx_sampling_freq_multiplier_read() {
    m_properties.rx_sampling_freq_multiplier = 1;
    return RCC_OK;
  }
  // notification that bb_pll_is_locked property will be read
  RCCResult bb_pll_is_locked_read() {
    m_properties.bb_pll_is_locked = (slave.get_overflow_ch_1() & BBPLL_LOCK) == BBPLL_LOCK;
    return RCC_OK;
  }
  // notification that rx_pll_is_locked property will be read
  RCCResult rx_pll_is_locked_read() {
    const uint8_t rx_synth_lock_detect_config = LOCK_DETECT_MODE(slave.get_rx_synth_lock_detect_config());
    bool RX_PLL_lock_detect_enabled = false;
    if(rx_synth_lock_detect_config == 0x01) RX_PLL_lock_detect_enabled = true;
    if(rx_synth_lock_detect_config == 0x02) RX_PLL_lock_detect_enabled = true;
    if(!RX_PLL_lock_detect_enabled)
    {
      m_properties.rx_pll_is_locked = RX_PLL_IS_LOCKED_UNKNOWN;
      return RCC_OK;
    }
    const uint8_t rx_synth_cp_ovrg_vco_lock = slave.get_rx_synth_cp_ovrg_vco_lock();
    if((rx_synth_cp_ovrg_vco_lock & VCO_LOCK) == VCO_LOCK)
    {
      m_properties.rx_pll_is_locked = RX_PLL_IS_LOCKED_TRUE;
    }
    else
    {
      m_properties.rx_pll_is_locked = RX_PLL_IS_LOCKED_FALSE;
    }
    return RCC_OK;
  }
  // notification that rx_fastlock_delete property has been written
  RCCResult rx_fastlock_delete_written() {
    std::vector<fastlock_profile_t>::iterator it =
        find_worker_fastlock_profile(m_properties.rx_fastlock_delete,
                                     m_tx_fastlock_profiles);
    if(it == m_rx_fastlock_profiles.end())
    {
      return setError("worker_profile_id not found");
    }
    m_rx_fastlock_profiles.erase(it);
    return RCC_OK;
  }
  // notification that tx_pll_is_locked property will be read
  RCCResult tx_pll_is_locked_read() {
    const uint8_t tx_synth_lock_detect_config = LOCK_DETECT_MODE(slave.get_tx_synth_lock_detect_config());
    bool TX_PLL_lock_detect_enabled = false;
    if(tx_synth_lock_detect_config == 0x01) TX_PLL_lock_detect_enabled = true;
    if(tx_synth_lock_detect_config == 0x02) TX_PLL_lock_detect_enabled = true;
    if(!TX_PLL_lock_detect_enabled)
    {
      m_properties.tx_pll_is_locked = TX_PLL_IS_LOCKED_UNKNOWN;
      return RCC_OK;
    }
    const uint8_t tx_synth_cp_overrange_vco_lock = slave.get_tx_synth_cp_overrange_vco_lock();
    if((tx_synth_cp_overrange_vco_lock & VCO_LOCK) == VCO_LOCK)
    {
      m_properties.tx_pll_is_locked = TX_PLL_IS_LOCKED_TRUE;
    }
    else
    {
      m_properties.tx_pll_is_locked = TX_PLL_IS_LOCKED_FALSE;
    }
    return RCC_OK;
  }
  // notification that tx_fastlock_delete property has been written
  RCCResult tx_fastlock_delete_written() {
    std::vector<fastlock_profile_t>::iterator it =
        find_worker_fastlock_profile(m_properties.tx_fastlock_delete,
                                     m_tx_fastlock_profiles);
    if(it == m_tx_fastlock_profiles.end())
    {
      return setError("worker_profile_id not found");
    }
    m_tx_fastlock_profiles.erase(it);
    return RCC_OK;
  }
  // notification that rx_vco_divider property will be read
  RCCResult rx_vco_divider_read() {
    uint8_t divider = (slave.get_general_rfpll_dividers() & 0x0f);
    switch(divider)
    {
      case 0: m_properties.rx_vco_divider = RX_VCO_DIVIDER_2; break;
      case 1: m_properties.rx_vco_divider = RX_VCO_DIVIDER_4; break;
      case 2: m_properties.rx_vco_divider = RX_VCO_DIVIDER_8; break;
      case 3: m_properties.rx_vco_divider = RX_VCO_DIVIDER_16; break;
      case 4: m_properties.rx_vco_divider = RX_VCO_DIVIDER_32; break;
      case 5: m_properties.rx_vco_divider = RX_VCO_DIVIDER_64; break;
      case 6: m_properties.rx_vco_divider = RX_VCO_DIVIDER_128; break;
      case 7: m_properties.rx_vco_divider = RX_VCO_DIVIDER_EXTERNAL_2; break;
      default: m_properties.rx_vco_divider = RX_VCO_DIVIDER_INVALID; break;
    }
    return RCC_OK;
  }
  // notification that rx_vco_n_integer property will be read
  RCCResult rx_vco_n_integer_read() {
    uint16_t integer = (uint16_t)slave.get_rx_synth_integer_byte_0();
    integer |= ((uint16_t)((slave.get_rx_synth_integer_byte_1() & 0x07)) << 8);
    m_properties.rx_vco_n_integer = integer;
    return RCC_OK;
  }
  // notification that rx_vco_n_fractional property will be read
  RCCResult rx_vco_n_fractional_read() {
    uint32_t frac = (uint32_t)slave.get_rx_synth_fract_byte_0();
    frac |= ((uint32_t)(slave.get_rx_synth_fract_byte_1()) << 8);
    frac |= ((uint32_t)((slave.get_rx_synth_fract_byte_2()) & 0x7f) << 16);
    m_properties.rx_vco_n_fractional = frac;
    return RCC_OK;
  }
  // notification that rx_ref_divider property will be read
  RCCResult rx_ref_divider_read() {
    uint8_t div = (uint8_t)((slave.get_ref_divide_config_1() & 0x01) << 1);
    div |= (uint8_t)((slave.get_ref_divide_config_2() & 0x80) >> 7);
    switch(div)
    {
      case 0: m_properties.rx_ref_divider = RX_REF_DIVIDER_1; break;
      case 1: m_properties.rx_ref_divider = RX_REF_DIVIDER_HALF; break;
      case 2: m_properties.rx_ref_divider = RX_REF_DIVIDER_FOURTH; break;
      default: m_properties.rx_ref_divider = RX_REF_DIVIDER_2; break;
    }
    return RCC_OK;
  }
  // notification that tx_vco_divider property will be read
  RCCResult tx_vco_divider_read() {
    uint8_t divider = (slave.get_general_rfpll_dividers() & 0xf0) >> 4;
    switch(divider)
    {
      case 0: m_properties.tx_vco_divider = TX_VCO_DIVIDER_2; break;
      case 1: m_properties.tx_vco_divider = TX_VCO_DIVIDER_4; break;
      case 2: m_properties.tx_vco_divider = TX_VCO_DIVIDER_8; break;
      case 3: m_properties.tx_vco_divider = TX_VCO_DIVIDER_16; break;
      case 4: m_properties.tx_vco_divider = TX_VCO_DIVIDER_32; break;
      case 5: m_properties.tx_vco_divider = TX_VCO_DIVIDER_64; break;
      case 6: m_properties.tx_vco_divider = TX_VCO_DIVIDER_128; break;
      case 7: m_properties.tx_vco_divider = TX_VCO_DIVIDER_EXTERNAL_2; break;
      default: m_properties.tx_vco_divider = TX_VCO_DIVIDER_INVALID; break;
    }
    return RCC_OK;
  }
  // notification that tx_vco_n_integer property will be read
  RCCResult tx_vco_n_integer_read() {
    uint16_t integer = (uint16_t)slave.get_tx_synth_integer_byte_0();
    integer |= ((uint16_t)((slave.get_tx_synth_integer_byte_1() & 0x07)) << 8);
    m_properties.tx_vco_n_integer = integer;
    return RCC_OK;
  }
  // notification that tx_vco_n_fractional property will be read
  RCCResult tx_vco_n_fractional_read() {
    uint32_t frac = (uint32_t)slave.get_tx_synth_fract_byte_0();
    frac |= ((uint32_t)(slave.get_tx_synth_fract_byte_1()) << 8);
    frac |= ((uint32_t)((slave.get_tx_synth_fract_byte_2()) & 0x7f) << 16);
    m_properties.tx_vco_n_fractional = frac;
    return RCC_OK;
  }
  // notification that tx_ref_divider property will be read
  RCCResult tx_ref_divider_read() {
    uint8_t div = (uint8_t)(slave.get_ref_divide_config_2() & 0x0c) >> 2;
    switch(div)
    {
      case 0: m_properties.tx_ref_divider = TX_REF_DIVIDER_1; break;
      case 1: m_properties.tx_ref_divider = TX_REF_DIVIDER_HALF; break;
      case 2: m_properties.tx_ref_divider = TX_REF_DIVIDER_FOURTH; break;
      default: m_properties.tx_ref_divider = TX_REF_DIVIDER_2; break;
    }
    return RCC_OK;
  }
  // notification that fpga_data_mode_property will be read
  RCCResult fpga_data_mode_read() {
    const uint8_t iostandard = (uint8_t)slave.get_iostandard();
    const uint8_t port_config = (uint8_t)slave.get_port_config();
    const uint8_t duplex_config = (uint8_t)slave.get_duplex_config();
    //uint8_t data_rate_config = (uint8_t)slave.get_data_rate_config();
    //std::cout << "adc_mode=" << (int)adc_mode << std::endl;
    if(iostandard == 1)
    {
      // is LVDS
      m_properties.fpga_data_mode = FPGA_DATA_MODE_LVDS_DUALPORT_FULLDUPLEX;
    }
    else
    {
      // is CMOS
      if(port_config == 0)
      {
        // is single port
        if(duplex_config == 0)
        {
          // is half duplex
          m_properties.fpga_data_mode = FPGA_DATA_MODE_CMOS_SINGLEPORT_HALFDUPLEX;
        }
        else
        {
          // is full duplex
          m_properties.fpga_data_mode = FPGA_DATA_MODE_CMOS_SINGLEPORT_FULLDUPLEX;
        }
      }
      else
      {
        // is dual port
        if(duplex_config == 0)
        {
          // is half duplex
          m_properties.fpga_data_mode = FPGA_DATA_MODE_CMOS_DUALPORT_HALFDUPLEX;
        }
        else
        {
          // is full duplex
          m_properties.fpga_data_mode = FPGA_DATA_MODE_CMOS_DUALPORT_FULLDUPLEX;
        }
      }
    }
    return RCC_OK;
  }
  // notification that DATA_CLK_P_rate_Hz property will be read
  RCCResult DATA_CLK_P_rate_Hz_read() {
    uint32_t rx_sampling_freq;
    RCCResult res = LIBAD9361_API_1PARAM(&ad9361_get_rx_sampling_freq,
                                         &rx_sampling_freq);
    if(res != RCC_OK) { return res; }
    res = CHECK_IF_POINTER_IS_NULL(ad9361_phy);
    if(res != RCC_OK) { return res; }
    res = CHECK_IF_POINTER_IS_NULL(ad9361_phy->pdata);
    if(res != RCC_OK) { return res; }
    const uint8_t parallel_port_conf_1 = (uint8_t)slave.get_parallel_port_conf_1();
    const uint8_t two_t_two_r_timing_enable = ((parallel_port_conf_1 & 0x04) >> 2);
    double DATA_CLK_P_rate_Hz = (two_t_two_r_timing_enable == 1) ?
       2.*rx_sampling_freq : rx_sampling_freq;
    const uint8_t iostandard    = (uint8_t)slave.get_iostandard();
    const bool modeIsCMOS       = (iostandard    == 0);
    DATA_CLK_P_rate_Hz *= modeIsCMOS ? 1. : 2.;
    m_properties.DATA_CLK_P_rate_Hz = DATA_CLK_P_rate_Hz;
    return RCC_OK;
  }
  RCCResult run(bool /*timedout*/) {
    return RCC_ADVANCE;
  }
};

AD9361_CONFIG_PROXY_START_INFO
// Insert any static info assignments here (memSize, memSizes, portInfo)
// e.g.: info.memSize = sizeof(MyMemoryStruct);
AD9361_CONFIG_PROXY_END_INFO
