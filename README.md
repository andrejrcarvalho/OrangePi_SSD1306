# **OrangePi - SSD1306**

Add a OLED display to your OrangePi with the system info!

## **Setup**

First you need to have the [WiringPi] instalLED and enable the I2C module.

If you use [armbian], just run `sudo armbian-config` and go to System > Hardware select i2c0 and then Save;


After that, clone the repository:
```sh
git clone https://github.com/andrejrcarvalho/OrangePi_SSD1306.git
```


Then build the executable file with:

```sh
cd ./OrangePi_SSD1306
make clean all 
chmod +x ./display.out
```

Now you need to connect the screen to your OrangePi following the next table:

| SSD1306 | OrangePi | 
|:-------:|:--------:|
| VCC | 3.3v |
| GND | 0v |
| SDA | SDA.0 |
| SDK | SCL.0 |

The program **needs also a button to wake up the screen**, this button should be connected between the `GPIO.1` pin and the `0v` pin;


To help with the wiring use the command `gpio readall` to get the pin layout of your OrangePi.

![Wiring Image](https://github.com/andrejrcarvalho/OrangePi_SSD1306/raw/main/pictures/IMG_20210721_204430.jpg)
NOTE: the extra wire from the button is to light up a LED inside the button

After this run the program:
```sh
sudo ./display.out
```

You can add a cron job to run the program each time the system boots up with the command `sudo crontab -e` and add the following line at the end:
```
@reboot /<path to the folder>/display.out
```
![Wiring Image](https://github.com/andrejrcarvalho/OrangePi_SSD1306/raw/main/pictures/IMG_20210721_204525.jpg)

[WiringPi]: <https://github.com/orangepi-xunlong/wiringOP>
[armbian]: <https://www.armbian.com/>