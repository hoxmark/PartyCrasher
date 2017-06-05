import java.net.*;
import java.util.*;
import java.nio.charset.StandardCharsets;
import java.nio.file.*;
import java.nio.charset.*;
import java.io.*;
import com.mongodb.*;


public class LoadBalancer extends Thread {
    static MongoClient mongoClient;
    ServerSocket serverSocket;
    static ReplicaSetStatus rss; 
    int numberOfRequests; 
    int numberOfServers = 3;
    String currentMaster = "";
    

    public LoadBalancer(int port){
        try {
            serverSocket = new ServerSocket(port);
        } catch (IOException e) {
            Logger.logException(e);
        }
        mongoClient = new MongoClient(
            Arrays.asList(new ServerAddress("104.198.30.238", 27017),
                new ServerAddress("104.197.154.195", 27017),
                new ServerAddress("104.197.239.143", 27017)));
        // MongoClient mongoClient = new MongoClient(
        //     Arrays.asList(new ServerAddress("10.128.0.2", 27017),
        //         new ServerAddress("10.128.0.5", 27017),
        //         new ServerAddress("10.128.0.6", 27017)));
        rss = mongoClient.getReplicaSetStatus();
        // ReplicaSetStatus rss = mongoClient.getReplicaSetStatus();
        System.out.println("----------");
        System.out.println(rss);
        System.out.println(rss.getMaster());
        System.out.println(rss.getName());
        System.out.println(rss.isMaster(new ServerAddress("104.198.30.238")));
        System.out.println(rss.isMaster(new ServerAddress("104.197.154.195")));
        System.out.println(rss.isMaster(new ServerAddress("104.197.239.143")));
        System.out.println("----------");
        // // System.out.println(mongoClient.getAddress());
        // // System.out.println(mongoClient.getConnectPoint());
        // System.out.println("getPrimary()");
        // System.out.println(mongoClient.getMongoOptions().getReadPreference());
        // System.out.println(mongoClient.getServerDescription());
        // ServerDescription a = new ServerDescription(mongoClient);
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
                        // System.out.println("----------");
                        // System.out.println(rss);
                        // System.out.println(rss.getMaster());
                        // System.out.println(rss.getName());
                        
                        // System.out.println("----------");
                        // //Requesting server IP 
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
                        client.close();
                    } catch (IOException e1) {
                        Logger.logException(e1);
                    }
                }
            }
        }
    }

    public void sendRestoreStateToServer(String ip){
        int port = 5004;
        try {
            Socket server = new Socket(ip, port);
            System.out.println("Just connected to " + server.getRemoteSocketAddress());
            // OutputStream outToServer = server.getOutputStream();
            // DataOutputStream serverOut = new DataOutputStream(outToServer);
            // serverOut.writeUTF("RestoreState");
            PrintStream out = new PrintStream(server.getOutputStream(), true);
            out.print("RestoreState LoadBalancer");
            server.close();
        }catch(IOException e) {
            e.printStackTrace();
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
        if ( rss.isMaster(new ServerAddress("104.198.30.238"))){
            System.out.println("104.198.30.238 "+Config.SERVERPORT+" ");
            out.print("104.198.30.238 "+Config.SERVERPORT + " ");

            if(!this.currentMaster.equals("104.198.30.238")){
                sendRestoreStateToServer("104.198.30.238");
            }
            this.currentMaster = "104.198.30.238";
        } else if ( rss.isMaster(new ServerAddress("104.197.154.195"))){
            System.out.println("104.197.154.195 "+Config.SERVERPORT+" ");
            out.print("104.197.154.195 " +Config.SERVERPORT+" ");

            if(!this.currentMaster.equals("104.197.154.195")){
                sendRestoreStateToServer("104.197.154.195");
            }
            this.currentMaster = "104.197.154.195";

        } else if ( rss.isMaster(new ServerAddress("104.197.239.143"))){
            System.out.println("104.197.239.143 "+Config.SERVERPORT+" ");
            out.print("104.197.239.143 "+Config.SERVERPORT+ " ");
            if(!this.currentMaster.equals("104.197.239.143")){
                sendRestoreStateToServer("104.197.239.143");
            }
            this.currentMaster = "104.197.239.143";
        } else {
            System.out.println("ERROR: no Primary DB");
        }

        try {
            client.close();
        } catch (IOException e) {
            e.printStackTrace();
        }
        // out.print(Config.SERVERIPS[toChoose]);
    }

    public static void main(String[] args) {
        int port = 5005; //Integer.parseInt(args[0]);

        Thread t = new LoadBalancer(port);
        t.start();
    }

    
}
