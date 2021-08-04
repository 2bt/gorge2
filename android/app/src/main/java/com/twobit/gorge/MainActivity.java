package com.twobit.gorge;

import android.Manifest;
import android.app.Activity;
import android.content.Context;
import android.content.SharedPreferences;
import android.content.pm.PackageManager;
import android.os.Bundle;
import android.util.Log;
import android.view.KeyEvent;
import android.view.inputmethod.InputMethodManager;

import androidx.annotation.NonNull;
import androidx.appcompat.app.AlertDialog;
import androidx.core.app.ActivityCompat;
import androidx.core.content.ContextCompat;

import com.google.android.gms.auth.api.signin.GoogleSignIn;
import com.google.android.gms.auth.api.signin.GoogleSignInAccount;
import com.google.android.gms.auth.api.signin.GoogleSignInClient;
import com.google.android.gms.auth.api.signin.GoogleSignInOptions;
import com.google.android.gms.common.api.ApiException;
import com.google.android.gms.games.Games;
import com.google.android.gms.games.LeaderboardsClient;
import com.google.android.gms.games.Player;
import com.google.android.gms.games.PlayersClient;
import com.google.android.gms.tasks.OnCompleteListener;
import com.google.android.gms.tasks.Task;


public class MainActivity extends Activity {
    static String TAG = "FOOBAR";
    static MainActivity             sInstance;
    static SharedPreferences        sPrefs;
    static SharedPreferences.Editor sPrefEdit;
    private MainView           mView;
    private GoogleSignInClient mGoogleSignInClient;
    private LeaderboardsClient mLeaderboardsClient;
    private PlayersClient      mPlayersClient;

    // display name of the signed in user
    private String mDisplayName = "";


    private boolean isSignedIn() {
        return GoogleSignIn.getLastSignedInAccount(this) != null;
    }
    private void signInSilently() {

        mGoogleSignInClient.silentSignIn().addOnCompleteListener(this,
                new OnCompleteListener<GoogleSignInAccount>() {
                    @Override public void onComplete(@NonNull Task<GoogleSignInAccount> task) {
                        if (task.isSuccessful()) {
                            Log.d(TAG, "signInSilently: success");
                            onConnected(task.getResult());
                        } else {
                            //Log.d(TAG, "signInSilently: failure", task.getException());
                            Log.d(TAG, "signInSilently: failure");
                            onDisconnected();
                        }
                    }
                });
    }


    private void onConnected(GoogleSignInAccount googleSignInAccount) {
        Log.d(TAG, "onConnected: connected to Google APIs");

        mLeaderboardsClient = Games.getLeaderboardsClient(this, googleSignInAccount);
        mPlayersClient      = Games.getPlayersClient(this, googleSignInAccount);

        // TODO: show sign-out button on main menu
        // TODO: show "you are signed in" message

        // Set the greeting appropriately on main menu
        mPlayersClient.getCurrentPlayer()
                .addOnCompleteListener(new OnCompleteListener<Player>() {
                    @Override public void onComplete(@NonNull Task<Player> task) {
                        String displayName;
                        if (task.isSuccessful()) {
                            displayName = task.getResult().getDisplayName();
                        } else {
                            Exception e = task.getException();
                            handleException(e, "players exception");
                            displayName = "???";
                        }
                        mDisplayName = displayName;
                        Log.d(TAG, "Hello, " + displayName);
                    }
                });

        // if we have accomplishments to push, push them
    }
    private void onDisconnected() {
        Log.d(TAG, "onDisconnected");

        mLeaderboardsClient = null;
        mPlayersClient      = null;

        // TODO: show sign-in button
    }
    private void handleException(Exception e, String details) {
        int status = 0;

        if (e instanceof ApiException) {
            ApiException apiException = (ApiException) e;
            status = apiException.getStatusCode();
        }

        String message = String.format("status_exception_error: %1$s (status %2$d). %3$s", details, status, e);

        new AlertDialog.Builder(MainActivity.this)
                .setMessage(message)
                .setNeutralButton(android.R.string.ok, null)
                .show();
    }


    static public void showKeyboard() {
        sInstance.runOnUiThread(new Runnable() {
            @Override public void run() {
                InputMethodManager imm = (InputMethodManager) sInstance.getSystemService(Context.INPUT_METHOD_SERVICE);
                imm.showSoftInput(sInstance.getWindow().getDecorView(), InputMethodManager.SHOW_FORCED);
            }
        });
    }
    static public void hideKeyboard() {
        sInstance.runOnUiThread(new Runnable() {
            @Override public void run() {
                InputMethodManager imm = (InputMethodManager) sInstance.getSystemService(Context.INPUT_METHOD_SERVICE);
                imm.hideSoftInputFromWindow(sInstance.getWindow().getDecorView().getWindowToken(), 0);
            }
        });
    }


    static public boolean loadPrefBool(String name, boolean value) {
        return sPrefs.getBoolean(name, value);
    }
    static public int loadPrefInt(String name, int value) {
        return sPrefs.getInt(name, value);
    }

    static public void storePrefBool(String name, boolean value) {
        sPrefEdit.putBoolean(name, value);
    }
    static public void storePrefInt(String name, int value) {
        sPrefEdit.putInt(name, value);
    }
    static public void storePrefApply() {
        sPrefEdit.apply();
    }



    @Override public boolean onKeyDown(final int code, final KeyEvent e) {
        mView.queueEvent(new Runnable() { public void run() {
            Lib.key(code, e.getUnicodeChar());
        }});
        return false;
    }

    @Override protected void onCreate(Bundle bundle) {
        super.onCreate(bundle);
        sInstance = this;
        mView = new MainView(getApplication());
        setContentView(mView);
        getWritePermission();

        sPrefs    = getPreferences(MODE_PRIVATE);
        sPrefEdit = sPrefs.edit();

        GoogleSignInOptions signInOptions = new GoogleSignInOptions.Builder(GoogleSignInOptions.DEFAULT_GAMES_SIGN_IN).build();
        mGoogleSignInClient = GoogleSignIn.getClient(this, signInOptions);
    }
    @Override protected void onResume() {
        super.onResume();
        signInSilently();
        mView.onResume();
        mView.queueEvent(new Runnable() { public void run() {
            Lib.onResume();
        }});
    }
    @Override protected void onPause() {
        super.onPause();
        mView.onPause();
        mView.queueEvent(new Runnable() { public void run() {
            Lib.onPause();
        }});
    }

    static final int PERMISSION_REQUEST_WRITE_EXTERNAL_STORAGE = 42;
    boolean mWritePermission = false;

    public void getWritePermission() {
        if (ContextCompat.checkSelfPermission(this, Manifest.permission.WRITE_EXTERNAL_STORAGE) == PackageManager.PERMISSION_GRANTED) {
            mWritePermission = true;
        }
        else {
            ActivityCompat.requestPermissions(this,
                    new String[]{Manifest.permission.WRITE_EXTERNAL_STORAGE},
                    PERMISSION_REQUEST_WRITE_EXTERNAL_STORAGE);
        }
    }

    @Override public void onRequestPermissionsResult(int requestCode, String[] permissions, int[] grantResults) {
        if (requestCode != PERMISSION_REQUEST_WRITE_EXTERNAL_STORAGE) return;
        if (grantResults.length < 1) return;
        if (grantResults[0] == PackageManager.PERMISSION_GRANTED) mWritePermission = true;
    }

}
