import com.mongodb.*;

import java.util.HashMap;
import java.util.Map;

class DAO {
    static MongoClient mongoClient = new MongoClient("localhost", 27017);

    public static int updateCalculationCount(String algorithm, int width, int calculations) {
        int ret = 0;
        try {

            // Now connect to your databases
            DB db = mongoClient.getDB("Calculations");
            DBCollection collection = db.getCollection("Calculations");

            int existing;
            BasicDBObject searchQuery = new BasicDBObject();
            searchQuery.put("width", width);
            DBCursor cursor = collection.find(searchQuery);
            boolean found = false;

            HashMap newMap = new HashMap<>();

            // Update if we have an existing entry for this width 
            if (cursor.hasNext()) {
                BasicDBObject ob = (BasicDBObject) cursor.next();
                if (ob.get("calculations") != null) {
                    newMap = (HashMap) ob.get("calculations");
                    // If entry for this algorithm, get existing and update it 
                    if (newMap.get(algorithm) != null) {
                        existing = (int) newMap.get(algorithm);
                        ret = existing + calculations;
                        newMap.put(algorithm, existing + calculations);
                    } else {
                        // We don't have an entry for this algorithm 
                        ret = calculations;
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
                ret = calculations;
            }

            DBCursor cursor2 = collection.find();
            while (cursor2.hasNext()) {
                BasicDBObject obj = (BasicDBObject) cursor2.next();
                System.out.println(obj);
            }
        } catch (Exception e) {
            Logger.logException(e);
        }
        return ret;
    }
}