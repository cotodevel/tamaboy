![tamaboy](img/tamaboy_nds.png)

NTR/TWL SDK: TGDS1.65

tamaboy is a [Tamagotchi v1](http://adb.arcadeitalia.net/?mame=tama) emulator
for Nintendo DS / DSi / 3DS based on [TamaLIB](https://github.com/jcrona/tamalib/).

## Usage

The Tamagotchi's Left, Middle and Right buttons are mapped respectively to the
d-pad Left, UpDown and Right. Or use the touchscreen controls

The same buttons are also mapped to Select, A and B respectively. This mapping
is intended to be as intuitive as possible to navigate the device's menus.

Normally only the dot-matrix "pixels" are shown on screen. The eight lcd icons
normally above and below the dot-matrix display are displayed as on overlay when
needed.

Press and hold the start button to show the lcd icon overlay. The overlay is
shown for 3 seconds when an icon changes. Press start to dismiss.

NB: Make sure to set the clock or your egg will never hatch. There should be
animated arrows on the clock screen.

## Compile Time Dependencies

- TGDS (https://bitbucket.org/Coto88/toolchaingenericds/src)

## How to Compile

Since this is a TGDS project, you can follow the same steps, as, for example, https://bitbucket.org/Coto88/toolchaingenericds-template/


TGDS Port status:

Todo: 
- sound
- RTC and filesystem support to allow the tamagotchi to live even if console is turned off.