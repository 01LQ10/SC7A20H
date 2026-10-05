/**
 * @file      driver_sc7a20h_self_test.c
 * @brief     driver sc7a20h self test source file
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

#include "driver_sc7a20h_self_test.h"

#define SC7A20H_SELF_TEST_REG_CTRL4        0x23U /**< scale and self test control */
#define SC7A20H_SELF_TEST_MASK             0x06U /**< self test mode mask */

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
 *             - 4 self test mode is invalid
 * @note       modes 0x00 and 0x03 are not valid self test selections
 */
uint8_t sc7a20h_self_test_set_mode(sc7a20h_handle_t *handle, sc7a20h_self_test_mode_t mode)
{
    uint8_t reg;
    uint8_t res;

    if (handle == NULL)
    {
        return 2; /* return error */
    }
    if (handle->inited != 1U)
    {
        return 3; /* return error */
    }
    if ((mode != SC7A20H_SELF_TEST_DISABLED) &&
        (mode != SC7A20H_SELF_TEST_MODE_0) &&
        (mode != SC7A20H_SELF_TEST_MODE_1))
    {
        return 4; /* return error */
    }
    res = sc7a20h_get_reg(handle, SC7A20H_SELF_TEST_REG_CTRL4, &reg, 1U); /* read control register */
    if (res == 0U)
    {
        reg = (uint8_t)((reg & (uint8_t)~SC7A20H_SELF_TEST_MASK) | ((uint8_t)mode << 1));
        res = sc7a20h_set_reg(handle, SC7A20H_SELF_TEST_REG_CTRL4, &reg, 1U); /* set self test */
    }
    if (res != 0U)
    {
        handle->debug_print("sc7a20h: set self test mode failed.\n"); /* set self test failed */
        return 1; /* return error */
    }

    return 0; /* success return 0 */
}

/**
 * @brief      get self test mode
 * @param[in]  *handle pointer to a sc7a20h handle structure
 * @param[out] *mode pointer to a self test mode
 * @return     status code
 *             - 0 success
 *             - 1 register read failed
 *             - 2 handle is invalid
 *             - 3 mode pointer is invalid
 *             - 4 self test state is invalid
 * @note       none
 */
uint8_t sc7a20h_self_test_get_mode(sc7a20h_handle_t *handle, sc7a20h_self_test_mode_t *mode)
{
    uint8_t reg;
    uint8_t value;
    uint8_t res;

    if (handle == NULL)
    {
        return 2; /* return error */
    }
    if (handle->inited != 1U)
    {
        return 3; /* return error */
    }
    if (mode == NULL)
    {
        handle->debug_print("sc7a20h: self test mode pointer is null.\n"); /* null pointer */
        return 3; /* return error */
    }
    res = sc7a20h_get_reg(handle, SC7A20H_SELF_TEST_REG_CTRL4, &reg, 1U); /* read control register */
    if (res != 0U)
    {
        return 1; /* return error */
    }
    value = (uint8_t)((reg & SC7A20H_SELF_TEST_MASK) >> 1);
    if (value == 0U)
    {
        *mode = SC7A20H_SELF_TEST_DISABLED; /* return disabled state */
    }
    else if (value == (uint8_t)SC7A20H_SELF_TEST_MODE_0)
    {
        *mode = SC7A20H_SELF_TEST_MODE_0; /* return mode 0 */
    }
    else if (value == (uint8_t)SC7A20H_SELF_TEST_MODE_1)
    {
        *mode = SC7A20H_SELF_TEST_MODE_1; /* return mode 1 */
    }
    else
    {
        handle->debug_print("sc7a20h: self test state is invalid.\n"); /* invalid mode */
        return 4; /* return error */
    }

    return 0; /* success return 0 */
}

/** @} */
