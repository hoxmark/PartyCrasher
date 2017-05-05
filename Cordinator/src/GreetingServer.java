
// File Name GreetingServer.java
import java.net.*;
import java.util.ArrayList;
import java.util.List;
import java.io.*;
import java.util.Collections; 


public class GreetingServer extends Thread {
    private ServerSocket serverSocket;

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
                // DataInputStream in = new DataInputStream(client.getInputStream());
                //
                // System.out.println(in.readUTF());
                PrintStream out = new PrintStream(client.getOutputStream(), true);
                // out.writeUTF("Thank you for connecting to " + client.getLocalSocketAddress()
                //         + "\nGoodbye!");
                //
                // out = new PrintStream(s.getOutputStream(), true);

                // out.println("HEY THERE CLIENT! :D ");

                File folder = new File("../../counterexamples/");
                File[] listOfFiles = folder.listFiles();
                List<Integer> counterexamples = new ArrayList<>(); 

                for (int i = 0; i < listOfFiles.length; i++) {
                    if (listOfFiles[i].isFile()) {
                        String num = listOfFiles[i].getName().split("\\.")[0]; 
                        if(!num.trim().isEmpty()){
                            counterexamples.add(Integer.parseInt(num));
                        }
                    }
                }
                System.out.println("Sending best: " + Collections.max(counterexamples)); 
                out.println(Collections.max(counterexamples)); 
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
