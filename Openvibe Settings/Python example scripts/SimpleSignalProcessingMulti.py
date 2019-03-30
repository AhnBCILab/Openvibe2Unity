import numpy
import math
import O2U

class MyOVBox(OVBox):
        def __init__(self):
            	OVBox.__init__(self)
		self.signalHeader = None
                
	def initialize(self):
                print('Python script module_\'SimpleSignalProcessingMulti\'... OK!')
                                
	def process(self):                
                # Player1 = (Input : self.input[0] / Output : self.output[0])
                II_p1 = 0       # The "II" stands for "Input Index".
                OI_p1 = 0       # The "OI" stands for "Output Index".
                # Player2 = (Input : self.input[1] / Output : self.output[1])
                II_p2 = 1
                OI_p2 = 1
                
                # Player1               
                for chunkIndex in range( len(self.input[II_p1]) ):
                        if(type(self.input[II_p1][chunkIndex]) == OVSignalHeader):
                                self.signalHeader = self.input[II_p1].pop()
                                outputHeader = OVSignalHeader(
                                        self.signalHeader.startTime,
                                        self.signalHeader.endTime,
                                        [1,1], # The form of output(Size = 1*1, Type = int || double).
                                        ['SimpleSignalProcessingMulti']+self.signalHeader.dimensionSizes[1]*[''],
                                        4) # self.signalHeader.samplingRate
                                self.output[OI_p1].append(outputHeader)
                        elif(type(self.input[II_p1][chunkIndex]) == OVSignalBuffer):
                                chunk = self.input[II_p1].pop()
                                
                                # To calculate a value averaged over all channels.
                                mean_p1 = O2U.returnMeanOfAll(chunk, self.signalHeader.dimensionSizes)
                                
                                # Send the mean value.
                                sendBuffer = numpy.array(mean_p1).reshape(tuple([1])) 
                                chunk = OVSignalBuffer(chunk.startTime, chunk.endTime, sendBuffer.tolist()) 
                                self.output[OI_p1].append(chunk)
                        elif(type(self.input[II_p1][chunkIndex]) == OVSignalEnd):
                                self.output[OI_p1].append(self.input[II_p1].pop())


                # Player2 
                for chunkIndex in range( len(self.input[II_p2]) ):
                        if(type(self.input[II_p2][chunkIndex]) == OVSignalHeader):
                                self.signalHeader = self.input[II_p2].pop()
                                outputHeader = OVSignalHeader(
                                        self.signalHeader.startTime,
                                        self.signalHeader.endTime,
                                        [1,1], # The form of output(Size = 1*1, Type = int || double).
                                        ['SimpleSignalProcessingMulti']+self.signalHeader.dimensionSizes[1]*[''],
                                        4) # self.signalHeader.samplingRate
                                self.output[OI_p2].append(outputHeader)
                        elif(type(self.input[II_p2][chunkIndex]) == OVSignalBuffer):
                                chunk = self.input[II_p2].pop()
                                
                                # To calculate a value averaged over all channels.
                                mean_p2 = O2U.returnMeanOfAll(chunk, self.signalHeader.dimensionSizes)
                                            
                                # Send the mean value.
                                sendBuffer = numpy.array(mean_p2).reshape(tuple([1])) 
                                chunk = OVSignalBuffer(chunk.startTime, chunk.endTime, sendBuffer.tolist()) 
                                self.output[OI_p2].append(chunk)
                        elif(type(self.input[II_p2][chunkIndex]) == OVSignalEnd):
                                self.output[OI_p2].append(self.input[II_p2].pop())                                
                                            

box = MyOVBox()

	
