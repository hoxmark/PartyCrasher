import java.text.DateFormat;
import java.text.SimpleDateFormat;
import java.util.Date;

class Logger {
    static void logEnterState(String stateName){
        printString("Entering " + stateName + " state");
    }

    static void logEvent(String eventName) {
        printString("**************************** " + eventName + " ****************************");
    }

    static void logClient(String address, String algorithm, String width, String cliqueCount) {
        printString(String.format("%-30s %-30s %-30s %-30s", "Client: " + address, "Alg: " + algorithm,
                "Problem: " + width, "Best Clique: " + cliqueCount));
    }

    static void logReturnContinue() {
        printString("RETURNING: CONTINUE\n");
    }

    static void logReturnClique(int width, int cliqueCount) {
        printString(String.format("%-10s %-5s %-5s\n", "RETURNING:", width, cliqueCount));
    }

    static void logReturnAnnealingState(){
        printString("RETURNING: Annealed state ");
    }

    static void logBetterCliqueCount(String algorithm, int cliqueCount) {
        printString(String.format("%-10s %-10s", algorithm + " better count", cliqueCount));
    }

    static void logNewCounterExample(String algorithm, String width) {
        printString(">>>>>>>>>>>> " + algorithm + " FOUND NEW COUNTER EXAMPLE AT " + width + "<<<<<<<<<<<<<<<<<<");
    }

    static void logString(String s) {
        printString(s + "\n");
    }

    static void logException(Exception e) {
        e.printStackTrace();
        DateFormat dateFormat = new SimpleDateFormat("yyyy/MM/dd HH:mm:ss");
        Date date = new Date();
        System.err.println(
                String.format("%-15s %-100s", dateFormat.format(date), e.getClass().getName() + ": " + e.getMessage()));
    }

    private static void printString(String s) {
        DateFormat dateFormat = new SimpleDateFormat("yyyy/MM/dd HH:mm:ss");
        Date date = new Date();
        System.out.println(String.format("%-15s %-50s", dateFormat.format(date), s));
    }
}

