# IPC - kolejki komunikatów

### Przydatne funkcje:

#### System V:

`<sys/msg.h> <sys/ipc.h> - msgget, msgctl, msgsnd, msgrcv, ftok`

#### POSIX:

`<mqueue.h> - mq_open, mq_send, mq_receive, mq_getattr, mq_setattr, mq_close, mq_unlink, mq_notify`

## Zadanie 1. System V

Celem zadania jest napisanie prostego programu typu klient-serwer, w którym komunikacja zrealizowana jest za pomocą kolejek komunikatów.

Serwer po uruchomieniu tworzy nową kolejkę komunikatów systemu V. Za pomocą tej kolejki klienci będą wysyłać komunikaty do serwera. Ścieżkę oraz id identyfikujące kolejkę (patrz funkcja ftok) serwer otrzymuje w argumentach wywołania. Klient bezpośrednio po uruchomieniu tworzy kolejkę z unikalnym kluczem IPC (patrz IPC_PRIVATE) i wysyła jej klucz komunikatem do serwera (ścieżkę i id kolejki serwera klient otrzymuje w argumentach wywołania). Po otrzymaniu takiego komunikatu, serwer otwiera kolejkę klienta, przydziela klientowi identyfikator (np. numer w kolejności zgłoszeń) i odsyła ten identyfikator do klienta (komunikacja w kierunku serwer->klient odbywa się za pomocą kolejki klienta). Po otrzymaniu identyfikatora klient rozpoczyna wykonywanie zadań zleconych przez serwer (w pętli). Wykonanie pojedynczego zadania przebiega następująco:

- klient wysyła serwerowi komunikat oznaczający gotowość do przyjęcia nowego zadania; w treści komunikatu klient umieszcza swój identyfikator,

- serwer odsyła klientowi komunikat zawierający losową liczbę całkowitą większą od zera,

- klient sprawdza czy otrzymana liczba jest pierwsza (można użyć dowolnego algorytm sprawdzającego czy liczba jest pierwsza),

- klient wysyła serwerowi komunikat zawierający jego identyfikator, sprawdzoną liczbę oraz informację, czy jest to liczba pierwsza. Jeśli liczba jest pierwsza serwer wypisuje na standardowe wyjście komunikat postaci: Liczba pierwsza: X (klient: Y), gdzie X to sprawdzona liczba a Y to identyfikator klienta.

Poszczególne rodzaje komunikatów należy identyfikować za pomocą typów komunikatów systemu V. Klient i serwer należy napisać w postaci osobnych programów (nie korzystamy z funkcji fork). Serwer musi być w stanie pracować z wieloma klientami naraz. Przed zakończeniem pracy każdy proces powinien usunąć kolejkę którą utworzył (patrz IPC_RMID oraz funkcja atexit). Dla uproszczenia można przyjąć, że serwer przechowuje informacje o klientach w statycznej tablicy (rozmiar tablicy ogranicza liczbę klientów, którzy mogą się zgłosić do serwera).

## Zadanie 2. POSIX

Zrealizuj zadanie alogiczne do Zadania 1 wykorzystując kolejki komunikatów POSIX.

Kolejka klienta powinna mieć losową nazwę zgodną z wymaganiami stawianymi przez POSIX. Na typ komunikatu można zarezerwować pierwszy bajt jego treści. Przed zakończeniem pracy klient wysyła do serwera komunikat informujący, że serwer powinien zamknąć po swojej stronie kolejkę klienta. Następnie klient zamyka i usuwa swoją kolejkę. Serwer przed zakończeniem pracy zamyka wszystkie otwarte kolejki i usuwa kolejkę, którą utworzył.
