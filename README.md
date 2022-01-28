<div id="top"></div>
<!--
*** Thanks for checking out the Best-README-Template. If you have a suggestion
*** that would make this better, please fork the repo and create a pull request
*** or simply open an issue with the tag "enhancement".
*** Don't forget to give the project a star!
*** Thanks again! Now go create something AMAZING! :D
-->



<!-- PROJECT SHIELDS -->
<!--
*** I'm using markdown "reference style" links for readability.
*** Reference links are enclosed in brackets [ ] instead of parentheses ( ).
*** See the bottom of this document for the declaration of the reference variables
*** for contributors-url, forks-url, etc. This is an optional, concise syntax you may use.
*** https://www.markdownguide.org/basic-syntax/#reference-style-links
-->
<!-- [![Contributors][contributors-shield]][contributors-url]
[![Forks][forks-shield]][forks-url]
[![Stargazers][stars-shield]][stars-url]
[![Issues][issues-shield]][issues-url]
[![MIT License][license-shield]][license-url]
[![LinkedIn][linkedin-shield]][linkedin-url] -->



<!-- PROJECT LOGO -->
<br />
<div align="center">
  <a href="https://github.com/github_username/repo_name">
    <img src="images/logo.png" alt="Logo" width="80" height="80">
  </a>

<h3 align="center">CNT++</h3>

  <p align="center">
    Firmware module for counting people using STM32F4x microcontroller 
    <br />
    <a href="https://github.com/SoC-Arch-polito/cnt21"><strong>Explore the docs »</strong></a>
    <br />
    <br />
    <!-- <a href="https://github.com/github_username/repo_name">View Demo</a> -->
    ·
    <a href="https://github.com/SoC-Arch-polito/cnt21/issues/new">Report Bug</a>
    ·
    <a href="https://github.com/SoC-Arch-polito/cnt21/issues/new">Request Feature</a>
  </p>
</div>



<!-- TABLE OF CONTENTS -->
<details>
  <summary>Table of Contents</summary>
  <ol>
    <li>
      <a href="#about-the-project">About The Project</a>
      <ul>
        <li><a href="#built-with">Built With</a></li>
      </ul>
    </li>
    <li>
      <a href="#getting-started">Getting Started</a>
      <ul>
        <li><a href="#prerequisites">Prerequisites</a></li>
        <li><a href="#installation">Installation</a></li>
      </ul>
    </li>
    <li><a href="#usage">Usage</a></li>
    <li><a href="#roadmap">Roadmap</a></li>
    <li><a href="#contributing">Contributing</a></li>
    <li><a href="#license">License</a></li>
    <li><a href="#contact">Contact</a></li>
    <li><a href="#acknowledgments">Acknowledgments</a></li>
  </ol>
</details>



<!-- ABOUT THE PROJECT -->
## About The Project

[![Product Name Screen Shot][product-screenshot]](https://example.com)

The people counter is able to count the number of people that enter/exit a room using two IR sensors. In addition to that, there are two leds that signal if the room is full (red) or if there are places left (green). Moreover, a display shows the number of free seats. The maximum number of people, the reset feature and the log transfer can be done via UART.
Our target is to develop firmware modules able to save the status of the system on log files in the flash memory, configure the device and download logs via UART + DMA. In addition to that, the system will also show the state of the room on the LCD display and on the leds and manage the IR sensors + RTC.

<p align="right">(<a href="#top">back to top</a>)</p>


### Built With

* [Renode](https://renode.io/)
* [PlatformIo](https://platformio.org/)
* [Mono](https://www.mono-project.com/)

<p align="right">(<a href="#top">back to top</a>)</p>


### Functional Specification

The expected results consist of providing a safe system that can be used in critical domains too (for example in applying restrictions in a public place due to Covid 19). The firmware will be as optimized as possible, in order to reduce the power consumption. The data displayed on the LCD will be shown in a user-friendly way so that everyone will understand. 


<!-- GETTING STARTED -->
## Getting Started

### Prerequisites
One important point of CNT++ is that it is very easy to be launched thanks to its adaptation capability to <a href="https://docs.platformio.org/en/latest/integration/ide/vscode.html#ide-vscode">VSCode, Platform IO</a> and Renode.

After installing the developements tools the system can be run following the steps listed below:

* Clone the repository.
  ```sh
  git clone https://github.com/SoC-Arch-polito/cnt21.git
  ```
* Build the project.
* Open Renode and write `include @./stm32f4_discovery_modified.resc`.
  ```sh
  cd cnt21
  renode
  include @./stm32f4_discovery_modified.resc
  ```
* Launch the firmware writing `start` command in the Renode console.
* Interact with CNT++ writing the commands both in the Renode console in the CommandConsole (UART4).

<!-- ROADMAP -->
## Roadmap

The overall firmware is divided into different modules, each one aiming to implement different functionalities.
 

- [] GPIO (LEDs and IR sensors)
- [] I2C DISPLAY
- [] UART and DMA
- [] FLASH Interface Register
- [] RTC
- [] FLASH

### GPIO
The GPIO module is composed of two parts that are used to manage both the two leds (red and green) and the interrupt coming from the two IR sensors (enter and exit).

#### LED
The green led is used to signal when the number of people inside is lower than the maximum configured number. On the other hand, the red led is used to signal when the room is full; in few words, it works just like a traffic light.
Accordingly to the available GPIO in the STM32F4 platform, the red and green led have been connected to the gpioPortD on port 6 and 7 respectively.

The software part is composed of two parts: an initial setup from the main, thanks to the HAL library provided by STM and the class that contains the abstraction method to access them. First of all, the clock on the gpioPortD port must be enabled with the HAL command `__HAL_RCC_GPIOD_CLK_ENABLE()` and the the pins 6 and 7 must be configured as pull up output.

At this point, the methods in the `led.c` class can be called to control the leds when needed.

#### IR Sensors
The system needs two IR sensors, one for the entrance and the other for the exit. At the firmware level, they are configured at the same GPIO port of the leds. So, the pins 8 and 9 are dedicated for this two sensors and configured during the main initialization as pull up with the interrupt that is generated at the falling edge.

In fact, in order to avoid polling the state of both the IR in main loop, two interrupts are generated; one for each sensor. One important aspect about these interrupt is that they must be masked while performing the download of the history, in order to avoid overlaps.

In order to be able to simulate the behaviour of a group of people that enters into a room, the IR sensor is a custom peripheral. I extends the behaviour of a normal button, but it exposes a method to simulate N interrupt with the a specified time interval, like in the following example:
```
sysbus.gpioPortD.IRSensorEnter TriggerSensorNTimes 10 1000
```

### I2C DISPLAY
The I2C display module is a wrapper over the HAL methods that are already provided. For this project, the simulated display 16x2 (LCD Module 1602A) has been connected to the first peripheral with the 0x34 address, since it has been considered with mounted a PCF8574 I2C interface in order to reduce the number of used pins. 
By referring to the [https://www.openhacks.com/uploadsproductos/eone-1602a1.pdf](LCD Module 1602A datasheet), all the commands are managed in 10 bits, in which the MSBs define the command and the LSBs the configuration.
In order to abstract from the complexity of the command management, the hexadecimal representation of each one of the has been put in the `i2c_lcd.h` header file.
The setup procedure of the display is based on sending a precise sequence of bits with the correct timing, in order to switch from the 8 to 4 bits mode. The steps are the following:
- Wait 40ms, send 0x31
- Wait 4.1ms, send 0x31
- Wait 100us, send 0x31
- Send 0x22 to the the interface in 4 bit mode
- Send a Function Set command with DL=0 (confirm the 4 bits mode), N=1 (2 line display) and F=0 (5x8 character)
- The the display is optionally turned off and cleared with the 0x01 command
- The cursor direction is set from left to right by sending 0x06

It's important to specify that all the previous commands are send in a specific manner, so that the controller on the LCD can understand if them. Commands are sent as 4 bytes, where the first and the second byte of the initial command are split and duplicated in 4 bytes and then an or logic operation is performed to set the backlight and if the display is enabled.

Starting from a defined location on the 16x2 matrix, characters can be sent one by one by sending their byte representation.
In order to make the simulated environment complete, a simulated LCD has been developed in C# with the Mono framework that is an open source implementation of Microsoft's .NET Framework. In this way, Form can be directly created from the C# of the custom peripheral. An image is loaded as background and each of the 16x2 characters are manged as a 5x8 dot matrix.
In the .cs definition of the peripheral there is a map that converts all ASCII characters from into the 5x8 matrix representation.

In this case, the LCD I2C protocol has been implemented 1:1, accordingly to the specification.
Not only the normal chars has been implemented but, since a normal LCD display has an internal memory that allows to store up to 5x8 custom char definition, the simulator and the firmware has been enhanced with this additional feature.


### UART and DMA
This module is in charge of managing communications between the system and the external world through UART. The idea is that an operator can interface with the system via a UART terminal so he can configure the system (maximum number of allowed people and system date and time) and retrieve the logs.

The main module, called COMM is composed of two sub modules, called UART and DMA.

#### UART 
The UART sub module is in charge of configuring one of the UART peripherals offered by the STM32F4 platform, in particular it uses `uart4`, that spaces between the two memory addresses 0x40004C00 and 0x40004CFF. The peripheral is configured in such way that an operator can connect to the system setting up as baud rate 115200, 8 bit word, 1 stop bit and no parity bit. Moreover, when data are sent from the external to the system via UART, an interrupt routine is fired that will launch a callback that is set when the sub module is initiated. Interrupt is set even when a transmission is completed, that will launch as usual an external callback.

#### DMA sub module
The DMA sub module is in charge to configure a DMA channel in order to transfer, when requested, data from memory to the uart4 peripheral. It uses the dma1 peripheral, placed in memory between the address      0x40026000 and 0x400263FF. Of this peripheral, the Stream 1, Channel 0 is used.

#### COMM main module
main module, finally, puts together them and creates the real interaction between the operator and the system. During its init phase, it initialise both the UART and DMA sub modules and links the DMA peripheral to the UART one by means of a function exposed by the UART sub module. It defines two internal callbacks both for UART's receive interrupt and transmit complete interrupt. The receive callback is in charge of handling the commands sent by the operator and for a complete list of commands available, the operator can use the command help.

![image](https://user-images.githubusercontent.com/9128612/147503209-bae8e391-c02f-44ee-81cd-5098d5b92036.png)

- `newValueSet`: launched when the operator set a new value, only if it's a valid value (<= 65535). The new value is passed as function argument to the callback.
- `onUARTDownload`: it's executed both when a download operation begins and ends (it can be distinguished thanks to the function's boolean argument). In the final implementation, it's used to set the LCD screen with the sentence "Downloading..".
- `texttt{onNewSysDateTime`: launched when the operator set a new date and time for the system, only if it has a valid format dd/mm/yyyy hh/mm/ss. In the final implementation, it's used to set up the RTC peripheral's time.

Finally, the important thing to underline is that the log are stored in the flash memory in a raw format (4 bytes timestamp + 2 bytes count) so a simple get command gives as output an unreadable string. In order to read and understand it, an external python script has been developed that sends via UART the command get and decodes the output given by the system.

### FLASH Interface Register
Unfortunately, Renode lacks of a lot of peripherals and one of these is the Interface Flash Register used to manage the flash memory. It does many operations on the flash memory but the most important one that is in the interests of this project is the functionality of sector erasing used to complete erase an entire sector.



### RTC
The Real Time CLock (RTC) is a very important part of the project, because is used to log all the entrance and exit of the people. The used source of the clock, as can be seen in the `SystemClock_Config()`, is the `RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE`. 
Starting from this, the RTC has been initialized thanks to the function `MX_RTC_Init()`, in which we declare the main parameters of the RTC, such as:
- Time format
- Time structure
- Data structure

These values are the default ones, because the user will then set the actual time via UART. This choice was forced by the fact that the microcontroller is not able to retrieve the current time and date, and so the only solutions was to force the user to set it at startup.

As explain in the FLASH paragraph, the date was converted into a standard format, that is the Unix timestamp.  The choice was done to have a default format but also in order to save space in the memory.

In addition to that, due to some Renode issue for the RTC peripherals, the RTC time format was internally set to `RTC_HOURFORMAT_12`. However, in order to simplify the usability, for the user everything is hidden and the time format used to insert date and time is in 24 hours format. 


When an interrupt is triggered by the sensor that retrieve the entrance/exit of a persona, the function `log_update_number()` is called. This method is used to retrieve current date and time, thanks to the HAL provide by the library`HAL_RTC_GetTime` and `HAL_RTC_GetDate` and then the result is converted into unix timestamp format. After all of that the result is written in the FLASH memory.

### FLASH
In this paragraph are discussed the decision made for the FLASH memory with the relative implementation. 
The memory of our microcontroller is divided into 12 sector, for a total of 1MBytes. The decision was to write on the Sector 11, that is safe and big (128Kbytes). 
The addresses of different sectors are:
- Sector 0  0x0800 0000 - 0x0800 3FFF  16 Kbytes
- Sector 1  0x0800 4000 - 0x0800 7FFF  16 Kbytes 
- Sector 2   0x0800 8000 - 0x0800 BFFF  16 Kbytes 
- Sector 3  0x0800 C000 - 0x0800 FFFF  16 Kbytes 
- Sector 4   0x0801 0000 - 0x0801 FFFF  64 Kbytes  
- Sector 5  0x0802 0000 - 0x0803 FFFF  128 Kbytes 
- Sector 6   0x0804 0000 - 0x0805 FFFF  128 Kbytes 
- Sector 7   0x0806 0000 - 0x0807 FFFF  128 Kbytes 
- Sector 8   0x0808 0000 - 0x0809 FFFF  128 Kbytes 
- Sector 9   0x080A 0000 - 0x080B FFFF  128 Kbytes 
- Sector 10   0x080C 0000 - 0x080D FFFF  128 Kbytes 
- Sector 11   0x080E 0000 - 0x080F FFFF  128 Kbytes   

Another important the group had to take was how many bytes in the memory were necessary for each timestamp. It was crucial to find a good tradeoff between readability and compactness.
The choice was the following: 
- 32 bits for the time and date (unix timestamp)
- 16 bits for the counter

Thanks to this convention, the memory occupation was 48 bits (6 bytes) for each entrance/exit. For this we can say that 128Kbytes = 131972 bytes.
Then, knowing we occupy 6 bytes, we can conclude that 131072/6 = 21845,333 = 21845 

So we can save in the flash memory 21845 entrace/exit.
The last important thig to mention si the fact that when the flash is full, the writing restarts from the beginning of it. So it works like a First in First out.

## Open issues
See the [open issues](https://github.com/SoC-Arch-polito/cnt21/issues) for a full list of proposed features (and known issues).

<p align="right">(<a href="#top">back to top</a>)</p>



<!-- CONTRIBUTING -->
## Contributing

Contributions are what make the open source community such an amazing place to learn, inspire, and create. Any contributions you make are **greatly appreciated**.

If you have a suggestion that would make this better, please fork the repo and create a pull request. You can also simply open an issue with the tag "enhancement".
Don't forget to give the project a star! Thanks again!

1. Fork the Project
2. Create your Feature Branch (`git checkout -b feature/AmazingFeature`)
3. Commit your Changes (`git commit -m 'Add some AmazingFeature'`)
4. Push to the Branch (`git push origin feature/AmazingFeature`)
5. Open a Pull Request

<p align="right">(<a href="#top">back to top</a>)</p>



<!-- LICENSE -->
## License

Distributed under the BSD 3-Clause License. See `LICENSE.txt` for more information.

<p align="right">(<a href="#top">back to top</a>)</p>



<!-- MARKDOWN LINKS & IMAGES -->
<!-- https://www.markdownguide.org/basic-syntax/#reference-style-links -->
[contributors-shield]: https://img.shields.io/github/contributors/github_username/repo_name.svg?style=for-the-badge
[contributors-url]: https://github.com/github_username/repo_name/graphs/contributors
[forks-shield]: https://img.shields.io/github/forks/github_username/repo_name.svg?style=for-the-badge
[forks-url]: https://github.com/github_username/repo_name/network/members
[stars-shield]: https://img.shields.io/github/stars/github_username/repo_name.svg?style=for-the-badge
[stars-url]: https://github.com/github_username/repo_name/stargazers
[issues-shield]: https://img.shields.io/github/issues/github_username/repo_name.svg?style=for-the-badge
[issues-url]: https://github.com/github_username/repo_name/issues
[license-shield]: https://img.shields.io/github/license/github_username/repo_name.svg?style=for-the-badge
[license-url]: https://github.com/github_username/repo_name/blob/master/LICENSE.txt
[linkedin-shield]: https://img.shields.io/badge/-LinkedIn-black.svg?style=for-the-badge&logo=linkedin&colorB=555
[linkedin-url]: https://linkedin.com/in/linkedin_username
[product-screenshot]: images/main.png
