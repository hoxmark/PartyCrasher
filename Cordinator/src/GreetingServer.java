
// File Name GreetingServer.java
import java.net.*;
import java.util.ArrayList;
import java.util.List;
import java.nio.charset.StandardCharsets;
import java.nio.file.*;
import java.text.Normalizer.Form;
import java.nio.charset.*;
import java.lang.Process.*;

import org.omg.CORBA.PRIVATE_MEMBER;

import java.io.*;
import java.util.Collections;

public class GreetingServer extends Thread {
    private ServerSocket serverSocket;
    private String R_CONTINE = "CONTINUE";
    private PartyState bestClique;
    private PartyState bestEndFlipClique;

    public GreetingServer(int port) throws IOException {
        serverSocket = new ServerSocket(port);
        bestClique = new PartyState(0, Integer.MAX_VALUE, "");
        bestEndFlipClique = new PartyState(0, Integer.MAX_VALUE, "");

        try {
            generateNewWidth();
        } catch (Exception e) {
            System.out.print("nei");
        }
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
                case "PostExample":
                    //PostExample alg bredde, clic, state  
                    postExample(client, lines[1], lines[2], lines[3], lines[4]);
                    break;
                case "GetNextWork":
                    //PostExample alg bredde 
                    try {
                        getNextWork(client, lines[1], lines[2], lines[3]);
                    } catch (Exception e) {
                        System.out.print("ERROR" + e);
                    }
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

    static String readFile(String path, Charset encoding) throws IOException {
        byte[] encoded = Files.readAllBytes(Paths.get(path));
        return new String(encoded, encoding);
    }

    void postExample(Socket client, String alg, String width, String clientClique, String s) throws IOException {
        int m = Integer.parseInt(width);
        int cliqueCount = Integer.parseInt(clientClique);

        System.out.println("Client: " + client.getInetAddress() + " \t\t alg: " + alg + " \t\tproblem: " + width
                + " \t\tBest Clique: " + clientClique);

        if (cliqueCount == 0 && m == bestClique.getWidth()) {
            System.out.println("########Clique count at 0, saving. ########");

            File file = new File("../../counterexamples/" + m + ".txt");
            BufferedWriter writer = new BufferedWriter(new FileWriter(file));
            writer.write(m + " 0 \n");

            for (int i = 0; i < m * m; i++) {
                if (i % m == 0 && i != 0)
                    writer.write("\n");
                writer.write(s.charAt(i) + " ");
            }
            writer.flush();
            try {
                generateNewWidth();

            } catch (Exception e) {
                //TODO: handle exception
            }

        } else {
            switch (alg) {
            case "BestClique":
                if ((m >= bestClique.getWidth()) && (cliqueCount < bestClique.getCliqueCount())) {
                    bestClique = new PartyState(m, cliqueCount, s);
                }
                break;
            case "EndFlip":
                if ((m >= bestClique.getWidth()) && (cliqueCount < bestClique.getCliqueCount())) {
                    bestEndFlipClique = new PartyState(m, cliqueCount, s);
                }
                break;
            }
        }
    }

    void generateNewWidth() throws Exception {
        String best = findBestCounterExample();
        int m = Integer.parseInt(best);
        bestClique.setWidth(m + 1);
        bestClique.setCliqueCount(Integer.MAX_VALUE);

        bestEndFlipClique.setWidth(m + 1);
        bestEndFlipClique.setCliqueCount(Integer.MAX_VALUE);

        String file = fileToString(best);
        String oldBody = file.substring(file.indexOf('\n') + 1).replace(" ", "").replace("\n", "");
        String newBody = "";
        for (int i = 0; i < m * m; i++) {
            newBody += oldBody.charAt(i);
            if ((i + 1) % m == 0)
                newBody += "0";
        }
        for (int i = 0; i < m + 1; i++) {
            newBody += "0";
        }
        // System.out.print(newBody);
        bestClique.setBody(newBody);
        bestEndFlipClique.setBody(newBody);
    }

    String getUniversalBestClique() {
        return bestEndFlipClique.getCliqueCount() < bestClique.getCliqueCount()
                ? bestEndFlipClique.getWidth() + " " + bestEndFlipClique.getBody()
                : bestClique.getWidth() + " " + bestClique.getBody();
    }

    void getNextWork(Socket client, String alg, String clientWidthString, String clientCliqueString) throws Exception {
        System.out.println("Client: " + client.getInetAddress() + " \t\t alg: " + alg + " \t\tproblem: "
                + clientWidthString + " \t\tBest Clique: " + clientCliqueString);
        int clientWidth = Integer.parseInt(clientWidthString);
        int clientCliqueCount = Integer.parseInt(clientCliqueString);

        System.out.println("Returning " + bestClique.getCliqueCount());
        PrintStream out = new PrintStream(client.getOutputStream(), true);

        switch (alg) {
        case "BestClique":
            if (clientWidth == bestClique.getWidth() && clientCliqueCount < bestClique.getCliqueCount()) {
                out.print(R_CONTINE);
            } else {
                out.print(getUniversalBestClique());
                client.close();
            }
            break;

        case "EndFlip":
            if (clientWidth == bestEndFlipClique.getWidth() && clientCliqueCount < bestEndFlipClique.getCliqueCount()) {
                out.print(R_CONTINE);
            } else {
                out.print(bestEndFlipClique.getWidth() + " " + bestEndFlipClique.getBody());
                client.close();
            }
            break;
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
        int port = 5001;//Integer.parseInt(args[0]);
        try {
            Thread t = new GreetingServer(port);
            t.start();
        } catch (IOException e) {
            e.printStackTrace();
        }
    }
}

class PartyState {
    private int width;
    private int cliqueCount;
    private String body;

    public PartyState(int width, int cliqueCount, String body) {
        this.width = width;
        this.cliqueCount = cliqueCount;
        this.body = body;
    }

    public int getWidth() {
        return width;
    }

    public void setWidth(int width) {
        this.width = width;
    }

    public int getCliqueCount() {
        return cliqueCount;
    }

    public void setCliqueCount(int cliqueCount) {
        this.cliqueCount = cliqueCount;
    }

    public String getBody() {
        return body;
    }

    public void setBody(String body) {
        this.body = body;
    }
}