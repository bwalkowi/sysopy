# IPC - pamieć wspólna, semafory

### Przydatne funkcje:

#### System V:

`<sys/shm.h> <sys/ipc.h> - shmget, shmclt, shmat, shmdt`

#### POSIX:

`<sys/mman.h> - shm_open, shm_close, shm_unlink, mmap, munmap`

## Zadanie 1

Wykorzystując semafory i pamięć wspólną z IPC Systemu V napisz program rozwiązujący problem producentów i konsumentów. Należy napisać dwa odrębne programy dla producenta i konsumenta. Po uruchomieniu, producent cyklicznie generuje zadania do wykonania. Tworząc zadanie producent losuje dodatnią liczbę całkowitą i umieszcza ją w tablicy przechowywanej w pamięci wspólnej. Celem zadania jest sprawdzenie, czy liczba ta jest parzysta. Po utworzeniu nowego zadania producent wypisuje na ekranie komunikat postaci:

`(pid timestamp) Dodałem liczbę: x na pozycję i. Liczba zadań oczekujących: m.`

gdzie: 
* ``pid`` to PID procesu producenta,  
* ``timestamp`` to aktualny czas (z dokładnością do milisekund),  
* ``x`` to wylosowana liczba (treść zadania)  
* ``m`` to liczba zadań w pamięci wspólnej (licząc z utworzonym zadaniem)  
* ``i`` - indeks wpisywanej liczby.

Konsument cyklicznie pobiera zadania umieszczone w pamięci wspólnej. Po pobraniu zadania konsument sprawdza, czy dana liczba jest parzysta i wypisuje na ekranie jeden z komunikatów:

`(pid timestamp) Sprawdziłem liczbę x na pozycji i - parzysta. Pozostało zadań oczekujących: m`

`(pid timestamp) sprawdziłem liczbę x na pozycji i - nieparzysta. Pozostało zadań oczekujących: m`

gdzie: 
* ``pid`` to PID procesu konsumenta,  
* ``timestamp`` to czas pobrania zadania (z dokładnością do milisekund),  
* ``x`` to liczba będąca treścią zadania  
* ``i`` -pozycja w tablicy  
* ``m`` to liczba zadań które pozostały w pamięci wspólnej (po pobraniu wykonanego zadania).  

Zakładamy, że równocześnie pracuje wielu producentów i wielu konsumentów. Można napisać program demonstrujący działanie dla m konsumentów i n producentów, uruchamiający odpowiednią liczbę procesów, gdzie m i n są argumentami wywołania programu.
Rozmiar tablicy z zadaniami (w pamięci wspólnej) jest ograniczony i ustalony na etapie kompilacji. Tablica ta indeksowana jest w sposób cykliczny - po dodaniu zadania na końcu tablicy, kolejne zadania dodawana są od indeksu 0.
Korzystając w odpowiedni sposób z semaforów należy zagwarantować, że liczba oczekujących zadań nie przekroczy rozmiaru tablicy oraz że określony element tablicy nie będzie modyfikowany przez kilka procesów równocześnie. Rozmiar tablicy zadań dobierz tak, aby mogła zajść sytuacja, w której tablica jest całkowicie zapełniona. W pamięci wspólnej oprócz tablicy można przechowywać także inne dane dzielone pomiędzy procesami.

## Zadanie 2.

Wykorzystując mechanizmy IPC - POSIX (pamięć wspólna, semafory) zaimplementuj poprawne rozwiązanie problemu czytelników i pisarzy przy następujących założeniach:

- jest wiele procesów czytelników i wiele procesów pisarzy,
- dla uproszczenia można przyjąć, że liczba czytelników jest jednak ograniczona,
- do synchronizacji używane są semafory,
- dane umieszczone w pamięci wspólnej to tablica liczb całkowitych,
- pisarz cyklicznie modyfikuje w sposób losowy wybrane liczby w tablicy (losuje ilość liczb do modyfikacji, ich pozycje w tablicy oraz wartość liczby wpisywanej za pierwszym razem, przy czym każdą kolejną zwiększa o 1 aż do górnego zakresu liczb losowych)
- czytelnik uruchamiany jest z jednym argumentem x - wartością poszukiwanej w tablicy liczby i cyklicznie wykonuje operację przeszukiwania tablicy
- pisarz raportuje na standardowym wyjściu wykonanie swojej operacji (modyfikacji tablicy)

    `(pid timestamp) Wpisałem liczbę x - na pozycję i. Pozostało m zadań`
    
    gdzie:  
    * ``pid`` to PID procesu pisarza,
    * ``timestamp`` to aktualny czas (z dokładnością do milisekund),
    * ``x`` to wylosowana liczba do wpisania do tablicy,
    * ``i`` - pozycja w tablicy a m to liczba pozostałych modyfikacji w pamięci wspólnej
- analogiczny raport wykonuje czytelnik, dodając dodatkowo wynik operacji: ilość znalezionych liczb o szukanej wartości.

    `(pid timestamp) znalazłem n liczb o wartości x`
    
    gdzie:
    * ``pid`` to PID procesu czytelnika,  
    * ``timestamp`` to czas pobrania zadania (z dokładnością do milisekund),
    * ``n`` - liczba znalezionych dotychczas wartości x
    * ``x`` to liczba będąca treścią zadania.
- opcja -u dla czytelnika wypisze tylko zbiorczy raport zawierający informacje, ile znaleziono liczb o wartości x.
- można napisać dodatkowy program, który będzie symulował działanie m czytelników i n pisarzy dla wartości losowych.
