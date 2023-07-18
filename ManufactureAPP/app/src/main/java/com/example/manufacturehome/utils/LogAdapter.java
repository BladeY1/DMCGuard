package com.example.manufacturehome.utils;

import com.example.manufacturehome.R;
import android.content.Context;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.ArrayAdapter;
import android.widget.TextView;

import androidx.annotation.NonNull;


import com.example.manufacturehome.R;
import com.example.manufacturehome.utils.Message;

import java.util.List;

public class LogAdapter extends ArrayAdapter<Log>{
    private int resourceId;
    public LogAdapter(@NonNull Context context ,int resource, List<Log> objects) {
        super(context, resource, objects);
        resourceId = resource;
    }

    @Override
    public View getView(int position , View convertView , ViewGroup parent)
    {
        class ViewHolder
        {
            TextView title;
            TextView content;
        }

        Log log = getItem(position);
        View view;
        ViewHolder viewHolder;

        if (convertView == null )
        {
            view = LayoutInflater.from(getContext()).inflate(resourceId,parent,false);
            viewHolder = new ViewHolder();
            viewHolder.title = (TextView) view.findViewById(R.id.title);
            viewHolder.content = (TextView) view.findViewById(R.id.content);
            view.setTag(viewHolder);
        }
        else {
            view = convertView;
            viewHolder = (ViewHolder) view.getTag();
        }
        viewHolder.title.setText(log.getTitle());
        viewHolder.content.setText(log.getContent());
        return view;
    }
}
