/**
 * @file      driver_sc7a20h.c
 * @brief     driver sc7a20h source file
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

#include "driver_sc7a20h.h"

#define SC7A20H_ID                         0x11U /**< expected device identity */
#define SC7A20H_RESET_VALUE                0xA5U /**< software reset command */
#define SC7A20H_STATUS_ZYXDA               0x08U /**< all axis data ready */
#define SC7A20H_CTRL0_HR                   0x01U /**< high performance mode bit */
#define SC7A20H_CTRL1_LPEN                 0x08U /**< low power mode bit */
#define SC7A20H_CTRL1_AXES                0x07U /**< all axes enable mask */
#define SC7A20H_CTRL1_ODR                 0xF0U /**< output data rate mask */
#define SC7A20H_CTRL4_SCALE               0x30U /**< full scale mask */
#define SC7A20H_CTRL4_BLE                 0x40U /**< byte order selection */
#define SC7A20H_CTRL4_BDU                 0x80U /**< block data update */
#define SC7A20H_GRAVITY_MPS2              9.80665f /**< standard gravity */

/**
 * @defgroup sc7a20h_base_driver sc7a20h base driver function
 * @brief    sc7a20h base driver modules
 * @{
 */

/**
 * @brief      check whether a register address is documented
 * @param[in]  reg register address
 * @return     status code
 *             - 1 supported address
 *             - 0 reserved or unsupported address
 * @note       none
 */
static uint8_t a_sc7a20h_register_valid(uint8_t reg)
{
    if (((reg >= 0x0EU) && (reg <= 0x0FU)) ||
        ((reg >= 0x1FU) && (reg <= 0x25U)) ||
        ((reg >= 0x27U) && (reg <= 0x3DU)) ||
        (reg == 0x57U) ||
        ((reg >= 0x61U) && (reg <= 0x66U)) ||
        (reg == SC7A20H_REG_SOFT_RESET) ||
        (reg == SC7A20H_REG_FIFO_DATA) ||
        (reg == SC7A20H_REG_I2C_CTRL) ||
        (reg == SC7A20H_REG_VERSION))
    {
        return 1; /* documented register */
    }
    return 0; /* reserved register */
}

/**
 * @brief      check whether a register address is writable
 * @param[in]  reg register address
 * @return     status code
 *             - 1 writable register
 *             - 0 read-only or unsupported register
 * @note       none
 */
static uint8_t a_sc7a20h_register_writable(uint8_t reg)
{
    if (((reg >= 0x1FU) && (reg <= 0x25U)) ||
        (reg == 0x2EU) ||
        ((reg >= 0x30U) && (reg <= 0x30U)) ||
        ((reg >= 0x32U) && (reg <= 0x34U)) ||
        ((reg >= 0x36U) && (reg <= 0x38U)) ||
        ((reg >= 0x3AU) && (reg <= 0x3DU)) ||
        (reg == 0x57U) ||
        (reg == SC7A20H_REG_SOFT_RESET) ||
        (reg == SC7A20H_REG_I2C_CTRL))
    {
        return 1; /* documented writable register */
    }

    return 0; /* read-only register */
}

/**
 * @brief      read registers without public argument checks
 * @param[in]  *handle pointer to a sc7a20h handle structure
 * @param[in]  reg register address
 * @param[out] *buf pointer to a data buffer
 * @param[in]  len length of the data buffer
 * @return     status code
 *             - 0 success
 *             - 1 bus read failed
 *             - 4 unsupported register or SPI address
 * @note       none
 */
static uint8_t a_sc7a20h_read(sc7a20h_handle_t *handle, uint8_t reg, uint8_t *buf, uint16_t len)
{
    uint16_t i;
    uint8_t addr;
    uint8_t ret;

    for (i = 0; i < len; i++)
    {
        if ((uint16_t)reg + i > 0x7FU)
        {
            handle->debug_print("sc7a20h: register address overflow.\n"); /* invalid register range */
            return 4; /* return error */
        }
        if (a_sc7a20h_register_valid((uint8_t)(reg + i)) == 0)
        {
            handle->debug_print("sc7a20h: register address is reserved.\n"); /* reserved register */
            return 4; /* return error */
        }
    }
    if ((handle->interface == SC7A20H_INTERFACE_SPI) && ((reg > 0x3FU) || ((uint16_t)reg + len - 1U > 0x3FU)))
    {
        handle->debug_print("sc7a20h: spi high address access is unsupported.\n"); /* unsupported SPI bank */
        return 4; /* return error */
    }

    addr = reg;
    if ((handle->interface == SC7A20H_INTERFACE_IIC) && (len > 1U))
    {
        addr = (uint8_t)(reg | 0x80U); /* enable IIC sub-address auto increment */
    }

    if (handle->interface == SC7A20H_INTERFACE_IIC)
    {
        ret = handle->iic_read(handle->iic_addr, addr, buf, len); /* iic read */
    }
    else
    {
        ret = handle->spi_read(addr, buf, len); /* spi read */
    }
    if (ret != 0U)
    {
        handle->debug_print("sc7a20h: read register failed.\n"); /* read register failed */
        return 1; /* return error */
    }

    return 0; /* success return 0 */
}

/**
 * @brief      write registers without public argument checks
 * @param[in]  *handle pointer to a sc7a20h handle structure
 * @param[in]  reg register address
 * @param[in]  *buf pointer to a data buffer
 * @param[in]  len length of the data buffer
 * @return     status code
 *             - 0 success
 *             - 1 bus write failed
 *             - 4 unsupported register
 * @note       none
 */
static uint8_t a_sc7a20h_write(sc7a20h_handle_t *handle, uint8_t reg, uint8_t *buf, uint16_t len)
{
    uint16_t i;
    uint8_t addr;
    uint8_t ret;

    for (i = 0; i < len; i++)
    {
        if (((uint16_t)reg + i > 0x7FU) ||
            (a_sc7a20h_register_valid((uint8_t)(reg + i)) == 0) ||
            (a_sc7a20h_register_writable((uint8_t)(reg + i)) == 0))
        {
            handle->debug_print("sc7a20h: register is read-only or reserved.\n"); /* invalid register write */
            return 4; /* return error */
        }
    }
    if ((handle->interface == SC7A20H_INTERFACE_SPI) && ((reg > 0x3FU) || ((uint16_t)reg + len - 1U > 0x3FU)))
    {
        handle->debug_print("sc7a20h: spi high address access is unsupported.\n"); /* unsupported SPI bank */
        return 4; /* return error */
    }

    addr = reg;
    if ((handle->interface == SC7A20H_INTERFACE_IIC) && (len > 1U))
    {
        addr = (uint8_t)(reg | 0x80U); /* enable IIC sub-address auto increment */
    }

    if (handle->interface == SC7A20H_INTERFACE_IIC)
    {
        ret = handle->iic_write(handle->iic_addr, addr, buf, len); /* iic write */
    }
    else
    {
        ret = handle->spi_write(addr, buf, len); /* spi write */
    }
    if (ret != 0U)
    {
        handle->debug_print("sc7a20h: write register failed.\n"); /* write register failed */
        return 1; /* return error */
    }

    return 0; /* success return 0 */
}

/**
 * @brief      read and update one register field
 * @param[in]  *handle pointer to a sc7a20h handle structure
 * @param[in]  reg register address
 * @param[in]  mask field mask
 * @param[in]  value field value
 * @return     status code
 *             - 0 success
 *             - 1 register read or write failed
 * @note       none
 */
static uint8_t a_sc7a20h_update_bits(sc7a20h_handle_t *handle, uint8_t reg, uint8_t mask, uint8_t value)
{
    uint8_t data;
    uint8_t res;

    res = a_sc7a20h_read(handle, reg, &data, 1U); /* read register */
    if (res != 0U)
    {
        return res; /* return error */
    }
    data = (uint8_t)((data & (uint8_t)(~mask)) | (value & mask)); /* update field */
    return a_sc7a20h_write(handle, reg, &data, 1U); /* write register */
}

/**
 * @brief      read device version
 * @param[in]  *handle pointer to a sc7a20h handle structure
 * @param[out] *version pointer to a version value
 * @return     status code
 *             - 0 success
 *             - 1 register read failed
 *             - 2 handle is invalid
 *             - 3 version pointer is invalid
 *             - 4 SPI high address is unsupported
 * @note       VERSION register 0x70 is available over IIC only
 */
uint8_t sc7a20h_get_version(sc7a20h_handle_t *handle, uint8_t *version)
{
    if (handle == NULL)
    {
        return 2; /* return error */
    }
    if (handle->inited != 1U)
    {
        return 3; /* return error */
    }
    if (version == NULL)
    {
        handle->debug_print("sc7a20h: version pointer is null.\n"); /* null pointer */
        return 3; /* return error */
    }

    return a_sc7a20h_read(handle, SC7A20H_REG_VERSION, version, 1U); /* read version */
}

/**
 * @brief      reset the chip
 * @param[in]  *handle pointer to a sc7a20h handle structure
 * @return     status code
 *             - 0 success
 *             - 1 reset or identity check failed
 *             - 2 handle is invalid
 *             - 3 handle is not initialized
 * @note       waits 1 ms before confirming device identity
 */
uint8_t sc7a20h_reset(sc7a20h_handle_t *handle)
{
    uint8_t reset = SC7A20H_RESET_VALUE;
    uint8_t id;
    uint8_t res;

    if (handle == NULL)
    {
        return 2; /* return error */
    }
    if (handle->inited != 1U)
    {
        return 3; /* return error */
    }
    res = a_sc7a20h_write(handle, SC7A20H_REG_SOFT_RESET, &reset, 1U); /* issue soft reset */
    if (res != 0U)
    {
        handle->debug_print("sc7a20h: soft reset failed.\n"); /* reset failed */
        return 1; /* return error */
    }
    handle->delay_ms(1U); /* wait for reset */
    res = a_sc7a20h_read(handle, SC7A20H_REG_WHO_AM_I, &id, 1U); /* confirm device identity */
    if ((res != 0U) || (id != SC7A20H_ID))
    {
        handle->debug_print("sc7a20h: reset identity check failed.\n"); /* identity check failed */
        return 1; /* return error */
    }
    handle->full_scale = SC7A20H_SCALE_2G; /* restore reset scale cache */

    return 0; /* success return 0 */
}

/**
 * @brief      get chip information
 * @param[out] *info pointer to a chip information structure
 * @return     status code
 *             - 0 success
 *             - 2 information pointer is invalid
 * @note       none
 */
uint8_t sc7a20h_info(sc7a20h_info_t *info)
{
    if (info == NULL)
    {
        return 2; /* return error */
    }

    (void)memset(info, 0, sizeof(sc7a20h_info_t)); /* clear information */
    (void)memcpy(info->chip_name, "SC7A20H", sizeof("SC7A20H")); /* set chip name */
    (void)memcpy(info->manufacturer_name, "Silan Microelectronics", sizeof("Silan Microelectronics")); /* set manufacturer */
    (void)memcpy(info->interface, "IIC/SPI", sizeof("IIC/SPI")); /* set supported interfaces */
    info->supply_voltage_min_v = 1.71f; /* set minimum voltage */
    info->supply_voltage_max_v = 3.60f; /* set maximum voltage */
    info->max_current_ma = 0.194f; /* use documented typical maximum mode current */
    info->temperature_min = -40.0f; /* set minimum temperature */
    info->temperature_max = 85.0f; /* set maximum temperature */
    info->driver_version = 1000U; /* set driver version */

    return 0; /* success return 0 */
}

/**
 * @brief      set IIC address pin state
 * @param[in]  *handle pointer to a sc7a20h handle structure
 * @param[in]  addr IIC 7-bit address
 * @return     status code
 *             - 0 success
 *             - 2 handle is invalid
 *             - 3 handle is initialized
 *             - 4 address is invalid
 * @note       none
 */
uint8_t sc7a20h_set_addr_pin(sc7a20h_handle_t *handle, sc7a20h_address_t addr)
{
    if (handle == NULL)
    {
        return 2; /* return error */
    }
    if (handle->inited != 0U)
    {
        return 3; /* return error */
    }
    if ((addr != SC7A20H_ADDRESS_LOW) && (addr != SC7A20H_ADDRESS_HIGH))
    {
        return 4; /* return error */
    }
    handle->iic_addr = (uint8_t)addr; /* set address */

    return 0; /* success return 0 */
}

/**
 * @brief      get IIC address pin state
 * @param[in]  *handle pointer to a sc7a20h handle structure
 * @param[out] *addr pointer to an IIC address
 * @return     status code
 *             - 0 success
 *             - 2 handle is invalid
 *             - 3 address pointer is invalid
 * @note       none
 */
uint8_t sc7a20h_get_addr_pin(sc7a20h_handle_t *handle, sc7a20h_address_t *addr)
{
    if (handle == NULL)
    {
        return 2; /* return error */
    }
    if (addr == NULL)
    {
        return 3; /* return error */
    }
    *addr = (sc7a20h_address_t)handle->iic_addr; /* get address */

    return 0; /* success return 0 */
}

/**
 * @brief      select bus interface
 * @param[in]  *handle pointer to a sc7a20h handle structure
 * @param[in]  interface selected bus interface
 * @return     status code
 *             - 0 success
 *             - 2 handle is invalid
 *             - 3 handle is initialized
 *             - 4 interface is invalid
 * @note       SPI access is limited to the documented 0x00 through 0x3F address bank
 */
uint8_t sc7a20h_set_interface(sc7a20h_handle_t *handle, sc7a20h_interface_t interface)
{
    if (handle == NULL)
    {
        return 2; /* return error */
    }
    if (handle->inited != 0U)
    {
        return 3; /* return error */
    }
    if ((interface != SC7A20H_INTERFACE_IIC) && (interface != SC7A20H_INTERFACE_SPI))
    {
        return 4; /* return error */
    }
    handle->interface = (uint8_t)interface; /* set interface */

    return 0; /* success return 0 */
}

/**
 * @brief      initialize the chip
 * @param[in]  *handle pointer to a sc7a20h handle structure
 * @return     status code
 *             - 0 success
 *             - 1 bus initialization or device read failed
 *             - 2 handle is invalid
 *             - 3 interface function pointer is null
 *             - 4 chip identification is invalid
 * @note       verifies WHO_AM_I at register 0x0F
 */
uint8_t sc7a20h_init(sc7a20h_handle_t *handle)
{
    uint8_t id;
    uint8_t ret;
    uint8_t (*bus_init)(void);
    uint8_t (*bus_deinit)(void);

    if (handle == NULL)
    {
        return 2; /* return error */
    }
    if (handle->inited != 0U)
    {
        return 3; /* return error */
    }
    if ((handle->delay_ms == NULL) || (handle->debug_print == NULL))
    {
        if (handle->debug_print != NULL)
        {
            handle->debug_print("sc7a20h: common interface function is null.\n"); /* null function */
        }
        return 3; /* return error */
    }
    if ((handle->interface == SC7A20H_INTERFACE_IIC) &&
        ((handle->iic_addr != (uint8_t)SC7A20H_ADDRESS_LOW) &&
         (handle->iic_addr != (uint8_t)SC7A20H_ADDRESS_HIGH)))
    {
        handle->debug_print("sc7a20h: iic address is invalid.\n"); /* invalid address */
        return 4; /* return error */
    }
    if (handle->interface == SC7A20H_INTERFACE_IIC)
    {
        if ((handle->iic_init == NULL) || (handle->iic_deinit == NULL) ||
            (handle->iic_read == NULL) || (handle->iic_write == NULL))
        {
            handle->debug_print("sc7a20h: iic interface function is null.\n"); /* null function */
            return 3; /* return error */
        }
        bus_init = handle->iic_init;
        bus_deinit = handle->iic_deinit;
    }
    else if (handle->interface == SC7A20H_INTERFACE_SPI)
    {
        if ((handle->spi_init == NULL) || (handle->spi_deinit == NULL) ||
            (handle->spi_read == NULL) || (handle->spi_write == NULL))
        {
            handle->debug_print("sc7a20h: spi interface function is null.\n"); /* null function */
            return 3; /* return error */
        }
        bus_init = handle->spi_init;
        bus_deinit = handle->spi_deinit;
    }
    else
    {
        handle->debug_print("sc7a20h: interface is invalid.\n"); /* invalid interface */
        return 3; /* return error */
    }

    ret = bus_init(); /* initialize bus */
    if (ret != 0U)
    {
        handle->debug_print("sc7a20h: bus init failed.\n"); /* bus init failed */
        return 1; /* return error */
    }
    handle->delay_ms(1U); /* wait for interface startup */
    ret = a_sc7a20h_read(handle, SC7A20H_REG_WHO_AM_I, &id, 1U); /* read device identity */
    if (ret != 0U)
    {
        handle->debug_print("sc7a20h: read device id failed.\n"); /* identity read failed */
        (void)bus_deinit(); /* rollback bus */
        return 1; /* return error */
    }
    if (id != SC7A20H_ID)
    {
        handle->debug_print("sc7a20h: device id is invalid.\n"); /* invalid identity */
        (void)bus_deinit(); /* rollback bus */
        return 4; /* return error */
    }
    handle->full_scale = SC7A20H_SCALE_2G; /* cache power-on scale */
    handle->inited = 1U; /* flag initialized */

    return 0; /* success return 0 */
}

/**
 * @brief      deinitialize the chip
 * @param[in]  *handle pointer to a sc7a20h handle structure
 * @return     status code
 *             - 0 success
 *             - 1 bus deinitialization failed
 *             - 2 handle is invalid
 *             - 3 handle is not initialized
 * @note       always clears the initialized state
 */
uint8_t sc7a20h_deinit(sc7a20h_handle_t *handle)
{
    uint8_t ret;

    if (handle == NULL)
    {
        return 2; /* return error */
    }
    if (handle->inited != 1U)
    {
        return 3; /* return error */
    }
    if (handle->interface == SC7A20H_INTERFACE_IIC)
    {
        ret = handle->iic_deinit(); /* deinitialize IIC */
    }
    else
    {
        ret = handle->spi_deinit(); /* deinitialize SPI */
    }
    handle->inited = 0U; /* clear initialized state */
    if (ret != 0U)
    {
        handle->debug_print("sc7a20h: bus deinit failed.\n"); /* bus deinit failed */
        return 1; /* return error */
    }

    return 0; /* success return 0 */
}

/**
 * @brief      read registers
 * @param[in]  *handle pointer to a sc7a20h handle structure
 * @param[in]  reg register address
 * @param[out] *buf pointer to a data buffer
 * @param[in]  len length of the data buffer
 * @return     status code
 *             - 0 success
 *             - 1 bus read failed
 *             - 2 handle is invalid
 *             - 3 handle is not initialized
 *             - 4 register range or SPI address is unsupported
 * @note       none
 */
uint8_t sc7a20h_get_reg(sc7a20h_handle_t *handle, uint8_t reg, uint8_t *buf, uint16_t len)
{
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
        handle->debug_print("sc7a20h: read buffer is null or empty.\n"); /* invalid buffer */
        return 4; /* return error */
    }

    return a_sc7a20h_read(handle, reg, buf, len); /* read registers */
}

/**
 * @brief      write registers
 * @param[in]  *handle pointer to a sc7a20h handle structure
 * @param[in]  reg register address
 * @param[in]  *buf pointer to a data buffer
 * @param[in]  len length of the data buffer
 * @return     status code
 *             - 0 success
 *             - 1 bus write failed
 *             - 2 handle is invalid
 *             - 3 handle is not initialized
 *             - 4 register is read-only, reserved, or unsupported
 * @note       none
 */
uint8_t sc7a20h_set_reg(sc7a20h_handle_t *handle, uint8_t reg, uint8_t *buf, uint16_t len)
{
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
        handle->debug_print("sc7a20h: write buffer is null or empty.\n"); /* invalid buffer */
        return 4; /* return error */
    }

    return a_sc7a20h_write(handle, reg, buf, len); /* write registers */
}

/**
 * @brief      read device identity
 * @param[in]  *handle pointer to a sc7a20h handle structure
 * @param[out] *id pointer to an identity value
 * @return     status code
 *             - 0 success
 *             - 1 register read failed
 *             - 2 handle is invalid
 *             - 3 device pointer is invalid
 *             - 4 unexpected identity
 * @note       expected identity is 0x11
 */
uint8_t sc7a20h_get_device_id(sc7a20h_handle_t *handle, uint8_t *id)
{
    uint8_t res;

    if (handle == NULL)
    {
        return 2; /* return error */
    }
    if (handle->inited != 1U)
    {
        return 3; /* return error */
    }
    if (id == NULL)
    {
        handle->debug_print("sc7a20h: id pointer is null.\n"); /* null pointer */
        return 3; /* return error */
    }
    res = a_sc7a20h_read(handle, SC7A20H_REG_WHO_AM_I, id, 1U); /* read identity */
    if (res != 0U)
    {
        return res; /* return error */
    }
    if (*id != SC7A20H_ID)
    {
        handle->debug_print("sc7a20h: device id is invalid.\n"); /* invalid identity */
        return 4; /* return error */
    }

    return 0; /* success return 0 */
}

/**
 * @brief      read output data status
 * @param[in]  *handle pointer to a sc7a20h handle structure
 * @param[out] *status pointer to a status value
 * @return     status code
 *             - 0 success
 *             - 1 register read failed
 *             - 2 handle is invalid
 *             - 3 status pointer is invalid
 * @note       none
 */
uint8_t sc7a20h_get_status(sc7a20h_handle_t *handle, uint8_t *status)
{
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
        handle->debug_print("sc7a20h: status pointer is null.\n"); /* null pointer */
        return 3; /* return error */
    }

    return a_sc7a20h_read(handle, SC7A20H_REG_STATUS, status, 1U); /* read status */
}

/**
 * @brief      read acceleration data
 * @param[in]  *handle pointer to a sc7a20h handle structure
 * @param[out] *data pointer to acceleration data
 * @return     status code
 *             - 0 success
 *             - 1 register read failed
 *             - 2 handle is invalid
 *             - 3 data pointer is invalid
 *             - 4 new data is not ready
 * @note       converts signed 12-bit left-aligned output to g and m/s2
 */
uint8_t sc7a20h_read(sc7a20h_handle_t *handle, sc7a20h_data_t *data)
{
    uint8_t status;
    uint8_t raw[6];
    uint8_t ctrl4;
    uint8_t res;
    uint8_t scale;
    uint8_t axis;
    uint16_t bits;
    int16_t signed_bits;
    float sensitivity;
    static const float sensitivity_mg[4] = {1.0f, 2.0f, 4.0f, 8.0f};

    if (handle == NULL)
    {
        return 2; /* return error */
    }
    if (handle->inited != 1U)
    {
        return 3; /* return error */
    }
    if (data == NULL)
    {
        handle->debug_print("sc7a20h: data pointer is null.\n"); /* null pointer */
        return 3; /* return error */
    }
    res = a_sc7a20h_read(handle, SC7A20H_REG_STATUS, &status, 1U); /* check data status */
    if (res != 0U)
    {
        return res; /* return error */
    }
    if ((status & SC7A20H_STATUS_ZYXDA) == 0U)
    {
        return 4; /* return error */
    }
    res = a_sc7a20h_read(handle, SC7A20H_REG_CTRL_REG4, &ctrl4, 1U); /* read current range */
    if (res != 0U)
    {
        return res; /* return error */
    }
    res = a_sc7a20h_read(handle, SC7A20H_REG_OUT_X_L, raw, 6U); /* read three axes */
    if (res != 0U)
    {
        return res; /* return error */
    }
    scale = (uint8_t)((ctrl4 & SC7A20H_CTRL4_SCALE) >> 4);
    handle->full_scale = scale; /* update cached scale */
    sensitivity = sensitivity_mg[scale] / 1000.0f; /* convert counts to g */
    for (axis = 0U; axis < 3U; axis++)
    {
        if ((ctrl4 & SC7A20H_CTRL4_BLE) == 0U)
        {
            bits = (uint16_t)(((uint16_t)raw[(uint8_t)(axis * 2U + 1U)] << 8) |
                              raw[(uint8_t)(axis * 2U)]);
        }
        else
        {
            bits = (uint16_t)(((uint16_t)raw[(uint8_t)(axis * 2U)] << 8) |
                              raw[(uint8_t)(axis * 2U + 1U)]);
        }
        signed_bits = (int16_t)bits;
        data->raw[axis] = (int16_t)(signed_bits / 16); /* remove four unused low bits */
        data->acceleration_g[axis] = (float)data->raw[axis] * sensitivity; /* convert to g */
        data->acceleration_mps2[axis] = data->acceleration_g[axis] * SC7A20H_GRAVITY_MPS2; /* convert to m/s2 */
    }

    return 0; /* success return 0 */
}

/**
 * @brief      set output data rate
 * @param[in]  *handle pointer to a sc7a20h handle structure
 * @param[in]  odr output data rate
 * @return     status code
 *             - 0 success
 *             - 1 register read or write failed
 *             - 2 handle is invalid
 *             - 3 handle is not initialized
 *             - 4 output data rate is invalid
 * @note       none
 */
uint8_t sc7a20h_set_odr(sc7a20h_handle_t *handle, sc7a20h_odr_t odr)
{
    uint8_t res;
    uint8_t ctrl0;

    if (handle == NULL)
    {
        return 2; /* return error */
    }
    if (handle->inited != 1U)
    {
        return 3; /* return error */
    }
    if ((uint8_t)odr > (uint8_t)SC7A20H_ODR_4P434_KHZ)
    {
        return 4; /* return error */
    }
    if ((uint8_t)odr >= (uint8_t)SC7A20H_ODR_1P48_KHZ)
    {
        res = a_sc7a20h_read(handle, SC7A20H_REG_CTRL_REG0, &ctrl0, 1U); /* check high performance mode */
        if (res != 0U)
        {
            handle->debug_print("sc7a20h: check output mode failed.\n"); /* mode read failed */
            return 1; /* return error */
        }
        if ((ctrl0 & SC7A20H_CTRL0_HR) == 0U)
        {
            handle->debug_print("sc7a20h: high output rate requires high performance mode.\n"); /* invalid mode */
            return 4; /* return error */
        }
    }
    res = a_sc7a20h_update_bits(handle, SC7A20H_REG_CTRL_REG1, SC7A20H_CTRL1_ODR, (uint8_t)((uint8_t)odr << 4));
    if (res != 0U)
    {
        handle->debug_print("sc7a20h: set output data rate failed.\n"); /* set ODR failed */
        return 1; /* return error */
    }

    return 0; /* success return 0 */
}

/**
 * @brief      get output data rate
 * @param[in]  *handle pointer to a sc7a20h handle structure
 * @param[out] *odr pointer to an output data rate
 * @return     status code
 *             - 0 success
 *             - 1 register read failed
 *             - 2 handle is invalid
 *             - 3 output data rate pointer is invalid
 *             - 4 register value is invalid
 * @note       none
 */
uint8_t sc7a20h_get_odr(sc7a20h_handle_t *handle, sc7a20h_odr_t *odr)
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
    if (odr == NULL)
    {
        handle->debug_print("sc7a20h: output data rate pointer is null.\n"); /* null pointer */
        return 3; /* return error */
    }
    res = a_sc7a20h_read(handle, SC7A20H_REG_CTRL_REG1, &reg, 1U); /* read rate register */
    if (res != 0U)
    {
        return res; /* return error */
    }
    reg = (uint8_t)((reg & SC7A20H_CTRL1_ODR) >> 4);
    if (reg > (uint8_t)SC7A20H_ODR_4P434_KHZ)
    {
        return 4; /* return error */
    }
    *odr = (sc7a20h_odr_t)reg; /* return rate */

    return 0; /* success return 0 */
}

/**
 * @brief      set power mode
 * @param[in]  *handle pointer to a sc7a20h handle structure
 * @param[in]  mode power mode
 * @return     status code
 *             - 0 success
 *             - 1 register read or write failed
 *             - 2 handle is invalid
 *             - 3 handle is not initialized
 *             - 4 mode is invalid
 * @note       none
 */
uint8_t sc7a20h_set_mode(sc7a20h_handle_t *handle, sc7a20h_mode_t mode)
{
    uint8_t res;
    uint8_t ctrl0_value;
    uint8_t ctrl1_value;

    if (handle == NULL)
    {
        return 2; /* return error */
    }
    if (handle->inited != 1U)
    {
        return 3; /* return error */
    }
    if ((uint8_t)mode > (uint8_t)SC7A20H_MODE_ENHANCED)
    {
        return 4; /* return error */
    }
    ctrl0_value = ((mode == SC7A20H_MODE_HIGH_PERFORMANCE) || (mode == SC7A20H_MODE_ENHANCED)) ?
                  SC7A20H_CTRL0_HR : 0U;
    ctrl1_value = ((mode == SC7A20H_MODE_LOW_POWER) || (mode == SC7A20H_MODE_ENHANCED)) ?
                  SC7A20H_CTRL1_LPEN : 0U;
    res = a_sc7a20h_update_bits(handle, SC7A20H_REG_CTRL_REG0, SC7A20H_CTRL0_HR, ctrl0_value);
    if (res == 0U)
    {
        res = a_sc7a20h_update_bits(handle, SC7A20H_REG_CTRL_REG1, SC7A20H_CTRL1_LPEN, ctrl1_value);
    }
    if (res != 0U)
    {
        handle->debug_print("sc7a20h: set power mode failed.\n"); /* set mode failed */
        return 1; /* return error */
    }

    return 0; /* success return 0 */
}

/**
 * @brief      get power mode
 * @param[in]  *handle pointer to a sc7a20h handle structure
 * @param[out] *mode pointer to a power mode
 * @return     status code
 *             - 0 success
 *             - 1 register read failed
 *             - 2 handle is invalid
 *             - 3 mode pointer is invalid
 * @note       none
 */
uint8_t sc7a20h_get_mode(sc7a20h_handle_t *handle, sc7a20h_mode_t *mode)
{
    uint8_t ctrl0;
    uint8_t ctrl1;
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
        handle->debug_print("sc7a20h: mode pointer is null.\n"); /* null pointer */
        return 3; /* return error */
    }
    res = a_sc7a20h_read(handle, SC7A20H_REG_CTRL_REG0, &ctrl0, 1U); /* read mode register */
    if (res == 0U)
    {
        res = a_sc7a20h_read(handle, SC7A20H_REG_CTRL_REG1, &ctrl1, 1U); /* read mode register */
    }
    if (res != 0U)
    {
        return res; /* return error */
    }
    if (((ctrl0 & SC7A20H_CTRL0_HR) != 0U) && ((ctrl1 & SC7A20H_CTRL1_LPEN) != 0U))
    {
        *mode = SC7A20H_MODE_ENHANCED; /* set mode */
    }
    else if ((ctrl0 & SC7A20H_CTRL0_HR) != 0U)
    {
        *mode = SC7A20H_MODE_HIGH_PERFORMANCE; /* set mode */
    }
    else if ((ctrl1 & SC7A20H_CTRL1_LPEN) != 0U)
    {
        *mode = SC7A20H_MODE_LOW_POWER; /* set mode */
    }
    else
    {
        *mode = SC7A20H_MODE_NORMAL; /* set mode */
    }

    return 0; /* success return 0 */
}

/**
 * @brief      set full scale
 * @param[in]  *handle pointer to a sc7a20h handle structure
 * @param[in]  scale full scale
 * @return     status code
 *             - 0 success
 *             - 1 register read or write failed
 *             - 2 handle is invalid
 *             - 3 handle is not initialized
 *             - 4 full scale is invalid
 * @note       none
 */
uint8_t sc7a20h_set_scale(sc7a20h_handle_t *handle, sc7a20h_scale_t scale)
{
    uint8_t res;

    if (handle == NULL)
    {
        return 2; /* return error */
    }
    if (handle->inited != 1U)
    {
        return 3; /* return error */
    }
    if ((uint8_t)scale > (uint8_t)SC7A20H_SCALE_16G)
    {
        return 4; /* return error */
    }
    res = a_sc7a20h_update_bits(handle, SC7A20H_REG_CTRL_REG4, SC7A20H_CTRL4_SCALE, (uint8_t)((uint8_t)scale << 4));
    if (res != 0U)
    {
        handle->debug_print("sc7a20h: set full scale failed.\n"); /* set scale failed */
        return 1; /* return error */
    }
    handle->full_scale = (uint8_t)scale; /* update cached scale */

    return 0; /* success return 0 */
}

/**
 * @brief      get full scale
 * @param[in]  *handle pointer to a sc7a20h handle structure
 * @param[out] *scale pointer to a full scale
 * @return     status code
 *             - 0 success
 *             - 1 register read failed
 *             - 2 handle is invalid
 *             - 3 scale pointer is invalid
 * @note       none
 */
uint8_t sc7a20h_get_scale(sc7a20h_handle_t *handle, sc7a20h_scale_t *scale)
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
    if (scale == NULL)
    {
        handle->debug_print("sc7a20h: scale pointer is null.\n"); /* null pointer */
        return 3; /* return error */
    }
    res = a_sc7a20h_read(handle, SC7A20H_REG_CTRL_REG4, &reg, 1U); /* read scale register */
    if (res != 0U)
    {
        return res; /* return error */
    }
    handle->full_scale = (uint8_t)((reg & SC7A20H_CTRL4_SCALE) >> 4); /* cache scale */
    *scale = (sc7a20h_scale_t)handle->full_scale; /* return scale */

    return 0; /* success return 0 */
}

/**
 * @brief      enable or disable sensor axes
 * @param[in]  *handle pointer to a sc7a20h handle structure
 * @param[in]  axes axis enable bit mask, bit 0 X, bit 1 Y, bit 2 Z
 * @return     status code
 *             - 0 success
 *             - 1 register read or write failed
 *             - 2 handle is invalid
 *             - 3 handle is not initialized
 *             - 4 axis mask is invalid
 * @note       at least one axis must be enabled
 */
uint8_t sc7a20h_set_axes(sc7a20h_handle_t *handle, uint8_t axes)
{
    uint8_t res;

    if (handle == NULL)
    {
        return 2; /* return error */
    }
    if (handle->inited != 1U)
    {
        return 3; /* return error */
    }
    if ((axes & (uint8_t)(~SC7A20H_CTRL1_AXES)) != 0U)
    {
        return 4; /* return error */
    }
    res = a_sc7a20h_update_bits(handle, SC7A20H_REG_CTRL_REG1, SC7A20H_CTRL1_AXES, axes);
    if (res != 0U)
    {
        handle->debug_print("sc7a20h: set axes failed.\n"); /* set axes failed */
        return 1; /* return error */
    }

    return 0; /* success return 0 */
}

/**
 * @brief      get enabled sensor axes
 * @param[in]  *handle pointer to a sc7a20h handle structure
 * @param[out] *axes pointer to an axis enable bit mask
 * @return     status code
 *             - 0 success
 *             - 1 register read failed
 *             - 2 handle is invalid
 *             - 3 axis pointer is invalid
 * @note       bit 0 X, bit 1 Y, bit 2 Z
 */
uint8_t sc7a20h_get_axes(sc7a20h_handle_t *handle, uint8_t *axes)
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
    if (axes == NULL)
    {
        handle->debug_print("sc7a20h: axes pointer is null.\n"); /* null pointer */
        return 3; /* return error */
    }
    res = a_sc7a20h_read(handle, SC7A20H_REG_CTRL_REG1, &reg, 1U); /* read axes register */
    if (res != 0U)
    {
        return res; /* return error */
    }
    *axes = (uint8_t)(reg & SC7A20H_CTRL1_AXES); /* return axis mask */

    return 0; /* success return 0 */
}

/**
 * @brief      set oversampling ratio
 * @param[in]  *handle pointer to a sc7a20h handle structure
 * @param[in]  osr oversampling ratio
 * @return     status code
 *             - 0 success
 *             - 1 register access failed
 *             - 2 handle is invalid
 *             - 3 handle is not initialized
 *             - 4 ratio is invalid
 * @note       none
 */
uint8_t sc7a20h_set_osr(sc7a20h_handle_t *handle, sc7a20h_osr_t osr)
{
    uint8_t res;

    if (handle == NULL)
    {
        return 2; /* return error */
    }
    if (handle->inited != 1U)
    {
        return 3; /* return error */
    }
    if ((uint8_t)osr > (uint8_t)SC7A20H_OSR_32_C)
    {
        return 4; /* return error */
    }
    res = a_sc7a20h_update_bits(handle, SC7A20H_REG_CTRL_REG0, 0x70U, (uint8_t)((uint8_t)osr << 4));
    if (res != 0U)
    {
        handle->debug_print("sc7a20h: set oversampling ratio failed.\n"); /* set OSR failed */
        return 1; /* return error */
    }

    return 0; /* success return 0 */
}

/**
 * @brief      configure high-pass and low-pass filters
 * @param[in]  *handle pointer to a sc7a20h handle structure
 * @param[in]  *config pointer to a filter configuration structure
 * @return     status code
 *             - 0 success
 *             - 1 register access failed
 *             - 2 handle is invalid
 *             - 3 handle is not initialized or config pointer is invalid
 *             - 4 configuration field is invalid
 * @note       preserves unrelated control register fields
 */
uint8_t sc7a20h_set_filter(sc7a20h_handle_t *handle, const sc7a20h_filter_config_t *config)
{
    uint8_t res;
    uint8_t ctrl2_value;
    uint8_t ctrl2_mask = 0x7BU;

    if (handle == NULL)
    {
        return 2; /* return error */
    }
    if (handle->inited != 1U)
    {
        return 3; /* return error */
    }
    if (config == NULL)
    {
        handle->debug_print("sc7a20h: filter config pointer is null.\n"); /* null pointer */
        return 3; /* return error */
    }
    if ((config->high_pass_cutoff > 3U) || (config->low_pass_cutoff > 3U) ||
        (config->high_pass_data > 1U) || (config->high_pass_raw_source > 1U) ||
        (config->aoi1_high_pass > 1U) || (config->aoi2_high_pass > 1U))
    {
        return 4; /* return error */
    }
    ctrl2_value = (uint8_t)((config->high_pass_raw_source << 6) |
                            (config->high_pass_cutoff << 4) |
                            (config->high_pass_data << 3) |
                            (config->aoi2_high_pass << 1) |
                            config->aoi1_high_pass);
    res = a_sc7a20h_update_bits(handle, SC7A20H_REG_CTRL_REG2, ctrl2_mask, ctrl2_value);
    if (res == 0U)
    {
        res = a_sc7a20h_update_bits(handle, SC7A20H_REG_CTRL_REG0, 0x02U,
                                    (uint8_t)((config->low_pass_cutoff >> 1) << 1));
    }
    if (res == 0U)
    {
        res = a_sc7a20h_update_bits(handle, SC7A20H_REG_CTRL_REG4, 0x08U,
                                    (uint8_t)((config->low_pass_cutoff & 0x01U) << 3));
    }
    if (res != 0U)
    {
        handle->debug_print("sc7a20h: set filter configuration failed.\n"); /* set filters failed */
        return 1; /* return error */
    }

    return 0; /* success return 0 */
}

/**
 * @brief      get high-pass and low-pass filter configuration
 * @param[in]  *handle pointer to a sc7a20h handle structure
 * @param[out] *config pointer to a filter configuration structure
 * @return     status code
 *             - 0 success
 *             - 1 register read failed
 *             - 2 handle is invalid
 *             - 3 handle is not initialized or config pointer is invalid
 * @note       none
 */
uint8_t sc7a20h_get_filter(sc7a20h_handle_t *handle, sc7a20h_filter_config_t *config)
{
    uint8_t ctrl0;
    uint8_t ctrl2;
    uint8_t ctrl4;
    uint8_t res;

    if (handle == NULL)
    {
        return 2; /* return error */
    }
    if (handle->inited != 1U)
    {
        return 3; /* return error */
    }
    if (config == NULL)
    {
        handle->debug_print("sc7a20h: filter config pointer is null.\n"); /* null pointer */
        return 3; /* return error */
    }
    res = a_sc7a20h_read(handle, SC7A20H_REG_CTRL_REG0, &ctrl0, 1U); /* read filter control */
    if (res == 0U)
    {
        res = a_sc7a20h_read(handle, SC7A20H_REG_CTRL_REG2, &ctrl2, 1U); /* read filter control */
    }
    if (res == 0U)
    {
        res = a_sc7a20h_read(handle, SC7A20H_REG_CTRL_REG4, &ctrl4, 1U); /* read filter control */
    }
    if (res != 0U)
    {
        handle->debug_print("sc7a20h: get filter configuration failed.\n"); /* get filters failed */
        return 1; /* return error */
    }
    config->high_pass_cutoff = (uint8_t)((ctrl2 >> 4) & 0x03U); /* get high-pass cutoff */
    config->low_pass_cutoff = (uint8_t)((((ctrl0 >> 1) & 0x01U) << 1) | ((ctrl4 >> 3) & 0x01U)); /* get low-pass cutoff */
    config->high_pass_data = (uint8_t)((ctrl2 >> 3) & 0x01U); /* get high-pass output select */
    config->high_pass_raw_source = (uint8_t)((ctrl2 >> 6) & 0x01U); /* get high-pass source */
    config->aoi2_high_pass = (uint8_t)((ctrl2 >> 1) & 0x01U); /* get AOI2 filter state */
    config->aoi1_high_pass = (uint8_t)(ctrl2 & 0x01U); /* get AOI1 filter state */

    return 0; /* success return 0 */
}

/**
 * @brief      set block data update
 * @param[in]  *handle pointer to a sc7a20h handle structure
 * @param[in]  enable enable or disable block data update
 * @return     status code
 *             - 0 success
 *             - 1 register access failed
 *             - 2 handle is invalid
 *             - 3 handle is not initialized
 *             - 4 boolean value is invalid
 * @note       none
 */
uint8_t sc7a20h_set_block_data_update(sc7a20h_handle_t *handle, sc7a20h_bool_t enable)
{
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
    res = a_sc7a20h_update_bits(handle, SC7A20H_REG_CTRL_REG4, SC7A20H_CTRL4_BDU,
                                (uint8_t)((uint8_t)enable << 7));
    if (res != 0U)
    {
        handle->debug_print("sc7a20h: set block data update failed.\n"); /* set BDU failed */
        return 1; /* return error */
    }

    return 0; /* success return 0 */
}

/**
 * @brief      set output byte order
 * @param[in]  *handle pointer to a sc7a20h handle structure
 * @param[in]  big_endian select high byte at lower register address
 * @return     status code
 *             - 0 success
 *             - 1 register access failed
 *             - 2 handle is invalid
 *             - 3 handle is not initialized
 *             - 4 boolean value is invalid
 * @note       read function decodes either setting
 */
uint8_t sc7a20h_set_big_endian(sc7a20h_handle_t *handle, sc7a20h_bool_t big_endian)
{
    uint8_t res;

    if (handle == NULL)
    {
        return 2; /* return error */
    }
    if (handle->inited != 1U)
    {
        return 3; /* return error */
    }
    if ((big_endian != SC7A20H_BOOL_FALSE) && (big_endian != SC7A20H_BOOL_TRUE))
    {
        return 4; /* return error */
    }
    res = a_sc7a20h_update_bits(handle, SC7A20H_REG_CTRL_REG4, SC7A20H_CTRL4_BLE,
                                (uint8_t)((uint8_t)big_endian << 6));
    if (res != 0U)
    {
        handle->debug_print("sc7a20h: set output byte order failed.\n"); /* set byte order failed */
        return 1; /* return error */
    }

    return 0; /* success return 0 */
}

/**
 * @brief      configure internal digital pull-ups
 * @param[in]  *handle pointer to a sc7a20h handle structure
 * @param[in]  disable_sdo_pullup disable SDO internal pull-up
 * @param[in]  disable_iic_pullup disable SDA and SCL internal pull-ups
 * @return     status code
 *             - 0 success
 *             - 1 register access failed
 *             - 2 handle is invalid
 *             - 3 handle is not initialized
 *             - 4 boolean value is invalid
 * @note       disabled pull-ups may require external bias resistors
 */
uint8_t sc7a20h_set_internal_pullups(sc7a20h_handle_t *handle,
                                     sc7a20h_bool_t disable_sdo_pullup,
                                     sc7a20h_bool_t disable_iic_pullup)
{
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
    if (((disable_sdo_pullup != SC7A20H_BOOL_FALSE) && (disable_sdo_pullup != SC7A20H_BOOL_TRUE)) ||
        ((disable_iic_pullup != SC7A20H_BOOL_FALSE) && (disable_iic_pullup != SC7A20H_BOOL_TRUE)))
    {
        return 4; /* return error */
    }
    res = a_sc7a20h_read(handle, SC7A20H_REG_DIG_CTRL, &value, 1U); /* read pull-up control */
    if (res != 0U)
    {
        handle->debug_print("sc7a20h: read pull-up control failed.\n"); /* pull-up read failed */
        return 1; /* return error */
    }
    value = (uint8_t)((value & 0xF3U) | ((uint8_t)disable_sdo_pullup << 3) |
                      ((uint8_t)disable_iic_pullup << 2));
    res = a_sc7a20h_write(handle, SC7A20H_REG_DIG_CTRL, &value, 1U); /* set pull-up control */
    if (res != 0U)
    {
        handle->debug_print("sc7a20h: set pull-up control failed.\n"); /* pull-up write failed */
        return 1; /* return error */
    }

    return 0; /* success return 0 */
}

/**
 * @brief      start continuous data acquisition
 * @param[in]  *handle pointer to a sc7a20h handle structure
 * @param[in]  odr output data rate
 * @return     status code
 *             - 0 success
 *             - 1 register update failed
 *             - 2 handle is invalid
 *             - 3 handle is not initialized
 *             - 4 output data rate is invalid
 * @note       enables all axes and normal mode
 */
uint8_t sc7a20h_start_continuous_read(sc7a20h_handle_t *handle, sc7a20h_odr_t odr)
{
    uint8_t res;

    if (handle == NULL)
    {
        return 2; /* return error */
    }
    if (handle->inited != 1U)
    {
        return 3; /* return error */
    }
    if (((uint8_t)odr == 0U) || ((uint8_t)odr > (uint8_t)SC7A20H_ODR_4P434_KHZ))
    {
        return 4; /* return error */
    }
    res = sc7a20h_set_axes(handle, SC7A20H_CTRL1_AXES); /* enable axes */
    if (res == 0U)
    {
        if ((uint8_t)odr >= (uint8_t)SC7A20H_ODR_1P48_KHZ)
        {
            res = sc7a20h_set_mode(handle, SC7A20H_MODE_HIGH_PERFORMANCE); /* select high performance mode */
        }
        else
        {
            res = sc7a20h_set_mode(handle, SC7A20H_MODE_NORMAL); /* select normal mode */
        }
    }
    if (res == 0U)
    {
        res = sc7a20h_set_odr(handle, odr); /* start measurements */
    }
    if (res != 0U)
    {
        handle->debug_print("sc7a20h: start continuous read failed.\n"); /* start failed */
        return 1; /* return error */
    }

    return 0; /* success return 0 */
}

/**
 * @brief      stop continuous data acquisition
 * @param[in]  *handle pointer to a sc7a20h handle structure
 * @return     status code
 *             - 0 success
 *             - 1 register update failed
 *             - 2 handle is invalid
 *             - 3 handle is not initialized
 * @note       sets the output data rate to power down
 */
uint8_t sc7a20h_stop_continuous_read(sc7a20h_handle_t *handle)
{
    uint8_t res;

    if (handle == NULL)
    {
        return 2; /* return error */
    }
    if (handle->inited != 1U)
    {
        return 3; /* return error */
    }
    res = sc7a20h_set_odr(handle, SC7A20H_ODR_POWER_DOWN); /* stop measurements */
    if (res != 0U)
    {
        handle->debug_print("sc7a20h: stop continuous read failed.\n"); /* stop failed */
        return 1; /* return error */
    }

    return 0; /* success return 0 */
}

/** @} */
