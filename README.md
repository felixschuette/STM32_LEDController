# STM32 LED Controller 
This repository includes an implementation of an LED Controller to control SK6812 RGB or RGBW LEDs attached 
to an STM32 MCU via USB from a host PC. It was implemented using STM32CubeIDE which is an eclipse-based development platform provided 
by ST. While the used MCU is the STM32 F103C8T6 - also called "bluepill" board - the project can 
also be ported to any other MCU of the ST product family as it is built with CubeIDE which allows for porting. 

## Pin-Out
This project uses the STM32F102C8T6 bluepill board. An image of the board's pin-out is given below:
![STM32 Bluepill Pin-Out](https://2.bp.blogspot.com/-gxCk0N-ntew/V5T_sjk-ASI/AAAAAAAAOC8/5ad3JX70DLwqF705LqAcdxwhtajkaYPxgCLcB/s1600/stm32f103c8t6_pinout.png "STM32 Bluepill Pin-Out")
## USB Communication
For the USB communication between host and MCU, message frames are defined. These message frames serve to trigger actions (e.g. showing an LED pattern)
to be performed by the MCU, or for the MCU to report on actions (e.g. button presses) to the Host. Currently, there are the following message types
defined: 
### LED Command
The _LED Command_ can be used to trigger the MCU to show specific color patterns on the attached LED stripes. Every LED Command begins with a specific header consisting of 10 bytes: 

| TYPE    | PACKET_NUM | PACKET_SIZE | LED_NUM | LED_TYPE | ANI_DIR | DURATION | BUS_NUM |
|---------|------------|-------------|---------|----------|---------|----------|---------|
| 1 Bytes | 1 Byte     | 1 Byte      | 2 Bytes | 1 Byte   | 1 Bytes | 2 Bytes  | 1 Bytes |

* TYPE:  identifies the message frame. In this case, "0x00" corresponds to the _LED Command_.
* PACKET_NUM:  Total number of packets subsequently following to the header. 
* PACKET_SIZE:  Regular size of each packet following. Note that the size of the last packet can be smaller. 
* LED_NUM:  indicates the number of LEDs to be controlled.
* LED_TYPE: indicates whether the attached LEDs are of type RGB or RGBW. Thereby, "0x00" corresponds to RGB and "0x01" to RGBW, respectively.
* ANI_DIR: specifies the direction of animation. If set to "0x00", no animation is performed. "0x01" corresponds to an animation in upward direction and "0x002" in downward direction.
* DURATION: specifies the time span in milliseconds in which the pattern should be displayed. 
* BUS_NUM: indicates by which stripe (0x00, or 0x01) the given pattern is to be shown. 

Note that all bytes are formatted MSB. 
After the header, the corresponding packets including the LED patterns consisting of a series of LED colors follow. 
The number of packets following the header is indicated by PACKET_NUM and the size of every following packet is indicated by PACKET_SIZE.
The header and each packet is sent separately but subsequently. Messages that are received by the MCU with a too large time interval may be rejected. 
Depending on whether the _LED_TYPE_ was set to RGB or RGBW, each LED color consists of 3 or 4 bytes, respectively. A packet consisting "n" LED colors:
<table class="tg">
  <tr>
    <th class="tg-0pky" colspan="4">LED #1 Color</th>
    <th class="tg-0pky" colspan="4">LED # 2 Color</th>
    <th class="tg-0pky">...</th>
    <th class="tg-0pky" colspan="4">LED #n Color</th>
  </tr>
  <tr>
    <td class="tg-0pky">RED</td>
    <td class="tg-0pky">GREEN</td>
    <td class="tg-0pky">BLUE</td>
    <td class="tg-0lax">WHITE</td>
    <td class="tg-0pky">RED</td>
    <td class="tg-0lax">GREEN</td>
    <td class="tg-0lax">BLUE</td>
    <td class="tg-0lax">WHITE</td>
    <td class="tg-0pky">...</td>
    <td class="tg-0pky">GREEN</td>
    <td class="tg-0lax">BLUE</td>
    <td class="tg-0lax">BLUE</td>
    <td class="tg-0lax">WHITE</td>
  </tr>
  <tr>
    <td class="tg-0lax">1 Byte</td>
    <td class="tg-0lax">1 Byte</td>
    <td class="tg-0lax">1 Byte</td>
    <td class="tg-0lax">1 Byte</td>
    <td class="tg-0lax">1 Byte</td>
    <td class="tg-0lax">1 Byte</td>
    <td class="tg-0lax">1 Byte</td>
    <td class="tg-0lax">1 Byte</td>
    <td class="tg-0lax">...</td>
    <td class="tg-0lax">1 Byte</td>
    <td class="tg-0lax">1 Byte</td>
    <td class="tg-0lax">1 Byte</td>
    <td class="tg-0lax">1 Byte</td>
  </tr>
</table>