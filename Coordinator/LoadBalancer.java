import java.net.*;
import java.util.*;
import java.nio.charset.StandardCharsets;
import java.nio.file.*;
import java.nio.charset.*;
import java.io.*;

public class LoadBalancer extends Thread {
    ServerSocket serverSocket;
    int numberOfRequests; 
    int numberOfServers = 3; 
    

    public LoadBalancer(int port){
        try {
            serverSocket = new ServerSocket(port);
        } catch (IOException e) {
            Logger.logException(e);
        }
    }

    public void run() {
        Logger.logString("Load Balancer running");
        Socket client = null;
        while (true) {
            try {
                client = serverSocket.accept();
                BufferedReader in = new BufferedReader(new InputStreamReader(client.getInputStream()));
                String line = in.readLine();
                String[] lines = line.split("\\s+");

                switch (lines[0]) {
                    case Config.GETSERVERIP:
                        //Requesting server IP 
                        System.out.println("recv:"+lines[0]);
                        numberOfRequests++;
                        getNewServerIp(client);
                        break;                   
                    default:
                        System.out.println("Error: Default in switch");
                }
                client.close();

            } catch (Exception e) {
                Logger.logException(e);
                if (client != null) {
                    try {
                        PrintStream out = new PrintStream(client.getOutputStream(), true);
                        out.println(Config.R_RETRY);
                    } catch (IOException e1) {
                        Logger.logException(e1);
                    }
                }
            }
        }
    }

    public void getNewServerIp(Socket client){
        PrintStream out = null;
        try {
            out = new PrintStream(client.getOutputStream(), true);
        } catch (IOException e) {
            Logger.logException(e);
        }

        int toChoose = numberOfRequests%numberOfServers;
        // System.out.println(numberOfRequests);
        // System.out.println(numberOfServers);
        // System.out.println(toChoose);
        out.print(Config.SERVERIPS[toChoose]);
    }

    public static void main(String[] args) {
        int port = 5005; //Integer.parseInt(args[0]);

        Thread t = new LoadBalancer(port);
        t.start();
    }
}
