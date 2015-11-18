package io.github.xTun.model;

import com.google.gson.annotations.SerializedName;

public class Profile {
    @SerializedName("id")
    private int id = 0;
    @SerializedName("name")
    private String name = "Untitled";
    @SerializedName("localIP")
    private String localIP = "10.0.0.3";
    @SerializedName("host")
    private String host = "";
    @SerializedName("remotePort")
    private int remotePort = 1082;
    @SerializedName("mtu")
    private int mtu = 1440;
    @SerializedName("password")
    private String password = "";
    @SerializedName("route")
    private String route = "all";
    @SerializedName("global")
    private boolean global = true;
    @SerializedName("route_bypass")
    private boolean bypass = false;
    private String individual = "";

    public void setId(int id) {
        this.id = id;
    }

    public void setName(String name) {
        this.name = name;
    }

    public void setLocalIP(String localIP) {
        this.localIP = localIP;
    }

    public void setHost(String host) {
        this.host = host;
    }

    public void setRemotePort(int remotePort) {
        this.remotePort = remotePort;
    }

    public void setMTU(int mtu) {
        this.mtu = mtu;
    }

    public void setPassword(String password) {
        this.password = password;
    }

    public void setRoute(String route) {
        this.route = route;
    }

    public void setGlobal(boolean global) {
        this.global = global;
    }

    public void setBypass(boolean bypass) {
        this.bypass = bypass;
    }

    public int getId() {
        return this.id;
    }

    public String getName() {
        return this.name;
    }

    public String getLocalIP() {
        return this.localIP;
    }

    public String getHost() {
        return this.host;
    }

    public int getRemotePort() {
        return remotePort;
    }

    public int getMTU() {
        return mtu;
    }

    public String getPassword() {
        return password;
    }

    public String getRoute() {
        return route;
    }

    public boolean isGlobal() {
        return global;
    }

    public boolean isBypass() {
        return bypass;
    }

    public void setIndividual(String individual) {
        this.individual = individual;
    }

    public String getIndividual() {

        return individual;
    }

}
