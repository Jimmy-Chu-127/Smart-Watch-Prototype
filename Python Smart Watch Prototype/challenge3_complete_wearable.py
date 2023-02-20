from ECE16Lib.Communication import Communication
from ECE16Lib.CircularList import CircularList
from ECE16Lib. Pedometer import Pedometer
from ECE16Lib.HRMonitor import HRMonitor
from ECE16Lib.IdleDetector import IdleDetector
import numpy as np
import time                                         # for timing purposes
import pytz                                         # for getting time at different timezone
from pyowm import OWM                               # the Weather library
from datetime import datetime                       # for getting time at different timezone
from datetime import date                           # for getting the date

# Data storage list for 4 different locations and their weather, time zone
# access names
time_list = ["US/Pacific","America/New_York","Europe/Amsterdam","Asia/Tokyo"]
loc_list = ["San Diego", "New York", "Amsterdam", "Tokyo"]
weather_list = ["San Diego,CA,US", "New York,US", "Amsterdam", "Tokyo"];

# using the setup key to access weather at specified places
#owm = OWM('[insert key]').weather_manager()
owm = OWM('[insert key]').weather_manager()
w = owm.weather_at_place('San Diego,CA,US').weather

# The get_time function takes in a string input for the location and output
# a string currentTimeInLocation, corresponding to the current time at the
# given location.
def get_time(location):
    locationTz = pytz.timezone(location)
    timeInLocation = datetime.now(locationTz)
    currentTimeInLocation = timeInLocation.strftime("%H:%M:%S")
    return currentTimeInLocation

# The get_weather function takes in a string input for the location and output
# a string weather, corresponding to weather at the given location.
def get_weather(location):
    w = owm.weather_at_place(location).weather
    weather = w.detailed_status
    return weather


if __name__ == "__main__":
  fs = 50                                           # sampling rate
  num_samples = 500                                 # 5 seconds of data @ 50Hz
  refresh_time = 2                                  # update heartrate every 2 second

  pre_time = 0
  now_time = 0
  move_time = 0
  avg_L2 = 0
  pre_str = "1,2,3,4,5,6"

  times = CircularList([], num_samples)
  ppg = CircularList([], num_samples)
  L2 = CircularList([], num_samples)

  comms = Communication("COM7", 115200)
  comms.clear()                                     # just in case any junk is in the pipes
  comms.send_message(pre_str + ",wearable")        # begin sending data

  ped = Pedometer(num_samples, fs, [])
  hrmon = HRMonitor(500, fs)
  hrmon.train()

  try:
    pre_time = time.time()
    move_time = time.time()
    previous_time = time.time()
    while(True):
      now_time = time.time()
      message = comms.receive_message()

      if(message != None):
        try:
          (m1, m2, m3, m4, m5) = message.split(',')
        except ValueError:                          # if corrupted data, skip the sample
          continue

        # add the new values to the circular lists
        times.add(int(m1))
        ppg.add(int(m5))

        # add data to Pedometer class
        ped.add(int(m2), int(m3), int(m4))

        # computing the L2 norm and appending it to L2
        norm_L2 = np.sqrt(int(m2) ** 2 + int(m3) ** 2 + int(m4) ** 2)
        L2.add(norm_L2)

        # if enough time has elapsed, update the LED display with the current Heartrate
        current_time = time.time()
        if(current_time - previous_time > refresh_time):
          previous_time = current_time

          steps, jumps, peaks, filtered = ped.process()     # Pedometer operations
          print("Step count: {:d}".format(steps))

          data = np.column_stack([times, ppg])              # HRMonitor operations
          data_t = data[:, 0]
          data_t = (data_t - data_t[0])/1e3
          data_ppg = data[:, 1]

          hrmon.add(data_t, data_ppg)
          hr = hrmon.predict()

          if(hr == -1):
            hr_str = "0"
          else:
            hr_round = round(hr, 3)
            hr_str = str(hr_round)

          print(hr_str)


          message1 = get_time(time_list[0])                 # Weather operations
          message2 = str(date.today())
          message3 = get_weather(weather_list[0])
          message4 = loc_list[0]
          message = message1 + "," + message2 + "," + message3 + "," + message4

          mov = 0                                           # Idle Detector operations
          avg = np.average(L2[-50:])
          output1 = ""

          if(avg > 3530):
            move_time = time.time()
            mov = 1
          else:
            if (now_time - move_time >= 5):
              move_time = time.time()
              mov = 2
            else:
              mov = 0

          if (mov == 1):
            output1 = "motion"
            print("motion")
          elif (mov == 2):
            output1 = "idle"
            print("idle")

          print(message)
          output = str(steps) + "," + hr_str + "," + message + "," + output1
          comms.send_message(output)


  except(Exception, KeyboardInterrupt) as e:
    print(e)                                                # exiting the program due to exception
  finally:
    print("Closing connection.")
    comms.send_message(pre_str + ",sleep")                     # stop sending data
    comms.close()