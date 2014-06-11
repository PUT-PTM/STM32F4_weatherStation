STM32F4_weatherStation
======================
Projekt prostej stacji pogodowej wraz z zegarem czasu rzeczywistego w oparciu o p�ytk� STM32F4 Discovery.

Cele naszego projektu:
 * Wy�wietlacz LCD 2x16
 * czujnik temperatury KAMOD TEMP
 * zegar czasu rzeczywistego KAMOD RTC
 * aplikacja na komputer PC wy�wietlaj�ca wykres temperatury w zale�no�ci od godziny korzystaj�ca z wirtualnego portu szeregowego

IDE
----------------------

CoCOX CoIDE

Sterowniki
----------------------

Sterowniki do wirtualnego portu szeregowego:
http://www.st.com/web/en/catalog/tools/PF257938

Plany na przysz�o��
----------------------
Wi�cej odczytywanych danych i poprawienie komunikacji aplikacja <-> STM32F4

Aplikacja na PC
----------------------

Napisana w C# s�u�y do generowania wykres�w temperatury i ~~wilgotno�ci~~ z u�yciem danych ~~pobranych z pami�ci STM32F4 Discovery~~ losowych.