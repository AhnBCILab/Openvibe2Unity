import numpy
import math
import O2U
import os

class MyOVBox(OVBox):
        def __init__(self):
            	OVBox.__init__(self)
		self.signalHeader = None
                
	def initialize(self):
                print('Python script module_\'Training\'... OK!')
                global meanArr, isTraining, filename
                global initTime, currentTime, trainingTime, classNum
                
                meanArr =[]
                isTraining = False

                # Read a configuration file.
                filename, trainingTime, classNum = O2U.readConfigFile('.\\O2U_PythonScript.cfg')
                                
	def process(self):
                global meanArr, isTraining, initTime, currentTime, trainingTime
                
                # Player1 = (Input : self.input[0] / Output : self.output[0])
                II_p1 = 0       # The "II" stands for "Input Index".
                OI_p1 = 0       # The "OI" stands for "Output Index".
                # Keyboard button 'a' = (Input : self.input[1])
                II_Key = 1
                # Stop stimulation = (Output : self.output[1])
                OI_Stop = 1  

                if(isTraining==False):
                        if(O2U.stimInputCheck(self.input, OVStimulationSet, II_Key, 33025)): # Keyboard 'a' == 33025
                                initTime = self.getCurrentTime()
                                isTraining = True
                                print('Start training.') 
                
                for chunkIndex in range( len(self.input[II_p1]) ):
                        if(type(self.input[II_p1][chunkIndex]) == OVSignalHeader):
                                self.signalHeader = self.input[II_p1].pop()
                                outputHeader = OVSignalHeader(
                                        self.signalHeader.startTime,
                                        self.signalHeader.endTime,
                                        [1,1], # The form of output(Size = 1*1, Type = int || double).
                                        ['Training']+self.signalHeader.dimensionSizes[1]*[''],
                                        4) # self.signalHeader.samplingRate
                                self.output[OI_p1].append(outputHeader)
                        elif(type(self.input[II_p1][chunkIndex]) == OVSignalBuffer):
                                chunk = self.input[II_p1].pop()
                                
                                # To calculate a value averaged over all channels.
                                mean = O2U.returnMeanOfAll(chunk, self.signalHeader.dimensionSizes)
                                if(isTraining):
                                        meanArr.append(mean)

                                        currentTime = self.getCurrentTime()
                                        progressTime = currentTime - initTime
                                        if(progressTime >= trainingTime):
                                                # To output a stimulation (id == 1) when the training should stop.
                                                stimSet = OVStimulationSet(0., 0.)
                                                stimSet.append(OVStimulation(1, self.getCurrentTime(), 0.)) # OVTK_StimulationId_Number_01 == 1
                                                self.output[OI_Stop].append(stimSet)
                                                
                                # Send the mean value.
                                sendBuffer = numpy.array(mean).reshape(tuple([1])) 
                                chunk = OVSignalBuffer(chunk.startTime, chunk.endTime, sendBuffer.tolist()) 
                                self.output[OI_p1].append(chunk)
                        elif(type(self.input[II_p1][chunkIndex]) == OVSignalEnd):
                                self.output[OI_p1].append(self.input[II_p1].pop())
                                
        def uninitialize(self):
                global meanArr, filename, classNum
                classifier = O2U.makeDevClassifierTxt(meanArr, filename, classNum) # The classifier will has 10 classes.
                return
                                            

box = MyOVBox()

	
