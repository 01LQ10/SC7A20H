/**
 * @file      driver_sc7a20h_interrupt_example.h
 * @brief     driver sc7a20h interrupt example header file
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

#ifndef DRIVER_SC7A20H_INTERRUPT_EXAMPLE_H
#define DRIVER_SC7A20H_INTERRUPT_EXAMPLE_H

#include "driver_sc7a20h.h"

#ifdef __cplusplus
extern "C" {
#endif

#define SC7A20H_INTERRUPT_DEFAULT_ODR SC7A20H_ODR_100_HZ /**< default interrupt data rate */

/**
 * @addtogroup sc7a20h_example_driver
 * @{
 */
/**
 * @brief      initialize interrupt example
 * @param[in]  interface bus interface
 * @param[in]  address IIC address selection
 * @return     status code
 *             - 0 success
 *             - 1 initialization failed
 * @note       routes data-ready to INT1
 */
uint8_t sc7a20h_interrupt_init(sc7a20h_interface_t interface, sc7a20h_address_t address);
/**
 * @brief  service sensor interrupt status
 * @return status code
 *         - 0 success
 *         - 1 service failed
 * @note   use deferred context if bus access is not ISR safe
 */
uint8_t sc7a20h_interrupt_handle(void);
/**
 * @brief  deinitialize interrupt example
 * @return status code
 *         - 0 success
 *         - 1 deinitialization failed
 * @note   none
 */
uint8_t sc7a20h_interrupt_deinit(void);
/** @} */

#ifdef __cplusplus
}
#endif

#endif
