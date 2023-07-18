# coding=utf-8
import snowboydecoder
import sys
import signal
import localconfig
# Demo code for listening to two hotwords at the same time

interrupted = False


def signal_handler(signal, frame):
    global interrupted
    interrupted = True


def interrupt_callback():
    global interrupted
    return interrupted

def red():
    print("turn on the red")
    
def green():
    print("light up the green")
    
def yellow():
    print("switch on the yellow")
    
def close():
    print("close all the light")

def open():
    print("open the light")

def increase():
    print("increase brightness")

def reduce():
    print ("reduce brightness")
    
#if len(sys.argv) != 8:
#    print("Error: need to specify 7 model names")
#    print("Usage: python demo.py 1st.model 2nd.model")
#    sys.exit(-1)

models = ["red.pmdl","green.pmdl","yellow.pmdl","close.pmdl","open.pmdl","increase.pmdl","reduce.pmdl"]#更新为新指令

# capture SIGINT signal, e.g., Ctrl+C
signal.signal(signal.SIGINT, signal_handler)

sensitivity = [0.5]*len(models)
#detector = snowboydecoder.HotwordDetector(models, sensitivity=sensitivity)
detector = snowboydecoder.HotwordDetector(models, sensitivity=0.5)
callbacks = [ lambda: red(), 
            lambda: green(), 
            lambda: yellow(), 
            lambda: close(),
            lambda: open(), 
            lambda: increase(), 
            lambda: reduce()
            ]
print('Listening... Press Ctrl+C to exit')

# main loop
# make sure you have the same numbers of callbacks and models
detector.start(detected_callback=callbacks,
               interrupt_check=interrupt_callback,
               sleep_time=0.03)

detector.terminate()
