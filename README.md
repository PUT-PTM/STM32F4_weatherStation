STM32F4_weatherStation
======================
Projekt prostej stacji pogodowej wraz z zegarem czasu rzeczywistego w oparciu o p³ytkê STM32F4 Discovery.

Cele naszego projektu:
 * Wyœwietlacz LCD 2x16
 * czujnik temperatury KAMOD TEMP
 * zegar czasu rzeczywistego KAMOD RTC
 * aplikacja na komputer PC wyœwietlaj¹ca wykres temperatury w zale¿noœci od godziny korzystaj¹ca z wirtualnego portu szeregowego

IDE
----------------------

CoCOX CoIDE

Sterowniki
----------------------

Sterowniki do wirtualnego portu szeregowego:
http://www.st.com/web/en/catalog/tools/PF257938

Plany na przysz³oœæ
----------------------
Wiêcej odczytywanych danych i poprawienie komunikacji aplikacja <-> STM32F4

Aplikacja na PC
----------------------

Napisana w C# s³u¿y do generowania wykresów temperatury i ~~wilgotnoœci~~ z u¿yciem danych ~~pobranych z pamiêci STM32F4 Discovery~~ losowych.