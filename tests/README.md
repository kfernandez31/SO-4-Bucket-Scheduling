## Testy rocznikowe
https://gitlab.com/mimuw-rocznik-2001/so-2022/testy-zad4

## Testy oficjalne


## Opis wyników i testów

### Uruchamianie testów

Należy wgrać łatkę i folder z testami na miniksa, zainstalować łatkę
zgodnie z poleceniem z treści zadania, wykonać `make` i po kolei uruchomić
wszystkie testy. Każdy z testów wypisuje na wyjście, czy zakończył się poprawnie.

### Opis raportu

Raport zazwyczaj zawiera kilka liczb oddzielonych przecinkami, a następnie pewien ułamek postaci `x/5`.
Oznacza to numery testów, które przeszły, a następnie liczbę podtestów z pliku `compare.c`, które przeszły.

Numery testów:

1. `syscall1`
2. `syscall2`
3. `run_driver_test.sh`
4. `4-example`
5. `fork`
6. `change_bucket`
7. `order` z parametrem `749286135` (oczekiwane wyjście: `749286135|123456789`)
8. `order` z parametrem `111222333444` (oczekiwane wyjście: `111222333444|123412341234`)

Testy 1-6 są warte po 0,5 punktu, testy 7-8 po 0,25 punktu. Każdy z podtestów w `compare.c` jest wart 0,3 punktu.
Za usterki (np. wieszanie się systemu, ostrzeżenia przy kompilacji testów, brak stałych w łatce) odjęte punkty
są zawarte w opisie. Punkty na koniec były zaokrąglane do wielokrotności 0,1.

**Uwaga**: uruchamianie wszystkich testów na pojedynczym rozwiązaniu może trwać nawet kilkanaście minut.
