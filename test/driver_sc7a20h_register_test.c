/**
 * @file      driver_sc7a20h_register_test.c
 * @brief     driver sc7a20h register test source file
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

#include "driver_sc7a20h_register_test.h"
#include "driver_sc7a20h_interface.h"

static sc7a20h_handle_t gs_handle; /**< sc7a20h test handle */

/**
 * @addtogroup sc7a20h_test_driver
 * @{
 */

/**
 * @brief      test writable register access
 * @param[in]  interface bus interface
 * @param[in]  address IIC address selection
 * @param[in]  times test count
 * @return     status code
 *             - 0 success
 *             - 1 test failed
 * @note       tests and restores the CTRL_REG4 BDU bit
 */
uint8_t sc7a20h_register_test(sc7a20h_interface_t interface, sc7a20h_address_t address, uint32_t times)
{
    uint32_t i;
    uint8_t res;
    uint8_t original;
    uint8_t changed;
    uint8_t readback;

    if (times == 0U)
    {
        sc7a20h_interface_debug_print("sc7a20h: register test count is zero.\n"); /* invalid test count */
        return 1; /* return error */
    }
    DRIVER_SC7A20H_LINK_INIT(&gs_handle, sc7a20h_handle_t); /* initialize handle */
    DRIVER_SC7A20H_LINK_IIC_INIT(&gs_handle, sc7a20h_interface_iic_init);
    DRIVER_SC7A20H_LINK_IIC_DEINIT(&gs_handle, sc7a20h_interface_iic_deinit);
    DRIVER_SC7A20H_LINK_IIC_WRITE(&gs_handle, sc7a20h_interface_iic_write);
    DRIVER_SC7A20H_LINK_IIC_READ(&gs_handle, sc7a20h_interface_iic_read);
    DRIVER_SC7A20H_LINK_SPI_INIT(&gs_handle, sc7a20h_interface_spi_init);
    DRIVER_SC7A20H_LINK_SPI_DEINIT(&gs_handle, sc7a20h_interface_spi_deinit);
    DRIVER_SC7A20H_LINK_SPI_WRITE(&gs_handle, sc7a20h_interface_spi_write);
    DRIVER_SC7A20H_LINK_SPI_READ(&gs_handle, sc7a20h_interface_spi_read);
    DRIVER_SC7A20H_LINK_DELAY_MS(&gs_handle, sc7a20h_interface_delay_ms);
    DRIVER_SC7A20H_LINK_DEBUG_PRINT(&gs_handle, sc7a20h_interface_debug_print);
    res = sc7a20h_set_interface(&gs_handle, interface); /* select bus */
    if (res == 0U)
    {
        res = sc7a20h_set_addr_pin(&gs_handle, address); /* select address */
    }
    if (res == 0U)
    {
        res = sc7a20h_init(&gs_handle); /* initialize sensor */
    }
    if (res != 0U)
    {
        sc7a20h_interface_debug_print("sc7a20h: register test init failed.\n"); /* init failed */
        return 1; /* return error */
    }
    res = sc7a20h_get_reg(&gs_handle, 0x23U, &original, 1U); /* read CTRL_REG4 */
    if (res != 0U)
    {
        sc7a20h_interface_debug_print("sc7a20h: register test read failed.\n"); /* read failed */
        (void)sc7a20h_deinit(&gs_handle); /* close bus */
        return 1; /* return error */
    }
    for (i = 0U; i < times; i++)
    {
        changed = (uint8_t)(original ^ 0x80U); /* toggle BDU bit */
        res = sc7a20h_set_reg(&gs_handle, 0x23U, &changed, 1U); /* write CTRL_REG4 */
        if (res == 0U)
        {
            res = sc7a20h_get_reg(&gs_handle, 0x23U, &readback, 1U); /* read back CTRL_REG4 */
        }
        if ((res != 0U) || (readback != changed))
        {
            sc7a20h_interface_debug_print("sc7a20h: register test compare failed.\n"); /* compare failed */
            (void)sc7a20h_set_reg(&gs_handle, 0x23U, &original, 1U); /* restore register */
            (void)sc7a20h_deinit(&gs_handle); /* close bus */
            return 1; /* return error */
        }
        res = sc7a20h_set_reg(&gs_handle, 0x23U, &original, 1U); /* restore register */
        if (res != 0U)
        {
            sc7a20h_interface_debug_print("sc7a20h: register test restore failed.\n"); /* restore failed */
            (void)sc7a20h_deinit(&gs_handle); /* close bus */
            return 1; /* return error */
        }
    }
    res = sc7a20h_deinit(&gs_handle); /* close bus */
    if (res != 0U)
    {
        sc7a20h_interface_debug_print("sc7a20h: register test cleanup failed.\n"); /* cleanup failed */
        return 1; /* return error */
    }

    return 0; /* success return 0 */
}

/** @} */
