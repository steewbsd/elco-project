# Imports
import tkinter as tk
import socket

# Define la app
app = tk.Tk()
app.geometry("200x100")

# Dirección IP y puerto del servidor TCP
IP = '192.168.4.1'
PUERTO = 6666

# Crear una conexión TCP con el servidor
cliente = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
cliente.connect((IP, PUERTO))

key_presses = []

def parar_motor():
    # Enviar un mensaje TCP al servidor con la parada
    movement = 0xAA0100FF
    mensaje = movement.encode()
    cliente.send(mensaje)

def mov_adelante():
    # Enviar un mensaje TCP al servidor con el movimiento hacia adelante
    movement = 0xAA010FFF
    mensaje = movement.encode()
    cliente.send(mensaje)

def mov_atras():
    # Enviar un mensaje TCP al servidor con el movimiento hacia atras
    movement = 0xAA01F0FF
    mensaje = movement.encode()
    cliente.send(mensaje)

def mov_der():
    # Enviar un mensaje TCP al servidor con el movimiento hacia la derecha
    movement = 0xAA01F0FF
    mensaje = movement.encode()
    cliente.send(mensaje)

def mov_izq():
    # Enviar un mensaje TCP al servidor con el movimiento hacia la izquierda
    movement = 0xAA01F0FF
    mensaje = movement.encode()
    cliente.send(mensaje)

def giro_adelante_der():
    # Enviar un mensaje TCP al servidor con el movimiento giro adelante derecha
    movement = 0xAA01F0FF
    mensaje = movement.encode()
    cliente.send(mensaje)

def giro_adelante_izq():
    # Enviar un mensaje TCP al servidor con el movimiento giro adelante izquierda
    movement = 0xAA01F0FF
    mensaje = movement.encode()
    cliente.send(mensaje)

def giro_atras_der():
    # Enviar un mensaje TCP al servidor con el movimiento giro atras derecha
    movement = 0xAA01F0FF
    mensaje = movement.encode()
    cliente.send(mensaje)

def giro_atras_izq():
    # Enviar un mensaje TCP al servidor con el movimiento giro atras izquierda
    movement = 0xAA01F0FF
    mensaje = movement.encode()
    cliente.send(mensaje)



# Función que determina que acción tomar en los motores en función de las teclas pulsadas
def wheels_control(list):
    if 'a' in key_presses and 's' in key_presses and 'w' in key_presses and 'd' in key_presses:
        # Acción cuando se ha pulsado a, s, w y d a la vez
        print("Parar motores")
        parar_motor()
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
    elif 'w' in key_presses and 'd' in key_presses:
        # Acción cuando se ha pulsado w y d a la vez
        print("Mover hacia adelante y hacia la derecha")
        giro_adelante_der()
    elif 'w' in key_presses and 'a' in key_presses:
        # Acción cuando se ha pulsado w y a a la vez
        print("Mover hacia adelante y hacia la izquierda")
        giro_adelante_izq()
    elif 's' in key_presses and 'd' in key_presses:
        # Acción cuando se ha pulsado s y d a la vez
        print("Mover hacia atras y hacia la derecha")
        giro_atras_der()
    elif 's' in key_presses and 'a' in key_presses:
        # Acción cuando se ha pulsado s y a a la vez
        print("Mover hacia atras y hacia la izquierda")
        giro_atras_izq()
    elif 'd' in key_presses and 'a' in key_presses:
        # Acción cuando se ha pulsado w y a a la vez
        print("Parar motores")
        parar_motor()
    elif 'w' in key_presses and 's' in key_presses:
        # Acción cuando se ha pulsado w y a a la vez
        print("Parar motores")
        parar_motor()
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
    else:
        print("Parar motores")
        parar_motor()

# Función que determina que acción tomar en la cámara en función de las teclas pulsadas
def camera_control(list):
    # Si se han pulsado dos teclas a la vez, realizar una acción especial
    if 'Up' in key_presses and 'Right' in key_presses:
        # Acción cuando se ha pulsado Up y Right a la vez
        print("Parar cámara")
    elif 'Up' in key_presses and 'Left' in key_presses:
        # Acción cuando se ha pulsado Up y Left a la vez
        print("Parar cámara")
    elif 'Down' in key_presses and 'Right' in key_presses:
        # Acción cuando se ha pulsado Down y Right a la vez
        print("Parar cámara")
    elif 'Down' in key_presses and 'Left' in key_presses:
        # Acción cuando se ha pulsado Down y Left a la vez
        print("Parar cámara")
    elif 'Right' in key_presses and 'Left' in key_presses:
        # Acción cuando se ha pulsado Right y Left a la vez
        print("Parar cámara")
    elif 'Up' in key_presses and 'Down' in key_presses:
        # Acción cuando se ha pulsado Up y Down a la vez
        print("Parar cámara")
    elif 'Up' in key_presses:
        # Acción cuando se ha pulsado la tecla Up
        print("Girar cámara hacia arriba")
    elif 'Left' in key_presses:
        # Acción cuando se ha pulsado la tecla Left
        print("Girar cámara hacia la izquierda")
    elif 'Down' in key_presses:
        # Acción cuando se ha pulsado la tecla Down
        print("Girar cámara hacia abajo")
    elif 'Right' in key_presses:
        # Acción cuando se ha pulsado la tecla Right
        print("Girar cámara hacia la derecha")
    else:
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

# Bindings for key press & release
app.bind("<KeyPress>", callback_press)
app.bind("<KeyRelease>", callback_release)
label = tk.Label(app, text="Press any key")
label.pack()

# Bucle principal
app.mainloop()

# Cerrar la conexión TCP con el servidor
cliente.close()

# Cosas de envio TCP
'''import tkinter as tk
import socket

# Dirección IP y puerto del servidor TCP
IP = '127.0.0.1'
PUERTO = 5000

# Crear una conexión TCP con el servidor
cliente = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
cliente.connect((IP, PUERTO))

# Función que se llama cuando se pulsa una tecla
def on_press(event):
    # Obtener la tecla pulsada
    tecla = event.keysym
    # Enviar un mensaje TCP al servidor con la tecla pulsada
    mensaje = tecla.encode()
    cliente.send(mensaje)

# Función que se llama cuando se suelta una tecla
def on_release(event):
    pass

# Crear una ventana Tkinter
ventana = tk.Tk()

# Asociar las funciones on_press y on_release con los eventos de teclado w, a, s y d
ventana.bind('<KeyPress>', on_press)
ventana.bind('<KeyRelease>', on_release)

# Bucle principal de la ventana Tkinter
ventana.mainloop()

# Cerrar la conexión TCP con el servidor
cliente.close()
'''