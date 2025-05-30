package com.cartotype.cartotypemaps;
import android.annotation.SuppressLint;
import android.app.AlertDialog;
import android.content.Context;
import android.content.DialogInterface;
import android.widget.Toast;

import com.cartotype.*;
import com.cartotype.Error;

@SuppressLint("ViewConstructor")
public class MainView extends MapView implements DialogInterface.OnClickListener, RouterAsyncInterface
    {
    MainView(Context aContext,Framework aFramework)
        {
        super(aContext,aFramework);
        m_framework = aFramework;
        }

    void showError(String aText)
        {
        Toast.makeText(getContext(),"error: " + aText,Toast.LENGTH_SHORT).show();
        }

    @Override
    public void onClick(DialogInterface aDialog,int aWhich)
        {
        if (aWhich == AlertDialog.BUTTON_NEGATIVE)
            {
            m_start_x = m_cur_x;
            m_start_y = m_cur_y;
            calculateAndDisplayRoute();
            }
        else if (aWhich == AlertDialog.BUTTON_POSITIVE)
            {
            m_end_x = m_cur_x;
            m_end_y = m_cur_y;
            calculateAndDisplayRoute();
            }
        }

    @Override
    public void onLongPress(double aX,double aY)
        {
        m_cur_x = aX;
        m_cur_y = aY;
        AlertDialog.Builder builder = new AlertDialog.Builder(getContext());
        builder.setTitle("Routing");
        String summary = m_framework.geoCodeSummary(aX,aY,CoordType.Degree);
        builder.setMessage(summary);
        builder.setCancelable(true);
        builder.setNegativeButton("Set start of route",this);
        builder.setPositiveButton("Set end of route",this);
        AlertDialog dialog = builder.create();
        dialog.show();
        }

    // handles routes calculated asynchronously
    @Override
    public void handler(int aResult, Route aRoute)
        {
        post(new Runnable()
            {
            public void run()
                {
                if (aResult == 0)
                    {
                    m_framework.useRoute(aRoute);
                    }
                else
                    showError(Error.string(aResult));
                }
            });
        }

    void calculateAndDisplayRoute()
        {
        if ((m_start_x == 0 && m_start_y == 0) || (m_end_x == 0 && m_end_y == 0))
            return;

        RouteCoordSet cs = new RouteCoordSet();
        cs.coordType = CoordType.Degree;
        cs.routePointArray = new RoutePoint[2];
        cs.routePointArray[0] = new RoutePoint();
        cs.routePointArray[0].x = m_start_x;
        cs.routePointArray[0].y = m_start_y;
        cs.routePointArray[1] = new RoutePoint();
        cs.routePointArray[1].x = m_end_x;
        cs.routePointArray[1].y = m_end_y;
        int error = m_framework.createRouteAsync(this,new RouteProfile(m_route_profile_type),cs,true);
        if (error != 0)
            showError(Error.string(error));
        }

    void setRouteProfileType(RouteProfileType aType)
        {
        if (aType != m_route_profile_type)
            {
            m_route_profile_type = aType;
            m_framework.setRouteProfileType(aType);
            calculateAndDisplayRoute();
            }
        }

    void reverseRoute()
        {
        int error = m_framework.reverseRoute();
        if (error == 0)
            {
            double x = m_start_x; m_start_x = m_end_x; m_end_x = x;
            double y = m_start_y; m_start_y = m_end_y; m_end_y = y;
            }
        }

    void deleteRoute()
        {
        m_framework.deleteRoute();
        m_start_x = 0; m_start_y = 0;
        m_end_x = 0; m_end_y = 0;
        }

    void setRouteStart(double aX,double aY)
        {
        m_start_x = aX;
        m_start_y = aY;
        calculateAndDisplayRoute();
        }

    boolean hasDestination()
        {
        return m_end_x != 0 && m_end_y != 0;
        }

    private final Framework m_framework;
    private double m_cur_x;
    private double m_cur_y;
    private double m_start_x;
    private double m_start_y;
    private double m_end_x;
    private double m_end_y;
    private RouteProfileType m_route_profile_type = RouteProfileType.Car;
    private Route m_route;
    }
