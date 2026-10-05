# SC7A20H driver

Platform-independent C driver for the Silan SC7A20H three-axis accelerometer. The driver uses the IIC 7-bit address selected by SDO (`0x18` or `0x19`) or SPI and keeps hardware-specific operations behind injected interface functions.

## Features

- Device identification and software reset.
- IIC register access, including the documented sub-address auto-increment bit.
- SPI register access for documented addresses in `0x00`–`0x3F`.
- Output data rate, power mode, axis enable, and full-scale configuration.
- Oversampling and high-/low-pass filter configuration, block data update, and output byte order.
- Signed 12-bit acceleration samples returned as raw counts, `g`, and `m/s²`; both documented output byte orders are handled.
- FIFO mode/threshold/status configuration and raw FIFO data-port reads.
- INT1/INT2 route and electrical-mode configuration; AOI threshold/duration, click detector, interrupt latching, status dispatch, and event callback.
- Self-test mode selection.
- Internal digital pull-up control.
- Basic, advanced FIFO, interrupt, and hardware-test entry points.

## Bus support limits

The original datasheet documents the SPI address bit AD6 as being selected by `ADR_SPI_AD6` in register `0x0E`, but does not specify the CS/session behavior necessary to access the upper bank. The register table also marks `SPI_CTRL` read-only. Therefore this implementation **does not assume an undocumented bank-switch sequence**: SPI reads or writes above `0x3F` return error `4`. This excludes SPI access to `OUT_*_New` (`0x61`–`0x66`), `SOFT_RESET` (`0x68`), `FIFO_DATA` (`0x69`), `I2C_CTRL` (`0x6F`), and `VERSION` (`0x70`). IIC supports these documented addresses.

The datasheet describes FIFO_SRC `FSS[4:0]` as unread groups, but its FIFO_DATA byte-count note and the 12-bit/8-bit format descriptions do not define one unambiguous group-to-byte conversion. The FIFO API reports the raw FSS field and reads exactly the number of data-port bytes requested by its caller; it does not infer sample count or unpack FIFO samples.

Register `0x57` pull-up controls and the optional IIC shutdown register `0x6F` are exposed through documented register access. Disabling IIC from the active IIC connection can make further access impossible; only change that register when the hardware transition has been planned.

## Integrating a platform

1. Add `src/*.c` to the target and include `src/` and `interface/`.
2. Implement the functions declared in `interface/driver_sc7a20h_interface.h`. The `_template.c` functions are buildable stubs, not hardware implementations.
3. Initialize a handle with `DRIVER_SC7A20H_LINK_INIT`, bind the interface functions, call `sc7a20h_set_interface`, select the IIC address with `sc7a20h_set_addr_pin` when using IIC, and call `sc7a20h_init`.
4. Configure the sensor with the public setters or the documented writable-register API. `sc7a20h_start_continuous_read` enables all axes and selects normal or high-performance mode as required by the requested ODR.
5. Call `sc7a20h_read` to receive one sample. Return code `4` means a new sample is not ready yet.
6. Stop acquisition and call `sc7a20h_deinit` when finished.

The SPI interface implementation must frame each transaction according to the SC7A20H SPI protocol: RW in bit 0, multi-byte auto-increment in bit 1, register address in bits 2–7, followed by data. For IIC burst operations, the driver sets bit 7 of the sub-address and passes the resulting sub-address to the platform interface.

## Datasheet

See [`doc/md-SC7A20HTR-数据手册.md`](doc/md-SC7A20HTR-数据手册.md) and the original [`doc/SC7A20HTR-数据手册.pdf`](doc/SC7A20HTR-数据手册.pdf).
