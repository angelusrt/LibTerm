import csv

file_read = open("../state/german_words.txt", mode="r")
data = csv.reader(file_read)
data_list = list(data)
data_list.sort()

file_write = open("../state/german-words-sorted.txt", mode="w")
for i, word in enumerate(data_list):
    if i % 100 == 0: 
        print("evaluated ", i)
        print("--")

    file_write.write(word[0]+"\n")

file_write.close()
