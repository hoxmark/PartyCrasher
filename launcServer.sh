sudo killall java
DATE=`date +%Y-%m-%d:%H:%M:%S`
CURDIR=`pwd`

BACKGROUND=$1
case "$1" in
    0)
        javac -cp ".:$CURDIR/libs/mongo-java-driver-3.0.0.jar:" ./Coordinator/*.java
        cd Coordinator
        # java -cp ".:$CURDIR/libs/mongo-java-driver-3.0.0.jar:" GreetingServer 2>&1
        java -cp ".:$CURDIR/libs/mongo-java-driver-3.0.0.jar:" LoadBalancer 2>&1
        ;;
    1 | *)
        mkdir -p logs
        javac -cp ".:$CURDIR/libs/mongo-java-driver-3.0.0.jar:" ./Coordinator/*.java
        cd Cordinator
        java -cp ".:$CURDIR/libs/mongo-java-driver-3.0.0.jar:" GreetingServer 2>> $CURDIR/logs/$DATE.error.log 1>> $CURDIR/logs/$DATE.log & disown
        echo "Server started"
        ;;
    # *)
    #
    #     javac -cp ".:$CURDIR/libs/mongo-java-driver-3.0.0.jar:" ./Cordinator/src/*.java
    #     cd Cordinator/src
    #     java -cp ".:$CURDIR/libs/mongo-java-driver-3.0.0.jar:" GreetingServer 2>> $CURDIR/logs/$DATE.error.log 1>> $CURDIR/logs/$DATE.log & disown
    #     ;;
esac
