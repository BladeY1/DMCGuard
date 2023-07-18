package com.example.zigbeehome.utils;

//用于共享全局变量
public class Data {
    public static String serverURL="https://101.43.239.148:5002";
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
