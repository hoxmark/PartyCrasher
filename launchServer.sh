sudo killall java
DATE=`date +%Y-%m-%d:%H:%M:%S`
CURDIR=`pwd`

BACKGROUND=$1
case "$1" in
    0)
        javac -cp ".:$CURDIR/libs/*:" ./Coordinator/*.java
        cd Coordinator
        java -cp ".:$CURDIR/libs/*:" Coordinator 2>&1
        ;;
    1 | *)
        mkdir -p logs
        javac -cp ".:$CURDIR/libs/*:" ./Coordinator/*.java
        cd Coordinator
        java -cp ".:$CURDIR/libs/*:" Coordinator 2>> $CURDIR/logs/$DATE.error.log 1>> $CURDIR/logs/$DATE.log & disown
        echo "Server started"
        ;;
esac
