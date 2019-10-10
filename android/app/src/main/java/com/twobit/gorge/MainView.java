package com.twobit.gorge;

import android.content.Context;
import android.opengl.GLSurfaceView;
import android.util.Log;
import android.view.MotionEvent;

import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;

class MainView extends GLSurfaceView {
    static String TAG = "FOOBAR";
    Renderer mRenderer;

    public MainView(Context context) {
        super(context);
        mRenderer = new Renderer();
        setPreserveEGLContextOnPause(true);
        setEGLContextClientVersion(2);
        setEGLConfigChooser(8, 8, 8, 8, 0, 0);
        setRenderer(mRenderer);
    }


    void touch(final int id, final int action, float x, float y) {
        final int ix = (int) x;
        final int iy = (int) y;
        queueEvent(new Runnable() {
            public void run() {
                Lib.touch(id, action, ix, iy);
            }
        });
    }

    @Override
    public boolean onTouchEvent(MotionEvent e) {
        int index  = e.getActionIndex();
        int id     = e.getPointerId(index);

        switch (e.getActionMasked()) {
        case MotionEvent.ACTION_DOWN:
        case MotionEvent.ACTION_POINTER_DOWN:
            touch(id, MotionEvent.ACTION_DOWN, e.getX(), e.getY());
            return true;
        case MotionEvent.ACTION_UP:
        case MotionEvent.ACTION_POINTER_UP:
            touch(id, MotionEvent.ACTION_UP, e.getX(), e.getY());
            return true;
        case MotionEvent.ACTION_MOVE:
            int count = e.getPointerCount();
            for (int i = 0; i < count; ++i) {
                touch(e.getPointerId(i), MotionEvent.ACTION_MOVE, e.getX(), e.getY());
            }
            return true;
        }
        return false;
    }

    class Renderer implements GLSurfaceView.Renderer {
        public void onSurfaceCreated(GL10 gl, EGLConfig config) {
            Lib.init(getResources().getAssets());
        }
        public void onSurfaceChanged(GL10 gl, int width, int height) {
            Lib.resize(width, height);
        }
        public void onDrawFrame(GL10 gl) {
            Lib.draw();
        }
    }
}

