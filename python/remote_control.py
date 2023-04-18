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
