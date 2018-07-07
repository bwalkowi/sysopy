### Zapoznaj się ze składnią i operacjami wykonywanymi przez poniższe funkcje:

    funkcje operujące na plikach i katalogach: open, close, read, write, fcntl, stat, fstat, mkdir, rmdir, opendir, closedir, readdir, rewinddir, ftw, fopen, fclose, getc, putc,
    funkcje i zmienne do obsługi błędów: perror, errno.

## Zadanie 1. Porównanie wydajności systemowych i bibliotecznych funkcji we/wy

Celem zadania jest napisanie programu porównującego wydajność systemowych i bibliotecznych funkcji wejścia/wyjścia. Zakładamy, że program operuje na pliku przechowującym zbiór pewnych rekordów. Dla uproszczenia, rekordem w zadaniu będzie ciąg (tablica) bajtów. Wszystkie rekordy mają taką samą długość. Po otwarciu pliku  program sortuje znajdujące się w nim rekordy za pomocą sortowania przez wstawianie. Program może przechowywać w pamięci tylko dwa rekordy równocześnie - rekord obecnie "wstawiany", oraz rekord, z którym jest porównywany. Po wykonaniu porównania rekord, z którym obecnie wstawiany był porównywany musi zostać z powrotem zapisany w odpowiednie miejsce pliku. Jako klucz sortowania przyjmujemy pierwszy bajt rekordu (interpretowany jako liczba bez znaku).
Program sortujący powinien być napisany w dwóch wariantach:

- przy użyciu funkcji systemowych: read i write,
- przy użyciu funkcji bibliotecznych: fread i fwrite.

Program powinien przyjmować trzy argumenty wywołania: ścieżkę do pliku z rekordami, długość (w bajtach) pojedynczego rekordu oraz wariant funkcji, których ma używać (lib - biblioteczne, sys - systemowe)

Należy również napisać program generujący plik z rekordami. Program ten przyjmuje w argumentach: nazwę pliku do wygenerowania, rozmiar pojedynczego rekordu i liczbę rekordów. Zawartość generowanych rekordów powinna być losowa (można wykorzystać np. funkcję rand).

Dla obu wariantów implementacji przeprowadź pomiary czasu użytkownika i czasu systemowego operacji sortowania. Testy wykonaj dla następujących rozmiarów rekordu: 4, 512, 4096 i 8192 bajty. Dla każdego rozmiaru rekordu wykonaj dwa testy różniące się liczbą rekordów w sortowanym pliku. Liczby rekordów dobierz tak, by czas sortowania mieścił się w przedziale od kilku do kilkudziesięciu sekund. Porównując dwa warianty implementacji należy korzystać z identycznego pliku do sortowania (po wygenerowaniu, a przed sortowaniem, utwórz jego kopię). Zmierzone czasy sortowania zestaw w pliku wyniki.txt. Do pliku dodaj komentarz podsumowujący wnioski z testów.

## Zadanie 2. Operacje na strukturze katalogów

Napisz program przyjmujący dwa argumenty wywołania: ścieżkę do katalogu w systemie plików oraz wzór praw dostępu do pliku. Po uruchomieniu, program przeszukuje drzewo katalogów zakorzenione w ścieżce przekazanej w pierwszym argumencie i szuka w nim plików z prawami dostępu takimi, jak podane w drugim argumencie. Program wypisuje na ekranie listę znalezionym plików. Przy każdym znalezionym pliku program wypisuje również rozmiar pliku w bajtach i datę ostatniego doń dostępu. Program powinien wyszukiwać tylko pliki zwyczajne (bez dowiązań, urządzeń blokowych, itp).

Program należy zaimplementować w dwóch wariantach:

- Korzystając z funkcji opendir, readdir oraz stat. Program powinien wypisywać ścieżki plików względem katalogu, od którego rozpoczynane jest wyszukiwanie.
- Korzystając z funkcji nftw. Program powinien wypisywać bezwzględne ścieżki do znalezionych plików.

## Zadanie 3. Ryglowanie plików

Napisz program umożliwiający w trybie interaktywnym (tekstowym) wykonanie następujących operacji dla pliku będącego jego argumentem:

- ustawienie rygla do odczytu na wybrany znak pliku,
- ustawienie rygla do zapisu na wybrany znak pliku,
- wyświetlenie listy zaryglowanych znaków pliku (z informacją o numerze PID procesu będącego właścicielem rygla oraz jego typie - odczyt/zapis),
- zwolnienie wybranego rygla,
- odczyt (funkcją read) wybranego znaku pliku,
- zmiana (funkcją write) wybranego znaku pliku.

Wybór znaku we wszystkich przypadkach polega na podaniu numeru bajtu w pliku. Do ryglowania należy wykorzystać funkcję fcntl i strukturę flock. Wykonaj eksperymenty uruchamiając program jednocześnie w dwóch terminalach dla tego samego pliku. Próbuj ryglować (do odczytu lub do zapisu) te same znaki pliku i modyfikować (lub odczytywać) wybrane znaki pliku. Zwróć uwagę, że domyślnie ryglowanie działa w trybie rygli zalecanych (advisory), a nie rygli wymuszanych (mandatory).
