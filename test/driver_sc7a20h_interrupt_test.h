/**
 * @file      driver_sc7a20h_interrupt_test.h
 * @brief     driver sc7a20h interrupt test header file
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

#ifndef DRIVER_SC7A20H_INTERRUPT_TEST_H
#define DRIVER_SC7A20H_INTERRUPT_TEST_H

#include "driver_sc7a20h.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @addtogroup sc7a20h_test_driver
 * @{
 */
/**
 * @brief      test interrupt status reading
 * @param[in]  interface bus interface
 * @param[in]  address IIC address selection
 * @param[in]  times polling count
 * @return     status code
 *             - 0 success
 *             - 1 test failed
 * @note       requires a physical sensor
 */
uint8_t sc7a20h_interrupt_test(sc7a20h_interface_t interface, sc7a20h_address_t address, uint32_t times);
/** @} */

#ifdef __cplusplus
}
#endif

#endif
