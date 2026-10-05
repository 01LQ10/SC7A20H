/**
 * @file      driver_sc7a20h.h
 * @brief     driver sc7a20h header file
 * @version   1.0.0
 * @author    LQ
 * @date      2026-10-05
 * @license   MIT
 *
 * <h3>history</h3>
 * <table>
 * <tr><th>Date        <th>Version  <th>Author  <th>Description
 * <tr><td>2026/10/05  <td>1.0.0    <td>LQ      <td>first upload
 * </table>
 */

#ifndef DRIVER_SC7A20H_H
#define DRIVER_SC7A20H_H

#include <stdint.h>
#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup sc7a20h_driver sc7a20h driver function
 * @brief    sc7a20h driver modules
 * @{
 */

/**
 * @brief sc7a20h interface enumeration definition
 */
typedef enum
{
    SC7A20H_INTERFACE_IIC = 0x00, /**< IIC interface */
    SC7A20H_INTERFACE_SPI = 0x01  /**< SPI interface */
} sc7a20h_interface_t;

/**
 * @brief sc7a20h IIC address enumeration definition
 */
typedef enum
{
    SC7A20H_ADDRESS_LOW  = 0x18, /**< SDO pin connected low */
    SC7A20H_ADDRESS_HIGH = 0x19  /**< SDO pin floating or connected high */
} sc7a20h_address_t;

/**
 * @brief sc7a20h power mode enumeration definition
 */
typedef enum
{
    SC7A20H_MODE_NORMAL       = 0x00, /**< normal mode */
    SC7A20H_MODE_LOW_POWER    = 0x01, /**< low power mode */
    SC7A20H_MODE_HIGH_PERFORMANCE = 0x02, /**< high performance mode */
    SC7A20H_MODE_ENHANCED     = 0x03  /**< enhanced mode */
} sc7a20h_mode_t;

/**
 * @brief sc7a20h output data rate enumeration definition
 */
typedef enum
{
    SC7A20H_ODR_POWER_DOWN = 0x00, /**< power down */
    SC7A20H_ODR_1P56_HZ    = 0x01, /**< 1.56 Hz */
    SC7A20H_ODR_12P5_HZ    = 0x02, /**< 12.5 Hz */
    SC7A20H_ODR_25_HZ      = 0x03, /**< 25 Hz */
    SC7A20H_ODR_50_HZ      = 0x04, /**< 50 Hz */
    SC7A20H_ODR_100_HZ     = 0x05, /**< 100 Hz */
    SC7A20H_ODR_200_HZ     = 0x06, /**< 200 Hz */
    SC7A20H_ODR_400_HZ     = 0x07, /**< 400 Hz */
    SC7A20H_ODR_800_HZ     = 0x08, /**< 800 Hz */
    SC7A20H_ODR_1P48_KHZ   = 0x09, /**< 1.48 kHz */
    SC7A20H_ODR_2P66_KHZ   = 0x0A, /**< 2.66 kHz */
    SC7A20H_ODR_4P434_KHZ  = 0x0B  /**< 4.434 kHz */
} sc7a20h_odr_t;

/**
 * @brief sc7a20h full scale enumeration definition
 */
typedef enum
{
    SC7A20H_SCALE_2G  = 0x00, /**< plus or minus 2 g */
    SC7A20H_SCALE_4G  = 0x01, /**< plus or minus 4 g */
    SC7A20H_SCALE_8G  = 0x02, /**< plus or minus 8 g */
    SC7A20H_SCALE_16G = 0x03  /**< plus or minus 16 g */
} sc7a20h_scale_t;

/**
 * @brief sc7a20h boolean enumeration definition
 */
typedef enum
{
    SC7A20H_BOOL_FALSE = 0x00, /**< disable function */
    SC7A20H_BOOL_TRUE  = 0x01  /**< enable function */
} sc7a20h_bool_t;

/**
 * @brief sc7a20h documented register addresses
 */
#define SC7A20H_REG_SPI_CTRL               0x0EU /**< SPI address bank control, read only */
#define SC7A20H_REG_WHO_AM_I               0x0FU /**< device identity register */
#define SC7A20H_REG_CTRL_REG0              0x1FU /**< mode, oversampling and filter control */
#define SC7A20H_REG_CTRL_REG1              0x20U /**< output rate and axis control */
#define SC7A20H_REG_CTRL_REG2              0x21U /**< high-pass filter control */
#define SC7A20H_REG_CTRL_REG3              0x22U /**< INT1 routing and FIFO format */
#define SC7A20H_REG_CTRL_REG4              0x23U /**< scale and output format control */
#define SC7A20H_REG_CTRL_REG5              0x24U /**< FIFO and INT1/INT2 control */
#define SC7A20H_REG_CTRL_REG6              0x25U /**< INT2 routing and electrical control */
#define SC7A20H_REG_STATUS                 0x27U /**< output data status */
#define SC7A20H_REG_OUT_X_L                0x28U /**< X axis low byte */
#define SC7A20H_REG_OUT_X_H                0x29U /**< X axis high byte */
#define SC7A20H_REG_OUT_Y_L                0x2AU /**< Y axis low byte */
#define SC7A20H_REG_OUT_Y_H                0x2BU /**< Y axis high byte */
#define SC7A20H_REG_OUT_Z_L                0x2CU /**< Z axis low byte */
#define SC7A20H_REG_OUT_Z_H                0x2DU /**< Z axis high byte */
#define SC7A20H_REG_FIFO_CTRL              0x2EU /**< FIFO mode and watermark */
#define SC7A20H_REG_FIFO_SRC               0x2FU /**< FIFO state */
#define SC7A20H_REG_AOI1_CFG               0x30U /**< AOI1 configuration */
#define SC7A20H_REG_AOI1_SRC               0x31U /**< AOI1 source status */
#define SC7A20H_REG_AOI1_THS               0x32U /**< AOI1 threshold */
#define SC7A20H_REG_AOI1_DURATION          0x33U /**< AOI1 duration */
#define SC7A20H_REG_AOI2_CFG               0x34U /**< AOI2 configuration */
#define SC7A20H_REG_AOI2_SRC               0x35U /**< AOI2 source status */
#define SC7A20H_REG_AOI2_THS               0x36U /**< AOI2 threshold */
#define SC7A20H_REG_AOI2_DURATION          0x37U /**< AOI2 duration */
#define SC7A20H_REG_CLICK_CTRL             0x38U /**< click control */
#define SC7A20H_REG_CLICK_SRC              0x39U /**< click source status */
#define SC7A20H_REG_CLICK_COEFF1           0x3AU /**< click coefficient 1 */
#define SC7A20H_REG_CLICK_COEFF2           0x3BU /**< click coefficient 2 */
#define SC7A20H_REG_CLICK_COEFF3           0x3CU /**< click coefficient 3 */
#define SC7A20H_REG_CLICK_COEFF4           0x3DU /**< click coefficient 4 */
#define SC7A20H_REG_DIG_CTRL               0x57U /**< digital pull-up control */
#define SC7A20H_REG_OUT_X_NEW_H            0x61U /**< live X axis high byte */
#define SC7A20H_REG_OUT_X_NEW_L            0x62U /**< live X axis low byte */
#define SC7A20H_REG_OUT_Y_NEW_H            0x63U /**< live Y axis high byte */
#define SC7A20H_REG_OUT_Y_NEW_L            0x64U /**< live Y axis low byte */
#define SC7A20H_REG_OUT_Z_NEW_H            0x65U /**< live Z axis high byte */
#define SC7A20H_REG_OUT_Z_NEW_L            0x66U /**< live Z axis low byte */
#define SC7A20H_REG_SOFT_RESET             0x68U /**< software reset */
#define SC7A20H_REG_FIFO_DATA              0x69U /**< FIFO data port */
#define SC7A20H_REG_I2C_CTRL               0x6FU /**< IIC enable control */
#define SC7A20H_REG_VERSION                0x70U /**< chip version */

/**
 * @brief sc7a20h oversampling ratio enumeration definition
 */
typedef enum
{
    SC7A20H_OSR_1 = 0x00, /**< output data rate */
    SC7A20H_OSR_2 = 0x01, /**< output data rate divided by 2 */
    SC7A20H_OSR_4 = 0x02, /**< output data rate divided by 4 */
    SC7A20H_OSR_8 = 0x03, /**< output data rate divided by 8 */
    SC7A20H_OSR_16 = 0x04, /**< output data rate divided by 16 */
    SC7A20H_OSR_32_A = 0x05, /**< output data rate divided by 32 */
    SC7A20H_OSR_32_B = 0x06, /**< output data rate divided by 32 */
    SC7A20H_OSR_32_C = 0x07 /**< output data rate divided by 32 */
} sc7a20h_osr_t;

/**
 * @brief sc7a20h filter configuration structure
 */
typedef struct sc7a20h_filter_config_s
{
    uint8_t high_pass_cutoff; /**< high-pass cutoff selection, 0 through 3 */
    uint8_t low_pass_cutoff; /**< low-pass cutoff selection, 0 through 3 */
    uint8_t high_pass_data; /**< select high-pass filtered output */
    uint8_t high_pass_raw_source; /**< select raw ADC data for high-pass */
    uint8_t aoi1_high_pass; /**< enable high-pass filtering for AOI1 */
    uint8_t aoi2_high_pass; /**< enable high-pass filtering for AOI2 */
} sc7a20h_filter_config_t;

/**
 * @brief sc7a20h data structure
 */
typedef struct sc7a20h_data_s
{
    int16_t raw[3];       /**< raw signed acceleration data */
    float acceleration_g[3]; /**< acceleration in g */
    float acceleration_mps2[3]; /**< acceleration in meters per second squared */
} sc7a20h_data_t;

/**
 * @brief sc7a20h handle structure
 */
typedef struct sc7a20h_handle_s
{
    uint8_t (*iic_init)(void); /**< point to an iic_init function address */
    uint8_t (*iic_deinit)(void); /**< point to an iic_deinit function address */
    uint8_t (*iic_write)(uint8_t addr, uint8_t reg, uint8_t *buf, uint16_t len); /**< point to an iic_write function address */
    uint8_t (*iic_read)(uint8_t addr, uint8_t reg, uint8_t *buf, uint16_t len); /**< point to an iic_read function address */
    uint8_t (*spi_init)(void); /**< point to a spi_init function address */
    uint8_t (*spi_deinit)(void); /**< point to a spi_deinit function address */
    uint8_t (*spi_write)(uint8_t reg, uint8_t *buf, uint16_t len); /**< point to a spi_write function address */
    uint8_t (*spi_read)(uint8_t reg, uint8_t *buf, uint16_t len); /**< point to a spi_read function address */
    void (*receive_callback)(uint16_t type); /**< point to a receive_callback function address */
    void (*delay_ms)(uint32_t ms); /**< point to a delay_ms function address */
    void (*debug_print)(const char *const fmt, ...); /**< point to a debug_print function address */
    uint8_t iic_addr; /**< IIC 7-bit device address */
    uint8_t interface; /**< selected bus interface */
    uint8_t full_scale; /**< cached full scale */
    uint8_t inited; /**< inited flag */
} sc7a20h_handle_t;

/**
 * @brief sc7a20h information structure
 */
typedef struct sc7a20h_info_s
{
    char chip_name[32]; /**< chip name */
    char manufacturer_name[32]; /**< manufacturer name */
    char interface[8]; /**< supported interface name */
    float supply_voltage_min_v; /**< chip min supply voltage */
    float supply_voltage_max_v; /**< chip max supply voltage */
    float max_current_ma; /**< chip max current */
    float temperature_min; /**< chip min operating temperature */
    float temperature_max; /**< chip max operating temperature */
    uint32_t driver_version; /**< driver version */
} sc7a20h_info_t;

/**
 * @defgroup sc7a20h_link_driver sc7a20h link driver function
 * @brief    sc7a20h link driver modules
 * @{
 */
#define DRIVER_SC7A20H_LINK_INIT(handle, structure)             memset((handle), 0, sizeof(structure))
#define DRIVER_SC7A20H_LINK_IIC_INIT(handle, f)                  ((handle)->iic_init = (f))
#define DRIVER_SC7A20H_LINK_IIC_DEINIT(handle, f)                ((handle)->iic_deinit = (f))
#define DRIVER_SC7A20H_LINK_IIC_WRITE(handle, f)                 ((handle)->iic_write = (f))
#define DRIVER_SC7A20H_LINK_IIC_READ(handle, f)                  ((handle)->iic_read = (f))
#define DRIVER_SC7A20H_LINK_SPI_INIT(handle, f)                  ((handle)->spi_init = (f))
#define DRIVER_SC7A20H_LINK_SPI_DEINIT(handle, f)                ((handle)->spi_deinit = (f))
#define DRIVER_SC7A20H_LINK_SPI_WRITE(handle, f)                 ((handle)->spi_write = (f))
#define DRIVER_SC7A20H_LINK_SPI_READ(handle, f)                  ((handle)->spi_read = (f))
#define DRIVER_SC7A20H_LINK_RECEIVE_CALLBACK(handle, f)          ((handle)->receive_callback = (f))
#define DRIVER_SC7A20H_LINK_DELAY_MS(handle, f)                  ((handle)->delay_ms = (f))
#define DRIVER_SC7A20H_LINK_DEBUG_PRINT(handle, f)               ((handle)->debug_print = (f))
/** @} */

/**
 * @addtogroup sc7a20h_base_driver
 * @{
 */
/**
 * @brief      get chip information
 * @param[out] *info pointer to a chip information structure
 * @return     status code
 *             - 0 success
 *             - 1 get information failed
 *             - 2 information pointer is invalid
 * @note       none
 */
uint8_t sc7a20h_info(sc7a20h_info_t *info);

/**
 * @brief      set IIC address pin state
 * @param[in]  *handle pointer to a sc7a20h handle structure
 * @param[in]  addr IIC 7-bit address
 * @return     status code
 *             - 0 success
 *             - 2 handle is invalid
 *             - 3 handle is initialized
 *             - 4 address is invalid
 * @note       set before initialization
 */
uint8_t sc7a20h_set_addr_pin(sc7a20h_handle_t *handle, sc7a20h_address_t addr);

/**
 * @brief      get IIC address pin state
 * @param[in]  *handle pointer to a sc7a20h handle structure
 * @param[out] *addr pointer to an IIC address
 * @return     status code
 *             - 0 success
 *             - 2 handle is invalid
 *             - 3 address pointer is invalid
 * @note       none
 */
uint8_t sc7a20h_get_addr_pin(sc7a20h_handle_t *handle, sc7a20h_address_t *addr);

/**
 * @brief      select bus interface
 * @param[in]  *handle pointer to a sc7a20h handle structure
 * @param[in]  interface selected bus interface
 * @return     status code
 *             - 0 success
 *             - 2 handle is invalid
 *             - 3 handle is initialized
 *             - 4 interface is invalid
 * @note       SPI access is limited to registers 0x00 through 0x3F
 */
uint8_t sc7a20h_set_interface(sc7a20h_handle_t *handle, sc7a20h_interface_t interface);

/**
 * @brief      initialize the chip
 * @param[in]  *handle pointer to a sc7a20h handle structure
 * @return     status code
 *             - 0 success
 *             - 1 bus initialization or device read failed
 *             - 2 handle is invalid
 *             - 3 interface function pointer is null
 *             - 4 chip identification is invalid
 * @note       verifies WHO_AM_I at register 0x0F
 */
uint8_t sc7a20h_init(sc7a20h_handle_t *handle);

/**
 * @brief      deinitialize the chip
 * @param[in]  *handle pointer to a sc7a20h handle structure
 * @return     status code
 *             - 0 success
 *             - 1 bus deinitialization failed
 *             - 2 handle is invalid
 *             - 3 handle is not initialized
 * @note       none
 */
uint8_t sc7a20h_deinit(sc7a20h_handle_t *handle);

/**
 * @brief      read registers
 * @param[in]  *handle pointer to a sc7a20h handle structure
 * @param[in]  reg register address
 * @param[out] *buf pointer to a data buffer
 * @param[in]  len length of the data buffer
 * @return     status code
 *             - 0 success
 *             - 1 bus read failed
 *             - 2 handle is invalid
 *             - 3 handle is not initialized or interface pointer is null
 *             - 4 buffer is invalid, register range is invalid, or SPI address is unsupported
 * @note       IIC bursts use the register auto-increment bit
 */
uint8_t sc7a20h_get_reg(sc7a20h_handle_t *handle, uint8_t reg, uint8_t *buf, uint16_t len);

/**
 * @brief      write registers
 * @param[in]  *handle pointer to a sc7a20h handle structure
 * @param[in]  reg register address
 * @param[in]  *buf pointer to a data buffer
 * @param[in]  len length of the data buffer
 * @return     status code
 *             - 0 success
 *             - 1 bus write failed
 *             - 2 handle is invalid
 *             - 3 handle is not initialized or interface pointer is null
 *             - 4 buffer is invalid, register is read-only/reserved, or SPI address is unsupported
 * @note       only documented writable registers are accepted
 */
uint8_t sc7a20h_set_reg(sc7a20h_handle_t *handle, uint8_t reg, uint8_t *buf, uint16_t len);

/**
 * @brief      read device identity
 * @param[in]  *handle pointer to a sc7a20h handle structure
 * @param[out] *id pointer to an identity value
 * @return     status code
 *             - 0 success
 *             - 1 register read failed
 *             - 2 handle is invalid
 *             - 3 device pointer is invalid
 *             - 4 unexpected identity
 * @note       expected identity is 0x11
 */
uint8_t sc7a20h_get_device_id(sc7a20h_handle_t *handle, uint8_t *id);

/**
 * @brief      read device version
 * @param[in]  *handle pointer to a sc7a20h handle structure
 * @param[out] *version pointer to a version value
 * @return     status code
 *             - 0 success
 *             - 1 register read failed
 *             - 2 handle is invalid
 *             - 3 version pointer is invalid
 *             - 4 SPI high address is unsupported
 * @note       VERSION register 0x70 is available over IIC only
 */
uint8_t sc7a20h_get_version(sc7a20h_handle_t *handle, uint8_t *version);

/**
 * @brief      reset the chip
 * @param[in]  *handle pointer to a sc7a20h handle structure
 * @return     status code
 *             - 0 success
 *             - 1 reset or identity check failed
 *             - 2 handle is invalid
 *             - 3 handle is not initialized
 * @note       writes 0xA5 to SOFT_RESET and waits 1 ms
 */
uint8_t sc7a20h_reset(sc7a20h_handle_t *handle);

/**
 * @brief      read output data status
 * @param[in]  *handle pointer to a sc7a20h handle structure
 * @param[out] *status pointer to a status value
 * @return     status code
 *             - 0 success
 *             - 1 register read failed
 *             - 2 handle is invalid
 *             - 3 status pointer is invalid
 * @note       none
 */
uint8_t sc7a20h_get_status(sc7a20h_handle_t *handle, uint8_t *status);

/**
 * @brief      read acceleration data
 * @param[in]  *handle pointer to a sc7a20h handle structure
 * @param[out] *data pointer to acceleration data
 * @return     status code
 *             - 0 success
 *             - 1 register read failed
 *             - 2 handle is invalid
 *             - 3 data pointer is invalid
 *             - 4 new data is not ready
 * @note       raw data is signed 12-bit two's complement, left aligned
 */
uint8_t sc7a20h_read(sc7a20h_handle_t *handle, sc7a20h_data_t *data);

/**
 * @brief      set output data rate
 * @param[in]  *handle pointer to a sc7a20h handle structure
 * @param[in]  odr output data rate
 * @return     status code
 *             - 0 success
 *             - 1 register read or write failed
 *             - 2 handle is invalid
 *             - 3 handle is not initialized
 *             - 4 output data rate is invalid
 * @note       none
 */
uint8_t sc7a20h_set_odr(sc7a20h_handle_t *handle, sc7a20h_odr_t odr);

/**
 * @brief      get output data rate
 * @param[in]  *handle pointer to a sc7a20h handle structure
 * @param[out] *odr pointer to an output data rate
 * @return     status code
 *             - 0 success
 *             - 1 register read failed
 *             - 2 handle is invalid
 *             - 3 output data rate pointer is invalid
 *             - 4 register value is invalid
 * @note       none
 */
uint8_t sc7a20h_get_odr(sc7a20h_handle_t *handle, sc7a20h_odr_t *odr);

/**
 * @brief      set power mode
 * @param[in]  *handle pointer to a sc7a20h handle structure
 * @param[in]  mode power mode
 * @return     status code
 *             - 0 success
 *             - 1 register read or write failed
 *             - 2 handle is invalid
 *             - 3 handle is not initialized
 *             - 4 mode is invalid
 * @note       updates CTRL_REG0 and CTRL_REG1 together
 */
uint8_t sc7a20h_set_mode(sc7a20h_handle_t *handle, sc7a20h_mode_t mode);

/**
 * @brief      get power mode
 * @param[in]  *handle pointer to a sc7a20h handle structure
 * @param[out] *mode pointer to a power mode
 * @return     status code
 *             - 0 success
 *             - 1 register read failed
 *             - 2 handle is invalid
 *             - 3 mode pointer is invalid
 *             - 4 register state is invalid
 * @note       none
 */
uint8_t sc7a20h_get_mode(sc7a20h_handle_t *handle, sc7a20h_mode_t *mode);

/**
 * @brief      set full scale
 * @param[in]  *handle pointer to a sc7a20h handle structure
 * @param[in]  scale full scale
 * @return     status code
 *             - 0 success
 *             - 1 register read or write failed
 *             - 2 handle is invalid
 *             - 3 handle is not initialized
 *             - 4 full scale is invalid
 * @note       none
 */
uint8_t sc7a20h_set_scale(sc7a20h_handle_t *handle, sc7a20h_scale_t scale);

/**
 * @brief      get full scale
 * @param[in]  *handle pointer to a sc7a20h handle structure
 * @param[out] *scale pointer to a full scale
 * @return     status code
 *             - 0 success
 *             - 1 register read failed
 *             - 2 handle is invalid
 *             - 3 scale pointer is invalid
 * @note       none
 */
uint8_t sc7a20h_get_scale(sc7a20h_handle_t *handle, sc7a20h_scale_t *scale);

/**
 * @brief      enable or disable sensor axes
 * @param[in]  *handle pointer to a sc7a20h handle structure
 * @param[in]  axes axis enable bit mask, bit 0 X, bit 1 Y, bit 2 Z
 * @return     status code
 *             - 0 success
 *             - 1 register read or write failed
 *             - 2 handle is invalid
 *             - 3 handle is not initialized
 *             - 4 axis mask is invalid
 * @note       bits outside the three axis enable bits are rejected
 */
uint8_t sc7a20h_set_axes(sc7a20h_handle_t *handle, uint8_t axes);

/**
 * @brief      get enabled sensor axes
 * @param[in]  *handle pointer to a sc7a20h handle structure
 * @param[out] *axes pointer to an axis enable bit mask
 * @return     status code
 *             - 0 success
 *             - 1 register read failed
 *             - 2 handle is invalid
 *             - 3 axis pointer is invalid
 * @note       bit 0 X, bit 1 Y, bit 2 Z
 */
uint8_t sc7a20h_get_axes(sc7a20h_handle_t *handle, uint8_t *axes);

/**
 * @brief      set oversampling ratio
 * @param[in]  *handle pointer to a sc7a20h handle structure
 * @param[in]  osr oversampling ratio
 * @return     status code
 *             - 0 success
 *             - 1 register access failed
 *             - 2 handle is invalid
 *             - 3 handle is not initialized
 *             - 4 ratio is invalid
 * @note       none
 */
uint8_t sc7a20h_set_osr(sc7a20h_handle_t *handle, sc7a20h_osr_t osr);

/**
 * @brief      configure high-pass and low-pass filters
 * @param[in]  *handle pointer to a sc7a20h handle structure
 * @param[in]  *config pointer to a filter configuration structure
 * @return     status code
 *             - 0 success
 *             - 1 register access failed
 *             - 2 handle is invalid
 *             - 3 handle is not initialized or config pointer is invalid
 *             - 4 configuration field is invalid
 * @note       filter selections follow CTRL_REG0, CTRL_REG2 and CTRL_REG4
 */
uint8_t sc7a20h_set_filter(sc7a20h_handle_t *handle, const sc7a20h_filter_config_t *config);

/**
 * @brief      get high-pass and low-pass filter configuration
 * @param[in]  *handle pointer to a sc7a20h handle structure
 * @param[out] *config pointer to a filter configuration structure
 * @return     status code
 *             - 0 success
 *             - 1 register read failed
 *             - 2 handle is invalid
 *             - 3 handle is not initialized or config pointer is invalid
 * @note       none
 */
uint8_t sc7a20h_get_filter(sc7a20h_handle_t *handle, sc7a20h_filter_config_t *config);

/**
 * @brief      set block data update
 * @param[in]  *handle pointer to a sc7a20h handle structure
 * @param[in]  enable enable or disable block data update
 * @return     status code
 *             - 0 success
 *             - 1 register access failed
 *             - 2 handle is invalid
 *             - 3 handle is not initialized
 *             - 4 boolean value is invalid
 * @note       BDU prevents output registers changing until both bytes are read
 */
uint8_t sc7a20h_set_block_data_update(sc7a20h_handle_t *handle, sc7a20h_bool_t enable);

/**
 * @brief      set output byte order
 * @param[in]  *handle pointer to a sc7a20h handle structure
 * @param[in]  big_endian select high byte at lower register address
 * @return     status code
 *             - 0 success
 *             - 1 register access failed
 *             - 2 handle is invalid
 *             - 3 handle is not initialized
 *             - 4 boolean value is invalid
 * @note       sc7a20h_read supports either byte order
 */
uint8_t sc7a20h_set_big_endian(sc7a20h_handle_t *handle, sc7a20h_bool_t big_endian);

/**
 * @brief      configure internal digital pull-ups
 * @param[in]  *handle pointer to a sc7a20h handle structure
 * @param[in]  disable_sdo_pullup disable SDO internal pull-up
 * @param[in]  disable_iic_pullup disable SDA and SCL internal pull-ups
 * @return     status code
 *             - 0 success
 *             - 1 register access failed
 *             - 2 handle is invalid
 *             - 3 handle is not initialized
 *             - 4 boolean value is invalid
 * @note       external pull-ups or defined pin levels may be required
 */
uint8_t sc7a20h_set_internal_pullups(sc7a20h_handle_t *handle,
                                     sc7a20h_bool_t disable_sdo_pullup,
                                     sc7a20h_bool_t disable_iic_pullup);

/**
 * @brief      start continuous data acquisition
 * @param[in]  *handle pointer to a sc7a20h handle structure
 * @param[in]  odr output data rate
 * @return     status code
 *             - 0 success
 *             - 1 register update failed
 *             - 2 handle is invalid
 *             - 3 handle is not initialized
 *             - 4 output data rate is invalid
 * @note       enables all axes and normal mode
 */
uint8_t sc7a20h_start_continuous_read(sc7a20h_handle_t *handle, sc7a20h_odr_t odr);

/**
 * @brief      stop continuous data acquisition
 * @param[in]  *handle pointer to a sc7a20h handle structure
 * @return     status code
 *             - 0 success
 *             - 1 register update failed
 *             - 2 handle is invalid
 *             - 3 handle is not initialized
 * @note       sets the output data rate to power down
 */
uint8_t sc7a20h_stop_continuous_read(sc7a20h_handle_t *handle);
/** @} */

/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif
