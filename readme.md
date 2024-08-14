LinTerm
--
Descrição: aplicação de terminal para aprendizado de linguagem escrito em C.

Em "state/" é necessário prover um arquivo chamado "dictionary.csv" conténdo uma lista de palavras e atributos conforme a seguinte descrição por linha:

    "palavra=frequência=artigo=tipo-gramatical=definição=exemplo"

O separador usado, é: "=".

Compilação recomendada: gcc -Wall -Wextra -Wpedantic main.c -o main -lm.

todo:
- criar inteligence.csv
- reimplementar set
- ...

arquitetura:
```mermaid
stateDiagram
    [*] --> model
    model --> draw
    draw --> listen
    listen --> controller
    controller --> model
    controller --> [*]
```

estado:
```mermaid
classDiagram 
    class state{
        +vector dict_lines 
        +string *dicts 
        +dictionaries_status dict_stats 
        +size_t dict_cursor 

        +vector note_lines 
        +string *notes 
        +notes_status notes_stats 
        +size_t note_cursor 

        +bool is_menu 
        +bool redraw 

        +ushort page 
        +char option 

        +vector *lines 
        +size_t *cursor 
    }
```

páginas:
```mermaid
flowchart TB
    draw --> menu & note & dictionary
``` 

controlador:
```mermaid
flowchart TB;
    controller --> next;
    controller --> previous;
    controller --> notes_toggle;
    controller --> menu_toggle;
    controller --> add_note;
    controller --> insert_note_in_dictionary;
    controller --> insert_note_in_notes;
    controller --> quit;
    controller --> listen;
``` 

