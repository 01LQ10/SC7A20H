/**
 * @file      driver_sc7a20h_interrupt_example.c
 * @brief     driver sc7a20h interrupt example source file
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

#include "driver_sc7a20h_interrupt_example.h"
#include "../src/driver_sc7a20h_interrupt.h"
#include "driver_sc7a20h_interface.h"

static sc7a20h_handle_t gs_handle; /**< sc7a20h handle */

/**
 * @brief      report interrupt event
 * @param[in]  type interrupt event type
 * @return     none
 * @note       callback executes in the caller context
 */
static void a_sc7a20h_interrupt_callback(uint16_t type)
{
    sc7a20h_interface_debug_print("sc7a20h: interrupt event 0x%04X.\n", (unsigned int)type); /* report event */
}

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
 * @note       route data ready to INT1
 */
uint8_t sc7a20h_interrupt_init(sc7a20h_interface_t interface, sc7a20h_address_t address)
{
    uint8_t res;

    DRIVER_SC7A20H_LINK_INIT(&gs_handle, sc7a20h_handle_t); /* initialize handle */
    DRIVER_SC7A20H_LINK_IIC_INIT(&gs_handle, sc7a20h_interface_iic_init);
    DRIVER_SC7A20H_LINK_IIC_DEINIT(&gs_handle, sc7a20h_interface_iic_deinit);
    DRIVER_SC7A20H_LINK_IIC_WRITE(&gs_handle, sc7a20h_interface_iic_write);
    DRIVER_SC7A20H_LINK_IIC_READ(&gs_handle, sc7a20h_interface_iic_read);
    DRIVER_SC7A20H_LINK_SPI_INIT(&gs_handle, sc7a20h_interface_spi_init);
    DRIVER_SC7A20H_LINK_SPI_DEINIT(&gs_handle, sc7a20h_interface_spi_deinit);
    DRIVER_SC7A20H_LINK_SPI_WRITE(&gs_handle, sc7a20h_interface_spi_write);
    DRIVER_SC7A20H_LINK_SPI_READ(&gs_handle, sc7a20h_interface_spi_read);
    DRIVER_SC7A20H_LINK_RECEIVE_CALLBACK(&gs_handle, a_sc7a20h_interrupt_callback);
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
        res = sc7a20h_start_continuous_read(&gs_handle, SC7A20H_INTERRUPT_DEFAULT_ODR); /* start conversion */
    }
    if (res == 0U)
    {
        res = sc7a20h_interrupt_set_route(&gs_handle, SC7A20H_INT1_DATA_READY, 0U); /* route data-ready */
    }
    if (res != 0U)
    {
        sc7a20h_interface_debug_print("sc7a20h: interrupt init failed.\n"); /* init failed */
        if (gs_handle.inited == 1U)
        {
            (void)sc7a20h_deinit(&gs_handle); /* rollback sensor */
        }
        return 1; /* return error */
    }

    return 0; /* success return 0 */
}

/**
 * @brief      service sensor interrupt
 * @return     status code
 *             - 0 success
 *             - 1 interrupt service failed
 * @note       defer this call if bus access is not interrupt-safe
 */
uint8_t sc7a20h_interrupt_handle(void)
{
    uint8_t res;

    res = sc7a20h_interrupt_irq_handler(&gs_handle); /* read and dispatch interrupt sources */
    if (res != 0U)
    {
        sc7a20h_interface_debug_print("sc7a20h: interrupt service failed.\n"); /* service failed */
        return 1; /* return error */
    }

    return 0; /* success return 0 */
}

/**
 * @brief      deinitialize interrupt example
 * @return     status code
 *             - 0 success
 *             - 1 deinitialization failed
 * @note       stops conversions before closing the bus
 */
uint8_t sc7a20h_interrupt_deinit(void)
{
    uint8_t res;

    res = sc7a20h_stop_continuous_read(&gs_handle); /* stop conversion */
    if (res == 0U)
    {
        res = sc7a20h_deinit(&gs_handle); /* close bus */
    }
    if (res != 0U)
    {
        sc7a20h_interface_debug_print("sc7a20h: interrupt deinit failed.\n"); /* deinit failed */
        return 1; /* return error */
    }

    return 0; /* success return 0 */
}

/** @} */
