/**
 * @file      driver_sc7a20h_advance.c
 * @brief     driver sc7a20h advance example source file
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

#include "driver_sc7a20h_advance.h"
#include "driver_sc7a20h_fifo.h"
#include "driver_sc7a20h_interface.h"

static sc7a20h_handle_t gs_handle; /**< sc7a20h handle */

/**
 * @addtogroup sc7a20h_example_driver
 * @{
 */

/**
 * @brief      initialize advanced fifo example
 * @param[in]  interface bus interface
 * @param[in]  address IIC address selection
 * @return     status code
 *             - 0 success
 *             - 1 initialization failed
 * @note       SPI high address registers are unsupported
 */
uint8_t sc7a20h_advance_init(sc7a20h_interface_t interface, sc7a20h_address_t address)
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
        res = sc7a20h_start_continuous_read(&gs_handle, SC7A20H_ODR_100_HZ); /* start acquisition */
    }
    if (res == 0U)
    {
        res = sc7a20h_fifo_configure(&gs_handle, SC7A20H_FIFO_STREAM, SC7A20H_BOOL_FALSE,
                                     SC7A20H_ADVANCE_DEFAULT_FIFO_WATERMARK,
                                     SC7A20H_BOOL_TRUE, SC7A20H_BOOL_FALSE); /* configure FIFO */
    }
    if (res != 0U)
    {
        sc7a20h_interface_debug_print("sc7a20h: advance init failed.\n"); /* init failed */
        if (gs_handle.inited == 1U)
        {
            (void)sc7a20h_deinit(&gs_handle); /* rollback sensor */
        }
        return 1; /* return error */
    }

    return 0; /* success return 0 */
}

/**
 * @brief      read fifo bytes in advanced example
 * @param[out] *buf pointer to a FIFO data buffer
 * @param[in]  len number of bytes to read
 * @return     status code
 *             - 0 success
 *             - 1 FIFO read failed
 *             - 2 buffer is invalid
 * @note       caller chooses the byte count; no group-size conversion is assumed
 */
uint8_t sc7a20h_advance_fifo_read(uint8_t *buf, uint16_t len)
{
    uint8_t res;

    if (buf == NULL)
    {
        return 2; /* return error */
    }
    res = sc7a20h_fifo_read_bytes(&gs_handle, buf, len); /* read raw FIFO bytes */
    if (res != 0U)
    {
        sc7a20h_interface_debug_print("sc7a20h: advance fifo read failed.\n"); /* FIFO read failed */
        return 1; /* return error */
    }

    return 0; /* success return 0 */
}

/**
 * @brief      deinitialize advanced example
 * @return     status code
 *             - 0 success
 *             - 1 deinitialization failed
 * @note       disables FIFO before closing the bus
 */
uint8_t sc7a20h_advance_deinit(void)
{
    uint8_t res;

    res = sc7a20h_fifo_configure(&gs_handle, SC7A20H_FIFO_BYPASS, SC7A20H_BOOL_FALSE,
                                 0U, SC7A20H_BOOL_FALSE, SC7A20H_BOOL_FALSE); /* disable FIFO */
    if (res != 0U)
    {
        sc7a20h_interface_debug_print("sc7a20h: disable fifo failed.\n"); /* FIFO disable failed */
        (void)sc7a20h_deinit(&gs_handle); /* close bus */
        return 1; /* return error */
    }
    res = sc7a20h_stop_continuous_read(&gs_handle); /* stop acquisition */
    if (res == 0U)
    {
        res = sc7a20h_deinit(&gs_handle); /* close bus */
    }
    if (res != 0U)
    {
        sc7a20h_interface_debug_print("sc7a20h: advance deinit failed.\n"); /* deinit failed */
        return 1; /* return error */
    }

    return 0; /* success return 0 */
}

/** @} */
