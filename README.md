# BasicDriver：常用的外设功能模块驱动

此工程编写了市面上常用的一些模块驱动以及常用的组件程序，单纯只是为了适应不同的平台上而使用；通过修改接口或者微调参数以达到快速开发，同时也避免在编写这些驱动时经常会出现重复劳动；移植不同的外设文件可实现对应的功能。同样，这些工程的代码编写，目标很明确：规范设计、简单易用、精小细致、不依赖具体的平台。



## Update log：

- [x] AT24C
- [x] W25Q
- [x] Software I2C
- [x] Software SPI
- [x] NRF24L
- [x] OLED
- [x] RF
- [x] LCD1602
- [x] Nixie
- [x] Ultrasound
- [x] Sense
- [ ] W5500
- [ ] Font_library
- [ ] IrDA_decode
- [ ] Touch_control



## Other

此外还有一些其他的外设功能组件开源项目：

1. [一个面向底层设备驱动的驱动库](https://github.com/foxclever/ExPeriphDriver)
2. [串行 Flash 通用驱动库](https://github.com/armink/SFUD)
3. [按键驱动](https://github.com/jiejieTop/ButtonDrive)
4. [软件定时器扩展模块](https://github.com/0x1abin/MultiTimer)
5. [简陋的操作系统](https://github.com/suxin1412/suos)