## Zadanie 1. Tworzenie procesów (fork, vfork, clone)

Napisz program, który sprawdza wydajność systemu w zakresie tworzenia procesów. Proces macierzysty powinien wykonać N cykli, w których wykonuje dwie operacje, pierwsza to utworzenie nowego procesu, druga to oczekiwanie na jego zakończenie.
Do tworzenia nowych operacji wykorzystaj cztery metody:

<ol type="a">
  <li>użycie funkcji fork</li>
  <li>użycie funkcji vfork</li>
  <li>użycie funkcji clone z odpowiednimi opcjami, rozwiązanie odpowiadające funkcji fork</li>
  <li>użycie funkcji clone z odpowiednimi opcjami, rozwiązanie odpowiadające funkcji vfork</li>
</ol>

Program powinien przed wykonaniem cykli zdefiniować zmienną globalną licznik z wartością początkową równą 0.
Proces potomny zaraz po utworzeniu powinien wykonać dwie operacje: zwiększyć wartość zmiennej licznik o 1 i wywołać funkcje _exit.
Po zakończeniu cykli proces macierzysty powinien wypisać wartość zmiennej licznik.

Przeprowadź pomiary czasów (rzeczywistego, użytkownika i systemowego) zarówno dla procesu macierzystego jak i dla wszystkich procesów potomnych (łącznie) w każdym z czterech wariantów: a), b), c) i d). Testy przeprowadź dla czterech różnych wartości N, tak by czasy rzeczywiste procesu macierzystego były w zakresie od kilku sekund do około minuty.
Wyniki uzyskanych pomiarów wraz wykresami i komentarzem przekaż w pliku wyniki.pdf

Wykresy powinny pokazywać zależność czasów (wydajności) od wartości N dla metod od a) do d), poprzez oddzielne wykresy dla:
- czasu systemowego
- czasu użytkownika
- sumy czasu użytkownika i systemowego
- czasu rzeczywistego
Na pojedynczym wykresie przedstawiamy wyniki dla czterech analizowanych metod - od a) do d), wykres ten tworzymy w trzech wariantach:
w1) tylko czas procesu macierzystego
w2) tylko łączny czas procesów potomnych
w3) sumaryczny czas w1) i w2)
Zatem otrzymujemy 12 (4 x 3) wykresów.

## Zadanie 2. Drzewo procesów zgodne z drzewem katalogów

Napisz program fcounter, który w zależności od liczby podkatalogów pierwszego poziomu znajdujących się w katalogu (wariant do wyboru)

- wskazanym jako argument programu (ścieżka względna lub bezwzględna, domyślnie w katalogu bieżącym), 

- (*) zdefiniowanym jako zmienna środowiskowa PATH_TO_BROWSE, (ścieżka względna lub bezwzględna do katalogu, w przypadku braku zdefiniowanej zmiennej w katalogu bieżącym),

wygeneruje odpowiednią liczbę procesów potomnych (jeden proces dla jednego podkatalogu), a każdy z tych procesów wykona przez odpowiednią funkcję z rodziny exec program fcounter (rekurencja).

Jeżeli w katalogu nie ma podkatalogów to proces programu fcounter przekazuje do procesu macierzystego ilość plików w swoim katalogu poprzez funkcję exit. Jeżeli istnieją podkatalogi to czeka na zakończenie procesów potomnych i przekazuje liczbę swoich plików powiększoną o wartości uzyskane od tych procesów.
Każdy proces opcjonalnie (argument -v wywołania programu fcounter lub jego brak) wypisuje ścieżkę do katalogu, który obsługuje, liczbę swoich plików oraz liczbę sumaryczną plików swoich i tych otrzymanych z procesów potomnych.

Należy pamiętać, aby po każdym rekurencyjnym wywołaniu w zależności od wybranego wariantu odpowiednio zmodyfikować zmienną środowiskową lub argument wywołania programu.

(*) Program powinien uwzględniać dodatkowo, drugą zmienną środowiskową EXT_TO_BROWSE, której wartość decyduje o uwzględnianych plikach, np. gdy EXT_TO_BROWSE=c to analizowane powinny być tylko pliki z takimi rozszerzeniami. Brak tej zmiennej lub jej wartości oznacza uwzględnianie wszystkich plików.

Zakładamy, że liczba plików w całym drzewie jest odpowiednio mała, tak, że nie nastąpi przepełnienie zakresu używanej liczby.
Wymagane jest, aby proces macierzysty pierwsze utworzył wszystkie procesy dla podkatalogów, a dopiero później oczekiwał na zwrócone przez nie wartości.
Dla celów testowych wprowadź też opcjonalny argument programu -w , który spowoduje, że każdy proces przed oczekiwaniem na procesy potomne (jeśli ich nie tworzył to przed wywołaniem funkcji exit) wywoła funkcję sleep dla np. 15 sekund. W tym czasie w drugim terminalu wywołaj polecenie ps -HC fcounter (lub pstree z pidem pierwszego procesu), aby zobaczyć utworzone drzewo procesów.

