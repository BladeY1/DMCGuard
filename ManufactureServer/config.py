MAN_ADDR = "101.43.239.148:5001"
ZIGBEE_ADDR = "101.43.239.148:5002"
SPEAKER_ADDR = "101.43.239.148:5003"
ZIGBEE_IP = "101.43.239.148"
SPEAKER_IP = "101.43.239.148"
ca='cert/server/rootCA.pem'
DEVICE_PIN="test"
Light_Brightness="50"
Light_Status="on"
Light_Color="yellow"

status_need_query = True
#为True时需要向设备查询状态

channel_code = {'mqtt':'0','zigbee':'1','speaker':'2','local':'3'}
ChannelStatus={'mqtt':1,'zigbee':1,'speaker':1,'local':1}
opcode_map={('zigbee','on'):'Z',('zigbee','off'):'z',('speaker','on'):'S',('speaker','off'):'s',('mqtt','off'):'e',('local','on'):'L',('local','off'):'l'}


#pubsub config
endpoint="a2vo7hd1vjfead-ats.iot.us-west-2.amazonaws.com"
port=443
cert="cert/aws/17f5-certificate.pem.crt"
key="cert/aws/17f5-private.pem.key"
root_ca="cert/aws/AmazonRootCA1.pem"
client_id="17f5-pubsub-client-1"
use_websocket=False
action='store_true'
signing_region='us-west-2'
class topics():
    dev_StatusCommand="Light_01-StatusCommand"
    dev_StatusReport="Light_01-StatusReport"
    StatusReport="Light_01-StatusReport"
    DeviceDmcCommand="Light_01-DeviceDmcCommand"
    DeviceDmcReport="Light_01-DeviceDmcReport"
    OperateCommand="Light_01-OperateCommand"
    OperateCommandReport="Light_01-OperateCommandReport"
    TokenCommand="Light_01-TokenCommand"
    TokenReport="Light_01-TokenReport"
    reports=[dev_StatusReport,StatusReport,DeviceDmcReport,OperateCommandReport,TokenReport]

# operation request payload format:
# channel,right,param,token
# e.g.:   
# 0,1,1,abcdefghijklmnopqrstuvwxyz123456
# means set light status to on

# token request payload format:
# param,channel,right,token
# e.g.:   
# a/d,0,1,abcdefghijklmnopqrstuvwxyz123456
# means add/del a token in channel zigbee in right 1

#R定义 0 1 2 3 4 5
#读开关 写开关 读亮度 写亮度 读颜色 写颜色
#Param定义
#开关  0：开 1：关
#亮度  0：+20 1：-20
#颜色  4红 5黄 6绿
#通道定义mqtt=0,zigbee=1,speaker=2