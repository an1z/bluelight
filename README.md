# Bluelight - Raspberry Pi Pico LED timer

Lights up the onboard LED every X seconds. Pressing BOOTSEL turns off the LED.

## To build from source

See [Getting Started with the Raspberry Pi Pico](https://rptl.io/pico-get-started) and the README in the [pico-sdk](https://github.com/raspberrypi/pico-sdk) for information
on getting up and running.

After installing SDK, run these commands
1. `mkdir build && cd build`
2. `cmake ..`
3. `cd bluelight`
4. `make -j4`
5. The UF2 should be built in `bluelight/build/bluelight/bluelight.uf2`

## Load prebuilt UF2 on Pico
Copy bluelight.uf2 to Pi Pico USB mass storage device.

## Change timer frequency
Modify `static const int64_t delay_us` value in bluelight.c.