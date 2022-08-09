import os

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

# for file in os.listdir('patch/'):
#     print(ParseDiff(os.path.join('patch', file)))
diff = ParseDiff('linux-2.6.33.7.diff')
diff = sorted(diff)
PrintList(diff)
