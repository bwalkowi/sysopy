# Zarządzanie pamięcią, biblioteki, pomiar czasu 

## Zadanie 1. Lista dwukierunkowa

Zaprojektuj i przygotuj prosty zestaw funkcji (bibliotekę) dla list dwukierunkowych przechowujących struktury z danymi o kontaktach, umożliwiającą:

- tworzenie listy, usuwanie listy,

- dodanie elementu do listy, usunięcie elementu z listy,

- wyszukiwanie elementu na liście, sortowanie listy.

Pola elementu listy (struktury): Imię, Nazwisko, Data urodzenia, email, telefon, adres.

Przygotuj plik Makefile, zawierający polecenia kompilujące pliki źródłowe biblioteki oraz tworzące biblioteki w dwóch wersjach: statyczną i dzieloną.

## Zadanie 2. Program korzystający z biblioteki

Napisz przykładowy program testujący działanie funkcji z biblioteki zadania 1. W programie wybierz co najmniej 4 punkty kontrolne (wśród których jest początek i koniec programu). Dla każdego z tych punktów kontrolnych wypisz informacje o aktualnym czasie wykonania (rzeczywistym, użytkownika, systemowym), opisujące dany punkt kontrolny: różnice czasów w odniesieniu do pierwszego i poprzedniego punktu kontrolnego.

Przygotuj plik Makefile, zawierający polecenia kompilujące programy na trzy sposoby:
- z wykorzystaniem bibliotek statycznych,
- z wykorzystaniem bibliotek dzielonych (dynamiczne, ładowane przy uruchomieniu programu),
- z wykorzystaniem bibliotek ładowanych dynamicznie (dynamiczne, ładowane przez program),
oraz uruchamiający testy.