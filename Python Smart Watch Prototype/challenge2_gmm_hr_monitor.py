from ECE16Lib.Communication import Communication
from ECE16Lib.CircularList import CircularList
from ECE16Lib.HRMonitor import HRMonitor
from matplotlib import pyplot as plt
from time import time
import numpy as np

if __name__ == "__main__":
  fs = 50                         # sampling rate
  num_samples = 500               # 5 seconds of data @ 50Hz
  refresh_time = 12               # update heartrate every second
  hr = 0

  comms = Communication("COM4", 115200)
  comms.clear()                   # just in case any junk is in the pipes
  comms.send_message("wearable")  # begin sending data

  hrmon = HRMonitor(500, 50)
  hrmon.train()
  
  try:
    previous_time = time()
    while(True):
      message = comms.receive_message()
      if(message != None):
        try:
          (m1, m2) = message.split(',')
        except (ValueError, TypeError):        # if corrupted data, skip the sample
          continue

        # add the new values to the circular lists

        try:
          hrmon.add(int(m1), int(m2))
        except ValueError:
          continue

        # if enough time has elapsed, update the LED display with the current heartrate
        current_time = time()
        if (current_time - previous_time > refresh_time):
          previous_time = current_time

          hr = hrmon.predict()

          if(hr != -1):
            hr_round = round(hr, 3)
            hr_str = str(hr_round)
            print(hr_str)
            comms.send_message(hr_str)
          elif(hr == -1):
            print("bad data")
            comms.send_message("-1")


  except(Exception, KeyboardInterrupt) as e:
    print(e) # exiting the program due to exception
  finally:
    print("Closing connection.")
    comms.send_message("sleep")  # stop sending data
    comms.close()