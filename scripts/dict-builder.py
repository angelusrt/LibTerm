import csv
import os
import time
import requests
from bs4 import BeautifulSoup
from dotenv import load_dotenv

load_dotenv()
url = os.getenv("URL")
wait = 15 
ishtmlfile = False
pad = "    "
sep = "="

files = os.listdir("../state/")
files = [file for file in files if file.endswith(".html")]

if len(files) > 0:
    print("cleaning files")
    print("--")

    for file in files:
        os.remove("../state/"+file)

try:
    dic = open("../state/dictionary.csv", mode="a+")
    err = open("../state/not-found.txt", mode="a+")
    index = open("../state/index.txt", mode="r+")

    file = open("../state/german-words-normalized.txt", mode="r")
    data = csv.reader(file)
    words = list(data)
except:
    print("error at opening some file...")
    print("--")
    exit(1)

start = index.read().split("\n")[0]
if start != None and start != "" and start.isnumeric():
    start = int(start)
else:
    start = 0

def write_index(i: int):
    index.seek(0)
    index.write(str(i)+"\n")
    index.truncate()

def print_error(word, i):
    assert isinstance(word, str)
    assert isinstance(i, int)

    err.write(word+"\n")
    write_index(i)
    print(pad+"not found - skipping")
    print("")
    time.sleep(wait)

def supplement(word):  
    response = requests.get(url=url+word)

    if ishtmlfile:
        html = open("../state/"+word+".html", mode="w")
        html.write(response.text)
        html.close()

    soup = BeautifulSoup(markup=response.text, features="html.parser")

    els = {}
    els["gender"] = soup.find("span", attrs={"class":"lemma__determiner"})
    els["freq"] = soup.find("span", attrs={"class":"shaft__full"})
    els["gram_type"] = soup.find("dd", attrs={"class":"tuple__val"})
    els["meaning_parent"] = soup.find("div", attrs={"id":"bedeutung"})
    els["meaning"] = None
    els["example"] = None

    if els["meaning_parent"] != None: 
        els["meaning"] = els["meaning_parent"].find("p")
        els["example"] = els["meaning_parent"].find("li")

    if els["meaning_parent"] == None: 
        els["meaning_parent"] = soup.find("li", attrs={"id":"Bedeutung-1"})

        if els["meaning_parent"] != None: 
            els["meaning"] = els["meaning_parent"].find("div")
            els["example"] = els["meaning_parent"].find("li")

    is_mean_par_valid = els["meaning_parent"] != None 
    is_gram_valid = els["gram_type"] != None
    is_mean_valid = els["meaning"] != None and not isinstance(els["meaning"], int)
    is_exam_valid = els["example"] != None
    is_valid = is_gram_valid and is_mean_par_valid and is_mean_valid and is_exam_valid

    return (els, is_valid)


for i in range(start, len(words)):
    if i % 7 == 0:
        os.system("clear")

    word = words[i][0]
    print(str(i)+": evaluating "+word)

    els, valid = supplement(word)
    time.sleep(wait)

    if not valid and word[0].isupper():
        word = word.lower()
        print(pad+"not found - trying ", word)

        els, valid = supplement(word)
        time.sleep(wait)

    if not valid and word.endswith("e"):
        word = word[:-1]
        print(pad+"not found - trying ", word)

        els, valid = supplement(word)
        time.sleep(wait)

    if not valid:
        print_error(word, i)
        continue

    gram = els["gram_type"].text

    #normalizing type
    if isinstance(gram, str):
        gram_sep = gram.find(",")

        if gram_sep > 0:
            gram = gram[0:gram_sep]

    mean = els["meaning"].text.strip()

    #normalizing mean
    if isinstance(mean, str):
        mean_sep = mean.find("(")

        if mean_sep > 0:
            mean = mean[0:mean_sep]

    exam = els["example"].text.replace("=", ",")

    #normalizing exam
    if isinstance(exam, str):
        exam_sep = exam.find("(")

        if exam_sep > 0:
            exam = exam[0:exam_sep]

    freq = "0"
    gender = ""

    if gram == "Eigenname":
        print(pad+"word is a name - skipping")
        print("")
        continue

    if els["gender"] != None:
        gender = els["gender"].text

    if els["freq"] != None:
        freq = str(len(els["freq"].text))

    print(pad+"found - writing")
    row = word+sep+freq+sep+gender+sep+gram+sep+mean+sep+exam

    print(pad+row)
    print("")

    write_index(i)
    dic.write(row+"\n")

