import java.lang.reflect.Array;
import java.net.*;
import java.util.*;
import java.nio.charset.StandardCharsets;
import java.nio.file.*;
import java.nio.charset.*;
import java.io.*;

public class Coordinator extends Thread {
    private ServerSocket serverSocket;
    private Map<String, Date> connectedServers;


    /* Annealing state we return to when annealing */
    private PartyState annealingState;

    /* State for random flip search */
    private PartyState bestClique;

    /* State for endflip search */
    private PartyState bestEndFlipClique;

    /* State for normal tabu search */
    private PartyState currentTabuSearchClique;
    private PartyState bestTabuSearchClique;
    private TabuPair<Integer, Integer> bestTabuSearchFlip;
    private List<TabuPair<Integer, Integer>> tabuSearchValidFlipList;
    private List<TabuPair<Integer, Integer>> tabuSearchTabuList;

    /* State for endflip tabu search */
    private PartyState currentEndFlipTabueClique;
    private PartyState bestEndFlipTabuClique;
    private TabuPair<Integer, Integer> bestEndFlipTabuFlip;
    private List<TabuPair<Integer, Integer>> endflipValidFlipList;
    private List<TabuPair<Integer, Integer>> endflipTabuList;


    private boolean annealing = false;
    private int annealingCalculations = 0;
    private int tabuCalculations = 0;


    public Coordinator(int port){
        try {
            serverSocket = new ServerSocket(port);
        } catch (IOException e) {
            Logger.logException(e);
        }
        connectedServers = new HashMap<>();
        bestClique = new PartyState(0, Integer.MAX_VALUE, "");
        bestEndFlipClique = new PartyState(0, Integer.MAX_VALUE, "");
        annealingState = new PartyState(0, Integer.MAX_VALUE, "");

        /* Normal tabu search  */
        currentTabuSearchClique = new PartyState(0, Integer.MAX_VALUE, "");
        bestTabuSearchClique = new PartyState(0, Integer.MAX_VALUE, "");
        tabuSearchValidFlipList = new ArrayList<>();
        tabuSearchTabuList = new ArrayList<>();

        /* Endflip tabu search */
        currentEndFlipTabueClique = new PartyState(0, Integer.MAX_VALUE, "");
        bestEndFlipTabuClique = new PartyState(0, Integer.MAX_VALUE, "");
        endflipValidFlipList = new ArrayList<>();
        endflipTabuList = new ArrayList<>();

        try {
            generateNewWidth();
        } catch (Exception e) {
            Logger.logException(e);
        }

        Timer timer = new Timer();
        timer.schedule(new TimerTask() {
            @Override
            public void run() {
                updateConnectedServersMap();
            }
        }, 0, Config.CLIENT_UPDATE_INTERVAL_SECONDS * 1000);
    }

    private void updateConnectedServersMap() {
        Date now = new Date();
        for (Iterator<Map.Entry<String, Date>> it = connectedServers.entrySet().iterator(); it.hasNext(); ) {
            Map.Entry<String, Date> entry = it.next();
            long secondsSinceConnected = (now.getTime() - entry.getValue().getTime()) / 1000;
            if (secondsSinceConnected > Config.CLIENT_UPDATE_INTERVAL_SECONDS * 6) {
                it.remove();
            }
        }
        Logger.logString("Connected clients: " + connectedServers.size());
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

                String clientId = lines[1];
                String clientAlgorithm;
                String clientWidth;
                String clientCliqueCount;

                this.connectedServers.put(clientId, new Date());

                switch (lines[0]) {
                    case Config.POSTEXAMPLE:
                        postExample(client, lines);
                        break;
                    case Config.GET_NEXT_WORK:
//                        TODO make consistent with the rest
                        clientId = lines[1];
                        clientAlgorithm = lines[2];
                        clientWidth = lines[3];
                        clientCliqueCount = lines[4];
                        getNextWork(client, clientId, clientAlgorithm, clientWidth, clientCliqueCount);
                        break;


                    case Config.POST_TABU:
                        postTabuExample(client, lines);
                        break;

                    case Config.GET_TABU_LIST:
                        getTabuList(client, lines);
                        break;

                    case Config.GET_NEXT_TABU_FLIP_INDEX:
                        getNextTabuFlipIndex(client, lines);
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

    String fileToString(String value) {
        String filename = ("../counterexamples/" + value + ".txt");
        return readFile(filename, StandardCharsets.UTF_8);
    }

    static String readFile(String path, Charset encoding) {
        byte[] encoded = new byte[0];
        try {
            encoded = Files.readAllBytes(Paths.get(path));
        } catch (IOException e) {
            Logger.logException(e);
        }
        return new String(encoded, encoding);
    }

    void getTabuList(Socket client, String[] args) {
        String clientAlgorithm = args[2];


        PrintStream out;
        try {
            out = new PrintStream(client.getOutputStream(), true);
            List<TabuPair<Integer, Integer>> tabuList = new ArrayList<>();
            switch (clientAlgorithm) {
                case Config.ENDFLIP_TABU_ALGORITHM_NAME:
                    tabuList = this.endflipTabuList;
                    break;

                case Config.TABU_ALGORITHM_NAME:
                    tabuList = this.tabuSearchTabuList;
                    break;
            }

            String ret = "";
            for (TabuPair tabu : tabuList) {
                System.out.println("WE ARE ADDING IT ");
                Logger.logString("(" + tabu.getLeft() + ", " + tabu.getRight() + ")");
                ret += "(" + tabu.getLeft() + ", " + tabu.getRight() + ") ";
            }
            out.print(ret);
        } catch (IOException e) {
            e.printStackTrace();
        }
    }

    void getNextTabuFlipIndex(Socket client, String[] args) {
        PrintStream out;
        String clientAlgorithm = args[2];

        try {
            out = new PrintStream(client.getOutputStream(), true);

            List<TabuPair<Integer, Integer>> validFLipList = null;
            switch (clientAlgorithm) {
                case Config.TABU_ALGORITHM_NAME:
                    validFLipList = this.tabuSearchValidFlipList;
                    break;

                case Config.ENDFLIP_TABU_ALGORITHM_NAME:
                    validFLipList = this.endflipValidFlipList;
                    break;
            }

            if (!validFLipList.isEmpty()) {
                TabuPair<Integer, Integer> pair = validFLipList.get(new Random().nextInt(validFLipList.size()));
                Logger.logEvent("GetNextEndFlipTabuIndex");
                Logger.logString("Returning (" + pair.getLeft() + ", " + pair.getRight() + ")");
                out.print(pair.getLeft() + " " + pair.getRight() + " ");
            } else {
                Logger.logEvent("No valid flips left - returning wait ");
                out.print("W ");
            }
        } catch (IOException e) {
            e.printStackTrace();
        }
    }

    void getNextWork(Socket client, String clientId, String alg, String clientWidthString, String clientCliqueString) {
        Logger.logEvent("GET NEXT WORK");
        Logger.logClient(client.getInetAddress().toString(), alg, clientWidthString, clientCliqueString);

        int clientWidth = Integer.parseInt(clientWidthString);
        int clientCliqueCount = Integer.parseInt(clientCliqueString);

        PrintStream out;
        try {
            out = new PrintStream(client.getOutputStream(), true);

            //             If we're annealing - return the basis again
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
                        }
                        break;

                    case Config.BESTFLIP_ALGORITHM_NAME:
                    case Config.ENDFLIP_ALGORITHM_NAME:
                        //                        if (this.bestEndFlipClique.getCliqueCount() < Config.ENDFLIP_TO_TABU_THRESHOLD) {
                        //                            /* RETURN SWITCH TO tabu search */
                        //                            Logger.logString("SWITCHING TO TABUUUU");
                        //                            out.print("N " + Config.TABU_ALGORITHM_NAME);
                        //                        } else {
                        if (clientWidth == bestEndFlipClique.getWidth()
                                && clientCliqueCount <= bestEndFlipClique.getCliqueCount()) {
                            out.print(Config.R_CONTINE);
                            Logger.logReturnContinue();
                        } else {
                            out.print(bestEndFlipClique.getWidth() + " " + bestEndFlipClique.getCliqueCount() + " "
                                    + bestEndFlipClique.getBody());
                            Logger.logReturnClique(bestEndFlipClique.getWidth(), bestEndFlipClique.getCliqueCount());
                        }
                        //                        }
                        break;

                    case Config.TABU_ALGORITHM_NAME:
                        out.print(currentTabuSearchClique.getWidth() + " " + currentTabuSearchClique.getCliqueCount() + " "
                                + currentTabuSearchClique.getBody());
                        Logger.logReturnClique(currentTabuSearchClique.getWidth(), currentTabuSearchClique.getCliqueCount());
                        break;

                    case Config.ENDFLIP_TABU_ALGORITHM_NAME:
                        out.print(currentEndFlipTabueClique.getWidth() + " " + currentEndFlipTabueClique.getCliqueCount() + " "
                                + currentEndFlipTabueClique.getBody());
                        Logger.logReturnClique(currentEndFlipTabueClique.getWidth(), currentEndFlipTabueClique.getCliqueCount());
                        break;
                }
                client.close();
            }
        } catch (IOException e) {
            Logger.logException(e);
        }
    }

    boolean checkAndWriteCounterexample(String clientAlgorithm, int clientWidth, int clientCliqueCount, String clientState) {
        if (clientCliqueCount == 0 && clientWidth == bestClique.getWidth()) {
            Logger.logNewCounterExample(clientAlgorithm, "" + clientWidth);

            File file = new File("../counterexamples/" + clientWidth + ".txt");
            BufferedWriter writer = null;
            try {
                writer = new BufferedWriter(new FileWriter(file));
                writer.write(clientWidth + " 0 \n");

                for (int i = 0; i < clientWidth * clientWidth; i++) {
                    if (i % clientWidth == 0 && i != 0)
                        writer.write("\n");
                    writer.write(clientState.charAt(i) + " ");
                }
                writer.flush();
            } catch (IOException e) {
                Logger.logException(e);
            }

            generateNewWidth();

            this.annealingCalculations = 0;
            this.tabuCalculations = 0;
            this.annealing = false;
            return true;
        }
        return false;
    }

    void updateEndFlipTabuState(String clientAlgorithm) {
        switch (clientAlgorithm) {
            case Config.ENDFLIP_TABU_ALGORITHM_NAME:
                if (this.endflipValidFlipList.isEmpty()) {
                    Logger.logEvent("Moving currentEndFlipTabu to bestEndFlipTabu. Best flip was (" + this.bestEndFlipTabuFlip.getLeft() + ", " + this.bestEndFlipTabuFlip.getRight() + ")");
                    this.currentEndFlipTabueClique.setWidth(this.bestEndFlipTabuClique.getWidth());
                    this.currentEndFlipTabueClique.setBody(this.bestEndFlipTabuClique.getBody());
                    this.currentEndFlipTabueClique.setCliqueCount(this.bestEndFlipTabuClique.getCliqueCount());
                    Logger.logEvent("NEW CLIQUE COUNT IS " + this.currentEndFlipTabueClique.getCliqueCount());

                    this.endflipValidFlipList = new ArrayList<>();
                    this.endflipTabuList.add(this.bestEndFlipTabuFlip);
                    for (int i = 0; i < this.bestEndFlipTabuClique.getWidth(); i++) {
                        TabuPair<Integer, Integer> pair = new TabuPair<>(0, i);
                        if (!this.endflipTabuList.contains(pair)) this.endflipValidFlipList.add(pair);
                    }
                    this.bestEndFlipTabuClique.setCliqueCount(Integer.MAX_VALUE);
                    Logger.logEvent("New valid flip size is " + this.endflipValidFlipList.size());
                }

            case Config.TABU_ALGORITHM_NAME:
                if (this.tabuSearchValidFlipList.isEmpty()) {
                    Logger.logEvent("Moving currentTabuSearchState to bestTabuSearchState. Best flip was (" + this.bestTabuSearchFlip.getLeft() + ", " + this.bestTabuSearchFlip.getRight() + ")");
                    this.currentTabuSearchClique.setWidth(this.bestTabuSearchClique.getWidth());
                    this.currentTabuSearchClique.setBody(this.bestTabuSearchClique.getBody());
                    this.currentTabuSearchClique.setCliqueCount(this.bestTabuSearchClique.getCliqueCount());
                    Logger.logEvent("NEW CLIQUE COUNT IS " + this.currentTabuSearchClique.getCliqueCount());

                    this.tabuSearchValidFlipList = new ArrayList<>();
                    this.tabuSearchTabuList.add(this.bestTabuSearchFlip);
                    for (int i = 0; i < this.bestTabuSearchClique.getWidth(); i++) {
                        for(int j = i; j < this.bestTabuSearchClique.getWidth(); j++){
                            TabuPair<Integer, Integer> pair = new TabuPair<>(i, j);
                            if (!this.tabuSearchTabuList.contains(pair)) this.tabuSearchValidFlipList.add(pair);
                        }
                    }
                    this.bestTabuSearchClique.setCliqueCount(Integer.MAX_VALUE);
                    Logger.logEvent("New valid flip size is " + this.tabuSearchValidFlipList.size());

                    /* If the valid flips are empty, we have to restart */
                    if(this.tabuSearchValidFlipList.isEmpty()) restartTabuSearchState();
                }
                break;
        }
    }

    private String generateRandomBody(int m){
        String randomBody = "";
        for(int i = 0; i <= m; i++){
            for(int j = 0; j <= m; j++){
                if(j < i ) randomBody += "0";
                else {
                    if(new Random().nextBoolean()) randomBody += "1";
                    else randomBody += "0";
                }
            }
        }
        return randomBody;
    }

    void restartTabuSearchState(){
        String best = findBestCounterExample();
        int m = Integer.parseInt(best);

        bestTabuSearchClique.setWidth(m + 1);
        bestTabuSearchClique.setCliqueCount(Integer.MAX_VALUE);
        currentTabuSearchClique.setWidth(m + 1);
        currentTabuSearchClique.setCliqueCount(Integer.MAX_VALUE);


        tabuSearchValidFlipList = new ArrayList<>();
        tabuSearchTabuList = new ArrayList<>();

        for(int i = 0; i < m; i++){
            for(int j = i; j < m; j++){
                tabuSearchValidFlipList.add(new TabuPair<>(i, j));
            }
        }

        String randomBody = generateRandomBody(m);
        currentTabuSearchClique.setBody(randomBody);
        bestTabuSearchClique.setBody(randomBody);
    }

    void postTabuExample(Socket client, String[] lines) {
        String clientId = lines[1];
        String clientAlgorithm = lines[2];
        String clientBestFlipIString = lines[3];
        String clientBestFlipJString = lines[4];
        String clientWidthString = lines[5];
        String clientCliqueCountString = lines[6];
        String clientCalculationsString = lines[7];
        String clientState = lines[8];

        int clientWidth = Integer.parseInt(clientWidthString);
        int clientCliqueCount = Integer.parseInt(clientCliqueCountString);
        int clientCalculations = Integer.parseInt(clientCalculationsString);
        int clientBestFlipI = Integer.parseInt(clientBestFlipIString);
        int clientBestFlipJ = Integer.parseInt(clientBestFlipJString);

        Logger.logEvent("POST EXAMPLE");
        Logger.logClient(client.getInetAddress().toString(), clientAlgorithm, clientWidthString, clientCliqueCountString);
        DAO.updateCalculationCount(clientAlgorithm, clientWidth, clientCalculations);

        /* Write counterexample if we fount it*/
        if (!checkAndWriteCounterexample(clientAlgorithm, clientWidth, clientCliqueCount, clientState)) {

            if (!this.annealing) {
                switch (clientAlgorithm) {
                    case Config.TABU_ALGORITHM_NAME:
                        if (clientWidth >= bestTabuSearchClique.getWidth()) {
                            TabuPair<Integer, Integer> clientPair = new TabuPair<>(clientBestFlipI, clientBestFlipJ);

                            this.tabuSearchValidFlipList.remove(clientPair);

                            Logger.logEvent("Number of valid flips remaining: " + this.tabuSearchValidFlipList.size());

                            if (clientCliqueCount < this.bestTabuSearchClique.getCliqueCount()) {
                                Logger.logEvent("Updating best endflip clique count");
                                this.bestTabuSearchClique = new PartyState(clientWidth, clientCliqueCount, clientState);
                                this.bestTabuSearchFlip = clientPair;
                            }

                            updateEndFlipTabuState(clientAlgorithm);
                        }
                        break;
                    case Config.ENDFLIP_TABU_ALGORITHM_NAME:
                        if (clientWidth >= bestEndFlipTabuClique.getWidth()) {
                            TabuPair<Integer, Integer> clientPair = new TabuPair<>(clientBestFlipI, clientBestFlipJ);

                            this.endflipValidFlipList.remove(clientPair);

                            Logger.logEvent("Number of valid flips remaining: " + this.endflipValidFlipList.size());

                            if (clientCliqueCount < this.bestEndFlipTabuClique.getCliqueCount()) {
                                Logger.logEvent("Updating best endflip clique count");
                                this.bestEndFlipTabuClique = new PartyState(clientWidth, clientCliqueCount, clientState);
                                this.bestEndFlipTabuFlip = clientPair;
                            }

                            updateEndFlipTabuState(clientAlgorithm);
                        }
                        break;
                }
            }
        }
    }

    void postExample(Socket client, String[] lines) {

        //        String clientId, String alg, String width, String clientClique, String calculations, String s
        String clientId = lines[1];
        String clientAlgorithm = lines[2];
        String clientWidthString = lines[3];
        String clientCliqueCountString = lines[4];
        String clientCalculationsString = lines[5];
        String clientState = lines[6];


        Logger.logEvent("POST EXAMPLE");
        Logger.logClient(client.getInetAddress().toString(), clientAlgorithm, clientWidthString, clientCliqueCountString);

        int clientWidth = Integer.parseInt(clientWidthString);
        int clientCliqueCount = Integer.parseInt(clientCliqueCountString);
        int clientCalculations = Integer.parseInt(clientCalculationsString);

        DAO.updateCalculationCount(clientAlgorithm, clientWidth, clientCalculations);
        checkAndWriteCounterexample(clientAlgorithm, clientWidth, clientCliqueCount, clientState);

        // Only process the posted state if we're not in an annealing state
        if (!this.annealing) {
            switch (clientAlgorithm) {
                case Config.BESTLICUQE_ALGORITHM_NAME:
                    if ((clientWidth >= bestClique.getWidth()) && (clientCliqueCount < bestClique.getCliqueCount())) {
                        //                        this.annealingCalculations = 0;
                        bestClique = new PartyState(clientWidth, clientCliqueCount, clientState);
                        Logger.logBetterCliqueCount(clientAlgorithm, bestClique.getCliqueCount());
                    }
                    break;

                case Config.BESTFLIP_ALGORITHM_NAME:
                case Config.ENDFLIP_ALGORITHM_NAME:
                    if ((clientWidth >= bestEndFlipClique.getWidth())
                            && (clientCliqueCount < bestEndFlipClique.getCliqueCount())) {
                        //                        this.annealingCalculations = 0;
                        bestEndFlipClique = new PartyState(clientWidth, clientCliqueCount, clientState);
                        Logger.logBetterCliqueCount(clientAlgorithm, bestEndFlipClique.getCliqueCount());
                    }
                    break;
            }
        }
    }


    void generateNewWidth() {
        String best = findBestCounterExample();
        int m = Integer.parseInt(best);

        annealingState.setWidth(m + 1);
        annealingState.setCliqueCount(Integer.MAX_VALUE);

        bestClique.setWidth(m + 1);
        bestClique.setCliqueCount(Integer.MAX_VALUE);

        bestEndFlipClique.setWidth(m + 1);
        bestEndFlipClique.setCliqueCount(Integer.MAX_VALUE);


        bestTabuSearchClique.setWidth(m + 1);
        bestTabuSearchClique.setCliqueCount(Integer.MAX_VALUE);
        currentTabuSearchClique.setWidth(m + 1);
        currentTabuSearchClique.setCliqueCount(Integer.MAX_VALUE);


        bestEndFlipTabuClique.setWidth(m + 1);
        bestEndFlipTabuClique.setCliqueCount(Integer.MAX_VALUE);
        currentEndFlipTabueClique.setWidth(m + 1);
        currentEndFlipTabueClique.setCliqueCount(Integer.MAX_VALUE);


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
        annealingState.setBody(newBody);
        bestClique.setBody(newBody);
        bestEndFlipClique.setBody(newBody);


        currentEndFlipTabueClique.setBody(newBody);
        bestEndFlipTabuClique.setBody(newBody);
        endflipTabuList = new ArrayList<>();
        endflipValidFlipList = new ArrayList<>();

        for (int i = 0; i < m; i++) {
            this.endflipValidFlipList.add(new TabuPair<>(0, i));
        }

        String randomBody = generateRandomBody(m);
        tabuSearchValidFlipList = new ArrayList<>();
        tabuSearchTabuList = new ArrayList<>();

        for(int i = 0; i < m; i++){
            for(int j = i; j < m; j++){
                tabuSearchValidFlipList.add(new TabuPair<>(i, j));
            }
        }
        currentTabuSearchClique.setBody(randomBody);
        bestTabuSearchClique.setBody(randomBody);
    }

    PartyState getUniversalBestClique() {
        ArrayList<PartyState> states = new ArrayList<>();
        states.add(bestClique);
        states.add(bestEndFlipClique);
        states.add(bestTabuSearchClique);

        PartyState bestState = bestEndFlipClique;

        for (PartyState state : states) {
            if (state.getCliqueCount() < bestState.getCliqueCount()) bestState = state;
        }
        return bestState;
    }

    String findBestCounterExample() {
        File folder = new File("../counterexamples/");
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

        Thread t = new Coordinator(port);
        t.start();
    }
}

class TabuPair<L, R> {

    private final L left;
    private final R right;

    public TabuPair(L left, R right) {
        this.left = left;
        this.right = right;
    }

    public L getLeft() {
        return left;
    }

    public R getRight() {
        return right;
    }


    @Override
    public int hashCode() {
        return left.hashCode() ^ right.hashCode();
    }

    @Override
    public boolean equals(Object o) {
        if (!(o instanceof TabuPair)) return false;
        TabuPair pairo = (TabuPair) o;
        return this.left.equals(pairo.getLeft()) &&
                this.right.equals(pairo.getRight());
    }

}