import sys
import serial as ser
import time
import PySimpleGUI as sg
import os
from os.path import exists
import Terminal_project as tp
import glob


ACK_flag = False

def menu_GUI():
    sg.theme('GreenTan')  # give our window a spiffy set of colors

    layout = [[sg.Button('chat mod', button_color=(sg.YELLOWS[0], sg.BLUES[0]), bind_return_key=True, size = (25,2))],
              [sg.Button('file transfer mod', button_color=(sg.YELLOWS[0], sg.BLUES[0]), bind_return_key=True, size = (25,2))],
               [sg.Button('Terminal Configuration', button_color=(sg.YELLOWS[0], sg.BLUES[0]), bind_return_key=True, size = (25,2))],
               [sg.Button('EXIT', button_color=(sg.YELLOWS[0], sg.GREENS[0]))]]

    window = sg.Window('Main MENU', layout,element_justification='c',font=('Helvetica', ' 13'), default_button_element_size=(8, 2),
                       use_default_focus=False)

    while True:  # The Event Loop
        event, value = window.read()
        if event in (sg.WIN_CLOSED, 'EXIT'):
            window.close()
            return
        elif event == 'chat mod':
            tp.currect_mod('1')
            window.close()
            chat_GUI()
            menu_GUI()
            #return
        elif event == 'file transfer mod':
            tp.currect_mod('2')
            window.close()
            menu_file_mod2_GUI()
            menu_GUI()
            #return
        elif event == 'Terminal Configuration':
            tp.currect_mod('3')
            window.close()
            Terminal_config()
            menu_GUI()
            #return

def chat_GUI():
    s = tp.create_uart_connection()
    sg.theme('GreenTan')  # give our window a spiffy set of colors

    layout = [[sg.Text('Your output will go here:', size=(40, 1))],
              [sg.Output(size=(110, 20), font=('Helvetica 10'), key ='-chat-')],
              [sg.Multiline(size=(70, 5), enter_submits=False, key='-QUERY-', do_not_clear=True),
               sg.Button('SEND', button_color=(sg.YELLOWS[0], sg.BLUES[0]), bind_return_key=True),
               sg.Button('EXIT', button_color=(sg.YELLOWS[0], sg.GREENS[0]))]]

    window = sg.Window('Chat window', layout, font=('Helvetica', ' 13'), default_button_element_size=(8, 2),
                       use_default_focus=False)

    while True:  # The Event Loop
        event, value = window.read(timeout = 2)
        if event in (sg.WIN_CLOSED, 'EXIT'):  # quit if exit button or X
            window.close()
            return
            #menu_GUI()
        if event == 'SEND':
            query = value['-QUERY-'].rstrip()
            window['-QUERY-'].update(value = '')
            # EXECUTE YOUR COMMAND HERE
            print('PC: {}'.format(query), flush=True)
            window.refresh()
            time.sleep(0.25)
            tp.send_massge_mod1(s, query)
            s.reset_output_buffer()
        responed = tp.read_massge_mod1(s)
        s.reset_input_buffer()
        if responed != '':
            print('MCU: {}'.format(responed), flush=True)
            #window.refresh()
            time.sleep(0.25)

def menu_file_mod2_GUI():
    global ACK_flag
    sg.theme('GreenTan')  # give our window a spiffy set of colors

    layout = [[sg.Text('Please select an option:')],
              [sg.Button('send file', button_color=(sg.YELLOWS[0], sg.BLUES[0]), bind_return_key=True, size = (15,2)),
               sg.Button('receive file', button_color=(sg.YELLOWS[0], sg.BLUES[0]), bind_return_key=True, size = (15,2))],
              [sg.Checkbox('ACK?',key='ack', default = ACK_flag, enable_events = True) , sg.Push(), sg.Button('EXIT', button_color=(sg.YELLOWS[0], sg.GREENS[0]))]]

    window = sg.Window('MENU - File RX/TX', layout, element_justification='c', font=('Helvetica', ' 13'), default_button_element_size=(8, 2),
                       use_default_focus=False)

    while True:  # The Event Loop
        event, value = window.read()
        if event in (sg.WIN_CLOSED, 'EXIT'):
            window.close()
            return
        elif event == 'send file':
            window.close()
            file_GUI_send()
            menu_file_mod2_GUI()
        elif event == 'receive file':
            window.close()
            file_GUI_receive()
            menu_file_mod2_GUI()
        elif event == 'ack':
            if value['ack'] == True:
                ACK_flag = True
                tp.currect_mod('4')

            elif value['ack'] == False:
                ACK_flag = False
                tp.currect_mod('5')


def file_GUI_send():
    global ACK_flag
    sg.theme('GreenTan')  # give our window a spiffy set of colors

    layout = [[sg.Text('Path of File:'), sg.Push()],
              [sg.Input(key = 'Path'), sg.FileBrowse()],
              [sg.Button('OK', button_color=(sg.YELLOWS[0], sg.BLUES[0]), bind_return_key=True), sg.Button('EXIT', button_color=(sg.YELLOWS[0], sg.BLUES[0]), bind_return_key=True)]]

    window = sg.Window('send File', layout, element_justification='c')

    while True:  # The Event Loop
        event, value = window.read()
        if event in (sg.WIN_CLOSED, 'EXIT'):
            window.close()
            return
        elif event == 'OK':
            if not exists(value['Path']):
                sg.popup("Error: file not exists")
            else:
                ACK = tp.file_send_mod2(value['Path'])
                tp.currect_mod('2')
                if ACK == -1:
                    sg.popup("Error! file is bigger than 8KB!!!")
                elif ord(ACK) == 6 and ACK_flag == True:
                    sg.popup("file send complete")
                window.close()
                return
def file_GUI_receive():
    data =''
    name_of_file =''
    name_of_file, data = file_GUI_receive_wait_for_file()
    #s = tp.create_uart_connection()
    sg.theme('GreenTan')  # give our window a spiffy set of colors

    layout = [[sg.Text('Path to save to:')],
              [sg.Input(key = 'Path'), sg.FolderBrowse()],
              [sg.Button('OK', button_color=(sg.YELLOWS[0], sg.BLUES[0]), bind_return_key=True), sg.Button('EXIT', button_color=(sg.YELLOWS[0], sg.BLUES[0]), bind_return_key=True)]]
    if data == '' or name_of_file == '':
        return
    window = sg.Window('reciving to...', layout)

    while True:  # The Event Loop
        event, value = window.read()
        if event in (sg.WIN_CLOSED, 'EXIT'):
            window.close()
            return
        elif event == 'OK':
            if not exists(value['Path']):
                sg.popup("Error: folder not exists")
            else:
                if data != '':
                    flie_name = name_of_file[0:-1]
                    text_file = open(value['Path'] + '/' + flie_name, 'w')
                    text_file.write(data)
                    text_file.close()
                    tp.currect_mod('2')
                    window.close()
                    return
                else:
                    sg.popup("Error something went worng")

def file_GUI_receive_wait_for_file():
    file_ACK = False
    name_ACK = False
    sg.theme('GreenTan')  # give our window a spiffy set of colors
    layout = [[sg.Text('Waiting for file to be send....')],
              [sg.Button('EXIT', button_color=(sg.YELLOWS[0], sg.BLUES[0]), bind_return_key=True)]]

    window = sg.Window('Waiting for file', layout, element_justification='c', size = (400,80))
    string_of_file = ''
    name_of_file = ''
    s = tp.create_uart_connection()
    while True:  # The Event Loop
        event, value = window.read(timeout = 1)
        if name_of_file == '':
            name_of_file = tp.file_name_recive_mod2(s)
        if name_of_file != '' and name_ACK == False:
            tp.send_ACK_to_MCU(s, 'n')
            name_ACK = True
        if name_ACK == True:
            if string_of_file == '':
                string_of_file = tp.file_recive_mod2(s)
            if string_of_file != '' and file_ACK == False:
                tp.send_ACK_to_MCU(s, 'f')
                file_ACK = True
        if event in (sg.WIN_CLOSED, 'EXIT'):
            window.close()
            return '', ''
        if file_ACK == True and name_ACK == True:
            window.close()
            return name_of_file, string_of_file

def Terminal_config():
    sg.theme('GreenTan')  # give our window a spiffy set of colors
    Ports = tp.serial_ports()
    SDA = [1 if 'SDA' in x else 0 for x in Ports].index(1)
    list_of_baund = [2400, 9600, 19200, 38400]
    layout = [[sg.Combo(Ports, default_value=Ports[SDA], s=(30,22), enable_events=True, readonly=True, k='-COMBO1-')],
              [sg.Combo(list_of_baund, default_value=list_of_baund[list_of_baund.index(tp.baudrate)], s=(30,22), enable_events=True, readonly=True, k='-COMBO2-')],
              [sg.Button('Apply', button_color=(sg.YELLOWS[0], sg.BLUES[0]), bind_return_key=True),sg.Push(), sg.Button('EXIT', button_color=(sg.YELLOWS[0], sg.GREENS[0]))]]

    window = sg.Window('Terminal_config', layout,element_justification='c', font=('Helvetica', ' 13'), default_button_element_size=(8, 2),
                       use_default_focus=False)

    while True:  # The Event Loop
        event, value = window.read()
        if event in (sg.WIN_CLOSED, 'EXIT'):
            window.close()
            return
        elif event == 'Apply':
            window.close()
            tp.baudrate_change_send(list_of_baund.index(value['-COMBO2-']) + 1)
            tp.change_COM(value['-COMBO1-'].split(' ')[0])
            tp.change_baundrate(value['-COMBO2-'])
            return



if __name__ == '__main__':

    #print(tp.serial_ports())
    tp.initial_Uart_avoid_cut()
    menu_GUI()