# Sygnały

```
Rodzaje sygnałów: SIGINT, SIGQUIT, SIGKILL, SIGTSTP, SIGSTOP, SIGTERM, SIGSEGV, SIGHUP, SIGALARM, SIGCHLD, SIGUSR1, SIGUSR2
Sygnały czasu rzeczywistego: SIGRTMIN, SIGRTMIN+n, SIGRTMAX
Przydatne polecenia Unix: kill, ps
Przydatne funkcje systemowe: kill, raise, sigqueue, signal, sigaction, sigemptyset, sigfillset, sigaddset, sigdelset, sigismember, sigprocmask, sigpending, pause, sigsuspend
```

## Zadanie 1

Napisz program printer wypisujący na standardowe wyjście zadany jako argument programu  wiersz tekstu. Po odebraniu sygnału SIGTSTP program zaczyna wypisywać zadany tekst wspak. Po ponownym wysłaniu SIGTSTP program powraca do normalnego wypisywania tekstu. Po trzecim wysłaniu sygnalu SIGTSTP program powinien wypisywac zadany tekst dwukrotnie, po czwartym wyslaniu sygnaluSIGTSTP program powinien wypisać zadany tekst dwukrotnie wspak itd. Maksymalna liczba zwielokrotniania wypisania tekstu (max-num) powinna być parametrem przekazanym w argumencie wywołania programu. Po osiągnięciu wartości max-num liczba zwielokrotnienia tekstu maleje o 1, a następnie po osiągnięciu 1 znowu rośnie. Program powinien umożliwiać wielokrotne przechodzenie przez powyższy cykl (sygnałem SIGTSTP). Program musi również obsługiwać sygnał SIGINT. Po jego odebraniu program wypisuje komunikat "Odebrano sygnał SIGINT" i kończy działanie. W kodzie programu, do przechwycenia sygnałów użyj zarówno funkcji signal, jak i sigaction (np. SIGINT odbierz za pomocą signal, a SIGTSTP za pomocą sigaction) .

## Zadanie 2

<ol type="a">
  <li> Napisz dwa programy: sender program wysyłający sygnały SIGUSR1 i  catcher - program zliczający ilość odebranych sygnałów. Ilość sygnałów SIGUSR1wysyłanych przez pierwszy program powinna być określana w parametrze wywołania tego programu. Po transmisji sygnałów SIGUSR1 sender powinien wysłać sygnał SIGUSR2, po otrzymaniu którego catcher wysyła do sendera tyle sygnałów SIGUSR1, ile sam ich otrzymał a „transmisję” kończy wysłaniem sygnału SIGUSR2 i zakończeniem działania. Program sender po otrzymaniu sygnału SIGUSR2 wyświetla komunikat o ilości otrzymanych sygnałów SIGUSR1 oraz o tym, ile powinien ich otrzymać i kończy działanie.</li> 

  <li> Do programów w zadaniu 1 dodaj potwierdzenie odbioru sygnału po każdorazowym ich odebraniu przez program catcher. W tym celu, niech catcher wysyła do sendera sygnał SIGUSR1 informujący o odbiorze sygnału. Sender powinien wysłać kolejny sygnał dopiero po uzyskaniu tego potwierdzenia. Zapewnij rozwiązanie, w którym ilość sygnałów odebranych jest zgodna z ilością sygnałów wysłanych, i w którym nie dochodzi do zakleszczenia.</li> 

  <li> Zmodyfikuj programy opisane w zadaniu 1 tak, by przesyłany sygnał był sygnałem czasu rzeczywistego (patrz man -s 7 signal). Jaka liczba sygnałów będzie teraz odebrana?</li>
</ol>
