import com.google.gson.reflect.TypeToken;
import com.mongodb.*;
import com.mongodb.client.model.Filters;

import java.lang.reflect.Type;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

import com.google.gson.Gson;

class DAO {
    static MongoClient mongoClient;
    static DB db;

    public static boolean isMaster(){
        ReplicaSetStatus rss = mongoClient.getReplicaSetStatus();
        return rss.isMaster(mongoClient.getAddress());
    }

    public static void initializeDatabaseClient(){
        mongoClient = new MongoClient("localhost", 27017);
        db = mongoClient.getDB("Calculations");
    }

    public static void savePartyState(String name, PartyState partyState) {
        Gson gson = new Gson();
        String json = gson.toJson(partyState);

        BasicDBObject query = new BasicDBObject();
        query.put("Name", name);

        BasicDBObject object = new BasicDBObject();
        object.put("Name", name);
        object.put("Value", json);
        DBCollection collection = db.getCollection(Config.PARTYSTATE_COLLECTION_NAME);
        collection.update(query, object, true, false);

    }

    public static void saveTabuPair(String name, TabuPair<Integer, Integer> tabuPair) {
        Gson gson = new Gson();
        String json = gson.toJson(tabuPair);

        BasicDBObject query = new BasicDBObject();
        query.put("Name", name);

        BasicDBObject object = new BasicDBObject();
        object.put("Name", name);
        object.put("Value", json);
        DBCollection collection = db.getCollection(Config.TABUPAIR_COLLECTION_NAME);
        collection.update(query, object, true, false);
    }

    public static void saveTabuPairList(String name, List<TabuPair<Integer, Integer>> list) {
        Gson gson = new Gson();
        String json = gson.toJson(list);

        BasicDBObject query = new BasicDBObject();
        query.put("Name", name);

        BasicDBObject object = new BasicDBObject();
        object.put("Name", name);
        object.put("Value", json);
        DBCollection collection = db.getCollection(Config.TABUPAIR_LIST_COLLECTION_NAME);
        collection.update(query, object, true, false);
    }

    public static PartyState loadPartyState(String name) {
        Gson gson = new Gson();

        BasicDBObject query = new BasicDBObject();
        query.put("Name", name);
        DBCursor cursor = db.getCollection(Config.PARTYSTATE_COLLECTION_NAME).find(query);

        while (cursor.hasNext()) {
            BasicDBObject obj = (BasicDBObject) cursor.next();
            String json = (String)obj.get("Value");
            PartyState res = gson.fromJson(json, PartyState.class);
            return res;
        }
        return null;
    }

    public static TabuPair<Integer, Integer> loadTabuPair(String name) {
        Gson gson = new Gson();
        Type collectionType = new TypeToken<TabuPair<Integer, Integer>>(){}.getType();

        BasicDBObject query = new BasicDBObject();
        query.put("Name", name);
        DBCursor cursor = db.getCollection(Config.TABUPAIR_COLLECTION_NAME).find(query);

        while (cursor.hasNext()) {
            BasicDBObject obj = (BasicDBObject) cursor.next();
            String json = (String)obj.get("Value");
            TabuPair<Integer, Integer> res = gson.fromJson(json, collectionType);
            return res;
        }
        return null;
    }
    public static List<TabuPair<Integer, Integer>> loadTabuPairList(String name) {
        Gson gson = new Gson();
        Type collectionType = new TypeToken<List<TabuPair<Integer, Integer>>>(){}.getType();

        BasicDBObject query = new BasicDBObject();
        query.put("Name", name);
        DBCursor cursor = db.getCollection(Config.TABUPAIR_LIST_COLLECTION_NAME).find(query);

        while (cursor.hasNext()) {
            BasicDBObject obj = (BasicDBObject) cursor.next();
            String json = (String)obj.get("Value");
            List<TabuPair<Integer, Integer>> res = gson.fromJson(json, collectionType);
            return res;
        }
        return null;
    }


    public static int updateCalculationCount(String algorithm, int width, int calculations) {
        int ret = 0;
        try {

            // Now connect to your databases

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

        } catch (Exception e) {
            Logger.logException(e);
        }
        return ret;
    }
}
