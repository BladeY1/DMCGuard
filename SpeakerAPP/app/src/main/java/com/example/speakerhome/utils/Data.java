package com.example.speakerhome.utils;

//用于共享全局变量
public class Data {
    public static String serverURL="https://192.168.1.110";
    public static String Jsessionid = "";
    public static String username = "";
    public static boolean bind = false;

    public static boolean Light_Status = true;
    public static float Light_Brightness = (float) 0.2;
    public static String Light_Color = "grey";

    public static boolean channel_mqtt=true;
    public static boolean channel_zigbee=true;
    public static boolean channel_speaker=true;

}
