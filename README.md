Note: These sources were based originally from (https://github.com/jeffayle/tamaboy)

![tamaboy](img/tamaboy_nds.png)

NTR/TWL SDK: TGDS1.65

tamaboy is a [Tamagotchi v1](http://adb.arcadeitalia.net/?mame=tama) emulator
for Nintendo DS / DSi / 3DS based on [TamaLIB](https://github.com/jcrona/tamalib/).

## Usage

NTR mode: copy arm7dldi-ntr/ndstamaboy.nds and release/drumSample.ima to SD root/base path.
TWL mode: copy arm7dldi-twl/ndstamaboy.srl and release/drumSample.ima to SD root/base path.

The Tamagotchi's Left, Middle and Right buttons are mapped respectively to the
d-pad Left, Up/Down and Right. Or use the touchscreen controls

The same buttons are also mapped to Select, A and B respectively. This mapping
is intended to be as intuitive as possible to navigate the device's menu.

Key R: Swap screens. Useful for GBA Macro NintendoDS mods where you only have a single screen available.

Note: Make sure to set the clock or your tama egg will never hatch. There should be
animated arrows on the clock screen.

!!!!NOTE!!!!
Saving: Press the Save & Exit button. If you're using NTR/TWL hardware, it should turn off automatically. Still, you should be greeted by a message confirming to shut off the NintendoDS unit.
Reading Save: Loading tamagotchi state occurs automatically on boot. It'll take a while! A drumming intro will play when the pet has loaded.
!!!!!!!!!!!!

RTC: You must manually update the clock by entering the clock (DOWN) then pressing LEFT + RIGHT. The NDS RTC code I added originally may break internal events, so it's disabled.

## Compile Time Dependencies

- TGDS (https://bitbucket.org/Coto88/toolchaingenericds/src)

## How to Compile

Since this is a TGDS project, you can follow the same steps, as, for example, https://bitbucket.org/Coto88/toolchaingenericds-template/

## 
Drums intro thanks to Thomas Drachmann - A Short Drum Solo


Latest stable release: https://bitbucket.org/Coto88/tamaboy/get/TGDS1.65.zip

Coto