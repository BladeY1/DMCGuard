try:
    import sys
    from time import *
    import socket
    import os
    import requests
    import localconfig
    from datetime import datetime

except ImportError:
    print("Import error occurred")
    exit(0)


# --------------------------------------------------#
# Function: open serial port, exit if having error
# --------------------------------------------------#
#print("waiting for connection")
#hot_point_socket = socket.socket()

#hot_point_socket.bind(('192.168.31.147', 443)) 
#hot_point_socket.listen(5)
#to_client, addr = hot_point_socket.accept()
#print("connected")


with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
    s.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
    s.bind((localconfig.Hub_HOST, localconfig.Hub_socket_PORT))
    s.listen()

    print(f'Server listening on port {localconfig.Hub_socket_PORT}')
    to_client, addr = s.accept()

    print(f'Connection with {addr} closed')
# --------------------------------------------------#
# Function: get device_key form server, send the key to device insuring that the device is certified
# --------------------------------------------------#


def main() -> None:
    """ """
    
    status_token: str = getToken(R="0")
    brightness_token: str = getToken(R="2")
    color_token: str = getToken(R="4")
    # --------------------------------------------------#
    # Function: transfer msg between device and server
    # when sending msg to the device, if timeout( > 5s), reconnect to the device
    # --------------------------------------------------#

    # init data_send_to_server
    status: str = "init"
    brightness: str = "init"
    color: str = "init"
    while True:

        # --------------------------------------------------#
        # first get status and brightness from device
        # --------------------------------------------------#

        data_send_status = "0" + "1"+ status_token#???
        print("sending...")
        to_client.send(data_send_status.encode())
        print("sent???")
        status=to_client.recv(4)
        print("status:")
        print(status)
        #if status == None:
            #return
        print("getting brightness token")
        data_send_brightness = "2" + "0" +brightness_token
	#print("socket sending brightness ")
        to_client.send(data_send_brightness.encode())
        brightness=to_client.recv(4)
        print("brightness:")
        print(brightness)
	#print("brightness above")
        data_send_color="4"+"8"+color_token
        to_client.send(data_send_color.encode())
        color=to_client.recv(4)
    # 新代码结束
       # if color[0]=='4':
        #        color="red"
        #else if color[0]=='5'
         #       color="yellow"
        #else if color[0]=='6'
         #       color="green"
        #if brightness == None:
        #    return

        # --------------------------------------------------#
        # Function: get operation msg from server by posting status and brightness
        # --------------------------------------------------#
        try:
            print("trying to get operation msg...")
            response = requests.post(
                url=localconfig.speaker_server_url + "/api/status",
                data={
                    "key": localconfig.server_key,
                    "status": status,
                    "brightness": brightness,
	            "color":color,
                },
                verify=localconfig.ca
            )
            data_response = response.json()
            # 以下是新增的代码
            now = datetime.now()
            current_time = now.strftime("%H:%M:%S.%f")[:-4]  # 取消注   释此行以获取时分秒，精确到小数点后两位的秒
            print("Hub接收：Current Time =", current_time)
            print(data_response)
        except requests.exceptions.RequestException:
            print("Server is not connected")
            sleep(2)
            continue
        
        # --------------------------------------------------#
        # Function: send operation msg to device
        # --------------------------------------------------#

        if data_response["msg"] == "success":
            data_send_operate = (
                data_response["R"]
                + data_response["param"]
                + data_response["token"]
            )
            print(data_send_operate)
            to_client.send(data_send_operate.encode())
            data_send_status=to_client.recv(4)
            print(data_send_status)
        


# --------------------------------------------------#
# get designated token by R
# --------------------------------------------------#
def getToken(R: str) -> str:
    token: str = ""
    while token == "" or token == None:
        try:
            print("try to get posts")
            response = requests.post(
                url=localconfig.speaker_server_url + "/api/token",
                data={"key": localconfig.server_key, "R": R},
                verify=localconfig.ca
            )
            data_response = response.json()
            print("data response:")
            print(data_response)
        except requests.exceptions.RequestException:
            print("Server is not connected")
            sleep(2)
            continue
        if data_response["msg"] == "success":
            print("token msg succeed")
            token = data_response["token"]
            if token == None:
                print("error:token is none")
                continue
            print("Token {} recieved for R {}".format(token, R))
        else :
            print("error: data_response.msg is not success")
    return token



try:
    if __name__ == "__main__":
        while True:
            main()
except KeyboardInterrupt:
    socket.close()
    print("\n\nProgram terminated by user")

