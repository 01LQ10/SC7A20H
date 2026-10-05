/**
 * @file      driver_sc7a20h_interface.h
 * @brief     driver sc7a20h interface header file
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

#ifndef DRIVER_SC7A20H_INTERFACE_H
#define DRIVER_SC7A20H_INTERFACE_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup sc7a20h_interface_driver sc7a20h interface driver function
 * @brief    sc7a20h interface driver modules
 * @{
 */
/**
 * @brief  interface iic bus init
 * @return status code
 *         - 0 success
 *         - 1 initialization failed
 * @note   none
 */
uint8_t sc7a20h_interface_iic_init(void);
/**
 * @brief  interface iic bus deinit
 * @return status code
 *         - 0 success
 *         - 1 deinitialization failed
 * @note   none
 */
uint8_t sc7a20h_interface_iic_deinit(void);
/**
 * @brief      interface iic register write
 * @param[in]  addr 7-bit IIC device address
 * @param[in]  reg register sub-address
 * @param[in]  *buf pointer to write data
 * @param[in]  len write data length
 * @return     status code
 *             - 0 success
 *             - 1 write failed
 * @note       bit 7 of reg indicates auto-increment for burst transfers
 */
uint8_t sc7a20h_interface_iic_write(uint8_t addr, uint8_t reg, uint8_t *buf, uint16_t len);
/**
 * @brief      interface iic register read
 * @param[in]  addr 7-bit IIC device address
 * @param[in]  reg register sub-address
 * @param[out] *buf pointer to read data
 * @param[in]  len read data length
 * @return     status code
 *             - 0 success
 *             - 1 read failed
 * @note       driver sets bit 7 of reg for burst auto-increment
 */
uint8_t sc7a20h_interface_iic_read(uint8_t addr, uint8_t reg, uint8_t *buf, uint16_t len);
/**
 * @brief  interface spi bus init
 * @return status code
 *         - 0 success
 *         - 1 initialization failed
 * @note   configure SPI mode and chip select in the platform
 */
uint8_t sc7a20h_interface_spi_init(void);
/**
 * @brief  interface spi bus deinit
 * @return status code
 *         - 0 success
 *         - 1 deinitialization failed
 * @note   none
 */
uint8_t sc7a20h_interface_spi_deinit(void);
/**
 * @brief      interface spi register write
 * @param[in]  reg register address
 * @param[in]  *buf pointer to write data
 * @param[in]  len write data length
 * @return     status code
 *             - 0 success
 *             - 1 write failed
 * @note       build RW=0 and set MS=1 for multi-byte transfers
 */
uint8_t sc7a20h_interface_spi_write(uint8_t reg, uint8_t *buf, uint16_t len);
/**
 * @brief      interface spi register read
 * @param[in]  reg register address
 * @param[out] *buf pointer to read data
 * @param[in]  len read data length
 * @return     status code
 *             - 0 success
 *             - 1 read failed
 * @note       build RW=1 and set MS=1 for multi-byte transfers
 */
uint8_t sc7a20h_interface_spi_read(uint8_t reg, uint8_t *buf, uint16_t len);
/**
 * @brief     interface delay in milliseconds
 * @param[in] ms delay time
 * @return    none
 * @note      none
 */
void sc7a20h_interface_delay_ms(uint32_t ms);
/**
 * @brief      interface formatted debug output
 * @param[in]  *fmt format string
 * @return     none
 * @note       none
 */
void sc7a20h_interface_debug_print(const char *const fmt, ...);
/**
 * @brief      interface receive event callback
 * @param[in]  type event identifier
 * @return     none
 * @note       event source bits are encoded in the low byte where applicable
 */
void sc7a20h_interface_receive_callback(uint16_t type);
/** @} */

#ifdef __cplusplus
}
#endif

#endif
