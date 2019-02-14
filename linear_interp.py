# -*- coding: utf-8 -*-
"""
Created on Wed Feb 13 09:42:23 2019

@author: ED
"""
#import numpy as np; 

#masterData = np.genfromtxt("MASTER.txt" ,  delimiter= ',' ) ;
#masterData = np.delete(masterData , (0) , axis=0 )
## replace master data with input for thois file to become a function
import numpy as np;
def linear_interp (input_array):

    end_time  = (input_array[-1, 0])
    end_time = int(end_time)
    output_array = np.zeros((end_time , 8 ))
    output_array[: , 0] =  range(end_time)
    
    for i in range (8):
        output_array[: , i] = np.interp( output_array[:,0] , input_array[:, 0] , input_array[:,i])
    
    for i in range (5):
        output_array[: , i+1] = output_array[: , i+1] - (np.mean(output_array[: , i+1]) ) 
    
    return ( output_array)