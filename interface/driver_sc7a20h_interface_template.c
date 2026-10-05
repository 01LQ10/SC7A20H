/**
 * @file      driver_sc7a20h_interface_template.c
 * @brief     driver sc7a20h interface template source file
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

#include "driver_sc7a20h_interface.h"
#include <stdarg.h>

/**
 * @addtogroup sc7a20h_interface_driver
 * @{
 */

uint8_t sc7a20h_interface_iic_init(void)
{
    return 0; /* replace with platform IIC initialization */
}

uint8_t sc7a20h_interface_iic_deinit(void)
{
    return 0; /* replace with platform IIC deinitialization */
}

uint8_t sc7a20h_interface_iic_write(uint8_t addr, uint8_t reg, uint8_t *buf, uint16_t len)
{
    (void)addr;
    (void)reg;
    (void)buf;
    (void)len;
    return 0; /* replace with IIC write; preserve the sub-address auto-increment bit */
}

uint8_t sc7a20h_interface_iic_read(uint8_t addr, uint8_t reg, uint8_t *buf, uint16_t len)
{
    (void)addr;
    (void)reg;
    (void)buf;
    (void)len;
    return 0; /* replace with IIC read */
}

uint8_t sc7a20h_interface_spi_init(void)
{
    return 0; /* replace with platform SPI initialization */
}

uint8_t sc7a20h_interface_spi_deinit(void)
{
    return 0; /* replace with platform SPI deinitialization */
}

uint8_t sc7a20h_interface_spi_write(uint8_t reg, uint8_t *buf, uint16_t len)
{
    (void)reg;
    (void)buf;
    (void)len;
    return 0; /* replace with SPI frame write; MS=1 for multi-byte transfers */
}

uint8_t sc7a20h_interface_spi_read(uint8_t reg, uint8_t *buf, uint16_t len)
{
    (void)reg;
    (void)buf;
    (void)len;
    return 0; /* replace with SPI frame read; MS=1 for multi-byte transfers */
}

void sc7a20h_interface_delay_ms(uint32_t ms)
{
    (void)ms; /* replace with platform millisecond delay */
}

void sc7a20h_interface_debug_print(const char *const fmt, ...)
{
    (void)fmt; /* replace with platform formatted logging */
}

void sc7a20h_interface_receive_callback(uint16_t type)
{
    (void)type; /* replace with platform event callback */
}

/** @} */
