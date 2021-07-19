const admin = require('firebase-admin')
const serviceAccount = require('./ServiceAccountKey.json')
admin.initializeApp({
    credential: admin.credential.cert(serviceAccount),
    databaseURL: "https://greenhouse-367c4-default-rtdb.firebaseio.com/"
});
const db = admin.firestore(); 
const rtdb = admin.database(); 
const ref = rtdb.ref('/')
let d = new Date(); 
console.log('Firestore update in progress...');
console.log(d.getTime())
let snapshotData; 
let docCount; 

let dataVal = {
    temperature: "", 
    humidity: "", 
    brightness: "", 
    pressure: ""
}




ref.on('value', (snapshot) => {
    console.log("Val: " + snapshot.val().lastPosted.unixTime);
    snapshotData = snapshot.val(); 
    updateFirestore();
}, (errorObject) => {
    console.log("Error: " + errorObject.name); 
}); 

function updateFirestore() {
    dataVal={
        temperature: snapshotData.readings.temperature,
        humidity: snapshotData.readings.humidity, 
        brightness: snapshotData.readings.brightness,
        pressure: snapshotData.readings.pressure,
        unixTime: snapshotData.lastPosted.unixTime
    }
    documentID = snapshotData.lastPosted.unixTime.toString();
    db.collection('historical_data').doc(documentID).set(dataVal); 
}


