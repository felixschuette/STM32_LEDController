import serial
import time
import math


class LedRGB(object):
    def __init__(self, _red=0, _green=0, _blue=0):
        self.red = _red & 0xFF
        self.green = _green & 0xFF
        self.blue = _blue & 0xFF

    @property
    def byte_string(self):
        string = [self.red, self.green, self.blue]
        return string


class LedRGBW(LedRGB):
    def __init__(self, _red=0, _green=0, _blue=0, _white=0):
        super().__init__(_red, _green, _blue)
        self.white = _white & 0xFF

    @property
    def byte_string(self):
        string = [self.red, self.green, self.blue, self.white]
        return string


class LedUSBCommand(object):
    def __init__(self, _led_colors, _animation_dir=0, _duration=20, _bus_num=0):
        self.LEDs = _led_colors
        self.led_num = [(len(_led_colors) & 0xFF00) >> 8, len(_led_colors) & 0xFF]
        self.animation = _animation_dir & 0xFF
        self.duration = [(_duration & 0xFF00) >> 8, _duration & 0xFF]
        self.bus_num = _bus_num & 0xFF
        self.max_packet_size = 60

    @property
    def led_type(self):
        led_type = 0
        if all(isinstance(x, LedRGBW) for x in self.LEDs):
            led_type = 1
        return led_type

    @property
    def packet_num(self):
        led_byte_width = 3 + self.led_type
        num = math.ceil(float((self.led_num[0] << 8 | self.led_num[1]) * led_byte_width) / self.max_packet_size)
        return num

    def next_led_pattern_packet(self):
        byte_cnt = 0
        for i in range(self.packet_num):
            yield self.led_patterns[byte_cnt:byte_cnt+self.max_packet_size]
            byte_cnt += self.max_packet_size

    @property
    def header(self):
        header = [0, self.packet_num, self.max_packet_size, self.led_num[0], self.led_num[1], self.led_type,
                  self.animation, self.duration[0], self.duration[1], self.bus_num]
        return header

    @property
    def led_patterns(self):
        string = []
        for led in LEDs:
            for byte in led.byte_string:
                string.append(byte)
        return string


if __name__ == "__main__":
    ser = serial.Serial('/dev/ttyACM0')
    ser.close()
    while True:
        ser.open()
        print(ser.name)

        LEDs = [LedRGBW(50, 3, 3, 3)]
        for i in range(0, 3):
            LEDs.append(LedRGBW(1, 50, 50))

        cmd = LedUSBCommand(LEDs, _animation_dir=1, _duration=700, _bus_num=1)
        print(cmd.header)
        ser.write(cmd.header)
        for packet in cmd.next_led_pattern_packet():
            ser.write(packet)
        print("closing socket")
        ser.close()
        time.sleep(5)
