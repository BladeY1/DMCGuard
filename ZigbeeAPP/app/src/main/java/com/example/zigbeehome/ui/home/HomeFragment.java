package com.example.zigbeehome.ui.home;

import android.os.Bundle;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;

import androidx.annotation.NonNull;
import androidx.fragment.app.Fragment;

import com.example.zigbeehome.R;
import com.example.zigbeehome.databinding.FragmentHomeBinding;
import com.example.zigbeehome.utils.Data;

public class HomeFragment extends Fragment {

    private FragmentHomeBinding binding;

    public View onCreateView(@NonNull LayoutInflater inflater,
                             ViewGroup container, Bundle savedInstanceState) {

        binding = FragmentHomeBinding.inflate(inflater, container, false);
        View root = binding.getRoot();

        if(Data.bind){
            binding.theLight.setVisibility(View.VISIBLE);
        }
        else {
            binding.theLight.setVisibility(View.GONE);
        }
        return root;
    }



    @Override
    public void onDestroyView() {
        super.onDestroyView();
        binding = null;
    }

}