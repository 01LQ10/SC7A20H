/**
 * @file      driver_sc7a20h_interrupt.c
 * @brief     driver sc7a20h interrupt source file
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

#include "driver_sc7a20h_interrupt.h"
#include "driver_sc7a20h_fifo.h"

#define SC7A20H_INT_REG_CTRL3            0x22U /**< INT1 routing register */
#define SC7A20H_INT_REG_CTRL6            0x25U /**< INT2 routing register */
#define SC7A20H_INT_REG_STATUS           0x27U /**< data ready status */
#define SC7A20H_INT_REG_FIFO_SRC         0x2FU /**< FIFO interrupt status */
#define SC7A20H_INT_REG_AOI1_SRC         0x31U /**< AOI1 source register */
#define SC7A20H_INT_REG_AOI2_SRC         0x35U /**< AOI2 source register */
#define SC7A20H_INT_REG_CLICK_SRC        0x39U /**< click source register */
#define SC7A20H_INT_REG_AOI1_CFG         0x30U /**< AOI1 configuration */
#define SC7A20H_INT_REG_AOI1_THS         0x32U /**< AOI1 threshold */
#define SC7A20H_INT_REG_AOI1_DURATION    0x33U /**< AOI1 duration */
#define SC7A20H_INT_REG_AOI2_CFG         0x34U /**< AOI2 configuration */
#define SC7A20H_INT_REG_AOI2_THS         0x36U /**< AOI2 threshold */
#define SC7A20H_INT_REG_AOI2_DURATION    0x37U /**< AOI2 duration */
#define SC7A20H_INT_REG_CLICK_CTRL       0x38U /**< click control */
#define SC7A20H_INT_REG_CLICK_COEFF1     0x3AU /**< click coefficient 1 */
#define SC7A20H_INT_REG_CLICK_COEFF2     0x3BU /**< click coefficient 2 */
#define SC7A20H_INT_REG_CLICK_COEFF3     0x3CU /**< click coefficient 3 */
#define SC7A20H_INT_REG_CLICK_COEFF4     0x3DU /**< click coefficient 4 */
#define SC7A20H_INT_REG_CTRL5            0x24U /**< interrupt latching control */
#define SC7A20H_INT_REG_CTRL6            0x25U /**< interrupt electrical control */
#define SC7A20H_INT1_MASK                0xF6U /**< valid INT1 route bits */
#define SC7A20H_INT2_MASK                0xF8U /**< valid INT2 route bits */
#define SC7A20H_INT_STATUS_READY         0x08U /**< data ready status bit */

/**
 * @addtogroup sc7a20h_extend_driver
 * @{
 */

/**
 * @brief      set interrupt routing
 * @param[in]  *handle pointer to a sc7a20h handle structure
 * @param[in]  int1_mask INT1 route mask
 * @param[in]  int2_mask INT2 route mask
 * @return     status code
 *             - 0 success
 *             - 1 register access failed
 *             - 2 handle is invalid
 *             - 3 handle is not initialized
 *             - 4 route mask is invalid
 * @note       masks use the register bit positions defined in this header
 */
uint8_t sc7a20h_interrupt_set_route(sc7a20h_handle_t *handle, uint8_t int1_mask, uint8_t int2_mask)
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
    if (((int1_mask & (uint8_t)~SC7A20H_INT1_MASK) != 0U) ||
        ((int2_mask & (uint8_t)~SC7A20H_INT2_MASK) != 0U))
    {
        return 4; /* return error */
    }
    res = sc7a20h_get_reg(handle, SC7A20H_INT_REG_CTRL3, &reg, 1U); /* read INT1 route */
    if (res == 0U)
    {
        reg = (uint8_t)((reg & (uint8_t)~SC7A20H_INT1_MASK) | int1_mask);
        res = sc7a20h_set_reg(handle, SC7A20H_INT_REG_CTRL3, &reg, 1U); /* write INT1 route */
    }
    if (res == 0U)
    {
        res = sc7a20h_get_reg(handle, SC7A20H_INT_REG_CTRL6, &reg, 1U); /* read INT2 route */
    }
    if (res == 0U)
    {
        reg = (uint8_t)((reg & (uint8_t)~SC7A20H_INT2_MASK) | int2_mask);
        res = sc7a20h_set_reg(handle, SC7A20H_INT_REG_CTRL6, &reg, 1U); /* write INT2 route */
    }
    if (res != 0U)
    {
        handle->debug_print("sc7a20h: set interrupt route failed.\n"); /* set route failed */
        return 1; /* return error */
    }

    return 0; /* success return 0 */
}

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
 * @note       none
 */
uint8_t sc7a20h_interrupt_configure_aoi(sc7a20h_handle_t *handle, uint8_t aoi,
                                        uint8_t config, uint8_t threshold, uint8_t duration)
{
    uint8_t cfg_reg;
    uint8_t ths_reg;
    uint8_t duration_reg;
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
    if (((aoi != 1U) && (aoi != 2U)) || (threshold > 0x7FU))
    {
        return 4; /* return error */
    }
    if (aoi == 1U)
    {
        cfg_reg = SC7A20H_INT_REG_AOI1_CFG;
        ths_reg = SC7A20H_INT_REG_AOI1_THS;
        duration_reg = SC7A20H_INT_REG_AOI1_DURATION;
    }
    else
    {
        cfg_reg = SC7A20H_INT_REG_AOI2_CFG;
        ths_reg = SC7A20H_INT_REG_AOI2_THS;
        duration_reg = SC7A20H_INT_REG_AOI2_DURATION;
    }
    res = sc7a20h_set_reg(handle, cfg_reg, &config, 1U); /* configure AOI logic */
    if (res == 0U)
    {
        data = (uint8_t)(threshold & 0x7FU);
        res = sc7a20h_set_reg(handle, ths_reg, &data, 1U); /* set threshold */
    }
    if (res == 0U)
    {
        res = sc7a20h_set_reg(handle, duration_reg, &duration, 1U); /* set duration */
    }
    if (res != 0U)
    {
        handle->debug_print("sc7a20h: configure AOI failed.\n"); /* AOI configuration failed */
        return 1; /* return error */
    }

    return 0; /* success return 0 */
}

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
uint8_t sc7a20h_interrupt_get_aoi_source(sc7a20h_handle_t *handle, uint8_t aoi, uint8_t *source)
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
    if (source == NULL)
    {
        handle->debug_print("sc7a20h: AOI source pointer is null.\n"); /* null pointer */
        return 3; /* return error */
    }
    if ((aoi != 1U) && (aoi != 2U))
    {
        return 4; /* return error */
    }
    reg = (aoi == 1U) ? SC7A20H_INT_REG_AOI1_SRC : SC7A20H_INT_REG_AOI2_SRC;
    res = sc7a20h_get_reg(handle, reg, source, 1U); /* read AOI source */
    if (res != 0U)
    {
        handle->debug_print("sc7a20h: read AOI source failed.\n"); /* read source failed */
        return 1; /* return error */
    }

    return 0; /* success return 0 */
}

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
 *             - 4 reserved bits are set
 * @note       coefficient 3 upper three bits must be zero
 */
uint8_t sc7a20h_interrupt_configure_click(sc7a20h_handle_t *handle, uint8_t control,
                                          const uint8_t coefficients[4])
{
    uint8_t res;
    uint8_t i;
    uint8_t reg;
    uint8_t value;
    static const uint8_t coefficient_reg[4] =
    {
        SC7A20H_INT_REG_CLICK_COEFF1,
        SC7A20H_INT_REG_CLICK_COEFF2,
        SC7A20H_INT_REG_CLICK_COEFF3,
        SC7A20H_INT_REG_CLICK_COEFF4
    };

    if (handle == NULL)
    {
        return 2; /* return error */
    }
    if (handle->inited != 1U)
    {
        return 3; /* return error */
    }
    if (coefficients == NULL)
    {
        handle->debug_print("sc7a20h: click coefficients pointer is null.\n"); /* null pointer */
        return 3; /* return error */
    }
    if (((control & 0xE0U) != 0U) || ((coefficients[2] & 0xE0U) != 0U))
    {
        return 4; /* return error */
    }
    res = sc7a20h_set_reg(handle, SC7A20H_INT_REG_CLICK_CTRL, &control, 1U); /* set click control */
    for (i = 0U; (i < 4U) && (res == 0U); i++)
    {
        reg = coefficient_reg[i];
        value = coefficients[i];
        res = sc7a20h_set_reg(handle, reg, &value, 1U); /* set click coefficient */
    }
    if (res != 0U)
    {
        handle->debug_print("sc7a20h: configure click detection failed.\n"); /* click config failed */
        return 1; /* return error */
    }

    return 0; /* success return 0 */
}

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
uint8_t sc7a20h_interrupt_get_click_source(sc7a20h_handle_t *handle, uint8_t *source)
{
    if (handle == NULL)
    {
        return 2; /* return error */
    }
    if (handle->inited != 1U)
    {
        return 3; /* return error */
    }
    if (source == NULL)
    {
        handle->debug_print("sc7a20h: click source pointer is null.\n"); /* null pointer */
        return 3; /* return error */
    }
    if (sc7a20h_get_reg(handle, SC7A20H_INT_REG_CLICK_SRC, source, 1U) != 0U)
    {
        handle->debug_print("sc7a20h: read click source failed.\n"); /* click read failed */
        return 1; /* return error */
    }

    return 0; /* success return 0 */
}

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
 * @note       none
 */
uint8_t sc7a20h_interrupt_set_latching(sc7a20h_handle_t *handle, sc7a20h_bool_t int1, sc7a20h_bool_t int2)
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
    if (((int1 != SC7A20H_BOOL_FALSE) && (int1 != SC7A20H_BOOL_TRUE)) ||
        ((int2 != SC7A20H_BOOL_FALSE) && (int2 != SC7A20H_BOOL_TRUE)))
    {
        return 4; /* return error */
    }
    res = sc7a20h_get_reg(handle, SC7A20H_INT_REG_CTRL5, &reg, 1U); /* read latching register */
    if (res == 0U)
    {
        reg = (uint8_t)((reg & 0xF5U) | ((uint8_t)int1 << 3) | ((uint8_t)int2 << 1));
        res = sc7a20h_set_reg(handle, SC7A20H_INT_REG_CTRL5, &reg, 1U); /* set latching */
    }
    if (res != 0U)
    {
        handle->debug_print("sc7a20h: set interrupt latching failed.\n"); /* latching failed */
        return 1; /* return error */
    }

    return 0; /* success return 0 */
}

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
                                          sc7a20h_bool_t active_low, sc7a20h_bool_t open_drain)
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
    if (((active_low != SC7A20H_BOOL_FALSE) && (active_low != SC7A20H_BOOL_TRUE)) ||
        ((open_drain != SC7A20H_BOOL_FALSE) && (open_drain != SC7A20H_BOOL_TRUE)))
    {
        return 4; /* return error */
    }
    res = sc7a20h_get_reg(handle, SC7A20H_INT_REG_CTRL6, &reg, 1U); /* read output control */
    if (res == 0U)
    {
        reg = (uint8_t)((reg & 0xFCU) | ((uint8_t)active_low << 1) | (uint8_t)open_drain);
        res = sc7a20h_set_reg(handle, SC7A20H_INT_REG_CTRL6, &reg, 1U); /* set output mode */
    }
    if (res != 0U)
    {
        handle->debug_print("sc7a20h: set interrupt output mode failed.\n"); /* output config failed */
        return 1; /* return error */
    }

    return 0; /* success return 0 */
}

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
uint8_t sc7a20h_interrupt_set_aoi_enable(sc7a20h_handle_t *handle, sc7a20h_bool_t enable)
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
    if ((enable != SC7A20H_BOOL_FALSE) && (enable != SC7A20H_BOOL_TRUE))
    {
        return 4; /* return error */
    }
    res = sc7a20h_get_reg(handle, SC7A20H_INT_REG_CTRL5, &reg, 1U); /* read AOI enable */
    if (res == 0U)
    {
        if (enable == SC7A20H_BOOL_TRUE)
        {
            reg &= 0xDFU; /* clear active-low disable bit */
        }
        else
        {
            reg |= 0x20U; /* set disable bit */
        }
        res = sc7a20h_set_reg(handle, SC7A20H_INT_REG_CTRL5, &reg, 1U); /* write AOI enable */
    }
    if (res != 0U)
    {
        handle->debug_print("sc7a20h: set AOI enable failed.\n"); /* AOI enable failed */
        return 1; /* return error */
    }

    return 0; /* success return 0 */
}

/**
 * @brief      handle sensor interrupt status
 * @param[in]  *handle pointer to a sc7a20h handle structure
 * @return     status code
 *             - 0 success
 *             - 1 register read failed
 *             - 2 handle is invalid
 *             - 3 handle is not initialized
 * @note       call from a deferred interrupt context if bus access is not ISR safe
 */
uint8_t sc7a20h_interrupt_irq_handler(sc7a20h_handle_t *handle)
{
    uint8_t status;
    uint8_t fifo;
    uint8_t aoi1;
    uint8_t aoi2;
    uint8_t click;
    uint8_t res;

    if (handle == NULL)
    {
        return 2; /* return error */
    }
    if (handle->inited != 1U)
    {
        return 3; /* return error */
    }
    res = sc7a20h_get_reg(handle, SC7A20H_INT_REG_STATUS, &status, 1U); /* read data status */
    if (res == 0U)
    {
        res = sc7a20h_get_reg(handle, SC7A20H_INT_REG_FIFO_SRC, &fifo, 1U); /* read FIFO status */
    }
    if (res == 0U)
    {
        res = sc7a20h_get_reg(handle, SC7A20H_INT_REG_AOI1_SRC, &aoi1, 1U); /* read AOI1 source */
    }
    if (res == 0U)
    {
        res = sc7a20h_get_reg(handle, SC7A20H_INT_REG_AOI2_SRC, &aoi2, 1U); /* read AOI2 source */
    }
    if (res == 0U)
    {
        res = sc7a20h_get_reg(handle, SC7A20H_INT_REG_CLICK_SRC, &click, 1U); /* read click source */
    }
    if (res != 0U)
    {
        handle->debug_print("sc7a20h: read interrupt status failed.\n"); /* interrupt read failed */
        return 1; /* return error */
    }
    if (handle->receive_callback != NULL)
    {
        if ((status & SC7A20H_INT_STATUS_READY) != 0U)
        {
            handle->receive_callback(SC7A20H_EVENT_DATA_READY); /* notify data ready */
        }
        if ((fifo & 0x80U) != 0U)
        {
            handle->receive_callback(SC7A20H_EVENT_FIFO_WATERMARK); /* notify FIFO watermark */
        }
        if ((fifo & 0x40U) != 0U)
        {
            handle->receive_callback(SC7A20H_EVENT_FIFO_OVERRUN); /* notify FIFO overrun */
        }
        if ((aoi1 & 0x80U) != 0U)
        {
            handle->receive_callback((uint16_t)(SC7A20H_EVENT_AOI1_PREFIX | aoi1)); /* notify AOI1 */
        }
        if ((aoi2 & 0x80U) != 0U)
        {
            handle->receive_callback((uint16_t)(SC7A20H_EVENT_AOI2_PREFIX | aoi2)); /* notify AOI2 */
        }
        if ((click & 0x0FU) != 0U)
        {
            handle->receive_callback((uint16_t)(SC7A20H_EVENT_CLICK_PREFIX | (click & 0x0FU))); /* notify click */
        }
    }

    return 0; /* success return 0 */
}

/** @} */
