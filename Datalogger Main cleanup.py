import numpy as np;
import pandas as pd;
#from scipy import interpolate
#import pylab as py

import matplotlib.pyplot as plt
import matplotlib.image as mpimg
import matplotlib.animation  as animation
from matplotlib import style
from matplotlib.animation import FuncAnimation,FFMpegFileWriter

from linear_interp import *


#import data from Master logger
masterData = np.genfromtxt("MASTER.txt" ,  delimiter= ',' ) ;
masterData = np.delete(masterData , (0) , axis=0 )
offsetTime = masterData[0,0];

#import data from SlaveB
slaveBdata = np.genfromtxt("SLAVEB.txt" ,  delimiter= ',' ) ;
slaveBdata = np.delete(slaveBdata , (0) , axis=0 );
slaveBbeginTime = slaveBdata[0,0];


#import data from SlaveC
slaveCdata = np.genfromtxt("SLAVEC.txt" ,  delimiter= ',' ) ;
slaveCdata = np.delete(slaveCdata , (0) , axis=0 );
slaveCbeginTime = slaveCdata[0,0];
  
#-----------------A list of what each column is---------------------#

#f0         time in milliseconds
#f1         acceleration in x
#f2         acceleration in y
#f3         acceleration in z
#f4         gyro x
#f5         gyro y
#f6         gyro z
#f7         FSR ( note , 0 to 20N = 0 to 900 ish)

#-------------------------------------------------------------------#
    
slaveBcorrection = slaveBbeginTime - offsetTime;
slaveBdata[: , 0] = slaveBdata[: , 0] - slaveBcorrection;

slaveCcorrection = slaveCbeginTime - offsetTime;
slaveCdata[: , 0] = slaveCdata[: , 0] - slaveCcorrection;

masterData[:,0] = masterData[:,0] - offsetTime
slaveBdata[:,0] = slaveBdata[:,0] - offsetTime
slaveCdata[:,0] = slaveCdata[:,0] - offsetTime


master_data_clean = linear_interp( masterData)
slaveB_data_clean = linear_interp( slaveBdata)
slaveC_data_clean = linear_interp( slaveCdata)

      

plt.plot(master_data_clean[: , 0] , master_data_clean[:,3])
plt.plot(slaveB_data_clean[: , 0] , slaveB_data_clean[: , 3])
plt.plot(slaveC_data_clean[: , 0] , slaveC_data_clean[: , 3])
plt.ylabel('acceleration in z axis')
plt.xlabel('time (in milliseconds)')
plt.show


#shape_master = master_data_clean.shape
#shape_slaveB = slaveB_data_clean.shape
#shape_slaveC = slaveC_data_clean.shape

#tester = pd.concat([master_data_clean , slaveB_data_clean] , axis = 'col')

master_dataset = pd.DataFrame({'Time in milliseconds':master_data_clean[:,0],'Acceleration in X':master_data_clean[:,1] , 
                        'Acceleration in Y':master_data_clean[:,2] , 'Acceleration in Z':master_data_clean[:,3] ,
                        'Gyro in X':master_data_clean[:,4] , 'Gyro in Y':master_data_clean[:,5] , 
                        'Gyro in Z':master_data_clean[:,6] , 'Analogue Read':master_data_clean[:,7] ,  })

slaveB_dataset = pd.DataFrame({'Slave B Time':slaveB_data_clean[:,0],'Acceleration in X':slaveB_data_clean[:,1] , 
                        'Acceleration in Y':slaveB_data_clean[:,2] , 'Acceleration in Z':slaveB_data_clean[:,3] ,
                        'Gyro in X':slaveB_data_clean[:,4] , 'Gyro in Y':slaveB_data_clean[:,5] , 
                        'Gyro in Z':slaveB_data_clean[:,6] , 'Analogue Read':slaveB_data_clean[:,7] ,  })

slaveC_dataset = pd.DataFrame({'Slave C Time':slaveC_data_clean[:,0],'Acceleration in X':slaveC_data_clean[:,1] , 
                        'Acceleration in Y':slaveC_data_clean[:,2] , 'Acceleration in Z':slaveC_data_clean[:,3] ,
                        'Gyro in X':slaveC_data_clean[:,4] , 'Gyro in Y':slaveC_data_clean[:,5] , 
                        'Gyro in Z':slaveC_data_clean[:,6] , 'Analogue Read':slaveC_data_clean[:,7] ,  })


export_dataset = pd.concat([ master_dataset , slaveB_dataset , slaveC_dataset ] , ignore_index = False , axis = 1)


export_dataset.to_csv('dataset_export.csv')


