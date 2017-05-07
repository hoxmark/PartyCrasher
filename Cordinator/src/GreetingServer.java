
// File Name GreetingServer.java
import java.net.*;
import java.util.ArrayList;
import java.util.List;
import java.nio.charset.StandardCharsets;
import java.nio.file.*;
import java.nio.charset.*;
import java.lang.Process.*;

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
                String[] lines = line.split("\\s+");
                
                switch (lines[0]) {
                case "ClientHello":
                    clientHello(client, lines[1], lines[2], lines[3]);                    
                    break;
                case "GetSample":
                    try {
                        sendBestSampleToClient(client);
                    } catch (Exception e) {
                        System.out.println("sendBestSampleToClient error");
                    }
                    break;
                case "PostExample": 
                    //Postexmaples alg bredde, clic, state  
                    postExample(client, lines[1], lines[2], lines[3], lines[4]);                    
                    break;

                default:
                    System.out.println("Error: Default in switch");
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

    String fileToString(String value) throws Exception {
        String filename = ("../../counterexamples/" + value + ".txt");
        return readFile(filename, StandardCharsets.UTF_8);
    }

    void sendBestSampleToClient(Socket client) throws Exception {
        PrintStream out = new PrintStream(client.getOutputStream(), true);
        String file = fileToString(findBestCounterExample());
        //System.out.println(file.substring(file.indexOf('\n')+1).replace(" ", "").replace("\n", ""));
        out.print(file.substring(file.indexOf('\n') + 1).replace(" ", "").replace("\n", ""));
    }

    static String readFile(String path, Charset encoding) throws IOException {
        byte[] encoded = Files.readAllBytes(Paths.get(path));
        return new String(encoded, encoding);
    }

    void clientHello(Socket client, String alg, String clientBest, String clientBestClique) throws IOException {
        int clientBestInt = Integer.parseInt(clientBest);
        String serverBestString = findBestCounterExample();
        int serverBestInt = Integer.parseInt(serverBestString);
        int clientBestCliqueCount = Integer.parseInt(clientBestClique);
        System.out.println("Client: "+client.getInetAddress()+" \t alg: "+alg+" \tproblem: "+ clientBest + " \tBest Clique: "+clientBestClique);

        PrintStream out = new PrintStream(client.getOutputStream(), true);
        switch(alg){
            case "RandomFlip":{
                if (clientBestInt <= serverBestInt) {
                    //return best counterexmaple to client
                    System.out.println("serverBestString:" + serverBestString);
                    out.println(serverBestString);
                } else {
                    out.println(R_CONTINE);
                }
                break;
            }
            case "BruteForce":{
                if (clientBestInt <= serverBestInt) {
                    //return best counterexmaple to client
                    System.out.println("serverBestString:" + serverBestString);
                    out.println(serverBestString);
                } else {
                    out.println(R_CONTINE);
                }
                break;
            }
            default: {
                System.out.print("Error: default in switch statement clientHello");                
            }
        }
        
    }


    //Postexmaples alg bredde clic 
    void postExample(Socket client, String alg, String width, String clientClique, String s) throws IOException {
        int m = Integer.parseInt(width); 
        int cliqueCount = Integer.parseInt(clientClique); 
        
        // System.out.println("PostExample " + m);

        if(cliqueCount==0){
            System.out.print("########Clique count at 0, saving. ########");
            
            File file = new File("../../counterexamples/" + m + ".txt");
            BufferedWriter writer = new BufferedWriter(new FileWriter(file)); 
            writer.write(m + " 0 \n"); 

            for(int i = 0; i < m * m; i++){
                if(i % m == 0 && i != 0) writer.write("\n");
                writer.write(s.charAt(i) + " "); 
            }
            writer.flush();                    
            // Process p = Runtime.getRuntime().exec("/usr/bin/python /home/bhoxmark/PartyCrasher/watcher.py");
        } else if (cliqueCount<10) {
            System.out.print("Clique count under 10, saving. ");
            File file = new File("../../cliqueexamples/"+width+"/" + clientClique + ".txt");
            file.getParentFile().mkdirs();
            BufferedWriter writer = new BufferedWriter(new FileWriter(file)); 
            writer.write(m + " 0 \n"); 

            for(int i = 0; i < m * m; i++){
                if(i % m == 0 && i != 0) writer.write("\n");
                writer.write(s.charAt(i) + " "); 
            }
            writer.flush(); 
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
        return counterexamples.isEmpty() ? "0" : Collections.max(counterexamples).toString();
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
