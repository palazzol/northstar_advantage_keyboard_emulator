# -*- coding: utf-8 -*-
"""
Created on Sun Nov 28 23:09:44 2021

@author: frank
"""

import tkinter as tk
import serial as serial
import time as t

# for an interactive keyboard
INTERACTIVE = True
# for automated entry of primary bootloader
#INTERACTIVE = False

keycode_to_keycode = {
   16 : 54, # shift
   17 : 37, # ctrl
   48 : 12, # 0
   49 : 3, # 1
   50 : 4, # 2
   51 : 5, # 3
   52 : 6, # 4
   53 : 7, # 5
   54 : 8, # 6
   55 : 9, # 7
   56 : 10, # 8
   57 : 11, # 9
   65 : 38, # a
   66 : 59, # b
   67 : 57, # c
   68 : 40, # d
   69 : 23, # e
   70 : 41, # f
   74 : 44, # j
   83 : 39, # s
   13 : 49, # return
   32 : 69, # space
   } 
   
   
s = serial.Serial('COM7',baudrate=115200)

def press(k):
    send(k)
    t.sleep(0.05)
    send(k+128)
    t.sleep(0.05)
    
def command(cmd):
    for c in cmd:
        c = ord(c)
        if c in keycode_to_keycode:
            press(keycode_to_keycode[c])
            
def send(k):
    print(k)
    s.write(k.to_bytes(k,byteorder='little'))
    
def keydown(event):
    if event.keycode in keycode_to_keycode:
        send(keycode_to_keycode[event.keycode])
    
def keyup(event):
    if event.keycode in keycode_to_keycode:
        send(keycode_to_keycode[event.keycode]+128)
        send(keycode_to_keycode[event.keycode]+128)

root = tk.Tk()
S = tk.Scrollbar(root)
T = tk.Text(root, height=4, width=50)
S.pack(side=tk.RIGHT, fill=tk.Y)
T.pack(side=tk.LEFT, fill=tk.Y)
S.config(command=T.yview)
T.config(yscrollcommand=S.set)
T.bind('<Key>', lambda event: keydown(event))
T.bind('<KeyRelease>', lambda event: keyup(event))
if INTERACTIVE:
    tk.mainloop()
else:
    t.sleep(1)
    press(12)  # clear buffer
    send(37)
    t.sleep(.1)
    send(57)
    t.sleep(.1)
    send(57+128)
    t.sleep(.1)
    send(37+128)
    t.sleep(.1)
    command('DFF00')
    command('3E80D351D3513E40D3513E7F')
    command('D3583E4ED3513E37D35106CB')  # <-- this CB has been changed from C9 - secondary bootloader length was changed
    command('0E502129FFDB50DB51E60228')
    command('FAEDA220F6')
    command(chr(13))
    command('JFF00')
    s.close()

