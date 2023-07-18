package com.example.zigbeehome.utils;

public class Message {
    public String name;
    public String status;
    public String R;
    public int apply_id;
    public int id;

    public Message(String name , String status , String R, int apply_id , int id)
    {
        this.name = name ;
        this.status = status ;
        this.R = R;
        this.apply_id = apply_id;
        this.id = id ;
    }

    public String getTitle()
    {
        String title = "name:" + name + " apply_id: " + apply_id;
        return title;
    }
    public String getContent()
    {
        String content = "id:" + id + " status:" + status + " R:" + R;
        return content;
    }
    public int getApply_id()
    {
        return apply_id;
    }
}