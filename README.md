# Blinkiverse Bluetooth Low Energy Shield

An Arduino shield using the S-Power BG312L BLE module:
http://s-power.com.hk/BG312L.html

This shield is designed for the serial version of the module.

The BLE Shield has plenty of different peripherals that you can use to prototype your own Bluetooth Low Energy designs: three buttons, one RGB led, and an output to control one digital RGB LED strip.

The BLE Shield has been tested on the Arduino Duemilanove, Uno, and Leonardo. It has been confirmed to work with the iPad Mini running the free LightBlue application, and should work fine with iPhone 4S, iPhone5, and iPad Retina running iOS 5 and 6. The iPad 2 does not support Bluetooth 4.0 (BLE). Android support for BLE is unfortunately incomplete but in active development. Support for MacOS, Windows, and Linux is currently untested but seems to be in active development.

We're currently working on demo applications for iOS and computers, but in the mean time, you can communicate with your BLE Shield using the free LightBlue application by Punch Through, available on the app store. There are several other BLE iOS apps available that should work as well.

The BG312L module is based on the [TI CC2541][ti] Bluetooth 4.0 chip. More information, as well as sample iOS code, is available on [TI's Wiki][ti-wiki].

Let us know what cool things you do with the BLE Shield!

# Arduino Library and Demo Sketch

From the `library` folder, copy the `Blinkiverse_BLEShield` folder to your Arduino `libraries` folder. Restart the Arduino IDE, then open File->Sketchbook->libraries->Blinkiverse_BLEShield->BLEShield. Upload to your Arduino and follow the instructions on our [website][ble] to connect to the shield using your Bluetooth 4.0 compatible device. Enjoy!

[ti]: http://www.ti.com/product/cc2541
[ti-wiki]: http://processors.wiki.ti.com/index.php/Category:BluetoothLE