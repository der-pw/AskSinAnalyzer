# AskSinSniffer328P

## Achtung:
Das Projekt liegt nun unter: https://github.com/psi-4ward/AskSinAnalyzer-Sniffer


  

Sketch für den Arduino (Pro Mini, Nano), um die Telegramme zu sniffen.

## Erweiterung um die Möglichkeit Anzeige des aktuellen Signalpegel

Dieser Fork bietet die Möglichkeit, ein 0.91" Dipsplay zu verwenden, um darauf den aktuellen Signalpegel
anzuzeigen. Eine dynamische Pegelanzeige ist ebenfalls implementiert, um Ändereungen zu visualisieren.

![AskSin-Analyzer Pegel](https://github.com/der-pw/AskSinAnalyzer/blob/master/Images/Sniffer_Display.jpg "AskSin-Analyzer Pegel")

## Flashen

Es gibt verschiedene Möglichkeiten, den Sketch zu flashen:

### Arduino IDE

Benötigte Bibliotheken: AskSinPP, Low-Power, EnableInterrupt. Ggf. Adafruit GFX Library und Adafruit SSD1306 für das OLED.

### avrdude

Direktes Flashen der HEX-Datei über avrdude für einen 328P:
```bash
avrdude -patmega328p -carduino -P/dev/ttyUSB0 -b57600 -D -Uflash:w:AskSinSniffer328P.hex:i
```

### PlatformIO

Dem Projekt liegt eine `platformio.ini` bei.

Um die Bibliotheken zu laden, den Sketch zu kompilieren und hochzuladen, reicht ein Command:

```bash
# Für Arduino Pro-Mini (328P)
pio run -e pro8MHzatmega328 -t upload

# Für Ardino Nano (zB AskSin-Analyzer-XS PCB von TomMajor)
pio run -e nano -t upload

# Für Arduino Nano mit OLED-Display
pio run -e nano-oled -t upload
```

```bash
# Serial monitor
pio device monitor
```


