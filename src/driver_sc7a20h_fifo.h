/**
 * @file      driver_sc7a20h_fifo.h
 * @brief     driver sc7a20h fifo header file
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

#ifndef DRIVER_SC7A20H_FIFO_H
#define DRIVER_SC7A20H_FIFO_H

#include "driver_sc7a20h.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief sc7a20h fifo mode enumeration definition
 */
typedef enum
{
    SC7A20H_FIFO_BYPASS = 0x00, /**< bypass mode */
    SC7A20H_FIFO_FIFO = 0x01, /**< fifo mode */
    SC7A20H_FIFO_STREAM = 0x02, /**< stream mode */
    SC7A20H_FIFO_TRIGGER = 0x03 /**< trigger mode */
} sc7a20h_fifo_mode_t;

/**
 * @brief sc7a20h fifo status structure
 */
typedef struct sc7a20h_fifo_status_s
{
    uint8_t watermark; /**< watermark flag */
    uint8_t overrun; /**< overrun flag */
    uint8_t empty; /**< empty flag */
    uint8_t unread_groups; /**< raw unread group count field */
} sc7a20h_fifo_status_t;

/**
 * @addtogroup sc7a20h_extend_driver
 * @{
 */
/**
 * @brief      configure fifo mode, threshold and data format
 * @param[in]  *handle pointer to a sc7a20h handle structure
 * @param[in]  mode fifo operating mode
 * @param[in]  trigger_aoi2 select AOI2 trigger source
 * @param[in]  watermark raw 5-bit watermark field
 * @param[in]  enable fifo enable state
 * @param[in]  data_8bit fifo data format
 * @return     status code
 *             - 0 success
 *             - 1 register access failed
 *             - 2 handle is invalid
 *             - 3 handle is not initialized
 *             - 4 configuration is invalid
 * @note       none
 */
uint8_t sc7a20h_fifo_configure(sc7a20h_handle_t *handle, sc7a20h_fifo_mode_t mode,
                               sc7a20h_bool_t trigger_aoi2, uint8_t watermark,
                               sc7a20h_bool_t enable, sc7a20h_bool_t data_8bit);
/**
 * @brief      read fifo status
 * @param[in]  *handle pointer to a sc7a20h handle structure
 * @param[out] *status pointer to a fifo status structure
 * @return     status code
 *             - 0 success
 *             - 1 register read failed
 *             - 2 handle is invalid
 *             - 3 status pointer is invalid
 * @note       unread_groups contains the raw FSS[4:0] field
 */
uint8_t sc7a20h_fifo_get_status(sc7a20h_handle_t *handle, sc7a20h_fifo_status_t *status);
/**
 * @brief      read bytes from the fifo data port
 * @param[in]  *handle pointer to a sc7a20h handle structure
 * @param[out] *buf pointer to a FIFO data buffer
 * @param[in]  len number of data-port bytes to read
 * @return     status code
 *             - 0 success
 *             - 1 register read failed
 *             - 2 handle is invalid
 *             - 3 buffer is invalid or empty
 *             - 4 SPI high address is unsupported
 * @note       byte count is supplied by caller
 */
uint8_t sc7a20h_fifo_read_bytes(sc7a20h_handle_t *handle, uint8_t *buf, uint16_t len);
/** @} */

#ifdef __cplusplus
}
#endif

#endif
