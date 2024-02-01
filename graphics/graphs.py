import csv
from telnetlib import X3PAD
import matplotlib.pyplot as plot
import pandas as pd

data1 = pd.read_csv('cyclictestURJC_idle_rt.csv')
data2 = pd.read_csv('cyclictestURJC_idle_nrt.csv')
#data2 = pd.read_csv('cyclictestURJC_hb_rt.csv')
#data3 = pd.read_csv('cyclictestURJC_bonnie_rt.csv')

df1 = pd.DataFrame(data1)
df2 = pd.DataFrame(data2)
#df3 = pd.DataFrame(data3)

X1 = list (df1.iloc[:, 2])
X2 = list (df2.iloc[:, 2])
#X3 = list (df3.iloc[:, 2])

plot.hist(X1, range=[0,55], color='r', edgecolor='black', linewidth=0.33, bins=100, alpha=0.5,
label="S1: RT")
plot.hist(X2, range=[0,55], color='b', edgecolor='black', linewidth=0.33, bins=100, alpha=0.5,
label="S2: NRT")
#plot.hist(X3, range=[0,55], color='g', edgecolor='black', linewidth=0.33, bins=100, alpha=0.5,
#label="S3: RT Bonnie")

plot.title("CyclictestURJC Raspberrys")
plot.xlabel('Latencia (microsegundos)')
plot.ylabel('Frecuencia')

plot.legend()
plot.show()