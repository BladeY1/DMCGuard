import datetime
import time
import os
import hashlib
from flask import Flask, request, session
from flask import jsonify
from flask_sqlalchemy import SQLAlchemy
import requests
import config

app = Flask(__name__)
# 使用 Windows 系统时 URI 前缀部分需要写入三个斜线 (即 sqlite:///) 否则为4个斜线
app.config["SECRET_KEY"] = "secret key"
app.config["SQLALCHEMY_DATABASE_URI"] = "sqlite:///" + os.path.join(
    app.root_path, "data.db"
)
app.config["SQLALCHEMY_TRACK_MODIFICATIONS"] = True
db = SQLAlchemy(app)
#ssl_keys = (
#    "cert/47.95.3.188+1.pem",
#    "cert/47.95.3.188+1-key.pem",
#)

ssl_keys = (
    SSL_KEYS_1,
    SSL_KEYS_2,
)


# 存储已绑定设备用户的cookie，提供给speaker
user_cookie = {}

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
    channel = db.Column(db.String(20), default="zigbee")
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
    type = db.Column(db.Enum("Operate", "Token", "Device"), nullable=False)
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


class Operation:
    def __init__(self, token: str, R: str, param: str, time: float) -> None:
        self.token = token
        self.R = R
        self.param = param
        self.time = time

    def __str__(self) -> str:
        return "Operation change\ntoken: {}\nR: {}\nparam: {}\ntime: {}".format(
            self.token, self.R, self.param, self.time
        )


class singleDevice:
    def __init__(self, key) -> None:
        self.status: str = ""
        self.brightness: str = ""
        self.color: str = ""
        self.key: str = key
        self.operation: Operation = None

    def __str__(self) -> str:
        return "singleDevice\nstatus: {}".format(self.status)

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


# 暂时写死   在user中添加device_binded字段作为flag
@app.route("/api/bindDevice", methods=["GET", "POST"])
def bindDevice():
    if session.get("logged_in") != "true":
        return jsonify({"code": 400, "msg": "Please login first"})
    name = session.get("name")
    pin = request.form.get("pin", default="NONE")
    if pin == "NONE":
        return jsonify({"code": 400, "msg": "pin is required"})
    if pin == config.DEVICE_PIN:
        user = User.query.filter(User.name == name).first()
        user.device_binded = True
        testlight.key = pin
        db.session.commit()
        
        # print(request.headers["Cookie"])
        user_cookie[name] = str(request.headers["Cookie"])

        return jsonify({"code": 200, "msg": "success"})
    else:
        return jsonify({"code": 400, "msg": "pin not found"})

@app.route("/api/getcookies", methods=["GET", "POST"])
def getCookies():
    if request.form.get("key") != config.server_key:
        return {"code": 400, "msg":"Wrong key!"}

    return {"msg":user_cookie, "code":200}

@app.route("/api/operate", methods=["GET", "POST"])
def operate():
    if session.get("logged_in") != "true":
        return jsonify({"code": 400, "msg": "Please login first"})
    name = session.get("name")
    user = User.query.filter(User.name == name).first()
    if user.device_binded == False:
        return jsonify({"code": 400, "msg": "Please bind device first"})
    R = request.form.get("R", default="NONE")
    Param = request.form.get("Param", default="NONE")
    the_Token = Token.query.filter(Token.name == name, Token.R == R)
    if the_Token.count() == 0:
        log = Log(
                type="Operate",
                name=name,
                R=R,
                Param=Param,
                channel="zigbee",
                status="permission denied",
            )
        
        log_temp = {
                'type':"Operate",
                'name':name,
                'R':R,
                'Param':Param,
                'channel':"zigbee",
                'status':"permission denied",
            }
        db.session.add(log)
        db.session.commit()
        requests.post("https://"+config.MAN_ADDR + "/api/notification", json={"msg": "log", "log":log_temp}, verify=False)
        
        return jsonify(
            {"code": 400, "msg": name + ",You don't have the right to operate"}
        )
    else:
        # channel,right,param,token

        testlight.operation = Operation(the_Token.first().token, R, Param, time.time())

        log = Log(
                type="Operate",
                name=name,
                R=R,
                Param=Param,
                channel="zigbee",
                status="success",
            )
        
        log_temp = {
                'type':"Operate",
                'name':name,
                'R':R,
                'Param':Param,
                'channel':"zigbee",
                'status':"success",
            }
        db.session.add(log)
        db.session.commit()
        requests.post("https://"+config.MAN_ADDR + "/api/notification", json={"msg": "log", "log":log_temp}, verify=False)
        
        return jsonify({"code": 200, "msg": "success"})


@app.route("/api/getstatus", methods=["GET", "POST"])
def getstatus():
    if session.get("logged_in") != "true":
        return jsonify({"code": 400, "msg": "Please login first"})

    name = session.get("name")

    if Token.query.filter(Token.name == name, Token.R == "0").count() == 0:
        return jsonify({"code": 400, "msg": "You do not have the right!"})
    
    return jsonify(
        {
            "code": 200,
            "msg": "success",
            "Light_Status": testlight.status,
            "Light_Brightness": testlight.brightness,
            "Light_Color": testlight.color,
        }
    )


@app.route("/api/getLogs", methods=["GET", "POST"])
def getLogs():
    if session.get("logged_in") != "true":
        return jsonify({"code": 400, "msg": "Please login first"})
    logs = Log.query.all()
    logarr = []
    for i in logs:
        logarr.append(i.to_json())
    return jsonify({"code": 200, "msg": logarr})


@app.route("/api/getMessages", methods=["GET", "POST"])
def getMessages():
    if session.get("logged_in") != "true":
        return jsonify({"code": 400, "msg": "Please login first"})
    name = session.get("name")
    messages = Message.query.filter(Message.name == name).all()
    messagesarr = []
    for i in messages:
        messagesarr.append(i.to_json())
    return jsonify({"code": 200, "msg": messagesarr})


@app.route("/api/token", methods=["POST"])
def getToken():
    if request.form.get("key", default="NONE") == config.server_key:
        R = request.form.get("R", default="NONE")
        if R == None:
            return jsonify({"code": 400, "msg": "No R"})

        tokens = Token.query.filter(Token.R == R)

        if tokens.count() == 0:
            return jsonify({"code": 400, "msg": "No token"})

        return jsonify({"code": 200, "msg": "success", "token": tokens.first().token})
    else:
        return jsonify({"code": 400, "msg": "Wrong key"})


@app.route("/api/devicekey", methods=["POST"])
def getDeviceKey():
    if request.form.get("key", default="NONE") == config.server_key:

        return jsonify({"code": 200, "msg": "success", "devicekey": testlight.key})
    else:
        return jsonify({"code": 400, "msg": "Wrong key"})


@app.route("/api/status", methods=["POST"])
def publishStatus():

    if request.form.get("key", default="NONE") == config.server_key:
        print(request.form)
        status = request.form.get("status", default="NONE")
        brightness = request.form.get("brightness", default="NONE")
        color = request.form.get("color", default="NONE")
        if status == 'lon':
            status = 'on'
        elif status == 'lof':
            status = 'off'
        else:
            status = testlight.status

        colors = ['red', 'yellow', 'green']
        colors_code = [4, 5, 6]

        if color in colors:
            pass
        elif ord(color[0]) in colors_code:
            color = colors[ord(color[0])-4]
        else:
            color = testlight.color


        try:
            brightness = str(ord(brightness[0])-10)
        except:
            brightness = testlight.brightness

        print(status)
        print(color)
        print(brightness)
        if status == None or status == "N" or brightness == None or color == None:
            return jsonify({"code": 400, "msg": "Unchange"})

        testlight.status = status
        testlight.brightness = brightness
        testlight.color = color
        print(testlight)

        if testlight.operation == None:
            return jsonify({"code": 400, "msg": "No operation"})
        else:
            param = testlight.operation.param
            R = testlight.operation.R
            token = testlight.operation.token

            testlight.operation = None

            return jsonify(
                {
                    "code": 200,
                    "msg": "success",
                    "param": param,
                    "R": R,
                    "token": token,
                }
            )
    else:
        return jsonify({"code": 400, "msg": "Wrong key"})


##############      以下为服务器间通信接口      ##############

# zigbee服务器实现
@app.route("/api/notification", methods=["GET", "POST"])
def notification():
    print("notification received:", request.form.to_dict())
    if request.form.get("msg", default="NONE") == "apply processed":
        status = request.form.get("status", default="NONE")
        if status == "accepted":
            name = request.form.get("name", default="NONE")
            R = request.form.get("R", default="NONE")
            token = request.form.get("Token", default="NONE")

            the_Token = Token(name=name, R=R, token=token)
            db.session.add(the_Token)
            db.session.commit()

            the_Message = Message(name=name, R=R, status="accepted")
            db.session.add(the_Message)
            db.session.commit()

        else:
            name = request.form.get("name", default="NONE")
            R = request.form.get("R", default="NONE")
            the_Message = Message(name=name, R=R, status="rejected")
            db.session.add(the_Message)
            db.session.commit()

    elif request.form.get("msg", default="NONE") == "delToken":
        token = request.form.get("token", default="NONE")
        name = request.form.get("name", default="NONE")
        R = request.form.get("R", default="NONE")

        tokens = Token.query.filter(Token.token == token)
        if tokens.count() == 0:
            return {"code": 400, "msg":"failed"}
        the_Token = tokens.first()
        db.session.delete(the_Token)
        db.session.commit()

        the_Message = Message(name=name, R=R, status="apply")
        db.session.add(the_Message)
        db.session.commit()

    return {"code": 200, "msg": "success"}


# 客户端向zigbee服务器发送申请
@app.route("/api/applytoken", methods=["GET", "POST"])
def applyToken():
    if session.get("logged_in") != "true":
        return jsonify({"code": 400, "msg": "Please login first"})
    name = session.get("name")

    R = request.form.get("R", default="NONE")
    if R == None:
        return jsonify({"code": 400, "msg": "No R"})

    the_Message = Message(name=name, R=R, status="toprocess")
    db.session.add(the_Message)
    db.session.commit()

    response = requests.post(
        url="https://" + config.MAN_ADDR + "/api/applyToken",
        data={"name": name, "R": R},
        verify=False,
    )

    if response.status_code != 200:
        return {"code": 400, "msg": "Error!"}

    return {"code": 200, "msg": "success"}


# 数据库debug
@app.route("/dbreset", methods=["GET", "POST"])
def dbreset():
    db.drop_all()
    db.create_all()
    return {"code": 200, "msg": "dbreset success"}


if __name__ == "__main__":

    testlight = singleDevice(key="test")
    testlight.status = "on"
    testlight.brightness = "10"
    testlight.color = "red"

    app.run(
        host="0.0.0.0", port=443, debug=True, ssl_context=ssl_keys, use_reloader=False
    )
