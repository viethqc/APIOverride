from sklearn import svm
import numpy as np
from sklearn.metrics import accuracy_score

f = open("C:\\Users\\VietHQ\\Desktop\\train_clean.csv")
f.readline()  # skip the header
data_clean = np.loadtxt(f, delimiter=",")
f.close()

f = open("C:\\Users\\VietHQ\\Desktop\\train_malware.csv")
f.readline()  # skip the header
data_malware = np.loadtxt(f, delimiter=",")
f.close()

X = []
Y = []

for data in data_clean:
    X.append(data)
    Y.append(0)

for data in data_malware:
    X.append(data)
    Y.append(1)
# X = [[0, 0], [1, 1]]
# y = [0, 1]

clf = svm.SVC()
clf.fit(X, Y)

f = open("C:\\Users\\VietHQ\\Desktop\\test_clean.csv")
f.readline()  # skip the header
test_clean = np.loadtxt(f, delimiter=",")
f.close()

f = open("C:\\Users\\VietHQ\\Desktop\\test_malware.csv")
f.readline()  # skip the header
test_malware = np.loadtxt(f, delimiter=",")
f.close()

print clf.predict(test_clean)
print clf.predict(test_malware)