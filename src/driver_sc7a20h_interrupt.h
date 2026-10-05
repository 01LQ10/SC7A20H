/**
 * @file      driver_sc7a20h_interrupt.h
 * @brief     driver sc7a20h interrupt header file
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

#ifndef DRIVER_SC7A20H_INTERRUPT_H
#define DRIVER_SC7A20H_INTERRUPT_H

#include "driver_sc7a20h.h"

#ifdef __cplusplus
extern "C" {
#endif

#define SC7A20H_INT1_CLICK              0x80U /**< route click to INT1 */
#define SC7A20H_INT1_AOI1               0x40U /**< route AOI1 to INT1 */
#define SC7A20H_INT1_AOI2               0x20U /**< route AOI2 to INT1 */
#define SC7A20H_INT1_DATA_READY         0x10U /**< route data ready to INT1 */
#define SC7A20H_INT1_FIFO_WATERMARK     0x04U /**< route FIFO watermark to INT1 */
#define SC7A20H_INT1_FIFO_OVERRUN       0x02U /**< route FIFO overrun to INT1 */
#define SC7A20H_INT2_CLICK              0x80U /**< route click to INT2 */
#define SC7A20H_INT2_AOI1               0x40U /**< route AOI1 to INT2 */
#define SC7A20H_INT2_AOI2               0x20U /**< route AOI2 to INT2 */
#define SC7A20H_INT2_BOOT               0x10U /**< route boot status to INT2 */
#define SC7A20H_INT2_DATA_READY         0x08U /**< route data ready to INT2 */
#define SC7A20H_EVENT_DATA_READY        0x0001U /**< data ready event */
#define SC7A20H_EVENT_FIFO_WATERMARK    0x0002U /**< FIFO watermark event */
#define SC7A20H_EVENT_FIFO_OVERRUN      0x0003U /**< FIFO overrun event */
#define SC7A20H_EVENT_AOI1_PREFIX       0x0100U /**< AOI1 event prefix; low byte carries source */
#define SC7A20H_EVENT_AOI2_PREFIX       0x0200U /**< AOI2 event prefix; low byte carries source */
#define SC7A20H_EVENT_CLICK_PREFIX      0x0300U /**< click event prefix; low nibble carries click count */

/**
 * @addtogroup sc7a20h_extend_driver
 * @{
 */
/**
 * @brief      configure INT1 and INT2 routing
 * @param[in]  *handle pointer to a sc7a20h handle structure
 * @param[in]  int1_mask INT1 routing bit mask
 * @param[in]  int2_mask INT2 routing bit mask
 * @return     status code
 *             - 0 success
 *             - 1 register access failed
 *             - 2 handle is invalid
 *             - 3 handle is not initialized
 *             - 4 route mask is invalid
 * @note       bit positions match CTRL_REG3 and CTRL_REG6
 */
uint8_t sc7a20h_interrupt_set_route(sc7a20h_handle_t *handle, uint8_t int1_mask, uint8_t int2_mask);
/**
 * @brief      configure one AOI event generator
 * @param[in]  *handle pointer to a sc7a20h handle structure
 * @param[in]  aoi AOI selection, 1 or 2
 * @param[in]  config raw AOI configuration byte
 * @param[in]  threshold 7-bit threshold field
 * @param[in]  duration duration field
 * @return     status code
 *             - 0 success
 *             - 1 register access failed
 *             - 2 handle is invalid
 *             - 3 handle is not initialized
 *             - 4 AOI number or threshold is invalid
 * @note       config bit definitions are in AOI1_CFG and AOI2_CFG
 */
uint8_t sc7a20h_interrupt_configure_aoi(sc7a20h_handle_t *handle, uint8_t aoi,
                                        uint8_t config, uint8_t threshold, uint8_t duration);
/**
 * @brief      read AOI event source
 * @param[in]  *handle pointer to a sc7a20h handle structure
 * @param[in]  aoi AOI selection, 1 or 2
 * @param[out] *source pointer to the source status byte
 * @return     status code
 *             - 0 success
 *             - 1 register read failed
 *             - 2 handle is invalid
 *             - 3 handle is not initialized or source pointer is invalid
 *             - 4 AOI number is invalid
 * @note       reading the source register may clear latched status
 */
uint8_t sc7a20h_interrupt_get_aoi_source(sc7a20h_handle_t *handle, uint8_t aoi, uint8_t *source);
/**
 * @brief      configure click detection registers
 * @param[in]  *handle pointer to a sc7a20h handle structure
 * @param[in]  control click control register value
 * @param[in]  *coefficients pointer to four coefficient bytes
 * @return     status code
 *             - 0 success
 *             - 1 register access failed
 *             - 2 handle is invalid
 *             - 3 handle is not initialized or coefficients pointer is invalid
 *             - 4 reserved control bits are set
 * @note       coefficient 3 upper three bits must be zero
 */
uint8_t sc7a20h_interrupt_configure_click(sc7a20h_handle_t *handle, uint8_t control,
                                          const uint8_t coefficients[4]);
/**
 * @brief      read click detection source
 * @param[in]  *handle pointer to a sc7a20h handle structure
 * @param[out] *source pointer to the click source byte
 * @return     status code
 *             - 0 success
 *             - 1 register read failed
 *             - 2 handle is invalid
 *             - 3 handle is not initialized or source pointer is invalid
 * @note       reading CLICK_SRC clears a latched click interrupt
 */
uint8_t sc7a20h_interrupt_get_click_source(sc7a20h_handle_t *handle, uint8_t *source);
/**
 * @brief      configure interrupt latching on both pins
 * @param[in]  *handle pointer to a sc7a20h handle structure
 * @param[in]  int1 enable INT1 latching
 * @param[in]  int2 enable INT2 latching
 * @return     status code
 *             - 0 success
 *             - 1 register access failed
 *             - 2 handle is invalid
 *             - 3 handle is not initialized
 *             - 4 boolean value is invalid
 * @note       AOI latches clear when the corresponding source register is read
 */
uint8_t sc7a20h_interrupt_set_latching(sc7a20h_handle_t *handle, sc7a20h_bool_t int1, sc7a20h_bool_t int2);
/**
 * @brief      configure interrupt output electrical mode
 * @param[in]  *handle pointer to a sc7a20h handle structure
 * @param[in]  active_low active-low output selection
 * @param[in]  open_drain select open-drain output
 * @return     status code
 *             - 0 success
 *             - 1 register access failed
 *             - 2 handle is invalid
 *             - 3 handle is not initialized
 *             - 4 boolean value is invalid
 * @note       open-drain outputs require external pull-ups
 */
uint8_t sc7a20h_interrupt_set_output_mode(sc7a20h_handle_t *handle,
                                          sc7a20h_bool_t active_low, sc7a20h_bool_t open_drain);
/**
 * @brief      enable or disable AOI interrupt generation
 * @param[in]  *handle pointer to a sc7a20h handle structure
 * @param[in]  enable enable AOI interrupt generation
 * @return     status code
 *             - 0 success
 *             - 1 register access failed
 *             - 2 handle is invalid
 *             - 3 handle is not initialized
 *             - 4 boolean value is invalid
 * @note       CTRL_REG5 AOI_EN is active low
 */
uint8_t sc7a20h_interrupt_set_aoi_enable(sc7a20h_handle_t *handle, sc7a20h_bool_t enable);
/**
 * @brief      read and dispatch interrupt sources
 * @param[in]  *handle pointer to a sc7a20h handle structure
 * @return     status code
 *             - 0 success
 *             - 1 register read failed
 *             - 2 handle is invalid
 *             - 3 handle is not initialized
 * @note       call from a deferred context when bus access is not ISR safe
 */
uint8_t sc7a20h_interrupt_irq_handler(sc7a20h_handle_t *handle);
/** @} */

#ifdef __cplusplus
}
#endif

#endif
