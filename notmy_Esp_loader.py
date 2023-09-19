import io
import queue
import traceback

import AWSIoTPythonSDK.MQTTLib as AWSIoTPyMQTT
import PySimpleGUI as GUI
from PIL import Image


class Application:
    def __init__(self):
        self.myAWSIoTMQTTClient = None
        self.gui_queue = queue.Queue()

        ######## GUI ########
        middle_font = ('Helvetica', 14)
        context_font = ('Helvetica', 12)
        GUI.theme('DarkGrey14')

        col1 = [[GUI.Column([
            [GUI.Frame('MQTT Panel', [[GUI.Column([
                [GUI.Text('Client Id:', font=middle_font)],
                [GUI.Input('Python_Client', key='_CLIENTID_IN_', size=(19, 1), font=context_font),
                 GUI.Button('Connect', key='_CONNECT_BTN_', font=context_font)],
                [GUI.Text('Notes:', font=middle_font)],
                [GUI.Multiline(key='_NOTES_', autoscroll=True, size=(26, 34), font=context_font, )],
            ], size=(235, 640), pad=(0, 0))]], font=middle_font)], ], pad=(0, 0), element_justification='c')]]

        col2 = [[GUI.Column([[GUI.Frame('CAM {}'.format((row + col)), [
            [GUI.Image(key='_ESP32/CAM_{}_'.format((row + col)), size=(480, 320))],
        ], font=middle_font) for row in range(0, 3, 2)]], pad=(0, 0), element_justification='c')] for col in range(2)]

        layout = [[
            GUI.Column(col1), GUI.Column(col2)
        ]]

        self.window = GUI.Window('Python MQTT Client - AWS IoT -', layout)
        ######## GUI ########

        while True:
            event, values = self.window.Read(timeout=5)
            if event is None or event == 'Exit':
                break

            if event == '_CONNECT_BTN_':
                if self.window[event].get_text() == 'Connect':

                    if len(self.window['_CLIENTID_IN_'].get()) == 0:
                        self.popup_dialog('Client Id is empty', 'Error', context_font)
                    else:
                        self.window['_CONNECT_BTN_'].update('Disconnect')
                        self.aws_connect(self.window['_CLIENTID_IN_'].get())
                
                else:
                    self.window['_CONNECT_BTN_'].update('Connect')
                    self.aws_disconnect()
            try:
                message = self.gui_queue.get_nowait()
            except queue.Empty:
                message = None
            if message is not None:
                _target_ui = message.get("Target_UI")
                _image = message.get("Image")
                self.window[_target_ui].update(data=_image)

        self.window.Close()

    def aws_connect(self, client_id):
        ENDPOINT = "a3611c7i1de58-ats.iot.ap-southeast-1.amazonaws.com"
        PATH_TO_CERT = "esp_device_certificate.txt"
        PATH_TO_KEY = "esp_private_key.txt"
        PATH_TO_ROOT = "esp_root_credentials.txt"

        self.myAWSIoTMQTTClient = AWSIoTPyMQTT.AWSIoTMQTTClient(client_id)
        self.myAWSIoTMQTTClient.configureEndpoint(ENDPOINT, 8883)
        self.myAWSIoTMQTTClient.configureCredentials(PATH_TO_ROOT, PATH_TO_KEY, PATH_TO_CERT)

        try:
            if self.myAWSIoTMQTTClient.connect():
                self.add_note('[MQTT] Connected')
                for i in range(1):
                    self.mqtt_subscribe('esp32/cam_1'.format(i))

            else:
                self.add_note('[MQTT] Cannot Access AWS IOT')
        except Exception as e:
            tb = traceback.format_exc()
            GUI.Print(f'An error happened.  Here is the info:', e, tb)

    def aws_disconnect(self):
        if self.myAWSIoTMQTTClient is not None:
            self.myAWSIoTMQTTClient.disconnect()
            self.add_note('[MQTT] Successfully Disconnected!')

    def mqtt_subscribe(self, topic):
        if self.myAWSIoTMQTTClient.subscribe(topic, 0, lambda client, userdata, message: {

            self.gui_queue.put({"Target_UI": "_{}_".format(str(message.topic).upper()),
                                "Image": self.byte_image_to_png(message)})
        }):
            self.add_note('[MQTT] Topic: {}\n-> Subscribed'.format(topic))
        else:
            self.add_note('[MQTT] Cannot subscribe\nthis Topic: {}'.format(topic))

    def add_note(self, note):
        note_history = self.window['_NOTES_'].get()
        self.window['_NOTES_'].update(note_history + note if len(note_history) > 1 else note)

    def byte_image_to_png(self, message):
        try:
            bytes_image = io.BytesIO(message.payload)
            Image.LOAD_TRUNCATED_IMAGES = True
            picture = Image.open(bytes_image)
            
            im_bytes = io.BytesIO()
            picture.save(im_bytes, format="PNG")
            return im_bytes.getvalue()
        except:
            return None

    def popup_dialog(self, contents, title, font):
        GUI.Popup(contents, title=title, keep_on_top=True, font=font)


if __name__ == '__main__':
    Application()
