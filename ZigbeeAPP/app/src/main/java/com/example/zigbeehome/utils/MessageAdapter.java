package com.example.zigbeehome.utils;
import com.example.zigbeehome.R;
import android.content.Context;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.ArrayAdapter;
import android.widget.TextView;

import androidx.annotation.NonNull;


import java.util.List;

public class MessageAdapter extends ArrayAdapter<Message>{
    private int resourceId;
    public MessageAdapter(@NonNull Context context ,int resource, List<Message> objects) {
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

        Message message = getItem(position);
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
        viewHolder.title.setText(message.getTitle());
        viewHolder.content.setText(message.getContent());
        return view;
    }
}
