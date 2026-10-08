/**
 * @file      driver_sc7a20h_read_test.c
 * @brief     driver sc7a20h read test source file
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

#include "driver_sc7a20h_read_test.h"
#include "driver_sc7a20h_interface.h"

static sc7a20h_handle_t gs_handle; /**< sc7a20h test handle */

/**
 * @addtogroup sc7a20h_test_driver
 * @{
 */

/**
 * @brief      test acceleration data reading
 * @param[in]  interface bus interface
 * @param[in]  address IIC address selection
 * @param[in]  times sample count
 * @return     status code
 *             - 0 success
 *             - 1 test failed
 * @note       hardware test; provide platform interface implementations
 */
uint8_t sc7a20h_read_test(sc7a20h_interface_t interface, sc7a20h_address_t address, uint32_t times)
{
    uint32_t i;
    uint8_t res;
    sc7a20h_data_t data;

    if (times == 0U)
    {
        sc7a20h_interface_debug_print("sc7a20h: read test count is zero.\n"); /* invalid test count */
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
    if (res == 0U)
    {
        res = sc7a20h_set_scale(&gs_handle, SC7A20H_SCALE_16G); /* configure scale */
    }
    if (res == 0U)
    {
        res = sc7a20h_start_continuous_read(&gs_handle, SC7A20H_ODR_100_HZ); /* start acquisition */
    }
    if (res != 0U)
    {
        sc7a20h_interface_debug_print("sc7a20h: read test setup failed.\n"); /* setup failed */
        if (gs_handle.inited == 1U)
        {
            (void)sc7a20h_deinit(&gs_handle); /* rollback sensor */
        }
        return 1; /* return error */
    }
    for (i = 0U; i < times; i++)
    {
        sc7a20h_interface_delay_ms(20U); /* wait for next conversion */
        res = sc7a20h_read(&gs_handle, &data); /* read sample */
        if (res != 0U)
        {
            sc7a20h_interface_debug_print("sc7a20h: read test sample failed.\n"); /* sample failed */
            (void)sc7a20h_stop_continuous_read(&gs_handle); /* stop acquisition */
            (void)sc7a20h_deinit(&gs_handle); /* close bus */
            return 1; /* return error */
        }
        sc7a20h_interface_debug_print("sc7a20h:%d,%d,%d,%0.3f,%0.3f,%0.3f\n",
                                      (int)data.raw[0], (int)data.raw[1], (int)data.raw[2],
                                      data.acceleration_g[0], data.acceleration_g[1],
                                      data.acceleration_g[2]); /* print sample */
    }
    res = sc7a20h_stop_continuous_read(&gs_handle); /* stop acquisition */
    if (res == 0U)
    {
        res = sc7a20h_deinit(&gs_handle); /* close bus */
    }
    if (res != 0U)
    {
        sc7a20h_interface_debug_print("sc7a20h: read test cleanup failed.\n"); /* cleanup failed */
        return 1; /* return error */
    }

    return 0; /* success return 0 */
}

/** @} */
