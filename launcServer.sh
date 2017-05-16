sudo killall java
DATE=`date +%Y-%m-%d:%H:%M:%S`
CURDIR=`pwd`
mkdir -p logs

javac -cp ".:$CURDIR/libs/mongo-java-driver-3.0.0.jar:" ./Cordinator/src/GreetingServer.java
cd Cordinator/src

java -cp ".:$CURDIR/libs/mongo-java-driver-3.0.0.jar:" GreetingServer 2>> $CURDIR/logs/$DATE.error.log 1>> $CURDIR/logs/$DATE.log
