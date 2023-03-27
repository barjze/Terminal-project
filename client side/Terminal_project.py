import sys
import serial as ser
import serial.tools.list_ports
import time
import PySimpleGUI as sg
import os

# this example implements a chat where the PC sends a message/character and receives a response
# arguments:  0 - read 1 byte each time from the buffer
#             1 - read from the buffer until the terminator sequence '\n' is received
COM = 'COM3'
baudrate = 9600



def serial_ports():
    ports = serial.tools.list_ports.comports()
    list = []
    for port, desc, hwid in sorted(ports):
        list.append(port + ' ' + desc.split('(')[0])
        #print("{}: {}".format(port, desc.split('(')[0]))
    return list


def change_COM(x):
    global COM
    COM = x

def change_baundrate(x):
    global baudrate
    baudrate = x

def currect_mod(x):
    s = create_uart_connection()
    bytetxMsg = bytes('4' + x + '\n', 'ascii')
    s.write(bytetxMsg)
    time.sleep(0.25)

def baudrate_change_send(x):
    s = create_uart_connection()
    bytetxMsg = bytes('3' + str(x) + '\n', 'ascii')
    s.write(bytetxMsg)
    time.sleep(0.25)

def create_uart_connection():
    global COM
    global baudrate
    s = ser.Serial(COM, baudrate=baudrate, bytesize=ser.EIGHTBITS,
                   parity=ser.PARITY_NONE, stopbits=ser.STOPBITS_ONE,
                   timeout=1)  # timeout of 1 sec so that the read and write operations are blocking,
    # when the timeout expires the program will continue
    # clear buffers
    s.reset_input_buffer()
    s.reset_output_buffer()
    return s

def initial_Uart_avoid_cut():
    s = create_uart_connection()
    bytetxMsg = bytes('00' + '\n', 'ascii')
    s.write(bytetxMsg)
    time.sleep(0.25)

def send_massge_mod1(s, x):
    enableTX = True
    # TX
    while (1):  # while the output buffer isn't empty
        txMsg = x
        bytetxMsg = bytes('1' + txMsg + '\n', 'ascii')
        s.write(bytetxMsg)
        time.sleep(0.25)  # delay for accurate read/write operations on both ends
        if s.out_waiting == 0:
            return
            #enableTX = False

def read_massge_mod1(s):
    # RX
    while (s.in_waiting > 0):  # while the input buffer isn't empty
        lineByte = s.read_until()    # read  from the buffer until the terminator is received,
        time.sleep(0.25)  # delay for accurate read/write operations on both ends
        return lineByte.decode("ascii")
    return ''

def file_send_mod2(x):
    path = x
    flag = 0
    file_name = path.split("/")[-1]
    file_read = open(path, "r")
    s = create_uart_connection()
    enableTX = True
    file_text = file_read.read()
    if len(file_text) >= 8192:
        return -1
    while (s.out_waiting > 0 or enableTX):
        bytetxMsg = bytes('2' + file_name + chr(2) + str(len(file_text) + 1)[::-1] + '\n', 'ascii') #+ file_text + chr(3), 'ascii')
        #print('2' + file_name + chr(2) + str(len(file_text) + 1) + '\n' + file_text + chr(3))
        s.write(bytetxMsg)
        time.sleep(0.25)  # delay for accurate read/write operations on both ends
        if s.out_waiting == 0:
            enableTX = False

    wait_to_ACK(s)
    enableTX = True

    while (s.out_waiting > 0 or enableTX):
        bytetxMsg = bytes('5' + '\n' + file_text + chr(3), 'ascii')
        #print('2' + file_name + chr(2) + str(len(file_text) + 1) + '\n' + file_text + chr(3))
        s.write(bytetxMsg)
        time.sleep(0.25)  # delay for accurate read/write operations on both ends
        if s.out_waiting == 0:
            enableTX = False

    return wait_to_ACK(s)

def file_recive_mod2(s):
    data = ''
    while (True):  # while the input buffer isn't empty
        lineByte = s.read_until(expected = b'\x03')  # read  from the buffer until the terminator is received,
        time.sleep(0.25)
        data += lineByte.decode("ascii")
        try:
            if data[-1] == chr(3):
                return data[:-1]
        except:
            pass


def file_name_recive_mod2(s):
    i = 0
    while(i < 20000):
        i += 1
        while (s.in_waiting > 0):  # while the input buffer isn't empty
            lineByte = s.read_until(expected = b'\x02')  # read  from the buffer until the terminator is received,
            time.sleep(0.25)
            data = lineByte.decode("ascii")
            if s.in_waiting == 0:
                return data
    return ''


def send_ACK_to_MCU(s, name_or_file = 'f'):
    enableTX = True
    if name_or_file == 'n':
        mod = '1'
    else:
        mod = '2'
    while (s.out_waiting > 0 or enableTX):
        bytetxMsg = bytes('6' + mod + '\n', 'ascii')
        s.write(bytetxMsg)
        time.sleep(0.25)  # delay for accurate read/write operations on both ends
        if s.out_waiting == 0:
            enableTX = False


def wait_to_ACK(s):
    while (1):
        while (s.in_waiting > 0):  # while the input buffer isn't empty
            lineByte = s.read_until()  # read  from the buffer until the terminator is received,
            time.sleep(0.25)
            # readline() can also be used if the terminator is '\n'
            if (ord(lineByte.decode("ascii")[-1]) == 6):
                return lineByte.decode("ascii")[-1]

#if __name__ == '__main__':
    #initial_Uart_avoid_cut()
    #file_send_mod2("C:\\Users\\Bar-PC\\Desktop\\flie tests for Project\\file2.txt")
    #file_send_mod2("C:\\Users\\Bar-PC\\Desktop\\flie tests for Project\\file1.txt")
    #file_send_mod2("C:\\Users\\Bar-PC\\Desktop\\flie tests for Project\\file3.txt")
    #currect_mod('2')

    # sg.theme('Dark Grey 13')
    #
    # layout = [[sg.Text('Filename')],
    #           [sg.Input(), sg.FileBrowse()],
    #           [sg.OK(), sg.Cancel()]]
    #
    # window = sg.Window('Get filename example', layout)
    #
    # event, values = window.read()
    # window.close()


    #read_or_send_massge_mod1()
    # print(bytes('AaBb123yz' + '\n', 'ascii'))
    # for j in range(0,129):
    #     print('----------------')
    #     print(j)
    #     print(bytes('', 'utf-8'))
    #     print(len(bytes('\n', 'utf-8')))
    ##print('----------------')
    ##print(bytes('A', 'utf-8'))
    ##print(len(bytes('A', 'utf-8')))
    # for i in range(0,256):
    #     print('----------------')
    #     print(i)
    #     print(bytes(chr(i), 'utf-8'))
    #     print(len(bytes(chr(i), 'utf-8')))