class Config {
    public static final String SERVERPORT = "5004";
    public static final String GETSERVERIP = "GetServerIp";
    public static final String POSTEXAMPLE = "PostExample";
    public static final String POST_TABU = "PostTabuExample";
    public static final String GET_NEXT_WORK = "GetNextWork";
    public static final String GET_TABU_LIST = "GetTabuList";
    public static final String ENDFLIP_ALGORITHM_NAME = "EndFlip";
    public static final String BESTLICUQE_ALGORITHM_NAME = "BestClique";
    public static final String BESTFLIP_ALGORITHM_NAME = "BestFlip";
    public static final String TABU_ALGORITHM_NAME = "TabuSearch";
    public static final String ENDFLIP_TABU_ALGORITHM_NAME = "EndFlipTabu";
    public static final String POST_ENDFLIP_TABU = "PostEndFlipTabu";
    public static final String GET_ENDFLIP_TABU_LIST = "GetEndFlipTabuList";
    public static final String GET_NEXT_TABU_FLIP_INDEX = "GetNextTabuFlipIndex";

    public static final int ENDFLIP_TO_TABU_THRESHOLD = 100;

    public static final int CLIENT_UPDATE_INTERVAL_SECONDS = 50;
    public static final int ANNEALING_THRESHOLD = 1000000;
    public static final int ANNEALING_TIMEOUT_MS = CLIENT_UPDATE_INTERVAL_SECONDS * 1000;
    public static final String R_CONTINE = "CONTINUE";
    public static final String R_RETRY = "RETRY";


    /* Database collection names */
    public static final String PARTYSTATE_COLLECTION_NAME = "PartyState";
    public static final String TABUPAIR_COLLECTION_NAME = "TabuPair";
    public static final String TABUPAIR_LIST_COLLECTION_NAME = "TabuPairList";

    public static final int SAVE_STATE_INTERVAL_SECONDS = 60;
    
    public static final String[] SERVERIPS = {"104.198.30.238", "104.198.30.238", "104.198.30.238"};
}
