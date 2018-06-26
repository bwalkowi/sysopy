# Potoki nazwane i nienazwane

## Zadanie 1

Napisz program, który wykorzystując potoki nienazwane realizuje to samo przekształcenie tekstu co polecenie

```bash
tr '[:lower:]' '[:upper:]' | fold -w N
```

gdzie N to liczba podana jako argument wywołania programu. Zadanie wykonaj za pomocą niskopoziomowej obsługi potoków (pipe, przydatna funkcja dup2).

## Zadanie 2

Zdefiniuj dwa  programy  klienta wrfifo i serwera rdfifo. Serwer tworzy ogólnodostępną kolejkę FIFO (potok nazwany) o nazwie podanej jako argument wywołania. Następnie program czeka na komunikaty zapisywane do klientów i po ich odczytaniu wyświetla je na ekranie w postaci:
Godzina odczytu -   PID procesu klienta - godzina zapisu - treść komunikatu.
Program klienta wrfifo w argumencie wywołania otrzymuje nazwę kolejki FIFO, następnie wczytuje w pętli z klawiatury treść kolejnych wiadomości i poprzez kolejkę FIFO wysyła do serwera odpowiedni komunikat postaci PID procesu klienta - godzina zapisu - treść komunikatu.

## Zadanie 3

Za pomocą funkcji popen napisz program, który realizuje polecenie

```bash
ls -l | grep ^d > OUTPUT
```

gdzie OUTPUT to argument wywołania programu.
