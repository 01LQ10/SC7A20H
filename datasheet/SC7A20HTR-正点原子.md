原文链接：https://wiki.alientek.com/docs/Boards/IoT/DNESP32S3B3/example-idf/sc7a20h/
三轴传感器实验

前言

本章，我们将介绍一款高性价比三轴角速度传感器。本章我们将使用ESP32S3 来驱动 SC7A20H，读取其原始数据，并结合 LCD 显示，教大家如何使用这款三轴传感器。



SC7A20H简介

SC7A20HTR 是一款由士兰微电子推出的超低功耗、高精度三轴数字加速度计。它支持 ±2/±4/±8/±16g 多量程，提供 12-bit 分辨率和 1.56Hz 至 4.434kHz 的灵活输出速率。芯片工作电压宽（1.71V-3.6V），在低功耗模式下电流可低至 2µA，并具备掉电模式。它通过 I²C 或 SPI 接口通信，内置 32 级 FIFO、多种智能中断（如点击、自由落体、方向检测）以及自测试功能，采用紧凑的 2x2mm LGA-12 封装，非常适合空间和功耗敏感的便携式及物联网设备。



硬件设计

例程功能

在 LCD 显示屏上，我们能够看到 XYZ 的数据。当我们翻转开发板时，这些数据会根据开发板的翻转角度来计算出 pitch 俯仰角和 roll 翻滚角。



硬件资源

LED灯

正点原子2.4寸LCD屏幕

SC7A20H

原理图

SC7A20H器件相关原理图，如下图所示：



00

程序设计

SC7A20H 函数解析

由于SC7A20H使用了IIC进行驱动，那么关于IIC的API函数的介绍作者在前边的IIC\_EXIO实验章节中已经讲解过了，在此不再赘述。



SC7A20H 驱动解析

在 IDF 版 13\_sc7a20h 例程中，作者在 13\_sc7a20h\\components\\BSP 路径下新增了一个 SC7A20H 文件夹，分别用于存放 sc7a20h.c、 sc7a20h.h sc7a20h.h 文件负责声明温度传感器相关的函数和变量，而 sc7a20h.c 文件则实现了温度传感器的驱动代码。下面，我们将详细解析这两个文件的实现内容。



sc7a20h.h文件

/\* SC7A20HA相关宏定义 \*/



\#define SC7A20H\_ADDR            0x19                        /\* SDO引脚悬空/高电平时的地址，接地时为0x18 \*/

\#define SC7A20H\_ID              0x11                        /\* 设备ID \*/

\#define ONE\_G                   9.807f                      /\* 加速度单位转换使用 \*/

\#define M\_PI                    (3.14159265358979323846f)   /\* 陀螺仪单位转换使用 \*/

\#define MAX\_CALI\_COUNT          100                         /\* 采样次数 \*/



/\* 寄存器地址定义 \*/

\#define SC7A20H\_REG\_CTRL0           0x1F        /\* 控制寄存器0 \*/

\#define SC7A20H\_REG\_CTRL1           0x20        /\* 控制寄存器1 \*/

\#define SC7A20H\_REG\_CTRL2           0x21        /\* 控制寄存器2 \*/

\#define SC7A20H\_REG\_CTRL3           0x22        /\* 控制寄存器3 \*/

\#define SC7A20H\_REG\_CTRL4           0x23        /\* 控制寄存器4 \*/

\#define SC7A20H\_REG\_CTRL5           0x24        /\* 控制寄存器5 \*/

\#define SC7A20H\_REG\_CTRL6           0x25        /\* 控制寄存器6 \*/

\#define SC7A20H\_DRDY\_STATUS\_REG     0x27        /\* 状态寄存器 \*/

\#define SC7A20H\_REG\_OUT\_X\_L         0x28        /\* X轴低字节 \*/

\#define SC7A20H\_REG\_OUT\_X\_H         0x29        /\* X轴高字节 \*/

\#define SC7A20H\_REG\_OUT\_Y\_L         0x2A        /\* Y轴低字节 \*/

\#define SC7A20H\_REG\_OUT\_Y\_H         0x2B        /\* Y轴高字节 \*/

\#define SC7A20H\_REG\_OUT\_Z\_L         0x2C        /\* Z轴低字节 \*/

\#define SC7A20H\_REG\_OUT\_Z\_H         0x2D        /\* Z轴高字节 \*/

\#define SC7A20H\_REG\_WHO\_AM\_I        0x0F        /\* 设备ID寄存器 \*/



/\* 控制寄存器1 (0x20) 位定义 \*/

\#define SC7A20H\_ODR\_1\_56HZ          0x10        /\* 1.56Hz输出数据率 \*/

\#define SC7A20H\_ODR\_12\_5HZ          0x20        /\* 12.5Hz输出数据率 \*/

\#define SC7A20H\_ODR\_25HZ            0x30        /\* 25Hz输出数据率 \*/

\#define SC7A20H\_ODR\_50HZ            0x40        /\* 50Hz输出数据率 \*/

\#define SC7A20H\_ODR\_100HZ           0x50        /\* 100Hz输出数据率 \*/

\#define SC7A20H\_ODR\_200HZ           0x60        /\* 200Hz输出数据率 \*/

\#define SC7A20H\_ODR\_400HZ           0x70        /\* 400Hz输出数据率 \*/

\#define SC7A20H\_ODR\_1\_48KHZ         0x80        /\* 1.48kHz输出数据率 \*/

\#define SC7A20H\_ODR\_2\_66KHZ         0x90        /\* 2.66kHz输出数据率 \*/

\#define SC7A20H\_ODR\_4\_434KHZ        0xA0        /\* 4.434kHz输出数据率 \*/

\#define SC7A20H\_LPEN                0x08        /\* 低功耗模式使能 \*/

\#define SC7A20H\_ZEN                 0x04        /\* Z轴使能 \*/

\#define SC7A20H\_YEN                 0x02        /\* Y轴使能 \*/

\#define SC7A20H\_XEN                 0x01        /\* X轴使能 \*/

\#define SC7A20H\_ENABLE\_ALL\_AXES     (SC7A20H\_XEN | SC7A20H\_YEN | SC7A20H\_ZEN) // 使能所有轴



/\* 控制寄存器4配置 \*/

\#define SC7A20H\_SCALE\_2G            0x00        /\* ±2G量程 \*/

\#define SC7A20H\_SCALE\_4G            0x10        /\* ±4G量程 \*/

\#define SC7A20H\_SCALE\_8G            0x20        /\* ±8G量程 \*/

\#define SC7A20H\_SCALE\_16G           0x30        /\* ±16G量程 \*/

\#define SC7A20H\_BDU\_ENABLE          0x88        /\* 块数据更新使能 \*/



/\* 中断映射 \*/

\#define SC7A20H\_MAP\_INT1            0x01

\#define SC7A20H\_MAP\_INT2            0x02



typedef struct {

&#x20;   uint8\_t data\[2];

&#x20;   float  acc\_x;

&#x20;   float  acc\_y;

&#x20;   float  acc\_z;

&#x20;   float  acc\_g;

&#x20;   float  pitch;                       /\* 围绕X轴旋转,也叫做俯仰角 \*/

&#x20;   float  roll;                        /\* 围绕Z轴旋转,也叫翻滚角 \*/

} sc7a20h\_rawdata\_t;



/\* 函数声明 \*/

float sc7a20h\_get\_temperature(void);                                /\* 获取传感器温度 \*/

uint8\_t get\_euler\_angles(float \*pitch, float \*roll, float \*yaw);    /\* 获取欧拉角数据 \*/

void sc7a20h\_read\_xyz(float \*acc, float \*gyro);                     /\* 获取加速度计和陀螺仪的三轴数据 \*/

void sc7a20h\_read\_rawdata(sc7a20h\_rawdata\_t \*rawdata);              /\* 读取原始数据 \*/

esp\_err\_t sc7a20h\_init(void);





sc7a20h.c文件

/\* 全局变量缓存区 \*/

i2c\_master\_dev\_handle\_t sc7a20h\_handle = NULL;

const char\* sc7a20h\_name = "sc7a20h"; 

\#define M\_G                         9.80665f

\#define RAD\_TO\_DEG                  (180.0f / M\_PI)                         /\* 0.017453292519943295 \*/

\#define SC7A20H\_AUTO\_INCREMENT      0x80



/\*\*

&#x20;\* @brief       读取sc7a20h寄存器的数据

&#x20;\* @param       reg\_addr       : 要读取的寄存器地址

&#x20;\* @param       data           : 读取的数据

&#x20;\* @param       len           : 数据大小

&#x20;\* @retval      错误值        ：0成功，其他值：错误

&#x20;\*/

esp\_err\_t sc7a20h\_register\_read(const uint8\_t reg, uint8\_t \*data, const size\_t len)

{

&#x20;   uint8\_t reg\_addr = reg;



&#x20;   if (len > 1)

&#x20;   {

&#x20;       reg\_addr |= SC7A20H\_AUTO\_INCREMENT;

&#x20;   }



&#x20;   return i2c\_master\_transmit\_receive(sc7a20h\_handle, \&reg\_addr, 1, data, len, -1);

}



/\*\*

&#x20;\* @brief       向sc7a20h寄存器写数据

&#x20;\* @param       reg\_addr       : 要写入的寄存器地址

&#x20;\* @param       data           : 要写入的数据

&#x20;\* @retval      错误值        ：0成功，其他值：错误

&#x20;\*/

static esp\_err\_t sc7a20h\_register\_write\_byte(uint8\_t reg, uint8\_t data)

{

&#x20;   esp\_err\_t ret;



&#x20;   uint8\_t \*buf = malloc(2);

&#x20;   if (buf == NULL)

&#x20;   {

&#x20;       ESP\_LOGE(sc7a20h\_name, "%s memory failed", \_\_func\_\_);

&#x20;       return ESP\_ERR\_NO\_MEM;

&#x20;   }



&#x20;   buf\[0] = reg;

&#x20;   buf\[1] = data;



&#x20;   ret = i2c\_master\_transmit(sc7a20h\_handle, buf, 2, -1);



&#x20;   free(buf);



&#x20;   return ret;

}



/\*\*

&#x20;\* @brief 读取单轴12位加速度值（左对齐，带符号扩展）

&#x20;\* @param lsb\_addr: 低字节寄存器地址 (e.g., 0x28)

&#x20;\* @param msb\_addr: 高字节寄存器地址 (e.g., 0x29)

&#x20;\* @retval int16\_t: 原始12位有符号值（单位：LSB，±2048 对应 ±2G）

&#x20;\*/

static int16\_t sc7a20h\_read\_axis\_12bit(uint8\_t lsb\_addr, uint8\_t msb\_addr)

{

&#x20;   uint8\_t lsb, msb;

&#x20;   esp\_err\_t ret;



&#x20;   /\* 先读低字节 \*/

&#x20;   ret = sc7a20h\_register\_read(lsb\_addr, \&lsb, 1);

&#x20;   if (ret != ESP\_OK) 

&#x20;   {

&#x20;       return 0;

&#x20;   }



&#x20;   ret = sc7a20h\_register\_read(msb\_addr, \&msb, 1);



&#x20;   if (ret != ESP\_OK) return 0;



&#x20;   uint16\_t temp = ((uint16\_t)msb << 8) | lsb;



&#x20;   temp >>= 4;



&#x20;   if (msb \& 0x80) 

&#x20;   {

&#x20;       temp |= 0xF000;  /\* 补高4位为1（16位补码） \*/

&#x20;   } 

&#x20;   else 

&#x20;   {

&#x20;       temp \&= 0x0FFF;  /\* 清高4位（确保非负） \*/

&#x20;   }



&#x20;   return (int16\_t)temp;

}



uint8\_t xyz\_data\[6] = {0};

short raw\_data\[3] = {0};

float accl\_data\[3];

float acc\_normal;

float scale\_factor = 0.001f;  /\* 默认±2G量程，1mg/digit \*/



/\*\*

&#x20;\* @brief       读取三轴数据(原始数据、加速度、俯仰角和翻滚角)

&#x20;\* @param       rawdata：sc7a20h数据结构体

&#x20;\* @retval      无

&#x20;\*/

void sc7a20h\_read\_rawdata(sc7a20h\_rawdata\_t \*rawdata)

{

&#x20;   float sensor\_acc\_x;

&#x20;   float sensor\_acc\_y;

&#x20;   float sensor\_acc\_z;



&#x20;   if (sc7a20h\_register\_read(SC7A20H\_REG\_OUT\_X\_L, xyz\_data, 6) != ESP\_OK)

&#x20;   {

&#x20;       return;

&#x20;   }



&#x20;   /\* 组合高低字节，SC7A20H为12位数据，左对齐 \*/

&#x20;   raw\_data\[0] = (int16\_t)(((uint16\_t)xyz\_data\[1] << 8) | xyz\_data\[0]) >> 4;

&#x20;   raw\_data\[1] = (int16\_t)(((uint16\_t)xyz\_data\[3] << 8) | xyz\_data\[2]) >> 4;

&#x20;   raw\_data\[2] = (int16\_t)(((uint16\_t)xyz\_data\[5] << 8) | xyz\_data\[4]) >> 4;



&#x20;   sensor\_acc\_x = (float)raw\_data\[0] \* M\_G / 1024.0f;

&#x20;   sensor\_acc\_y = (float)raw\_data\[1] \* M\_G / 1024.0f;

&#x20;   sensor\_acc\_z = (float)raw\_data\[2] \* M\_G / 1024.0f;



&#x20;   rawdata->acc\_x = sensor\_acc\_y;

&#x20;   rawdata->acc\_y = -sensor\_acc\_x;

&#x20;   rawdata->acc\_z = -sensor\_acc\_z;



&#x20;   rawdata->acc\_g = sqrt(rawdata->acc\_x\*rawdata->acc\_x + rawdata->acc\_y \* rawdata->acc\_y + rawdata->acc\_z\*rawdata->acc\_z);



&#x20;   acc\_normal = sqrtf(rawdata->acc\_x \* rawdata->acc\_x + rawdata->acc\_y \* rawdata->acc\_y + rawdata->acc\_z \* rawdata->acc\_z);

&#x20;   if (acc\_normal == 0.0f)

&#x20;   {

&#x20;       rawdata->pitch = 0.0f;

&#x20;       rawdata->roll = 0.0f;

&#x20;       return;

&#x20;   }



&#x20;   accl\_data\[0] = rawdata->acc\_x / acc\_normal;

&#x20;   accl\_data\[1] = rawdata->acc\_y / acc\_normal;

&#x20;   accl\_data\[2] = rawdata->acc\_z / acc\_normal;



&#x20;   rawdata->pitch = atan2f(rawdata->acc\_y, rawdata->acc\_z) \* RAD\_TO\_DEG;

&#x20;   rawdata->roll = atan2f(rawdata->acc\_x, rawdata->acc\_z) \* RAD\_TO\_DEG;

}



/\*\*

&#x20;\* @brief       配置自由落体检测

&#x20;\* @param       threshold：阈值 (mg)

&#x20;\* @param       duration：持续时间 (ODR周期数)

&#x20;\* @retval      无

&#x20;\*/

void sc7a20h\_config\_freefall(uint8\_t threshold, uint8\_t duration)

{

&#x20;   /\* 配置自由落体阈值 (THS = threshold / 7.81mg) \*/

&#x20;   uint8\_t ths\_value = (uint8\_t)(threshold / 7.81f);

&#x20;   sc7a20h\_register\_write\_byte(0x32, ths\_value); /\* AOI1\_THS \*/



&#x20;   /\* 配置自由落体持续时间 \*/

&#x20;   sc7a20h\_register\_write\_byte(0x33, duration); /\* AOI1\_DURATION \*/



&#x20;   /\* 配置中断源为自由落体 \*/

&#x20;   sc7a20h\_register\_write\_byte(0x30, 0x90); /\* AOI1\_CFG (Z低和Y低检测) \*/

}



/\*\*

&#x20;\* @brief       初始化sc7a20h

&#x20;\* @param       无

&#x20;\* @retval      0, 成功;

&#x20;               1, 失败;

\*/

uint8\_t sc7a20h\_config(void)

{

&#x20;   uint8\_t id\_data = 0;



&#x20;   /\* 读取设备ID \*/

&#x20;   sc7a20h\_register\_read(SC7A20H\_REG\_WHO\_AM\_I, \&id\_data, 1);



&#x20;   /\* 检查设备ID \*/

&#x20;   if (id\_data != SC7A20H\_ID) 

&#x20;   {

&#x20;       ESP\_LOGE("sc7a20h", "Device ID mismatch: expected 0x%02X, got 0x%02X", SC7A20H\_ID, id\_data);

&#x20;       return 1;

&#x20;   }



&#x20;   /\* 配置控制寄存器1: 100Hz ODR, 使能三轴, 正常模式 \*/

&#x20;   uint8\_t ctrl\_reg1\_val = SC7A20H\_ODR\_100HZ | SC7A20H\_ENABLE\_ALL\_AXES;

&#x20;   sc7a20h\_register\_write\_byte(SC7A20H\_REG\_CTRL1, ctrl\_reg1\_val);



&#x20;   /\* 配置控制寄存器4: ±2G量程, 块数据更新使能 \*/

&#x20;   uint8\_t ctrl\_reg4\_val = SC7A20H\_SCALE\_2G | SC7A20H\_BDU\_ENABLE;

&#x20;   sc7a20h\_register\_write\_byte(SC7A20H\_REG\_CTRL4, ctrl\_reg4\_val);



&#x20;   /\* 配置为正常模式 \*/

&#x20;   sc7a20h\_register\_write\_byte(0x2E, 0x00); /\* FIFO\_CTRL\_REG (禁用FIFO) \*/

&#x20;   sc7a20h\_register\_write\_byte(0x24, 0x00); /\* CTRL\_REG5 (禁用高通滤波器) \*/



&#x20;   /\* 设置量程对应的缩放因子 \*/

&#x20;   scale\_factor = 0.001f; /\* ±2G量程，1mg/digit \*/



&#x20;   ESP\_LOGI("sc7a20h", "SC7A20H initialized successfully!");

&#x20;   return 0;

}



/\*\*

&#x20;\* @brief       sc7a20h初始化

&#x20;\* @param       无

&#x20;\* @retval      无

&#x20;\*/

esp\_err\_t sc7a20h\_init(void)

{

&#x20;   /\* 未调用myiic\_init初始化IIC \*/

&#x20;   if (bus\_handle == NULL)

&#x20;   {

&#x20;       ESP\_ERROR\_CHECK(myiic\_init());

&#x20;   }



&#x20;   i2c\_device\_config\_t sc7a20h\_i2c\_dev\_conf = {

&#x20;       .dev\_addr\_length = I2C\_ADDR\_BIT\_LEN\_7,  /\* 从机地址长度 \*/

&#x20;       .scl\_speed\_hz    = IIC\_SPEED\_CLK,       /\* 传输速率 \*/

&#x20;       .device\_address  = SC7A20H\_ADDR,        /\* 从机7位的地址 \*/

&#x20;   };

&#x20;   /\* I2C总线上添加sc7a20h设备 \*/

&#x20;   ESP\_ERROR\_CHECK(i2c\_master\_bus\_add\_device(bus\_handle, \&sc7a20h\_i2c\_dev\_conf, \&sc7a20h\_handle));



&#x20;   while (sc7a20h\_config())   /\* 检测不到sc7a20h \*/

&#x20;   {

&#x20;       ESP\_LOGE("sc7a20h", "sc7a20h init fail!!!");

&#x20;       vTaskDelay(500);

&#x20;   }



&#x20;   return 0;

}





程序通过IIC总线初始化设备，验证芯片ID后配置100Hz输出速率和±2g量程。核心功能包括读取12位原始加速度数据（左对齐格式），将其转换为标准重力单位（m/s²），并计算俯仰角和翻滚角。代码还实现了自由落体检测配置，通过设置阈值和持续时间参数触发中断。数据处理采用符号扩展确保12位有符号数正确解析，同时支持自动递增寄存器地址以提高读取效率。



CMakeLists.txt文件

打开本实验的BSP文件夹下的CMakeList.txt文件，其内容如下所示：



set(src\_dirs

&#x20;           MYIIC

&#x20;           LCD

&#x20;           MYSPI

&#x20;           AW9523B

&#x20;           SC7A20H)



set(include\_dirs

&#x20;           MYIIC

&#x20;           LCD

&#x20;           MYSPI

&#x20;           AW9523B

&#x20;           SC7A20H)



set(requires

&#x20;           driver

&#x20;           esp\_lcd)



idf\_component\_register(SRC\_DIRS ${src\_dirs} INCLUDE\_DIRS ${include\_dirs} REQUIRES ${requires})



component\_compile\_options(-ffast-math -O3 -Wno-error=format=-Wno-format)





上述代码中的 SC7A20H 驱动需要由开发者自行添加，以确保 SC7A20H 驱动能够顺利集成到构建系统中。这一步骤是必不可少的，它确保了 SC7A20H 驱动的正确性和可用性，为后续的开发工作提供了坚实的基础。



实验应用代码

打开main.c文件，该文件定义了工程入口函数，名为main。该函数代码如下。



/\*\*

&#x20;\* @brief       显示原始数据

&#x20;\* @param       x, y : 坐标

&#x20;\* @param       title: 标题

&#x20;\* @param       val  : 值

&#x20;\* @retval      无

&#x20;\*/

void user\_show\_mag(uint16\_t x, uint16\_t y, char \*title, float val)

{

&#x20;   char buf\[20];



&#x20;   sprintf(buf,"%s%3.1f", title, val);                 /\* 格式化输出 \*/

&#x20;   lcd\_fill(x + 30, y + 16, x + 160, y + 16, WHITE);   /\* 清除上次数据(最多显示20个字符,20\*8=160) \*/

&#x20;   lcd\_show\_string(x, y, 160, 16, 16, buf, BLUE);      /\* 显示字符串 \*/

}



/\*\*

&#x20;\* @brief       程序入口

&#x20;\* @param       无

&#x20;\* @retval      无

&#x20;\*/

void app\_main(void)

{

&#x20;   esp\_err\_t ret;

&#x20;   uint8\_t t = 0;

&#x20;   sc7a20h\_rawdata\_t xyz\_rawdata;

&#x20;   

&#x20;   ret = nvs\_flash\_init();             /\* 初始化NVS \*/

&#x20;   if (ret == ESP\_ERR\_NVS\_NO\_FREE\_PAGES || ret == ESP\_ERR\_NVS\_NEW\_VERSION\_FOUND)

&#x20;   {

&#x20;       ESP\_ERROR\_CHECK(nvs\_flash\_erase());

&#x20;       ESP\_ERROR\_CHECK(nvs\_flash\_init());

&#x20;   }



&#x20;   my\_spi\_init();                      /\* 初始化SPI \*/

&#x20;   myiic\_init();                       /\* 初始化IIC \*/

&#x20;   aw9523b\_init();                     /\* 初始化AW9523B \*/

&#x20;   lcd\_init();                         /\* 初始化LCD \*/

&#x20;   sc7a20h\_init();                     /\* 初始化SC7A20H \*/



&#x20;   lcd\_show\_string(30, 50, 200, 16, 16, "ESP32-S3", RED);

&#x20;   lcd\_show\_string(30, 70, 200, 16, 16, "SC7A20H TEST", RED);

&#x20;   lcd\_show\_string(30, 90, 200, 16, 16, "ATOM@ALIENTEK", RED);

&#x20;   

&#x20;   lcd\_show\_string(30, 110, 200, 16, 16, " ACC\_X :", RED);

&#x20;   lcd\_show\_string(30, 130, 200, 16, 16, " ACC\_Y :", RED);

&#x20;   lcd\_show\_string(30, 150, 200, 16, 16, " ACC\_Z :", RED);

&#x20;   lcd\_show\_string(30, 170, 200, 16, 16, " Pitch :", RED);

&#x20;   lcd\_show\_string(30, 190, 200, 16, 16, " Roll  :", RED);



&#x20;   while (1)

&#x20;   {

&#x20;       vTaskDelay(pdMS\_TO\_TICKS(10));

&#x20;       t++;



&#x20;       if (t == 10)

&#x20;       {   

&#x20;           sc7a20h\_read\_rawdata(\&xyz\_rawdata);

&#x20;           

&#x20;           user\_show\_mag(30, 110, "ACC\_X :", xyz\_rawdata.acc\_x);

&#x20;           user\_show\_mag(30, 130, "ACC\_Y :", xyz\_rawdata.acc\_y);

&#x20;           user\_show\_mag(30, 150, "ACC\_Z :", xyz\_rawdata.acc\_z);

&#x20;           user\_show\_mag(30, 170, "Pitch :", xyz\_rawdata.pitch);

&#x20;           user\_show\_mag(30, 190, "Roll  :", xyz\_rawdata.roll);

&#x20;           

&#x20;           t = 0;

&#x20;           LEDR\_TOGGLE();

&#x20;       }

&#x20;   }

}





从上述源码可知，我们首先初始化各个外设，如IIC、 SPI、 XL9555、 SC7A20H和LCD等驱动，然后调用 sc7a20h\_read\_rawdata ()函数测量数据，最终计算出pitch俯仰角和roll翻滚角数据，并在LCD上显示。LED灯每隔100毫秒状态翻转，实现闪烁效果。

