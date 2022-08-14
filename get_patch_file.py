import sys

def ParseDiff(filename):
    with open(filename, 'r') as f:
        files = []
        lines = f.readlines()
        for i in range(len(lines)):
            if lines[i][:3] == "+++":
                tmp =lines[i][4:-1].split('\t')[0]
                tmp = tmp[tmp.find('/')+1:]
                files.append(tmp)
    return files

def PrintList(l):
    for i in l:
        print(i)

for f in sys.argv[1:]:
    print(f)
    print("===================================================================")
    diff = ParseDiff(f)
    diff = sorted(diff)
    PrintList(diff)
