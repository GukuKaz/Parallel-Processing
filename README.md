# Algorithmen und Programmierung 4

Bitte wenn Code compiliert wird die entstandenen Dateien in einen Unterordner ./bin/ einfügen.\\
Dann wird die ausführbare Datei nicht mit ins git geladen

## Übung 2

Compilieren des Programms:\\
im Terminal in der Ordner ueb02/ gehen und anschließend

```
  "make" oder "make test"
```

eingeben. Dadurch wird automatisch der Ordner ./bin/ erstellt und dort die Output-Dateien ausgegeben.\\
Anschließend kann das Testprogramm aufgerufen werden durch

"bin/test" oder alternativ "bin/test <Anzahl der Iterationen>"

Damit werden ALLE Programme der Aufgaben getestet mit entsprechend vielen Durchläufen wie in <Anzahl der Iterationen> angegeben (default: 100.000 Iterationen)

### Aufgabe 2
Aufgabe 2 wurde abgesichert mit zweifacher Schlossvariable mit wechselseitigen Zutritt nach Dekker (Vorlesungsfoliensatz 5 Parallelisierung Folie 20)

### Aufgabe 3
Aufgabe 3 wurde abgesichert mit vielfacher Schlossvariable mit Tickets nach Lamport's Bakery (Vorlesungsfoliensatz 5 Parallelisierung Folie 27)

## Übung 3

### Aufgabe 2

Bei nacheinander Ausführen auf einem Computer mit dem Betriebssystem Ubuntu 18.04.2 LTS und einem Intel® Core™ i5-7200U CPU @ 2.50GHz × 4 Prozessor
- Übung 2 Aufgabe 3: 0 Unfälle bei 500,000 Iterationen mit 5 Threads in 5177s
- Übung 3 Aufgabe 1: 0 Unfälle bei 500,000 Iterationen mit 5 Threads in 199s


## Übung 4

### Aufgabe 2

Ja, da, wenn die Philosophen denken, immer beide Gabeln versucht werden zu
nehmen und wenn dies bei einer Gabel nicht geht, die bisher genommenen Gabeln
nicht zurückgegeben werden (Zeile 26-31). Damit kann es vorkommen, dass alle
Philosophen die von ihnen aus linken bzw. alle die von ihnen aus rechten
Gabeln nehmen und somit keiner mehr vorankommt (deadlock).
# Parallel-Processing
