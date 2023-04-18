#Importaciones
import tkinter as tk

#Define la app
app = tk.Tk()
app.geometry("200x100")

#Eventos de tecla
def callbackw(event):
    label["text"] = "You pressed w"
    print("w")
def callbacka(event):
    label["text"] = "You pressed a"
    print("a")
def callbacks(event):
    label["text"] = "You pressed s"
    print("s")
def callbackd(event):
    label["text"] = "You pressed d"
    print("d")
def callbackq(event):
    label["text"] = "You pressed q"
    print("q")
def callbacke(event):
    label["text"] = "You pressed e"
    print("e")
def callbackz(event):
    label["text"] = "You pressed z"
    print("z")
def callbackc(event):
    label["text"] = "You pressed c"
    print("c")
def callbackx(event):
    label["text"] = "You pressed x"
    print("x")
def callbackUp(event):
    label["text"] = "You pressed Up"
    print("Up")
def callbackLeft(event):
    label["text"] = "You pressed Left"
    print("Left")
def callbackDown(event):
    label["text"] = "You pressed Down"
    print("Down")
def callbackRight(event):
    label["text"] = "You pressed Right"
    print("Right")

#Asignamos función a las teclas
app.bind('<q>', callbackq)
app.bind('<w>', callbackw)
app.bind('<e>', callbacke)
app.bind('<a>', callbacka)
app.bind('<d>', callbackd)
app.bind('<z>', callbackz)
app.bind('<x>', callbackx)
app.bind('<c>', callbackc)
app.bind('<Up>', callbackUp)
app.bind('<Left>', callbackLeft)
app.bind('<Down>', callbackDown)
app.bind('<Right>', callbackRight)

#
label = tk.Label(app, text="")
label.pack()

#Bucle principal
app.mainloop()