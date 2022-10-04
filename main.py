import re
import tkinter as tk
from tkinter import filedialog
import tkinter.scrolledtext as tkst
import random

class Mesh:
    def __init__(self):
        self.vertexs = []

    def setName(self, name):
        self.name = name

    def addVertex(self, x, y, z):
        self.vertexs.append([x, y, z])

    def read(self):
        print(self.name)
        print(self.vertexs)

    def getVertexs(self):
        return self.vertexs

    def getName(self):
        return self.name

def CPP(Mesh):
    vertexs = Mesh.getVertexs()
    output = []
    output.append('void ' + formatName(Mesh.getName())+'(){\n')
    output.append('   glBegin("GL_POLYGON")\n')
    for vertex in vertexs:
        x, y, z = vertex
        output.append('   glVertex3f({},{},{})\n'.format(x, y, z))
    output.append('   glEnd()\n')
    output.append('}\n')
    return output


def formatName(name):
    return name.replace('.', '_')

def convert():
    objFile = filedialog.askopenfilename(
        initialdir="./models", title="Select file", filetypes=(("obj files", "*.obj"), ("all files", "*.*")))
    # obj = open(objFile).read().split('\n')
    print(objFile)
    objCount = 0
    objects = []

    reComp = re.compile("(?<=^)(v |vn |vt |f )(.*)(?=$)", re.MULTILINE)

    with open(objFile) as f:
        data = [line.group()
                for line in reComp.finditer(f.read().replace("\t", ""))]
    v_arr, vn_arr, vt_arr, f_arr = [], [], [], []
    for line in data:
        tokens = line.split(' ')
        if tokens[0] == 'v':
            v_arr.append([float(c) for c in tokens[1:]])
        elif tokens[0] == 'vn':
            vn_arr.append([float(c) for c in tokens[1:]])
        elif tokens[0] == 'vt':
            vn_arr.append([float(c) for c in tokens[1:]])
        elif tokens[0] == 'f':
            f_arr.append([[int(i) if len(i) else 0 for i in c.split('/')]
                          for c in tokens[1:]])
    vertices, normals = [], []
    for face in f_arr:
        print("glBegin(GL_POLYGON);")
        app.setOutput("glBegin(GL_POLYGON);\n")
        Red = random.random()
        Green = random.random()
        Blue = random.random()
        colour = 'glColor3f('+str(Red)+','+str(Green)+','+str(Blue)+');\n'
        app.setOutput(colour)
        vert = [v_arr[tp[0]-1] for tp in face]
        for v in vert:
            # print("glVertex3f(", ",".join(
            #     [str(ve).split(".")[0]+"0" for ve in v]), ");\n")
            app.setOutput('glVertex3d(')
            app.setOutput(",".join([str(ve) for ve in v]))
            app.setOutput(");\n")

        for tp in face:
            vertices += v_arr[tp[0]-1]
            normals += vn_arr[tp[2]-1]
        print("glEnd();")
        app.setOutput("glEnd();\n")
    print(vertices)

    

class Application(tk.Frame):
    def __init__(self, master=None):
        super().__init__(master)
        self.master = master
        self.pack()
        self.create_widgets()

    def create_widgets(self):
        self.output = tkst.ScrolledText(master=self,
                                        wrap=tk.WORD,
                                        width=60,
                                        height=20)
        self.select = tk.Button(self, text="Select obj file", fg="black",
                                command=convert)
        self.select.pack(side="bottom")
        self.output.pack(side="right")

    def setOutput(self, text):
        self.output.insert(tk.INSERT, text)


root = tk.Tk()
app = Application(master=root)


app.mainloop()