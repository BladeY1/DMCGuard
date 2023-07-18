import datetime
import json
import os
import hashlib
from uuid import uuid4
from flask import Flask, request, session
from flask import jsonify
from flask_sqlalchemy import SQLAlchemy
from sqlalchemy import text
import PubSub
import requests
from config import DEVICE_PIN, ZIGBEE_ADDR, SPEAKER_ADDR, ChannelStatus, ZIGBEE_IP, SPEAKER_IP
import config

app = Flask(__name__)
# 使用 Windows 系统时 URI 前缀部分需要写入三个斜线 (即 sqlite:///) 否则为4个斜线
app.config["SECRET_KEY"] = "secret key"
app.config["SQLALCHEMY_DATABASE_URI"] = "sqlite:///" + os.path.join(
    app.root_path, "data.db"
)
app.config["SQLALCHEMY_TRACK_MODIFICATIONS"] = True
db = SQLAlchemy(app)
ssl_keys = (
    "cert/server/101.43.239.148+2.pem",
    "cert/server/101.43.239.148+2-key.pem",
)
#ssl_keys = (
#    "cert/server/10.0.16.12.pem",
#    "cert/server/10.0.16.12-key.pem",
#)
#ssl_keys = (
#    "cert/server/manufacture-server.com+3.pem",
#    "cert/server/manufacture-server.com+3-key.pem",
#)
# ssl_keys=(
#     "cert/server/192.168.1.185.pem",
#     "cert/server/192.168.1.185-key.pem",
# )

# 重写PubSub.py中的on_message_received函数


def on_message_received(topic, payload):
	with app.app_context():
        # print("Received message from topic '{}': {}".format(
        #    topic, payload.decode('utf-8')))
        rece = payload.decode('utf-8')
        # print(rece)
        recordLog(Log(type='Device',topic=topic,msg=rece))
        if topic == config.topics.dev_StatusReport:
            pass
            #config.Light_Brightness = rece
        elif topic == config.topics.OperateCommandReport:
            config.status_need_query = True
            if int(rece)==1:
                config.Light_Status = 'on'
            elif int(rece)==2:
                config.Light_Status = 'off'
            elif int(rece)==4:
                config.Light_Color = 'red'
            elif int(rece)==5:
                config.Light_Color = 'yellow'
            elif int(rece)==6:
                config.Light_Color = 'green'
            elif int(rece)>10:
                config.Light_Brightness = str(int(rece)-10)
        elif topic == config.topics.DeviceDmcReport:
            config.ChannelStatus['mqtt'] = int(rece[-5])
            config.ChannelStatus['zigbee'] = int(rece[-4])
            config.ChannelStatus['speaker'] = int(rece[-3])
            config.ChannelStatus['local'] = int(rece[-2])
        else:
            print(rece)

PubSub.on_message_received = on_message_received
with app.app_context():
    PubSub.connect()


class User(db.Model):
    id = db.Column(db.Integer, primary_key=True)  # 主键
    name = db.Column(db.String(20))
    password = db.Column(db.String(20))
    device_binded = db.Column(db.Boolean)


class Token(db.Model):
    id = db.Column(db.Integer, primary_key=True)  # 主键
    name = db.Column(db.String(20))
    R = db.Column(db.String(20))
    token = db.Column(db.String(32))

    def to_json(self):
        """将自身实例对象转化为json"""
        item = self.__dict__
        if "_sa_instance_state" in item:
            del item["_sa_instance_state"]
        return item


class Message(db.Model):
    id = db.Column(db.Integer, primary_key=True)  # 主键
    apply_id = db.Column(db.Integer)
    name = db.Column(db.String(20))
    channel = db.Column(db.String(20),default='mqtt')
    R = db.Column(db.String(20))
    # 该申请的状态 ["rejected","accepted","toprocess"]
    status = db.Column(db.String(32))

    def to_json(self):
        """将自身实例对象转化为json"""
        item = self.__dict__
        if "_sa_instance_state" in item:
            del item["_sa_instance_state"]
        return item


class Log(db.Model):
    id = db.Column(db.Integer, primary_key=True)  # 主键
    time = db.Column(db.DateTime, default=datetime.datetime.now)
    type = db.Column(db.Enum('Operate', 'Token', 'Device'), nullable=False)
    # operate
    name = db.Column(db.String(20))
    channel = db.Column(db.String(20))
    R = db.Column(db.String(20))
    Param = db.Column(db.String(20))
    status = db.Column(db.String(32))  # [success","permission denied"]
    # token
    name = db.Column(db.String(20))
    channel = db.Column(db.String(20))
    R = db.Column(db.String(20))
    # ["delete","apply","apply accepted","apply rejected"]
    status = db.Column(db.String(32))
    # device
    topic = db.Column(db.String(32))
    msg = db.Column(db.String(128))

    def to_json(self):
        """将自身实例对象转化为json"""
        item = self.__dict__
        if "_sa_instance_state" in item:
            del item["_sa_instance_state"]
        return item


def recordLog(Log):
    with app.app_context():
        # zigbee服务器应上报使用日志
        # requests.post("https://"+MAN_ADDR + "/api/notification",
        #                  data={"msg": "log", "log":Log.to_json() }, verify=config.ca)
        if Log.R=="0" or Log.R=="2" or Log.R=="4":
            return
        if Log.topic==config.topics.StatusReport or Log.topic==config.topics.DeviceDmcReport:
            return
        db.session.add(Log)
        db.session.commit()


@app.route("/")
def hello_world():
    return "Hello, World!"


@app.route("/api/login", methods=["GET", "POST"])
def login():
    name = request.form.get("name", default="NAME")
    password = request.form.get("password", default="PASSWORD")
    if name == "NAME" or password == "PASSWORD":
        return jsonify({"code": 400, "msg": "name or password is required"})
    if User.query.filter(User.name == name).count() == 0:
        return jsonify({"code": 400, "msg": "user not exist"})
    log_user = User.query.filter(User.name == name).first()
    if log_user.password == password:
        session["name"] = name
        session["logged_in"] = "true"
        return jsonify({"code": 200, "msg": "Hello " + name})
    else:
        return jsonify({"code": 400, "msg": "Login Failed"})


@app.route("/api/register", methods=["GET", "POST"])
def register():
    reg_user = User()
    reg_user.name = request.form.get("name", default="NAME")
    reg_user.password = request.form.get("password", default="PASSWORD")
    reg_user.device_binded = False
    if reg_user.name == "NAME" or reg_user.password == "PASSWORD":
        return jsonify({"code": 400, "msg": "name or password is required"})
    reg_user.password = hashlib.md5(reg_user.password.encode()).hexdigest()
    query = User.query.filter(User.name == reg_user.name)
    if query.count() >= 1:
        return jsonify({"code": 400, "msg": "Username already exists"})
    else:
        db.session.add(reg_user)  # 将变化添加
        db.session.commit()  # 将变化提交
        return jsonify(
            {
                "code": 200,
                "msg": "success",
                "name": reg_user.name,
                "password": reg_user.password,
            }
        )

#暂时写死   在user中添加device_binded字段作为flag
@app.route("/api/bindDevice", methods=["GET", "POST"])
def bindDevice():
    if session.get("logged_in") != "true":
        return jsonify({"code": 400, "msg": "Please login first"})
    name = session.get("name")
    pin = request.form.get("pin", default="NONE")
    if pin == "NONE":
        return jsonify({"code": 400, "msg": "pin is required"}) 
    user = User.query.filter(User.name == name).first()
    if user.device_binded == True:
            return jsonify({"code": 403, "msg": "device already binded"})
    if pin==DEVICE_PIN:
        user.device_binded = True
        db.session.commit()
        #Token 生成
        for R in range(0, 6):
            the_Token = Token(name=name,R=R,token=hashlib.md5(str(uuid4()).encode()).hexdigest())
            db.session.add(the_Token)
            db.session.commit()
            # raspi设备端更新
            to_publish = ",".join(["a", "0", the_Token.R, the_Token.token])
            PubSub.publish(config.topics.TokenCommand, to_publish)
        
        return jsonify({"code": 200, "msg": "success"})
    else:
        return jsonify({"code": 400, "msg": "pin not found"})


@app.route("/api/operate", methods=["GET", "POST"])
def operate():
    if session.get("logged_in") != "true":
        return jsonify({"code": 400, "msg": "Please login first"})
    name = session.get("name")
    user = User.query.filter(User.name == name).first()
    if user.device_binded == False:
        return jsonify({"code": 400, "msg": "Please bind device first"})
    R = request.form.get("R", default="9")
    Param = request.form.get("Param", default="0")
    the_Token = Token.query.filter(
        Token.name == name, Token.R == R)
    if the_Token.count() == 0:
        recordLog(Log(type='Operate', name=name, R=R, Param=Param,
                  channel="mqtt", status="permission denied"))
        return jsonify(
            {"code": 400, "msg": name + ",You don't have the right to operate"}
        )
    else:
        # channel,right,param,token
        to_publish = ",".join(["0", the_Token[0].R, Param, the_Token[0].token])
        PubSub.publish(config.topics.OperateCommand, to_publish)
        config.status_need_query = True
        recordLog(Log(type='Operate', name=name, R=R, Param=Param,
                  channel="mqtt", status="success"))
        return jsonify({"code": 200, "msg": "success"})


@app.route("/api/getstatus", methods=["GET", "POST"])
def getstatus():
    if session.get("logged_in") != "true":
        return jsonify({"code": 400, "msg": "Please login first"})
    name = session.get("name")
    #发送查询请求
    def send_query(R):
        the_Token = Token.query.filter(
            Token.name == name, Token.R == R)
        if the_Token.count() == 0:
            recordLog(Log(type='Operate', name=name, R=R, channel="mqtt", status="permission denied"))
            return jsonify({"code": 400, "msg": name + ",You don't have the right to operate"})
        else:
            to_publish = ",".join(["0", the_Token[0].R, "0", the_Token[0].token])
            PubSub.publish(config.topics.OperateCommand, to_publish,False)
            recordLog(Log(type='Operate', name=name, R=R,channel="mqtt", status="success"))
    if config.status_need_query:
        send_query("0")
        send_query("2")
        send_query("4")
        config.status_need_query = False
    return jsonify({"code": 200, "Light_Status": config.Light_Status, "Light_Brightness": config.Light_Brightness,"Light_Color": config.Light_Color})


@app.route("/api/channel", methods=["GET", "POST"])
def channel():
    if session.get("logged_in") != "true":
        return jsonify({"code": 400, "msg": "Please login first"})
    name = session.get("name")
    user = User.query.filter(User.name == name).first()
    if user.device_binded == False:
        return jsonify({"code": 400, "msg": "Please bind device first"})

    operate = request.form.get("operate", default="NONE")
    target = request.form.get("target", default="NONE")
    param = request.form.get("param", default="NONE")
    if operate == "get":
        return jsonify({"code": 200, "msg": ChannelStatus})
    elif operate == "set" and (target, param) in config.opcode_map:
        #ChannelStatus[target] = 1 if param == 'on' else 0
        # rasp通知
        opcode = config.opcode_map[(target, param)]
        PubSub.publish(config.topics.DeviceDmcCommand, opcode)
        return jsonify({"code": 200, "msg": ChannelStatus})
    else:
        return jsonify({"code": 400, "msg": "operate or target is wrong"})


@app.route("/api/getLogs", methods=["GET", "POST"])
def getLogs():
    if session.get("logged_in") != "true":
        return jsonify({"code": 400, "msg": "Please login first"})
    logs = Log.query.all()
    logarr = []
    for i in logs:
        logarr.append(i.to_json())
    return jsonify(
        {"code": 200, "msg": logarr}
    )

@app.route("/api/getDeviceUser", methods=["GET", "POST"])
def getDeviceUser():
    if session.get("logged_in") != "true":
        return jsonify({"code": 400, "msg": "Please login first"})
    name = session.get("name")
    Tokenlist = Token.query.filter(Token.name != name).all()
    user_name={}
    user_token_status={"name":"","R0":"0","R1":"0","R2":"0","R3":"0","R4":"0","R5":"0"}
    userarr=[]
    #生成用户对应的token状态
    for i in Tokenlist:
        if i.name not in user_name:
            user_name[i.name] = user_token_status.copy()
            user_name[i.name]["name"]=i.name
        user_name[i.name]["R"+i.R] = "1"
    for i in user_name:
        userarr.append(user_name[i])
    return jsonify(
        {"code": 200, "msg": userarr}
    )

@app.route("/api/getMessages", methods=["GET", "POST"])
def getMessages():
    if session.get("logged_in") != "true":
        return jsonify({"code": 400, "msg": "Please login first"})
    name = session.get("name")
    messages = Message.query.all()
    messagesarr = []
    for i in messages:
        messagesarr.append(i.to_json())
    return jsonify(
        {"code": 200, "msg": messagesarr}
    )


# app api   eg:{"apply_id":"","operate":"reject"}
@app.route("/api/processMessages", methods=["GET", "POST"])
def processMessages():
    if session.get("logged_in") != "true":
        return jsonify({"code": 400, "msg": "Please login first"})
    name = session.get("name")
    apply_id = request.form.get("apply_id", default="")
    operate = request.form.get("operate", default="rejected")

    #message_toprocess = Message.query.filter(apply_id)
    message_toprocess = Message.query.filter(text("Message.apply_id = :apply_id")).params(apply_id=apply_id)
    if(message_toprocess.count() == 0):
        return jsonify({"code": 400, "msg": "Message not exists"})
    message_toprocess = message_toprocess.first()
    if(message_toprocess.status != "toprocess"):
        return jsonify({"code": 400, "msg": "Message already processed"})
    #判断通道地址
    ADDR=SPEAKER_ADDR
    if message_toprocess.channel == "zigbee":
        ADDR=ZIGBEE_ADDR
    elif message_toprocess.channel == "speaker":
        ADDR=SPEAKER_ADDR

    # 拒绝请求
    if operate == "reject":
        # 本地数据库更新
        message_toprocess.status = "rejected"
        db.session.commit()
        recordLog(Log(type='Token',name=message_toprocess.name, R=message_toprocess.R,
                  channel=message_toprocess.channel, status="apply rejected"))
        # 通知Zigbee服务器
        requests.post("https://"+ADDR + "/api/notification",
                      data={"msg": "apply processed", "name":message_toprocess.name,"R": message_toprocess.R, "status": "rejected"}, verify=config.ca, timeout=5)
        return jsonify({"code": 200, "msg": "success"})
    # 接受请求
    elif operate == "accept":
        # 本地数据库更新
        message_toprocess.status = "accepted"
        db.session.commit()
        the_Token = Token()
        the_Token.name = message_toprocess.name
        the_Token.R = message_toprocess.R
        recordLog(Log(type='Token',name=the_Token.name, R=the_Token.R,
                  channel=message_toprocess.channel, status="apply accepted"))
        token_exist = Token.query.filter(
            Token.name == message_toprocess.name, Token.R == message_toprocess.R)
        if token_exist.count() > 0:
            return jsonify({"code": 400, "msg": "Token already exists"})
        the_Token.token = hashlib.md5(str(uuid4()).encode()).hexdigest()
        db.session.add(the_Token)
        db.session.commit()
        # raspi设备端更新
        # means add a token in channel 0 in right 1
        channel=config.channel_code[message_toprocess.channel]
        to_publish = ",".join(["a", channel, the_Token.R, the_Token.token])

        PubSub.publish(config.topics.TokenCommand, to_publish)
        # 通知Zigbee服务器
        requests.post("https://"+ADDR + "/api/notification",
                      data={"msg": "apply processed", "status": "accepted", "name": the_Token.name, "R": the_Token.R, "Token": the_Token.token, }, verify=config.ca, timeout=5)
        return jsonify({"code": 200, "msg": "success"})
    else:
        return jsonify({"code": 400, "msg": "operate error"})


# app api   eg:{"name":"test",
#               "R":"lightread"}
@app.route("/api/delToken", methods=["GET", "POST"])
def delToken():
    # 获取要删除的token
    name = request.form.get("name", default="NONE")
    R = request.form.get("R", default="NONE")
    if session.get("logged_in") != "true":
        return jsonify({"code": 400, "msg": "Please login first"})
    if name == "NONE" or R == "NONE":
        return jsonify({"code": 400, "msg": "name,R can't be NONE"})
    # 删除请求的token并提交给数据库
    todel = Token.query.filter(Token.name == name, Token.R == R)
    if todel.count() == 0:
        return jsonify({"code": 400, "msg": "Token not exist"})
    todel_token, todel_R,todel_name =todel.first().token, todel.first().R,todel.first().name
    todel.delete()
    db.session.commit()

    # raspi设备端更新
    to_publish = ",".join(["d", "0", todel_R, todel_token])
    PubSub.publish(config.topics.TokenCommand, to_publish)
    
    recordLog(Log(type='Token',name=todel_name, R=todel_name,
                  channel='mqtt', status="delete"))
    # 向Zigbee服务器发送删除token的消息
    requests.post("https://" +
                  ZIGBEE_ADDR + "/api/notification", data={"msg": "delToken", "name":todel_name,"R":todel_R,"token": todel_token}, verify=config.ca, timeout=5)
    requests.post("https://" +
                  SPEAKER_ADDR + "/api/notification", data={"msg": "delToken", "name":todel_name,"R":todel_R,"token": todel_token}, verify=config.ca, timeout=5)
    return jsonify({"code": 200, "msg": "success"})



##############      以下为服务器间通信接口      ##############

# zigbee服务器向本地服务器发送token申请    参数name,r,o
@app.route("/api/applyToken", methods=["GET", "POST"])
def applyToken():
    # 暂时仅验证申请者的ip
    print(request.remote_addr)
    if request.remote_addr != ZIGBEE_IP and request.remote_addr != SPEAKER_IP:
       return jsonify({"code": 400, "msg": "Not allowed"})


    # 创建token申请message并提交给数据库，等待用户处理
    the_Message = Message()
    the_Message.name = request.form.get("name", default="NONE")
    the_Message.R = request.form.get("R", default="NONE")
    if the_Message.name == "NONE" or the_Message.R == "NONE":
        return jsonify({"code": 400, "msg": "name,R can not be empty"})
    the_Message.status = "toprocess"

    the_Message.channel = the_Message.name
    db.session.add(the_Message)
    db.session.commit()
    the_Message.apply_id = the_Message.id
    db.session.commit()
    recordLog(Log(type='Token',name=the_Message.name, R=the_Message.R,
                  channel=the_Message.channel, status="apply"))
    return jsonify({
        "code": 200,
        "msg": "apply success",
        "apply_id": the_Message.apply_id,
        "name": the_Message.name,
        "R": the_Message.R,
    })


# zigbee服务器实现
@app.route("/api/notification", methods=["GET", "POST"])
def notification():
    print("notification received:", request.json)
    if request.json['msg']=='log':
        log = request.json['log']
        the_log=Log(channel=log["channel"],type=log["type"],name=log["name"],R=log["R"],status=log["status"],Param=log["Param"])
        db.session.add(the_log)
        db.session.commit()
    return {"code": 200, "msg": "success"}

#数据库debug
@app.route("/dbreset", methods=["GET", "POST"])
def dbreset():
    db.drop_all()
    db.create_all()
    return {"code": 200, "msg": "dbreset success"}



if __name__ == "__main__":
    app.run(
        host="0.0.0.0", port=5001, debug=True, ssl_context=ssl_keys, use_reloader=False
    )
