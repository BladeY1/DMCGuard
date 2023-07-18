package com.example.zigbeehome;

import androidx.annotation.NonNull;
import androidx.appcompat.app.ActionBar;
import androidx.appcompat.app.AppCompatActivity;

import android.content.Intent;
import android.os.Bundle;
import android.util.Log;
import android.view.MenuItem;
import android.view.View;
import android.widget.Toast;

import com.example.zigbeehome.utils.Data;
import com.example.zigbeehome.utils.HttpsUtils;

import org.json.JSONException;
import org.json.JSONObject;

import java.io.IOException;

import okhttp3.Call;
import okhttp3.Callback;
import okhttp3.FormBody;
import okhttp3.OkHttpClient;
import okhttp3.Request;
import okhttp3.RequestBody;
import okhttp3.Response;

public class TokenApplyActivity extends AppCompatActivity {

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        ActionBar actionBar = getSupportActionBar();
        actionBar.setTitle(R.string.token_apply);
        actionBar.setDisplayHomeAsUpEnabled(true);
        setContentView(R.layout.activity_token_apply);
    }

    public void onApplyClick(View view){
        String Apply_R="0";
        if(view.getId()==R.id.R0_card){
            Apply_R="0";
        }else if(view.getId()==R.id.R1_card){
            Apply_R="1";
        }else if(view.getId()==R.id.R2_card){
            Apply_R="2";
        }else if(view.getId()==R.id.R3_card){
            Apply_R="3";
        }else if(view.getId()==R.id.R4_card){
            Apply_R="4";
        }else if(view.getId()==R.id.R5_card){
            Apply_R="5";
        }

        OkHttpClient okHttpClient = HttpsUtils.getTrustClient();
        RequestBody requestBody = new FormBody.Builder()
                .add("R",Apply_R)
                .build();
        final Request request = new Request.Builder()
                .url(Data.serverURL+"/api/applytoken")
                .addHeader("cookie", Data.Jsessionid)
                .post(requestBody)
                .build();
        Callback setColorCallback = new Callback() {
            @Override
            public void onFailure(final Call call, IOException e) {
                //在UI线程才能更新UI
                TokenApplyActivity.this.runOnUiThread(new Runnable() {
                    @Override
                    public void run() {
                        Toast.makeText(TokenApplyActivity.this, R.string.net_error, Toast.LENGTH_SHORT).show();
                    }
                });
            }
            @Override
            public void onResponse(Call call, Response response) throws IOException {
                String ResponseText = response.body().string();
                Integer responseCode = 0;
                try {
                    JSONObject object = new JSONObject(ResponseText);
                    responseCode = object.getInt("code");
                } catch (JSONException e) {
                    e.printStackTrace();
                }
                Integer finalResponseCode = responseCode;
                TokenApplyActivity.this.runOnUiThread(new Runnable() {
                    @Override
                    public void run() {
                        if (finalResponseCode == 200) {
                            Toast.makeText(TokenApplyActivity.this, R.string.Success, Toast.LENGTH_SHORT).show();
                        } else if (finalResponseCode == 400){
                            Toast.makeText(TokenApplyActivity.this, R.string.Fail, Toast.LENGTH_SHORT).show();
                        }
                    }
                });

            }
        };
        Call call = okHttpClient.newCall(request);
        call.enqueue(setColorCallback);
    }

    //toolbar返回键
    @Override
    public boolean onOptionsItemSelected(@NonNull MenuItem item) {
        if (item.getItemId() == android.R.id.home) {
            Intent intent = new Intent(TokenApplyActivity.this, DeviceDetailActivity.class);
            startActivity(intent);
            return true;
        }
        return super.onOptionsItemSelected(item);
    }

    @Override
    public void onBackPressed() {
        Intent intent = new Intent(TokenApplyActivity.this, DeviceDetailActivity.class);
        startActivity(intent);
        finish();
    }
}