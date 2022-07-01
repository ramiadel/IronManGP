from PyQt5.QtGui import *
from PyQt5 import QtCore
from PyQt5.QtWidgets import *
from PyQt5.QtCore import *
import PyQt5.QtGui 
from IronmanTool import Ui_Ironman
from ctypes import sizeof
from time import sleep, time
from intelhex import IntelHex
import firebase_admin
from firebase_admin import db
import zlib
import speech_recognition


class MainWindow(QMainWindow, Ui_Ironman):
    def __init__(self, *args, **kwargs):
        super(MainWindow, self).__init__(*args, **kwargs)
        self.setupUi(self)
        self.path = ""
        self.cred_object = firebase_admin.credentials.Certificate('ironman-ef172-firebase-adminsdk-ke32y-8b6f09fed5.json')
        self.default_app = firebase_admin.initialize_app(self.cred_object, {'databaseURL':'https://ironman-ef172-default-rtdb.firebaseio.com/'})
        self.recognizer = speech_recognition.Recognizer()
        size = QtCore.QSize(1000, 400)
        self.movie = QMovie("iron-man.gif") # Create a QMovie from our gif
        self.movie.setScaledSize(size)
        self.label_4.setMovie(self.movie) # use setMovie function in our QLabe     
        self.movie.start() # now start the gif
        self.movie.stop()
        self.show()
        self.UploadHex.clicked.connect(self.upload_hex)
        self.FlashHex.clicked.connect(self.Flash_hex)
        self.openPC.clicked.connect(self.Open_by_PC)
        self.VR_BTN.clicked.connect(self.Open_by_VR)

    def upload_hex(self):
        self.path, _ = QFileDialog.getOpenFileName(self, "Open file", "", "Hex file (*.hex)")
        self.guiStatus.setText("Hex uploaded")

    def Flash_hex(self):
        
        self.guiStatus.setText("Flashing")
        QApplication.processEvents()
        self.movie.start()
        hex_obj = IntelHex()
        if(self.path == ""):
            self.guiStatus.setText("Please upload Hex")
        else:
            hex_obj.fromfile(self.path,format='hex')
            ##################### Get Data and CRC ############################
            bin_data = hex_obj.tobinarray() 
            CRC= zlib.crc32(bin_data)
            CRC=CRC.to_bytes(4, 'big')
            CRC=list(CRC)
            bin_data = bin_data.tolist() 
            dataSize = len(bin_data)
            dataChunks=[]
            st=0
            end=256
            ChunkSizes=[]
            while(dataSize):
                b=[str(x) for x in bin_data[st:end]]
                dataChunks.append('-'.join(b)+"-")
                if( dataSize >= 256):
                    st=end
                    end=end+256
                    dataSize-=256
                elif (dataSize % 256 != 0 and dataSize < 256):
                    st=end
                    end=end+dataSize
                    dataSize-=dataSize
                ChunkSizes.append(len('-'.join(b)))
            tot_chuncks= len(dataChunks)  
            #################### Get first address ########################
            first_address = list(hex_obj.addresses()[0].to_bytes(4,byteorder='big'))
            #################### Get entry point ##########################
            entry_point = hex_obj.start_addr['EIP']
            entry_point = entry_point.to_bytes(4,byteorder='big')
            entry_point = list(entry_point)
            #################### Get code size ############################
            code_size =len(bin_data).to_bytes(4, byteorder='big')
            code_size = list(code_size)
            #################### Prepare App Info ########################
            AppInfoSent = []
            AppInfo= entry_point + code_size + first_address + CRC
            strAppInfo=[str(x) for x in AppInfo]
            AppInfoSent.append('-'.join(strAppInfo)+"-")
            ############################################## Firebase Code ##############################################
            status = db.reference("/AppStatus")
            App_info = db.reference("/AppInfo")
            Done_uploading = db.reference("/uploadedFromPC")
            blockSize= db.reference("/blockSize")
            totChunks = db.reference("/blockSize/totalChuncks")
            BU_Chunks = db.reference("/blockSize/BackUp_totChuncks")
            dataChunck =db.reference("/dataCh")


            App_info.set(AppInfoSent)
            Done_uploading.set(False)
            dataChunck.update({"data":dataChunks})
            Done_uploading.set(True)
            blockSize.update({"sizes":ChunkSizes})

            BU_Chunks.set(tot_chuncks)
            totChunks.set(tot_chuncks)
            status.set("Flash")
            

            while(status.get()=="Flash"):
                if(status.get()=="Done"):
                    self.guiStatus.setText("Done Flashing")
                if(status.get()=="Error"):
                    self.guiStatus.setText("Error please Flash Again.")

    def Open_by_PC(self):
        Order = db.reference("/IronManHelmet/AppOrder")
        Status = db.reference("/IronManHelmet/Status")
        if(Status.get()=="o"):
            Order.set("\"close\"")
            self.openPC.setText("Open")
            self.movie.start()
        elif(Status.get()=="c"):
            Order.set("\"open\"")
            self.openPC.setText("Close")
            self.movie.stop()
        QApplication.processEvents()

    def Open_by_VR(self):
        Order = db.reference("/IronManHelmet/AppOrder")
        with speech_recognition.Microphone() as src:
            try:
                audio = self.recognizer.adjust_for_ambient_noise(src)
                print("Threshold Value After calibration:" + str(self.recognizer.energy_threshold))
                print("Please speak:")
                audio = self.recognizer.listen(src)
                speech_to_txt = self.recognizer.recognize_google(audio).lower()
                print(speech_to_txt)
                Order.set("\""+speech_to_txt+"\"")
            except Exception as ex:
                print("Sorry. Could not understand.")

if __name__ == '__main__':
    app = QApplication([])
    app.setApplicationName("Failamp")
    app.setStyle("Fusion")
    palette = QPalette()
    palette.setColor(QPalette.Window, QColor(33, 33, 33))
    palette.setColor(QPalette.WindowText, Qt.white)
    palette.setColor(QPalette.Base, QColor(25, 25, 25))
    palette.setColor(QPalette.AlternateBase, QColor(53, 53, 53))
    palette.setColor(QPalette.ToolTipBase, Qt.white)
    palette.setColor(QPalette.ToolTipText, Qt.white)
    palette.setColor(QPalette.Text, Qt.white)
    palette.setColor(QPalette.Button, QColor(53, 53, 53))
    palette.setColor(QPalette.ButtonText, Qt.white)
    palette.setColor(QPalette.BrightText, Qt.red)
    palette.setColor(QPalette.Link, QColor(42, 130, 218))
    palette.setColor(QPalette.Highlight, QColor(42, 130, 218))
    palette.setColor(QPalette.HighlightedText, Qt.black)
    app.setPalette(palette)
    app.setStyleSheet("QToolTip { color: #ffffff; background-color: #2a82da; border: 1px solid white; }")

    window = MainWindow()
    app.exec_()