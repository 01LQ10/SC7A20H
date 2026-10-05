/**
 * @file      driver_sc7a20h_basic.h
 * @brief     driver sc7a20h basic example header file
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

#ifndef DRIVER_SC7A20H_BASIC_H
#define DRIVER_SC7A20H_BASIC_H

#include "driver_sc7a20h.h"

#ifdef __cplusplus
extern "C" {
#endif

#define SC7A20H_BASIC_DEFAULT_ODR SC7A20H_ODR_100_HZ /**< default output data rate */
#define SC7A20H_BASIC_DEFAULT_SCALE SC7A20H_SCALE_2G /**< default full scale */

/**
 * @defgroup sc7a20h_example_driver sc7a20h example driver function
 * @brief    sc7a20h example driver modules
 * @{
 */
/**
 * @brief      initialize basic example
 * @param[in]  interface bus interface
 * @param[in]  address IIC address selection
 * @return     status code
 *             - 0 success
 *             - 1 initialization failed
 * @note       none
 */
uint8_t sc7a20h_basic_init(sc7a20h_interface_t interface, sc7a20h_address_t address);
/**
 * @brief      read acceleration sample
 * @param[out] *data pointer to acceleration data
 * @return     status code
 *             - 0 success
 *             - 1 read failed
 *             - 2 data pointer is invalid
 * @note       none
 */
uint8_t sc7a20h_basic_read(sc7a20h_data_t *data);
/**
 * @brief  deinitialize basic example
 * @return status code
 *         - 0 success
 *         - 1 deinitialization failed
 * @note   none
 */
uint8_t sc7a20h_basic_deinit(void);
/** @} */

#ifdef __cplusplus
}
#endif

#endif
