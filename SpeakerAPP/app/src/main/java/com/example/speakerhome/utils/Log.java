package com.example.speakerhome.utils;

import java.util.Objects;

public class Log {
    String time = "";
    String type = "";
    String name = "";
    String channel= "";
    String Param = "";
    String R = "";
    String status= "";
    String topic = "";
    String msg = "";
    public Log(String time, String type, String name, String channel, String Param, String R, String status, String topic, String msg) {
        this.time = time;
        this.type = type;
        this.name = name;
        this.channel = channel;
        this.Param = Param;
        this.R = R;
        this.status = status;
        this.topic = topic;
        this.msg = msg;
    }
    public String getTitle(){
        if(Objects.equals(this.type, "Operate")){
            return "操作记录"+this.time;
        }
        else if(Objects.equals(this.type, "Token")){
            return "Token记录"+this.time;
        }
        else if(Objects.equals(this.type, "Device")){
            return "设备记录"+this.time;
        }
        else{
            return this.type;
        }
    }

    public String getContent(){
        if(Objects.equals(this.type, "Operate")){
            return "name:"+this.name+"channel:"+this.channel+"Param:"+this.Param+"R:"+this.R+"status:"+this.status;
        }
        else if(Objects.equals(this.type, "Token")){
            return "name:"+this.name+"channel:"+this.channel+"Param:"+this.Param+"R:"+this.R;
        }
        else if(Objects.equals(this.type, "Device")){
            return "topic:"+this.topic+"msg:"+this.msg;
        }
        else{
            return this.type;
        }
    }
}