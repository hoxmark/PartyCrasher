
// File Name GreetingServer.java
import java.net.*;
import java.util.ArrayList;
import java.util.List;

import org.omg.CORBA.PRIVATE_MEMBER;

import java.io.*;
import java.util.Collections;

public class GreetingServer extends Thread {
    private ServerSocket serverSocket;
    
    private String R_CONTINE = "CONTINUE";

    public GreetingServer(int port) throws IOException {
        serverSocket = new ServerSocket(port);
        // serverSocket.setSoTimeout(10000);
    }

    public void run() {
        while (true) {
            try {
                System.out.println("Waiting for client on port " + serverSocket.getLocalPort() + "...");
                Socket client = serverSocket.accept();

                System.out.println("Just connected to " + client.getRemoteSocketAddress());

                BufferedReader in = new BufferedReader(new InputStreamReader(client.getInputStream()));
                String line = in.readLine();

                switch (line.split("\\s+")[0]) {
                case "ClientHello":
                    clientHello(client, line.split("\\s+")[1]);
                    break;
                default:
                    System.out.println("Error: Default in switch");
                    System.out.println(line.split("\\s+")[0]);
                }

                client.close();

            } catch (SocketTimeoutException s) {
                System.out.println("Socket timed out!");
                break;
            } catch (IOException e) {
                e.printStackTrace();
                break;
            }
        }
    }

    void clientHello(Socket client, String clientBest) throws IOException{
        int clientBestInt = Integer.parseInt(clientBest);
        String serverBestString = findBestCounterExample();
        int serverBestInt = Integer.parseInt(serverBestString);
        
        PrintStream out = new PrintStream(client.getOutputStream(), true);
        if (clientBestInt < serverBestInt) {
            //return best counterexmaple to client
            System.out.println("serverBestString:"+serverBestString);
            out.println(serverBestString);            
        } else {
            out.println(R_CONTINE);
        }
    }

    String findBestCounterExample() {
        File folder = new File("../../counterexamples/");
        File[] listOfFiles = folder.listFiles();
        List<Integer> counterexamples = new ArrayList<>();

        for (int i = 0; i < listOfFiles.length; i++) {
            if (listOfFiles[i].isFile()) {
                String num = listOfFiles[i].getName().split("\\.")[0];
                if (!num.trim().isEmpty()) {
                    counterexamples.add(Integer.parseInt(num));
                }
            }
        }
        return Collections.max(counterexamples).toString();
    }

    public static void main(String[] args) {
        int port = 5000;//Integer.parseInt(args[0]);
        try {
            Thread t = new GreetingServer(port);
            t.start();
        } catch (IOException e) {
            e.printStackTrace();
        }
    }
}
