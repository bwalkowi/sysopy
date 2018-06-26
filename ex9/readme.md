## Zadanie 1

Zaimplementuj wielowątkowy program realizujący poprawne rozwiązanie problemu pięciu filozofów. Należy zastosować rozwiązanie wykorzystujące semafory nienazwane standardu POSIX.

## Zadanie 2

Zaimplementuj z wykorzystaniem monitorów poprawne rozwiązanie problemu lotniskowca:

Lotniskowiec ma pokład o pojemności N samolotów oraz pas startowy. Pas startowy jest konieczny do startowania i lądowania samolotów, a może z niego korzystać w danej chwili tylko jeden samolot. Gdy liczba samolotów na lotniskowcu jest mniejsza niż K (0 < K < N), priorytet w dostępie do pasa startowego mają samoloty lądujące, w przeciwnym razie startujące.
Napisać należy algorytm samolotu, działającego w pętli według następującego schematu:

- start
- lot
- lądowanie
- postój

Lot i postój zaimplementować należy jako oczekiwanie przez pewną losową ilość czasu. Samolotów może być więcej niż N, wtedy część z nich zawsze będzie w powietrzu. Priorytet X nad Y oznacza, że dopóki są samoloty oczekujące na X, żaden z tych oczekujących na Y nie zostanie dopuszczony do wykonania pożądanej operacji.
