/**
 * @file      driver_sc7a20h_self_test.h
 * @brief     driver sc7a20h self test header file
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

#ifndef DRIVER_SC7A20H_SELF_TEST_H
#define DRIVER_SC7A20H_SELF_TEST_H

#include "driver_sc7a20h.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief sc7a20h self test mode enumeration definition
 */
typedef enum
{
    SC7A20H_SELF_TEST_DISABLED = 0x00, /**< self test disabled */
    SC7A20H_SELF_TEST_MODE_0 = 0x01, /**< self test mode 0 */
    SC7A20H_SELF_TEST_MODE_1 = 0x02  /**< self test mode 1 */
} sc7a20h_self_test_mode_t;

/**
 * @addtogroup sc7a20h_extend_driver
 * @{
 */
/**
 * @brief      set self test mode
 * @param[in]  *handle pointer to a sc7a20h handle structure
 * @param[in]  mode self test mode
 * @return     status code
 *             - 0 success
 *             - 1 register access failed
 *             - 2 handle is invalid
 *             - 3 handle is not initialized
 *             - 4 mode is invalid
 * @note       none
 */
uint8_t sc7a20h_self_test_set_mode(sc7a20h_handle_t *handle, sc7a20h_self_test_mode_t mode);
/**
 * @brief      get self test mode
 * @param[in]  *handle pointer to a sc7a20h handle structure
 * @param[out] *mode pointer to a self test mode
 * @return     status code
 *             - 0 success
 *             - 1 register read failed
 *             - 2 handle is invalid
 *             - 3 mode pointer is invalid
 *             - 4 mode is invalid
 * @note       none
 */
uint8_t sc7a20h_self_test_get_mode(sc7a20h_handle_t *handle, sc7a20h_self_test_mode_t *mode);
/** @} */

#ifdef __cplusplus
}
#endif

#endif
