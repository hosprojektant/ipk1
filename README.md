# IPK1

Úkolem je vytvoření serveru v jazyce C/C++ komunikujícího prostřednictvím protokolu HTTP, který bude poskytovat různé informace o systému. Server bude naslouchat na zadaném portu a podle url bude vracet požadované informace. Server musí správně zpracovávat hlavičky HTTP a vytvářet správné HTTP odpovědi. Typ odpovědi bude text/plain. Komunikace se serverem by měla být možná jak pomocí webového prohlížeče, tak nástroji typu wget a curl.

## Spouštění projektu 


* extrahujeme soubor 

* přeložíme soubor do binárky pomocí příkazu:
```
make 
```
* následné spuštění projektu na portu: 12345 provedeme pomocí následujícího příkazu:
```
./hinfosvc 12345
```

## Použití 

Seznam příkazů pro používání 

* pomocí následujícího příkazu získame doménové jméno na serveru(localhost) a portu 12345
```
GET http://localhost:12345/hostname
```
![image](https://user-images.githubusercontent.com/45818202/157080846-e11ca830-952e-431d-a144-8b13b5d03998.png)

* pomocí následujícího příkazu získame informace o procesoru na serveru(localhost) a portu 12345
```
GET http://localhost:12345/cpu-name
```
![image](https://user-images.githubusercontent.com/45818202/157080574-b7371b1d-cebf-4923-bd0c-8a7795b6cdd3.png)

* pomocí následujícího příkazu získame aktualní zátěž procesoru na serveru(localhost) a portu 12345
```
GET http://localhost:12345/load
```
![image](https://user-images.githubusercontent.com/45818202/157080724-0e07a2d8-a630-47e2-9ee8-da7a612dcb5b.png)

## Popis Jednotlivých funkcí 

* funkce cpu_usage() přečte hodnoty z 2x /proc/stat jedny staré a nové je mezi nimi sleep(1) a následně pomocí algoritmu z https://stackoverflow.com/questions/23367857/accurate-calculation-of-cpu-usage-given-in-percentage-in-linux vypočítá jednotlivou zátěž a výnasobí ji 100. 
* funkce get_cpu_name() nám vrátí jméno modelu našeho procesoru které vyčte z /proc/cpuinfo
* pro host name není funkce pouze z /proc/sys/kernel/hostname přečteme jméno

