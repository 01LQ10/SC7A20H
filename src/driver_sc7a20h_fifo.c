/**
 * @file      driver_sc7a20h_fifo.c
 * @brief     driver sc7a20h fifo source file
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

#include "driver_sc7a20h_fifo.h"

#define SC7A20H_FIFO_REG_CTRL               0x2EU /**< FIFO control register */
#define SC7A20H_FIFO_REG_STATUS             0x2FU /**< FIFO source register */
#define SC7A20H_FIFO_REG_CTRL3              0x22U /**< interrupt and FIFO format register */
#define SC7A20H_FIFO_REG_CTRL5              0x24U /**< FIFO enable register */
#define SC7A20H_FIFO_REG_DATA               0x69U /**< FIFO data port */
#define SC7A20H_FIFO_MODE_MASK              0xC0U /**< FIFO mode mask */
#define SC7A20H_FIFO_TRIGGER_MASK           0x20U /**< trigger source mask */
#define SC7A20H_FIFO_WATERMARK_MASK         0x1FU /**< watermark mask */
#define SC7A20H_FIFO_ENABLE_MASK            0x40U /**< FIFO enable bit */
#define SC7A20H_FIFO_DATA_FORMAT_MASK       0x01U /**< FIFO data format bit */

/**
 * @defgroup sc7a20h_extend_driver sc7a20h extend driver function
 * @brief    sc7a20h extend driver modules
 * @{
 */

/**
 * @brief      configure fifo
 * @param[in]  *handle pointer to a sc7a20h handle structure
 * @param[in]  mode fifo operating mode
 * @param[in]  trigger_aoi2 select AOI2 as trigger source
 * @param[in]  watermark fifo watermark field
 * @param[in]  enable enable or disable fifo
 * @param[in]  data_8bit select 8-bit fifo data format
 * @return     status code
 *             - 0 success
 *             - 1 register access failed
 *             - 2 handle is invalid
 *             - 3 handle is not initialized
 *             - 4 configuration is invalid
 * @note       watermark uses the raw 5-bit FTH field
 */
uint8_t sc7a20h_fifo_configure(sc7a20h_handle_t *handle, sc7a20h_fifo_mode_t mode,
                               sc7a20h_bool_t trigger_aoi2, uint8_t watermark,
                               sc7a20h_bool_t enable, sc7a20h_bool_t data_8bit)
{
    uint8_t reg;
    uint8_t data;
    uint8_t res;

    if (handle == NULL)
    {
        return 2; /* return error */
    }
    if (handle->inited != 1U)
    {
        return 3; /* return error */
    }
    if (((uint8_t)mode > (uint8_t)SC7A20H_FIFO_TRIGGER) ||
        ((trigger_aoi2 != SC7A20H_BOOL_FALSE) && (trigger_aoi2 != SC7A20H_BOOL_TRUE)) ||
        ((enable != SC7A20H_BOOL_FALSE) && (enable != SC7A20H_BOOL_TRUE)) ||
        ((data_8bit != SC7A20H_BOOL_FALSE) && (data_8bit != SC7A20H_BOOL_TRUE)) ||
        (watermark > 0x1FU))
    {
        return 4; /* return error */
    }

    data = (uint8_t)(((uint8_t)mode << 6) |
                     ((uint8_t)trigger_aoi2 << 5) |
                     (watermark & SC7A20H_FIFO_WATERMARK_MASK));
    res = sc7a20h_get_reg(handle, SC7A20H_FIFO_REG_CTRL, &reg, 1U); /* read FIFO control */
    if (res == 0U)
    {
        reg = (uint8_t)((reg & (uint8_t)~(SC7A20H_FIFO_MODE_MASK |
                                         SC7A20H_FIFO_TRIGGER_MASK |
                                         SC7A20H_FIFO_WATERMARK_MASK)) | data);
        res = sc7a20h_set_reg(handle, SC7A20H_FIFO_REG_CTRL, &reg, 1U); /* set FIFO control */
    }
    if (res == 0U)
    {
        res = sc7a20h_get_reg(handle, SC7A20H_FIFO_REG_CTRL3, &reg, 1U); /* read FIFO format */
    }
    if (res == 0U)
    {
        reg = (uint8_t)((reg & (uint8_t)~SC7A20H_FIFO_DATA_FORMAT_MASK) |
                        ((uint8_t)data_8bit & SC7A20H_FIFO_DATA_FORMAT_MASK));
        res = sc7a20h_set_reg(handle, SC7A20H_FIFO_REG_CTRL3, &reg, 1U); /* set FIFO format */
    }
    if (res == 0U)
    {
        res = sc7a20h_get_reg(handle, SC7A20H_FIFO_REG_CTRL5, &reg, 1U); /* read FIFO enable */
    }
    if (res == 0U)
    {
        if (enable == SC7A20H_BOOL_TRUE)
        {
            reg |= SC7A20H_FIFO_ENABLE_MASK; /* enable FIFO */
        }
        else
        {
            reg &= (uint8_t)~SC7A20H_FIFO_ENABLE_MASK; /* disable FIFO */
        }
        res = sc7a20h_set_reg(handle, SC7A20H_FIFO_REG_CTRL5, &reg, 1U); /* write FIFO enable */
    }
    if (res != 0U)
    {
        handle->debug_print("sc7a20h: configure fifo failed.\n"); /* FIFO configuration failed */
        return 1; /* return error */
    }

    return 0; /* success return 0 */
}

/**
 * @brief      read fifo status
 * @param[in]  *handle pointer to a sc7a20h handle structure
 * @param[out] *status pointer to a fifo status structure
 * @return     status code
 *             - 0 success
 *             - 1 register read failed
 *             - 2 handle is invalid
 *             - 3 status pointer is invalid
 * @note       unread_groups is returned exactly as encoded in FSS[4:0]
 */
uint8_t sc7a20h_fifo_get_status(sc7a20h_handle_t *handle, sc7a20h_fifo_status_t *status)
{
    uint8_t data;
    uint8_t res;

    if (handle == NULL)
    {
        return 2; /* return error */
    }
    if (handle->inited != 1U)
    {
        return 3; /* return error */
    }
    if (status == NULL)
    {
        handle->debug_print("sc7a20h: fifo status pointer is null.\n"); /* null pointer */
        return 3; /* return error */
    }
    res = sc7a20h_get_reg(handle, SC7A20H_FIFO_REG_STATUS, &data, 1U); /* read FIFO status */
    if (res != 0U)
    {
        handle->debug_print("sc7a20h: read fifo status failed.\n"); /* status read failed */
        return 1; /* return error */
    }
    status->watermark = (uint8_t)((data >> 7) & 0x01U); /* get watermark flag */
    status->overrun = (uint8_t)((data >> 6) & 0x01U); /* get overrun flag */
    status->empty = (uint8_t)((data >> 5) & 0x01U); /* get empty flag */
    status->unread_groups = (uint8_t)(data & 0x1FU); /* get raw unread group count */

    return 0; /* success return 0 */
}

/**
 * @brief      read raw bytes from fifo data port
 * @param[in]  *handle pointer to a sc7a20h handle structure
 * @param[out] *buf pointer to a data buffer
 * @param[in]  len number of FIFO data-port reads
 * @return     status code
 *             - 0 success
 *             - 1 register read failed
 *             - 2 handle is invalid
 *             - 3 buffer is invalid or empty
 *             - 4 SPI FIFO data address is unsupported
 * @note       each byte is read separately because FIFO_DATA is a data port
 */
uint8_t sc7a20h_fifo_read_bytes(sc7a20h_handle_t *handle, uint8_t *buf, uint16_t len)
{
    uint16_t i;
    uint8_t res;

    if (handle == NULL)
    {
        return 2; /* return error */
    }
    if (handle->inited != 1U)
    {
        return 3; /* return error */
    }
    if ((buf == NULL) || (len == 0U))
    {
        handle->debug_print("sc7a20h: fifo buffer is null or empty.\n"); /* invalid buffer */
        return 3; /* return error */
    }
    for (i = 0U; i < len; i++)
    {
        res = sc7a20h_get_reg(handle, SC7A20H_FIFO_REG_DATA, &buf[i], 1U); /* read next FIFO byte */
        if (res != 0U)
        {
            handle->debug_print("sc7a20h: read fifo data failed.\n"); /* FIFO data read failed */
            return res; /* return error */
        }
    }

    return 0; /* success return 0 */
}

/** @} */
