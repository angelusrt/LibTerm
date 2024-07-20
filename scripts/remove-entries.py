import csv

postfixes = (
    "st",
    "s",
    "che",
    "chen",
    "nde",
    "nden",
    "te",
    "ten",
    "em",
    "ere",
    "eren",
    "erer",
    "ster",
    "gt",
    "bt",
    "mt",
)

prefixes = (
    "Mit",
    "Miß",
    "Ab",
    "ein",
    "ge",
    "um",
    "ver",
    "vor",
    "zu",
    "nach",
    "weg",
    "über",
    "Über",
)

file_read = open("../state/german-words-sorted.txt", mode="r")
data = csv.reader(file_read)
words = list(data)
words_alt: list[str] = []

word_comp = "sanduiche"
word_comp_len = 0
for i, word in enumerate(words):
    word = word[0]

    prefix = word.startswith(prefixes)
    postfix = word.endswith(postfixes)
    word_len = len(word)
    if prefix or postfix or word_len > 10 or word.isupper():
        continue

    comparison = word.find(word_comp)
    diff_len = word_len-word_comp_len

    if comparison == -1 or diff_len > 4 or diff_len <= 0:
        words_alt.append(word)
        word_comp = word
        word_comp_len = len(word)

file_write = open("../state/german-words-normalized.txt", mode="w")
for i, word in enumerate(words_alt):
    if i % 10000 == 0: 
        print("evaluated ", i)
        print("--")

    file_write.write(word+"\n")

file_write.close()
