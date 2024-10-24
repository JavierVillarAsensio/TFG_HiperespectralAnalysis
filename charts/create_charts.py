import numpy as np
import matplotlib.pyplot as plt
from sklearn.metrics import confusion_matrix, ConfusionMatrixDisplay
from sklearn import datasets

def confusion_matrix():
    cm = []
    with open('confusion_matrix.txt', 'r') as file:
        for line in file:
            read_line = list(map(int, line.split()))
            cm.append(read_line)

    labels = []
    with open('legend.txt', 'r') as file:
        for line in file:
            read_line = list(map(str, line.split()))
            label = read_line[1].split('.')
            label = label[:2]
            label = '.'.join(label)
            labels.append(label)

    disp = ConfusionMatrixDisplay(confusion_matrix=np.array(cm), display_labels=np.array(labels))
    disp.plot(cmap=plt.cm.Blues)
    plt.title('Confusion matrix of proposed solution')
    plt.xticks(rotation=15, fontsize=9)
    plt.yticks(fontsize=9)

    plt.savefig('matriz_confusion.png', format='png')

def main():
    confusion_matrix()

if __name__ == "__main__":
    main()