# Importaciones
import tkinter as tk

# Define la app
app = tk.Tk()
app.geometry("200x100")

key_presses = []

# Key callback for button press
def callback_press(event):
    try:
        key_presses.index(event.keysym)
    except:
        key_presses.append(event.keysym)
        label["text"] = key_presses
        print(f"After add: {key_presses}")
        if len(key_presses) == 2:
            # Si se han pulsado dos teclas a la vez, realizar una acción especial
            if 'w' in key_presses and 'd' in key_presses:
                # Acción cuando se ha pulsado w y d a la vez
                print("Mover hacia arriba y hacia la derecha")
            elif 'w' in key_presses and 'a' in key_presses:
                # Acción cuando se ha pulsado w y a a la vez
                print("Mover hacia arriba y hacia la izquierda")
            elif 's' in key_presses and 'd' in key_presses:
                # Acción cuando se ha pulsado s y d a la vez
                print("Mover hacia abajo y hacia la derecha")
            elif 's' in key_presses and 'a' in key_presses:
                # Acción cuando se ha pulsado s y a a la vez
                print("Mover hacia abajo y hacia la izquierda")
        else:
            # Si solo se ha pulsado una tecla, realizar una acción normal
            if 'w' in key_presses:
                # Acción cuando se ha pulsado la tecla w
                print("Avanzar hacia adelante")
            elif 'a' in key_presses:
                # Acción cuando se ha pulsado la tecla a
                print("Mover hacia la izquierda")
            elif 's' in key_presses:
                # Acción cuando se ha pulsado la tecla s
                print("Avanzar hacia atrás")
            elif 'd' in key_presses:
                # Acción cuando se ha pulsado la tecla d
                print("Mover hacia la derecha")

# Key callback for button release
def callback_release(event):
    try:
        key_presses.remove(event.keysym)
        print(f"After del: {key_presses}")
        label["text"] = key_presses
    except:
        label["text"] = key_presses

# Bindings for key press & release
app.bind("<KeyPress>", callback_press)
app.bind("<KeyRelease>", callback_release)
label = tk.Label(app, text="Press any key")
label.pack()

# Bucle principal
app.mainloop()
