
// File Name GreetingServer.java
import java.net.*;
import java.util.ArrayList;
import java.util.List;
import java.nio.charset.StandardCharsets;
import java.nio.file.*;
import java.text.Normalizer.Form;
import java.nio.charset.*;
import java.lang.Process.*;
import java.util.Date;
import java.text.DateFormat;
import java.text.SimpleDateFormat;
import java.util.Map;
import java.util.HashMap;

import org.omg.CORBA.PRIVATE_MEMBER;

import com.mongodb.BasicDBObject;
import com.mongodb.DBObject;
import com.mongodb.DB;
import com.mongodb.DBCollection;
import com.mongodb.DBCursor;
import com.mongodb.MongoClient;
import static com.mongodb.client.model.Filters.*;

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
        Logger.logString("Server running");
        while (true) {
            try {
                Socket client = serverSocket.accept();
                BufferedReader in = new BufferedReader(new InputStreamReader(client.getInputStream()));
                String line = in.readLine();
                String[] lines = line.split("\\s+");

                switch (lines[0]) {
                case "PostExample":
                    //PostExample alg bredde, clic, calcs, state  
                    postExample(client, lines[1], lines[2], lines[3], lines[4], lines[5]);
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

    void postExample(Socket client, String alg, String width, String clientClique, String calculations, String s)
            throws IOException {
        Logger.logEvent("POST EXAMPLE");
        Logger.logClient(client.getInetAddress().toString(), alg, width, clientClique);

        DAO.updateCalculationCount(alg, Integer.parseInt(width), Integer.parseInt(calculations));

        int m = Integer.parseInt(width);
        int cliqueCount = Integer.parseInt(clientClique);

        if (cliqueCount == 0 && m == bestClique.getWidth()) {
            Logger.logNewCounterExample(alg, width);

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
                    Logger.logBetterCliqueCount(alg, bestClique.getCliqueCount());
                }
                break;
            case "EndFlip":
                if ((m >= bestEndFlipClique.getWidth()) && (cliqueCount < bestEndFlipClique.getCliqueCount())) {
                    bestEndFlipClique = new PartyState(m, cliqueCount, s);
                    Logger.logBetterCliqueCount(alg, bestEndFlipClique.getCliqueCount());
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

    PartyState getUniversalBestClique() {
        return bestEndFlipClique.getCliqueCount() < bestClique.getCliqueCount() ? bestEndFlipClique : bestClique;
    }

    void getNextWork(Socket client, String alg, String clientWidthString, String clientCliqueString) throws Exception {
        Logger.logEvent("GET NEXT WORK");
        Logger.logClient(client.getInetAddress().toString(), alg, clientWidthString, clientCliqueString);

        int clientWidth = Integer.parseInt(clientWidthString);
        int clientCliqueCount = Integer.parseInt(clientCliqueString);

        PrintStream out = new PrintStream(client.getOutputStream(), true);
        switch (alg) {
        case "BestClique":
            PartyState bestState = getUniversalBestClique();
            if (clientWidth == bestState.getWidth() && clientCliqueCount <= bestState.getCliqueCount()) {
                out.print(R_CONTINE);
                Logger.logReturnContinue();
            } else {
                Logger.logReturnClique(bestState.getWidth(), bestState.getCliqueCount());
                out.print(bestState.getWidth() + " " + bestState.getCliqueCount() + " " + bestState.getBody());
                client.close();
            }
            break;

        case "EndFlip":
            if (clientWidth == bestEndFlipClique.getWidth()
                    && clientCliqueCount <= bestEndFlipClique.getCliqueCount()) {
                out.print(R_CONTINE);
                Logger.logReturnContinue();
            } else {
                out.print(bestEndFlipClique.getWidth() + " " + bestEndFlipClique.getCliqueCount() + " "
                        + bestEndFlipClique.getBody());
                Logger.logReturnClique(bestEndFlipClique.getWidth(), bestEndFlipClique.getCliqueCount());
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
        int port = 5004;//Integer.parseInt(args[0]);
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

class Logger {
    public static void logEvent(String eventName) {
        printString("**************************** " + eventName + " ****************************");
    }

    public static void logClient(String address, String algorithm, String width, String cliqueCount) {
        printString(String.format("%-30s %-30s %-30s %-30s", "Client: " + address, "Alg: " + algorithm,
                "Problem: " + width, "Best Clique: " + cliqueCount));
    }

    public static void logReturnContinue() {
        printString("RETURNING: CONTINUE\n");
    }

    public static void logReturnClique(int width, int cliqueCount) {
        printString(String.format("%-10s %-5s %-5s\n", "RETURNING:", width, cliqueCount));
    }

    public static void logBetterCliqueCount(String algorithm, int cliqueCount) {
        printString(String.format("%-10s %-10s", algorithm + " better count", cliqueCount));
    }

    public static void logNewCounterExample(String algorithm, String width) {
        printString(">>>>>>>>>>>> " + algorithm + " FOUND NEW COUNTER EXAMPLE AT " + width + "<<<<<<<<<<<<<<<<<<");
    }

    public static void logString(String s) {
        printString(s + "\n");
    }

    public static void logError(Exception e) {
        System.err.println(e.getClass().getName() + ": " + e.getMessage());
    }

    private static void printString(String s) {
        DateFormat dateFormat = new SimpleDateFormat("yyyy/MM/dd HH:mm:ss");
        Date date = new Date();
        System.out.println(String.format("%-15s %-50s", dateFormat.format(date), s));
    }
}

class DAO {
    static MongoClient mongoClient = new MongoClient("localhost", 27017);

    public static void updateCalculationCount(String algorithm, int width, int calculations) {
        try {
            // To connect to mongodb server

            // Now connect to your databases
            DB db = mongoClient.getDB("Calculations");
            DBCollection collection = db.getCollection("Calculations");

            int existing = 0;
            BasicDBObject searchQuery = new BasicDBObject();
            searchQuery.put("width", width);
            DBCursor cursor = collection.find(searchQuery);
            boolean found = false;

            Map<String, Integer> newMap = new HashMap<>();

            // Update if we have an existing entry for this width 
            if (cursor.hasNext()) {
                BasicDBObject ob = (BasicDBObject) cursor.next();
                if (ob.get("calculations") != null) {
                    newMap = (HashMap) ob.get("calculations");
                    // If entry for this algorithm, get existing and update it 
                    if (newMap.get(algorithm) != null) {
                        existing = newMap.get(algorithm);
                        newMap.put(algorithm, existing + calculations);
                    } else {
                        // We don't have an entry for this algorithm 
                        newMap.put(algorithm, calculations);
                    }
                }

                // Update the document
                BasicDBObject newDocument = new BasicDBObject();
                newDocument.put("calculations", newMap);

                BasicDBObject updateObj = new BasicDBObject();
                updateObj.put("$set", newDocument);
                collection.update(searchQuery, updateObj);
            } else {
                // We don't have an entry for this width. create it 
                BasicDBObject document = new BasicDBObject();
                document.put("width", width);
                Map<String, Integer> calcs = new HashMap<>();
                calcs.put(algorithm, calculations);
                document.put("calculations", calcs);
                collection.insert(document);
            }

            DBCursor cursor2 = collection.find();
            while (cursor2.hasNext()) {
                BasicDBObject obj = (BasicDBObject) cursor2.next();
                System.out.println(obj);
            }
        } catch (Exception e) {
            Logger.logError(e);
        }
    }
}