#Experimentelle Firmware 

von Travis Goodspeed, KK4VCZ


##Vorwort
Diese Seite versucht die experimentelle Firmware für das Tytera MD-380/MD-390/G/Retevis RT-3/RT-8 zu dokumentieren, die Features zu erläutern und die Installation zu erklären.

Übersetzung durch Kim - DG9VH - Ergänzungen: DL2MF / 20161025

###Unterstützte Geräte
Die experimentelle Firmware unterstützt die folgenden Geräte:

    Tytera/TYT MD380 (old vocoder)
    Tytera/TYT MD380 (new vocoder)
    Tytera/TYT MD390 (new vocoder, no gps)
    Tytera/TYT MD390/G (new vocoder / GPS) - derzeit noch ohne NetMonitor!
    Retevis RT3
    Retevis RT8 (new vocoder / GPS) - derzeit noch ohne NetMonitor!

##Installation
Nachfolgende Schritte beziehen sich ausschließlich auf die Vorgehensweise unter Linux. Auf anderen Betriebssystemen kann entsprechend eine andere Vorgehensweise notwendig werden.

###Voraussetzungen zur Installation
Um das Github-Repository von Travis Goodspeed auf den eigenen Rechner zu bekommen und komfortabel Aktualisierungen einspielen zu können, ist die Installation des Programmpakets `git` notwendig. Ebenfalls sind zur erfolgreichen Übersetzung die Pakete `gcc` sowie die `build-essentials` wie auch ein `arm-none-eabi` Cross Compiler notwendig.

Ebenso sind zur Nutzung folgende Pakete erforderlich:

* Python 2.7 or newer: http://www.python.org
* PyUSB 1.0: (0.4 does not work.) http://sourceforge.net/apps/mediawiki/pyusb/
* libusb 1.0: (0.4 does not work.) http://www.libusb.org/

####Automatische Installation der benötigten Pakete

Debian Stretch:

    apt-get install gcc-arm-none-eabi binutils-arm-none-eabi \
            libnewlib-arm-none-eabi libusb-1.0 python-usb

Debian Jessie / Ubuntu 16.04 LTS:

    apt-get install gcc-arm-none-eabi binutils-arm-none-eabi libusb-1.0 git \
                    libnewlib-arm-none-eabi make curl python-pip unzip
    pip install pyusb -U # update PyUSB to 1.0


###Git-Repository klonen
Mit dem Kommando
 `git clone https://github.com/travisgoodspeed/md380tools.git`
wird in das aktuelle Verzeichnis eine aktuelle Kopie des Repositories erzeugt. Um dieses hin und wieder zu aktualisieren, braucht man im beim clone durch git angelegtem Verzeichnis nur ein
 `git pull`
auszuführen.

###Berechtigungen zur Nutzung der USB-Schnittstelle
Unter Linux ist es erforderlich, wenn man die Tools nicht immer mit 'sudo' als root ausführen möchte, dass die Datei `99-md380.rules` im Hauptverzeichnis des Repositories nach `/etc/udev/rules.d/` kopiert wird. 

###Firmware übersetzen
Um die Firmware zu übersetzen, führt man im obersten Verzeichnis des geklonten Repositories den Befehl

 `make`

aus.

Möchte man nach der Übersetzung gleich die neue Firmware in das Funkgerät flashen, so bringt man das Funkgerät zunächst in den Flashmodus (Einschalten mit gedrückter PTT-Taste und der oberen Funktionstaste), schließt es mit dem USB-Programmierkabel an den Computer an und führt einen der folgenden Befehle aus:
 `make flash`         
 `make flash_D02` - für das MD-380 (flashen der älteren Firmware-Version)
 
 `make flash_D13` - für das MD-380  
 `make flash_S13` - für das MD-390/G
auf.

<note tip>**Firmware mit dem Tytera Updater einspielen:**
Die gepatchte Firmware lässt sich auch mit dem Tytera Updater unter Windows ins Funkgerät flashen. Hierzu nimmt man einfach die ''experiment.bin'' aus dem Verzeichnis ''applet'' und spielt diese mit dem Updater ein.</note>

##Installation unter Windows
###Installation von Git
Zunächst installiert man Git für Windows, indem man es unter https://git-scm.com/download/win herunterlädt und den Installationsprozess durchläuft. Am Ende der Installation wählt man günstigerweise die Option 'use windows default console window'.

###Installation von Make
Make für Windows lässt sich unter http://gnuwin32.sourceforge.net/packages/make.htm herunterladen.

###Installationo von Python 2.7
Zunächst lädt man Python 2.7 von  https://www.python.org/downloads/ runter und installiert dies in das Verzeichnis `C:\Python27`. Dort macht man eine Kopie von `python.exe` benennt diese um in `python2.exe`.

Anschließend sind folgende Pfadangaben zu den Umgebungsvariablen hinzuzufügen, unter Windows XP durch Rechts-Klick auf den Start-Button -> System -> Erweiterte Systemeinstellungen -> Umgebungsvariablen, für Nutzer, die nicht Windows XP nutzen: Start - Systemsteuerung - SUCHE "Umgebungsvariablen. Hier die Variable "Path" auswählen und bearbeiten. Folgende Pfade hinzufügen:
 C:\Program Files\GnuWin32\bin;C:\Python27

###Installation von gcc-arm-none-eabi
Unter https://launchpad.net/gcc-arm-embedded/4.8/4.8-2014-q1-update den Cross-Compiler tunerladen und am Ende der Installatin die Option 'add path to environment variable' anwählen, bevor man die Installation abschließt.

nach erfolgter Installation, bitte das Rebooten des PCs nicht vergessen, damit alle Änderungen aktiv werden!

###Installation von PyUSB
PyUSB unter https://sourceforge.net/projects/pyusb/ herunterladen und entpacken. Mit "Start" -> "Ausführen" -> "cmd.exe" eine Eingabeaufforderung öffnen, mit `cd [zielordner]` in den Zielordner wechseln und mit `python setup.py install` das Paket installieren.

###libusb-win32 installieren
Unter https://sourceforge.net/projects/libusb-win32/ die libusb herunterladen, entpacken. Den `bin`-Ordner öffnen und das Programmierkabel in den Computer und das Funkgerät stecken.

Nun das Funkgerät bei gedrückter PTT-Taste und gleichzeitig gedrückter oberer Funktionstaste einschalten und damit in den Flash-Modus schalten.

Nun das Programm `inf-wizard.exe` starten -> Next
Auswahl `digital radio in USB mode` -> Next -> Next
Die .inf-Datei speichern und anschließend auf `Install Now` klicken.

###Lokale Repository-Kopie erstellen
Git aus dem Start-Menü starten und den Punkt 'Clone Existing Repository' auswählen.

Source: http://github.com/travisgoodspeed/md380tools
Target: C:/Users/post2/Documents/git/travisgoodspeed/md380tools

Nach erfolgtem Clone-Vorgang:
Den Ordner `md380tools` suchen, Rechtsklick -> Git Bash Here

Um die experimentelle Firmware zu kompilieren und ins Radio zu flashen (hierbei das Radio in den Flash-Mode schalten mit PTT+oberer Taste beim Einschalten), folgende Befehlssequenz eingeben:
`make clean flash`

Um die User-Datenbank einzuspielen, das Radio im Normalbetrieb anschließen (einfach einschalten) und den Befehl `make flashdb` eingeben.

Im Anschluss wird die Nutzerdatenbank aus dem Internet heruntergeladen, entsprechend aufbereitet und in das Funkgerät überspielt. Das Funkgerät wird nach erfolgtem Einspielen automatisch neu gestartet.
 
##Bisher verfügbare Features
###Freischaltung des Empfangs aller Talkgroups und Private Calls
Die Firmware bietet seit Anfang an die Möglichkeit, sämtliche Talkgroups eines Repeaters, also nicht nur diejenigen, die in einer RX-Grouplist aufgeführt werden, sowie alle Private Calls, egal an welche ID, zu empfangen. Diese Funktion bietet die Möglichkeit, neben der Orientierung auf fremden Repeatern auch einen Bug der original Firmware ein wenig zu umgehen: Das Ignorieren des Admit Kriteriums. Durch die Tatsache, dass nun jede Talkgroup gehört werden kann, können Kollisionen mit anderen Signalen händisch vermieden werden.

###Verbesserte Anzeigeschriftart
Die Anzeigeschriftart in der Firmware verwendet eine serifenlose Schrift, diese verbessert die Lesbarkeit und ersetzt die für chinesische Geräte typische Serifenschriftart der originalen Firmware. Hierdurch erfährt das im Originalzustand etwas billig wirkende Gerät einen weiteren Akzeptanz-Schub (durch die Optik der Darstellung).

###Einbindung eines eigenen Einschaltlogos
Um ein eigenes Einschaltlogo (Welcome-Screen) zu aktivieren, muss man zunächst auf einem Linux-Rechner das Repository clonen. Ist dies geschehen, beginnt zunächst der kreative Part: Man öffnet ein Bildbearbeitungsprogramm, welches Grafiken im PPM-Format speichern kann - zum Beispiel (https://www.gimp.org) Gimp. Mit diesem Programm erzeugt man sich eine Grafik im 16-Farben-Modus und der Auflösung 160x40 Pixel. Hier kann man sich uns seiner Kreativität dann freien Lauf lassen.

Ist der kreative Teil erledigt, speichert man, wie erwähnt, die Grafik im PPM-Format, als Beispiel jetzt mal unter dem Namen "0x80f9ca8-eigenes_logo.ppm" und öffnet diese in einem Hex-Editor, um entsprechende Kopfinformationen zu editieren.

Man ersetzt die Passage:
> P6

> \# CREATOR: GIMP PNM Filter Version 1.1

> 160 40

> 255

durch
> P6

> \# MD380 address: 0x80f9ca8

> \# MD380 checksum: -941681526

> 160 40

> 255

Einige Beispiele fertiger ppm-Dateien, die man als Einschaltlogo verwenden kann, findet man im Repository im Verzeichnis ''md380tools/patches/2.032''.

Nach den besagten Änderungen kopiert man die bearbeitete PPM-Datei in das Verzeichnis ''md380tools/patches/2.032'' und trägt die Grafikdatei in das Makefile ein, indem man folgende Zeile einfügt und die entsprechend voerher aktive Zeile mit einer # auskommentiert:

 `../../md380-gfx --firmware=patched.img --gfx=0x80f9ca8-eigenes_logo.ppm relocate`
 
Ist dies erledigt, kann man im Grunde im Verzeichnis md380tools, nachdem man das Funkgerät per drücken der PTT und der oberen Menütaste beim Einschalten in den Flash-Mode geschaltet hat und das Programmierkabel an das Funkgerät wie auch an den PC angesteckt hat, per 

 `make flash`


zum einen die Firmware kompilieren und danach automatisch in das Funkgerät reinflashen.

Sollte man mit der Grafik alles richtig gemacht haben, sollte man nun beim Einschalten sein eigenes Logo zu sehen bekommen.

####Bereitstellung des Kommandozeilen-Tools md380-tool
Mit dem md380-tool können verschiedene Interaktionen mit dem Funkgerät über das USB-Programmierkabel vorgenommen werden. Nachfolgend eine kleine Erläuterung der Kommando-Parameter, die hinter den Programmaufruf gesetzt werden (Beispiel: ''md380-tool dmesg''):

Parameter | Verwendung
----------- | -----------
lookup 12345 | Ausgabe der Namensinformationen zu einer DMR-ID (hier 12345)
dmesg | Ausgabe des dmesg-Logs
dmesgtail | fortlaufende Ausgabe des dmesg-Logs
c5000 | Ausgabe des c5000 Basisband-Registers
findcc | Sucht innerhalb einer DMR-Aussendung nach entsprechend verwendeten Color-Codes
messages | Ausgabe aller eingegangenen und ausgegangenen Textnachrichten
keys | Ausgabe aller Schlüssel
spiflashid | Ausgabe des SPI Flash-Typs
flashdump <dateiname.bin> | Ausgabe des gesamten Flash-Speichers in eine Datei <dateiname.bin>
spiflashdump | Ausgabe des gesamten SPI-Flashabbildes (16 Megabyte)
coredump <dateiname.bin> | Ausgabe des Kern-RAMs
hexdump <0xcafebabe> | Einmalige Ausgabe eines Hexdumps einer Speicheradresse (hier <0xcafebabe>)
hexwatch | Fortlaufende Ausgabe eines Hexdumps einer Speicheradresse (hier <0xcafebabe>)
readword <0xcafebabe> | Ausgabe eines Speicherwortes (hier an der Adresse <0xcafebabe>)
dump  <dateiname.bin> <address> | Schreib ein 1kB großes Abbild ab der angegebenen Adresse
spiflashwrite <filename> <address> | Kopiert die angegebene Datei <filename> an die angegebene Speicheradresse <address> des SPI-Flashspeichers
wc -c < db/users.csv > data ; cat db/users.csv >> data && md380-tool spiflashwrite data 0x100000 | Kopiert die User-Datenbank aus dem Repository in den SPI-Flashspeicher an die Adresse 0x100000 

###Erweitertes Menü mit weiteren Funktionen
Die Firmware besitzt einen neuen Menüpunkt im Menü "Utilities" namens "MD380Tools". Diese Funktionen sind größtenteils nur in der UHF-Version (70 cm-Version) der Geräte nutzbar.

Die dort aufgeführten Menüpunkte wären derzeit folgende (mit nachfolgender Beschreibung)

Menüpunkt | Funktion 
--------- | -------
M. RogerBeep | Schaltet den modifizierten Roger-Piep ein oder aus
Date format | Hier kann zwischen der originalen Schreibweise und der deutschen Schreibweise des Datums umgeschaltet werden
UsersCSV | Aktiviert/deaktiviert die im SPI-Flash einspielbare User-Datenbank (DMR-ID-Datenbank der DMR-MARC-Datenbank)
Debug | Aktiviert/deaktiviert den Debug-Modus
Promiscuous | Aktiviert/deaktiviert (aktuell noch nicht getestet von mir) die Monitoring-Funktion (siehe **Freischaltung des Empfangs aller Talkgroups und Private Calls**)
Mic bargraph | Aktiviert die Modulations-Aussteuerungs-Anzeige
Edit DMR-ID | Ändern der DMR-ID des Funkgerätes
Experimental | Dieses Einstellung aktiviert experimentelle Funktionen welche nicht unbedingt funktionieren müssen. Zur Sicherheit ist diese Einstellung nicht Rebootfest.
DevOnly!! / NetMon | Dieses Feature bietet 3 zusätzliche Monitor-Anzeigen, mit denen wie über die USB-Schnittstelle Paketdaten der Verbindung im Rohdatenformat dargestellt werden können (Anzeige im Empfangsmodus mit 8 / 9 / # - Deaktivierung mit 7). Derzeit nur für das MD-380, da die Daten beim MD-390/G an anderen Speicheradressen zu finden sind.

###Aktivierung der User-Datenbank
**Wichtiger Hinweis:** Diese Funktion sollte nur auf Funkgeräten mit 16 MB SPI-Flashspeicher (nach aktuellen Erkenntnissen in der Regel UHF-Geräte) ausgeführt werden, da auf 1 MB-Geräten (in der Regel VHF-Geräte) der Codeplug bei älteren Versionen des Quellcodes (vor dem 28.04.2016) überschrieben wird und ggf. das Funkgerät hierdurch zunächst nicht mehr nutzbar wird.</note>
Zuerst wechselt man dann in das Verzeichnis ''db'' und führt dort ein

 `make clean`

und ein 

 `make update`

aus, um die User-Datenbank relativ tagesaktuell zu halten (Datenquelle ist hier die DMR-MARC-DB).

Im Verzeichnis ''md380tools'' führt man dann wieder den Befehl:

  `make flashdb`

im Hauptverzeichnis des Repositories aus. 

Alternativ hierzu funktioniert auch ein

  `make updatedb flashdb`

Besitzt man ein Gerät mit zu kleinem SPI-Flashspeicher (siehe Hinweis oben), erhält man seit dem 28.04.2016 folgende Meldung:
> SPI Flash ID: ef 40 14

> W25Q80BL 1MByte

> can't programm spi flash wrong flash type

Während des Flash-Vorgangs schaltet das Funkgerät in den USB-Programmiermodus - nach erfolgtem Schreiben der Daten startet das Funkgerät automatisch neu und man kann es vom Programmierkabel trennen.

Nach dem Flash-Vorgang muss im Menü des Funkgerätes die Userdatenbank wie unter **Erweitertes Menü mit weiteren Funktionen beschrieben** aktiviert werden.

## Support
Es wurde eine neue Google-Group ins Leben gerufen, die die Tools
supporten soll. Ebenso werden Fragen zur experimentellen / gepatchten 
Firmware für Tytera MD-380 / MD-390 / Retevis-RT-3, RT-8 und kompatible Geräte beantwortet.

Zögert also nicht, eure Fragen dort zu stellen bzw. eure Probleme zu 
schildern.

URL für die Gruppe: https://groups.google.com/forum/#!forum/md380tools

Diese ist aktuell öffentlich verfügbar und Bedarf zur Nutzung keiner 
Registrierung, wenn man entsprechend per E-Mail diese bestückt: 
md380tools@googlegroups.com
