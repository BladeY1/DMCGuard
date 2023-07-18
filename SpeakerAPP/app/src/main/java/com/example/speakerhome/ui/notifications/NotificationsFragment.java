package com.example.speakerhome.ui.notifications;

import android.os.Bundle;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.Toast;

import androidx.annotation.NonNull;
import androidx.fragment.app.Fragment;
import androidx.swiperefreshlayout.widget.SwipeRefreshLayout;

import com.example.speakerhome.databinding.FragmentNotificationsBinding;
import com.example.speakerhome.BaseApplication;
import com.example.speakerhome.R;
import com.example.speakerhome.utils.Data;
import com.example.speakerhome.utils.HttpsUtils;
import com.example.speakerhome.utils.Message;
import com.example.speakerhome.utils.MessageAdapter;

import org.json.JSONArray;
import org.json.JSONException;
import org.json.JSONObject;

import java.io.IOException;
import java.util.ArrayList;
import java.util.List;

import okhttp3.Call;
import okhttp3.Callback;
import okhttp3.OkHttpClient;
import okhttp3.Request;
import okhttp3.Response;

public class NotificationsFragment extends Fragment {
    private OkHttpClient okHttpClient;
    private FragmentNotificationsBinding binding;
    MessageAdapter adapter;

    private List<Message> MessagesList = new ArrayList<>();

    public View onCreateView(@NonNull LayoutInflater inflater,
                             ViewGroup container, Bundle savedInstanceState) {
        okHttpClient= HttpsUtils.getTrustClient();
        binding = FragmentNotificationsBinding.inflate(inflater, container, false);
        View root = binding.getRoot();
        adapter = new MessageAdapter(BaseApplication.getAppContext(), R.layout.message_item, MessagesList);
        binding.messagesListView.setAdapter(adapter);
        binding.swipeLayout.setOnRefreshListener(new SwipeRefreshLayout.OnRefreshListener() {
            @Override
            public void onRefresh() {
                Request request = new Request.Builder()
                        .addHeader("cookie", Data.Jsessionid)
                        .url(Data.serverURL+"/api/getMessages")
                        .build();
                Call call = okHttpClient.newCall(request);
                call.enqueue(new Callback() {
                    @Override
                    public void onFailure(Call call, IOException e) {
                        getActivity().runOnUiThread(new Runnable() {
                            @Override
                            public void run() {
                                binding.swipeLayout.setRefreshing(false);
                                Log.d("TSS", e.toString());
                                Toast.makeText(getActivity().getApplicationContext(), "Network error", Toast.LENGTH_SHORT).show();
                            }
                        });
                    }

                    @Override
                    public void onResponse(Call call, Response response) throws IOException {
                        String ResponseText = response.body().string();
                        Integer responseCode = 0;
                        String responseMsgMessage = "NUll";
                        JSONArray jsonArray = null;
                        try {
                            JSONObject object = new JSONObject(ResponseText);
                            responseCode = object.getInt("code");
                            if (responseCode == 400) {
                                responseMsgMessage = object.getString("msg");
                            } else {
                                jsonArray = object.getJSONArray("msg");
                            }
                        } catch (JSONException e) {
                            e.printStackTrace();
                        }

                        if (responseCode == 200) {
                            MessagesList.clear();
                            binding.swipeLayout.setRefreshing(false);
                            try {
                                for (int i = 0; i < jsonArray.length(); i++) {
                                    JSONObject object = jsonArray.getJSONObject(i);
                                    Log.i("JSON", object.toString());
                                    Log.i("JSON", "id=" + object.getInt("id"));
                                    Log.i("JSON", "name=" + object.getString("name"));
                                    Log.i("JSON", "apply_id=" + object.getString("apply_id"));
                                    Log.i("JSON", "----------------");
                                    Message message = new Message(object.getString("name"), object.getString("status"), object.getString("R") , object.getInt("apply_id"), object.getInt("id"));
                                    MessagesList.add(message);
                                }
                            } catch (JSONException e) {
                                e.printStackTrace();
                            }

                            getActivity().runOnUiThread(new Runnable() {
                                @Override
                                public void run() {
                                    Log.d("TSS", "Success");
                                    adapter.notifyDataSetChanged();
                                    Toast.makeText(getActivity().getApplicationContext(), "Success", Toast.LENGTH_SHORT).show();
                                }
                            });
                        } else if (responseCode == 400) {
                            final String MSG = responseMsgMessage;
                            getActivity().runOnUiThread(new Runnable() {
                                @Override
                                public void run() {
                                    Log.d("TSS", "Failed");
                                    Toast.makeText(getActivity().getApplicationContext(), "Error: " + MSG, Toast.LENGTH_LONG).show();
                                }
                            });
                        }
                    }
                });

            }
        });
        //进入页面刷新
        refreshList();
        //结束
        return root;
    }



    public void refreshList(){
        binding.swipeLayout.post(new Runnable() {
            @Override
            public void run() {
                binding.swipeLayout.setRefreshing(true);
            }
        });
        Request request = new Request.Builder()
                .addHeader("cookie", Data.Jsessionid)
                .url(Data.serverURL+"/api/getMessages")
                .build();
        Call call = okHttpClient.newCall(request);
        call.enqueue(new Callback() {
            @Override
            public void onFailure(Call call, IOException e) {
                getActivity().runOnUiThread(new Runnable() {
                    @Override
                    public void run() {
                        binding.swipeLayout.setRefreshing(false);
                        Log.d("TSS", e.toString());
                        Toast.makeText(getActivity().getApplicationContext(), R.string.net_error, Toast.LENGTH_SHORT).show();
                    }
                });
            }
            @Override
            public void onResponse(Call call, Response response) throws IOException {
                String ResponseText = response.body().string();
                Integer responseCode = 0;
                String responseMsgMessage = "NUll";
                JSONArray jsonArray = null;
                try {
                    JSONObject object = new JSONObject(ResponseText);
                    responseCode = object.getInt("code");
                    if (responseCode == 400) {
                        responseMsgMessage = object.getString("msg");
                    } else {
                        jsonArray = object.getJSONArray("msg");
                    }
                } catch (JSONException e) {
                    e.printStackTrace();
                }

                if (responseCode == 200) {
                    MessagesList.clear();
                    binding.swipeLayout.setRefreshing(false);
                    try {
                        for (int i = 0; i < jsonArray.length(); i++) {
                            JSONObject object = jsonArray.getJSONObject(i);
                            Log.i("JSON", object.toString());
                            Message message = new Message(object.getString("name"), object.getString("status"), object.getString("R") , object.getInt("apply_id"), object.getInt("id"));
                            MessagesList.add(message);
                        }
                    } catch (JSONException e) {
                        e.printStackTrace();
                    }
                    getActivity().runOnUiThread(new Runnable() {
                        @Override
                        public void run() {
                            Log.d("TSS", "Success");
                            adapter.notifyDataSetChanged();
                            Toast.makeText(getActivity().getApplicationContext(), R.string.Success, Toast.LENGTH_SHORT).show();
                        }
                    });
                } else if (responseCode == 400) {
                    final String MSG = responseMsgMessage;
                    getActivity().runOnUiThread(new Runnable() {
                        @Override
                        public void run() {
                            Log.d("TSS", "Failed");
                            Toast.makeText(getActivity().getApplicationContext(), R.string.Fail + MSG, Toast.LENGTH_LONG).show();
                        }
                    });
                }
            }
        });
    }
    @Override
    public void onDestroyView() {
        super.onDestroyView();
        binding = null;
    }
}