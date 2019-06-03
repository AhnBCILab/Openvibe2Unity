############################################################################JEJUS############################
#  [SimpleSignalProcessing.py]                                                                              #
#                                                                                                           #
#  Function : "SimpleSignalProcessing.py" is an example script to show how to send the signal, measured     #
#             from an EEG device and processed in "Python scripting" box, to Unity3d.                       #
#                                                                                                           #
#                                                                                                           #
# The "Python Scripting" box had four inputs which are two signal inputs and two stimulation inputs.        #
#                                                                                                           #
# <Signal Inputs>                                                                                           #
# : self.input[0](used), self.input[1]                                                                      #
#                                                                                                           #
# <Stimulation Inputs>                                                                                      #
# : self.input[2], self.input[3]                                                                            #
#                                                                                                           #
#                                                                                                           #
# Also, the box had two outputs which are a signal output and a stimulation output.                         #
#                                                                                                           #
# <Signal Output>                                                                                           #
# : self.output[0](used)                                                                                    #
#                                                                                                           #
# <Stimulation Output>                                                                                      #
# : self.output[1]                                                                                          #
#                                                                                                           #
# --------------------------------------------------------------------------------------------------------- #
#                                                                                                           #
# Author:                                                                                                   #                                                                 
#  Seongjoon Jeong (jdd01299@naver.com)                                                                     #
#  Brain-Computer Interface (BCI) Lab.                                                                      #
#  School of Computer Science and Electrical Engineering,                                                   #
#  Handong Global University, Pohang, Korea                                                                 #
#                                                                                                           #
#                                                                                                           #
# Histroy: (1.0) 2018.12.08 by Seongjoon Jeong                                                              #
#                                                                                                           #
#                                                                                                           #
#############################################################################################################

import numpy
import math
import O2U

class MyOVBox(OVBox):
        def __init__(self):
            	OVBox.__init__(self)
		self.signalHeader = None
                
	def initialize(self):
                print('Python script module_\'SimpleSignalProcessing\'... OK!')
                                
	def process(self):
                # Player1 = (Input : self.input[0] / Output : self.output[0])
                II_p1 = 0       # The "II" stands for "Input Index".
                OI_p1 = 0       # The "OI" stands for "Output Index".
                
                for chunkIndex in range( len(self.input[II_p1]) ):
                        if(type(self.input[II_p1][chunkIndex]) == OVSignalHeader):
                                self.signalHeader = self.input[II_p1].pop()
                                outputHeader = OVSignalHeader(
                                        self.signalHeader.startTime,
                                        self.signalHeader.endTime,
                                        [1,1], # The form of output(Size = 1*1, Type = int || double).
                                        ['SimpleSignalProcessing']+self.signalHeader.dimensionSizes[1]*[''],
                                        4) # self.signalHeader.samplingRate
                                self.output[OI_p1].append(outputHeader)
                        elif(type(self.input[II_p1][chunkIndex]) == OVSignalBuffer):
                                chunk = self.input[II_p1].pop()

                                # To calculate a value averaged over all channels.
                                mean = O2U.returnMeanOfAll(chunk, self.signalHeader.dimensionSizes)
                                
                                # Send the mean value.
                                sendBuffer = numpy.array(mean).reshape(tuple([1])) 
                                chunk = OVSignalBuffer(chunk.startTime, chunk.endTime, sendBuffer.tolist()) 
                                self.output[OI_p1].append(chunk)
                        elif(type(self.input[II_p1][chunkIndex]) == OVSignalEnd):
                                self.output[OI_p1].append(self.input[II_p1].pop())
                                            

box = MyOVBox()

	
