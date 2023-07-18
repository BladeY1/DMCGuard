package com.example.manufacturehome.utils;

public class User {
    public String name = "";
    public Boolean R0=false;
    public Boolean R1=false;
    public Boolean R2=false;
    public Boolean R3=false;
    public Boolean R4=false;
    public Boolean R5=false;
    public User(String name, String R0,String R1,String R2,String R3,String R4,String R5) {
        this.name = name;
        this.R0=R0.equals("1");
        this.R1=R1.equals("1");
        this.R2=R2.equals("1");
        this.R3=R3.equals("1");
        this.R4=R4.equals("1");
        this.R5=R5.equals("1");
    }
}
