
package com.moonbc.moonclock;

import java.util.Calendar;
import java.util.Date;
import java.util.TimeZone;

import com.example.qrcodetest.R;

import android.support.v7.app.ActionBarActivity;



import android.support.v7.app.ActionBar;
import android.support.v4.app.Fragment;
import android.app.Activity;
import android.os.Bundle;
import android.os.Handler;
import android.view.LayoutInflater;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;
import android.view.ViewGroup;
import android.widget.TextView;
import android.os.Build;




//public class MainActivity extends ActionBarActivity {
public class MainActivity extends Activity {

    private static final String DATE_FORMAT = "%2d:%02d:%02d:%02d";
//    private static final int REFRESH_DELAY = 500;
    private static final int REFRESH_DELAY = 5;
    
    private final Handler mHandler = new Handler();
    private final Runnable mTimeRefresher = new Runnable() {
      @Override
      public void run() {

        Calendar calendar = Calendar.getInstance(TimeZone.getTimeZone("GMT+9:00"));
//        final Calendar c = new Calendar();
        
        if(calendar != null) {
            if(mTextView != null) {
                mTextView.setText(String.format(DATE_FORMAT, calendar.get(Calendar.HOUR_OF_DAY),
                        calendar.get(Calendar.MINUTE), calendar.get(Calendar.SECOND),calendar.get(Calendar.MILLISECOND)/10 ));
            }
        }

        
        mHandler.postDelayed(this, REFRESH_DELAY);
      }
    };

   private TextView mTextView;
    
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.fragment_main);
        
        mTextView = (TextView) findViewById(R.id.main_clock_time);
//        if (savedInstanceState == null) {
//            getFragmentManager().beginTransaction().add(R.id.container, new PlaceholderFragment())
//                    .commit();
//        }
    }
    
    @Override
    protected void onResume() {
        // TODO Auto-generated method stub
        super.onResume();
        mHandler.post(mTimeRefresher);
        
    }
    
    @Override
    protected void onStop() {
        // TODO Auto-generated method stub
        super.onStop();
        mHandler.removeCallbacks(mTimeRefresher);
    }
    
    

    @Override
    public boolean onCreateOptionsMenu(Menu menu) {

        // Inflate the menu; this adds items to the action bar if it is present.
        getMenuInflater().inflate(R.menu.main, menu);
        return true;
    }

    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        // Handle action bar item clicks here. The action bar will
        // automatically handle clicks on the Home/Up button, so long
        // as you specify a parent activity in AndroidManifest.xml.
        int id = item.getItemId();
        if (id == R.id.action_settings) {
            return true;
        }
        return super.onOptionsItemSelected(item);
    }

}