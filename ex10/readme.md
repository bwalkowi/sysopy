Celem zadania jest napisanie prostego komunikatora w architekturze klient/serwer. Bezpośrednio po uruchomieniu klient tworzy nowy wątek, który powinien obsługiwać wejście/wyście z klawiatury. Wątek główny obsługuje natomiast komunikację z serwerem. Po wczytaniu wiadomości z klawiatury klient wysyła ją do serwera. Wraz z wiadomością klient wysyła również identyfikator użytkownika nadającego wiadomość. Po otrzymaniu wiadomości od klienta serwer rozsyła ją do wszystkich pozostałych klientów. Po otrzymaniu wiadomości od serwera klient wypisuje na ekranie identyfikator użytkownika, który ją wysłał oraz treść wiadomości. Zakładamy, że wiadomościami w systemie są napisy zakończone znakiem nowej linii. Można przyjąć górne ograniczenie na długość napisu (na przykład 256 znaków). W przypadku, gdy klient jest uruchomiony na tym samym komputerze co serwer, komunikacja powinna być prowadzona za pomocą soketu z dziedziny Unixa. Klienci uruchomieni na innych komputerach komunikują się z serwerem soketem z dziedziny Internet. Serwer powinien być napisany jako aplikacja jednowątkowa/jednoprocesowa i obsługiwać równocześnie sokety z obu dziedzin.

Należy zadbać, aby zarówno serwer jak i klient nie zużywały więcej zasobów (np. CPU) niż jest to konieczne. Ponadto, komunikacja w systemie powinna być w pełni asynchroniczna. Niedopuszczalne jest więc rozwiązanie, w którym klient wykonuje operacje nieblokujące w sposób zużywający nadmiernie czas procesora. Niedopuszczalne jest również ograniczenie zużycia CPU poprzez zasypianie (np. funkcją sleep) serwera lub klienta - rozwiązanie to nie jest w pełni asynchroniczne. Prawidłowa implementacja zadania powinna wykorzystywać funkcje z rodziny select/poll. W kliencie można dodatkowo wykorzystać sygnały do komunikacji zdarzeń pomiędzy wątkami.

Klient przyjmuje następujące argumenty wywołania:

- identyfikator użytkownika (napis alfanumeryczny),
- typ serwera: lokalny lub zdalny,
- w przypadku serwera zdalnego: adres IP oraz port serwera,
- w przypadku serwera lokalnego: ścieżkę w systemie plików do soketu serwera.

Serwer przyjmuje w argumentach wywołania numer portu sieciowego oraz ścieżkę w systemie plików do soketu z dziedziny Unix.

Komunikator należy zaimplementować w dwóch wymienionych poniżej wariantach.

## Zadanie 1

Klienci i serwer używają protokołu datagramowego a komunikacja jest bezpołączeniowa. Serwer używa dokładnie jednego soketu z dziedziny Unix oraz dokładnie jednego soketu z dziedziny Internet. Serwer rejestruje nowego klienta gdy otrzyma od niego pierwszy komunikat do rozesłania. Rejestracja jest ważna przez określony okres czasu (np. 30 sekund) i jest automatycznie odnawiana gdy klient wyśle serwerowi kolejny komunikat. Serwer powinien co pewien czas sprawdzać listę klientów i usuwać z niej klientów, których okres rejestracji wygasł.

## Zadanie 2

Klienci i serwer używają protokołu strumieniowego i komunikacji połączeniowej. Klient rejestruje się w serwerze poprzez nawiązanie z nim połączenia. Jeśli w określonym czasie od nawiązania połączenia (np. 30 sekund) klient nie wyśle żadnego komunikatu, serwer zamyka połączenie z nim i usuwa go z listy klientów.