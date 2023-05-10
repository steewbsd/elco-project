# Imports
import tkinter as tk
from PIL import Image
from PIL import ImageTk
import socket
import time
import cv2
import imutils

# Define la app
app = tk.Tk()
'''app.geometry('600x400')'''

now = time.time()

# Crear un socket UDP
UDP_IP = "192.168.4.2"  # Dirección IP de destino
UDP_PORT = 6667  # Puerto de destino
sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
sock.bind((UDP_IP, UDP_PORT))

def iniciar():
    global cap
    # Capturar el flujo de vídeo desde el socket UDP
    # cap = cv2.VideoCapture("udp://{}:{}".format(UDP_IP, UDP_PORT))
    data, addr = sock.recvfrom(1024)  # Recibir hasta 1024 bytes de datos
    print("Recibido:", data.decode())
    if cap is not None:
        print('Conectado correctamente')
    visualizar()

def visualizar():
    global cap
    if cap is not None:
        print('Envia cosas')
        ret, frame = cap.read()
        if ret == True:
            frame = imutils.resize(frame, width=640)
            frame = cv2.cvtColor(frame, cv2.COLOR_BGR2RGB)
            im = Image.fromarray(frame)
            img = ImageTk.PhotoImage(image=im)
            label.configure(image=img)
            label.image = img
            label.after(10, visualizar)
        else:
            label.image = ""
            cap.release()

def finalizar():
    global cap
    cap.release()

cap = None

# Dirección IP y puerto del servidor TCP
TCP_IP = '192.168.4.1'
TCP_PORT = 6666

# Crear una conexión TCP con el servidor
cliente = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
cliente.connect((TCP_IP, TCP_PORT))

key_presses = []

# Enviar un mensaje TCP al servidor con la parada
def parar_motor():
    movement = 0xAA0100FF
    mensaje = bytes(str(movement),'utf8')
    cliente.send(mensaje)

# Enviar un mensaje TCP al servidor con el movimiento hacia adelante
def mov_adelante():
    movement = 0xAA010FFF
    mensaje = bytes(str(movement),'utf8')
    cliente.send(mensaje)

def mov_atras():
    # Enviar un mensaje TCP al servidor con el movimiento hacia atras
    movement = 0xAA01F0FF
    mensaje = bytes(str(movement),'utf8')
    cliente.send(mensaje)

# Enviar un mensaje TCP al servidor con el movimiento hacia la derecha
def mov_der():
    movement = 0xAA0169FF
    mensaje = bytes(str(movement),'utf8')
    cliente.send(mensaje)

# Enviar un mensaje TCP al servidor con el movimiento hacia la izquierda
def mov_izq():
    movement = 0xAA0196FF
    mensaje = bytes(str(movement),'utf8')
    cliente.send(mensaje)

# Enviar un mensaje TCP al servidor con el movimiento giro adelante derecha
def giro_adelante_der():
    movement = 0xAA010AFF
    mensaje = bytes(str(movement),'utf8')
    cliente.send(mensaje)

# Enviar un mensaje TCP al servidor con el movimiento giro adelante izquierda
def giro_adelante_izq():
    movement = 0xAA0105FF
    mensaje = bytes(str(movement),'utf8')
    cliente.send(mensaje)

# Enviar un mensaje TCP al servidor con el movimiento giro atras derecha
def giro_atras_der():
    movement = 0xAA01A0FF
    mensaje = bytes(str(movement),'utf8')
    cliente.send(mensaje)

# Enviar un mensaje TCP al servidor con el movimiento giro atras izquierda
def giro_atras_izq():
    movement = 0xAA0150FF
    mensaje = bytes(str(movement),'utf8')
    cliente.send(mensaje)

# Enviar un mensaje TCP al servidor con el movimiento diagonal alante derecha
def diag_der_alante():
    movement = 0xAA0109FF
    mensaje = bytes(str(movement),'utf8')
    cliente.send(mensaje)

# Enviar un mensaje TCP al servidor con el movimiento diagonal alante izquierda
def diag_izq_alante():
    movement = 0xAA0106FF
    mensaje = bytes(str(movement),'utf8')
    cliente.send(mensaje)

# Enviar un mensaje TCP al servidor con el movimiento diagonal atras derecha
def diag_der_atras():
    movement = 0xAA0190FF
    mensaje = bytes(str(movement),'utf8')
    cliente.send(mensaje)

# Enviar un mensaje TCP al servidor con el movimiento diagonal atras izquierda
def diag_izq_atras():
    movement = 0xAA0160FF
    mensaje = bytes(str(movement),'utf8')
    cliente.send(mensaje)

# Enviar un mensaje TCP al servidor con el movimiento giro fijo a la derecha
def rotar_der():
    movement = 0xAA015AFF
    mensaje = bytes(str(movement),'utf8')
    cliente.send(mensaje)

# Enviar un mensaje TCP al servidor con el movimiento giro fijo a la izquierda
def rotar_izq():
    movement = 0xAA01A5FF
    mensaje = bytes(str(movement),'utf8')
    cliente.send(mensaje)

# Enviar un mensaje TCP al servidor con el movimiento giro desde alante
def gir_alante():
    movement = 0xAA0112FF
    mensaje = bytes(str(movement),'utf8')
    cliente.send(mensaje)

# Enviar un mensaje TCP al servidor con el movimiento giro desde atras
def gir_atras():
    movement = 0xAA0184FF
    mensaje = bytes(str(movement),'utf8')
    cliente.send(mensaje)

def cam_up():
    movement = 0xAA0804FF
    mensaje = bytes(str(movement),'utf8')
    cliente.send(mensaje)

def cam_down():
    movement = 0xAA0808FF
    mensaje = bytes(str(movement),'utf8')
    cliente.send(mensaje)

def cam_right():
    movement = 0xAA0801FF
    mensaje = bytes(str(movement),'utf8')
    cliente.send(mensaje)

def cam_left():
    movement = 0xAA0802FF
    mensaje = bytes(str(movement),'utf8')
    cliente.send(mensaje)

def cam_stop():
    movement = 0xAA0800FF
    mensaje = bytes(str(movement),'utf8')
    cliente.send(mensaje)


# Función que determina que acción tomar en los motores en función de las teclas pulsadas
def wheels_control(list):
# Si todas las teclas se pulsan a la vez se para
    if 'a' in key_presses and 's' in key_presses and 'w' in key_presses and 'd' in key_presses:
        # Acción cuando se ha pulsado a, s, w y d a la vez
        print("Parar motores")
        parar_motor()

# Control de rotación de 3 teclas

    elif 'd' in key_presses and 'w' in key_presses and 'a' in key_presses and 'v' in key_presses:
        # Acción cuando se ha pulsado w y a a la vez
        print("Parar motores")
        parar_motor()
    elif 'w' in key_presses and 's' in key_presses and 'd' in key_presses and 'v' in key_presses:
        # Acción cuando se ha pulsado w y a a la vez
        print("Parar motores")
        parar_motor()
    elif 'd' in key_presses and 's' in key_presses and 'a' in key_presses and 'v' in key_presses:
        # Acción cuando se ha pulsado w y a a la vez
        print("Parar motores")
        parar_motor()
    elif 's' in key_presses and 'w' in key_presses and 'a' in key_presses and 'v' in key_presses:
        # Acción cuando se ha pulsado w y a a la vez
        print("Parar motores")
        parar_motor()

# Control de 3 teclas pulsadas a la vez

    elif 'd' in key_presses and 'w' in key_presses and 'a' in key_presses:
        # Acción cuando se ha pulsado d, w y a a la vez
        print("Mover hacia adelante")
        mov_adelante()
    elif 'w' in key_presses and 's' in key_presses and 'd' in key_presses:
        # Acción cuando se ha pulsado w, s y d a la vez
        print("Mover hacia la derecha")
        mov_der()
    elif 'd' in key_presses and 's' in key_presses and 'a' in key_presses:
        # Acción cuando se ha pulsado d, s y a a la vez
        print("Mover hacia atras")
        mov_atras()
    elif 's' in key_presses and 'w' in key_presses and 'a' in key_presses:
        # Acción cuando se ha pulsado s, w y a a la vez
        print("Mover hacia la izquierda")
        mov_izq()

# Control de rotación de 2 teclas

    elif 'w' in key_presses and 'd' in key_presses and 'v' in key_presses:
        # Acción cuando se ha pulsado w y d a la vez
        print("Rotar hacia adelante y hacia la derecha")
        giro_adelante_der()
    elif 'w' in key_presses and 'a' in key_presses and 'v' in key_presses:
        # Acción cuando se ha pulsado w y a a la vez
        print("Rotar hacia adelante y hacia la izquierda")
        giro_adelante_izq()
    elif 's' in key_presses and 'd' in key_presses and 'v' in key_presses:
        # Acción cuando se ha pulsado s y d a la vez
        print("Rotar hacia atras y hacia la derecha")
        giro_atras_der()
    elif 's' in key_presses and 'a' in key_presses and 'v' in key_presses:
        # Acción cuando se ha pulsado s y a a la vez
        print("Rotar hacia atras y hacia la izquierda")
        giro_atras_izq()
    elif 'd' in key_presses and 'a' in key_presses and 'v' in key_presses:
        # Acción cuando se ha pulsado w y a a la vez
        print("Parar motores")
        parar_motor()
    elif 'w' in key_presses and 's' in key_presses and 'v' in key_presses:
        # Acción cuando se ha pulsado w y a a la vez
        print("Parar motores")
        parar_motor()

# Control de 2 teclas pulsadas a la vez

    elif 'w' in key_presses and 'd' in key_presses:
        # Acción cuando se ha pulsado w y d a la vez
        print("Mover hacia adelante y hacia la derecha")
        diag_der_alante()
    elif 'w' in key_presses and 'a' in key_presses:
        # Acción cuando se ha pulsado w y a a la vez
        print("Mover hacia adelante y hacia la izquierda")
        diag_izq_alante()
    elif 's' in key_presses and 'd' in key_presses:
        # Acción cuando se ha pulsado s y d a la vez
        print("Mover hacia atras y hacia la derecha")
        diag_der_atras()
    elif 's' in key_presses and 'a' in key_presses:
        # Acción cuando se ha pulsado s y a a la vez
        print("Mover hacia atras y hacia la izquierda")
        diag_izq_atras()
    elif 'd' in key_presses and 'a' in key_presses:
        # Acción cuando se ha pulsado w y a a la vez
        print("Parar motores")
        parar_motor()
    elif 'w' in key_presses and 's' in key_presses:
        # Acción cuando se ha pulsado w y a a la vez
        print("Parar motores")
        parar_motor()

# Control de rotación de 1 tecla

    elif 'w' in key_presses and 'v' in key_presses:
        # Acción cuando se ha pulsado la tecla w
        print("Rotar por la zona de alante")
        gir_alante()
    elif 'a' in key_presses and 'v' in key_presses:
        # Acción cuando se ha pulsado la tecla a
        print("Rotar hacia la izquierda")
        rotar_izq()
    elif 's' in key_presses and 'v' in key_presses:
        # Acción cuando se ha pulsado la tecla s
        print("Rotar por la zona de atras")
        gir_atras()
    elif 'd' in key_presses and 'v' in key_presses:
        # Acción cuando se ha pulsado la tecla d
        print("Rotar hacia la derecha")
        rotar_der()

# Control de una sola tecla

    elif 'w' in key_presses:
        # Acción cuando se ha pulsado la tecla w
        print("Mover hacia adelante")
        mov_adelante()
    elif 'a' in key_presses:
        # Acción cuando se ha pulsado la tecla a
        print("Mover hacia la izquierda")
        mov_izq()
    elif 's' in key_presses:
        # Acción cuando se ha pulsado la tecla s
        print("Mover hacia atrás")
        mov_atras()
    elif 'd' in key_presses:
        # Acción cuando se ha pulsado la tecla d
        print("Mover hacia la derecha")
        mov_der()

# Si no se cumple ninguna de las condiciones se para
    
    else:
        print("Parar motores")
        parar_motor()

# Función que determina que acción tomar en la cámara en función de las teclas pulsadas
def camera_control(list):
    # Si se han pulsado dos teclas a la vez, realizar una acción especial
    if 'Up' in key_presses and 'Right' in key_presses:
        # Acción cuando se ha pulsado Up y Right a la vez
        cam_stop()
        print("Parar cámara")
    elif 'Up' in key_presses and 'Left' in key_presses:
        # Acción cuando se ha pulsado Up y Left a la vez
        cam_stop()
        print("Parar cámara")
    elif 'Down' in key_presses and 'Right' in key_presses:
        # Acción cuando se ha pulsado Down y Right a la vez
        cam_stop()
        print("Parar cámara")
    elif 'Down' in key_presses and 'Left' in key_presses:
        # Acción cuando se ha pulsado Down y Left a la vez
        cam_stop()
        print("Parar cámara")
    elif 'Right' in key_presses and 'Left' in key_presses:
        # Acción cuando se ha pulsado Right y Left a la vez
        cam_stop()
        print("Parar cámara")
    elif 'Up' in key_presses and 'Down' in key_presses:
        # Acción cuando se ha pulsado Up y Down a la vez
        cam_stop()
        print("Parar cámara")
    elif 'Up' in key_presses:
        # Acción cuando se ha pulsado la tecla Up
        cam_up()
        print("Girar cámara hacia arriba")
    elif 'Left' in key_presses:
        # Acción cuando se ha pulsado la tecla Left
        cam_left()
        print("Girar cámara hacia la izquierda")
    elif 'Down' in key_presses:
        # Acción cuando se ha pulsado la tecla Down
        cam_down()
        print("Girar cámara hacia abajo")
    elif 'Right' in key_presses:
        # Acción cuando se ha pulsado la tecla Right
        cam_right
        print("Girar cámara hacia la derecha")
    else:
        cam_stop()
        print("Parar cámara")

# Key callback for button press
def callback_press(event):
    try:
        key_presses.index(event.keysym)
    except:    
        key_presses.append(event.keysym)
        label["text"] = key_presses
        print(f"After add: {key_presses}")
        wheels_control(key_presses)
        camera_control(key_presses)
    
       
# Key callback for button release
def callback_release(event):
    try:
        key_presses.remove(event.keysym)
        print(f"After del: {key_presses}")
        label["text"] = key_presses
        wheels_control(key_presses)
        camera_control(key_presses)
    except:
        label["text"] = key_presses


btnIniciar = tk.Button(app, text="Iniciar", width=45, command=iniciar)
btnIniciar.grid(column=0, row=0, padx=5, pady=5)

btnFinalizar = tk.Button(app, text="Finalizar", width=45, command=finalizar)
btnFinalizar.grid(column=1, row=0, padx=5, pady=5)

'''label = tk.Label(app)
label.grid(column=0, row=1, columnspan=2)'''

# Bindings for key press & release
app.bind("<KeyPress>", callback_press)
app.bind("<KeyRelease>", callback_release)
label = tk.Label(app, text="Press any key")
label.grid(column=0, row=1, columnspan=2)
'''label.pack()'''

# Bucle principal
app.mainloop()

# Cerrar la conexión TCP con el servidor
cliente.close()