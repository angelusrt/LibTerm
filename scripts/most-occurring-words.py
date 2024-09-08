import csv

file_read = open("../state/german-words-normalized.txt", mode="r")
data = csv.reader(file_read)
words = list(data)
words_alt: list[tuple[int, str]] = []

word_comp = "sanduiche"
word_comp_len = 0
word_count = 0
for word in words:
    word = word[0]

    if len(word) <= 2:
        continue

    comparison = word.find(word_comp)
    word_len = len(word)
    diff_len = word_len-word_comp_len

    if comparison == -1:
        if word_count > 0:
            words_alt.append((word_count, word_comp))

        word_comp = word
        word_comp_len = len(word)
        word_count = 0
    else:
        word_count = word_count + 1

words_alt.sort(reverse=True)
file_write = open("../state/german-words-most-ocurring.txt", mode="w")
for i, word in enumerate(words_alt):
    if i % 100 == 0: 
        print("evaluated ", i)
        print("--")

    file_write.write(str(word[0])+", "+word[1]+"\n")

file_write.close()
