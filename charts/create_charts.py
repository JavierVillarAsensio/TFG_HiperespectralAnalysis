import numpy as np
import matplotlib.pyplot as plt
from sklearn.metrics import confusion_matrix, ConfusionMatrixDisplay
import os

confusion_matrix_file = 'confusion_matrix.txt'
legend_file = 'legend.txt'
static_times_file = 'static_times.txt'
compose_times_file = 'compose_times.txt'

def confusion_matrix():
    cm = []
    with open(confusion_matrix_file, 'r') as file:
        for line in file:
            read_line = list(map(int, line.split()))
            cm.append(read_line)

    labels = []
    with open(legend_file, 'r') as file:
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
    plt.clf()

def read_times(file_name):
    values = []
    with open(file_name, 'r') as file:
        for line in file:
            read = list(map(str, line.split()))
            read = list(map(str, read[1].split('m')))
            read[1] = read[1][:-1]
            read[1] = read[1].replace(',', '.')
            new_value = (float(read[0]*60)) + float(read[1])
            values.append(new_value)

    return values

def bar_chart(file_name):
    values = read_times(file_name)
    labels = ['4 specs', '8 specs', '12 specs', '16 specs', '20 specs']

    plt.bar(labels, values)
    plt.title(file_name[:-4])
    plt.xlabel('Number of spectrums')
    plt.ylabel('Elapsed time')

    plt.savefig(file_name[:-4]+'.png', format='png')
    plt.clf()

def main():
    if os.path.exists(confusion_matrix_file) and os.path.exists(legend_file):
        confusion_matrix()

    if os.path.exists(static_times_file):
        bar_chart(static_times_file)

    if os.path.exists(compose_times_file):
        bar_chart(compose_times_file)

if __name__ == "__main__":
    main()