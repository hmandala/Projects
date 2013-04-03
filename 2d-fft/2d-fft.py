# I wrote this because I couldn't find a similar example anywhere on the internet.    
#Python code to find 2D FFT of an image, remove higher frequency components, i.e, perform filtering, and convert pack to spatial representation.
# This code is currently uncommented and has many redundancies. I'll work on it soon...

#Note: If you find something I'm doing wrong, let me know. AFAIK this code is correct.

# you probably won't need all these
import math
import numpy
from sys import argv
import os
import cv
import scipy
import scipy.io
from scipy import fftpack
import adaptors
import Image
import matplotlib.pyplot as mp
from numpy.fft import *


RV_img = numpy.asfarray(Image.open("RV_ENDO_CONTOUR.bmp"))
temp = fft2(RV_img)
temp2 = fftshift(temp)
scipy.misc.imsave('temp2_real.jpg', temp2.real)
scipy.misc.imsave('temp2_imag.jpg', temp2.imag)


 # find phase magnitude representation from given complex number representation
mag = numpy.zeros((bi_rownum, bi_colnum))
phase = numpy.zeros((bi_rownum, bi_colnum))
for i in range(bi_rownum):
        for j in range(bi_colnum):
                mag[i,j] = pow(temp2.real[i,j]**2 + temp2.imag[i,j]**2, 0.5)
                phase[i,j] = math.atan2(temp2.imag[i,j], temp2.real[i,j])

# Perform simple threshold filtering. Higher frequency components will be removed.
for i in range(bi_rownum):
        for j in range(bi_colnum):
                if pow((i-bi_rownum/2)**2 + (j - bi_colnum/2)**2, 0.5) > 20:
                        mag[i,j] = 0
                        phase[i,j] = 0

 # reconvert to complex number representation
for i in range(bi_rownum):
        for j in range(bi_colnum):
                temp.real[i,j] = mag[i,j] * math.cos(phase[i,j])
                temp.imag[i,j] = mag[i,j] * math.sin(phase[i,j])


temp3 = ifftshift(temp)
final_img = ifft2(temp3)

scipy.misc.imsave('smoothed.jpg', final_image.real)
