# RainbowSyringe
Dit is de uitleg horende bij de vaccintester uit de escape room.

## Inhoud

- [Algemene uitleg](#algemene-uitleg)
- [Flowcharts](#flowcharts)
- [Implementatie](#implementatie)
- [Budget](#budget)
- [Risico's](#risicos)
- [Gantt chart](#gantt-chart)

## Algemene uitleg

Door het oplossen van de 5G-puzzel, wordt er een RFID kaart verkregen. Die kaart kan gebruikt worden om de uiteindelijke kleurensequentie voor de vaccintester te vinden. In de deur van een kast zit op een bepaalde plaats een NFC reader. De spelers moeten dan met de kaart op zoek gaan naar de kast en uitzoeken waar in de deur de NFC reader zit. Door de kaart dan te scannen worden er witte leds in de kast geactiveerd. Boven elke witte led bevindt zich een gekleurde vloeistof in een proefbuis. De houders voor de proefbuis kunnen gebruikt worden om de witte leds weg te steken. De leds gaan in een bepaalde volgorde oplichten, zodat er een sequentie gevonden kan worden van 7 kleuren die na elkaar oplichten. Om die kleurensequentie in te geven wordt er gebruik gemaakt van drie knoppen die verspreid zijn in de ruimte. Bij elke knop hoort een kastje, hierin kan een voorwerp met een primaire kleur gestoken worden. Om de kleuren te vormen worden de knoppen dan ingedrukt, de kleur van het voorwerp wordt gescand. Om alle spelers te laten meedoen moeten de knoppen om de kleuren te vormen binnen een bepaald interval ingedrukt worden. Zo wordt geel gevormd door groen, rood en zwart moeten groen, rood en zwart dus ongeveer op hetzelfde moment ingedrukt worden. Die knoppen zorgen er dan voor dat de bijhorende ledstrip die zich in een spuit bevindt oplicht. Wanneer de juiste kleur verkregen is wordt er gesprongen naar de volgende kleur in de sequentie en herhaalt het vorige zich. Wanneer de laatste kleur juist is, wordt een random getal gegenereerd dat een digit van de te zoeken code is. Deze wordt doorgestuurd naar de centrale eenheid en ook getoond aan de deelnemers door het aantal proefbuizen die oplichten binnen de kast.

Om dit alles te kunnen uitvoeren wordt er gebruik gemaakt van twee ESP32's, die met elkaar kunnen communiceren via een ESPnow-connectie. Wanneer de juiste RFID gescand wordt, wordt er een signaal verzonden van de ESP32 die verbonden is met de NFC-reader naar de ESP32 die verbonden is met de kleursensoren, de drukknoppen en de leds binnen de spuit en in de kast. Dat signaal zal ervoor zorgen dat de kleursensoren, de leds binnen de spuit en in de kast en de drukknoppen geactiveerd worden.

Daarnaast zal de laatst besproken ESP via het wifi-netwerk geconnecteerd zijn met de broker. Op die manier kan er uiteindelijk een digit van de code die ingevoerd moet worden bij alohomara doorgestuurd worden naar de centrale eenheid of broker.

## Flowcharts
### Vaccintester
![Flowchart_vaccintester](https://github.com/ViktorMorre/RainbowSyringe/blob/main/Vaccintester_flowchart.png)
### RFID
![Flowchart_RFID](https://github.com/ViktorMorre/RainbowSyringe/blob/main/flowchart_RFID.png)
### Buttons
![Flowchart_buttons](https://github.com/ViktorMorre/RainbowSyringe/blob/main/flowchart_button.png)
### Centrale ESP32
![Flowchart_centralESP32](https://github.com/ViktorMorre/RainbowSyringe/blob/main/flowchart_central_ESP32.png)



## Implementatie

De positie van deze puzzel binnen het lokaal maakt in principe niet zo veel uit. Er moet wel voor gezorgd worden dat de knoppen minstens 1,5 meter van elkaar gescheiden zijn zodat telkens aan de voorwaarde van de "T'is beter op anderhalve meter"-puzzel voldaan is. Daarnaast gaat de spuit aan een muur moeten hangen zodat alles op een correcte manier aangesloten kan worden.

## Budget

| Artikel                                      | Aantal | Kostprijs | Totaal  |
|----------------------------------------------|--------|-----------|---------|
| Kleurensensor                                | 1      | 4.00 €    | 4.00 €  |
| Esp32                                        | 2      | 5.79 €    | 11.58 € |
| NRF24L01+ 2.4GHz wireless transceiver module | 3      | 1.95 €    | 5.85 €  |
| Witte led                                    | 7      | 0.42 €    | 2.94 €  |
| Kleurenledstrip 1m                           | 1      | 10.00 €   | 10.00 € |
| NFC reader                                   | 1      | 5.00 €    | 5.00 €  |
| Weerstanden                                  | 7      | x         | x       |
| RobotDynUNO R3                               | 1      | 11.00 €   | 11.00 € |
|                                              |        |           |         |
| Hebben we reeds                              |        |           | 50.37 € |
|                                              |        |           |         |
| Drukknoppen                                  | 3      | 2.00 €    | 6.00 €  |
| Adapters                                     | 5      | x         | x       |
| PCBway                                       | 1      | 5.00 €    | 5.00 €  |
| Kleurensensor                                | 2      | 4.00 €    | 8.00 €  |
| RobotDynUNO R3                               | 2      | 11.00 €   | 22.00 € |
|                                              |        |           |         |
| Nog aan te kopen                             |        |           | 41.00 € |
|                                              |        |           |         |
| Totaal budget                                |        |           | 91.37 € |
|                                              |        |           |         |
## Risico's

- Wanneer de 5G-puzzle niet werkt, kan de nodige RFID kaart niet verkregen worden. Dit kan opgelost worden door een tweede kaart te gebruiken die ook gebruikt kan worden om onze puzzel te starten. Deze kaart kan ergens verstopt worden binnen de kamer. De positie van die kaart kan dan via een raadsel achterhaald worden.

- Wanneer de RFID reader en de receiver niet geconnecteerd zijn, kan de puzzel niet volledig gestart worden. Dit kan eventueel opgelost worden door Alohomora een signaal te laten versturen naar de tweede ESP32. 

- De kleursensor kan soms bij de eerste meting een fout resultaat geven. Dit kan opgelost worden door bijvoorbeeld tien metingen na elkaar te doen en het gemiddelde te nemen van de verschillende waarden.

- De kleursensor kan foute resultaten geven door reflectie. Er moet dus gebruik gemaakt worden van een buis die mat is, waardoor de resultaten geen invloed van reflectie zullen hebben.

## Gantt chart

Via volgende link gaat u naar onze Gantt chart horende bij dit project: 
[Gantt vaccintester (ClickUp)](https://share.clickup.com/g/h/4dne7-50/c3532202026c060)

