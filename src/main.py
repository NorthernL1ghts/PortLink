import cv2
import os
import requests
from datetime import datetime

class Application:
    def __init__(self):
        self.cascadeURL = "https://raw.githubusercontent.com/opencv/opencv/master/data/haarcascades/haarcascade_frontalface_default.xml"
        self.cascadePath = "../haarcascade_frontalface_default.xml"
        self.faceCascade = None
        self.videoCapture = None

    def DownloadHaarCascade(self):
        if not os.path.exists(self.cascadePath):
            print("[INFO] Downloading Haar cascade...")
            response = requests.get(self.cascadeURL)
            with open(self.cascadePath, "wb") as file:
                file.write(response.content)
            print("[INFO] Download complete.")
        else:
            print("[INFO] Haar cascade already exists.")

    def GetTime(self):
        return datetime.now().strftime("%H:%M:%S")

    def GetDate(self):
        return datetime.now().strftime("%Y-%m-%d")

    def FormatTime(self, timeStr):
        return timeStr

    def FormatDate(self, dateStr):
        return dateStr

    def Initialize(self):
        self.DownloadHaarCascade()

        self.faceCascade = cv2.CascadeClassifier(self.cascadePath)
        if self.faceCascade.empty():
            print("[ERROR] Failed to load cascade.")
            return False

        self.videoCapture = cv2.VideoCapture(0)
        if not self.videoCapture.isOpened():
            print("[ERROR] Cannot access webcam.")
            return False

        return True

    def Run(self):
        if not self.Initialize():
            return

        # Set the window title
        cv2.namedWindow("PortLink - Python Prototype")

        print("[INFO] Press 'q' to quit.")

        while True:
            ret, frame = self.videoCapture.read()
            if not ret:
                print("[ERROR] Failed to grab frame.")
                break

            grayFrame = cv2.cvtColor(frame, cv2.COLOR_BGR2GRAY)
            detectedFaces = self.faceCascade.detectMultiScale(grayFrame, scaleFactor=1.1, minNeighbors=5)

            for (faceX, faceY, faceWidth, faceHeight) in detectedFaces:
                cv2.rectangle(frame, (faceX, faceY), (faceX + faceWidth, faceY + faceHeight), (255, 255, 255), 2)

            currentTime = self.FormatTime(self.GetTime())
            currentDate = self.FormatDate(self.GetDate())

            cv2.putText(frame, f"Time: {currentTime}", (10, 25), cv2.FONT_HERSHEY_SIMPLEX, 0.6, (255, 255, 255), 2)
            cv2.putText(frame, f"Date: {currentDate}", (10, 50), cv2.FONT_HERSHEY_SIMPLEX, 0.6, (255, 255, 255), 2)

            cv2.imshow("PortLink - Python Prototype", frame)

            if cv2.waitKey(1) & 0xFF == ord('q'):
                break

        self.Cleanup()

    def Cleanup(self):
        if self.videoCapture:
            self.videoCapture.release()
        cv2.destroyAllWindows()

def Main():
    app = Application()
    app.Run()

if __name__ == "__main__":
    Main()
