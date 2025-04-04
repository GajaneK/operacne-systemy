# Domáca úloha 1

Vašou úlohou je naprogramovať síce zjednodušenú, ale viacvláknovú verziu UNIXového programu `cat`. Účelom programu je všetky dáta, ktoré dostane na štandardný vstup (a žiadne iné), vypísať na štandardný výstup v presne tom istom poradí. Keď sa vstup skončí, program má korektne dovypisovať všetky zostávajúce dáta na štandardný výstup a skončiť.


### Požiadavky na program

Odovzdaný program musí používať multithreading s aspoň 2 vláknami (môže ich byť viac) --- minimálne jedno vlákno obsluhuje načítavanie zo vstupu a druhé výpis na štandardný výstup. Tieto vlákna si nejakým spôsobom koordinovane a _efektívne_ (bez plytvania systémovými prostriedkami) odovzdávajú dáta. Akékoľvek formy [činného čakania (busy waiting)](https://josephmate.github.io/2016-02-04-how-to-avoid-busy-waiting/) sú vylúčené, nemali by ste ani použiť funkcie ako `sleep`.

Všetky dáta, ktoré prišli na štandardný vstup programu, sa musia v (primeranom) konečnom čase a nezmenené dostať na štandardný výstup. Množstvo pamäti, ktoré váš program použije, musí byť zhora ohraničené konštantou (nesmie rásť do nekonečna s veľkosťou vstupu --- musíte sa vyhnúť situácii, že najprv prečítate celý vstup a potom ho zapíšete na výstup).

Vlastný program má byť napísaný v jazyku C a používať iba [štandardnú knižnicu jazyka C](https://en.wikipedia.org/wiki/C_standard_library) a [knižnicu na synchronizovanie vlákien _pthread_](pthread.md). Vyhýbajte sa veciam mimo štandardu POSIX a neprenositeľným doplnkom, ktoré vaša konkrétna implementácia možno ponúka.

Na synchronizáciu vlákien použite `pthread_mutex_lock`, `pthread_mutex_unlock`, `pthread_cond_wait`, `pthread_cond_signal`. Nie je povolené použiť mechanizmy, ktoré to vybavia za vás (napr. `pipe`). Úloha sa dá riešiť s malým počtom mutexov a podmienok (`pthread_cond_t`), premyslite si preto ešte pred písaním samotného programu, či ich nejdete použiť zbytočne veľa (jednoduchší program sa ľahšie ladí).

Na prácu so vstupom a výstupom je vhodné použiť systémové volania [`read`](http://man7.org/linux/man-pages/man2/read.2.html) a [`write`](http://man7.org/linux/man-pages/man2/write.2.html), funkcie vyššej úrovne typu `getc` sú podstatne pomalšie. Pozor: `write` negarantuje zápis celého buffera naraz.


### Kompilácia

Kompilácia vášho programu by mala prebehnúť bez chýb pomocou príkazu

    gcc -std=c99 -Werror=implicit-function-declaration -pthread priezvisko.c -o priezvisko

Odporúčame tiež zapnúť prepínače `-Wall -Wextra -Wpedantic -Wconversion`; neraz pomôžu odhaliť nenápadnú závažnú chybu.

**Aspoň raz spustite program cez nástroj [valgrind](https://valgrind.org/) (odhalí o.i. chybnú prácu s pamäťou).**

Zvážte tiež využitie ďalších nástrojov na kontrolu kódu, napr. [clang static analyzer](https://clang-analyzer.llvm.org/).

Ak nemáte dostupný Linux, môžete použiť študentský server Davinci (davinci.fmph.uniba.sk), na ktorý sa viete pripojiť napr. pomocou PuTTy, alebo [Windows Subsystem for Linux](https://docs.microsoft.com/en-us/windows/wsl/about), prípadne využite Linux cez [virtuálnu mašinu](https://www.makeuseof.com/tag/install-linux-windows-vmware-virtual-machine/).


### Ako to vyskúšať?

Ak chcete otestovať funkčnosť programu z príkazového riadka, najjednoduchší spôsob je vytvoriť si súbor s testovacími dátami (napr. `test.in`) a spustiť:

    ./priezvisko < test.in > test.out

Tento príkaz dá na štandardný vstup vášho programu obsah súboru `test.in` a vytvorí nový subor `test.out`, do ktorého je zachytený celý štandardný výstup. Môžete ich porovnať pomocou `diff`.

Môžete tiež využiť [testovač](tester). Použitie:
1. Naklonujte si repozitár.
2. Umiestnite testované riešenie do adresára `du1/tester/src`.
3. V adresári `du1/tester` spusťte `./testall.sh`.

Testovač nie je úplne spoľahlivý: môže sa stať, že nedokáže váš program násilne ukončiť po presiahnutí časového limitu (napr. ak vyrobíte deadlock). Nebojte sa ho vylepšiť (spravte pull request so zmenami).


### Odovzdávanie a hodnotenie

Programy sa odovzdávajú e-mailom na adresu `jan.mazak@fmph.uniba.sk`, predmet `OS-DU1`, v súbore `<priezvisko>.c` (kde `<priezvisko>` je vaše priezvisko) ako príloha k e-mailu. Odovzdať je možné viackrát (najviac raz denne). Najneskorší možný termín odovzdania je **7. 4. 2025**.

Plný počet bodov dostane program, ktorý za každých podmienok zvládne splniť zadanie úlohy; 0 bodov program, ktorý sa nepodarí donútiť, aby aspoň v nejakom prípade dostatočne spĺňal zadanie. Body medzi sa škálujú podľa rozsahu (ne-)funkčnosti.

Očakáva sa, že program vie spracovať aj vstup veľkosti 1 GB do 10 sekúnd. ("Time limit exceeded" na menších vstupoch zvyčajne znamená chybne implementovanú komunikáciu medzi vláknami.) Stĺpec Testcase udáva veľkosť vstupu. Červené záznamy zodpovedajú chybnému správaniu programu a znamenajú stratu bodov, konkrétne bodové hodnotenie je však prideľované až na konci, keď si odovzdané programy prečítam a pochopím, či sú chyby zásadné alebo len marginálne. (Ak je program napísaný nezrozumiteľne, nemá to okamžitý vplyv na hodnotenie, ale v prípade chýb pravdepodobne bude hodnotený menším počtom bodov, pretože sa mi nepodarí zistiť, čo je v ňom správne a čo nie.)

Oficiálny výstup z testovača bude zverejnený po odovzdaní všetkých domácich úloh na http://dcs.fmph.uniba.sk/~mazak/vyucba/os/results1.html.
