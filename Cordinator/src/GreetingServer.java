import java.net.*;
import java.util.ArrayList;
import java.util.List;
import java.nio.charset.StandardCharsets;
import java.nio.file.*;
import java.nio.charset.*;
import java.io.*;
import java.util.Collections;

public class GreetingServer extends Thread {
    private ServerSocket serverSocket;
    private PartyState bestClique;
    private PartyState bestEndFlipClique;
    private PartyState annealingState;
    private boolean annealing = false;
    private int annealingCalculations = 0;

    public GreetingServer(int port) throws IOException {
        serverSocket = new ServerSocket(port);
        bestClique = new PartyState(0, Integer.MAX_VALUE, "");
        bestEndFlipClique = new PartyState(0, Integer.MAX_VALUE, "");
        annealingState = new PartyState(0, Integer.MAX_VALUE, "");

        try {
            generateNewWidth();
        } catch (Exception e) {
            Logger.logException(e);
        }
    }

    public void run() {
        Logger.logString("Server running");
        Socket client = null;
        while (true) {
            try {
                client = serverSocket.accept();
                BufferedReader in = new BufferedReader(new InputStreamReader(client.getInputStream()));
                String line = in.readLine();
                String[] lines = line.split("\\s+");

                switch (lines[0]) {
                case Config.POSTEXAMPLE:
                    //PostExample alg bredde, clic, calcs, state  
                    postExample(client, lines[1], lines[2], lines[3], lines[4], lines[5]);
                    break;
                case Config.GET_NEXT_WORK:
                    //PostExample alg bredde 
                    try {
                        getNextWork(client, lines[1], lines[2], lines[3]);
                    } catch (Exception e) {
                        Logger.logException(e);
                    }
                    break;

                default:
                    System.out.println("Error: Default in switch");
                }
                client.close();

            } catch (Exception e) {
                Logger.logException(e);
                if(client != null) {
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

        int clientWidth = Integer.parseInt(width);
        int clientCliqueCount = Integer.parseInt(clientClique);
        int clientCalculations = Integer.parseInt(calculations);

        DAO.updateCalculationCount(alg, Integer.parseInt(width), clientCalculations);

        /* Add the new calculations done by the client if we are not annealing */
        if(clientWidth == this.bestEndFlipClique.getWidth() && !this.annealing) this.annealingCalculations += clientCalculations;

        /* If the new count puts us over the annealing threshold, anneal for some time */
        if (clientWidth == getUniversalBestClique().getWidth() && this.annealingCalculations > Config.ANNEALING_THRESHOLD && !this.annealing) {
            Logger.logEnterState("Annealing");
            this.annealing = true;
            this.generateNewWidth();

            // Launch a thread that sets annealing to false after a timeout

            new Thread(new Runnable() {
                @Override
                public void run() {
                    try {
                        Thread.sleep(Config.ANNEALING_TIMEOUT_MS);
                    } catch (InterruptedException e) {
                        Logger.logException(e);
                    }
                    Logger.logEnterState("Default");
                    annealing = false;
                    annealingCalculations = 0;
                }
            }).start();
        }
        // Only process the posted state if we're not in an annealing state
        if (!this.annealing) {
            if (clientCliqueCount == 0 && clientWidth == bestClique.getWidth()) {
                this.annealingCalculations = 0;
                Logger.logNewCounterExample(alg, width);

                File file = new File("../../counterexamples/" + clientWidth + ".txt");
                BufferedWriter writer = new BufferedWriter(new FileWriter(file));
                writer.write(clientWidth + " 0 \n");

                for (int i = 0; i < clientWidth * clientWidth; i++) {
                    if (i % clientWidth == 0 && i != 0)
                        writer.write("\n");
                    writer.write(s.charAt(i) + " ");
                }
                writer.flush();
                try {
                    generateNewWidth();

                } catch (Exception e) {
                    Logger.logException(e);
                }

            } else {
                switch (alg) {
                case Config.BESTLICUQE_ALGORITHM_NAME:
                    if ((clientWidth >= bestClique.getWidth()) && (clientCliqueCount < bestClique.getCliqueCount())) {
                        this.annealingCalculations = 0;
                        bestClique = new PartyState(clientWidth, clientCliqueCount, s);
                        Logger.logBetterCliqueCount(alg, bestClique.getCliqueCount());
                    }
                    break;
                case Config.ENDFLIP_ALGORITHM_NAME:
                    if ((clientWidth >= bestEndFlipClique.getWidth())
                            && (clientCliqueCount < bestEndFlipClique.getCliqueCount())) {
                        this.annealingCalculations = 0;
                        bestEndFlipClique = new PartyState(clientWidth, clientCliqueCount, s);
                        Logger.logBetterCliqueCount(alg, bestEndFlipClique.getCliqueCount());
                    }
                    break;
                }
            }
        }
    }

    void generateNewWidth() {
        String best = findBestCounterExample();
        int m = Integer.parseInt(best);

        bestClique.setWidth(m + 1);
        bestClique.setCliqueCount(Integer.MAX_VALUE);

        bestEndFlipClique.setWidth(m + 1);
        bestEndFlipClique.setCliqueCount(Integer.MAX_VALUE);

        annealingState.setWidth(m + 1);
        annealingState.setCliqueCount(Integer.MAX_VALUE);

        String file = null;
        try {
            file = fileToString(best);
        } catch (Exception e) {
            Logger.logException(e);
        }
        String oldBody = file.substring(file.indexOf('\n') + 1).replace(" ", "").replace("\n", "");
        String newBody = "";

        for (int i = 0; i < m + 1; i++) {
            newBody += "0";
        }

        for (int i = 0; i < m * m; i++) {
            if (i % m == 0)
                newBody += "0";
            newBody += oldBody.charAt(i);
        }

        // System.out.print(newBody);
        bestClique.setBody(newBody);
        bestEndFlipClique.setBody(newBody);
        annealingState.setBody(newBody);
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

        // If we're annealing - return the basis again
        if (this.annealing) {
            Logger.logReturnAnnealingState();
            out.print(this.annealingState.getWidth() + " " + this.annealingState.getCliqueCount() + " "
                    + this.annealingState.getBody());
        } else {
            switch (alg) {
            case Config.BESTLICUQE_ALGORITHM_NAME:
                PartyState bestState = getUniversalBestClique();
                if (clientWidth == bestState.getWidth() && clientCliqueCount <= bestState.getCliqueCount()) {
                    out.print(Config.R_CONTINE);
                    Logger.logReturnContinue();
                } else {
                    Logger.logReturnClique(bestState.getWidth(), bestState.getCliqueCount());
                    out.print(bestState.getWidth() + " " + bestState.getCliqueCount() + " " + bestState.getBody());
                    client.close();
                }
                break;

            case Config.ENDFLIP_ALGORITHM_NAME:
                if (clientWidth == bestEndFlipClique.getWidth()
                        && clientCliqueCount <= bestEndFlipClique.getCliqueCount()) {
                    out.print(Config.R_CONTINE);
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
            Logger.logException(e);
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

