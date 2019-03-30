import numpy
import math
import O2U

class MyOVBox(OVBox):
        def __init__(self):
            	OVBox.__init__(self)
		self.signalHeader = None
                
	def initialize(self):                
                print('Python script module_\'Playing\'... OK!')
                global classifier

                # Read a configuration file.
                filename, trainingTime, classNum = O2U.readConfigFile('.\\O2U_PythonScript.cfg')

                # Read a classifier.
                classifier = O2U.readClassifierTxt(filename)
                                
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
                                        ['Playing']+self.signalHeader.dimensionSizes[1]*[''],
                                        4) # self.signalHeader.samplingRate
                                self.output[OI_p1].append(outputHeader)
                        elif(type(self.input[II_p1][chunkIndex]) == OVSignalBuffer):
                                chunk = self.input[II_p1].pop()

                                # To calculate a value averaged over all channels.
                                mean = O2U.returnMeanOfAll(chunk, self.signalHeader.dimensionSizes)

                                # Determine a signal among N classes. (ex> N=10 )
                                classNum = O2U.determineClass(classifier, mean)
                                        
                                # Send a number of determined class.
                                sendBuffer = numpy.array(classNum).reshape(tuple([1])) 
                                chunk = OVSignalBuffer(chunk.startTime, chunk.endTime, sendBuffer.tolist()) 
                                self.output[OI_p1].append(chunk)
                        elif(type(self.input[II_p1][chunkIndex]) == OVSignalEnd):
                                self.output[OI_p1].append(self.input[II_p1].pop())
                                            

box = MyOVBox()

	
